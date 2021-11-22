#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include "station.h"
#include "satellite.h"

extern struct satVals sat_arr[100];
extern float threshold;

void randomize_q(int* arr, int n);

void* satellite_func(void *pArg)
{	
	int world_size;
	MPI_Comm world_comm = ((struct satArgs*)pArg)->world_comm;
	MPI_Comm_size(world_comm, &world_size);
	int sensor_size = world_size-1;
	//create a random order to fill up sat array
	int sensor_ord[sensor_size];
	srand((unsigned int)time(NULL));
	randomize_q(sensor_ord, sensor_size); 
	float max = 1000.0;	

	//fill up global array with random values above threshold at random coords
	//replace the data FIFO
	while(1){
		for (int i=0; i<sensor_size; i++){
			time_t rawtime;
			struct tm * timeinfo;
			time ( &rawtime );
			timeinfo = localtime ( &rawtime );
			float level = threshold;
			level = level + (((float)rand()/(float)(RAND_MAX)) * max);
			int rank = sensor_ord[i];
			sat_arr[rank].value = level;
			sat_arr[rank].date[0] = timeinfo->tm_mday;
			sat_arr[rank].date[1] = timeinfo->tm_mon +1;
			sat_arr[rank].date[2] = (timeinfo->tm_year+1900);
			sat_arr[rank].date[3] = timeinfo->tm_hour;
			sat_arr[rank].date[4] = timeinfo->tm_min;
			sat_arr[rank].date[5] = timeinfo->tm_sec;
		}
		sleep(1);
	}
	pthread_exit(0);
}

//populates an array up to n indexes in a random order
void randomize_q(int* arr, int n){
	for (int i=0; i<n; i++){
		arr[i] = i;
	}
	srand(time(NULL)*n);
	for (int i=0; i<n-1; i++){
		int j = rand()%(n-1);
		int temp = arr[j];
		arr[j] = arr[i];
		arr[i] = temp;
	}
}