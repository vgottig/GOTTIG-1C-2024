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
 * @author Albano Peñalva (albano.penalva@uner.edu.ar)
 *
 */

/*==================[inclusions]=============================================*/
#include <stdio.h>
#include <stdint.h>
#include "gpio_mcu.h"
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/
typedef struct
{
	gpio_t pin; /*!< GPIO pin number */
	io_t dir; /*!< GPIO direction '0' IN; '1' OUT*/
} gpioConf_t;


/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/
int8_t convertToBcdArray (uint32_t data, uint8_t digits, uint8_t * bcd_number) 
{
	while(digits>0){
		bcd_number[digits-1]=data%10; //123%10=3 ... 12%10=2 ...
		data=data/10; //123/10=12 ... 12/10=1
		digits--;
	}
	return true;
}

void functionBCD(uint8_t digit, gpioConf_t *vector){
	for(uint8_t i=0; i<4; i++){
		GPIOInit(vector[i].pin,vector[i].dir);
	}

	for(int i=0; i<4; i++){
		if((digit&(1<<i))==0){
			GPIOOff(vector[i].pin);
		}
		else
			GPIOOn(vector[i].pin);
	}
}

void mostrarPorDisplay(uint32_t dato, uint8_t digitos, gpioConf_t *vectorPines, gpioConf_t *vectorPuertos){

	uint8_t array[3];

	convertToBcdArray(dato,digitos,array);

	for(int i=0; i<digitos; i++){
		functionBCD(array[i],vectorPines);
		GPIOOn(vectorPuertos[i].pin);
		GPIOOff(vectorPuertos[i].pin);
	}
}

void app_main(void){

	gpioConf_t vectorPines[4]={
		{GPIO_20,GPIO_OUTPUT},
		{GPIO_21,GPIO_OUTPUT},
		{GPIO_22,GPIO_OUTPUT},
		{GPIO_23,GPIO_OUTPUT}
	};

	for(uint8_t i=0; i<4; i++){
		GPIOInit(vectorPines[i].pin,vectorPines[i].dir);
	}

gpioConf_t vectorPuertos[3]={
		{GPIO_19,GPIO_OUTPUT},
		{GPIO_18,GPIO_OUTPUT},
		{GPIO_9,GPIO_OUTPUT},
};

	 for(uint8_t i=0; i<3; i++){
		GPIOInit(vectorPuertos[i].pin,vectorPuertos[i].dir);
	}

	uint32_t num = 200;
	uint8_t digitos = 3;

	mostrarPorDisplay(num, digitos, vectorPines, vectorPuertos);
}
/*==================[end of file]============================================*/