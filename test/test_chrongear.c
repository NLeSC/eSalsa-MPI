#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define MAX_COMM 25

#define COUNT 1000
#define REPEAT 1

double totalT = 0.0;
double irecvT = 0.0;
double isendT = 0.0;
double waitrT = 0.0;
double waitsT = 0.0;
double barT = 0.0;
double redT = 0.0;

int runtest(int rank, int size, int *messageDest, int *messageSize, int messages)
{
    int i, bufferSize, error, offset;
    double *sendbuf;
    double *receivebuf;

    double t1, t2, t3, t4, t5, t6, t7;

    double *tmpSum;
    double sum;

    MPI_Status *rstat;
    MPI_Status *sstat;

    MPI_Request *rreq;
    MPI_Request *sreq;

    rreq = malloc(messages*sizeof(MPI_Request));
    sreq = malloc(messages*sizeof(MPI_Request));

    for (i=0;i<messages;i++) { 
        rreq[i] = MPI_REQUEST_NULL;
        sreq[i] = MPI_REQUEST_NULL;
    }

    rstat = malloc(messages*sizeof(MPI_Status));
    sstat = malloc(messages*sizeof(MPI_Status));

    bufferSize = 0;

    for (i=0;i<messages;i++) {
       bufferSize += messageSize[i];
    }

    sendbuf = malloc(bufferSize*sizeof(double));
    receivebuf = malloc(bufferSize*sizeof(double));

    t1 = MPI_Wtime();

    offset = 0;

    for (i=0;i<messages;i++) {
       error = MPI_Irecv(receivebuf+offset, messageSize[i], MPI_DOUBLE, messageDest[i], 0, MPI_COMM_WORLD, rreq+i);
       offset += messageSize[i];

       if (error != MPI_SUCCESS) {
          fprintf(stderr, "Irecv failed (1)! %d\n", error);
          return 1;
       }
    }

    t2 = MPI_Wtime();

    offset = 0;

    for (i=0;i<messages;i++) {

       // start by sending to nodes in other cluster. 
       if ((rank < 128 && messageDest[i] >= 128) || (rank >= 128 && messageDest[i] < 128)) { 
          error = MPI_Isend(sendbuf+offset, messageSize[i], MPI_DOUBLE, messageDest[i], 0, MPI_COMM_WORLD, sreq+i);
          offset += messageSize[i];

          if (error != MPI_SUCCESS) {
             fprintf(stderr, "Isend failed (1)! %d\n", error);
             return 1;
          }
       }
    }

    for (i=0;i<messages;i++) {

       // then send to nodes in my cluster. 
       if ((rank < 128 && messageDest[i] < 128) || (rank >= 128 && messageDest[i] >= 128)) { 
          error = MPI_Isend(sendbuf+offset, messageSize[i], MPI_DOUBLE, messageDest[i], 0, MPI_COMM_WORLD, sreq+i);
          offset += messageSize[i];

          if (error != MPI_SUCCESS) {
             fprintf(stderr, "Isend failed (1)! %d\n", error);
             return 1;
          }
       }
    }

    t3 = MPI_Wtime();

    error = MPI_Waitall(messages, sreq, sstat);

//    error = MPI_Waitall(messages, rreq, rstat);

    if (error != MPI_SUCCESS) {
       fprintf(stderr, "Waitall(r) failed (1)! %d\n", error);
       return 1;
    }

    t4 = MPI_Wtime();

//    error = MPI_Waitall(messages, sreq, sstat);
    error = MPI_Waitall(messages, rreq, rstat);


    if (error != MPI_SUCCESS) {
       fprintf(stderr, "Waitall(s) failed (1)! %d\n", error);
       return 1;
    }

//    MPI_Barrier(MPI_COMM_WORLD);

    free(rreq);
    free(sreq);
    free(rstat);
    free(sstat);

    t5 = MPI_Wtime();

#if 0 
    rreq = malloc(size*sizeof(MPI_Request));
    rstat = malloc(size*sizeof(MPI_Status));

    for (i=0;i<size;i++) { 
        rreq[i] = MPI_REQUEST_NULL;
    }

    sendbuf = malloc(2*sizeof(double));
    receivebuf = malloc(2*size*sizeof(double));

    sendbuf[0] = rank;
    sendbuf[1] = size;

    if (rank == 0) { 
       for (i=1;i<size;i++) { 
          error = MPI_Irecv(receivebuf+(i*2), 2, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, rreq+i);

          if (error != MPI_SUCCESS) {
             fprintf(stderr, "Irecv failed (1)! %d\n", error);
             return 1;
          }
       }
    
       error = MPI_Waitall(size, rreq, rstat);

       for (i=1;i<size;i++) { 
          error = MPI_Send(sendbuf, 2, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);

          if (error != MPI_SUCCESS) {
             fprintf(stderr, "Irecv failed (1)! %d\n", error);
             return 1;
          }
       }

    } else { 
     
       error = MPI_Send(sendbuf, 2, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);

       if (error != MPI_SUCCESS) {
          fprintf(stderr, "Send failed (1)! %d\n", error);
          return 1;
       }

       error = MPI_Recv(receivebuf, 2, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, NULL);

       if (error != MPI_SUCCESS) {
          fprintf(stderr, "Receive failed (1)! %d\n", error);
          return 1;
       }
    }    

    free(rreq);
    free(rstat);
    free(sendbuf);
    free(receivebuf);
#endif

    error = MPI_Allreduce(sendbuf, receivebuf, 2, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

    if (error != MPI_SUCCESS) {
       fprintf(stderr, "ALLREDUCE failed! %d\n", error);
       return 1;
    }

    t6 = MPI_Wtime();

    free(sendbuf);
    free(receivebuf);

//    fprintf(stderr, "Time total=%f irecv=%f isnd=%f waitr=%f waits=%f red=%f\n", t6-t0, t2-t1, t3-t2, t4-t3, t5-t4, t1-t0); 

    totalT += t6-t1;
    irecvT += t2-t1;
    isendT += t3-t2;
    waitrT += t4-t3;
    waitsT += t5-t4;
   // barT  += t6-t5;    
    redT   += t6-t5;
   
    return 0;
}


int runtest2(int rank, int size, int *messageDest, int *messageSize, int messages)
{
    int i, bufferSize, error, offset;
    double *sendbuf;
    double *receivebuf;

    double t1, t2, t3, t4, t5, t6, t7;

    double *tmpSum;
    double sum;

    MPI_Status *rstat;
    MPI_Status *sstat;

    MPI_Request *rreq;
    MPI_Request *sreq;

    rreq = malloc(messages*sizeof(MPI_Request));
    sreq = malloc(messages*sizeof(MPI_Request));

    for (i=0;i<messages;i++) { 
        rreq[i] = MPI_REQUEST_NULL;
        sreq[i] = MPI_REQUEST_NULL;
    }

    rstat = malloc(messages*sizeof(MPI_Status));
    sstat = malloc(messages*sizeof(MPI_Status));

    bufferSize = 0;

    for (i=0;i<messages;i++) {
       bufferSize += messageSize[i];
    }

    sendbuf = malloc(bufferSize*sizeof(double));
    receivebuf = malloc(bufferSize*sizeof(double));

    t1 = MPI_Wtime();

    offset = 0;

    for (i=0;i<messages;i++) {
       error = MPI_Irecv(receivebuf+offset, messageSize[i], MPI_DOUBLE, messageDest[i], 0, MPI_COMM_WORLD, rreq+i);
       offset += messageSize[i];

       if (error != MPI_SUCCESS) {
          fprintf(stderr, "Irecv failed (1)! %d\n", error);
          return 1;
       }
    }

    t2 = MPI_Wtime();

    offset = 0;

    for (i=0;i<messages;i++) {

          error = MPI_Send(sendbuf+offset, messageSize[i], MPI_DOUBLE, messageDest[i], 0, MPI_COMM_WORLD);
          offset += messageSize[i];

          if (error != MPI_SUCCESS) {
             fprintf(stderr, "Isend failed (1)! %d\n", error);
             return 1;
          }
    }

/*
    for (i=0;i<messages;i++) {

       // start by sending to nodes in other cluster. 
       if ((rank < 128 && messageDest[i] >= 128) || (rank >= 128 && messageDest[i] < 128)) { 
          error = MPI_Send(sendbuf+offset, messageSize[i], MPI_DOUBLE, messageDest[i], 0, MPI_COMM_WORLD);
          offset += messageSize[i];

          if (error != MPI_SUCCESS) {
             fprintf(stderr, "Isend failed (1)! %d\n", error);
             return 1;
          }
       }
    }

    for (i=0;i<messages;i++) {

       // then send to nodes in my cluster. 
       if ((rank < 128 && messageDest[i] < 128) || (rank >= 128 && messageDest[i] >= 128)) { 
          error = MPI_Isend(sendbuf+offset, messageSize[i], MPI_DOUBLE, messageDest[i], 0, MPI_COMM_WORLD, sreq+i);
          offset += messageSize[i];

          if (error != MPI_SUCCESS) {
             fprintf(stderr, "Isend failed (1)! %d\n", error);
             return 1;
          }
       }
    }
*/
    t3 = MPI_Wtime();

    error = MPI_Waitall(messages, sreq, sstat);

//    error = MPI_Waitall(messages, rreq, rstat);

    if (error != MPI_SUCCESS) {
       fprintf(stderr, "Waitall(r) failed (1)! %d\n", error);
       return 1;
    }

    t4 = MPI_Wtime();

//    error = MPI_Waitall(messages, sreq, sstat);
    error = MPI_Waitall(messages, rreq, rstat);


    if (error != MPI_SUCCESS) {
       fprintf(stderr, "Waitall(s) failed (1)! %d\n", error);
       return 1;
    }

    t5 = MPI_Wtime();

    error = MPI_Allreduce(sendbuf, receivebuf, 2, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD);

    if (error != MPI_SUCCESS) {
       fprintf(stderr, "ALLREDUCE failed! %d\n", error);
       return 1;
    }

    t6 = MPI_Wtime();

    free(rreq);
    free(sreq);
    free(rstat);
    free(sstat);
    free(sendbuf);
    free(receivebuf);

    totalT += t6-t1;
    irecvT += t2-t1;
    isendT += t3-t2;
    waitrT += t4-t3;
    waitsT += t5-t4;
   // barT  += t6-t5;    
    redT   += t6-t5;
   
    return 0;
}


int main(int argc, char *argv[])
{
    FILE *fr;
    int  namelen, rank, size, i, j, error;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    int messageDest[MAX_COMM];
    int messageSize[MAX_COMM];

    int index = 0;
    char line[80];

    int src, dst, data;

    double start, end;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Get_processor_name(processor_name, &namelen);

    fprintf(stderr, "Process %d of %d on %s\n", rank, size, processor_name);

    fr = fopen ("comm.txt", "rt");

    while(fgets(line, 80, fr) != NULL)
    {
       sscanf (line, "%d %d %d", &src, &dst, &data);

       if (src == rank)
       {
            messageDest[index] = dst;
            messageSize[index] = data;
            index++;
       }
   }

   fclose(fr);  /* close the file prior to exiting the routine */

   fprintf(stderr, "Read %d message definitions\n", index);

   for (i=0;i<index;i++)
   {
      fprintf(stderr, "%d %d %d\n", rank, messageDest[i], messageSize[i]);
   }

   for (j=0;j<REPEAT;j++) 
   {
      totalT = 0.0;
      irecvT = 0.0;
      isendT = 0.0;
      waitrT = 0.0;
      waitsT = 0.0;
      barT = 0.0;
      redT = 0.0;

      start = MPI_Wtime();

      for (i=0;i<COUNT;i++)
      {
         error = runtest2(rank, size, messageDest, messageSize, index);

         if (error != 0) {
            fprintf(stderr, "Failed!\n");
	    break;
         }
      }

      end = MPI_Wtime();

      fprintf(stderr, "Time total %f irecv %f isnd %f waitr %f waits %f bar %f red %f\n", totalT, irecvT, isendT, waitrT, waitsT, barT, redT); 
   }

   MPI_Finalize();

   return 0;
}

