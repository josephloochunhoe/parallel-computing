#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <signal.h>
#include "station.h"
#include "sensor.h"
#include "satellite.h"
#include "check_sentinel.h"


//create a global array for satellite
struct satVals sat_arr[100];
int terminate = 0;

// obtaining global variables from main function
extern pthread_mutex_t g_Mutex;
extern MPI_Datatype sensorData_t;
extern int nsim;

int station_func(MPI_Comm world_comm, MPI_Comm sensor_comm)
{	
	//creating satellite thread
	struct satArgs *sat_args = (struct satArgs *)malloc(sizeof(struct satArgs));
	sat_args -> world_comm = world_comm;
	sat_args -> sensor_comm = sensor_comm;
	pthread_t tid;
	pthread_create(&tid, 0, satellite_func, (void*) sat_args);
	
	//creating thread to read from a file for a sentinel value
	pthread_t tid1;
	pthread_create(&tid1, 0, check_sentinel, (void*) sat_args);

	int size;
	MPI_Status status;
	MPI_Comm_size(world_comm, &size );
	struct sensorData recvData;
	fflush(stdout);
	FILE  *outFile;
	outFile = fopen("log.txt", "w"); 
	int nAlerts=0, nMatches=0, nMidAlerts=0, nMidMatches=0, nEdgeAlerts=0, nEdgeMatches=0;
	double world_comm_begin = MPI_Wtime();

	for(int j=1; j < nsim+1; j++){
		if (terminate) {
			break;
		}
		//receives any incoming reports from the sensor nodes
		MPI_Recv(&recvData, 1, sensorData_t, MPI_ANY_SOURCE, MPI_ANY_TAG, world_comm, &status);
		switch(status.MPI_TAG){
			case MSG_ALERT:
			{
				pthread_mutex_lock(&g_Mutex); // stop the sat_arr from updating
				struct satVals satValue = sat_arr[recvData.rank[0]];
				pthread_mutex_unlock(&g_Mutex);

				nAlerts++;

				//logging for each iteration
				time_t rawtime;
				struct tm * logtime;
				rawtime = time ( NULL );
				logtime = localtime ( &rawtime );
				double comm_end = MPI_Wtime();
				double comm_time = comm_end - recvData.comm_start;

				fprintf(outFile, "Iteration: %d\n", j);
				fprintf(outFile, "Logged time: %02d-%02d-%04d %02d:%02d:%02d\n",logtime->tm_mday,logtime->tm_mon +1,logtime->tm_year+1900,logtime->tm_hour,logtime->tm_min,logtime->tm_sec);
				fprintf(outFile, "Alert reported time:  %02d-%02d-%02d %02d:%02d:%02d\n", recvData.date[0], recvData.date[1], recvData.date[2], recvData.date[3], recvData.date[4], recvData.date[5]);
				if (recvData.value[0] >= (satValue.value-SAT_TOL) && recvData.value[0] <= (satValue.value+SAT_TOL)) {
					fprintf(outFile, "Alert type: Match\n\n");
					nMatches++;
					(recvData.edge == 1)? nEdgeMatches++ :nMidMatches++;
				} else {
					fprintf(outFile, "Alert type: Mismatch\n\n");
				}

				fprintf(outFile, "Reporting Node\t\tCoord\t\tHeight(m)\t\n");
				fprintf(outFile,"%d\t\t\t\t\t(%d,%d)\t\t%.3f\n", recvData.rank[0], recvData.coordi[0], recvData.coordj[0], recvData.value[0]);
				fprintf(outFile, "\nAdjacent Nodes\t\tCoord\t\tHeight(m)\t\n");
				for (int adj=1; adj<5; adj++){
					if (recvData.rank[adj]>=0) fprintf(outFile,"%d\t\t\t\t\t(%d,%d)\t\t%.3f\n", recvData.rank[adj], recvData.coordi[adj], recvData.coordj[adj], recvData.value[adj]);
				}    

				fprintf(outFile, "\nSatellite altimeter reporting time: %02d-%02d-%02d %02d:%02d:%02d\n", satValue.date[0], satValue.date[1], satValue.date[2], satValue.date[3], satValue.date[4], satValue.date[5]);
				fprintf(outFile, "Satellite altimeter reporting height (m): %.3f\n", satValue.value);
				fprintf(outFile, "Satellite altimeter reporting Coord : (%d,%d)\n", recvData.coordi[0], recvData.coordj[0]);

				fprintf(outFile, "\nCommunication time (s): %f\n",comm_time);
				fprintf(outFile, "Number of adjecent matches betweeen reporting node: %d\n", recvData.matches);
				fprintf(outFile, "Max. tolerance range between nodes readings (m): %.3d\n",SEN_TOL);
				fprintf(outFile, "Max. tolerance range between satellite altimeter and reporting node readings (m):  %.3d\n",SAT_TOL);

				if (recvData.edge == 1){
					fprintf(outFile, "\nReporting node is at the edge of the grid.\n");
					nEdgeAlerts++;
				} else {
					fprintf(outFile, "\nReporting node is in the middle of the grid.\n");
					nMidAlerts++;
				}

				fprintf(outFile, "\n======================================================================================\n\n");
				
				break;
			}
			default:
			{
				break;
			}
		}
	}
	//logging for summary of whole simulation
	double world_comm_end = MPI_Wtime();
	double world_comm_time = world_comm_end-world_comm_begin;
	fprintf(outFile, "Log Summary\n");
	fprintf(outFile, "Total number of alerts: %d\n",nAlerts);
	fprintf(outFile, "Total number of matches: %d\n",nMatches);
	fprintf(outFile, "Total number of network communication requests: %d\n",nAlerts*4);
	fprintf(outFile, "Total communication time (s): %f\n",world_comm_time);

	fprintf(outFile, "Middle Node\nAlerts: %d, Matches %d, Mismatches %d\n", nMidAlerts, nMidMatches, nMidAlerts-nMidMatches);
	fprintf(outFile, "Edge Node\nAlerts: %d, Matches %d, Mismatches %d\n", nEdgeAlerts, nEdgeMatches, nEdgeAlerts-nEdgeMatches);

	//send termination signal to all of the sensor nodes
	fclose(outFile);
	for (int i=0; i < size-1; i++) {
		MPI_Send(&i, 1, MPI_INT, i, 1, world_comm);
	}
	//send termination signal to terminate the satellite thread
	pthread_kill(tid, SIGTERM);
	pthread_kill(tid1, SIGTERM);

    return 0;
}
