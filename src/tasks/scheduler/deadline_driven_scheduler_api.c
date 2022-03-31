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

void vAddTaskToList( DeadlineDrivenTaskNode_t **pxTaskListHead, DeadlineDrivenTask_t xNewTask )
{
    if( pxTaskListHead == NULL )
    {
        *pxTaskListHead = pxCreateTaskNode( xNewTask );
    }
    else
    {
        DeadlineDrivenTaskNode_t *pxNewNode = pxCreateTaskNode( xNewTask );
        pxNewNode->pxNext = *pxTaskListHead;
        *pxTaskListHead = pxNewNode;
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
        printf("Failed to delete task.\n");
        return;
    }

    pxPrevNode->pxNext = pxTempNode->pxNext;
    vPortFree( pxTempNode );
}


void vPrintDeadlineDrivenTaskInfo( DeadlineDrivenTask_t xTask )
{
    /* printf( "%s\n", xTask.cName ); */
    printf( "\n**************************************************\n" );
    printf( "ID: %d\n", ( int ) xTask.xId );
    printf( "Absolute Deadline: %d\n", ( int ) xTask.xAbsoluteDeadline );
    printf( "Period: %d\n", ( int ) xTask.xPeriod );
    printf( "Release Time: %d\n", ( int ) xTask.xReleaseTime );
    printf( "Start Time: %d\n", ( int ) xTask.xStartTime );
    printf( "Completion Time: %d\n", ( int ) xTask.xCompletionTime );
}

void vPrintTaskList( DeadlineDrivenTaskNode_t *pxTaskListHead )
{
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
    /* Create FreeRTOS task */
    TaskHandle_t xFTaskHandle = NULL;
    xTaskCreate( vTaskFunction, cName, configMINIMAL_STACK_SIZE, NULL, PRIORITY_LOW, &xFTaskHandle );
    return ulCreateDeadlineDrivenTaskMetadata( xFTaskHandle, cName, xAbsoluteDeadline, xPeriod, xReleaseTime );
}

uint32_t ulCreateDeadlineDrivenTaskMetadata( TaskHandle_t xFTaskHandle,
                                             char cName[],
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
        xFTaskHandle: xFTaskHandle,
        xAbsoluteDeadline: xAbsoluteDeadline,
        xPeriod: xPeriod,
        xReleaseTime: xReleaseTime,
        xStartTime: 0,
        xCompletionTime: 0
    };
    /*  strcpy(xNewTask.cName, cName); */

    if( xQueueSend( xNewTasksQueueHandle, &xNewTask, 1000 ) )
    {
        printf("New task successfully created and sent to scheduler: %s\n", cName);
        return 1;
    }

    printf("Failed to send new task to New Tasks Queue.\n");
    return 0;
}

/*
uint8_t xConvertTaskListToArray( DeadlineDrivenTaskNode_t *pxTaskListHead, DeadlineDrivenTask_t *pxTaskArray )
{
    DeadlineDrivenTaskNode_t *pxCurrentNode = pxTaskListHead;
    uint8_t ucCounter = 0;

    while( pxCurrentNode != NULL )
    {
        pxTaskArray[ucCounter] = pxCurrentNode->xTask;
        pxCurrentNode = pxCurrentNode->pxNext;
        ucCounter++;
    }

    return ucCounter + 1;
}
*/

void vCompleteDeadlineDrivenTask()
{
    printf("Task completed!\n");
    TickType_t xCurrentTime = xTaskGetTickCount();
    xQueueSend( xTaskMessagesQueueHandle, &xCurrentTime, 1000 );
    xEventGroupSetBits( xCurrentTaskCompleteEventGroup, CURRENT_TASK_COMPLETE_BIT );
}

//static DDTaskNode* xReturnActiveDeadlineDrivenTasks();
//static DDTaskNode* xReturnOverdueDeadlineDrivenTasks();
//static DDTaskNode* xReturnCompletedDeadlineDrivenTasks();

BaseType_t xSchedulerMessageRequest( MessageType_t xRequestType )
{
    /*
    DeadlineDrivenTask_t xTasks[MAX_TASKS];
    DeadlineDrivenTask_t xNullTask = { 0 };
    uint8_t ucNumTasks = 0;
    uint8_t ucCounter = 0;

    for( ucCounter = 0; ucCounter < MAX_TASKS; ucCounter++ )
    {
        xTasks[ucCounter] = xNullTask;
    }
    SchedulerMessage_t xSchedulerMessage =
    {
        xMessageType: RequestType,
        xTasks: xTasks,
        ucNumTasks: ucNumTasks
    };
    return xQueueSend(xSchedulerMessagesQueueHandle, (void *) &xSchedulerMessage, 1000 );
    */
    return xQueueSend(xSchedulerMessagesQueueHandle, (void *) &xRequestType, 1000 );
}

