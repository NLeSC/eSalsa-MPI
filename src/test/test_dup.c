#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
    int rank, size, error, comm;

    error = MPI_Init(&argc, &argv);

    if (error != MPI_SUCCESS) {
       fprintf(stderr, "Init failed! %d\n", error);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    fprintf(stderr, "(WORLD) Process %d of %d\n", rank, size);

    error = MPI_Comm_dup(MPI_COMM_WORLD, &comm);

    if (error != MPI_SUCCESS) {
    	fprintf(stderr, "Dup failed! %d\n", error);
    }

    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);

    fprintf(stderr, "(DUP) Process %d of %d\n", rank, size);

    MPI_Finalize();

    return 0;
}

