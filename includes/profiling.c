#include "flags.h"

#include <stdio.h>
#include <stdlib.h>

#include "mpi.h"

//#include "generated_header.h"
// #include "messaging.h"

#include "logging.h"
#include "profiling.h"

static char *statistic_names[STATS_TOTAL+1] = {
STATS_NAME_BARRIER,
STATS_NAME_SEND,
STATS_NAME_RSEND,
STATS_NAME_BSEND,
STATS_NAME_SSEND,
STATS_NAME_ISEND,
STATS_NAME_IRSEND,
STATS_NAME_IBSEND,
STATS_NAME_ISSEND,
STATS_NAME_RECV,
STATS_NAME_IRECV,
STATS_NAME_SEND_RECV,
STATS_NAME_PROBE,
STATS_NAME_IPROBE,
STATS_NAME_BCAST,
STATS_NAME_SCATTER,
STATS_NAME_GATHER,
STATS_NAME_ALLGATHER,
STATS_NAME_ALLTOALL,
STATS_NAME_REDUCE,
STATS_NAME_ALLREDUCE,
STATS_NAME_REDUCE_SCATTER,
STATS_NAME_SCAN,
STATS_NAME_EXSCAN,
STATS_NAME_WAIT,
STATS_NAME_WAITALL,
STATS_NAME_WAITANY,
STATS_NAME_WAITSOME,
STATS_NAME_TEST,
STATS_NAME_TESTALL,
STATS_NAME_TESTANY,
STATS_NAME_TESTSOME,
STATS_NAME_MISC,
STATS_NAME_TOTAL
};

static uint32_t *total_use[MAX_COMMUNICATORS];
static uint64_t *total_ticks[MAX_COMMUNICATORS];

static uint32_t *current_use[MAX_COMMUNICATORS];
static uint64_t *current_ticks[MAX_COMMUNICATORS];

static uint32_t running = 0;

// Profiling timers
uint64_t start_ticks;
uint64_t current_start_ticks;

uint32_t current_interval;

void profile_init()
{
   int i; //, error;

   for (i=0;i<MAX_COMMUNICATORS;i++) {
      total_use[i] = NULL;
      total_ticks[i] = NULL;
      current_use[i] = NULL;
      current_ticks[i] = NULL;
   }

   running = 1;
   current_interval = 0;

   start_ticks = current_start_ticks = profile_start_ticks();

   printf("Profiling initialized! (MAX=%d)", MAX_COMMUNICATORS);
}

static void print_and_reset_current_interval()
{
   int i, j;
   uint64_t ticks = 0;
   uint32_t use = 0;
   uint64_t current;

   printf("Statistics for interval %d\n", current_interval);

   for (i=0;i<MAX_COMMUNICATORS;i++) {
      if (current_ticks[i] != NULL) {

         printf("  Communicator %d ", i);

         for (j=0;j<STATS_TOTAL+1;j++) {
            printf("%s %ld %d ", statistic_names[j], current_ticks[i][j], current_use[i][j]);

            total_ticks[i][j] += current_ticks[i][j];
            total_use[i][j]   += current_use[i][j];
         }

         printf("\n");

         ticks += current_ticks[i][STATS_TOTAL];
         use += current_use[i][STATS_TOTAL];

         for (j=0;j<STATS_TOTAL+1;j++) {
            current_ticks[i][j] = 0;
            current_use[i][j] = 0;
         }
      }
   }

   current = profile_stop_ticks();

   printf("  Overall ticks in interval %d - total: %ld mpi: %ld calls: %d\n", current_interval, (current-current_start_ticks), ticks, use);

   current_interval++;
   current_start_ticks = profile_start_ticks();
}

void profile_finalize()
{
   uint64_t end_ticks;
   int i, j;
   uint64_t ticks = 0;
   uint32_t use = 0;

//   int rank, size;

   if (running != 1) {
      WARN(1, "Profiling not running!");
      return;
   }

   end_ticks = profile_stop_ticks();

   printf("Statistics for entire application (total intervals %d)\n", current_interval);

   for (i=0;i<MAX_COMMUNICATORS;i++) {
      if (current_ticks[i] != NULL) {

         for (j=0;j<STATS_TOTAL+1;j++) {
            total_ticks[i][j] += current_ticks[i][j];
            total_use[i][j]   += current_use[i][j];
         }

         printf("  Communicator %d ", i);

         for (j=0;j<STATS_TOTAL+1;j++) {
            printf("%s %ld %d ", statistic_names[j], total_ticks[i][j], total_use[i][j]);

         }

         printf("\n");

         ticks += total_ticks[i][STATS_TOTAL];
         use += total_use[i][STATS_TOTAL];
      }
   }

   printf("Total profiled ticks in %d intervals - total: %ld mpi: %ld calls: %d\n", current_interval, end_ticks-start_ticks, ticks, use);

   INFO(1, "Profiling done!");
}

void profile_add_statistics(MPI_Comm comm, int field, uint64_t ticks)
{
   int index;

   if (running != 1) {
      WARN(1, "Profiling not running!");
      return;
   }

   index = comm;

   if (index < 0 || index >= MAX_COMMUNICATORS) {
      ERROR(1, "Communicator index out of bounds: %d", index);
      return;
   }

   if (field < 0 || field >= STATS_TOTAL) {
      ERROR(1, "Statistics index out of bounds: %d (comm=%d)", field, index);
      return;
   }

   if (current_ticks[index] == NULL) {

      current_ticks[index] = calloc((STATS_TOTAL+1), sizeof(uint64_t));

      if (current_ticks[index] == NULL) {
         ERROR(1, "Failed to allocate profiling buffer for communicator %d (1)", index);
         return;
      }

      current_use[index] = calloc((STATS_TOTAL+1), sizeof(uint32_t));

      if (current_use[index] == NULL) {
         ERROR(1, "Failed to allocate profiling buffer for communicator %d (2)", index);
         return;
      }

      total_ticks[index] = calloc((STATS_TOTAL+1), sizeof(uint64_t));

      if (total_ticks[index] == NULL) {
         ERROR(1, "Failed to allocate profiling buffer for communicator %d (3)", index);
         return;
      }

      total_use[index] = calloc((STATS_TOTAL+1), sizeof(uint32_t));

      if (total_use[index] == NULL) {
         ERROR(1, "Failed to allocate profiling buffer for communicator %d (4)", index);
         return;
      }
   }

   current_ticks[index][field] += ticks;
   current_ticks[index][STATS_TOTAL] += ticks;

   current_use[index][field]++;
   current_use[index][STATS_TOTAL]++;
}

