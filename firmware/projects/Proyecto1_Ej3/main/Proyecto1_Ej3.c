/*! @mainpage Template
 *
 * @section genDesc General Description
 *
 * This section describes how the program works.
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |    Peripheral  |   ESP32   	|
 * |:--------------:|:--------------|
 * | 	PIN_X	 	| 	GPIO_X		|
 *
 *
 * @section changelog Changelog
 *
 * |   Date	    | Description                                    |
 * |:----------:|:-----------------------------------------------|
 * | 12/09/2023 | Document creation		                         |
 *
 * @author Valentina Gottig (valentinagottig@gmail.com)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led.h"
#include "switch.h"
/*==================[macros and definitions]=================================*/
#define ON 1
#define OFF 0
#define TOGGLE 2
#define CONFIG_BLINK_PERIOD 100
/*==================[internal data definition]===============================*/

/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/

struct leds
{
    uint8_t mode;       //ON, OFF, TOGGLE
	uint8_t n_led;      //indica el número de led a controlar
	uint8_t n_ciclos;   //indica la cantidad de ciclos de ncendido/apagado
	uint16_t periodo;    //indica el tiempo de cada ciclo
} my_leds;

void controlLeds(struct leds *led)
{
	switch (led->mode)
	{
	case ON:
		switch (led->n_led)
		{
		case 1:
			LedOn(LED_1);
			break;

		case 2:
			LedOn(LED_2);
			break;

		case 3:
			LedOn(LED_3);
			break;
		}
		break;

	case OFF:
		switch (led->n_led)
		{
		case 1:
			LedOff(LED_1);
			break;

		case 2:
			LedOff(LED_2);
			break;

		case 3:
			LedOff(LED_3);
			break;
		}
		break;

	case TOGGLE:
		for (int i = 0; i < led->n_ciclos; i++)
		{

			if (led->n_led == 1)
			{
				LedToggle(LED_1);
			}

			else if (led->n_led == 2)
			{
				LedToggle(LED_2);
			}

			else
				{
					LedToggle(LED_3);
				}

			for (int j = 0; j < led->periodo / CONFIG_BLINK_PERIOD; j++)
			{
				vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
			}
		}

		break;
	}
}

void app_main(void){
	printf("Hello world!\n");

	LedsInit();

	my_leds.mode=TOGGLE;
	my_leds.n_led=2;
	my_leds.n_ciclos=10;
	my_leds.periodo=500;

	controlLeds(&my_leds);

}
/*==================[end of file]============================================*/