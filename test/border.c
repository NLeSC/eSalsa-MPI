#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

#define COUNT 100
#define REPEAT 100

//#define COPY 

void copyIn(double *from, double *to, int size, int off)
{
#ifdef COPY
   int i;

   for (i=0;i<size;i++) {
     to[REPEAT*i + off] = from[i];
   }
#endif
}

void copyOut(double *from, double *to, int size, int off)
{
#ifdef COPY
   int i;

   for (i=0;i<size;i++) {
     to[i] = from[REPEAT*i + off];
   }
#endif
}


int runtest0(int dsize, int rank, int size)
{
    int i, j, error, prev, next;

    double start, end;

    double *sbufPrev;
    double *rbufPrev;

    double *sbufNext;
    double *rbufNext;

    double *bufFrom;
    double *bufTo;

    MPI_Status  rstat[2];
    MPI_Status  sstat[2];

    MPI_Request rreq[2];
    MPI_Request sreq[2];

    sbufPrev = malloc(dsize*sizeof(double));
    rbufPrev = malloc(dsize*sizeof(double));

    sbufNext = malloc(dsize*sizeof(double));
    rbufNext = malloc(dsize*sizeof(double));

    bufFrom = malloc(2 * dsize * REPEAT * sizeof(double));
    bufTo = malloc(2 * dsize * REPEAT * sizeof(double));

    rreq[0] = MPI_REQUEST_NULL;
    rreq[1] = MPI_REQUEST_NULL;

    sreq[0] = MPI_REQUEST_NULL;
    sreq[1] = MPI_REQUEST_NULL;

    next = rank+1;

    if (next == size) {
       next = 0;
    }

    prev = rank-1;

    if (prev == -1) {
       prev = size-1;
    }

    for (i=0;i<COUNT;i++) {

       start = MPI_Wtime();

       for (j=0;j<REPEAT;j++) {

           // Handle prev
           error = MPI_Irecv(rbufPrev, dsize, MPI_DOUBLE, prev, 0, MPI_COMM_WORLD, &rreq[0]);

           if (error != MPI_SUCCESS) {
              fprintf(stderr, "Irecv failed (1)! %d\n", error);
              return 1;
           }

           copyOut(bufFrom, sbufPrev, dsize, 2*j);

           error = MPI_Isend(sbufPrev, dsize, MPI_DOUBLE, prev, 1, MPI_COMM_WORLD, &sreq[0]);

           if (error != MPI_SUCCESS) {
              fprintf(stderr, "Irecv failed (1)! %d\n", error);
              return 1;
           }

           // Handle next
           error = MPI_Irecv(rbufNext, dsize, MPI_DOUBLE, next, 1, MPI_COMM_WORLD, &rreq[1]);

           if (error != MPI_SUCCESS) {
              fprintf(stderr, "Irecv failed (2)! %d\n", error);
              return 1;
           }

           copyOut(bufFrom, sbufNext, dsize, 2*j+1);

           error = MPI_Isend(sbufNext, dsize, MPI_DOUBLE, next, 0, MPI_COMM_WORLD, &sreq[1]);

           if (error != MPI_SUCCESS) {
              fprintf(stderr, "Irecv failed (2)! %d\n", error);
              return 1;
           }

           error = MPI_Wait(&rreq[0], &rstat[0]);

           if (error != MPI_SUCCESS) {
              fprintf(stderr, "WAIT failed (1)! %d\n", error);
              return 1;
           }

           copyIn(rbufPrev, bufTo, dsize, 2*j);

           error = MPI_Wait(&rreq[1], &rstat[1]);

           if (error != MPI_SUCCESS) {
              fprintf(stderr, "WAIT failed (2)! %d\n", error);
              return 1;
           }

           copyIn(rbufNext, bufTo, dsize, 2*j+1);

           error = MPI_Waitall(2, sreq, sstat);

           if (error != MPI_SUCCESS) {
              fprintf(stderr, "WAITAll failed (1)! %d\n", error);
              return 1;
           }
       }

       end = MPI_Wtime();

       printf("Iteration %d / %d took %f sec. (%f/sec/phase)\n", dsize, i, end-start, (end-start)/REPEAT);
    }

    free(sbufPrev);
    free(rbufPrev);

    free(sbufNext);
    free(rbufNext);

    free(bufTo);
    free(bufFrom);

    return 0;
}