/*
void profile_print_statistics(MPI_Comm comm)
{
   int i, index;

   if (running != 1) {
      WARN(1, "Profiling not running!");
      return;
   }

   index = MPI_Comm_c2f(comm);

   if (index < 0 || index >= MAX_COMMUNICATORS) {
      WARN(1, "Communicator index out of bounds: %d", index);
      return;
   }

   if (total_ticks[index] == NULL) {
      printf("No statistics available for communicator %d\n", index);
      return;
   }

   printf("Communicator %d: ", index);

   for (i=0;i<STATS_TOTAL+1;i++) {
      printf("%s %ld %d ", statistic_names[i], total_ticks[index][i], total_use[index][i]);
   }

   printf("\n");
}

void profile_print_all_statistics()
{
   int i, j;

   uint64_t ticks = 0;
   uint32_t use = 0;

   if (running != 1) {
      WARN(1, "Profiling not running!");
      return;
   }

   printf("Statistics for all communicators:\n");

   for (i=0;i<MAX_COMMUNICATORS;i++) {

      if (total_ticks[i] != NULL) {

         printf("  Communicator %d ", i);

         for (j=0;j<STATS_TOTAL+1;j++) {
            printf("%s %ld %d ", statistic_names[j], total_ticks[i][j], total_use[i][j]);
         }

         printf("\n");

         ticks += total_ticks[i][STATS_TOTAL];
         use += total_use[i][STATS_TOTAL];
      }
   }

   printf("  Overall ticks %ld use %d\n", ticks, use);
}
*/

void dump_profile_info_()
{
   dump_profile_info();
}

void dump_profile_info()
{
//   int error;

//   int rank, size;

   if (running != 1) {
      WARN(1, "Profiling not running!");
      return;
   }

//   error = MPI_Barrier(profile_comm);

//   if (error != MPI_SUCCESS) {
//      ERROR(1, "Barrier failed");
//      return;
//   }

//   MPI_Comm_rank(profile_comm, &rank);
//   MPI_Comm_size(profile_comm, &size);

//   GPTLpr(current_interval*size + rank);

   print_and_reset_current_interval();
}

static uint64_t timer1 = 0;
static uint64_t timer2 = 0;
static uint64_t timer3 = 0;
static uint64_t timer4 = 0;
static uint64_t timer5 = 0;
static uint64_t timer6 = 0;
static uint64_t timer7 = 0;
static uint64_t timer8 = 0;

void jason_start_timer1_()
{
   timer1 = profile_start_ticks();
}

void jason_stop_timer1_(int *count, int *size)
{
   uint64_t tmp = profile_stop_ticks();
   tmp -= timer1;

   WARN(0, "timer1 took %ld ticks for copy of %d x %d (%d) from matrix", tmp, *count, *size, (*count)*(*size));
}

void jason_start_timer2_()
{
   timer2 = profile_start_ticks();
}


void jason_stop_timer2_(int *count, int *size)
{
   uint64_t tmp = profile_stop_ticks();
   tmp -= timer2;

   WARN(0, "timer2 took %ld ticks for copy of %d x %d (%d) to matrix", tmp, *count, *size, (*count)*(*size));
}

void jason_start_timer3_()
{
   timer3 = profile_start_ticks();
}

void jason_stop_timer3_()
{
   uint64_t tmp = profile_stop_ticks();
   tmp -= timer3;

   WARN(0, "timer3 (mp_sendirr) took %ld ticks", tmp);
}

void jason_start_timer4_()
{
   timer4 = profile_start_ticks();
}

void jason_stop_timer4_()
{
   uint64_t tmp = profile_stop_ticks();
   tmp -= timer4;

   WARN(0, "timer4 (mp_recvirr) took %ld ticks", tmp);
}

void jason_start_timer5_()
{
   timer5 = profile_start_ticks();
}

void jason_stop_timer5_()
{
   uint64_t tmp = profile_stop_ticks();
   tmp -= timer5;

   WARN(0, "timer5 (mp_sendirr_i4) took %ld ticks", tmp);
}

void jason_start_timer6_()
{
   timer6 = profile_start_ticks();
}

void jason_stop_timer6_()
{
   uint64_t tmp = profile_stop_ticks();
   tmp -= timer6;

   WARN(0, "timer6 (mp_recvirr_i4) took %ld ticks", tmp);
}

void jason_start_timer7_()
{
   timer7 = profile_start_ticks();
}

void jason_stop_timer7_()
{
   uint64_t tmp = profile_stop_ticks();
   tmp -= timer7;

   WARN(0, "timer7 (mp_swappirr) took %ld ticks", tmp);
}

void jason_start_timer8_()
{
   timer8 = profile_start_ticks();
}

void jason_stop_timer8_()
{
   uint64_t tmp = profile_stop_ticks();
   tmp -= timer8;

   WARN(0, "timer8 (mp_swapirr_i4) took %ld ticks", tmp);
}


