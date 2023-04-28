/*
    Funkcje do obslugi wyswietlacza alfanumerycznego LCD

    * wyswietlacz podlaczony do portu D
    * tryb 4-bitowy
    * linie danych DB7-DB4 dolaczone do PD7-PD4
    * linie sterujace E -> PD2, RS -> PD3

 */

#include "lcd.h"

void LCD_WriteCommand(unsigned char commandToWrite)
{
    uint8_t tmp = (1 << LCD_BKLight);
	
	//LCD_RS_PORT &= ~LCD_RS;

    //LCD_E_PORT |= LCD_E;
    //_LCD_OutNibble(dataToWrite >> 4);
    //LCD_E_PORT &= ~LCD_E;
	tmp |= (1 << LCD_E);
	tmp = (tmp & 0x0F) | ((commandToWrite >> 4) << LCD_DATA);
	PCF8574_WritePort(tmp);
	tmp &= ~(1 << LCD_E);
	PCF8574_WritePort(tmp);

    //LCD_E_PORT |= LCD_E;
    //_LCD_OutNibble(dataToWrite);
    //LCD_E_PORT &= ~LCD_E;
	tmp |= (1 << LCD_E);
	tmp = (tmp & 0x0F) | ((commandToWrite & 0x0F) << LCD_DATA);
	PCF8574_WritePort(tmp);
	tmp &= ~(1 << LCD_E);
	PCF8574_WritePort(tmp);

    _delay_us(50);
}

void LCD_WriteData(unsigned char dataToWrite)
{
    uint8_t tmp = (1 << LCD_BKLight);

	//LCD_RS_PORT |= LCD_RS;
	tmp |= (1 << LCD_RS);
	
    //LCD_E_PORT |= LCD_E;
    //_LCD_OutNibble(dataToWrite >> 4);
    //LCD_E_PORT &= ~LCD_E;
	tmp |= (1 << LCD_E);
	tmp = (tmp & 0x0F) | ((dataToWrite >> 4) << LCD_DATA);
	PCF8574_WritePort(tmp);
	tmp &= ~(1 << LCD_E);
	PCF8574_WritePort(tmp);

    //LCD_E_PORT |= LCD_E;
    //_LCD_OutNibble(dataToWrite);
    //LCD_E_PORT &= ~LCD_E;
	tmp |= (1 << LCD_E);
	tmp = (tmp & 0x0F) | ((dataToWrite & 0x0F) << LCD_DATA);
	PCF8574_WritePort(tmp);
	tmp &= ~(1 << LCD_E);
	PCF8574_WritePort(tmp);

    _delay_us(50);
}

void LCD_WriteText(char *text)
{
    while(*text) LCD_WriteData(*text++);
}

void LCD_GoTo(unsigned char x, unsigned char y)
{
    LCD_WriteCommand(HD44780_DDRAM_SET | (x + (0x40 * y)));
}

void LCD_Clear(void)
{
    LCD_WriteCommand(HD44780_CLEAR);
    _delay_ms(2);
}

void LCD_Home(void)
{
    LCD_WriteCommand(HD44780_HOME);
    _delay_ms(2);
}

void LCD_Init(void)
{
    uint8_t tmp = (1 << LCD_BKLight);
    _delay_ms(15);          // oczekiwanie na ustalibizowanie siê napiecia zasilajacego

	PCF8574_WritePort(tmp);
    //LCD_RS_PORT &= ~LCD_RS; // wyzerowanie linii RS
    //LCD_E_PORT &= ~LCD_E;   // wyzerowanie linii E

    for(uint8_t i = 0; i < 3; i++)  // trzykrotne powtórzenie bloku instrukcji
    {
        //LCD_E_PORT |= LCD_E;    //  E = 1
		tmp |= (1 << LCD_E);
        tmp = (tmp & 0x0F) | (0x03 << LCD_DATA);
		//_LCD_OutNibble(0x03);   // tryb 8-bitowy
		PCF8574_WritePort(tmp);
        //LCD_E_PORT &= ~LCD_E;   // E = 0
		tmp &= ~(1 << LCD_E);
		PCF8574_WritePort(tmp);
        _delay_ms(5);           // czekaj 5ms
    }

    //LCD_E_PORT |= LCD_E;    // E = 1
    //_LCD_OutNibble(0x02);   // tryb 4-bitowy
	tmp |= (1 << LCD_E);
	tmp = (tmp & 0x0F) | (0x02 << LCD_DATA);
	PCF8574_WritePort(tmp);
	
    //LCD_E_PORT &= ~LCD_E;   // E = 0
	tmp &= ~(1 << LCD_E);
	PCF8574_WritePort(tmp);

    _delay_ms(1); // czekaj 1ms
    LCD_WriteCommand(HD44780_FUNCTION_SET | HD44780_FONT5x7 | HD44780_TWO_LINE | HD44780_4_BIT); // interfejs 4-bity, 2-linie, znak 5x7
    LCD_WriteCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_OFF); // wylaczenie wyswietlacza
    LCD_WriteCommand(HD44780_CLEAR); // czyszczenie zawartosci pamieci DDRAM
    _delay_ms(2);
    LCD_WriteCommand(HD44780_ENTRY_MODE | HD44780_EM_SHIFT_CURSOR | HD44780_EM_INCREMENT);// inkrementaja adresu i przesuwanie kursora
    LCD_WriteCommand(HD44780_DISPLAY_ONOFF | HD44780_DISPLAY_ON | HD44780_CURSOR_OFF | HD44780_CURSOR_NOBLINK); // wlacz LCD, bez kursora i mrugania
}
