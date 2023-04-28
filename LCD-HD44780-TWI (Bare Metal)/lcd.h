/*
    Funkcje do obslugi wyswietlacza alfanumerycznego LCD

    * wyswietlacz podlaczony do portu D
    * tryb 4-bitowy
    * linie danych DB7-DB4 dolaczone do PD7-PD4
    * linie sterujace E -> PD2, RS -> PD3

 */

#ifndef LCD_H
#define LCD_H

#include "main.h"
#include <avr/io.h>
#include <util/delay.h>
#include "pcf8574.h"

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

void LCD_WriteCommand(unsigned char commandToWrite);
void LCD_WriteData(unsigned char dataToWrite);
void LCD_WriteText(char *text);
void LCD_GoTo(unsigned char x, unsigned char y);
void LCD_Clear(void);
void LCD_Home(void);
void LCD_Init(void);

// wrapper

#define LCDPutChar(data)	LCD_WriteData(data)
#define LCDInit				LCD_Init
#define LCDGoTo(pos)		LCD_GoTo(pos % 0x40, pos / 0x40)
#define LCDClear			LCD_Clear
#define LCDPutsCode(text)	LCD_WriteText(text)

#endif//LCD_H
