/** @file utility.h
  * 
  * @author B.W.
  *
  * Biblioteka pozwalaj¹ca na wyœwietlenie wartoœci na wyœwietlaczu 
  * LCD (HD77480) wartoœci sta³oprzecinkowych
  *
  * @note Wykorzystuje bibliotekê lcd.h 
  *
  */

#ifndef UTILITY_H_
#define UTILITY_H_

#include <avr/pgmspace.h>
#include "lcd.h"

/**
  * Funkcja wypisuj¹ca na wyœwietlaczu LCD wartoœæ u³amkow¹
  *
  * Wypisywanie wartoœci odbywa siê od bie¿¹cej pozycji kursora.
  *
  * @param [in] fract wartoœæ u³amkowa (brane pod uwagê s¹ tylko 4 bity)
  * @return brak
  *
  */
void LCDWriteFractional(uint8_t fract);

/**
  * Funkcja wypisuj¹ca na wyœwietlaczu LCD wartoœæ ca³kowit¹
  *
  * Wypisywanie wartoœci odbywa siê od bie¿¹cej pozycji kursora.
  *
  * @param [in] integer wartoœæ ca³kowita
  * @return brak
  *
  */
void LCDWriteInteger(uint8_t integer);

#endif //UTILITY_H_