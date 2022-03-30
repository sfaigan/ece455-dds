/* Deadline Driven Scheduler */

#ifndef DEADLINE_DRIVEN_SCHEDULER_H_
#define DEADLINE_DRIVEN_SCHEDULER_H_

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/event_groups.h"
//#include "../FreeRTOS_Source/portable/MemMang/heap_4.c"

/* Macros */
#define MAX_CONCURRENT_TASKS 1
#define MAX_TASKS 16
#define MAX_TASK_NAME_LENGTH 32

#define PRIORITY_LOW 4
#define PRIORITY_HIGH 2

#define CURRENT_TASK_COMPLETE_BIT ( 1 << 0 )

/* Global variable declarations */
xQueueHandle xNewTasksQueueHandle;
xQueueHandle xTaskMessagesQueueHandle;
xQueueHandle xTaskRegenerationRequestsQueueHandle;
xQueueHandle xTaskMonitorQueueHandle;
EventGroupHandle_t xCurrentTaskCompleteEventGroup;

/* Type definitions */
typedef struct DeadlineDrivenTask
{
    uint32_t     ulId;                        /* Unique identifier for the task */
    //char         cName[MAX_TASK_NAME_LENGTH]; /* Text name for the task */
    TaskHandle_t xFTaskHandle;                /* Handle of the corresponding FreeRTOS task */
    TickType_t   xAbsoluteDeadline;           /* Time (in ticks) that the task must be completed by */
    TickType_t   xPeriod;                     /* The task's period in ticks */
    TickType_t   xReleaseTime;                /* Time (in ticks) when the task is released to the DDS */
    TickType_t   xStartTime;                  /* Time (in ticks) when the task was started */
    TickType_t   xCompletionTime;             /* Time (in ticks) that the task was completed */
} DeadlineDrivenTask_t;

typedef struct DeadlineDrivenTaskNode
{
    struct DeadlineDrivenTaskNode *pxNext; /* Pointer to the next task in the list */
    DeadlineDrivenTask_t          xTask;   /* The task */
} DeadlineDrivenTaskNode_t;

/* Function declarations */
void vDeadlineDrivenScheduler( void *pvParameters );

uint32_t ulCreateDeadlineDrivenTask( void (*vTaskFunction)( void * ),
                                     char cName[],
                                     TickType_t xAbsoluteDeadline,
                                     TickType_t xPeriod,
                                     TickType_t xReleaseTime
                                   );

void vDeleteDeadlineDrivenTask( uint32_t ulTaskId );
void vCompleteDeadlineDrivenTask();
//static DDTaskNode* xReturnActiveDeadlineDrivenTasks();
//static DDTaskNode* xReturnOverdueDeadlineDrivenTasks();
//static DDTaskNode* xReturnCompletedDeadlineDrivenTasks();

#endif /* DEADLINE_DRIVEN_SCHEDULER_H_ */
