/* Deadline Driven Scheduler API */

#ifndef DEADLINE_DRIVEN_SCHEDULER_API_H_
#define DEADLINE_DRIVEN_SCHEDULER_API_H_

/* Includes */
#include <stdlib.h>
#include <string.h>
#include "../../../FreeRTOS_Source/include/FreeRTOS.h"
#include "../../../FreeRTOS_Source/include/task.h"
#include "../../../FreeRTOS_Source/include/queue.h"
#include "../../../FreeRTOS_Source/include/event_groups.h"
#include "../../tiny_printf.h"

/* Macros */
#define MAX_CONCURRENT_TASKS 1
#define MAX_TASKS 16
#define MAX_TASK_NAME_LENGTH 32
#define NUM_TASK_LISTS 3
#define CURRENT_TASK_COMPLETE_BIT ( 1 << 0 )
#define PRIORITY_LOW 1
#define PRIORITY_HIGH 4

/* Global variable declarations */
xQueueHandle xNewTasksQueueHandle;
xQueueHandle xTaskMessagesQueueHandle;
xQueueHandle xTaskRegenerationRequestsQueueHandle;
xQueueHandle xSchedulerMessagesQueueHandle;
EventGroupHandle_t xCurrentTaskCompleteEventGroup;

/* Type definitions */
typedef uint32_t DeadlineDrivenTaskId_t;

typedef struct DeadlineDrivenTask
{
    DeadlineDrivenTaskId_t xId;                /* Unique identifier for the task */
    char  cName[MAX_TASK_NAME_LENGTH];       /* Text name for the task */
    TaskHandle_t           xFTaskHandle;       /* Handle of the corresponding FreeRTOS task */
    TickType_t             xAbsoluteDeadline;  /* Time (in ticks) that the task must be completed by */
    TickType_t             xPeriod;            /* The task's period in ticks */
    TickType_t             xReleaseTime;       /* Time (in ticks) when the task is released to the DDS */
    TickType_t             xStartTime;         /* Time (in ticks) when the task was started */
    TickType_t             xCompletionTime;    /* Time (in ticks) that the task was completed */
} DeadlineDrivenTask_t;

typedef struct DeadlineDrivenTaskNode
{
    struct DeadlineDrivenTaskNode *pxNext;     /* Pointer to the next task in the list */
    DeadlineDrivenTask_t          xTask;       /* The task */
} DeadlineDrivenTaskNode_t;

typedef enum SchedulerMessageType
{
    REQUEST_COMPLETED_LIST,
    REQUEST_ACTIVE_LIST,
    REQUEST_OVERDUE_LIST,
    INCOMING_COMPLETED_LIST,
    INCOMING_ACTIVE_LIST,
    INCOMING_OVERDUE_LIST
} MessageType_t;

/* Function declarations */
DeadlineDrivenTaskNode_t *pxCreateTaskNode( DeadlineDrivenTask_t xTask );

void vAddTaskToList( DeadlineDrivenTaskNode_t **pxTaskListHead, DeadlineDrivenTask_t xNewTask );

void vDeleteTaskFromList( DeadlineDrivenTaskNode_t **pxTaskListHead, DeadlineDrivenTaskId_t xTaskId );

void vPrintDeadlineDrivenTaskInfo( DeadlineDrivenTask_t xTask );

void vPrintTaskList( DeadlineDrivenTaskNode_t *pxTaskListHead );

uint32_t ulCreateDeadlineDrivenTask( void (*vTaskFunction)( void * ),
                                     char cName[],
                                     TickType_t xAbsoluteDeadline,
                                     TickType_t xPeriod,
                                     TickType_t xReleaseTime
                                   );

uint32_t ulCreateDeadlineDrivenTaskMetadata( TaskHandle_t xFTaskHandle,
                                             char cName[],
                                             TickType_t xAbsoluteDeadline,
                                             TickType_t xPeriod,
                                             TickType_t xReleaseTime
                                           );

void vCompleteDeadlineDrivenTask();

BaseType_t xSchedulerMessageRequest( MessageType_t xRequestType );

#endif /* DEADLINE_DRIVEN_SCHEDULER_API_H_ */
