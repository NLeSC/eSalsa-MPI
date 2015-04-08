#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
    MPI_Group world;
    MPI_Group reverse;
 
    MPI_Comm c1;

    int  namelen, rank, size, newrank, newsize, color, key, i, error;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int buffer[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    int rev[1][3];

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Get_processor_name(processor_name, &namelen);

    fprintf(stderr, "Process %d of %d on %s\n", rank, size, processor_name);

    if (size != 16) {
        fprintf(stderr, "Need 16 processes for this test\n");
        return 1;
    }

    fprintf(stderr, "Performing MPI_Comm_group\n");

    error = MPI_Comm_group(MPI_COMM_WORLD, &world);

    if (error != MPI_SUCCESS) {
    	fprintf(stderr, "MPI_Comm_Group failed!\n");
        MPI_Finalize();
        return 1;
    }

    fprintf(stderr, "Performing MPI_Group_rank\n");

    error = MPI_Group_rank(world, &newrank);

    if (error != MPI_SUCCESS) {
	fprintf(stderr, "MPI_Group_rank failed!\n");
        MPI_Finalize();
        return 1;
    }

    fprintf(stderr, "Performing MPI_Group_size\n");

    error = MPI_Group_size(world, &newsize);

    if (error != MPI_SUCCESS) {
	fprintf(stderr, "MPI_Group_size failed!\n");
        MPI_Finalize();
        return 1;
    }

    fprintf(stderr, "I am process %d of %d in world group\n", newrank, newsize);

    rev[0][0] = 15;
    rev[0][1] = 0;
    rev[0][2] = -1;

    fprintf(stderr, "Performing MPI_Group_range_incl\n");

    error = MPI_Group_range_incl(world, 1, rev, &reverse);

    if (error != MPI_SUCCESS) {
	fprintf(stderr, "MPI_Group_range_incl failed!\n");
        MPI_Finalize();
        return 1;
    }

    fprintf(stderr, "Performing MPI_Group_rank of reverse group\n");

    error = MPI_Group_rank(reverse, &newrank);

    if (error != MPI_SUCCESS) {
	fprintf(stderr, "MPI_Group_rank failed!\n");
        MPI_Finalize();
        return 1;
    }

    fprintf(stderr, "Performing MPI_Group_size of reverse group\n");

    error = MPI_Group_size(reverse, &newsize);

    if (error != MPI_SUCCESS) {
	fprintf(stderr, "MPI_Group_size failed!\n");
        MPI_Finalize();
        return 1;
    }

    fprintf(stderr, "I am process %d of %d in reverse group\n", newrank, newsize);

    fprintf(stderr, "Performing MPI_Comm_create of reverse group\n");

    error = MPI_Comm_create(MPI_COMM_WORLD, reverse, &c1);

    if (error != MPI_SUCCESS) {
	fprintf(stderr, "MPI_Comm_create failed!\n");
        MPI_Finalize();
        return 1;
    }

    fprintf(stderr, "Performing MPI_Comm_size of reverse comm\n");

    error = MPI_Comm_size(c1, &newsize);

    if (error != MPI_SUCCESS) {
	fprintf(stderr, "MPI_Comm_size failed!\n");
        MPI_Finalize();
        return 1;
    }

    fprintf(stderr, "Performing MPI_Comm_rank of reverse comm\n");

    error = MPI_Comm_rank(c1, &newrank);

    if (error != MPI_SUCCESS) {
	fprintf(stderr, "MPI_Comm_rank failed!\n");
        MPI_Finalize();
        return 1;
    }

    fprintf(stderr, "I am process %d of %d in reverse comm\n", newrank, newsize);

    fprintf(stderr, "Done!\n");

    MPI_Finalize();

    return 0;
}