int runtest2(int dsize, int rank, int size)
{
    int i, j, error, prev, next;

    double start, end;

    double *sbufPrev;
    double *rbufPrev;

    double *sbufNext;
    double *rbufNext;

    double *bufFrom;
    double *bufTo;

    MPI_Status  rstat[2];
    MPI_Status  sstat[2];

    MPI_Request rreq[2];
    MPI_Request sreq[2];

    sbufPrev = malloc(dsize*sizeof(double));
    rbufPrev = malloc(dsize*sizeof(double));

    sbufNext = malloc(dsize*sizeof(double));
    rbufNext = malloc(dsize*sizeof(double));

    bufFrom = malloc(2 * dsize * REPEAT * sizeof(double));
    bufTo = malloc(2 * dsize * REPEAT * sizeof(double));

    rreq[0] = MPI_REQUEST_NULL;
    rreq[1] = MPI_REQUEST_NULL;

    sreq[0] = MPI_REQUEST_NULL;
    sreq[1] = MPI_REQUEST_NULL;

    next = rank+1;

    if (next == size) {
       next = 0;
    }

    prev = rank-1;

    if (prev == -1) {
       prev = size-1;
    }

    for (i=0;i<COUNT;i++) {

       start = MPI_Wtime();

       for (j=0;j<REPEAT;j++) {

           // Handle prev
           error = MPI_Irecv(rbufPrev, dsize, MPI_DOUBLE, prev, 0, MPI_COMM_WORLD, &rreq[0]);

           if (error != MPI_SUCCESS) {
              fprintf(stderr, "Irecv failed (1)! %d\n", error);
              return 1;
           }

           copyOut(bufFrom, sbufPrev, dsize, 2*j);

           error = MPI_Isend(sbufPrev, dsize, MPI_DOUBLE, prev, 1, MPI_COMM_WORLD, &sreq[0]);

           if (error != MPI_SUCCESS) {
              fprintf(stderr, "Irecv failed (1)! %d\n", error);
              return 1;
           }

           error = MPI_Irecv(rbufNext, dsize, MPI_DOUBLE, next, 1, MPI_COMM_WORLD, &rreq[1]);

           if (error != MPI_SUCCESS) {
              fprintf(stderr, "Irecv failed (2)! %d\n", error);
              return 1;
           }

           copyOut(bufFrom, sbufNext, dsize, 2*j+1);

           error = MPI_Isend(sbufNext, dsize, MPI_DOUBLE, next, 0, MPI_COMM_WORLD, &sreq[1]);

           if (error != MPI_SUCCESS) {
              fprintf(stderr, "Irecv failed (2)! %d\n", error);
              return 1;
           }


/*
           error = MPI_Wait(&rreq[0], &rstat[0]);

           if (error != MPI_SUCCESS) {
              fprintf(stderr, "WAIT failed (1)! %d\n", error);
              return 1;
           }

           copyIn(rbufPrev, bufTo, dsize, 2*j);

           error = MPI_Wait(&rreq[1], &rstat[1]);

           if (error != MPI_SUCCESS) {
              fprintf(stderr, "WAIT failed (2)! %d\n", error);
              return 1;
           }

           copyIn(rbufNext, bufTo, dsize, 2*j+1);
*/
           error = MPI_Waitall(2, rreq, rstat);

           if (error != MPI_SUCCESS) {
              fprintf(stderr, "WAITAll failed (2)! %d\n", error);
              return 1;
           }

           copyIn(rbufPrev, bufTo, dsize, 2*j);
           copyIn(rbufNext, bufTo, dsize, 2*j+1);

           error = MPI_Waitall(2, sreq, sstat);

           if (error != MPI_SUCCESS) {
              fprintf(stderr, "WAITAll failed (1)! %d\n", error);
              return 1;
           }

           error = MPI_Barrier(MPI_COMM_WORLD);
       }

       end = MPI_Wtime();

       printf("Iteration %d / %d took %f sec. (%f/sec/phase)\n", dsize, i, end-start, (end-start)/REPEAT);
    }

    free(sbufPrev);
    free(rbufPrev);

    free(sbufNext);
    free(rbufNext);

    free(bufTo);
    free(bufFrom);

    return 0;
}


