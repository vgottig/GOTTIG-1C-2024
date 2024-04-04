/*! @mainpage Proyecto 1. Ejercicio 6
 *
 * @section genDesc General Description
 *
 * Se realiza una función que recibe un dato de 32 bits, la cantidad de dígitos
 * de salida y dos vectores de estructuras del tipo gpioConf_t. Uno  de estos vectores
 * mapea los bits y el otro los puertos con el dígito del LCD a donde mostrar un dato.
 * La función muestra por display el valor que recibe.
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
#include "gpio_mcu.h"
/*==================[macros and definitions]=================================*/

/*==================[internal data definition]===============================*/

/** Struct que almacena el numero de pin del GPIO y la dirección del mismo.
*/
typedef struct
{
	gpio_t pin; /*!< GPIO pin number */
	io_t dir; /*!< GPIO direction '0' IN; '1' OUT*/
} gpioConf_t;


/*==================[internal functions declaration]=========================*/

/*==================[external functions definition]==========================*/

/** @fn void  convertToBcdArray (uint32_t data, uint8_t digits, uint8_t * bcd_number);
 *  @brief Convierte el dato recibido a BCD y guarda cada uno de los dígitos de salida en
 *         el arreglo pasado como puntero.
 *  @param[in] uint32_t data, uint8_t digits, uint8_t * bcd_number
 *  @return */
int8_t convertToBcdArray (uint32_t data, uint8_t digits, uint8_t * bcd_number) 
{
	while(digits>0){
		bcd_number[digits-1]=data%10; //123%10=3 ... 12%10=2 ...
		data=data/10; //123/10=12 ... 12/10=1
		digits--;
	}
	return true;
}


/** @fn void functionBCD(uint8_t digit, gpioConf_t *vector);
 *  @brief Cambia el estado de cada GPIO, a ‘0’ o a ‘1’, según el estado del bit
 *         correspondiente en el BCD ingresado.
 *  @param[in] uint8_t digit, gpioConf_t *vector
 *  @return 
*/
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


/** @fn void mostrarPorDisplay(uint32_t dato, uint8_t digitos, gpioConf_t *vectorPines, gpioConf_t *vectorPuertos);
 *  @brief Muestra por display el valor que recibe.
 *  @param[in] uint32_t dato, uint8_t digitos, gpioConf_t *vectorPines, gpioConf_t *vectorPuertos
 *  @return 
*/
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

/** Arreglo que mapea los GPIO de los bits.
*/
	gpioConf_t vectorPines[4]={
		{GPIO_20,GPIO_OUTPUT},
		{GPIO_21,GPIO_OUTPUT},
		{GPIO_22,GPIO_OUTPUT},
		{GPIO_23,GPIO_OUTPUT}
	};

	for(uint8_t i=0; i<4; i++){
		GPIOInit(vectorPines[i].pin,vectorPines[i].dir);
	}

/** Arreglo que mapea los GPIO de los puertos.
*/
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