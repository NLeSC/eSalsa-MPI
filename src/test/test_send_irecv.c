#include <stdio.h>
#include "mpi.h"

int main(int argc, char *argv[])
{
    int  namelen, rank, size, i, error;
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int buffer[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };

    MPI_Request request;
    MPI_Status status;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Get_processor_name(processor_name, &namelen);

    fprintf(stderr, "Process %d of %d on %s\n", rank, size, processor_name);

    if (rank == 0) {
       for (i=0;i<10;i++) {
          buffer[i] = 42+i;
       }

       for (i=1;i<size;i++) {
          fprintf(stderr, "Send to %d\n", i);

          error = MPI_Send(buffer, 10, MPI_INTEGER, i, 0, MPI_COMM_WORLD);

          if (error != MPI_SUCCESS) {
             fprintf(stderr, "Send failed! %d\n", error);
          }
       }
    } else {
       fprintf(stderr, "IRecv from to %d\n", 0);

       error = MPI_Irecv(buffer, 10, MPI_INTEGER, 0, 0, MPI_COMM_WORLD, &request);

       if (error != MPI_SUCCESS) {
          fprintf(stderr, "Irecv failed! %d\n", error);
       } else {

          error = MPI_Wait(&request, &status);

          if (error != MPI_SUCCESS) {
             fprintf(stderr, "Wait failed! %d\n", error);
          } else {
             fprintf(stderr, "Received from 0: {");

             for (i=0;i<10;i++) {
                fprintf(stderr, "%d ", buffer[i]);
             }

             fprintf(stderr, "}\n");
          }
       }
    }

    fprintf(stderr, "Done!\n");

    MPI_Finalize();

    return 0;
}

