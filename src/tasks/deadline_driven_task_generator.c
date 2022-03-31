/* Deadline Driven Task Generator */

#include "deadline_driven_task_generator.h"

void vDeadlineDrivenTaskGenerator( void *pvParameters )
{
    uint8_t ucRequestCounter;
    uint8_t ucMessagesAvailable = 0;
    DeadlineDrivenTask_t xTask;

    while( 1 )
    {
        ucMessagesAvailable = uxQueueMessagesWaiting(TaskRegenerationRequestsQueueHandle, 1000 );
        for( ucRequestCounter = 0; ucRequestCounter < ucMessagesAvailable; ucRequestCounter++ )
        {
            if ( xQueueReceive(xTaskRegenerationRequestsQueueHandle, &xTask, 1000 ) )
            {
                if ( xTask.completion_time!= 0 )
                {
                    ulCreateDeadlineDrivenTask( function,
                                                "function name",
                                                xTask.xAbsoluteDeadline +xTask.xPeriod,
                                                xTask.xPeriod,
                                                xTask.xReleaseTime + xTask.xPeriod
                                               );
                }
                else
                {
                /* task is invalid
                }
            }
            else
            {
            /* failed to receive message */
            }
        }
        vTaskDelay( 10 );
    }
}
