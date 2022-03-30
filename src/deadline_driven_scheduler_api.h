/* Deadline Driven Scheduler API */

#ifndef DEADLINE_DRIVEN_SCHEDULER_API_H_
#define DEADLINE_DRIVEN_SCHEDULER_API_H_

/* Includes */
#include <stdio.h>
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/task.h"
#include "../FreeRTOS_Source/include/queue.h"
#include "../FreeRTOS_Source/include/event_groups.h"
#include "deadline_driven_scheduler.h"

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

typedef enum MessageType
{
    REQUEST_COMPLETED_LIST,
    REQUEST_ACTIVE_LIST,
    REQUEST_OVERDUE_LIST,
    INCOMING_COMPLETED_LIST,
    INCOMING_ACTIVE_LIST,
    INCOMING_OVERDUE_LIST
} MessageType_t;

typedef struct SchedulerMessage
{
    MessageType_t            xMessageType;
    DeadlineDrivenTaskNode_t *xListHead;
} SchedulerMessage_t;

/* Function declarations */
BaseType_t xSchedulerMessageRequest(MessageType_t xRequestType);

StatusMessage_t xCheckSchedulerMessage();

StatusMessage_t xGetSchedulerMessage();

#endif /* DEADLINE_DRIVEN_SCHEDULER_API_H_ */
