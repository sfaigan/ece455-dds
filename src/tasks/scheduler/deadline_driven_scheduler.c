/* Deadline Driven Scheduler */

#include "deadline_driven_scheduler.h"

void vDeadlineDrivenScheduler( void *pvParameters )
{
    DeadlineDrivenTaskNode_t *xActiveTasksHead = NULL;
    DeadlineDrivenTaskNode_t *xOverdueTasksHead = NULL;
    DeadlineDrivenTaskNode_t *xCompletedTasksHead = NULL;
    DeadlineDrivenTaskNode_t *pxCursorNode = NULL;
    DeadlineDrivenTaskNode_t *pxEarliestDeadlineTaskNode = NULL;
    DeadlineDrivenTask_t xNewTask;
    TickType_t xEarliestDeadline = 0;
    TickType_t xStartTime = 0;
    TickType_t xCompletionTime = 0;
    MessageType_t xSchedulerMessage;
    EventBits_t uxBits;

    /*
    MessageType_t xSchedulerMessageType;
    SchedulerMessage_t xSchedulerMessage;
    DeadlineDrivenTask_t xTaskList[MAX_TASKS];
    uint8_t ucCounter;
    uint8_t ucNumTasks;
    */
    
    while( 1 )
    {
        printf( "[Deadline Driven Scheduler] My turn!\n" );
        while( xQueueReceive( xNewTasksQueueHandle, &xNewTask, 0 ) )
        {
            if( xNewTask.xAbsoluteDeadline <= xTaskGetTickCount() )
            {
                vAddTaskToList( &xOverdueTasksHead, xNewTask );
            }
            else
            {
                vAddTaskToList( &xActiveTasksHead, xNewTask );
            }
        }

        if ( pxEarliestDeadlineTaskNode != NULL )
        {
            xCompletionTime = 0;
            if( xQueueReceive( xTaskMessagesQueueHandle, &xCompletionTime, 0 ) )
            {
                pxEarliestDeadlineTaskNode->xTask.xCompletionTime = xCompletionTime;
                vAddTaskToList( &xCompletedTasksHead, pxEarliestDeadlineTaskNode->xTask );
                vDeleteTaskFromList( &xActiveTasksHead, pxEarliestDeadlineTaskNode->xTask.xId );
            }
            else if( xEventGroupGetBits( xCurrentTaskCompleteEventGroup ) == 0 )
            {
                vAddTaskToList( &xOverdueTasksHead, pxEarliestDeadlineTaskNode->xTask );
                vDeleteTaskFromList( &xActiveTasksHead, pxEarliestDeadlineTaskNode->xTask.xId );
            }

            /* If periodic, regenerate */
            if( pxEarliestDeadlineTaskNode->xTask.xPeriod > 0 )
            {
                xQueueSend( xTaskRegenerationRequestsQueueHandle, &pxEarliestDeadlineTaskNode->xTask, 0 );
            }

        }

        printf( "[Deadline Driven Scheduler] Checking messages...\n" );

        /*
        if( xQueueReceive( xSchedulerMessagesQueueHandle, (void *) &xSchedulerMessage, 1000 ) )
        {
            printf( "[Deadline Driven Scheduler] Received message." );
            switch( xSchedulerMessage.xMessageType )
            {
                printf( "[Deadline Driven Scheduler] Monitor requested completed list." );
                case REQUEST_COMPLETED_LIST:
                    ucNumTasks = xConvertTaskListToArray( xCompletedTasksHead, xTaskList );
                    xSchedulerMessage.xMessageType = INCOMING_COMPLETED_LIST;
                    break;
                case REQUEST_ACTIVE_LIST:
                    printf( "[Deadline Driven Scheduler] Monitor requested active list." );
                    ucNumTasks = xConvertTaskListToArray( xActiveTasksHead, xTaskList );
                    xSchedulerMessage.xMessageType = INCOMING_ACTIVE_LIST;
                    break;
                case REQUEST_OVERDUE_LIST:
                    printf( "[Deadline Driven Scheduler] Monitor requested overdue list." );
                    ucNumTasks = xConvertTaskListToArray( xOverdueTasksHead, xTaskList );
                    xSchedulerMessage.xMessageType = INCOMING_OVERDUE_LIST;
                    break;
                default:
                    printf( "[Deadline Driven Scheduler] Message was not for me. Sending it back. Sorry about that." );
            }
            xSchedulerMessage.xTasks = xTaskList;
            xSchedulerMessage.ucNumTasks = ucNumTasks;
            xQueueSend( xSchedulerMessagesQueueHandle, (void *) &xSchedulerMessage, 1000 );
        }
        else
        {
            printf( "[Deadline Driven Scheduler] No message found :(" );
        }
        */

        if( xQueueReceive( xSchedulerMessagesQueueHandle, &xSchedulerMessage, 0 ) )
        {
            printf( "[Deadline Driven Scheduler] Received message.\n" );
            switch( xSchedulerMessage )
            {
                case REQUEST_COMPLETED_LIST:
                    printf( "[Deadline Driven Scheduler] Monitor requested completed list.\n" );
                    vPrintTaskList( xCompletedTasksHead );
                    break;
                case REQUEST_ACTIVE_LIST:
                    printf( "[Deadline Driven Scheduler] Monitor requested active list.\n" );
                    vPrintTaskList( xActiveTasksHead );
                    break;
                case REQUEST_OVERDUE_LIST:
                    printf( "[Deadline Driven Scheduler] Monitor requested overdue list.\n" );
                    vPrintTaskList( xOverdueTasksHead );
                    break;
                default:
                    printf( "[Deadline Driven Scheduler] Unknown message found.\n" );
            }
        }
        else
        {
            printf( "[Deadline Driven Scheduler] No message found :(\n" );
        }

        if( xActiveTasksHead != NULL )
        {
            printf( "[Deadline Driven Scheduler]\n" );
            vPrintTaskList( xActiveTasksHead );
            pxCursorNode = xActiveTasksHead;
            xEarliestDeadline = pxCursorNode->xTask.xAbsoluteDeadline;
            pxEarliestDeadlineTaskNode = pxCursorNode;
            while( pxCursorNode->pxNext != NULL )
            {
                pxCursorNode = pxCursorNode->pxNext;

                if( pxCursorNode->xTask.xReleaseTime <= xTaskGetTickCount() &&
                    pxCursorNode->xTask.xAbsoluteDeadline < xEarliestDeadline )
                {
                    xEarliestDeadline = pxCursorNode->xTask.xAbsoluteDeadline;
                    pxEarliestDeadlineTaskNode = pxCursorNode;
                }
            }

            xStartTime = xTaskGetTickCount();
            vTaskPrioritySet( pxEarliestDeadlineTaskNode->xTask.xFTaskHandle, PRIORITY_HIGH );
            xEventGroupClearBits( xCurrentTaskCompleteEventGroup, CURRENT_TASK_COMPLETE_BIT );
            pxEarliestDeadlineTaskNode->xTask.xStartTime = xStartTime;
            uxBits = xEventGroupWaitBits( xCurrentTaskCompleteEventGroup,
                                          CURRENT_TASK_COMPLETE_BIT,
                                          pdFALSE,
                                          pdFALSE,
                                          (xEarliestDeadline - xStartTime)
                                        );
            vTaskPrioritySet( pxEarliestDeadlineTaskNode->xTask.xFTaskHandle, PRIORITY_LOW );
        }
        vTaskDelay( 10 );
    }
}
