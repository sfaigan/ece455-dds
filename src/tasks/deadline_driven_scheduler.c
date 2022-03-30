/* Deadline Driven Scheduler */

#include "deadline_driven_scheduler.h"

void vDeadlineDrivenScheduler( void *pvParameters )
{
    /* Variables go here */
    
    while( 1 )
    {
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
