/*! @mainpage Proyecto 2. Ejercicio 3
 *
 * @section genDesc General Description
 *
 * Se modifica la actividad del Ejercicio 2 agregando ahora el puerto serie.
 * Se envían los datos de las mediciones para poder observarlos en un terminal en la PC,
 * siguiendo el siguiente formato:
 * - 3 dígitos ascii + 1 carácter espacio + dos caracteres para la unidad (cm) + cambio de línea “ \r\n”
 *
 * Además debe ser posible controlar la EDU-ESP de la siguiente manera:
 * - Con las teclas “O” y “H”, replicar la funcionalidad de las teclas 1 y 2 de la EDU-ESP
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
#include <uart_mcu.h>
/*==================[macros and definitions]=================================*/
#define CONFIG_BLINK_PERIOD_LED_1_US 1000000
#define UART_NO_INT 0

/*==================[internal data definition]===============================*/
TaskHandle_t led1_task_handle = NULL; // Etiqueta

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

			UartSendString(UART_PC, (char *)UartItoa(distancia, 10)); // Lo que mido, lo convierto a string y lo muestro en la PC
			UartSendString(UART_PC, " cm\n\r");

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

/**
 * @brief Función que con las teclas “O” y “H”, replica la funcionalidad de las teclas 1 y 2 de la placa
 */
void teclasControl()
{
	uint8_t letra;
	UartReadByte(UART_PC, &letra);
	switch (letra)
	{
	case 'O':
		OnOff();
		break;
	case 'H':
		mantenerResultado();
		break;
	}
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

	serial_config_t my_uart = {
		.port = UART_PC,
		.baud_rate = 9600,
		.func_p = teclasControl,
		.param_p = NULL};
	UartInit(&my_uart);

	// Interrupciones
	SwitchActivInt(SWITCH_1, &OnOff, NULL);
	SwitchActivInt(SWITCH_2, &mantenerResultado, NULL);

	// Creación de tarea
	xTaskCreate(&distancias_task, "distancias_task", 512, NULL, 5, &led1_task_handle);

	TimerStart(my_timer.timer); // Inicia el recuento del timer
}
/*==================[end of file]============================================*/