#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
    MPI_Comm half;
    MPI_Comm oddeven;
    MPI_Comm three;

    int  namelen, rank, size, newrank, newsize, color, key, i, error;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int buffer[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    MPI_Request *request;
    MPI_Status status;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Get_processor_name(processor_name, &namelen);

    fprintf(stderr, "Process %d of %d on %s\n", rank, size, processor_name);

    if (size != 16) {
        fprintf(stderr, "Need 16 processes for this test\n");
        return 1;
    }

    // Split halfway
    color = rank / 8;
    key = rank % 8;

    fprintf(stderr, "Testing half split: %d %d %d %d\n", rank, size, color, key);

    error = MPI_Comm_split(MPI_COMM_WORLD, color, key, &half);

    if (error != MPI_SUCCESS) {
	fprintf(stderr, "Half split failed!\n");
        MPI_Finalize();
        return 1;
    }

    MPI_Comm_size(half, &newsize);
    MPI_Comm_rank(half, &newrank);

    fprintf(stderr, "Half split succeeded: I am now process %d of %d on %s\n", newrank, newsize, processor_name);

    // Split odd-even
    color = rank / 2;
    key = rank % 2;

    fprintf(stderr, "Testing odd-even split: %d %d %d %d\n", rank, size, color, key);

    error = MPI_Comm_split(MPI_COMM_WORLD, color, key, &oddeven);

    if (error != MPI_SUCCESS) {
	fprintf(stderr, "Odd-even split failed!\n");
        MPI_Finalize();
        return 1;
    }

    MPI_Comm_size(oddeven, &newsize);
    MPI_Comm_rank(oddeven, &newrank);

    fprintf(stderr, "Odd-even split succeeded: I am now process %d of %d on %s\n", newrank, newsize, processor_name);

    // Split in three, the middle 6 don't participate.

    if (rank < 5) {
    	color=0;
	key=rank;
    } else if (rank > 10) {
    	color=1;
	key=rank-10;
    } else {
        color=MPI_UNDEFINED;
        key=rank-5;
    }

    fprintf(stderr, "Testing three way split: %d %d %d %d\n", rank, size, color, key);

    error = MPI_Comm_split(MPI_COMM_WORLD, color, key, &three);

    if (error != MPI_SUCCESS) {
	fprintf(stderr, "Three way split failed!\n");
        MPI_Finalize();
        return 1;
    }

    if (rank < 5 || rank > 10) {
        MPI_Comm_size(three, &newsize);
        MPI_Comm_rank(three, &newrank);

        fprintf(stderr, "Three way split succeeded: I am now process %d of %d on %s\n", newrank, newsize, processor_name);
    } else {
        if (three == MPI_COMM_NULL) {
           fprintf(stderr, "Three way split succeeded: I do not participate on %s\n", processor_name);
        } else {
           fprintf(stderr, "Three way split failed: I do not get MPI_COMM_NULL on %s\n", processor_name);
           MPI_Finalize();
           return 1;
        }
    }

    fprintf(stderr, "Done!\n");

    MPI_Finalize();

    return 0;
}

