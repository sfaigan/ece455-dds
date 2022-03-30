/* Deadline Driven Scheduler */

#include "deadline_driven_scheduler.h"

static DeadlineDrivenTaskNode_t *xAddTaskToList ( DeadlineDrivenTaskNode_t *pxTaskListHead,
                                                          DeadlineDrivenTask_t xNewTask )
{
    if( !pxTaskListHead )
    {
        pxTaskListHead = pvPortMalloc( sizeof( DeadlineDrivenTaskNode_t ) );
        pxTaskListHead->xTask = xNewTask;
        pxTaskListHead->pxNext = NULL;
    }
    else
    {
        DeadlineDrivenTaskNode_t *pxNewNode = pvPortMalloc( sizeof( DeadlineDrivenTaskNode_t ) );
        pxNewNode->xTask = xNewTask;
        pxNewNode->pxNext = NULL;
        DeadlineDrivenTaskNode_t pxCurrentNode = pxTaskListHead;
        while( pxCurrentNode->pxNext )
        {
            pxCurrentNode = pxTaskListHead->pxNext;
        }
        pxCurrentNode->pxNext = pxNewNode;
    }
    return pxTaskListHead;
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
        cName: cName,
        xFTaskHandle: xFTaskHandle,
        xAbsoluteDeadline: xAbsoluteDeadline,
        xPeriod: xPeriod,
        xReleaseTime: xReleaseTime,
        xStartTime: 0,
        xCompletionTime: 0
    };

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

BaseType_t xDeleteDeadlineDrivenTask( uint32_t ulTaskId )
{
    printf("Request to delete task: %lu\n", ulTaskId);
    return 0;
}

//static DDTaskNode* xReturnActiveDeadlineDrivenTasks();
//static DDTaskNode* xReturnOverdueDeadlineDrivenTasks();
//static DDTaskNode* xReturnCompletedDeadlineDrivenTasks();

void vDeadlineDrivenScheduler( void *pvParameters )
{
    DeadlineDrivenTaskNode_t *xActiveTasksHead = NULL;
    DeadlineDrivenTaskNode_t *xOverdueTasksHead = NULL;
    DeadlineDrivenTaskNode_t *xCompletedTasksHead = NULL;
    DeadlineDrivenTask_t xNewTask;
    
    while( 1 )
    {
        while( xQueueReceive( xNewTasksQueueHandle, &xNewTask, 1000 ) )
        {
            if( xNewTask.xAbsoluteDeadline > xTaskGetTickCount() )
            {
                xOverdueTasksHead = xAddTaskToList(&xOverdueTasksHead, xNewTask);
            }
            else
            {
                xActiveTasksHead = xAddTaskToList(&xActiveTasksHead, xNewTask);
            }
        }
        /*
            Receive from new task queue
                For each task in new task queue
                    If current time >= absolute deadline
                        Copy task to overdue list
                    Else
                        Copy task to active list

                Receive from task message queue
                    If not empty
                        Copy completed DDTask (in active tasks) to completed tasks
                        Update completed DDTask's completion time from message
                        Remove completed DDTask from active tasks
                        Set priority of completed task's FTask to LOW
                        Send copy of completed DDTask to task generation requests queue
                
                Check monitor message queue (empty)
                    If not empty, perform requested action (switch/case)
                
                Find task with earliest deadline where current time > release time
                Set start time of EDF Task's DDTask to current time (in ticks)
                Set priority of EDF Task's FTask to HIGH
                Unset CURRENT_TASK_COMPLETE event bit
                Sleep until CURRENT_TASK_COMPLETE event bit is set or for a maximum of (absolute deadline - current time)
        */
        vTaskDelay(1000);
    }
}
