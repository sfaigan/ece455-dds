/* Deadline Driven Task Generator */

#include "deadline_driven_task_generator.h"

void vDeadlineDrivenTaskGenerator( void *pvParameters )
{
    uint8_t ucRequestCounter;
    uint8_t ucMessagesAvailable = 0;
    DeadlineDrivenTask_t xTask;

    while( 1 )
    {
        ucMessagesAvailable = uxQueueMessagesWaiting( xTaskRegenerationRequestsQueueHandle );
        for( ucRequestCounter = 0; ucRequestCounter < ucMessagesAvailable; ucRequestCounter++ )
        {
            if( xQueueReceive(xTaskRegenerationRequestsQueueHandle, &xTask, 1000 ) )
            {
                ulCreateDeadlineDrivenTaskMetadata( xTask.xFTaskHandle,
                                                    xTask.cName,
                                                    xTask.xAbsoluteDeadline +xTask.xPeriod,
                                                    xTask.xPeriod,
                                                    xTask.xReleaseTime + xTask.xPeriod
                                                  );
            }
            else
            {
                printf( "[Deadline Driven Task Generator] Failed to receive a task from the queue.\n" );
            }
        }
        vTaskDelay( 1 );
    }
}
