/* Deadline Driven Scheduler API */

#include "deadline_driven_scheduler_api.h"

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
