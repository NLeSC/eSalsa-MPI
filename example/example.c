/*
 * A simple example application that only initialize MPI and then exists.
 *
 */
#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
    int rank, size, error;

    error = MPI_Init(&argc, &argv);

    if (error != MPI_SUCCESS) {
       fprintf(stderr, "Init failed! %d\n", error);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    fprintf(stderr, "Process %d of %d\n", rank, size);

    MPI_Finalize();

    return 0;
}

