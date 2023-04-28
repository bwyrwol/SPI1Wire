#include <avr/io.h>
#include <avr/interrupt.h>

//#define F_CPU 14745600UL
//#include <util/delay.h>
//#include <avr/pgmspace.h>

#include "main.h"
#include <util/delay.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "pcf8574.h"
#include "twi.h"
#include "lcd.h"

//
// Definicja sposobu podlaczenia ukladu DS18B20
//   Uwaga: podajemy adres portu PINx oraz numer wyprowadzenia portu
//
#define PORT_1Wire PINB
#define PIN_1Wire  PINB0

//
// Makra ustawiajace stan magistrali 1Wire:
//  * wysoki (poprzez rezystor polaryzujacy magistrale, wyprowadzenie portu
//    skonfigurowane jako wejscie)
#define SET_1Wire  _SFR_IO8(_SFR_IO_ADDR(PORT_1Wire)+1) &= ~(1<<PIN_1Wire)
//  * niski (przy zalozeniu, ze PORTx.y ma wpisana wartosc "0")
#define CLR_1Wire  _SFR_IO8(_SFR_IO_ADDR(PORT_1Wire)+1) |= (1<<PIN_1Wire)

//
// Definicje preskalera dla licznika T0
//
#define timer_stop             TCCR0 = 0
#define timer_start_presc_1    TCCR0 = (0<<CS02)|(0<<CS01)|(1<<CS00)
#define timer_start_presc_8    TCCR0 = (0<<CS02)|(1<<CS01)|(0<<CS00)
#define timer_start_presc_64   TCCR0 = (0<<CS02)|(1<<CS01)|(1<<CS00)
#define timer_start_presc_256  TCCR0 = (1<<CS02)|(0<<CS01)|(0<<CS00)
#define timer_start_presc_1024 TCCR0 = (1<<CS02)|(0<<CS01)|(1<<CS00)

// start licznika i natychmiastowe wywolanie przerwania
#define timer_start {						\
					TCNT0 = 255;			\
					timer_start_presc_1;	\
					}

//
// Definicje opoznien realizowanych za pomoca licznika T0
//   Uwaga: dla czestotliwosci zegara 14,7456MHz
//
#define delay500us_1Wire {							\
							TCNT0 = 255 - 115; 		\
							timer_start_presc_64;	\
						 }
#define delay30us_1Wire  {							\
							TCNT0 = 255 - 55; 		\
							timer_start_presc_8;	\
						 }
#define delay470us_1Wire {							\
							TCNT0 = 255 - 108; 		\
							timer_start_presc_64;	\
						 }
#define delay2us_1Wire   {							\
							TCNT0 = 255 - 29; 		\
							timer_start_presc_1;	\
						 }
#define delay15us_1Wire  {							\
							TCNT0 = 255 - 221; 		\
							timer_start_presc_1;	\
						 }
#define delay5us_1Wire   {							\
							TCNT0 = 255 - 74; 		\
							timer_start_presc_1;	\
						 }
#define delay80us_1Wire  {							\
							TCNT0 = 255 - 147; 		\
							timer_start_presc_8;	\
						 }

#define delay480us_1Wire {							\
							TCNT0 = 255 - 111; 		\
							timer_start_presc_64;	\
						 }
#define delay390us_1Wire {							\
							TCNT0 = 255 - 90; 		\
							timer_start_presc_64;	\
						 }
#define delay90us_1Wire  {							\
							TCNT0 = 255 - 166; 		\
							timer_start_presc_8;	\
						 }
#define delay60us_1Wire  {							\
							TCNT0 = 255 - 111; 		\
							timer_start_presc_8;	\
						 }
#define delay45us_1Wire  {							\
							TCNT0 = 255 - 83; 		\
							timer_start_presc_8;	\
						 }
#define delay14us_1Wire  {							\
							TCNT0 = 255 - 206; 		\
							timer_start_presc_1;	\
						 }
#define delay1us_1Wire   {							\
							TCNT0 = 255 - 15; 		\
							timer_start_presc_1;	\
						 }




//
// Podstawowe operacje na interfejsie 1Wire
//
#define cmd_1Wire_ResetPulse		0x00
#define cmd_1Wire_ReadBit			0x04
#define cmd_1Wire_WriteBit			0x08
#define cmd_1Wire_Ready				0x0F

#define cmd_1Wire_result_mask		0x80

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

//
// Zmienna wykorzystana do obslugi interfejsu 1Wire
//   Uwaga: najmniej znaczace bity okreslaja kod (stan poczatkowy automatu)
//          realizowanej operacji, po jej wykonaniu przyjmuja wartosc 0x0F,
//          najbardziej znaczacy bit sluzy do przekazywania parametru tj. dla
//          * cmd_1Wire_ResetPulse - 0b1xxxxxxx - uklad obecny na magistrali
//                                   0b0xxxxxxx - uklad niedostepny
//          * cmd_1Wire_ReadBit    - odczytany bit z magistrali
//          * cmd_1Wire_WriteBit   - zapisywany bit do ukladu

