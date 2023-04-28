#include "pcf8574.h"

uint8_t PCF8574_ReadPort(void)
{
	TWI_start();
	TWI_write(PCF8574Addr | 0b00000001);
	uint8_t tmp = TWI_read();
	TWI_stop();

	return tmp;
}

void PCF8574_WritePort(uint8_t data)
{
	TWI_start();
	TWI_write(PCF8574Addr);
	TWI_write(data);
	TWI_stop();
}
