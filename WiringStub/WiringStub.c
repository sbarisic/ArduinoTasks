#include "WProgram.h"

#include <stdio.h>
#include <sys/timeb.h>

struct timeb* start = NULL;

int millis() {
	if (start == NULL) {
		start = malloc(sizeof(struct timeb));
		ftime(start);
	}

	struct timeb end;
	ftime(&end);

	return (int)(1000.0 * (end.time - start->time) + (end.millitm - start->millitm));
}

void delay(int ms) {
	int ResumeTime = millis() + ms;

	while (ResumeTime >= millis())
		;
}