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
    
    while( 1 )
    {
        while( xQueueReceive( xNewTasksQueueHandle, &xNewTask, 1000 ) )
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
            vTaskPrioritySet( pxEarliestDeadlineTaskNode->xTask.xFTaskHandle, PRIORITY_LOW );
            pxEarliestDeadlineTaskNode->xTask.xCompletionTime = xCompletionTime;
            xQueueSend( xTaskRegenerationRequestsQueueHandle, &pxEarliestDeadlineTaskNode->xTask, 1000 );

            if( xQueueReceive( xTaskMessagesQueueHandle, &xCompletionTime, 1000 ) )
            {
                vAddTaskToList( &xCompletedTasksHead, pxEarliestDeadlineTaskNode->xTask );
                vDeleteTaskFromList( &xActiveTasksHead, pxEarliestDeadlineTaskNode->xTask.ulId );
            }
            else if ( xEventGroupGetBits( xCurrentTaskCompleteEventGroup ) == 0 )
            {
                vAddTaskToList( &xOverdueTasksHead, pxEarliestDeadlineTaskNode->xTask );
                vDeleteTaskFromList( &xActiveTasksHead, pxEarliestDeadlineTaskNode->xTask.ulId );
            }
        }

        /*
            Check monitor message queue (empty)
                If not empty, perform requested action (switch/case)
                
        */

        if( xActiveTasksHead != NULL )
        {
            vPrintTaskList( xActiveTasksHead );
            pxCursorNode = xActiveTasksHead;
            xEarliestDeadline = pxCursorNode->xTask.xAbsoluteDeadline;
            pxEarliestDeadlineTaskNode = pxCursorNode;
            while( pxCursorNode->pxNext != NULL )
            {
                pxCursorNode = pxCursorNode->pxNext;

                if( pxCursorNode->xTask.xReleaseTime >= xTaskGetTickCount() &&
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
            xEventGroupWaitBits( xCurrentTaskCompleteEventGroup,
                                 CURRENT_TASK_COMPLETE_BIT,
                                 pdFALSE,
                                 pdFALSE,
                                 (xEarliestDeadline - xStartTime)
                               );
        }
        vTaskDelay(1000);
    }
}
