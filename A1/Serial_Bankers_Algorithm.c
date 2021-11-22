// Banker's Algorithm
#include <stdio.h>
#include <stdlib.h>
#include <time.h>       // for clock_t, clock(), CLOCKS_PER_SEC
#include <unistd.h>

#define PROCESS_NUM 5
#define RESOURCE_NUM 4

// Function prototype
int safety(int alloc[PROCESS_NUM][RESOURCE_NUM], int avail[RESOURCE_NUM], 
int max[PROCESS_NUM][RESOURCE_NUM], int need[PROCESS_NUM][RESOURCE_NUM], int ans[PROCESS_NUM] );

void request(int alloc[PROCESS_NUM][RESOURCE_NUM], int avail[PROCESS_NUM][RESOURCE_NUM], int need[PROCESS_NUM][RESOURCE_NUM]);

void final_output(int k[PROCESS_NUM][RESOURCE_NUM]);

int main()
{
	int i, j;
	int ans[PROCESS_NUM], need[PROCESS_NUM][RESOURCE_NUM];
	
	// Arbitrary current state of the resources
	int alloc[PROCESS_NUM][RESOURCE_NUM] = 
					{ {4,0,0,1},
						{1,1,0,0},
						{1,2,5,4},
						{0,6,3,3},
						{0,2,1,2} };

	int max[PROCESS_NUM][RESOURCE_NUM] = 
				{ {6,0,1,2},
					{1,7,5,0},
					{2,3,5,6},
					{1,6,5,3},
					{1,6,5,6}};

	int avail[RESOURCE_NUM] = { 3,2,1,2 }; // Available Resources

	// Find need matrix
	for (i = 0; i < PROCESS_NUM; i++) {
		for (j = 0; j < RESOURCE_NUM; j++)
			need[i][j] = max[i][j] - alloc[i][j];
	}


	printf("\n\n***********Need Matrix*********");
	final_output(need);

	// Make a process request for additional resources
	request(alloc, avail, need);

	printf("\n\n");
	// Check if still in safe state or not
	int r = safety(alloc, avail, max, need, ans);

	if (r == 1) {
		printf("Following is the SAFE Sequence\n");
		for (i = 0; i < PROCESS_NUM - 1; i++)
			printf(" P%d ->", ans[i]+1);
		printf(" P%d", ans[PROCESS_NUM - 1]+1);
	}
	else {
		printf("Deadlock occurred\n");
	}
	return (0);
}

// Safety Algorithm
int safety( int alloc[PROCESS_NUM][RESOURCE_NUM], int avail[RESOURCE_NUM], 
int max[PROCESS_NUM][RESOURCE_NUM], int need[PROCESS_NUM][RESOURCE_NUM], int ans[PROCESS_NUM] ) {
	
	int i, j, k, ind = 0;
	int finish[PROCESS_NUM];
    double time_spent = 0.0;
 
 	// Step 4
	for (k = 0; k < PROCESS_NUM; k++) {
		finish[k] = 0;
	}

    clock_t begin = clock();

    printf("Start time is %f seconds\n", (double) begin / CLOCKS_PER_SEC);
	
	// Step 5
	int safe_processes = 0, y = 0;
	for (k = 0; k < PROCESS_NUM; k++) {
		for (i = 0; i < PROCESS_NUM; i++) {
			if (finish[i] == 0) {

				int canFinish = 1;
				for (j = 0; j < RESOURCE_NUM; j++) {
					if (need[i][j] > avail[j]){
						canFinish = 0;
						break;
					}
				}

				if (canFinish) {
					// To simulate process being executed
					sleep(1);

					ans[ind++] = i;
					for (y = 0; y < RESOURCE_NUM; y++)
						avail[y] += alloc[i][y];
					finish[i] = 1;
					safe_processes += 1;
				}
			}
		}
		if (safe_processes == PROCESS_NUM) {
			break;
		}
	}

    clock_t end = clock();
 
    printf("End time is %f seconds\n", (double) end / CLOCKS_PER_SEC);
    time_spent += ((double)(end - begin)) / CLOCKS_PER_SEC;

    printf("The elapsed time is %f seconds\n", time_spent);
	
	// Step 6
	if (safe_processes == PROCESS_NUM) {
		return 1;
	}
	else {
		return 0;
	}
}


//Resource Request algorithm
void request(int alloc[PROCESS_NUM][RESOURCE_NUM], int avail[PROCESS_NUM][RESOURCE_NUM], int need[PROCESS_NUM][RESOURCE_NUM]) {
	int rmat[1][RESOURCE_NUM];
	int i, pid;

	printf("\n Enter process number making additional request: ");
	scanf("%d", &pid);
	pid -= 1;
	printf("\n Enter additional request : \n");
	for (i = 0; i < RESOURCE_NUM; i++){
		printf(" Request for resource %d : ", i + 1);
		scanf("%d", &rmat[0][i]);
	}

	// Step 1
	for (i = 0; i < RESOURCE_NUM; i++) {
		if (rmat[0][i] > need[pid][i]){
			printf("\n ******Error encountered******\n");
			exit(0);
		}
	}

	// Step 2
	for (i = 0; i < RESOURCE_NUM; i++) {
		if (rmat[0][i] > avail[0][i]) {
			printf("\n ******Resources unavailable*****\n");
			exit(0);
		}
	}

	// Step 3
	for (i = 0; i < RESOURCE_NUM; i++){
		avail[0][i] -= rmat[0][i];
		alloc[pid][i] += rmat[0][i];
		need[pid][i] -= rmat[0][i];
	}
}


void final_output(int k[PROCESS_NUM][RESOURCE_NUM]) {
	int i, j;
	for (i = 0; i < PROCESS_NUM; i++) {
		printf("\n");
		for (j = 0; j < RESOURCE_NUM; j++) {
			printf("%d\t", k[i][j]);
		}
	}

	printf("\n\n");
}