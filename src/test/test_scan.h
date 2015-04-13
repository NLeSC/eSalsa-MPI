#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int test_scan(MPI_Comm comm, char *name)
{
   int i, j, p, error, rank, size;
   int *sendbuffer;
   int *recvbuffer;

   MPI_Comm_size(comm, &size);
   MPI_Comm_rank(comm, &rank);

   sendbuffer = malloc(size * sizeof(int));

   if (sendbuffer == NULL) {
      fprintf(stderr, "Failed to allocate sendbuffer!\n");
      MPI_Finalize();
      return 1;
   }

   recvbuffer = malloc(size * sizeof(int));

   if (recvbuffer == NULL) {
      fprintf(stderr, "Failed to allocate recvbuffer!\n");
      MPI_Finalize();
      return 1;
   }

   fprintf(stderr, "SCAN %s ************\n", name);

   for (j=0;j<size;j++) {
      sendbuffer[j] = j;
      recvbuffer[j] = -1;
   }

   error = MPI_Scan(sendbuffer, recvbuffer, size, MPI_INT, MPI_SUM, comm);

   if (error != MPI_SUCCESS) {
      fprintf(stderr, "SCAN %s failed!\n", name);
      MPI_Finalize();
      return 1;
   }

   for (j=0;j<size;j++) {
      if (recvbuffer[j] != (rank+1)*j) {
         fprintf(stderr, "SCAN %s result incorrect on %d (recvbuffer[%d] expected %d got %d)\n", name, rank, j, j*(rank+1), recvbuffer[j]);
         MPI_Finalize();
         return 1;
      }
   }

   fprintf(stderr, " - SCAN %s OK\n", name);

   free(sendbuffer);
   free(recvbuffer);

   return 0;
}


