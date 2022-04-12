/* Task Monitor */

#ifndef TASK_MONITOR_H_
#define TASK_MONITOR_H_

/* Includes */
#include "../../FreeRTOS_Source/include/FreeRTOS.h"
#include "../../FreeRTOS_Source/include/queue.h"
#include "../../FreeRTOS_Source/include/event_groups.h"
#include "scheduler/deadline_driven_scheduler_api.h"
#include "../tiny_printf.h"

/* 
 * Deadline Driven Task Monitor
 * Regularly requests the active, overdue, and completed task lists.
 * Contents of each list are printed to stdout.
 */
void vDeadlineDrivenTaskMonitor( void *pvParameters );

#endif /* TASK_MONITOR_H_ */
