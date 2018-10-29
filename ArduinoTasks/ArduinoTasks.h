#ifndef _ARDUINO_TASKS_H
#define _ARDUINO_TASKS_H

#include "WConstants.h"

typedef void(*TaskFunc)(void* Arg);

typedef struct {
	int NextRunTime;
	int HasTaskStarted;

	jmp_buf TaskState;

	void* TaskStartArg;
	TaskFunc Func;
} Task;

void task_init(void* TOS, int StackSize);

int task_create(Task* T, TaskFunc F, void* Arg);
int task_kill(Task* T);

void task_yield();
void task_delay(int ms);

Task* task_get_current();

int task_get_active_count();
int task_step();

// Schedules

#endif