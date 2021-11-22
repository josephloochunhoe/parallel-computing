#define SHIFT_ROW 0
#define SHIFT_COL 1
#define MSG_ALERT 2
#define SEN_TOL 100
#define NDIMS 2
#define DISP 1

int sensor_func(MPI_Comm world_comm, MPI_Comm sensor_comm, int *dims);

struct sensorData{
	int rank[5]; //cur,adjT,adjB,adjL,adjR
	int coordi[5];
	int coordj[5];
	float value[5];
	int matches;
	double comm_start;
	int edge;
	int date[6]; //day, month, year, hr, min, ss
};

