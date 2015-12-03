/* ------------------------------------------------------------------------------
|                      Non-preemptive Arduino Task Scheduler                    |
|                    =========================================                  |
|                                  Version 1.0                                  |
|                    =========================================                  |
|                                 Cam Ketchmark                                 |
|                    -----------------------------------------                  |
| * Based on TimerOne interrupt library (uses timer1 interrupt for tasks        |
| * Must initScheduler() before scheduling tasks                                |
|	- define the interrupt period (the LCD of your tasks)                       |
| * Use createTask() to create individual tasks, then scheduleTask()            |
|	to add them to the taskList                                                 |
| * Tasks stored in linked list based on priority (highest priority first)      |
| * As tasks are performed in an interrupt, they should be kept to a minimum    |
|	and refrain from long I/O or using functions like Serial.println()          |
------------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <Arduino.h>
#include <TimerOne.h>
#include "TaskScheduler.h"

/* Default Global Variables */
taskList tasks;
taskPriority currentPriority = PRIOR_VERY_HIGH;
bool processingTasks = false;
long TIMER1_PERIOD = 1000000;

void initScheduler(long period)
{
	tasks.front = NULL;
	tasks.count = 0;
	processingTasks = false;
	TIMER1_PERIOD = period;
	Timer1.initialize(TIMER1_PERIOD); //Timer set interrupt once per seocnd
	Timer1.attachInterrupt(scheduleTimerISR);
	Timer1.stop();
}

task createTask(int ID, long period, taskFunc func, taskPriority priority, bool isEnabled)
{
	task newTask;
	newTask.ID = ID;
	newTask.period = period;
	newTask.elapsedTime = 0;
	newTask.func = func; // Set newTask->func to point to same function as func points to
	newTask.priority = priority;
	newTask.isEnabled = isEnabled;
	return newTask;
}

void scheduleTask(task newTask)
{
	taskNode *cursor;
	taskNode *temp;

	// Create node
	taskNode *node;
	node = (taskNode*)malloc(sizeof(*node)); // Allocate memory for new node
	node->taskData = newTask;
	node->next = NULL;

	// Add node to linked list according to priority
	if (tasks.front == NULL) // If linked list empty
	{
		tasks.front = node; // Set front pointer to point to node
	}
	else
	{
		temp = (taskNode*)malloc(sizeof(*temp));
		cursor = tasks.front;

		// Search for spot in queue
		while ((node->taskData.priority >= cursor->taskData.priority) && (cursor->next != NULL)) // Place after all lower priority tasks
		{
			cursor = cursor->next; // Iterate cursor through linked list
		}
		temp = cursor->next; // Temporarily store cursor->next node to temp
		cursor->next = node; // Set next pointer of cursor to new node
		node->next = temp; // Set next pointer of new node to temp
	}
	tasks.count++;
}

bool enableTask(int ID)
{
	taskNode *cursor;
	cursor = tasks.front;
	while(cursor != NULL)
	{
		if (cursor->taskData.ID == ID)
		{
			cursor->taskData.isEnabled = true;
			return true;
		}
		else
		{
			cursor = cursor->next;
		}
	}
	return false;
}

bool disableTask(int ID)
{
	taskNode *cursor;
	cursor = tasks.front;
	while(cursor != NULL)
	{
		if (cursor->taskData.ID == ID)
		{
			cursor->taskData.isEnabled = false;
			return true;
		}
		else
		{
			cursor = cursor->next;
		}
	}
	return false;
}

long getTaskTimeElapsed(int ID)
{
	taskNode *cursor;
	cursor = tasks.front;
	while(cursor != NULL)
	{
		if (cursor->taskData.ID == ID)
		{
			return (cursor->taskData.elapsedTime);
		}
		else
		{
			cursor = cursor->next;
		}
	}
	return -1;
}

void scheduleTimerISR()
{
	// Timer 1 interrupt routine 
	if (processingTasks)
	{
		//Task not completed yet
	}
	else
	{
		processingTasks = true;
		taskNode *cursor;
		cursor = tasks.front;
		while (cursor != NULL)
		{	
			if (cursor->taskData.isEnabled)
			{
				if (cursor->taskData.elapsedTime >= cursor->taskData.period)
				{
					cursor->taskData.func(); // Call task's function
					currentPriority = cursor->taskData.priority;
					cursor->taskData.elapsedTime = 0; // Set time since last run to 0
				}
			cursor->taskData.elapsedTime += TIMER1_PERIOD;
			}
			cursor = cursor->next; // Iterate cursor to next task in taskList
			interrupts();
		}
		processingTasks = false;
	}
}

void printTaskList()
{
	taskNode *cursor;
	cursor = tasks.front;
	while(cursor != NULL)
	{
		Serial.println(cursor->taskData.ID);
		Serial.println(cursor->taskData.period);
		cursor = cursor->next;
	}
}

void runScheduler()
{
	Timer1.start();
}

void stopScheduler()
{
	Timer1.stop();
}
