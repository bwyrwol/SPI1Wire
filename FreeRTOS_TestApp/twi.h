/** @file twi.h
  * 
  * @author B.W.
  *
  * Biblioteka obs³ugi interfejsu TWI (I2C)
  *
  * @note Obs³uga programowa dla mikrokontrolera AVR
  *
  */

#ifndef TWI_H
#define TWI_H

#include <avr/io.h>

/**
  * Funkcja inicjalizuj¹ca interfejs TWI 
  *
  * Inicjalizacja sprowadza siê wy³¹cznie do ustawienia dzielnika czêstotliwoœci
  * sygna³u zegarowego zgodnie ze wzorem
  * @f[
  * SCLfreq = \frac{CLKXtal}{(16+2*TWBR*4^{TWPS1:TWPS0})}
  * @f]
  * Znaczenie bitów mo¿na znaleŸæ w dokumentacji 
  * <a href="https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ProductDocuments/DataSheets/ATmega32A-DataSheet-Complete-DS40002072A.pdf">ATmega32A-DataSheet-Complete-DS40002072A.pdf</a> .
  *
  * @param  brak
  * @return brak
  *
  */
void TWI_init(void);

/**
  * Funkcja inicjuj¹ca sekwencjê START
  *
  * Wywo³ywana zawsze jako pierwsze podczas transmisji danych.
  *
  * @param  brak
  * @return brak
  *
  */
void TWI_start(void);

/**
  * Funkcja inicjuj¹ca sekwencjê STOP
  *
  * Wywo³ywana zawsze jako ostatnia podczas transmisji danych, koñczy
  * przesy³anie danych.
  *
  * @param  brak
  * @return brak
  *
  */
void TWI_stop(void);

/**
  * Funkcja oczekuj¹ca na zakoñczenie bie¿¹cej sekwencji
  *
  * @note Funkcja blokuj¹ca, w przypadku b³êdu funkcja mo¿e siê
  * zapêtliæ.
  *
  * @todo Wprowadziæ czas oczekiwania na znacznik TWINT, po przekroczeniu
  *       którego nast¹pi wyjœcie z funkcji (z odpowiednim kodem b³êdu).
  *
  * @param  brak
  * @return brak
  *
  */
void TWI_wait(void);

/**
  * Funkcja wysy³aj¹ca dan¹ za poœrednictwem interfejsu TWI
  *
  * @param  data [in] wysy³ana dana
  * @return brak
  *
  */
void TWI_write(uint8_t data);

/**
  * Funkcja odczytuj¹ca dan¹ za poœrednictwem interfejsu TWI
  *
  * @param  brak
  * @return odczytana dana
  *
  */
uint8_t TWI_read(void);

#endif //TWI_H
