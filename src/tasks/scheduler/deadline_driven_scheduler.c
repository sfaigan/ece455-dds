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
    TickType_t xCurrentTime = 0;
    MessageType_t xSchedulerMessage;
    EventBits_t uxBits;
    
    while( 1 )
    {
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

        if( xQueueReceive( xSchedulerMessagesQueueHandle, &xSchedulerMessage, 0 ) )
        {
            switch( xSchedulerMessage )
            {
                case REQUEST_COMPLETED_LIST:
                    printf( "Completed Tasks:\n" );
                    vPrintTaskList( xCompletedTasksHead );
                    break;
                case REQUEST_ACTIVE_LIST:
                    printf( "Active Tasks:\n" );
                    vPrintTaskList( xActiveTasksHead );
                    break;
                case REQUEST_OVERDUE_LIST:
                    printf( "Overdue Tasks:\n" );
                    vPrintTaskList( xOverdueTasksHead );
                    break;
                default:
                    printf( "[Deadline Driven Scheduler] Unknown message found.\n" );
            }
        }

        if( xActiveTasksHead != NULL )
        {
            pxEarliestDeadlineTaskNode = NULL;
            xEarliestDeadline = portMAX_DELAY;
            pxCursorNode = xActiveTasksHead;

            while( pxCursorNode != NULL )
            {
                xCurrentTime = xTaskGetTickCount();
                if( pxCursorNode->xTask.xReleaseTime <= xCurrentTime &&
                    pxCursorNode->xTask.xAbsoluteDeadline < xEarliestDeadline )
                {
                    xEarliestDeadline = pxCursorNode->xTask.xAbsoluteDeadline;
                    pxEarliestDeadlineTaskNode = pxCursorNode;
                }
                pxCursorNode = pxCursorNode->pxNext;
            }

            if( pxEarliestDeadlineTaskNode != NULL )
            {
                xStartTime = xTaskGetTickCount();
                printf("%s released at %d\n", pxEarliestDeadlineTaskNode->xTask.cName, (int) pxEarliestDeadlineTaskNode->xTask.xReleaseTime);
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
                xCompletionTime = 0;
                if( xQueueReceive( xTaskMessagesQueueHandle, &xCompletionTime, 0 ) )
                {
                    pxEarliestDeadlineTaskNode->xTask.xCompletionTime = xCompletionTime;
                    printf("%s completed at %d\n", pxEarliestDeadlineTaskNode->xTask.cName, (int) pxEarliestDeadlineTaskNode->xTask.xCompletionTime);
                    vAddTaskToList( &xCompletedTasksHead, pxEarliestDeadlineTaskNode->xTask );
                    vDeleteTaskFromList( &xActiveTasksHead, pxEarliestDeadlineTaskNode->xTask.xId );
                }
                else if( ( uxBits & CURRENT_TASK_COMPLETE_BIT ) == 0 )
                {
                    printf("%s overdue at %d\n", pxEarliestDeadlineTaskNode->xTask.cName, (int) xTaskGetTickCount() );
                    vAddTaskToList( &xOverdueTasksHead, pxEarliestDeadlineTaskNode->xTask );
                    vDeleteTaskFromList( &xActiveTasksHead, pxEarliestDeadlineTaskNode->xTask.xId );
                }

                /* If periodic, regenerate */
                if( pxEarliestDeadlineTaskNode->xTask.xPeriod > 0 )
                {
                    xQueueSend( xTaskRegenerationRequestsQueueHandle, &pxEarliestDeadlineTaskNode->xTask, 0 );
                }
            }
        }
        vTaskDelay( 1 );
    }
}
