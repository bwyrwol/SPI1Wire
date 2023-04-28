/** @file lcd.h 
  *
  * @author B.W.
  * @date 2023/02/26
  *
  * Biblioteka funkcji do obs³ugi wyœwietlacza LCD ze sterownikiem HD77480
  * z interfejsem szeregowym opartym na interfejsie I2C/TWI (port PCF8574)
  *
  * @note Wymagana biblioteka twi.h oraz pcf8574.h
  *
  */
#ifndef LCD_H
#define LCD_H

/**< pliki nag³ówkowe bibliotek u¿ytkowych */
#include "main.h"
#include "pcf8574.h"
/**< standardowe pliki nag³ówkowe AVR-GCC */
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

/**< @def definicje sta³ych, trybów pracy, konfiguracji wyœwietlacza HD77480 */
#define HD44780_CLEAR                   0x01

#define HD44780_HOME                    0x02

#define HD44780_ENTRY_MODE              0x04
   #define HD44780_EM_SHIFT_CURSOR      0
   #define HD44780_EM_SHIFT_DISPLAY     1
   #define HD44780_EM_DECREMENT         0
   #define HD44780_EM_INCREMENT         2

#define HD44780_DISPLAY_ONOFF           0x08
   #define HD44780_DISPLAY_OFF          0
   #define HD44780_DISPLAY_ON           4
   #define HD44780_CURSOR_OFF           0
   #define HD44780_CURSOR_ON            2
   #define HD44780_CURSOR_NOBLINK       0
   #define HD44780_CURSOR_BLINK         1

#define HD44780_DISPLAY_CURSOR_SHIFT    0x10
   #define HD44780_SHIFT_CURSOR         0
   #define HD44780_SHIFT_DISPLAY        8
   #define HD44780_SHIFT_LEFT           0
   #define HD44780_SHIFT_RIGHT          4

#define HD44780_FUNCTION_SET            0x20
   #define HD44780_FONT5x7              0
   #define HD44780_FONT5x10             4
   #define HD44780_ONE_LINE             0
   #define HD44780_TWO_LINE             8
   #define HD44780_4_BIT                0
   #define HD44780_8_BIT                16

#define HD44780_CGRAM_SET               0x40

#define HD44780_DDRAM_SET               0x80

/**
  * Funkcja wpisuj¹ca polecenie/rozkaz do sterownika wyœwietlacza LCD
  *
  * @param  commandToWrite polecenie dla sterownika wyœwietlacza
  * @return brak
  *
  * @note Konfiguracja wyœwietlacza, polecenia steruj¹ce - dokumentacja.
  *
  */
void LCD_WriteCommand(uint8_t commandToWrite);

/**
  * Funkcja wpisuj¹ca dane do sterownika wyœwietlacza LCD
  *
  * @param  dataToWrite dana/znak (w kodzie ASCII) do wyœwietlenia
  * @return brak
  *
  * @note Znak wpisywany jest na pozycji kursora.
  *
  */
void LCD_WriteData(unsigned char dataToWrite);

/**
  * Funkcja wpisuj¹ca ³añcuch znaków na wyœwietlaczu
  *
  * @param  *text adres ³añcucha znaków (SRAM)
  * @return brak
  *
  * @note Znaki wpisywane s¹ od pozycji kursora, konfiguracja
  *       wymaga autoinkrementacjê pozycji kursora.
  * @todo Opracowaæ funkcjê do pobierania sta³ych ³añcuchów znaków
  *       z pamiêci programu (FLASH)
  *
  */
void LCD_WriteText(char *text);

/**
  * Funkcja przesuwaj¹ca pozycjê kursora
  *
  * @param  x numer kolumny (dla wyœwietlacza 2x16: 0..15)
  * @param  y numer wiersza (dla wyœwietlacza 2x16: 0..1)
  * @return brak
  *
  */
void LCD_GoTo(uint8_t x, uint8_t y);

/**
  * Funkcja czyszcz¹ca ekran wyœwietlacza
  *
  * @param  brak
  * @return brak
  *
  * @note Kursor przesuwany jest na pozycjê (0, 0)
  *
  */
void LCD_Clear(void);

/**
  * Funkcja przesuwaj¹ca kursor do pozycji pocz¹tkowej (0, 0)
  *
  * @param  brak
  * @return brak
  *
  */
void LCD_Home(void);

/**
  * Funkcja inicjalizuj¹ca wyœwietlacz w trybie domyœlnym
  *
  * @param  brak
  * @return brak
  *
  * @note Tryb domyœlny dla wyœwietlacza 2x16.
  *
  */
void LCD_Init(void);

// wrapper
#define LCDPutChar(data)	LCD_WriteData(data)
#define LCDInit				LCD_Init
#define LCDGoTo(pos)		LCD_GoTo(pos % 0x40, pos / 0x40)
#define LCDClear			LCD_Clear
#define LCDPutsCode(text)	LCD_WriteText(text)

#endif//LCD_H
