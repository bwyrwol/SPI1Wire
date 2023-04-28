/** @file test_app_m32.c
  *
  *	Przyk�adowa aplikacja testuj�ca metod� obs�ugi interfejsu 1-Wire
  *	z wykorzystaniem uk�adu transmisji szeregowej SPI
  *	
  *	Do magistrali 1-Wire do��czony zosta� uk�ad czujnika temperatury
  * typu DS18B20. Oprogramowanie u�ytkowe pracuje w �rodowisku
  * systemu operacyjnego FreeRTOS.
  * Konfiguracja systemu operacyjnego w pliku FreeRTOSConfig.h.
  *
  */

/**< pliki nag��wkowe FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/**< pliki nag��wkowe AVR-GCC */
#include <avr/pgmspace.h>
#include <avr/io.h>
#include <avr/interrupt.h>

/**< pliki nag��wkowe projektu */
/**< obs�uga wy�wietlacza LCD-HD77480 z interfejsem I2C */
#include "twi.h"
#include "pcf8574.h"
#include "lcd.h"
/**< obs�uga interfejsu 1-Wire z wykorzystaniem SPI */
#include "spi1wire.h"
/**< obs�uga czujnika temperatury */
#include "ds18b20.h"
/**< funkcje pomocnicze do wy�wietlania temperatury */
#include "utility.h"

/**< podstawowy priorytet zadania */
#define main_TASK_PRIORITY (tskIDLE_PRIORITY + 1)


/**
  * Funkcja inicjalizuj�ca wszystkie uk�ady peryferyjne
  */
static void prvInitHardware(void);
static void prvInitHardware(void)
{
	TWI_init();
	LCD_Init();
	
	SPI1Wire_Init();
}


/**< kolejka wykorzystywana do przekazywania warto�ci temperatury */
xQueueHandle QueueMeasurement = NULL;
/**< semafor binarny wykorzystany do sygnalizacji konieczno�ci wykonania pomiaru temperatury */
xSemaphoreHandle SemaphoreStartMeasure = NULL;


/**
  * Zadanie realizuj�ce pomiar temperatury
  *
  * Stanowi element posrednicz�cy pomi�dzy aplikacj� u�ytkow� (zadania u�ytkowe) a funkcjami
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
		/**< oczekiwanie na ��danie wykonania pomiaru */
		if (xSemaphoreTake(SemaphoreStartMeasure, portMAX_DELAY))
		{
			/**< zerowanie oraz sprawdzenie dost�pno�ci uk�adu SLAVE
			     na magistrali 1-Wire */
			if (SPI1Wire_ResetPresence() != SPI1WIRE_NO_PRESENCE)
			{
				/**< pomini�cie adresowania uk�adu SLAVE, z za�o�enia jest tylko jeden */
  				SPI1Wire_Write(cmd_DS18x20_SkipROM);
				/**< wys�anie rozkazu inicjuj�cego pomiar temperatury w uk�adzie SLAVE*/
				SPI1Wire_Write(cmd_DS18x20_ConvertT);
				/**< oczekiwanie na zako�czenie pomiaru, typowo czas ten nie przekracza 750ms */
				vTaskDelay(750 / portTICK_RATE_MS);

				/**< odczyt temperatury, wys�anie rozkazu RESET */
				SPI1Wire_ResetPresence();
				/**< pomini�cie adresowania uk�adu SLAVE */
				SPI1Wire_Write(cmd_DS18x20_SkipROM);
				/**< odczyt pami�ci RAM czujnika, pierwsze dwa bajty to temperatura */
				SPI1Wire_Write(cmd_DS18x20_ReadScratchpad);
				uint8_t tempL = SPI1Wire_Read();
				uint8_t tempH = SPI1Wire_Read();
				/**< odczytana warto�� temperatury */
				measure = (tempH << 8) + tempL;
			}
			else
			    /**< brak uk�adu SLAVE lub nie odpowiada kodowana jako -1 (lub 0xFFFF),
				     nie mo�e by� 0, bo warto�� ta mo�e okre�la� temperatur� 0C */
				measure = 0xFFFF;

			/**< umieszczenie warto�ci temperatury w kolejce */
			xQueueSend(QueueMeasurement, &measure, 10 / portTICK_RATE_MS);
		}
	}
}


/**
  * Zadanie g��wne wy�wietlaj�ce temperatur�
  *
  */
static void vDisplayTask(void *pvParameters);
static void vDisplayTask(void *pvParameters)
{
	( void ) pvParameters;
	
	for( ;; )
	{
		/**< wys�anie ��dania wykonania pomiaru temperatury */
		xSemaphoreGive(SemaphoreStartMeasure);
		
		uint16_t measure;
		
		LCD_Clear();

		/**< pobranie warto�ci temperatury */
		if (xQueueReceive(QueueMeasurement, &measure, 800 / portTICK_RATE_MS))
		{
			if (measure != 0xFFFF)
			{
				/**< sprawdzenie czy wynik pomiaru jest liczb� ujemn�,
				     wy�wietlenie znaku i ew. wyznaczenie modu�u */
				if ((measure & 0x8000) != 0)
				{
					LCDPutChar('-');
					measure = ~measure + 1;
				}
				else
				{
					LCDPutChar('+');
				}

				/**< wy�wietlenie warto�ci ca�kowitej */
				LCDWriteInteger(measure >> 4);	

				LCDPutChar('.');

				/**< wy�wietlenie warto�ci u�amkowej */
				LCDWriteFractional(measure & 0x0F);
				LCDPutChar('C');
			}
			else
			{
				/**< brak uk�adu SLAVE lub nie odpowiada */
				LCDPutsCode("No DS18B20 found!");
			}
		}
		/**< zadanie vMeasureTask nie odpowiada */
		else LCDPutsCode("Time out");
		
		/**< aktualizacja wy�wietlacza co ok. 2 sek. */
		vTaskDelay(1250 / portTICK_RATE_MS);
	}
}


void main(void)
{
	/**< inicjalizacja uk�ad�w peryferyjnych */
	prvInitHardware();

	/**< utworzenie kolejki i semafora binarnego */
	QueueMeasurement = xQueueCreate(5, sizeof(uint16_t));
	vSemaphoreCreateBinary(SemaphoreStartMeasure);
	/**< wst�pne zrowanie semafora, poprzednie wersje FreeRTOS-a ustawia�y
	     go od razu po utworzeniu */
	xSemaphoreTake(SemaphoreStartMeasure, 0 / portTICK_RATE_MS);
	
	/**< utworzenie zada� */
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
