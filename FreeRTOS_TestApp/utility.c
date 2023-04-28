/** @file utility.c
  */

#include "utility.h"

/**
  * @var str00-str15 sta�e, przechowywane w pami�ci FLASH (architektura AVR),
  * zawieraj�ce �a�cuchy znak�w, wykorzystywane do konwersji warto�ci u�amkowej
  * na �a�cuch znak�w (metod� tablicow�)
  */
const uint8_t str00[] PROGMEM = "0000";
const uint8_t str01[] PROGMEM = "0625";
const uint8_t str02[] PROGMEM = "1250";
const uint8_t str03[] PROGMEM = "1875";
const uint8_t str04[] PROGMEM = "2500";
const uint8_t str05[] PROGMEM = "3125";
const uint8_t str06[] PROGMEM = "3750";
const uint8_t str07[] PROGMEM = "4375";
const uint8_t str08[] PROGMEM = "5000";
const uint8_t str09[] PROGMEM = "5625";
const uint8_t str10[] PROGMEM = "6250";
const uint8_t str11[] PROGMEM = "6875";
const uint8_t str12[] PROGMEM = "7500";
const uint8_t str13[] PROGMEM = "8125";
const uint8_t str14[] PROGMEM = "8750";
const uint8_t str15[] PROGMEM = "9375";

/**
  * @var stringFraction tablica zawieraj�ca wska�niki do �a�cuch�w znak�w
  * str00-str15, wykorzystywana do konwersji warto�ci u�amkowej (4-bitowej)
  * na �a�cuch znak�w 
  */
const uint8_t* const stringFraction[] PROGMEM = { str00, str01, str02, str03,
												  str04, str05, str06, str07,
	                                              str08, str08, str10, str11,
                                                  str12, str13, str14, str15 };
												  
void LCDWriteFractional(uint8_t fract)
{
	uint8_t tmp;
	uint8_t *addr = 0;

	addr = (uint8_t*)pgm_read_word(&stringFraction[fract & 0x0F]);

	for (uint8_t i = 0; i < 4; i++)
	{
		tmp = pgm_read_byte(addr + i);
		LCDPutChar(tmp);
	}
}

void LCDWriteInteger(uint8_t integer)
{
	LCDPutChar((integer / 100) + '0');
	LCDPutChar(((integer % 100) / 10) + '0');
	LCDPutChar((integer % 10) + '0');
}