volatile unsigned char cmd_1Wire;

ISR(TIMER0_OVF_vect)
{
	//PORTC ^= 0x01;
	timer_stop;
	switch(cmd_1Wire & 0x0F)
	{
		// cmd_1Wire_ResetPulse
		case 0x00: 	CLR_1Wire;
					delay480us_1Wire;
					cmd_1Wire++;
					break;
		case 0x01: 	SET_1Wire;
					delay90us_1Wire;
					cmd_1Wire++;
					break;
		case 0x02: 	if ((PORT_1Wire & (1<<PIN_1Wire)) == 0) cmd_1Wire |= 0x80;
					delay390us_1Wire;
					cmd_1Wire++;
					break;
		case 0x03: 	cmd_1Wire |= cmd_1Wire_Ready;
					break;
		// cmd_1Wire_ReadBit
		case 0x04: 	CLR_1Wire;
					delay2us_1Wire;//bylo 1us
					cmd_1Wire++;
					break;
		case 0x05: 	SET_1Wire;
					delay15us_1Wire;//bylo 14us
					cmd_1Wire++;
					break;
		case 0x06: 	if ((PORT_1Wire & (1<<PIN_1Wire)) != 0) cmd_1Wire |= 0x80;
					delay45us_1Wire;
					cmd_1Wire++;
					break;
		case 0x07:	cmd_1Wire |= cmd_1Wire_Ready;
					break;
		// cmd_1Wire_WriteBit
		case 0x08: 	CLR_1Wire;
					delay5us_1Wire;//bylo 1us
					cmd_1Wire++;
					break;
		case 0x09: 	if ((cmd_1Wire & 0x80) != 0) SET_1Wire;
					delay90us_1Wire;//bylo 60us
					cmd_1Wire++;
					break;
		case 0x0A: 	SET_1Wire;
					cmd_1Wire |= cmd_1Wire_Ready;
					break;
		// cmd_1Wire_Ready
		case 0x0F:	break;
	}
}

unsigned char read_byte_1Wire(void)
{
	unsigned char tmp = 0;
	
	for (unsigned char i=0; i<8; i++)
	  {
		// wybor operacji odczytu pojedynczego bitu
		cmd_1Wire = cmd_1Wire_ReadBit;
		// rozpoczecie wykonywania komendy
		timer_start;
		// oczekiwanie na zakonczenie
		while((cmd_1Wire & 0x0F) != cmd_1Wire_Ready);

		if ((cmd_1Wire & 0x80) != 0) tmp |= (1<<i);
	  }
	return tmp;
}

void write_byte_1Wire(unsigned char byte)
{
	for (unsigned char i=0; i<8; i++)
	  {
		// wybor operacji zapisu pojedynczego bitu
		cmd_1Wire = cmd_1Wire_WriteBit;

		if ((byte & (1<<i)) != 0) cmd_1Wire |= 0x80;

		// rozpoczecie wykonywania komendy
		timer_start;
		// oczekiwanie na zakonczenie
		while((cmd_1Wire & 0x0F) != cmd_1Wire_Ready);
	  }
}

unsigned int reset_presence_1Wire()
{
	// wybor realizowanej operacji
	cmd_1Wire = cmd_1Wire_ResetPulse;
	// uruchomienie komunikacji
	timer_start;
	// oczekiwanie na zakonczenie
	while((cmd_1Wire & 0x0F) != cmd_1Wire_Ready);
	// sprawdzenie wyniku - obecnosci ukladu na magistrali
	if ((cmd_1Wire & 0x80) != 0) return 1;
	else return 0;
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
	TWI_init();
	LCDInit();  // Inicjalizacja
	LCDGoTo(0); // Pisz od pozycji 0
	
	// inicjalizacja licznika Timer0 i przerwania
	// przerwanie przy przepelnieniu
    TIMSK |= (1<<TOIE0);
	TCCR0 = timer_stop;
	// enable interrupts
	sei();


	// test czy jest uklad dolaczony do magistrali 1Wire
	// wybor realizowanej operacji
	//cmd_1Wire = cmd_1Wire_ResetPulse;
	// uruchomienie komunikacji
	//timer_start;
	// oczekiwanie na zakonczenie
	//while((cmd_1Wire & 0x0F) != cmd_1Wire_Ready);
	// sprawdzenie wyniku - obecnosci ukladu na magistrali

	// test cyklicznego pomiaru temperatury

	unsigned char tempH, tempL;

	for(;;)
	{
	  if (reset_presence_1Wire() != 0)
	    {
		  // uklad obecny na magistrali
		  write_byte_1Wire(cmd_DS18x20_SkipROM);
		  write_byte_1Wire(cmd_DS18x20_ConvertT);
		  _delay_ms(750);

		  reset_presence_1Wire();
		  write_byte_1Wire(cmd_DS18x20_SkipROM);
		  write_byte_1Wire(cmd_DS18x20_ReadScratchpad);
		  tempL = read_byte_1Wire();
		  tempH = read_byte_1Wire();
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
