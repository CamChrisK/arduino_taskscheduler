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

#ifndef TaskScheduler_H
#define TaskScheduler_H

#include <stdio.h>
#include <stdlib.h>
#include <Arduino.h>

typedef void(*taskFunc)(void);

typedef enum taskPriority { PRIOR_VERY_HIGH = 1, PRIOR_HIGH = 2, PRIOR_MEDIUM = 3, PRIOR_LOW = 4, PRIOR_VERY_LOW = 5 } taskPriority; // Smaller number = higher priority

typedef struct task
{
	int ID;
	long period; // How often task called (every period seconds)
	long elapsedTime; // How long since task last called
	taskFunc func; // Pointer to task function
	taskPriority priority;
	bool isEnabled;
} task;

typedef struct taskNode
{
	task taskData; // Data stored at node
	struct taskNode *next; // Pointer to next node
} taskNode;

typedef struct taskList
{
	int count; // Active count of number of nodes
	taskNode *front; //Pointer to front node
} taskList;

extern taskList tasks;
extern taskPriority currentPriority;
extern bool processingTasks;
extern long TIMER1_PERIOD;

extern void initScheduler(long);
extern task createTask(int, long, taskFunc, taskPriority, bool);
extern void scheduleTask(task);
extern bool enableTask(int);
extern bool disableTask(int);
extern long getTaskTimeElapsed(int);
extern void scheduleTimerISR(void);
extern void runScheduler(void);
extern void stopScheduler(void);
extern void printTaskList(void);

#endif		
