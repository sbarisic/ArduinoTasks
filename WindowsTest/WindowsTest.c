#include <stdio.h>

#include "WProgram.h"
#include "ArduinoTasks.h"

#pragma comment(lib, "ArduinoTasks.lib")
#pragma comment(lib, "WiringStub.lib")

Task TaskA;
Task TaskB;
Task KillTask;

void A(void* Arg) {
	int i;
	for (i = 0; i < 10; i++) {
		printf("    A! (Arg = %i)\n", Arg);
		task_delay(160);
	}
}

void B(void* Arg) {
	int i;
	for (i = 0; i < 20; i++) {
		printf("    B! (Arg = %i)\n", Arg);
		task_delay(100);
	}
}


int main(int argc, const char** argv) {
	int TopOfStack = 0;
	task_init(&TopOfStack, 1024);

	task_create(&TaskA, A, 69);
	task_create(&TaskB, B, 420);

	while (task_get_active_count() > 0)
		task_step();

	printf("Done!\n");
	while (1)
		;
}