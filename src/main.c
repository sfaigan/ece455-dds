/*
    FreeRTOS V9.0.0 - Copyright (C) 2016 Real Time Engineers Ltd.
    All rights reserved

    VISIT http://www.FreeRTOS.org TO ENSURE YOU ARE USING THE LATEST VERSION.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation >>>> AND MODIFIED BY <<<< the FreeRTOS exception.

    ***************************************************************************
    >>!   NOTE: The modification to the GPL is included to allow you to     !<<
    >>!   distribute a combined work that includes FreeRTOS without being   !<<
    >>!   obliged to provide the source code for proprietary components     !<<
    >>!   outside of the FreeRTOS kernel.                                   !<<
    ***************************************************************************

    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT ANY
    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
    FOR A PARTICULAR PURPOSE.  Full license text is available on the following
    link: http://www.freertos.org/a00114.html

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS provides completely free yet professionally developed,    *
     *    robust, strictly quality controlled, supported, and cross          *
     *    platform software that is more than just the market leader, it     *
     *    is the industry's de facto standard.                               *
     *                                                                       *
     *    Help yourself get started quickly while simultaneously helping     *
     *    to support the FreeRTOS project by purchasing a FreeRTOS           *
     *    tutorial book, reference manual, or both:                          *
     *    http://www.FreeRTOS.org/Documentation                              *
     *                                                                       *
    ***************************************************************************

    http://www.FreeRTOS.org/FAQHelp.html - Having a problem?  Start by reading
    the FAQ page "My application does not run, what could be wwrong?".  Have you
    defined configASSERT()?

    http://www.FreeRTOS.org/support - In return for receiving this top quality
    embedded software for free we request you assist our global community by
    participating in the support forum.

    http://www.FreeRTOS.org/training - Investing in training allows your team to
    be as productive as possible as early as possible.  Now you can receive
    FreeRTOS training directly from Richard Barry, CEO of Real Time Engineers
    Ltd, and the world's leading authority on the world's leading RTOS.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool, a DOS
    compatible FAT file system, and our tiny thread aware UDP/IP stack.

    http://www.FreeRTOS.org/labs - Where new FreeRTOS products go to incubate.
    Come and try FreeRTOS+TCP, our new open source TCP/IP stack for FreeRTOS.

    http://www.OpenRTOS.com - Real Time Engineers ltd. license FreeRTOS to High
    Integrity Systems ltd. to sell under the OpenRTOS brand.  Low cost OpenRTOS
    licenses offer ticketed support, indemnification and commercial middleware.

    http://www.SafeRTOS.com - High Integrity Systems also provide a safety
    engineered and independently SIL3 certified version for use in safety and
    mission critical applications that require provable dependability.

    1 tab == 4 spaces!
*/

/* Standard includes. */
#include <stdint.h>
#include <time.h>
#include "stm32f4_discovery.h"
/* Kernel includes. */
#include "stm32f4xx.h"
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/semphr.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/timers.h"
/* User includes */
#include "tasks/scheduler/deadline_driven_scheduler.h"
#include "tasks/scheduler/deadline_driven_scheduler_api.h"
#include "tasks/deadline_driven_task_generator.h"
#include "tasks/deadline_driven_task_monitor.h"
#include "tiny_printf.h"

static void prvSetupHardware( void );
/*-----------------------------------------------------------*/

/*
 *  void vSampleTask( void *pvParameters )
 *  {
 *      uint8_t ucTaskNumber = ( uint8_t ) pvParameters;
 *      // Declare any other local variables here
 *
 *      while( 1 )
 *      {
 *          if( ucGetNthEventBit( ucTaskNumber ) )
            {
                ucClearNthEventBit( ucTaskNumber );
                // Reset local variables here
            }
            // Insert arbitrary code here
            vCompleteDeadlineDrivenTask()
 *      }
 *  }
 *
 */

void vTestTask1( void *pvParameters )
{
    TickType_t xStartTime;
    uint8_t ucTaskNumber = ( uint8_t ) pvParameters;
    while( 1 )
    {
        xStartTime = xTaskGetTickCount();
        while( xTaskGetTickCount() - xStartTime < 100 ) {
            if( ucGetNthEventBit( ucTaskNumber ) )
            {
                xStartTime = xTaskGetTickCount();
                ucClearNthEventBit( ucTaskNumber );
            }
        }
        vCompleteDeadlineDrivenTask();
    }
}

