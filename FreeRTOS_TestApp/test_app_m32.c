/** @file test_app_m32.c
  *
  *	Przyk³adowa aplikacja testuj¹ca metodê obs³ugi interfejsu 1-Wire
  *	z wykorzystaniem uk³adu transmisji szeregowej SPI
  *	
  *	Do magistrali 1-Wire do³¹czony zosta³ uk³ad czujnika temperatury
  * typu DS18B20. Oprogramowanie u¿ytkowe pracuje w œrodowisku
  * systemu operacyjnego FreeRTOS.
  * Konfiguracja systemu operacyjnego w pliku FreeRTOSConfig.h.
  *
  */

/**< pliki nag³ówkowe FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/**< pliki nag³ówkowe AVR-GCC */
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>

/**< pliki nag³ówkowe projektu */
/**< obs³uga wyœwietlacza LCD-HD77480 z interfejsem I2C */
#include "twi.h"
#include "pcf8574.h"
#include "lcd.h"
/**< obs³uga interfejsu 1-Wire z wykorzystaniem SPI */
#include "spi1wire.h"
/**< obs³uga czujnika temperatury */
#include "ds18b20.h"
/**< funkcje pomocnicze do wyœwietlania temperatury */
#include "utility.h"

/**< podstawowy priorytet zadania */
#define main_TASK_PRIORITY (tskIDLE_PRIORITY + 1)


/**
  * Funkcja inicjalizuj¹ca wszystkie uk³ady peryferyjne
  */
static void prvInitHardware(void);
static void prvInitHardware(void)
{
	TWI_init();
	LCD_Init();
	
	SPI1Wire_Init();
}


/**< kolejka wykorzystywana do przekazywania wartoœci temperatury */
xQueueHandle QueueMeasurement = NULL;
/**< semafor binarny wykorzystany do sygnalizacji koniecznoœci wykonania pomiaru temperatury */
xSemaphoreHandle SemaphoreStartMeasure = NULL;


/**
  * Zadanie realizuj¹ce pomiar temperatury
  *
  * Stanowi element posrednicz¹cy pomiêdzy aplikacj¹ u¿ytkow¹ (zadania u¿ytkowe) a funkcjami
  * niskopoziomowymi z biblioteki spi1wire.h.
  *
  */
static void vMeasureTask(void *pvParameters);
static void vMeasureTask(void *pvParameters)
{
	( void ) pvParameters;

	/**< zmienna wykorzystywana do przechowywania wyniku pomiaru temperatury */
	uint16_t measure = 0;
	
	for( ;; )
	{
		/**< oczekiwanie na ¿¹danie wykonania pomiaru */
		if (xSemaphoreTake(SemaphoreStartMeasure, portMAX_DELAY))
		{
			/**< zerowanie oraz sprawdzenie dostêpnoœci uk³adu SLAVE
			     na magistrali 1-Wire */
			if (SPI1Wire_ResetPresence() != SPI1WIRE_NO_PRESENCE)
			{
				/**< pominiêcie adresowania uk³adu SLAVE, z za³o¿enia jest tylko jeden */
  				SPI1Wire_Write(cmd_DS18x20_SkipROM);
				/**< wys³anie rozkazu inicjuj¹cego pomiar temperatury w uk³adzie SLAVE*/
				SPI1Wire_Write(cmd_DS18x20_ConvertT);
				/**< oczekiwanie na zakoñczenie pomiaru, typowo czas ten nie przekracza 750ms */
				vTaskDelay(750 / portTICK_RATE_MS);

				/**< odczyt temperatury, wys³anie rozkazu RESET */
				SPI1Wire_ResetPresence();
				/**< pominiêcie adresowania uk³adu SLAVE */
				SPI1Wire_Write(cmd_DS18x20_SkipROM);
				/**< odczyt pamiêci RAM czujnika, pierwsze dwa bajty to temperatura */
				SPI1Wire_Write(cmd_DS18x20_ReadScratchpad);
				uint8_t tempL = SPI1Wire_Read();
				uint8_t tempH = SPI1Wire_Read();
				/**< odczytana wartoœæ temperatury */
				measure = (tempH << 8) + tempL;
			}
			else
			    /**< brak uk³adu SLAVE lub nie odpowiada kodowana jako -1 (lub 0xFFFF),
				     nie mo¿e byæ 0, bo wartoœæ ta mo¿e okreœlaæ temperaturê 0C */
				measure = 0xFFFF;

			/**< umieszczenie wartoœci temperatury w kolejce */
			xQueueSend(QueueMeasurement, &measure, 10 / portTICK_RATE_MS);
		}
	}
}


/**
  * Zadanie g³ówne wyœwietlaj¹ce temperaturê
  *
  */
static void vDisplayTask(void *pvParameters);
static void vDisplayTask(void *pvParameters)
{
	( void ) pvParameters;
	
	for( ;; )
	{
		/**< wys³anie ¿¹dania wykonania pomiaru temperatury */
		xSemaphoreGive(SemaphoreStartMeasure);
		
		uint16_t measure;
		
		LCD_Clear();

		/**< pobranie wartoœci temperatury */
		if (xQueueReceive(QueueMeasurement, &measure, 800 / portTICK_RATE_MS))
		{
			if (measure != 0xFFFF)
			{
				/**< sprawdzenie czy wynik pomiaru jest liczb¹ ujemn¹,
				     wyœwietlenie znaku i ew. wyznaczenie modu³u */
				if ((measure & 0x8000) != 0)
				{
					LCDPutChar('-');
					measure = ~measure + 1;
				}
				else
				{
					LCDPutChar('+');
				}

				/**< wyœwietlenie wartoœci ca³kowitej */
				LCDWriteInteger(measure >> 4);	

				LCDPutChar('.');

				/**< wyœwietlenie wartoœci u³amkowej */
				LCDWriteFractional(measure & 0x0F);
				LCDPutChar('C');
			}
			else
			{
				/**< brak uk³adu SLAVE lub nie odpowiada */
				LCDPutsCode("No DS18B20 found!");
			}
		}
		/**< zadanie vMeasureTask nie odpowiada */
		else LCDPutsCode("Time out");
		
		/**< aktualizacja wyœwietlacza co ok. 2 sek. */
		vTaskDelay(1250 / portTICK_RATE_MS);
	}
}


void main(void)
{
	/**< inicjalizacja uk³adów peryferyjnych */
	prvInitHardware();

	/**< utworzenie kolejki i semafora binarnego */
	QueueMeasurement = xQueueCreate(5, sizeof(uint16_t));
	vSemaphoreCreateBinary(SemaphoreStartMeasure);
	/**< wstêpne zrowanie semafora, poprzednie wersje FreeRTOS-a ustawia³y
	     go od razu po utworzeniu */
	xSemaphoreTake(SemaphoreStartMeasure, 0 / portTICK_RATE_MS);
	
	/**< utworzenie zadañ */
	xTaskCreate(vDisplayTask,
				(const int8_t*) "lcd",
				configMINIMAL_STACK_SIZE,
				NULL,
				main_TASK_PRIORITY + 1,
				NULL);

	xTaskCreate(vMeasureTask,
				(const int8_t*) "lcd",
				configMINIMAL_STACK_SIZE,
				NULL,
				main_TASK_PRIORITY,
				NULL);

	/**< uruchomienie systemu operacyjnego */
	vTaskStartScheduler();

	for(;;);
}
