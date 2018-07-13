
// -------------- Librerias utilizadas -------------- //
/* Standard includes. */
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include <NXP/crp.h>
#include "lpc17xx_gpio.h"
#include "lpc17xx_timer.h"

/* Library includes. */
#include "LPC17xx.h"

/* Used in the run time stats calculations. */
static uint32_t ulClocksPer10thOfAMilliSecond = 0UL;

// -------------- prototipo de funcion -------------- //
/* The task functions. */
void contadorBinario();

// -------------- funcion principal -------------- //
int main( void )
{
	// Se crea la tarea para ejecutar el contador binario
	xTaskCreate( contadorBinario, "Task", 240, NULL, 1, NULL );

	/* Start the FreeRTOS scheduler. */
	vTaskStartScheduler();

	/* The following line should never execute.  If it does, it means there was
	insufficient FreeRTOS heap memory available to create the Idle and/or timer
	tasks.  See the memory management section on the http://www.FreeRTOS.org web
	site for more information. */
	for( ;; );
}
/*-----------------------------------------------------------*/

// -------------- Definir funciones -------------- //
void contadorBinario(){

	// Definir variable de conteo
	uint32_t var = 0;

	// Declarar salidas
	GPIO_SetDir(0,0x000000FF,1);

	// loop infinito
	while(1){

		GPIO_SetValue(0,var); 	// Asignar valores dependiendo de la variable
		//Timer0_Wait(100); funcion aux de conteo (no compila)
		var++; 	// Incrementa el valor de la variable

		vTaskDelay(100/portTICK_RATE_MS); 	// Retardo de 100 ms
		GPIO_ClearValue(0,0x000000FF); 		// Limpia los valores a la salida
	}
}
