/* Deadline Driven Scheduler */

#ifndef DEADLINE_DRIVEN_SCHEDULER_H_
#define DEADLINE_DRIVEN_SCHEDULER_H_

/* Includes */
#include <stdio.h>
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/event_groups.h"

/* Macros */
#define MAX_CONCURRENT_TASKS 1
#define MAX_TASKS 16

/* Global variable declarations */
xQueueHandle xNewTasksQueueHandle;
xQueueHandle xTaskMessagesQueueHandle;
xQueueHandle xTaskRegenerationRequestsQueueHandle;
xQueueHandle xTaskMonitorQueueHandle;
EventGroupHandle_t xCurrentTaskCompleteEventGroup;

/* Type definitions */
typedef struct DDTask
{
    uint8_t      cTaskId;            /* Unique identifier for the task */
    TaskHandle_t xTaskHandle;        /* Handle of the corresponding FreeRTOS task */
    TickType_t   xAbsoluteDeadline;  /* Time (in ticks) that the task must be completed by */
    TickType_t   xPeriod;            /* The task's period in ticks */
    TickType_t   xReleaseTime;       /* Time (in ticks) when the task is released to the DDS */
    TickType_t   xStartTime;         /* Time (in ticks) when the task was started */
    TickType_t   xCompletionTime;    /* Time (in ticks) that the task was completed */ 
} DDTask;

typedef struct DDTaskNode
{
    DDTaskNode   *pxNext;            /* Pointer to the next task in the list */
    DDTask       xDDTask             /* The task */
} DDTaskNode;

/* Function declarations */
void vDeadlineDrivenScheduler( void *pvParameters );

#endif /* DEADLINE_DRIVEN_SCHEDULER_H_ */
