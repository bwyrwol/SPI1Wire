/*
	Uklad:
		* LCD: PC0-SDA; PC1-SDL
		* DS18B20: 

	Test application:
		* Bare-Metal embedded system with interrupt

 */


#include <avr/io.h>
#include <avr/interrupt.h>

#include "main.h"
#include <util/delay.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "pcf8574.h"
#include "twi.h"
#include "lcd.h"

//
// Podstawowe polecenia ukladu DS18B20
//
#define cmd_DS18x20_SearchROM		0xF0
#define cmd_DS18x20_ReadROM			0x33
#define cmd_DS18x20_MatchROM		0x55
#define cmd_DS18x20_SkipROM			0xCC
#define cmd_DS18x20_AlarmSearch		0xEC
#define cmd_DS18x20_ConvertT		0x44
#define cmd_DS18x20_ReadScratchpad	0xBE
#define cmd_DS18x20_WrireScratchpad	0x4E
#define cmd_DS18x20_CopyScratchpad	0x48
#define cmd_DS18x20_RecallEE		0xB8
#define cmd_DS18x20_ReadPowerSupply	0xB4

// definicje wyprowadzen interfejsu SPI
					// MASTER       SLAVE
#define MOSI PB5	// out(user)    in
#define MISO PB6	// in           out(user)
#define SCK  PB7    // out(user)    in
#define SS   PB4    // (user)       in

void SPI_Init(void)
{
	DDRB |= (1 << MOSI) | (1 << SCK);// | (1 << SS); // w MASTER SS musi byc jako wyjscie lub wejscie ale w stanie wysokim
	PORTB |= (1 << SS);
	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0);
}

/*
	1WIRE LOW LEVEL SPI FUNCTIONS
 */

#define SPI_1WIRE_NO_PRESENCE	0


volatile uint8_t spi_1wire_command = 0;
volatile uint8_t spi_1wire_data = 0;

/*
    Kodowanie rozkazu
	
	|R|C|C|L|L|L|L|L|
	
	R     - znacznik zakonczenia wykonywania rozkazu (ready)
    CC    - typ rozkazu (00 - reset-pulse; 01 - read byte; 10 - write byte)
	LLLLL - odliczanie przesylanych bajtow
*/

#define SPI_1WIRE_CMD_READY			0x80
#define SPI_1WIRE_CMD_READY_MASK	0x7F
	
#define SPI_1WIRE_CMD				0x60
#define SPI_1WIRE_CMD_MASK			0x9F

#define SPI_1WIRE_CMD_RESETPULSE	0x00
#define SPI_1WIRE_CMD_READ			0x20
#define SPI_1WIRE_CMD_WRITE			0x40

#define SPI_1WIRE_CMD_CNT_MASK		0x1F

ISR(SPI_STC_vect)
{
	switch(spi_1wire_command & SPI_1WIRE_CMD_READY_MASK)
	{
		// RESET-PULSE COMMAND
		case 0x00:  // reset sequence 
		case 0x01:  
		case 0x02: //spi_1wire_command = 0x04; break;
		case 0x03: 
		case 0x04: 
		case 0x05:
		case 0x06:	SPDR = 0x00;
					spi_1wire_command++;
					break;
		case 0x07:  SPDR = 0xFF;
					spi_1wire_command++;
					spi_1wire_data = 0;
					break;
		case 0x08:  // pulse seqeunce
		case 0x09:  
		case 0x0A:
					if (SPDR == 0) spi_1wire_data++;
					SPDR = 0xFF;
					spi_1wire_command++;
					break;
		case 0x0B:
					if (SPDR == 0) spi_1wire_data++;
					spi_1wire_command |= SPI_1WIRE_CMD_READY;
					SPCR &= ~(1 << SPIE);
					break;
					
		//READ COMMAND
		case 0x20:
		case 0x21:
		case 0x22:
		case 0x23:
		case 0x24:
		case 0x25:
		case 0x26:
		case 0x27:					
					if ((SPDR & 0x3F) == 0x3F) spi_1wire_data = (spi_1wire_data >> 1) | 0x80;
					else spi_1wire_data = (spi_1wire_data >> 1) & 0x7F;
					SPDR = 0x7F;
					if (spi_1wire_command != 0x27) spi_1wire_command++;
					else
					{
						spi_1wire_command |= SPI_1WIRE_CMD_READY;
						SPCR &= ~(1 << SPIE);
					}
					break;
				
		case 0x40:	// wersja z liczeniem bitow
		case 0x41:
		case 0x42:
		case 0x43:
		case 0x44:
		case 0x45:
		case 0x46:
		case 0x47:
					spi_1wire_data = (spi_1wire_data >> 1) & 0x7F;
					if ((spi_1wire_data & 0x01) == 0) SPDR = 0x00;
					else SPDR = 0x7F;
					if (spi_1wire_command != 0x47) spi_1wire_command++;
					else
					{
						spi_1wire_command |= SPI_1WIRE_CMD_READY;
						SPCR &= ~(1 << SPIE);
					}
					break;
		
		// STOP TRANSMISSION
		default:	;	
	}
}

