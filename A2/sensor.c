#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include "sensor.h"

extern MPI_Datatype sensorData_t;
extern float threshold;

int sensor_func(MPI_Comm world_comm, MPI_Comm sensor_comm, int* dims)
{
	int world_size, size, my_rank, reorder, my_cart_rank, ierr;
	int iter=0;
	MPI_Comm comm2D;
	int coord[NDIMS];
	int wrap_around[NDIMS];    
	
	//create cartesian grid
	MPI_Comm_size(world_comm, &world_size);
	MPI_Comm_size(sensor_comm, &size);
	MPI_Comm_rank(sensor_comm, &my_rank);
	MPI_Dims_create(size, NDIMS, dims);
	wrap_around[0] = 0;
	wrap_around[1] = 0;
	reorder = 0;
	ierr = 0;
	ierr = MPI_Cart_create(sensor_comm, NDIMS, dims, wrap_around, reorder, &comm2D);
	if(ierr != 0) printf("ERROR[%d] creating CART\n",ierr);
	MPI_Cart_coords(comm2D, my_rank, NDIMS, coord);
	MPI_Cart_rank(comm2D, coord, &my_cart_rank);
	
	//sensors can communicate with adjacent sensors
	srand((unsigned int)time(NULL)*my_rank);
	//calculate the moving average
	float level_total = 0;
	float level_avg;
	float max = 1000.00;	

	//sensors communcate to the station via sattelite
	int adj_rank[5]; //cur,adjT,adjB,adjL,adjR
	adj_rank[0] = my_cart_rank;
	float adj_val[5]={-1,-1,-1,-1,-1};
	MPI_Cart_shift( comm2D, SHIFT_ROW, DISP, &adj_rank[1], &adj_rank[2] );
	MPI_Cart_shift( comm2D, SHIFT_COL, DISP, &adj_rank[3], &adj_rank[4] );
	MPI_Request send_request[4];
	MPI_Request receive_request[4];
	MPI_Status send_status[4];
	MPI_Status receive_status[4];

	struct sensorData data;	
	while(1){
		//tries to receive a termination signal from base station
		int broadcast_received = 0;
		int flag=0;
		MPI_Status status;
		MPI_Request request;
		MPI_Irecv(&broadcast_received, 1, MPI_INT, world_size-1, MPI_ANY_TAG, world_comm, &request);
		MPI_Test(&request, &flag,&status);
		if (flag == 1) if (status.MPI_TAG == 1)break;

		//generate random floats values to simulate sea level
		float level =  threshold-500;
		level = level + (((float)rand()/(float)(RAND_MAX)) * max);
		level_total = level_total + level;
		level_avg = level_total/(iter+1);

		//sends moving average to its adjacent nodes and receives their moving averages
		adj_val[0] = level_avg;
		for (int adj = 1; adj<=4; adj++){
			MPI_Isend(&level_avg, 1, MPI_FLOAT, adj_rank[adj], 0, comm2D, &send_request[adj-1]);
			MPI_Irecv(&adj_val[adj], 1, MPI_FLOAT, adj_rank[adj], 0, comm2D, &receive_request[adj-1]);
		}

		MPI_Waitall(4, send_request, send_status);
		MPI_Waitall(4, receive_request, receive_status);

		//if avg >threshold, check adj nodes. alert station when 2 or more adj nodes match the reading (+-100m)
		if (level_avg > threshold){
			time_t rawtime;
			struct tm * timeinfo;
			rawtime = time ( NULL );
			timeinfo = localtime ( &rawtime );
			int count = 0;
			int edge = 0;
			for (int adj = 1; adj<=4; adj++){
				if (adj_val[adj]>=(level_avg-SEN_TOL) && adj_val[adj]<=(level_avg+SEN_TOL)) count++;
				if (adj_rank[adj]<0) edge = 1;
			}
			if (count>=2){
				//send an alert to station 
				for (int adj = 0; adj<5; adj++){
					if (adj_rank[adj]>=0){
						int temp_coords[NDIMS];
						MPI_Cart_coords(comm2D,  adj_rank[adj], NDIMS, temp_coords);
						data.rank[adj] = adj_rank[adj];
						data.coordi[adj]=temp_coords[0];
						data.coordj[adj]=temp_coords[1];
						data.value[adj]= adj_val[adj];
					} else {
						data.rank[adj]=-1;
						data.coordi[adj]=-1;
						data.coordj[adj]=-1;
						data.value[adj]=-1;
					}
				}
				data.matches = count;
				data.date[0] = timeinfo->tm_mday;
				data.date[1] = timeinfo->tm_mon +1;
				data.date[2] = timeinfo->tm_year+1900;
				data.date[3] = timeinfo->tm_hour;
				data.date[4] = timeinfo->tm_min;
				data.date[5] = timeinfo->tm_sec;
				data.comm_start = MPI_Wtime();
				data.edge = edge;
				MPI_Send(&data, 1, sensorData_t, world_size-1, MSG_ALERT, world_comm);
			}
		} 
		iter++;
		//setting a cycle of 1 second
		sleep(1);
	}
	MPI_Finalize();
	MPI_Comm_free( &comm2D );
	return 0;
}
