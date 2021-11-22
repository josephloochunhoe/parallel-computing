/*
To run Code:
compile with: "mpicc test.c -lm -o out"
run with: "mpirun -oversubscribe -np 10  out 3 3"
note number of processes must be +1 expected grid size as in the above example.
a 3*3 grid needs 9 processes so the program requires 10.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <signal.h>

#include "station.h"
#include "sensor.h"


pthread_mutex_t g_Mutex = PTHREAD_MUTEX_INITIALIZER;
MPI_Datatype sensorData_t;
float threshold = 6000;
int nsim = 10;

/********************************************************************************************************************************
********************************************************************************************************************************/
int main(int argc, char **argv)
{
	MPI_Init(&argc, &argv);

	MPI_Datatype type[8] = { MPI_INT, MPI_INT, MPI_INT, MPI_FLOAT, MPI_INT, MPI_DOUBLE, MPI_INT, MPI_INT};
	int blocklen[8] = {5,5,5,5,1,1,1,6};
	MPI_Aint disp[8];
	disp[0] = offsetof(struct sensorData, rank);
	disp[1] = offsetof(struct sensorData, coordi);
	disp[2] = offsetof(struct sensorData, coordj);
	disp[3] = offsetof(struct sensorData, value);
	disp[4] = offsetof(struct sensorData, matches);
	disp[5] = offsetof(struct sensorData, comm_start);
	disp[6] = offsetof(struct sensorData, edge);
	disp[7] = offsetof(struct sensorData, date);
	// Create MPI struct
	MPI_Type_create_struct(8, blocklen, disp, type, &sensorData_t);
	MPI_Type_commit(&sensorData_t);

	//initialise variables
    int rank, world_size, sensor_size, nrows=0, ncols=0;
	int dims[NDIMS];
    MPI_Comm new_comm;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_split( MPI_COMM_WORLD,rank < world_size-1, 0, &new_comm);
	MPI_Comm_size(new_comm, &sensor_size);
	pthread_mutex_init(&g_Mutex, NULL);

	//allows user to define m*n grid of sensors via cmdline input
	if (argc >= 3) {
		nrows = atoi (argv[1]);
		ncols = atoi (argv[2]);
		dims[0] = nrows; /* number of rows */
		dims[1] = ncols; /* number of columns */
		if( rank==0) printf("Grid dimensions: [%d x %d]\n", dims[0],dims[1]);
	} else{
		if( rank==0) printf("Grid dimensions not defined\n");
	}
	//mpi is free to set grid dimensions if no user input
	if ((nrows*ncols) != sensor_size) {
		dims[0] = 0;
		dims[1] = 0;
		MPI_Dims_create(sensor_size, NDIMS, dims);
		if( rank==0) printf("Setting grid dimensions: [%d x %d]\n", dims[0],dims[1]);
	}

	if (argc >= 4){
		threshold = atof(argv[3]);
		if( rank==0) printf("Setting water threshold to %.3f metres\n", threshold);
	}else{
		if( rank==0) printf("Water threshold is at %.3f metres\n", threshold);
	}

	if (argc >= 5){
		nsim = atoi(argv[4]);
		if( rank==0) printf("Setting number of iterations to %d\n", nsim);
	}else{
		if( rank==0) printf("Number of iterations at %d\n", nsim);
	}

    if (rank == world_size-1){
		station_func( MPI_COMM_WORLD, new_comm );
	}else if (rank < world_size-1)
		sensor_func( MPI_COMM_WORLD, new_comm, dims );

    MPI_Finalize();
    return 0;
}

