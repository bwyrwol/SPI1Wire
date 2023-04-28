/** @file utility.h
  * 
  * @author B.W.
  *
  * Biblioteka pozwalaj�ca na wy�wietlenie warto�ci na wy�wietlaczu 
  * LCD (HD77480) warto�ci sta�oprzecinkowych
  *
  * @note Wykorzystuje bibliotek� lcd.h 
  *
  */

#ifndef UTILITY_H_
#define UTILITY_H_

#include <avr/pgmspace.h>
#include "lcd.h"

/**
  * Funkcja wypisuj�ca na wy�wietlaczu LCD warto�� u�amkow�
  *
  * Wypisywanie warto�ci odbywa si� od bie��cej pozycji kursora.
  *
  * @param [in] fract warto�� u�amkowa (brane pod uwag� s� tylko 4 bity)
  * @return brak
  *
  */
void LCDWriteFractional(uint8_t fract);

/**
  * Funkcja wypisuj�ca na wy�wietlaczu LCD warto�� ca�kowit�
  *
  * Wypisywanie warto�ci odbywa si� od bie��cej pozycji kursora.
  *
  * @param [in] integer warto�� ca�kowita
  * @return brak
  *
  */
void LCDWriteInteger(uint8_t integer);

#endif //UTILITY_H_