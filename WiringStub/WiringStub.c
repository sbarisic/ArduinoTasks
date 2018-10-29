#include "WProgram.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/timeb.h>
#include <time.h>

struct __timeb32* start = NULL;

int millis() {
	if (start == NULL) {
		start = malloc(sizeof(struct __timeb32));
		ftime(start);
	}

	struct __timeb32 end;
	ftime(&end);

	return (int)(1000.0 * (end.time - start->time) + (end.millitm - start->millitm));
}

void delay(int ms) {
	int ResumeTime = millis() + ms;

	while (ResumeTime >= millis())
		;
}