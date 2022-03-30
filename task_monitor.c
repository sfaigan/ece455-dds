/* Task Monitor */

#include "task_monitor.h"

void vTaskMonitor( void *pvParameters )
{
    while(1)
	{
        DeadlineDrivenTaskNode_t CompletedList, ActiveList, OverdueList;
        StatusMessage_t incoming_message;
        MessageType_t message_type;
        /* Check for messages from scheduler */
        incoming_message = xCheckSchedulerMessage();
        if(incoming_message != 0)
	    {
            message_type = incoming_message->MessageType;
            switch(message_type)
            {
            /* For cases 0-2 message is outgoing, not for monitor */
            case 0:

	    	    break;
            case 1:

	    	    break;
            case 2:

	    	    break;
            case 3:
	    	    incoming_message = xGetSchedulerMessage();
	    	    CompletedList = incoming_message->List;
	    	    break;
            case 4:
	    	    incoming_message = xGetSchedulerMessage();
	    	    ActiveList = incoming_message->List;
	    	    break;
            case 5:
	    	    incoming_message = xGetSchedulerMessage();
	    	    OverdueList = incoming_message->List;
	    	    break;
            }
	    }
        /* Request data from scheduler */
        if (xSchedulerMessageRequest(0))
        {
            printf("Requested completed list");
        }
        else
        {
            printf("Completed request failed!");
        }

        if (xSchedulerMessageRequest(1))
        {
            printf("Requested active list");
        }
        else
        {
            printf("Active request failed!");
        }

        if (xSchedulerMessageRequest(2))
        {
            printf("Requested overdue list");
        }
        else
        {
            printf("Overdue request failed!");
        }
    }


}