int runtest1(int dsize, int rank, int size)
{
    int i, j, error, prev, next;

    double start, end;

    double *sbufPrev;
    double *rbufPrev;

    double *sbufNext;
    double *rbufNext;

    MPI_Status  rstat[2];
    MPI_Status  sstat[2];

    MPI_Request rreq[2];
    MPI_Request sreq[2];

    sbufPrev = malloc(dsize*sizeof(double));
    rbufPrev = malloc(dsize*sizeof(double));

    sbufNext = malloc(dsize*sizeof(double));
    rbufNext = malloc(dsize*sizeof(double));

    rreq[0] = MPI_REQUEST_NULL;
    rreq[1] = MPI_REQUEST_NULL;

    sreq[0] = MPI_REQUEST_NULL;
    sreq[1] = MPI_REQUEST_NULL;

    next = rank+1;

    if (next == size) {
       next = 0;
    }

    prev = rank-1;

    if (prev == -1) {
       prev = size-1;
    }

    for (i=0;i<COUNT;i++) {

       start = MPI_Wtime();

       for (j=0;j<REPEAT;j++) {

	if (rank % 2 == 0) {

           // Handle prev
           error = MPI_Irecv(rbufPrev, dsize, MPI_DOUBLE, prev, 0, MPI_COMM_WORLD, &rreq[0]);

           if (error != MPI_SUCCESS) {
              fprintf(stderr, "Irecv failed (1)! %d\n", error);
              return 1;
           }

           error = MPI_Isend(sbufPrev, dsize, MPI_DOUBLE, prev, 1, MPI_COMM_WORLD, &sreq[0]);

           if (error != MPI_SUCCESS) {
              fprintf(stderr, "Irecv failed (1)! %d\n", error);
              return 1;
           }

           // Handle next
           error = MPI_Irecv(rbufNext, dsize, MPI_DOUBLE, next, 1, MPI_COMM_WORLD, &rreq[1]);

           if (error != MPI_SUCCESS) {
              fprintf(stderr, "Irecv failed (2)! %d\n", error);
              return 1;
           }


           error = MPI_Isend(sbufNext, dsize, MPI_DOUBLE, next, 0, MPI_COMM_WORLD, &sreq[1]);

           if (error != MPI_SUCCESS) {
              fprintf(stderr, "Irecv failed (2)! %d\n", error);
              return 1;
           }


       } else { 

           // Handle next
           error = MPI_Irecv(rbufNext, dsize, MPI_DOUBLE, next, 1, MPI_COMM_WORLD, &rreq[1]);

           if (error != MPI_SUCCESS) {
              fprintf(stderr, "Irecv failed (2)! %d\n", error);
              return 1;
           }


           error = MPI_Isend(sbufNext, dsize, MPI_DOUBLE, next, 0, MPI_COMM_WORLD, &sreq[1]);

           if (error != MPI_SUCCESS) {
              fprintf(stderr, "Irecv failed (2)! %d\n", error);
              return 1;
           }

           // Handle prev
           error = MPI_Irecv(rbufPrev, dsize, MPI_DOUBLE, prev, 0, MPI_COMM_WORLD, &rreq[0]);

           if (error != MPI_SUCCESS) {
              fprintf(stderr, "Irecv failed (1)! %d\n", error);
              return 1;
           }

           error = MPI_Isend(sbufPrev, dsize, MPI_DOUBLE, prev, 1, MPI_COMM_WORLD, &sreq[0]);

           if (error != MPI_SUCCESS) {
              fprintf(stderr, "Irecv failed (1)! %d\n", error);
              return 1;
           }
       }

           error = MPI_Wait(&rreq[0], &rstat[0]);

           if (error != MPI_SUCCESS) {
              fprintf(stderr, "WAIT failed (1)! %d\n", error);
              return 1;
           }

           error = MPI_Wait(&rreq[1], &rstat[1]);

           if (error != MPI_SUCCESS) {
              fprintf(stderr, "WAIT failed (2)! %d\n", error);
              return 1;
           }

           error = MPI_Waitall(2, sreq, sstat);

           if (error != MPI_SUCCESS) {
              fprintf(stderr, "WAITAll failed (1)! %d\n", error);
              return 1;
           }
       }

       end = MPI_Wtime();

       printf("Iteration %d / %d took %f sec. (%f/sec/phase)\n", dsize, i, end-start, (end-start)/REPEAT);
    }

    free(sbufPrev);
    free(rbufPrev);

    free(sbufNext);
    free(rbufNext);

    return 0;
}


int main(int argc, char *argv[])
{
    int  namelen, rank, size, i, j, error, prev, next;
    char processor_name[MPI_MAX_PROCESSOR_NAME];

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    MPI_Get_processor_name(processor_name, &namelen);

    fprintf(stderr, "Process %d of %d on %s\n", rank, size, processor_name);

    for (i=16;i<((32*1024)+1);i=i*2) {
       runtest2(i, rank, size);
    }

    fprintf(stderr, "Done!\n");

    MPI_Finalize();

    return 0;
}

