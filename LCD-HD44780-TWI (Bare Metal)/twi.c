#include "twi.h"

void TWI_init(void)
{
	TWBR = 12 * 3;

	//
	// SCLfreq = CLKXtal / (16 + 2 * TWBR * 4 ^ TWPS1:TWPS0)
	//
}

void TWI_start(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);
	TWI_wait();
}

void TWI_stop(void)
{
	TWCR = (1<<TWINT)|(1<<TWSTO)|(1<<TWEN);
}

void TWI_wait(void)
{
	while ((TWCR & (1 << TWINT)) == 0);
}

void TWI_write(uint8_t data)
{
	TWDR = data;
	TWCR = (1<<TWINT)|(1<<TWEN);
	TWI_wait();
}

uint8_t TWI_read(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN);
	TWI_wait();
	return TWDR;
}
