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
xQueueHandle xTaskMonitorQueueHandle;

/* Type definitions */
typedef enum MessageType_t
{
    RequestCompletedList,
    RequestActiveList,
    RequestOverdueList,
    IncomingCompletedList,
    IncomingActiveList,
    IncomingOverdueList
}MessageType_t;

typedef struct StatusMessage_t
{
    MessageType_t MessageType;
    DeadlineDrivenTaskNode List;
} StatusMessage_t;

/* Function declarations */
xSchedulerMessageRequest(MessageType_t RequestType);

StatusMessage_t xCheckSchedulerMessage();

StatusMessage_t xGetSchedulerMessage();

#endif /* DEADLINE_DRIVEN_SCHEDULER_API_H_ */
