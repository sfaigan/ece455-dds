/* Task Monitor */

#ifndef TASK_MONITOR_H_
#define TASK_MONITOR_H_

/* Includes */
#include <stdio.h>
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/event_groups.h"
#include "scheduler/deadline_driven_scheduler.h"
#include "scheduler/deadline_driven_scheduler_api.h"

/* Global variable declarations */
xQueueHandle xTaskMonitorQueueHandle;

/* Function declarations */
void vDeadlineDrivenTaskMonitor( void *pvParameters );

void vPrintList( DeadlineDrivenTaskNode_t *pxTaskListHead );

#endif /* TASK_MONITOR_H_ */
