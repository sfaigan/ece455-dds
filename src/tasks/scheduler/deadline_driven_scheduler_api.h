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
#define CURRENT_TASK_COMPLETE_BIT 0
#define EVENT_BIT( N ) ( 1 << N )
#define PRIORITY_LOW 1
#define PRIORITY_HIGH 4

/* Global variable declarations */
xQueueHandle xNewTasksQueueHandle;
xQueueHandle xTaskMessagesQueueHandle;
xQueueHandle xTaskRegenerationRequestsQueueHandle;
xQueueHandle xSchedulerMessagesQueueHandle;
EventGroupHandle_t xTaskEventGroup;

/* Type definitions */
typedef uint32_t DeadlineDrivenTaskId_t;

typedef struct DeadlineDrivenTask
{
    DeadlineDrivenTaskId_t xId;                             /* Unique identifier for the task */
    uint8_t                ucTaskNumber;                    /* Task number */
    char                   cName[MAX_TASK_NAME_LENGTH];     /* Text name for the task */
    TaskHandle_t           xFTaskHandle;                    /* Handle of the corresponding FreeRTOS task */
    TickType_t             xAbsoluteDeadline;               /* Time (in ticks) that the task must be completed by */
    TickType_t             xPeriod;                         /* The task's period in ticks */
    TickType_t             xReleaseTime;                    /* Time (in ticks) when the task is released to the DDS */
    TickType_t             xStartTime;                      /* Time (in ticks) when the task was started */
    TickType_t             xCompletionTime;                 /* Time (in ticks) that the task was completed */
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

/* Returns a new task linked list node */
DeadlineDrivenTaskNode_t *pxCreateTaskNode( DeadlineDrivenTask_t xTask );

/* Creates a node for the provided task and appends it to the provided list */
void vAddTaskToList( DeadlineDrivenTaskNode_t **pxTaskListHead, DeadlineDrivenTask_t xNewTask );

/* Delete a task node from a list given its unique task ID */
void vDeleteTaskFromList( DeadlineDrivenTaskNode_t **pxTaskListHead, DeadlineDrivenTaskId_t xTaskId );

/* Pretty print a task's metadata */
void vPrintDeadlineDrivenTaskInfo( DeadlineDrivenTask_t xTask );

/* Pretty print all the tasks (metadata) in the provided list */
void vPrintTaskList( DeadlineDrivenTaskNode_t *pxTaskListHead );

/* Create a new deadline driven task (with an F-task) and return the unique ID */
uint32_t ulCreateDeadlineDrivenTask( void (*vTaskFunction)( void * ),
                                     char cName[],
                                     TickType_t xAbsoluteDeadline,
                                     TickType_t xPeriod,
                                     TickType_t xReleaseTime
                                   );

/* Create a DD-task to store the metadata for a deadline driven task */
uint32_t ulCreateDeadlineDrivenTaskMetadata( TaskHandle_t xFTaskHandle,
                                             char cName[],
                                             uint8_t ucTaskNumber,
                                             TickType_t xAbsoluteDeadline,
                                             TickType_t xPeriod,
                                             TickType_t xReleaseTime
                                           );

/* Sends the completion time of the task to the scheduler and mark the task as completed */
void vCompleteDeadlineDrivenTask();

/* Request a task list (active, completed, overdue) from the scheduler */
BaseType_t xSchedulerMessageRequest( MessageType_t xRequestType );

/* Abstracted functions to access/manipulate event bits */
uint8_t ucGetNthEventBit( uint8_t ucN );
uint8_t ucSetNthEventBit( uint8_t ucN );
uint8_t ucClearNthEventBit( uint8_t ucN );

#endif /* DEADLINE_DRIVEN_SCHEDULER_API_H_ */
