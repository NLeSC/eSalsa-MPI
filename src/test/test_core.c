#include <stdio.h>
#include "mpi.h"

int core()
{
    int *ptr = NULL;
    // Test if signal handler works...
    *ptr = 42;

    return 42;
}

int bar()
{
	return core();
}

int foo()
{
	return bar();
}

int main(int argc, char *argv[])
{
    int rank, size, error;
    int *ptr = NULL;

    error = MPI_Init(&argc, &argv);

    if (error != MPI_SUCCESS) {
       fprintf(stderr, "Init failed! %d\n", error);
    }

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    fprintf(stderr, "Process %d of %d\n", rank, size);

    // Test if signal handler works...
    foo();

    MPI_Finalize();

    return 0;
}

