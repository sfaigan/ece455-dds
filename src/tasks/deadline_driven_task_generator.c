/* Deadline Driven Task Generator */

#include "deadline_driven_task_generator.h"

void vDeadlineDrivenTaskGenerator( void *pvParameters )
{
    /* Variables go here */
    
    while( 1 )
    {
        /*
            Receive from task regeneration requests queue
                For each task in task regeneration requests queue
                    Create copy
                    If completion time != 0 // regeneration of periodic task
                        Set release time to release time + period
                        Set absolute deadline to absolute deadline + period
                        Clear start time
                        Clear completion time
                    Send copy to queue
        */
        vTaskDelay(1000);
    }
}