uint8_t SPI_1WireResetPresence_noblock(void)
{
	  spi_1wire_command = SPI_1WIRE_CMD_RESETPULSE;
	  SPCR |= (1 << SPIE);
	  SPDR = 0x00;
	  while((spi_1wire_command & SPI_1WIRE_CMD_READY) == 0);
	  return spi_1wire_data;
}

void SPI_1WireWrite_noblock(uint8_t byte)
{
  spi_1wire_data = byte;
  spi_1wire_command = SPI_1WIRE_CMD_WRITE;
  SPCR |= (1 << SPIE);
  if ((spi_1wire_data & 0x01) == 0x00) SPDR = 0x00; else SPDR = 0x7F;
  while((spi_1wire_command & SPI_1WIRE_CMD_READY) == 0);
}

uint8_t SPI_1WireRead_noblock(void)
{
  spi_1wire_command = SPI_1WIRE_CMD_READ;

  SPCR |= (1 << SPIE);
  SPDR = 0x7F;
  while((spi_1wire_command & SPI_1WIRE_CMD_READY) == 0);
  return spi_1wire_data;
}


const uint8_t str00[] PROGMEM = "0000";
const uint8_t str01[] PROGMEM = "0625";
const uint8_t str02[] PROGMEM = "1250";
const uint8_t str03[] PROGMEM = "1875";
const uint8_t str04[] PROGMEM = "2500";
const uint8_t str05[] PROGMEM = "3125";
const uint8_t str06[] PROGMEM = "3750";
const uint8_t str07[] PROGMEM = "4375";
const uint8_t str08[] PROGMEM = "5000";
const uint8_t str09[] PROGMEM = "5625";
const uint8_t str10[] PROGMEM = "6250";
const uint8_t str11[] PROGMEM = "6875";
const uint8_t str12[] PROGMEM = "7500";
const uint8_t str13[] PROGMEM = "8125";
const uint8_t str14[] PROGMEM = "8750";
const uint8_t str15[] PROGMEM = "9375";

const uint8_t* const stringFraction[] PROGMEM = { str00, str01, str02, str03,
                                                  str04, str05, str06, str07,
										          str08, str08, str10, str11,
											      str12, str13, str14, str15 };
void LCDWriteFraction(unsigned char fract)
{
	unsigned char tmp;
	uint8_t *addr = 0;

	addr = (uint8_t*)pgm_read_word(&stringFraction[fract & 0x0F]);

	LCDPutChar('.');
	for (uint8_t i = 0; i < 4; i++)
	  {
         tmp = pgm_read_byte(addr + i);
		 LCDPutChar(tmp);
      }
}

void main(void)
{

	TWI_init(); // wyswietlacz z interfejsem TWI, inicjalizacja TWI
	LCDInit();  // Inicjalizacja wyswietlacza
	LCDGoTo(0); // Pisz od pozycji 0
	
	SPI_Init(); // inicjalizacja interfejsu SPI
	
	// test cyklicznego pomiaru temperatury
	uint8_t tempH, tempL;
	
	sei();

	for(;;)
	{
	  if (SPI_1WireResetPresence_noblock() != SPI_1WIRE_NO_PRESENCE)
	    {
		  // uklad obecny na magistrali, inicjalizacja konwersji temperatury
		  
		  SPI_1WireWrite_noblock(cmd_DS18x20_SkipROM);
		  
		  SPI_1WireWrite_noblock(cmd_DS18x20_ConvertT);
		  _delay_ms(750);

		  // odczyt temperatury
		  SPI_1WireResetPresence_noblock();
		  SPI_1WireWrite_noblock(cmd_DS18x20_SkipROM);
		  SPI_1WireWrite_noblock(cmd_DS18x20_ReadScratchpad);
		  
		  
		  tempL = SPI_1WireRead_noblock();
		  tempH = SPI_1WireRead_noblock();

		  // wyniku pomiaru temperatury
		  uint16_t temp = (tempH << 8) + tempL;

		  LCDClear();
		  LCDGoTo(0);

          if ((temp & 0x8000) != 0)
		  {
			LCDPutChar('-');
			temp = ~temp + 1;
		  }
		  else
		  {
			LCDPutChar('+');
		  }

		  uint8_t temp_t = temp >> 4;

		  LCDPutChar((temp_t / 100) + '0');
		  LCDPutChar(((temp_t % 100) / 10) + '0');
		  LCDPutChar((temp_t % 10) + '0');

		  LCDWriteFraction(temp & 0x0F);
	    }
	  else
	    {
		  // uklad nieobecny
		  LCDClear();
		  LCDGoTo(0);
		  LCDPutsCode("No DS18B20 found!");
		  _delay_ms(1000);
	    }
	}//for
}
