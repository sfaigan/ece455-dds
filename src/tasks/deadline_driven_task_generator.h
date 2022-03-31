/* Deadline Driven Task Generator */

#ifndef DEADLINE_DRIVEN_TASK_GENERATOR_H_
#define DEADLINE_DRIVEN_TASK_GENERATOR_H_

/* Includes */
#include "../../FreeRTOS_Source/include/FreeRTOS.h"
#include "../../FreeRTOS_Source/include/task.h"
#include "scheduler/deadline_driven_scheduler_api.h"
#include "../tiny_printf.h"

/* Function declarations */
void vDeadlineDrivenTaskGenerator( void *pvParameters );

#endif /* DEADLINE_DRIVEN_TASK_GENERATOR_H_ */
