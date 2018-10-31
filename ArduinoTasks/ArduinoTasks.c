#include "WProgram.h"
#include "ArduinoTasks.h"

#include <stdio.h>
//#define DBG_FUNC printf("%s\n", __func__)
#define DBG_FUNC

char UsedTasks[MAX_TASKS];
Task TaskList[MAX_TASKS];
Task* CurrentTask;

void task_setup() {
	DBG_FUNC;

	size_t i;

	for (i = 0; i < MAX_TASKS; i++) {
		TaskList[i].TaskIndex = i;
		task_destroy(&TaskList[i]);
	}
}

Task* task_find_free_child(Task* Root) {
	DBG_FUNC;

	if (Root->Children != NULL)
		return task_find_free_child(Root->Children);

	return Root;
}

void task_attach_child(Task* Root, Task* Child) {
	DBG_FUNC;

	Task* FreeChild = task_find_free_child(Root);
	FreeChild->Children = Child;
	Child->Parents = FreeChild;
}

void task_unlink(Task* T) {
	Task* C = T->Children;
	Task* P = T->Parents;

	if (C != NULL)
		C->Parents = P;

	if (P != NULL)
		P->Children = C;
}

void task_destroy_children(Task* Root) {
	DBG_FUNC;

	Task* Child = Root->Children;

	if (Child != NULL) 
		task_destroy_children(Child);

	task_destroy(Root);
}

Task* task_create(TaskFunc Func, unsigned long Delay) {
	DBG_FUNC;

	size_t i;

	for (i = 0; i < MAX_TASKS; i++) {
		if (UsedTasks[i] == 0) {
			UsedTasks[i] = 1;

			Task* T = &TaskList[i];
			task_delay_for(T, Delay);

			T->Func = Func;
			T->TaskIndex = i;

			T->TriggerCount = 0;
			T->MaxTriggerCount = 1;
			T->LoopDelay = Delay;

			T->TweenAmount = 100;
			T->InvTweenAmount = 0;
			T->LastTweenAmount = -1;

			T->Paused = 0;
			T->Children = NULL;

			if (CurrentTask != NULL)
				task_attach_child(CurrentTask, T);

			return T;
		}
	}

	return NULL;
}

Task* task_create_linear(TaskFunc Func, unsigned long Interval, unsigned long Steps) {
	DBG_FUNC;

	Task* T = task_create(Func, 0);
	T->LoopDelay = Interval / Steps;
	T->MaxTriggerCount = Steps;
	return T;
}

Task* task_delay_for(Task* T, unsigned long Delay) {
	DBG_FUNC;

	if (T == NULL)
		return NULL;

	if (Delay == 0)
		T->NextRunTime = 0;
	else
		T->NextRunTime = millis() + Delay;

	return T;
}

void task_destroy(Task* T) {
	DBG_FUNC;

	if (T == NULL || T->TaskIndex < 0)
		return;

	task_unlink(T);

	if (CurrentTask == T)
		CurrentTask = NULL;

	if (T->Children != NULL)
		task_destroy_children(T->Children);

	UsedTasks[T->TaskIndex] = 0;
	size_t i;

	for (i = 0; i < sizeof(Task); i++)
		((char*)T)[i] = 0;

	T->TaskIndex = -1;
}

Task* task_loop(Task* T, int Count) {
	DBG_FUNC;

	T->MaxTriggerCount = Count;
	return T;
}

void task_step() {
	size_t i;

	for (i = 0; i < MAX_TASKS; i++) {
		if (UsedTasks[i] && TaskList[i].NextRunTime <= millis()) {
			CurrentTask = &TaskList[i];

			if (!CurrentTask->Paused) {
				if (CurrentTask->MaxTriggerCount == -1 || CurrentTask->TriggerCount < CurrentTask->MaxTriggerCount) {
					if (CurrentTask->MaxTriggerCount != -1) {
						CurrentTask->LastTweenAmount = CurrentTask->TweenAmount;
						CurrentTask->TweenAmount = (int)((100.0f / CurrentTask->MaxTriggerCount) * (CurrentTask->TriggerCount + 1));
						CurrentTask->InvTweenAmount = 100 - CurrentTask->TweenAmount;
					} else
						CurrentTask->LastTweenAmount = CurrentTask->TweenAmount = CurrentTask->InvTweenAmount = -1;

					((TaskFunc)TaskList[i].Func)(CurrentTask);

					if (CurrentTask != NULL) {
						CurrentTask->TriggerCount++;
						CurrentTask->NextRunTime = millis() + (CurrentTask->LoopDelay + CurrentTask->NextDelayOffset);
						CurrentTask->NextDelayOffset = 0;
					}
				} else
					task_destroy(CurrentTask);
			}

			CurrentTask = NULL;
		}
	}
}