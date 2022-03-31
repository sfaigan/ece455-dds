/* Task Monitor */

#include "deadline_driven_task_monitor.h"

void vDeadlineDrivenTaskMonitor( void *pvParameters )
{
    while( 1 )
	{
        printf( "[Deadline Driven Task Monitor] My turn!\n" );
        /* Request data from scheduler */
        if( uxQueueMessagesWaiting( xTaskRegenerationRequestsQueueHandle ) == 0 )
        {
            if( xSchedulerMessageRequest( REQUEST_COMPLETED_LIST ) )
            {
                printf("[Deadline Driven Task Monitor] Requested completed list.\n");
            }
            else
            {
                printf("[Deadline Driven Task Monitor] Request for completion task failed!\n");
            }

            if( xSchedulerMessageRequest( REQUEST_ACTIVE_LIST ) )
            {
                printf("[Deadline Driven Task Monitor] Requested active list.\n");
            }
            else
            {
                printf("[Deadline Driven Task Monitor] Request for active task failed!\n");
            }

            if( xSchedulerMessageRequest( REQUEST_OVERDUE_LIST ) )
            {
                printf("[Deadline Driven Task Monitor] Requested overdue list.\n");
            }
            else
            {
                printf("[Deadline Driven Task Monitor] Request for overdue task failed!\n");
            }
        }
        vTaskDelay( 100 );
	}

}