void vTestTask2( void *pvParameters )
{
    uint8_t ucTaskNumber = ( uint8_t ) pvParameters;
    TickType_t xStartTime;
    while( 1 )
    {
        xStartTime = xTaskGetTickCount();
        while( xTaskGetTickCount() - xStartTime < 200 ) {
            if( ucGetNthEventBit( ucTaskNumber ) )
            {
                ucClearNthEventBit( ucTaskNumber );
                xStartTime = xTaskGetTickCount();
            }
        }
        vCompleteDeadlineDrivenTask();
    }
}

void vTestTask3( void *pvParameters )
{
    uint8_t ucTaskNumber = ( uint8_t ) pvParameters;
    TickType_t xStartTime;
    while( 1 )
    {
        xStartTime = xTaskGetTickCount();
        while( xTaskGetTickCount() - xStartTime < 200 ) {
            if( ucGetNthEventBit( ucTaskNumber ) )
            {
                ucClearNthEventBit( ucTaskNumber );
                xStartTime = xTaskGetTickCount();
            }
        }
        vCompleteDeadlineDrivenTask();
    }
}

int main( void )
{

	/* Configure the system ready to run the demo.  The clock configuration
	can be done here if it was not done before main() was called. */
	prvSetupHardware();

	/* Seed rand() for generating task IDs */
	srand(time(NULL));

    /* Create queues */
    xNewTasksQueueHandle = xQueueCreate( MAX_TASKS, sizeof( DeadlineDrivenTask_t ) );
    xTaskMessagesQueueHandle = xQueueCreate( MAX_CONCURRENT_TASKS, sizeof( TickType_t ) );
    xTaskRegenerationRequestsQueueHandle = xQueueCreate( MAX_TASKS, sizeof( DeadlineDrivenTask_t ) );
    xSchedulerMessagesQueueHandle = xQueueCreate( NUM_TASK_LISTS, sizeof( MessageType_t ) );

    /* Add queues to the registry, for the benefit of kernel aware debugging */
    vQueueAddToRegistry( xNewTasksQueueHandle, "New Tasks" );
    vQueueAddToRegistry( xTaskMessagesQueueHandle, "Task Messages" );
    vQueueAddToRegistry( xTaskRegenerationRequestsQueueHandle, "Task Regeneration Requests" );
    vQueueAddToRegistry( xSchedulerMessagesQueueHandle, "Scheduler (Monitor) Messages" );

    /* Create tasks */
    xTaskCreate( vDeadlineDrivenScheduler, "DDS", configMINIMAL_STACK_SIZE, NULL, 5, NULL );
    xTaskCreate( vDeadlineDrivenTaskGenerator, "Generator", configMINIMAL_STACK_SIZE, NULL, 3, NULL );
    xTaskCreate( vDeadlineDrivenTaskMonitor, "Monitor", configMINIMAL_STACK_SIZE, NULL, 2, NULL );

    xTaskEventGroup = xEventGroupCreate();
    if( xTaskEventGroup )
    {
        ucSetNthEventBit( CURRENT_TASK_COMPLETE_BIT );
    }
    else
    {
        printf("Failed to create event group for task completion.\n");
        return EXIT_FAILURE;
    }

    ulCreateDeadlineDrivenTask( vTestTask1, "Task 1", 500, 500, 0 );
    ulCreateDeadlineDrivenTask( vTestTask2, "Task 2", 500, 500, 0 );
    ulCreateDeadlineDrivenTask( vTestTask3, "Task 3", 500, 500, 0 );

	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	return EXIT_SUCCESS;
}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* The malloc failed hook is enabled by setting
	configUSE_MALLOC_FAILED_HOOK to 1 in FreeRTOSConfig.h.

	Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software 
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( xTaskHandle pxTask, signed char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configconfigCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected.  pxCurrentTCB can be
	inspected in the debugger if the task name passed into this function is
	corrupt. */
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
volatile size_t xFreeStackSpace;

	/* The idle task hook is enabled by setting configUSE_IDLE_HOOK to 1 in
	FreeRTOSConfig.h.

	This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amount of FreeRTOS heap that
	remains unallocated. */
	xFreeStackSpace = xPortGetFreeHeapSize();

	if( xFreeStackSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	/* Ensure all priority bits are assigned as preemption priority bits.
	http://www.freertos.org/RTOS-Cortex-M3-M4.html */
	NVIC_SetPriorityGrouping( 0 );

	/* TODO: Setup the clocks, etc. here, if they were not configured before
	main() was called. */
}

