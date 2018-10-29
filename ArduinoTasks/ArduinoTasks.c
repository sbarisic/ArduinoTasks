#include "WProgram.h"

#include "ArduinoTasks.h"

int TaskNum;
Task* CurTask;
Task* Tasks[32];

#define NUM_TASKS (sizeof(Tasks) / sizeof(*Tasks))

void task_init() {
	for (size_t i = 0; i < NUM_TASKS; i++)
		Tasks[i] = NULL;

	TaskNum = -1;
	CurTask = NULL;
}

int task_create(Task* T, TaskFunc F) {
	if (TaskNum >= NUM_TASKS)
		return 1;

	Tasks[++TaskNum] = T;
	return 0;
}

int task_kill(Task* T) {
	for (size_t i = 0; i < NUM_TASKS; i++) {
		if (Tasks[i] == T) {
			Tasks[i] = NULL;
			return 0;
		}
	}

	return 1;
}

Task* task_get_current() {
	return CurTask;
}

void task_yield() {
	if (CurTask == NULL)
		return;

	// setjmp/longjmp here
}

void task_delay(int ms) {
	if (CurTask == NULL)
		return;

	CurTask->NextRunTime = millis() + ms;
	task_yield();
}

void task_begin_loop() {

}