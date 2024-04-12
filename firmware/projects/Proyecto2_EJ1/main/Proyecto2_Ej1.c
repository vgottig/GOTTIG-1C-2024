/*! @mainpage Proyecto 2. Ejercicio 1
 *
 * @section genDesc General Description
 *
 * Se muestra la distancia, utilizando el display LCD,
 * que mide el sensor de ultrasonido HCRS-04 utilizando los leds de la siguiente manera:
 * Si la distancia es menor a 10 cm, se apagan todos los LEDs.
 * Si la distancia está entre 10 y 20 cm, se enciende el LED_1.
 * Si la distancia está entre 20 y 30 cm, se enciende el LED_2 y LED_1.
 * Si la distancia es mayor a 30 cm, se enciende el LED_3, LED_2 y LED_1.
 * 
 * Utilizando las tecla 1 y tecla 2 se activa y detiene la medición, y se mantiene
 * el resultado respectivamente con un refresco de medición de 1 segundo
 *
 * <a href="https://drive.google.com/...">Operation Example</a>
 *
 * @section hardConn Hardware Connection
 *
 * |  EDU-CIAA-NXP  |    Peripheral 	|
 * |:--------------:|:------------------|
 * | 	GPIO_2	 	|  	  ECHO  	   	|
 * | 	GPIO_3	 	|  	  TRIGGER 	   	|
 * | 	+5V 	 	|  	  +5V   	   	|
 * | 	GND 	 	|  	  GND   	   	|
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
#include "gpio_mcu.h"
#include "lcditse0803.h"
#include "hc_sr04.h"
#include "delay_mcu.h"
/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD 1000
#define CONFIG_BLINK_PERIOD_2 50

/*==================[internal data definition]===============================*/

/** @def tecla1
*   @brief Activa y desactiva la medición
*/ 
bool tecla1 = false;

/** @def tecla2
*   @brief Mantiene el resultado de la medición
*/ 
bool tecla2 = false;

/*==================[internal functions declaration]=========================*/

/** @fn void distancias_task(void *pvParameter)
 *  @brief Tarea que mide la distancia utilizando el sensor y los leds
 *  @param[in] void *pvParameter
 *  @return 
*/
void distancias_task(void *pvParameter)
{

	uint16_t distancia;

	while (1)
	{
		if (tecla1)
		{

			distancia = HcSr04ReadDistanceInCentimeters();

			if (!tecla2)
			{								 // si no apreto la tecla2
				LcdItsE0803Write(distancia); // pantalla LCD
			}

			if (distancia < 10)
			{
				LedsOffAll();
			}

			else if (distancia > 10 && distancia < 20)
			{
				LedOn(LED_1);
				LedOff(LED_2);
				LedOff(LED_3);
			}

			else if (distancia > 20 && distancia < 30)
			{
				LedOn(LED_1);
				LedOn(LED_2);
				LedOff(LED_3);
			}

			else if (distancia > 30)
			{
				LedOn(LED_1);
				LedOn(LED_2);
				LedOn(LED_3);
			}
		}
		else
		{
			LcdItsE0803Off();
			LedsOffAll();
		}

		vTaskDelay(CONFIG_BLINK_PERIOD / portTICK_PERIOD_MS);
	}
}

/** @fn void switchTeclas_task(void *pvParameter)
 *  @brief Tarea que activa y detiene la medición utilizando la tecla 1 y mantiene un resultado utilizando la tecla 2
 *  @param[in] void *pvParameter
 *  @return 
*/
void switchTeclas_task(void *pvParameter)
{
	uint8_t teclas;

	while (1)
	{
		teclas = SwitchesRead();
		switch (teclas)
		{
		case SWITCH_1:
			// TEC1 para activar y detener la medición
			tecla1 = !tecla1;
			break;
		case SWITCH_2:
			// TEC2 para mantener el resultado (“HOLD”)
			tecla2 = !tecla2;
			break;
		}

		vTaskDelay(CONFIG_BLINK_PERIOD_2 / portTICK_PERIOD_MS);
	}
}
/*==================[external functions definition]==========================*/
void app_main(void)
{
	LedsInit();
	LcdItsE0803Init();
	HcSr04Init(GPIO_3, GPIO_2);
	SwitchesInit();

	xTaskCreate(&distancias_task, "distancias_task", 512, NULL, 5, NULL);
	xTaskCreate(&switchTeclas_task, "switchTeclas_task", 512, NULL, 5, NULL);
}
/*==================[end of file]============================================*/