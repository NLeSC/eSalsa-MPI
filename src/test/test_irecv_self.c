#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
    int  namelen, rank, size, i, error;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int buffer[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    MPI_Request *requests;
    MPI_Status *statusses;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Get_processor_name(processor_name, &namelen);

    fprintf(stderr, "Process %d of %d on %s\n", rank, size, processor_name);

    if (rank == 0) {

       requests = malloc(size * sizeof(MPI_Request));
       statusses = malloc(size * sizeof(MPI_Status));

       if (requests == 0) {
          fprintf(stderr, "Failed to allocate requests!");
          return 1;
       }

       if (statusses == 0) {
          fprintf(stderr, "Failed to allocate statusses!");
          return 1;
       }

       for (i=0;i<size;i++) {
          error = MPI_Irecv(buffer, 10, MPI_INT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &requests[i]);

          if (error != MPI_SUCCESS) {
             fprintf(stderr, "Failed to post ireceive! %d\n", error);
             return 1;
          }
       }
    }

    error = MPI_Send(buffer, 10, MPI_INTEGER, 0, 0, MPI_COMM_WORLD);

    if (error != MPI_SUCCESS) {
       fprintf(stderr, "Send failed on %d! %d\n", rank, error);
       return 1;
    }

    if (rank == 0) {
       error = MPI_Waitall(size, requests, statusses);

       if (error != MPI_SUCCESS) {
          fprintf(stderr, "Waitall failed on %d! %d\n", rank, error);
          return 1;
       }

       free(requests);
       free(statusses);
    }

    fprintf(stderr, "Done!\n");

    MPI_Finalize();

    return 0;
}

