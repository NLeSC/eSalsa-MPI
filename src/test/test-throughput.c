#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mpi.h"

// Total data to send per pair (must be power of two)
//#define TOTAL_DATA   (1024L*1024L*1024L)
#define TOTAL_DATA   (512*1024L*1024L)

// Min message size used (must be power of two)
#define MIN_MSG_SIZE (256)

// Max message size used (must be power of two)
//#define MAX_MSG_SIZE (1024L*1024L)
#define MAX_MSG_SIZE (8*1024L*1024L)

// Number of times to repeat a test
#define REPEAT (1)

static uint64_t current_time_micros()
{
   uint64_t result;
   struct timeval t;

   gettimeofday(&t,NULL);

   result = (t.tv_sec * 1000000LU) + t.tv_usec;

   return result;
}

int run_test(int rank, int size, int sender, int peer, int msgsize)
{
    uint64_t start, end, tpt, tpl, time;
    long total, count, msg_per_process;
    int i, j, error;

    unsigned char *buf;

    buf = malloc(msgsize);

    if (buf == NULL) {
       fprintf(stderr, "Failed to allocate buffer of size %d\n", msgsize);
       return 1;
    }

    count = TOTAL_DATA / msgsize;

    if ((TOTAL_DATA % msgsize) != 0) {
       count++;
    }

    msg_per_process = count / (size/2);

    if (msg_per_process % (size/2) != 0) {
       msg_per_process++;
    }

    total = msg_per_process * (size/2) * msgsize;

    if (rank == 0) {
       fprintf(stderr, "Each of %d senders will send %ld messages of %d bytes (total = %ld bytes)\n", (size/2), msg_per_process, msgsize, total);
       fprintf(stderr, "Test will be repeated %d times\n", REPEAT);
    }

    if (sender) {
       fprintf(stderr, "I am a sender\n");
    } else {
       fprintf(stderr, "I am a receiver\n");
    }

    error = MPI_Barrier(MPI_COMM_WORLD);

    if (error != MPI_SUCCESS) {
       fprintf(stderr, "Barrier failed! (error = %d)\n", error);
       free(buf);
       return 1;
    }

    for (i=0;i<REPEAT;i++) {

       start = current_time_micros();

       for (j=0;j<msg_per_process;j++) {

          if (sender) {
             error = MPI_Send(buf, msgsize, MPI_BYTE, peer, j, MPI_COMM_WORLD);
          } else {
             error = MPI_Recv(buf, msgsize, MPI_BYTE, peer, j, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          }

          if (error != MPI_SUCCESS) {
             fprintf(stderr, "Test %d/%d failed! (error = %d)\n", i, j, error);
             free(buf);
             return 1;
          }
       }

    if (sender) {
       fprintf(stderr, "Done sending -- Barrier\n");
    } else {
       fprintf(stderr, "Done receiving -- Barrier\n");
    }

       error = MPI_Barrier(MPI_COMM_WORLD);

       if (error != MPI_SUCCESS) {
          fprintf(stderr, "Barrier after test %d failed! (error = %d)\n", i, error);
          free(buf);
          return 1;
       }

       end = current_time_micros();

       // Time in micro seconds
       time = (end-start);

       // Total data volume in bits (as send by all senders)
       tpt = (8L*total);

       // Local data volume in bits (as send by this sender)
       tpl = (8L * msgsize * msg_per_process);

       if (rank == 0) {
          fprintf(stderr, "Test %d - %d of %d took %ld usec for %ld bytes = %ld Mbit/sec total, %ld MBit/sec local.\n", msgsize, i, REPEAT, time, total, (tpt/time), (tpl/time));
       }
    }

    free(buf);

    return 0;
}

int main(int argc, char *argv[])
{
    int size, rank, sender, peer, i, result;

    int msgsize = MIN_MSG_SIZE;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (size % 2 == 1) {
       fprintf(stderr, "Throughput test requires even number of processors!\n");
       MPI_Finalize();
       return 1;
    }

    if (rank < size/2) {
       sender = 1;
       peer = size/2 + rank;
       fprintf(stderr, "My rank is %d and I will send to %d\n", rank, peer);
    } else {
       sender = 0;
       peer = rank - size/2;
       fprintf(stderr, "My rank is %d and I will receive from %d\n", rank, peer);
    }

    while (msgsize <= MAX_MSG_SIZE) {
       result = run_test(rank, size, sender, peer, msgsize);

       if (result != 0) {
          fprintf(stderr, "Test failed! (msgsize=%d)\n", msgsize);
          MPI_Finalize();
          return 1;
       }

       msgsize *= 2;
    }

    fprintf(stderr, "Done!\n");

    MPI_Finalize();

    return 0;
}

