/* Deadline Driven Task Generator */

#ifndef DEADLINE_DRIVEN_TASK_GENERATOR_H_
#define DEADLINE_DRIVEN_TASK_GENERATOR_H_

/* Includes */
#include <stdio.h>
#include "../FreeRTOS_Source/include/FreeRTOS.h"
#include "../FreeRTOS_Source/include/task.h"

/* Macros */

/* Function declarations */
void vDeadlineDrivenTaskGenerator( void *pvParameters );

#endif /* DEADLINE_DRIVEN_TASK_GENERATOR_H_ */
