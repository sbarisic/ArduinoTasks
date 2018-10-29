#include "WProgram.h"

#include "ArduinoTasks.h"

Task* CurTask;
Task* Tasks[32];

jmp_buf TaskScheduler;

static void* Temp;
static char* OriginalTopOfStack;
static char* TopOfStack;
static int AllowedStackSize;

#define NUM_TASKS ((int)(sizeof(Tasks) / sizeof(*Tasks)))

void task_init(void* TOS, int StackSize) {
	size_t i;

	for (i = 0; i < NUM_TASKS; i++)
		Tasks[i] = NULL;

	AllowedStackSize = StackSize;
	OriginalTopOfStack = TopOfStack = TOS;
	Temp = NULL;
	CurTask = NULL;
}

int task_create(Task* T, TaskFunc F, void* Arg) {
	int FreeTaskNum = -1;
	size_t i;

	for (i = 0; i < NUM_TASKS; i++) {
		if (Tasks[i] == NULL) {
			FreeTaskNum = i;
			break;
		}
	}

	if (FreeTaskNum == -1)
		return 1;

	Tasks[FreeTaskNum] = T;
	T->NextRunTime = 0;
	T->HasTaskStarted = 0;
	T->Func = F;
	T->TaskStartArg = Arg;

	return 0;
}

int task_kill(Task* T) {
	size_t i;

	for (i = 0; i < NUM_TASKS; i++) {
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

	if (setjmp(CurTask->TaskState))
		return;

	longjmp(TaskScheduler, 1);
}

void task_delay(int ms) {
	if (CurTask == NULL)
		return;

	CurTask->NextRunTime = millis() + ms;
	task_yield();
}

void task_internal_continue(Task* T) {
	if (T == NULL)
		return;

	CurTask = T;

	if (setjmp(TaskScheduler))
		return;

	if (!T->HasTaskStarted) {
		// Start Stack Hack
		TopOfStack += -AllowedStackSize;
		printf("Allocating stack, new offset %i\n", (int)(TopOfStack - OriginalTopOfStack));
		char N[-(TopOfStack - (char*)&T)];
		Temp = N;
		// End Stack Hack

		T->HasTaskStarted = 1;
		T->Func(T->TaskStartArg);

		TopOfStack -= -AllowedStackSize;
		printf("Freeing stack, new offset %i\n", (int)(TopOfStack - OriginalTopOfStack));

		CurTask = NULL;
		task_kill(T);
		return;
	}

	longjmp(T->TaskState, 1);
	printf("UNREACHABLE CODE!\n");
	return;
}

int task_get_active_count() {
	int Active = 0;
	size_t i;

	for (i = 0; i < NUM_TASKS; i++) {
		if (Tasks[i] != NULL)
			Active++;
	}

	return Active;
}

int task_step() {
	int ExecutedAny = 0;
	size_t i;



	for (i = 0; i < NUM_TASKS; i++) {
		if (Tasks[i] == NULL)
			continue;

		if (Tasks[i]->NextRunTime <= millis()) {
			ExecutedAny = 1;
			task_internal_continue(Tasks[i]);
		}
	}

	return ExecutedAny;
}