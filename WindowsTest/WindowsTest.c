#include <stdio.h>

#include "WProgram.h"
#include "ArduinoTasks.h"

#include "Debug.h"

#pragma comment(lib, "ArduinoTasks.lib")
#pragma comment(lib, "WiringStub.lib")

#include <stdlib.h>

typedef struct {
	int BlinkerPWM;
	int BlinkerState;
} BlinkerInfo;

Task* TaskA;
Task* TaskB;
Task* TaskC;

void C(Task* T) {
	printf("----C\n");

	if (T->TriggerCount >= 1)
		task_destroy(TaskA);
}

void B(Task* T) {
	printf("--B\n");

	TaskC = task_loop(task_create(C, 100), 3);
}

void A(Task* T) {
	printf("A\n");

	TaskB = task_loop(task_create(B, 500), 2);
}

void setup() {
	task_setup();

	TaskA = task_loop(task_create(A, 2000), 2);
}

void loop() {
	task_step();
}

int main(int argc, const char** argv) {
	DebugInit();
	setup();

	while (1)
		loop();
}