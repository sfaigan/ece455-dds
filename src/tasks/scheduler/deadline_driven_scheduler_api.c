/* Deadline Driven Scheduler API */

#include "deadline_driven_scheduler_api.h"

DeadlineDrivenTaskNode_t *pxCreateTaskNode( DeadlineDrivenTask_t xTask )
{
    DeadlineDrivenTaskNode_t *pxTaskNode = pvPortMalloc( sizeof( DeadlineDrivenTaskNode_t ) );

    if ( pxTaskNode == NULL )
    {
        printf( "Failed to allocate a new task node.\n" );
        exit( EXIT_FAILURE );
    }

    pxTaskNode->xTask = xTask;
    pxTaskNode->pxNext = NULL;

    return pxTaskNode;
}

/* APPEND to list */
void vAddTaskToList( DeadlineDrivenTaskNode_t **pxTaskListHead, DeadlineDrivenTask_t xNewTask )
{
    DeadlineDrivenTaskNode_t *pxNewNode = pxCreateTaskNode( xNewTask );

    if( *pxTaskListHead == NULL )
    {
        *pxTaskListHead = pxNewNode;
    }
    else
    {
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
    if( pxTempNode != NULL && pxTempNode->xTask.xId == xTaskId )
    {
        *pxTaskListHead = pxTempNode->pxNext;
        vPortFree( pxTempNode );
        return;
    }

    while( pxTempNode != NULL && pxTempNode->xTask.xId != xTaskId )
    {
        pxPrevNode = pxTempNode;
        pxTempNode = pxTempNode->pxNext;
    }

    if( pxTempNode == NULL)
    {
        printf("Request to delete task. Could not find task matching ID %d.\n", ( int ) xTaskId);
        return;
    }

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
    static uint8_t ucTaskCounter = 0;
    ucTaskCounter++;
    /* Create FreeRTOS task */
    TaskHandle_t xFTaskHandle = NULL;
    xTaskCreate( vTaskFunction, cName, configMINIMAL_STACK_SIZE, ( void * ) ucTaskCounter, PRIORITY_LOW, &xFTaskHandle );
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
    /* Create custom task */
    DeadlineDrivenTaskId_t xTaskId = rand();
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

    if( xQueueSend( xNewTasksQueueHandle, &xNewTask, 1000 ) )
    {
        return pdPASS;
    }

    printf("Failed to send the following task to the New Tasks Queue:\n");
    vPrintDeadlineDrivenTaskInfo( xNewTask );

    return pdFAIL;
}

void vCompleteDeadlineDrivenTask()
{
    TickType_t xCurrentTime = xTaskGetTickCount();
    xQueueSend( xTaskMessagesQueueHandle, &xCurrentTime, 0 );
    ucSetNthEventBit( CURRENT_TASK_COMPLETE_BIT );
}

BaseType_t xSchedulerMessageRequest( MessageType_t xRequestType )
{
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
