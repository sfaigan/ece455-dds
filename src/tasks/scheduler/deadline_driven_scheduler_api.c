/* Deadline Driven Scheduler API */

#include "deadline_driven_scheduler_api.h"

static DeadlineDrivenTaskNode_t *pxCreateTaskNode( DeadlineDrivenTask_t xTask )
{
    DeadlineDrivenTaskNode_t *pxTaskNode = pvPortMalloc( sizeof( DeadlineDrivenTaskNode_t ) );

    if ( pxTaskNode == NULL )
    {
        printf( "Failed to allocate a new task node." );
        exit( EXIT_FAILURE );
    }

    pxTaskNode->xTask = xTask;
    pxTaskNode->pxNext = NULL;

    return pxTaskNode;
}

static void vAddTaskToList( DeadlineDrivenTaskNode_t **pxTaskListHead, DeadlineDrivenTask_t xNewTask )
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

static void vDeleteTaskFromList( DeadlineDrivenTaskNode_t **pxTaskListHead, uint32_t ulTaskId )
{
    DeadlineDrivenTaskNode_t *pxTempNode = *pxTaskListHead;
    DeadlineDrivenTaskNode_t *pxPrevNode;
    if( pxTempNode != NULL && pxTempNode->xTask.ulId == ulTaskId )
    {
        *pxTaskListHead = pxTempNode->pxNext;
        vPortFree( pxTempNode );
        return;
    }

    while( pxTempNode != NULL && pxTempNode->xTask.ulId != ulTaskId )
    {
        pxPrevNode = pxTempNode;
        pxTempNode = pxTempNode->pxNext;
    }

    if( pxTempNode == NULL)
    {
        printf("Failed to delete task.");
        return;
    }

    pxPrevNode->pxNext = pxTempNode->pxNext;
    vPortFree( pxTempNode );
}


static void vPrintDeadlineDrivenTaskInfo( DeadlineDrivenTask_t xTask )
{
    // printf( "%s\n", xTask.cName );
    printf( "**************************************************\n" );
    printf( "ID: %lu\n", xTask.ulId );
    printf( "Absolute Deadline: %lu\n", xTask.xAbsoluteDeadline );
    printf( "Period: %lu\n", xTask.xPeriod );
    printf( "Release Time: %lu\n", xTask.xReleaseTime );
    printf( "Start Time: %lu\n", xTask.xStartTime );
    printf( "Completion Time: %lu\n", xTask.xCompletionTime );
}

static void vPrintTaskList( DeadlineDrivenTaskNode_t *pxTaskListHead )
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

    /* Create custom task */
    uint32_t ulId = rand();
    DeadlineDrivenTask_t xNewTask =
    {
        ulId: ulId,
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
    }
    else
    {
        printf("Failed to send new task to New Tasks Queue.\n");
    }
    return 0;
}

void vDeleteDeadlineDrivenTask( uint32_t ulTaskId )
{
    printf("Request to delete task: %lu\n", ulTaskId);
}

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

xSchedulerMessageRequest(MessageType_t RequestType)
{
    if( xQueueSend(xTaskMonitorQueueHandle, RequestType, (Tick_Type_t) 10) )
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

SchedulerMessage_t xCheckSchedulerMessage()
{
    SchedulerMessage_t message;
    if( xQueuePeek(xTaskMonitorQueueHandle, &message, (Tick_Type_t) 10) )
    {
        return message;
    }
    else
    {
        return 0;
    }
}

StatusMessage_t xGetSchedulerMessage()
{
    StatusMessage_t message;
    if( xQueueRecieve(xTaskMonitorQueueHandle, &message, (Tick_Type_t) 10) )
    {
        return message;
    }
    else
    {
        return 0;
    }
}
