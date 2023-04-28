/** @file twi.h
  * 
  * @author B.W.
  *
  * Biblioteka obs�ugi interfejsu TWI (I2C)
  *
  * @note Obs�uga programowa dla mikrokontrolera AVR
  *
  */

#ifndef TWI_H
#define TWI_H

#include <avr/io.h>

/**
  * Funkcja inicjalizuj�ca interfejs TWI 
  *
  * Inicjalizacja sprowadza si� wy��cznie do ustawienia dzielnika cz�stotliwo�ci
  * sygna�u zegarowego zgodnie ze wzorem
  * @f[
  * SCLfreq = \frac{CLKXtal}{(16+2*TWBR*4^{TWPS1:TWPS0})}
  * @f]
  * Znaczenie bit�w mo�na znale�� w dokumentacji 
  * <a href="https://ww1.microchip.com/downloads/aemDocuments/documents/OTH/ProductDocuments/DataSheets/ATmega32A-DataSheet-Complete-DS40002072A.pdf">ATmega32A-DataSheet-Complete-DS40002072A.pdf</a> .
  *
  * @param  brak
  * @return brak
  *
  */
void TWI_init(void);

/**
  * Funkcja inicjuj�ca sekwencj� START
  *
  * Wywo�ywana zawsze jako pierwsze podczas transmisji danych.
  *
  * @param  brak
  * @return brak
  *
  */
void TWI_start(void);

/**
  * Funkcja inicjuj�ca sekwencj� STOP
  *
  * Wywo�ywana zawsze jako ostatnia podczas transmisji danych, ko�czy
  * przesy�anie danych.
  *
  * @param  brak
  * @return brak
  *
  */
void TWI_stop(void);

/**
  * Funkcja oczekuj�ca na zako�czenie bie��cej sekwencji
  *
  * @note Funkcja blokuj�ca, w przypadku b��du funkcja mo�e si�
  * zap�tli�.
  *
  * @todo Wprowadzi� czas oczekiwania na znacznik TWINT, po przekroczeniu
  *       kt�rego nast�pi wyj�cie z funkcji (z odpowiednim kodem b��du).
  *
  * @param  brak
  * @return brak
  *
  */
void TWI_wait(void);

/**
  * Funkcja wysy�aj�ca dan� za po�rednictwem interfejsu TWI
  *
  * @param  data [in] wysy�ana dana
  * @return brak
  *
  */
void TWI_write(uint8_t data);

/**
  * Funkcja odczytuj�ca dan� za po�rednictwem interfejsu TWI
  *
  * @param  brak
  * @return odczytana dana
  *
  */
uint8_t TWI_read(void);

#endif //TWI_H
