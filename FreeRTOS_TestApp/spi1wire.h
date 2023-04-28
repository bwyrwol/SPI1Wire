/** @file spi1wire.h
  * 
  * @author B.W.
  *
  * Biblioteka do obs³ugi interfejsu 1-Wire z wykorzystaniem sprzêtowego
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
  * @note Interfejs w uk³adzie MASTER jest aktywny tylko wówczas, gdy 
  *       wyprowadzenie SS jest skonfigurowane jako:
  *       - wyjœcie,
  *       - wejœcie (i pod³¹czone do stanu wysokiego).
  */
#define MOSI PB5	/**< MASTER: out(user)    SLAVE: in        */
#define MISO PB6	/**< MASTER: in           SLAVE: out(user) */
#define SCK  PB7    /**< MASTER: out(user)    SLAVE: in        */
#define SS   PB4    /**< MASTER: (user)       SLAVE: in        */

/** 
  * @def definicje rozkazów (oraz masek) interfejsu 1-Wire
  *
  * Kodowanie rozkazu (wartoœci bitów)
  * |R|C|C|-|L|L|L|L|
  * R    - znacznik zakoñczenia wykonywania rozkazu (Ready)
  * CC   - typ rozkazu: 00 - reset-pulse
  *                     01 - odczyt bajtu
  *                     10 - zapis bajtu
  * LLLL - odliczanie przesy³anych bitów  
  */
#define SPI1WIRE_NO_PRESENCE		0		/**< oznacza brak odpowiedzi uk³adu SLAVE */

#define SPI1WIRE_CMD_READY			0x80	/**< znacznik wskazuj¹cy na zakoñczenie 
                                                 generowania sekwencji interfejsu 1-Wire */
#define SPI1WIRE_CMD_READY_MASK		0x7F	/**< maska wartoœci SPI1WIRE_CMD_READY */

#define SPI1WIRE_CMD				0x60	/**< maska pozwalaj¹ca wyodrêbniæ bity CC */
#define SPI1WIRE_CMD_MASK			0x9F	/**< maska pozwalaj¹ca wyzerowaæ pole CC,
                                                 nie wykorzystywana */

#define SPI1WIRE_CMD_RESETPULSE		0x00	/**< kod rozkazu RESET-PULSE-PRESENCE */
#define SPI1WIRE_CMD_READ			0x20	/**< kod rozkazu odczytuj¹cego bajt
                                                 z magistrali 1-Wire */
#define SPI1WIRE_CMD_WRITE			0x40	/**< kod rozkazu wysy³aj¹cego bajt
                                                 na magistralê 1-Wire */

#define SPI1WIRE_CMD_CNT_MASK		0x1F	/**< maska pozwalaj¹ca wyodrêbniæ bity LLLL */

/**
  * Funkcja inicjalizuj¹ca interfejs SPI mikrokontrolera
  *
  * Wybierany jest tryb pracy interfejsu SPI, tak by mo¿liwe by³o generowanie
  * poprawnych sekwencji (przebiegów czasowych) dla interfejsu 1-Wire.
  * Dodatkowo wymagane jest odblokowanie przerwañ.
  *
  * @param  brak
  * @return brak
  *
  */
void SPI1Wire_Init(void);

/**
  * Funkcja generuj¹ca sekwencjê RESET-PULSE-PRESENCE protoko³u 1-Wire
  *
  * Wybierany jest tryb pracy interfejsu SPI, tak by mo¿liwe by³o generowanie
  * poprawnych sekwencji (przebiegów czasowych) dla interfejsu 1-Wire.
  * Wymagana jest wczeœniejsze zainicjowanie interfejsu SPI.
  *
  * @param  brak
  * @return zwracana jest wartoœæ równa 0, je¿eli do magistrali nie zosta³
  *         pod³¹czony przynajmniej jeden uk³ad SLAVE (lub nie odpowiada),
  *         w przeciwnym razie zwracana jest wartoœæ ró¿na od zera
  *
  */
uint8_t SPI1Wire_ResetPresence(void);

/**
  * Funkcja wysy³aj¹ca bajt danych na magistralê 1-Wire
  *
  * Wymagana jest wczeœniejsze zainicjowanie interfejsu SPI.
  *
  * @param  byte wysy³ana wartoœæ
  * @return brak
  *
  */
void SPI1Wire_Write(uint8_t byte);

/**
  * Funkcja pobieraj¹ca bajt danych z magistrali 1-Wire
  *
  * Wymagana jest wczeœniejsze zainicjowanie interfejsu SPI.
  *
  * @param  brak
  * @return odczytany bajt danych z magistrali 1-Wire
  *
  */
uint8_t SPI1Wire_Read(void);

#endif //SPI1WIRE_H_