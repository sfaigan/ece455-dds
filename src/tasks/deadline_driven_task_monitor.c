/* Task Monitor */

#include "deadline_driven_task_monitor.h"

void vDeadlineDrivenTaskMonitor( void *pvParameters )
{
    while( 1 )
	{
        /* Request data from scheduler */
        if( uxQueueMessagesWaiting( xTaskRegenerationRequestsQueueHandle ) == 0 )
        {
            if( xSchedulerMessageRequest( REQUEST_COMPLETED_LIST ) != pdPASS )
            {
                printf("[Deadline Driven Task Monitor] Request for completed tasks failed.\n");
            }

            if( xSchedulerMessageRequest( REQUEST_ACTIVE_LIST ) != pdPASS )
            {
                printf("[Deadline Driven Task Monitor] Request for active tasks failed.\n");
            }

            if( xSchedulerMessageRequest( REQUEST_OVERDUE_LIST ) != pdPASS )
            {
                printf("[Deadline Driven Task Monitor] Request for overdue tasks failed!\n");
            }
        }
        vTaskDelay( 100 );
	}

}
