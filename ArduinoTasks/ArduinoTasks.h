#ifndef _ARDUINO_TASKS_H
#define _ARDUINO_TASKS_H

#include "WConstants.h"

typedef void(*TaskFunc)(void* Arg);

typedef struct {
	int NextRunTime;

	TaskFunc Func;
} Task;

void task_init();

int task_create(Task* T, TaskFunc F);
int task_kill(Task* T);

void task_yield();
void task_delay(int ms);

Task* task_get_current();

void task_begin_loop();

#endif