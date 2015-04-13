#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include "test_scan.h"

int main(int argc, char *argv[])
{
    int  namelen, rank, size, newrank, newsize, color, key, i, j, error;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    int halfsize;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Get_processor_name(processor_name, &namelen);

    fprintf(stderr, "Process %d of %d on %s\n", rank, size, processor_name);

    fprintf(stderr, "Starting SCAN tests\n");

    error = test_scan(MPI_COMM_WORLD, "MPI_COMM_WORLD");

    if (error != 0) {
    	fprintf(stderr, "FAILED error=%d\n", error);
    	return 1;
    }

    fprintf(stderr, "Done!\n");
    MPI_Finalize();

    return 0;
}

