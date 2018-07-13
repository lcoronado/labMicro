

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


void configGPIO();

void mainMenu();

void contadorBinario();
void barrido();
void autoIncreible();

void test_in_out();
void test_var();

int main( void )
{
	configGPIO();
	//xTaskCreate( test_in_out, "in_out", 240, NULL, 1, NULL );
	//test_var();
	xTaskCreate( mainMenu, "menu", 240, NULL, 1, NULL );

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
	//GPIO_IntCmd(2,0x00000001,0);
	//GPIO_ClearInt(2,0x00000001);
}

void mainMenu(){
	uint32_t sw1 = 0x00;
	while(1){
		sw1 = GPIO_ReadValue(2);
		if (sw1 == 0x03FF9){
			contadorBinario();
		}
		else if(sw1 == 0x03FFA){
			barrido();
		}
		else if(sw1 == 0x03FFC){
			autoIncreible();
		}
		else{
			GPIO_SetValue(0,0xFF);
			vTaskDelay(100 / portTICK_RATE_MS);
			GPIO_ClearValue(0,0x000000FF);
		}
	}
}


void contadorBinario(){
	//uint32_t sw1 = 0x00000000;
	uint32_t var = 0;
	uint32_t sw1 = 0x00;

	while(1){
		GPIO_SetValue(0,var);
		var++;
		vTaskDelay(100 / portTICK_RATE_MS);
		GPIO_ClearValue(0,0x000000FF);

		sw1 = GPIO_ReadValue(2);
		if(sw1 != 0x3FF8){
			vTaskDelay(500 / portTICK_RATE_MS);
			break;
		}
	}
}

void barrido(){

	uint32_t var = 1;
	uint32_t sw1 = 0x00;

	while(1){

		GPIO_SetValue(0,var);
		var = var << 1;
		vTaskDelay(100 / portTICK_RATE_MS);
		GPIO_ClearValue(0,0x000000FF);

		sw1 = GPIO_ReadValue(2);
		if(sw1 != 0x3FF8){
			vTaskDelay(500 / portTICK_RATE_MS);
			break;
		}
		if (var > 0b10000000){
			var = 1;
		}
	}

}

void autoIncreible(){

	uint32_t var = 1;
	uint32_t sw1 = 0x00;
	int i;
	int up = 1;

	while(1){
		GPIO_SetValue(0,var);
		vTaskDelay(100 / portTICK_RATE_MS);
		GPIO_ClearValue(0,0x000000FF);

		if (var >= 0b10000000){
			up = 0;
		}
		if (var <= 0b00000001){
			up = 1;
		}


		if (up == 0){
			var = var >> 1;
		}
		else if(up == 1){
			var = var << 1;
		}
		else{
			var = 1;
		}

		sw1 = GPIO_ReadValue(2);
		if(sw1 != 0x3FF8){
			vTaskDelay(500 / portTICK_RATE_MS);
			break;
		}
	}
}


/* --------------------- Tests --------------------- */
void test_in_out(){
	uint32_t sw1 = 0x00000000;

	while(1){
		sw1 = GPIO_ReadValue(2);
		if(sw1 == 0x00003FFE){
			GPIO_SetValue(0,sw1); 	// Asignar valores dependiendo de la variable
			//vTaskDelay(100/portTICK_RATE_MS); 	// Retardo de 100 ms
			//GPIO_ClearValue(0,0x000000FF);
		}
		//GPIO_SetValue(0,0x000000FF);
		vTaskDelay(100/portTICK_RATE_MS); 	// Retardo de 100 ms
		GPIO_ClearValue(0,0x000000FF);
	}
	//uint32 sw1 = 0x00000000;
}

void test_var(){
	uint32_t sw1 = 0x00000000;
	GPIO_SetValue(0,0x000000FF);
	while(1){
		sw1 = GPIO_ReadValue(2);
		if (sw1 == 0x3FF8){
			GPIO_SetValue(0,0x00000001);
		}

		if (sw1 == 0x3FFF){
			GPIO_SetValue(0,0x00000002);
		}
		//GPIO_SetValue(0,0x000000FF);
		GPIO_ClearValue(0,0x000000FF);
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

