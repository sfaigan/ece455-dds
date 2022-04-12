/* Deadline Driven Scheduler API */

#include "deadline_driven_scheduler_api.h"

DeadlineDrivenTaskNode_t *pxCreateTaskNode( DeadlineDrivenTask_t xTask )
{
    /* Allocate memory for a new task node */
    DeadlineDrivenTaskNode_t *pxTaskNode = pvPortMalloc( sizeof( DeadlineDrivenTaskNode_t ) );

    /* Error handling */
    if ( pxTaskNode == NULL )
    {
        printf( "Failed to allocate a new task node.\n" );
        exit( EXIT_FAILURE );
    }

    /* Initialize node */
    pxTaskNode->xTask = xTask;
    pxTaskNode->pxNext = NULL;

    return pxTaskNode;
}

void vAddTaskToList( DeadlineDrivenTaskNode_t **pxTaskListHead, DeadlineDrivenTask_t xNewTask )
{
    /* Create a new task node for the given task */
    DeadlineDrivenTaskNode_t *pxNewNode = pxCreateTaskNode( xNewTask );

    /* If the list provided is empty, set the head to the new node */
    if( *pxTaskListHead == NULL )
    {
        *pxTaskListHead = pxNewNode;
    }
    else
    {
        /* Otherwise, append the new node to the list */
        DeadlineDrivenTaskNode_t *pxCursorNode = *pxTaskListHead;
        while( pxCursorNode->pxNext != NULL )
        {
            pxCursorNode = pxCursorNode->pxNext;
        }

        pxCursorNode->pxNext = pxNewNode;
    }
}

void vDeleteTaskFromList( DeadlineDrivenTaskNode_t **pxTaskListHead, DeadlineDrivenTaskId_t xTaskId )
{
    DeadlineDrivenTaskNode_t *pxTempNode = *pxTaskListHead;
    DeadlineDrivenTaskNode_t *pxPrevNode;

    /* If the task node is at the head */
    if( pxTempNode != NULL && pxTempNode->xTask.xId == xTaskId )
    {
        *pxTaskListHead = pxTempNode->pxNext;
        vPortFree( pxTempNode );
        return;
    }

    /* Find the task node */
    while( pxTempNode != NULL && pxTempNode->xTask.xId != xTaskId )
    {
        pxPrevNode = pxTempNode;
        pxTempNode = pxTempNode->pxNext;
    }

    /* If the task node isn't in the list */
    if( pxTempNode == NULL)
    {
        printf("Request to delete task. Could not find task matching ID %d.\n", ( int ) xTaskId);
        return;
    }

    /* Delete the task node */
    pxPrevNode->pxNext = pxTempNode->pxNext;
    vPortFree( pxTempNode );
}


void vPrintDeadlineDrivenTaskInfo( DeadlineDrivenTask_t xTask )
{
    printf( "%s\n", xTask.cName );
    printf( "ID: %d\n", ( int ) xTask.xId );
    printf( "Task Number: %d\n", xTask.ucTaskNumber );
    printf( "Absolute Deadline: %d\n", ( int ) xTask.xAbsoluteDeadline );
    printf( "Period: %d\n", ( int ) xTask.xPeriod );
    printf( "Release Time: %d\n", ( int ) xTask.xReleaseTime );
    printf( "Start Time: %d\n", ( int ) xTask.xStartTime );
    printf( "Completion Time: %d\n", ( int ) xTask.xCompletionTime );
    printf( "**************************************************\n" );
}

void vPrintTaskList( DeadlineDrivenTaskNode_t *pxTaskListHead )
{
    printf( "**************************************************\n" );

    /* Iterate over the list and print each task's metadata */
    DeadlineDrivenTaskNode_t *pxCurrentNode = pxTaskListHead;
    while( pxCurrentNode != NULL )
    {
        vPrintDeadlineDrivenTaskInfo( pxCurrentNode->xTask );
        pxCurrentNode = pxCurrentNode->pxNext;
    }
}

uint32_t ulCreateDeadlineDrivenTask( void (*vTaskFunction)( void * ),
                                     char cName[],
                                     TickType_t xAbsoluteDeadline,
                                     TickType_t xPeriod,
                                     TickType_t xReleaseTime
                                   )
{
    /* Initialize the task counter to 0 at compile-time
       The task counter is used in the event group */
    static uint8_t ucTaskCounter = 0;

    /* Increment the task counter
       The task counter starts at 1 because the first event bit is reserved
       for indicating whether the last task that ran completed */
    ucTaskCounter++;

    /* Create FreeRTOS task */
    TaskHandle_t xFTaskHandle = NULL;
    xTaskCreate( vTaskFunction, cName, configMINIMAL_STACK_SIZE, ( void * ) ucTaskCounter, PRIORITY_LOW, &xFTaskHandle );

    /* Create and return a new DD-task */
    return ulCreateDeadlineDrivenTaskMetadata( xFTaskHandle, cName, ucTaskCounter, xAbsoluteDeadline, xPeriod, xReleaseTime );
}

uint32_t ulCreateDeadlineDrivenTaskMetadata( TaskHandle_t xFTaskHandle,
                                             char cName[],
                                             uint8_t ucTaskNumber,
                                             TickType_t xAbsoluteDeadline,
                                             TickType_t xPeriod,
                                             TickType_t xReleaseTime
                                           )
{
    /* Generate unique task ID */
    DeadlineDrivenTaskId_t xTaskId = rand();

    /* Initialize new DD-task */
    DeadlineDrivenTask_t xNewTask =
    {
        xId: xTaskId,
        ucTaskNumber: ucTaskNumber,
        xFTaskHandle: xFTaskHandle,
        xAbsoluteDeadline: xAbsoluteDeadline,
        xPeriod: xPeriod,
        xReleaseTime: xReleaseTime,
        xStartTime: 0,
        xCompletionTime: 0
    };
    strncpy( xNewTask.cName, cName, MAX_TASK_NAME_LENGTH-1 );

    /* Send the new task to the scheduler */
    if( xQueueSend( xNewTasksQueueHandle, &xNewTask, 1000 ) )
    {
        return pdPASS;
    }

    /* Error handling */
    printf("Failed to send the following task to the New Tasks Queue:\n");
    vPrintDeadlineDrivenTaskInfo( xNewTask );
    return pdFAIL;
}

void vCompleteDeadlineDrivenTask()
{
    /* Send the completion time to the scheduler to maximize accuracy */
    TickType_t xCurrentTime = xTaskGetTickCount();
    xQueueSend( xTaskMessagesQueueHandle, &xCurrentTime, 0 );

    /* Mark the task as complete to wake up the scheduler */
    ucSetNthEventBit( CURRENT_TASK_COMPLETE_BIT );
}

BaseType_t xSchedulerMessageRequest( MessageType_t xRequestType )
{
    /* Send the specified request to the scheduler and return the result */
    return xQueueSend(xSchedulerMessagesQueueHandle, (void *) &xRequestType, 0 );
}

uint8_t ucGetNthEventBit( uint8_t ucN )
{
    EventBits_t xEventBits = xEventGroupGetBits( xTaskEventGroup );
    return ( xEventBits & ( 1 << ucN ) ) >> ucN;
}

uint8_t ucSetNthEventBit( uint8_t ucN )
{
    return xEventGroupSetBits( xTaskEventGroup, EVENT_BIT( ucN ) );
}

uint8_t ucClearNthEventBit( uint8_t ucN )
{
    return xEventGroupClearBits( xTaskEventGroup, EVENT_BIT( ucN ) );
}
