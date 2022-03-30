/* Deadline Driven Task */

#include "deadline_driven_task.h"

void vDeadlineDrivenTask( void *pvParameters )
{
    /* Variables go here */
    
    while( 1 )
    {
        /*
            Count
        */
        vTaskDelay(1000);
    }
}
