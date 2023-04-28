/** @file pcf8574.h
  * 
  * @author B.W.
  *
  * Biblioteka obs�ugi portu r�wnoleg�ego PCF8574 (I2C/TWI)
  *
  * @note Wymaga biblioteki do obs�ugi interfejsu TWI
  *
  */
#ifndef PCF8574_H
#define PCF8574_H

#include <avr/io.h>
#include "twi.h"

/**< @def definicja adresu uk�adu PCF8574 */ 
//#define PCF8574Addr				0b01111110		// adres ukladu typu A
#define PCF8574Addr				0b01001110		// adres ukladu

/**
  * Funkcja odczytuj�ca port uk�adu PCF8574
  *
  * @note Przed wywo�aniem wymagana inicjalizacja interfejsu TWI.
  *
  * @param  brak
  * @return odczytany bajt danych z uk�adu
  *
  */
uint8_t PCF8574_ReadPort(void);

/**
  * Funkcja zapisuj�ca bajt danych do uk�adu PCF8574
  *
  * @note Przed wywo�aniem wymagana inicjalizacja interfejsu TWI.
  *
  * @param  data zapisywana warto��
  * @return brak
  *
  */
void PCF8574_WritePort(uint8_t data);

#endif //PCF8574
