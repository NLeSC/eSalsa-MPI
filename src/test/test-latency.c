#include <stdio.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mpi.h"

#define REPEAT 10
#define COUNT 1000

static uint64_t current_time_micros()
{
   uint64_t result;
   struct timeval t;

   gettimeofday(&t,NULL);

   result = (t.tv_sec * 1000000LU) + t.tv_usec;

   return result;
}

int main(int argc, char *argv[])
{
	uint64_t start, end;
    int rank, size, i, r, error, msg;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (size != 2) {
    	fprintf(stderr, "ERROR: need 2 nodes for simple latency test!\n");
    	MPI_Finalize();
    	return 1;
    }

    printf("Process %d of %d\n", rank, size);

    if (rank == 0) {

    	for (r=0;r<REPEAT;r++) {

    		msg = 42;

    		start = current_time_micros();

    		for (i=0;i<COUNT;i++) {
    			error = MPI_Send(&msg, 1, MPI_INTEGER, 1, i, MPI_COMM_WORLD);

    			if (error != MPI_SUCCESS) {
    				fprintf(stderr, "Send failed! %d\n", error);
    		    	MPI_Finalize();
    				return 1;
    			}

    			error = MPI_Recv(&msg, 1, MPI_INTEGER, 1, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    			if (error != MPI_SUCCESS) {
    				fprintf(stderr, "Receive failed! %d\n", error);
    				MPI_Finalize();
    				return 1;
    			}
    		}

    		end = current_time_micros();

    		printf("Test %d took %ld usec, rtt = %f usec\n", r, (end-start), (end-start)/(double)COUNT);
    	}

    } else {

    	for (r=0;r<REPEAT;r++) {

    		msg = 42;

    		for (i=0;i<COUNT;i++) {
    			error = MPI_Recv(&msg, 1, MPI_INTEGER, 0, i, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

    			if (error != MPI_SUCCESS) {
    				fprintf(stderr, "Receive failed! %d\n", error);
    				MPI_Finalize();
    				return 1;
    			}

    			error = MPI_Send(&msg, 1, MPI_INTEGER, 0, i, MPI_COMM_WORLD);

    			if (error != MPI_SUCCESS) {
    				fprintf(stderr, "Send failed! %d\n", error);
    				MPI_Finalize();
    				return 1;
    			}
    		}
    	}
    }

    printf("Done!\n");

    MPI_Finalize();

    return 0;
}

