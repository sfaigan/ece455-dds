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
#include "deadline_driven_scheduler_api.h"

/* Macros */
#define MAX_CONCURRENT_TASKS 1
#define MAX_TASKS 16
#define MAX_TASK_NAME_LENGTH 32

#define PRIORITY_LOW 4
#define PRIORITY_HIGH 2

/* Function declarations */
void vDeadlineDrivenScheduler( void *pvParameters );

#endif /* DEADLINE_DRIVEN_SCHEDULER_H_ */
