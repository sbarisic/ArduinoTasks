#ifndef _ARDUINO_TASKS_H
#define _ARDUINO_TASKS_H

#include "WConstants.h"

#define MAX_TASKS 6

typedef struct TaskStruct {
	int TaskIndex;
	unsigned long NextRunTime;

	int TriggerCount;
	int MaxTriggerCount;
	unsigned long LoopDelay;
	int NextDelayOffset;

	int LastTweenAmount;
	int TweenAmount;
	int InvTweenAmount;

	int Paused;

	void* Func;
	void* Userdata;

	void* Parents;
	void* Children;
} Task;

typedef void(*TaskFunc)(Task* T);

void task_setup();
void task_step();

Task* task_create(TaskFunc Func, unsigned long Delay);
Task* task_create_linear(TaskFunc Func, unsigned long Interval, unsigned long Steps);
void task_destroy(Task* T);

Task* task_delay_for(Task* T, unsigned long Delay);
Task* task_loop(Task* T, int Count);

#endif