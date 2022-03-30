/* Deadline Driven Scheduler */

#include "deadline_driven_scheduler.h"

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
//    strcpy(xNewTask.cName, cName);

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
