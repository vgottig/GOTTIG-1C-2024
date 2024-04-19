/*! @mainpage Proyecto 2. Ejercicio 2
 *
 * @section genDesc General Description
 *
 * Se modifica la actividad del Ejercicio 1 de manera de utilizar interrupciones
 * para el control de las teclas y el control de tiempos (Timers).
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
#include "timer_mcu.h"
/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD_LED_1_US 1000000

/*==================[internal data definition]===============================*/
TaskHandle_t led1_task_handle = NULL;

/** @def tecla1
 *  @brief Activa y desactiva la medición
 */
bool tecla1 = false;

/** @def tecla2
 *  @brief Mantiene el resultado de la medición
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
		ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // La tarea espera en este punto hasta recibir una notificación

		if (tecla1)
		{

			distancia = HcSr04ReadDistanceInCentimeters();

			if (!tecla2) // Si no apreto la tecla2
			{								 
				LcdItsE0803Write(distancia); // Pantalla LCD
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
	}
}

/**
 * @brief Función para interrumpir con la tecla 1 
 */
void OnOff()
{
	tecla1 = !tecla1;
}

/**
 * @brief Función para interrumpir con la tecla 2
 */
void mantenerResultado()
{
	tecla2 = !tecla2;
}

/**
 * @brief Función invocada en la interrupción del timer 
 */
void FuncTimerB(void *param)
{
	vTaskNotifyGiveFromISR(led1_task_handle, pdFALSE); // Envía una notificación a la tarea asociada al LED_2
}

/*==================[external functions definition]==========================*/
void app_main(void)
{
	LcdItsE0803Init();
	HcSr04Init(GPIO_3, GPIO_2);
	SwitchesInit();
	LedsInit();

	// Llamada a todos los parámetros
	timer_config_t my_timer = {
		.timer = TIMER_B,
		.period = 1000000,
		.func_p = FuncTimerB,
		.param_p = NULL};
	TimerInit(&my_timer); // Inicialización del timer
	TimerStart(my_timer.timer);

	// Creación de tarea
	xTaskCreate(&distancias_task, "distancias_task", 512, NULL, 5, &led1_task_handle);

	// Interrupciones
	SwitchActivInt(SWITCH_1, &OnOff, NULL);
	SwitchActivInt(SWITCH_2, &mantenerResultado, NULL);
}
/*==================[end of file]============================================*/