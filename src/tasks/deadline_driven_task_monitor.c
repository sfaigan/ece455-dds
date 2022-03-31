/* Task Monitor */

#include "deadline_driven_task_monitor.h"

void vDeadlineDrivenTaskMonitor( void *pvParameters )
{
    /*
    SchedulerMessage_t xSchedulerMessage;
    uint8_t ucCounter1, ucCounter2;
    */

    while( 1 )
	{
        printf( "[Deadline Driven Task Monitor] My turn!\n" );
        /*
        for ( ucCounter1 = 0; ucCounter1 < NUM_TASK_LISTS; ucCounter1++ )
        {
            if ( xQueueReceive( xSchedulerMessagesQueueHandle, &xSchedulerMessage, 1000 ) )
            {
                switch( xSchedulerMessage.xMessageType )
                {
                    case INCOMING_COMPLETED_LIST:
                        printf( "[Deadline Driven Task Monitor] Completed List:\n" );
                        break;
                    case INCOMING_ACTIVE_LIST:
                        printf( "[Deadline Driven Task Monitor] Active List:\n" );
                        break;
                    case INCOMING_OVERDUE_LIST:
                        printf( "[Deadline Driven Task Monitor] Overdue List:\n" );
                        break;
                    default:
                        printf( "[Deadline Driven Task Monitor] This was unexpected...\n" );
                }

                for ( ucCounter2 = 0; ucCounter2 < xSchedulerMessage.ucNumTasks; ucCounter2++ )
                {
                    vPrintDeadlineDrivenTaskInfo( xSchedulerMessage.xTasks[ucCounter2] );
                }
            }
            else
            {
                printf( "[Deadline Driven Task Monitor] Failed to receive a list from queue." );
            }
        }
        */

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
