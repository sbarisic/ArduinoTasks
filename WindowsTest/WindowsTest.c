#include <stdio.h>

#include "WProgram.h"
#include "ArduinoTasks.h"

#pragma comment(lib, "ArduinoTasks.lib")
#pragma comment(lib, "WiringStub.lib")

Task TaskA;
Task TaskB;
Task KillTask;

void A(void* Arg) {
	while (1) {
		printf("Hello A!\n");
		task_delay(1600);
	}
}

void B(void* Arg) {
	while (1) {
		printf("Hello B!\n");
		task_delay(1000);
	}
}

void Kill(void* Arg) {
	task_delay(5000);
	task_kill(&TaskA);
	task_kill(&TaskB);
}

int main(int argc, const char** argv) {
	task_init();

	task_create(&TaskA, A);
	task_create(&TaskB, B);
	task_create(&KillTask, Kill);

	task_begin_loop();
}