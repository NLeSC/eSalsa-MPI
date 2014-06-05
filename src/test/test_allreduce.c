#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

static int test_allreduce(MPI_Comm comm, char *name)
{
   int i, error, rank, size;

   double start, end;

   double sendbuffer[2];
   double recvbuffer[2];

   MPI_Comm_size(comm, &size);
   MPI_Comm_rank(comm, &rank);

   fprintf(stderr, "ALLREDUCE %s ************\n", name);

   sendbuffer[0] = rank;
   sendbuffer[1] = size;
 
   start = MPI_Wtime(); 

   for (i=0;i<1000;i++) { 

      error = MPI_Allreduce(sendbuffer, recvbuffer, 2, MPI_DOUBLE, MPI_SUM, comm);

      if (error != MPI_SUCCESS) {
         fprintf(stderr, "ALLREDUCE %s failed!\n", name);
         MPI_Finalize();
         return 1;
      }
   }

   end = MPI_Wtime(); 

   fprintf(stderr, " - ALLREDUCE took %f seconds.\n", end-start);

   return 0;
}

int main(int argc, char *argv[])
{
    int  namelen, rank, size, error;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Get_processor_name(processor_name, &namelen);

    fprintf(stderr, "Process %d of %d on %s\n", rank, size, processor_name);

    fprintf(stderr, "Starting ALLREDUCE tests\n");

    error = test_allreduce(MPI_COMM_WORLD, "MPI_COMM_WORLD");

    if (error != 0) return error;

    MPI_Finalize();

    return 0;
}

