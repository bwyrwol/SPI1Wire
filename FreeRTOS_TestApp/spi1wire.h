/** @file spi1wire.h
  * 
  * @author B.W.
  *
  * Biblioteka do obs�ugi interfejsu 1-Wire z wykorzystaniem sprz�towego
  * interfejsu SPI mikrokontrolera 
  * 
  */

#ifndef SPI1WIRE_H_
#define SPI1WIRE_H_

#include <avr/io.h>
#include <avr/interrupt.h>

/** 
  * @def definicje wyprowadzen interfejsu SPI mikrokontrolera
  *
  * @note Interfejs w uk�adzie MASTER jest aktywny tylko w�wczas, gdy 
  *       wyprowadzenie SS jest skonfigurowane jako:
  *       - wyj�cie,
  *       - wej�cie (i pod��czone do stanu wysokiego).
  */
#define MOSI PB5	/**< MASTER: out(user)    SLAVE: in        */
#define MISO PB6	/**< MASTER: in           SLAVE: out(user) */
#define SCK  PB7    /**< MASTER: out(user)    SLAVE: in        */
#define SS   PB4    /**< MASTER: (user)       SLAVE: in        */

/** 
  * @def definicje rozkaz�w (oraz masek) interfejsu 1-Wire
  *
  * Kodowanie rozkazu (warto�ci bit�w)
  * |R|C|C|-|L|L|L|L|
  * R    - znacznik zako�czenia wykonywania rozkazu (Ready)
  * CC   - typ rozkazu: 00 - reset-pulse
  *                     01 - odczyt bajtu
  *                     10 - zapis bajtu
  * LLLL - odliczanie przesy�anych bit�w  
  */
#define SPI1WIRE_NO_PRESENCE		0		/**< oznacza brak odpowiedzi uk�adu SLAVE */

#define SPI1WIRE_CMD_READY			0x80	/**< znacznik wskazuj�cy na zako�czenie 
                                                 generowania sekwencji interfejsu 1-Wire */
#define SPI1WIRE_CMD_READY_MASK		0x7F	/**< maska warto�ci SPI1WIRE_CMD_READY */

#define SPI1WIRE_CMD				0x60	/**< maska pozwalaj�ca wyodr�bni� bity CC */
#define SPI1WIRE_CMD_MASK			0x9F	/**< maska pozwalaj�ca wyzerowa� pole CC,
                                                 nie wykorzystywana */

#define SPI1WIRE_CMD_RESETPULSE		0x00	/**< kod rozkazu RESET-PULSE-PRESENCE */
#define SPI1WIRE_CMD_READ			0x20	/**< kod rozkazu odczytuj�cego bajt
                                                 z magistrali 1-Wire */
#define SPI1WIRE_CMD_WRITE			0x40	/**< kod rozkazu wysy�aj�cego bajt
                                                 na magistral� 1-Wire */

#define SPI1WIRE_CMD_CNT_MASK		0x1F	/**< maska pozwalaj�ca wyodr�bni� bity LLLL */

/**
  * Funkcja inicjalizuj�ca interfejs SPI mikrokontrolera
  *
  * Wybierany jest tryb pracy interfejsu SPI, tak by mo�liwe by�o generowanie
  * poprawnych sekwencji (przebieg�w czasowych) dla interfejsu 1-Wire.
  * Dodatkowo wymagane jest odblokowanie przerwa�.
  *
  * @param  brak
  * @return brak
  *
  */
void SPI1Wire_Init(void);

/**
  * Funkcja generuj�ca sekwencj� RESET-PULSE-PRESENCE protoko�u 1-Wire
  *
  * Wybierany jest tryb pracy interfejsu SPI, tak by mo�liwe by�o generowanie
  * poprawnych sekwencji (przebieg�w czasowych) dla interfejsu 1-Wire.
  * Wymagana jest wcze�niejsze zainicjowanie interfejsu SPI.
  *
  * @param  brak
  * @return zwracana jest warto�� r�wna 0, je�eli do magistrali nie zosta�
  *         pod��czony przynajmniej jeden uk�ad SLAVE (lub nie odpowiada),
  *         w przeciwnym razie zwracana jest warto�� r�na od zera
  *
  */
uint8_t SPI1Wire_ResetPresence(void);

/**
  * Funkcja wysy�aj�ca bajt danych na magistral� 1-Wire
  *
  * Wymagana jest wcze�niejsze zainicjowanie interfejsu SPI.
  *
  * @param  byte wysy�ana warto��
  * @return brak
  *
  */
void SPI1Wire_Write(uint8_t byte);

/**
  * Funkcja pobieraj�ca bajt danych z magistrali 1-Wire
  *
  * Wymagana jest wcze�niejsze zainicjowanie interfejsu SPI.
  *
  * @param  brak
  * @return odczytany bajt danych z magistrali 1-Wire
  *
  */
uint8_t SPI1Wire_Read(void);

#endif //SPI1WIRE_H_