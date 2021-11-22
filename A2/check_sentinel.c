#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <pthread.h>
#include "station.h"
#include "satellite.h"

extern int terminate;

void* check_sentinel(void *pArg)
{	
	char buffer[256];
	int found = 0;
	char sentinel[] = "terminate\n";

	FILE *file;
	while (!found) {
		file = fopen("sentinel.txt", "r");
		if (file) {
			fgets(buffer, 256, file);
			if (strcmp(buffer, sentinel)==0) {
				terminate = 1;
				found = 1;
			}
		}
	}
	fclose(file);
	return 0;
}