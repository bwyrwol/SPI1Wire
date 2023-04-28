/** @file spi1wire.c
  */

#include "spi1wire.h"

void SPI1Wire_Init(void)
{
	DDRB |= (1 << MOSI) | (1 << SCK); /**< nale¿y dodaæ | (1 << SS); lub */
	PORTB |= (1 << SS);               /**< ustawiæ wejœcie w stan wysoki, jak tutaj */
	SPCR = (1 << SPE)  |			  /**< odblokowanie uk³adu SPI */
	       (1 << MSTR) |			  /**< tryb pracy jako MASTER */
		   (1 << SPR1) |              /**< ustawienie dzielnika sygna³u zegarowego na 128 */
		   (1 << SPR0);
}

/**
  * Niskopoziomowa funkcja (program obs³ugi przerwania od uk³adu SPI) wykorzystana
  * do generowania podstawowych sekwencji przebiegów na magistrali 1-Wire
  *
  */

volatile uint8_t spi_1wire_command = 0; /**< zakodowany rozkaz do wykonania, opis w pliku spi1wire.h */
volatile uint8_t spi_1wire_data = 0;    /**< dana do zapisu lub odczytana z magistrali 1-Wire */

ISR(SPI_STC_vect)
{
	switch(spi_1wire_command & SPI1WIRE_CMD_READY_MASK)
	{
		/**< Rozkaz RESET-PULSE */
		case 0x00:  /**< sekwencja RESET */ 
		case 0x01:  
		case 0x02:
		case 0x03: 
		case 0x04: 
		case 0x05:
		case 0x06:	SPDR = 0x00;
					spi_1wire_command++;
					break;
		case 0x07:  /**< sekwencja PULSE */
					SPDR = 0xFF;
					spi_1wire_command++;
					spi_1wire_data = 0;
					break;
		case 0x08:
		case 0x09:  
		case 0x0A:  /**< identyfikacja impulsu PRESENCE */
					if (SPDR == 0) spi_1wire_data++;
					SPDR = 0xFF;
					spi_1wire_command++;
					break;
		case 0x0B:
					if (SPDR == 0) spi_1wire_data++;
					/**< zakoñczenie sekwencji, blokada przerwania */
					spi_1wire_command |= SPI1WIRE_CMD_READY;
					SPCR &= ~(1 << SPIE);
					break;
					
		/**< Rozkaz READ */
		case 0x20:
		case 0x21:
		case 0x22:
		case 0x23:
		case 0x24:
		case 0x25:
		case 0x26:
		case 0x27:	/**< odczyt pojedynczego bitu */
					if ((SPDR & 0x3F) == 0x3F) spi_1wire_data = (spi_1wire_data >> 1) | 0x80;
					else spi_1wire_data = (spi_1wire_data >> 1) & 0x7F;
					/**< wygenerowanie kolejnej sekwencji do odczytu pojedynczego bitu */
					SPDR = 0x7F;
					if (spi_1wire_command != 0x27) spi_1wire_command++;
					else
					{
						/**< zakoñczenie sekwencji, blokada przerwania */
						spi_1wire_command |= SPI1WIRE_CMD_READY;
						SPCR &= ~(1 << SPIE);
					}
					break;

		/**< Rozkaz WRITE */
		case 0x40:
		case 0x41:
		case 0x42:
		case 0x43:
		case 0x44:
		case 0x45:
		case 0x46:
		case 0x47:	/**< wys³anie pojedynczego bitu */
					spi_1wire_data = (spi_1wire_data >> 1) & 0x7F;
					/**< wygenerowanie sekwencji wysy³aj¹cej pojedynczy bit */
					if ((spi_1wire_data & 0x01) == 0) SPDR = 0x00;
					else SPDR = 0x7F;
					if (spi_1wire_command != 0x47) spi_1wire_command++;
					else
					{
						/**< zakoñczenie sekwencji, blokada przerwania */
						spi_1wire_command |= SPI1WIRE_CMD_READY;
						SPCR &= ~(1 << SPIE);
					}
					break;
		
		/**< Rozkaz nieznany, wstrzymanie transmisji */
		default:	SPCR &= ~(1 << SPIE);	
	}
}

uint8_t SPI1Wire_ResetPresence(void)
{
	/**< wybór rozkazu */
	spi_1wire_command = SPI1WIRE_CMD_RESETPULSE;
	/**< odblokowanie przerwania od interfejsu SPI */
	SPCR |= (1 << SPIE);
	/**< rozpoczêcie transmisji, po zakoñczeniu wywo³ywany jest program obs³ugi przerwania ISR */
	SPDR = 0x00;
	/**< oczekiwania na zakoñczenie sekwencji */
	while((spi_1wire_command & SPI1WIRE_CMD_READY) == 0)
	{
		/**< @todo opracowaæ wersjê nieblokuj¹c¹ */
	};
	return spi_1wire_data;
}

void SPI1Wire_Write(uint8_t byte)
{
	/**< dana do wys³ania */
	spi_1wire_data = byte;
	/**< wybór rozkazu */
	spi_1wire_command = SPI1WIRE_CMD_WRITE;
	/**< odblokowanie przerwania od interfejsu SPI */
	SPCR |= (1 << SPIE);
	/**< rozpoczêcie transmisji, wyslanie pierwszej sekwencji (bitu),
	     po zakoñczeniu wywo³ywany jest program obs³ugi przerwania ISR */
	if ((spi_1wire_data & 0x01) == 0x00) SPDR = 0x00; else SPDR = 0x7F;
	/**< oczekiwania na zakoñczenie sekwencji wysy³ania wszystkich bitów */
	while((spi_1wire_command & SPI1WIRE_CMD_READY) == 0)
	{
		/**< @todo opracowaæ wersjê nieblokuj¹c¹ */
	};
}

uint8_t SPI1Wire_Read(void)
{
	/**< wybór rozkazu */
	spi_1wire_command = SPI1WIRE_CMD_READ;
	/**< odblokowanie przerwania od interfejsu SPI */
	SPCR |= (1 << SPIE);
	/**< rozpoczêcie transmisji, po zakoñczeniu wywo³ywany jest program
	     obs³ugi przerwania ISR, w którym odczytywane bêd¹ kolejne bity */
	SPDR = 0x7F;
	/**< oczekiwania na zakoñczenie sekwencji wysy³ania wszystkich bitów */
	while((spi_1wire_command & SPI1WIRE_CMD_READY) == 0)
	{
		/**< @todo opracowaæ wersjê nieblokuj¹c¹ */
	};
	return spi_1wire_data;
}
