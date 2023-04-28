#ifndef PCF8574_H
#define PCF8574_H

#include <avr/io.h>
#include "twi.h"

//#define PCF8574Addr				0b01111110		// adres ukladu typu A
#define PCF8574Addr				0b01001110		// adres ukladu

uint8_t PCF8574_ReadPort(void);
void PCF8574_WritePort(uint8_t data);

#endif //PCF8574
