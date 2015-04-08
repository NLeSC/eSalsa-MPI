#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
    int  namelen, rank, size, i, error, j;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int buffer[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    int *recbuffer;

    MPI_Request *requests;
    MPI_Status status;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Get_processor_name(processor_name, &namelen);

    fprintf(stderr, "Process %d of %d on %s\n", rank, size, processor_name);


    if (rank == 0) {

    	recbuffer = malloc(size * sizeof(int) * 10);

        if (recbuffer == NULL) {
           fprintf(stderr, "Failed to allocate receive buffer!");
           return 1;
        }

        for (i=0;i<size*10;i++) {
        	recbuffer[i] = -999;
        }

    	requests = malloc(size * sizeof(MPI_Request));

    	if (requests == NULL) {
    		fprintf(stderr, "Failed to allocate requests!");
    		return 1;
    	}

    	for (i=0;i<size;i++) {
    		error = MPI_Irecv(recbuffer+(i*10), 10, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &requests[i]);

    		if (error != MPI_SUCCESS) {
    			fprintf(stderr, "Failed to post ireceive! %d\n", error);
    			return 1;
    		}
    	}
    }

    for (i=0;i<10;i++) {
    	buffer[i] = rank + i;
    }

    error = MPI_Send(buffer, 10, MPI_INTEGER, 0, 0, MPI_COMM_WORLD);

    if (error != MPI_SUCCESS) {
       fprintf(stderr, "Send failed on %d! %d\n", rank, error);
       return 1;
    }

    if (rank == 0) {

    	for (i=0;i<size;i++) {
    		error = MPI_Wait(&requests[i], &status);

    		if (error != MPI_SUCCESS) {
    			fprintf(stderr, "Wait failed on %d! %d\n", rank, error);
    			return 1;
    		}

    		// check the buffer data
    		for (j=0;j<10;j++) {
    			if (recbuffer[status.MPI_SOURCE*10+j] != status.MPI_SOURCE+j) {
    				fprintf(stderr, "ERROR: Received %d expected %d from %d\n", recbuffer[status.MPI_SOURCE*10+j],
    						status.MPI_SOURCE+j, status.MPI_SOURCE);
    			}
    		}
    	}

    	free(requests);
    	free(recbuffer);
    }

    fprintf(stderr, "Done!\n");

    MPI_Finalize();

    return 0;
}

