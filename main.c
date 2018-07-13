

/* Standard includes. */
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "lpc17xx_gpio.h"


/* Library includes. */
#include "LPC17xx.h"



/* Used in the run time stats calculations. */
static uint32_t ulClocksPer10thOfAMilliSecond = 0UL;

#define ZERO 0X3F
#define UNO 0X06
#define DOS 0x5B
#define TRES 0x4F
#define CUATRO 0X66
#define CINCO 0X6D
#define SEIS 0X7D
#define SIETE 0X47
#define OCHO 0X7F
#define NUEVE 0X67
#define ERROR 0x79

uint32_t numeros[11] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x79};

void configGPIO();
void display();
void show(int var);

int main( void )
{
	configGPIO();
	xTaskCreate( display, "display", 240, NULL, 1, NULL );


	/* Start the FreeRTOS scheduler. */
	vTaskStartScheduler();

	/* The following line should never execute.  If it does, it means there was
	insufficient FreeRTOS heap memory available to create the Idle and/or timer
	tasks.  See the memory management section on the http://www.FreeRTOS.org web
	site for more information. */
	for( ;; );
}

void configGPIO(){
	// Declarar salidas puerto 0
	GPIO_SetDir(0,0x000000FF,1);
	GPIO_ClearValue(0,0x000000FF);

	// Declarar entradas puerto 2
	GPIO_SetDir(2,0x00000001,0);
	GPIO_ClearValue(2,0x00000001);
	GPIO_SetValue(0,ZERO);
}

void display(){

	uint32_t sw1 = 0x00;
	int varConteo = 0;

	while(1){
		sw1 = GPIO_ReadValue(2);
		if (sw1 == 0x03FF9){
			varConteo = varConteo + 1;
		}
		if (sw1 == 0x03FFA){
			varConteo = varConteo - 1;
		}
		show(varConteo);
		vTaskDelay(100 / portTICK_RATE_MS);
	}
}

void show(int var){
	switch(var){

		case 0:
			GPIO_ClearValue(0,0x000000FF);
			GPIO_SetValue(0,ZERO);
			vTaskDelay(100 / portTICK_RATE_MS);
			break;

		case 1:
			GPIO_ClearValue(0,0x000000FF);
			GPIO_SetValue(0,UNO);
			vTaskDelay(100 / portTICK_RATE_MS);
			break;

		case 2:
			GPIO_ClearValue(0,0x000000FF);
			GPIO_SetValue(0,DOS);
			vTaskDelay(100 / portTICK_RATE_MS);
			break;

		case 3:
			GPIO_ClearValue(0,0x000000FF);
			GPIO_SetValue(0,TRES);
			vTaskDelay(100 / portTICK_RATE_MS);
			break;

		case 4:
			GPIO_ClearValue(0,0x000000FF);
			GPIO_SetValue(0,CUATRO);
			vTaskDelay(100 / portTICK_RATE_MS);
			break;

		case 5:
			GPIO_ClearValue(0,0x000000FF);
			GPIO_SetValue(0,CINCO);
			vTaskDelay(100 / portTICK_RATE_MS);
			break;

		case 6:
			GPIO_ClearValue(0,0x000000FF);
			GPIO_SetValue(0,SEIS);
			vTaskDelay(100 / portTICK_RATE_MS);
			break;

		case 7:
			GPIO_ClearValue(0,0x000000FF);
			GPIO_SetValue(0,SIETE);
			vTaskDelay(100 / portTICK_RATE_MS);
			break;

		case 8:
			GPIO_ClearValue(0,0x000000FF);
			GPIO_SetValue(0,OCHO);
			vTaskDelay(100 / portTICK_RATE_MS);
			break;

		case 9:
			GPIO_ClearValue(0,0x000000FF);
			GPIO_SetValue(0,NUEVE);
			vTaskDelay(100 / portTICK_RATE_MS);
			break;

		default:
			GPIO_ClearValue(0,0x000000FF);
			GPIO_SetValue(0,ERROR);
			vTaskDelay(100 / portTICK_RATE_MS);
			break;
	}
}

/*-----------------------------------------------------------*/





















void vMainConfigureTimerForRunTimeStats( void )
{
	/* How many clocks are there per tenth of a millisecond? */
	ulClocksPer10thOfAMilliSecond = configCPU_CLOCK_HZ / 10000UL;
}
/*-----------------------------------------------------------*/

uint32_t ulMainGetRunTimeCounterValue( void )
{
uint32_t ulSysTickCounts, ulTickCount, ulReturn;
const uint32_t ulSysTickReloadValue = ( configCPU_CLOCK_HZ / configTICK_RATE_HZ ) - 1UL;
volatile uint32_t * const pulCurrentSysTickCount = ( ( volatile uint32_t *) 0xe000e018 );
volatile uint32_t * const pulInterruptCTRLState = ( ( volatile uint32_t *) 0xe000ed04 );
const uint32_t ulSysTickPendingBit = 0x04000000UL;

	/* NOTE: There are potentially race conditions here.  However, it is used
	anyway to keep the examples simple, and to avoid reliance on a separate
	timer peripheral. */


	/* The SysTick is a down counter.  How many clocks have passed since it was
	last reloaded? */
	ulSysTickCounts = ulSysTickReloadValue - *pulCurrentSysTickCount;

	/* How many times has it overflowed? */
	ulTickCount = xTaskGetTickCountFromISR();

	/* Is there a SysTick interrupt pending? */
	if( ( *pulInterruptCTRLState & ulSysTickPendingBit ) != 0UL )
	{
		/* There is a SysTick interrupt pending, so the SysTick has overflowed
		but the tick count not yet incremented. */
		ulTickCount++;

		/* Read the SysTick again, as the overflow might have occurred since
		it was read last. */
		ulSysTickCounts = ulSysTickReloadValue - *pulCurrentSysTickCount;
	}

	/* Convert the tick count into tenths of a millisecond.  THIS ASSUMES
	configTICK_RATE_HZ is 1000! */
	ulReturn = ( ulTickCount * 10UL ) ;

	/* Add on the number of tenths of a millisecond that have passed since the
	tick count last got updated. */
	ulReturn += ( ulSysTickCounts / ulClocksPer10thOfAMilliSecond );

	return ulReturn;
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

