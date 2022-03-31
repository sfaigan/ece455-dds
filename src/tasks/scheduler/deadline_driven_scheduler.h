/* Deadline Driven Scheduler */

#ifndef DEADLINE_DRIVEN_SCHEDULER_H_
#define DEADLINE_DRIVEN_SCHEDULER_H_

/* Includes */
#include <stdlib.h>
#include <time.h>
#include "../../../FreeRTOS_Source/include/FreeRTOS.h"
#include "../../../FreeRTOS_Source/include/task.h"
#include "../../../FreeRTOS_Source/include/queue.h"
#include "../../../FreeRTOS_Source/include/event_groups.h"
#include "deadline_driven_scheduler_api.h"
#include "../../tiny_printf.h"

/* Function declarations */
void vDeadlineDrivenScheduler( void *pvParameters );

#endif /* DEADLINE_DRIVEN_SCHEDULER_H_ */
