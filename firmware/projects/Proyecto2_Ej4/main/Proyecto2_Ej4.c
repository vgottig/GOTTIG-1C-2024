/*! @mainpage Proyecto 2. Ejercicio 4
 *
 * @section genDesc General Description
 *
 * Se diseña e implementa una aplicación, basada en el driver analog_io_mcu.h y
 * el driver de transmisión serie uart_mcu.h, que va a digitalizar una señal analógica y
 * la va a transmitir a un graficador de puerto serie de la PC. Se toma la entrada CH1 del
 * conversor AD y se realiza la transimisón por la UART conectada al puerto serie de la PC,
 * en un formato compatible con un graficador por puerto serie.
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
#include "analog_io_mcu.h"
#include "uart_mcu.h"
#include "timer_mcu.h"

/*==================[macros and definitions]=================================*/
#define TIME_PERIOD 2000
#define TIME_PERIOD2 4000
#define BUFFER_SIZE 231
uint8_t dato = 0;

/*==================[internal data definition]===============================*/
const char ecg[BUFFER_SIZE] = {
    76, 77, 78, 77, 79, 86, 81, 76, 84, 93, 85, 80,
    89, 95, 89, 85, 93, 98, 94, 88, 98, 105, 96, 91,
    99, 105, 101, 96, 102, 106, 101, 96, 100, 107, 101,
    94, 100, 104, 100, 91, 99, 103, 98, 91, 96, 105, 95,
    88, 95, 100, 94, 85, 93, 99, 92, 84, 91, 96, 87, 80,
    83, 92, 86, 78, 84, 89, 79, 73, 81, 83, 78, 70, 80, 82,
    79, 69, 80, 82, 81, 70, 75, 81, 77, 74, 79, 83, 82, 72,
    80, 87, 79, 76, 85, 95, 87, 81, 88, 93, 88, 84, 87, 94,
    86, 82, 85, 94, 85, 82, 85, 95, 86, 83, 92, 99, 91, 88,
    94, 98, 95, 90, 97, 105, 104, 94, 98, 114, 117, 124, 144,
    180, 210, 236, 253, 227, 171, 99, 49, 34, 29, 43, 69, 89,
    89, 90, 98, 107, 104, 98, 104, 110, 102, 98, 103, 111, 101,
    94, 103, 108, 102, 95, 97, 106, 100, 92, 101, 103, 100, 94, 98,
    103, 96, 90, 98, 103, 97, 90, 99, 104, 95, 90, 99, 104, 100, 93,
    100, 106, 101, 93, 101, 105, 103, 96, 105, 112, 105, 99, 103, 108,
    99, 96, 102, 106, 99, 90, 92, 100, 87, 80, 82, 88, 77, 69, 75, 79,
    74, 67, 71, 78, 72, 67, 73, 81, 77, 71, 75, 84, 79, 77, 77, 76, 76,
};

/*==================[internal functions declaration]=========================*/
TaskHandle_t task_handle1 = NULL;
TaskFunction_t task_handle2 = NULL;

void notifyTask(void *param)
{
    vTaskNotifyGiveFromISR(task_handle1, pdFALSE);
    vTaskNotifyGiveFromISR(task_handle2, pdFALSE);
}

static void analogToDigital(void *pvParameter)
{
    uint16_t valorDigitalizado;

    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        AnalogInputReadSingle(CH1, &valorDigitalizado);

        UartSendString(UART_PC, (char *)UartItoa(valorDigitalizado, 10));

        UartSendString(UART_PC, (char *)"\r\n");
    }
}

static void digitalToAnalog(void *pvParamete)
{
    while (true)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        AnalogOutputWrite(ecg[dato]); // Convierte de digital a analógico
        dato ++;
        
        if (dato == sizeof(ecg)) // Recorro los datos del ECG
            dato = 0;
    }
}

/*==================[external functions definition]==========================*/
void app_main(void)
{
    analog_input_config_t analogInput1 = {
        .input = CH1,
    };

    analog_input_config_t analogInput2 = {
        .input = CH0,
        .mode = ADC_SINGLE,
    };

    // Inicialización de timers
    timer_config_t timer_1 = {
        .timer = TIMER_A,
        .period = TIME_PERIOD,
        .func_p = notifyTask,
        .param_p = NULL};

    timer_config_t timer_2 = {
        .timer = TIMER_B,
        .period = TIME_PERIOD,
        .func_p = notifyTask,
        .param_p = NULL};

    // Configuro los parámetros de comunicación serial, específicamente para la UART
    serial_config_t serial_global = {
        .port = UART_PC,     // Indica el puerto
        .baud_rate = 115200, // Velocidad de transmisión de datos en baudios
        .func_p = NULL,
        .param_p = NULL};

    TimerInit(&timer_1);
    TimerInit(&timer_2);
    AnalogOutputInit(); // Inicializa la salida analógica
    AnalogInputInit(&analogInput1); // Inicializa la entrada analógica utilizando la configuración proporcionada en analogInput1
   
    xTaskCreate(&analogToDigital, "leer y enviar", 2048, NULL, 5, &task_handle1); // Crea una tarea llamada deAnalogico_aDigital
                                                                                  // El puntero task_handle1 es una bandera se utiliza para
                                                                                  // almacenar el identificador de la tarea creada
    xTaskCreate(&digitalToAnalog, "leer y enviar", 2048, NULL, 5, &task_handle2);

    UartInit(&serial_global);
    TimerStart(timer_1.timer);
    TimerStart(timer_2.timer);
}
/*==================[end of file]============================================*/