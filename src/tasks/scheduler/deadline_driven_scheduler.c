/* Deadline Driven Scheduler */

#include "deadline_driven_scheduler.h"

void vDeadlineDrivenScheduler( void *pvParameters )
{
    /* Pointers to active, overdue, and completed task lists */
    DeadlineDrivenTaskNode_t *xActiveTasksHead = NULL;
    DeadlineDrivenTaskNode_t *xOverdueTasksHead = NULL;
    DeadlineDrivenTaskNode_t *xCompletedTasksHead = NULL;

    /* Temporary pointer used to iterate over lists */
    DeadlineDrivenTaskNode_t *pxCursorNode = NULL;

    /* Pointer to the earliest deadline task node (next to be scheduled) */
    DeadlineDrivenTaskNode_t *pxEarliestDeadlineTaskNode = NULL;

    /* Stores each new task before they are pushed to the active or overdue list */
    DeadlineDrivenTask_t xNewTask;

    /* Tick counter variables for scheduling */
    TickType_t xEarliestDeadline = 0;
    TickType_t xStartTime = 0;
    TickType_t xCompletionTime = 0;
    TickType_t xCurrentTime = 0;

    /* Stores messages from currently running tasks */
    MessageType_t xSchedulerMessage;

    /* Event bits used to keep track of completed and overdue tasks */
    EventBits_t uxBits;
    
    while( 1 )
    {
        /* Get new tasks from New Tasks Queue */
        while( xQueueReceive( xNewTasksQueueHandle, &xNewTask, 0 ) )
        {
            if( xNewTask.xAbsoluteDeadline <= xTaskGetTickCount() )
            {
                /* If the task is already overdue, push it to the overdue list */
                vAddTaskToList( &xOverdueTasksHead, xNewTask );
            }
            else
            {
                /* Otherwise, push it to the active list for scheduling */
                vAddTaskToList( &xActiveTasksHead, xNewTask );
            }
        }

        /* Handle messages from the task monitor */
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
                    /* Should not reach this point */
                    printf( "[Deadline Driven Scheduler] Unknown message found.\n" );
            }
        }

        /* Handle scheduling active tasks */
        if( xActiveTasksHead != NULL )
        {
            pxEarliestDeadlineTaskNode = NULL;
            xEarliestDeadline = portMAX_DELAY;
            pxCursorNode = xActiveTasksHead;

            /* Find the task with the earliest deadline that has been released to the scheduler */
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

            /* Schedule the task */
            if( pxEarliestDeadlineTaskNode != NULL )
            {
                xStartTime = xTaskGetTickCount();
                /* printf("%s released at %d\n", pxEarliestDeadlineTaskNode->xTask.cName, (int) pxEarliestDeadlineTaskNode->xTask.xReleaseTime); */
                
                /* Set the priority of the task to HIGH (one level below the DDS) */
                vTaskPrioritySet( pxEarliestDeadlineTaskNode->xTask.xFTaskHandle, PRIORITY_HIGH );
                
                /* Mark the task as incomplete/in-progress */
                ucClearNthEventBit( CURRENT_TASK_COMPLETE_BIT );

                /* Update the start time */
                pxEarliestDeadlineTaskNode->xTask.xStartTime = xStartTime;

                /* Wait until the task completes (the event bit is set)
                   or the relative deadline, whatever comes first */
                uxBits = xEventGroupWaitBits( xTaskEventGroup,
                                              EVENT_BIT( CURRENT_TASK_COMPLETE_BIT ),
                                              pdFALSE,
                                              pdFALSE,
                                              (xEarliestDeadline - xStartTime)
                                            );

                /* Once the task has completed or been pre-empted because it did not finish on time,
                   update its priority so it will not be scheduled */
                vTaskPrioritySet( pxEarliestDeadlineTaskNode->xTask.xFTaskHandle, PRIORITY_LOW );

                /* If the task completed, update its completion time and move it from
                   the active task list to the completed task list */
                xCompletionTime = 0;
                if( xQueueReceive( xTaskMessagesQueueHandle, &xCompletionTime, 0 ) )
                {
                    pxEarliestDeadlineTaskNode->xTask.xCompletionTime = xCompletionTime;
                    /* printf("%s completed at %d\n", pxEarliestDeadlineTaskNode->xTask.cName, (int) pxEarliestDeadlineTaskNode->xTask.xCompletionTime); */
                    vAddTaskToList( &xCompletedTasksHead, pxEarliestDeadlineTaskNode->xTask );
                    vDeleteTaskFromList( &xActiveTasksHead, pxEarliestDeadlineTaskNode->xTask.xId );
                }
                else if( ( uxBits & EVENT_BIT( CURRENT_TASK_COMPLETE_BIT ) ) == 0 )
                {
                    /* If the task did not complete by its deadline, move it from the active task list
                       to the overdue task list and mark the task as overdue so that the next time it
                       runs, it restarts rather than continuing from where it left off */
                    /* printf("%s overdue at %d\n", pxEarliestDeadlineTaskNode->xTask.cName, (int) xTaskGetTickCount() ); */
                    ucSetNthEventBit( pxEarliestDeadlineTaskNode->xTask.ucTaskNumber );
                    vAddTaskToList( &xOverdueTasksHead, pxEarliestDeadlineTaskNode->xTask );
                    vDeleteTaskFromList( &xActiveTasksHead, pxEarliestDeadlineTaskNode->xTask.xId );
                }

                /* If the task is periodic, send it to the task generator for regeneration */
                if( pxEarliestDeadlineTaskNode->xTask.xPeriod > 0 )
                {
                    xQueueSend( xTaskRegenerationRequestsQueueHandle, &pxEarliestDeadlineTaskNode->xTask, 0 );
                }
            }
        }
        vTaskDelay( 1 );
    }
}
