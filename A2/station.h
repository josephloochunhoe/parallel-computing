#define SAT_TOL 100

int station_func(MPI_Comm world_comm, MPI_Comm sensor_comm);

struct satVals{
	float value;
	int date[6];
};

struct satArgs{
	MPI_Comm world_comm;
	MPI_Comm sensor_comm;
};
