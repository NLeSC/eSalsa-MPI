#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "../include/settings.h"

#include "../shared/profiling.h"
#include "../shared/logging.h"

#include "../include/mpi.h"
#include "../backend/empi.h"

// The indexes and names of the profiled communication operations.
#define STATS_BARRIER         0
#define STATS_SEND            1
#define STATS_RSEND           2
#define STATS_BSEND           3
#define STATS_SSEND           4
#define STATS_ISEND           5
#define STATS_IRSEND          6
#define STATS_IBSEND          7
#define STATS_ISSEND          8
#define STATS_RECV            9
#define STATS_IRECV          10
#define STATS_SEND_RECV      11
#define STATS_IPROBE         12
#define STATS_PROBE          13
#define STATS_BCAST          14
#define STATS_SCATTER        15
#define STATS_GATHER         16
#define STATS_ALLGATHER      17
#define STATS_ALLTOALL       18
#define STATS_REDUCE         19
#define STATS_ALLREDUCE      20
#define STATS_REDUCE_SCATTER 21
#define STATS_SCAN           22
#define STATS_EXSCAN         23
#define STATS_WAIT           24
#define STATS_WAITALL        25
#define STATS_WAITANY        26
#define STATS_WAITSOME       27
#define STATS_TEST           28
#define STATS_TESTALL        29
#define STATS_TESTANY        30
#define STATS_TESTSOME       31
#define STATS_MISC           32
#define STATS_TOTAL          33

#define STATS_NAME_BARRIER        "barrier"
#define STATS_NAME_SEND           "send"
#define STATS_NAME_RSEND          "rsend"
#define STATS_NAME_BSEND          "bsend"
#define STATS_NAME_SSEND          "ssend"
#define STATS_NAME_ISEND          "isend"
#define STATS_NAME_IRSEND         "irsend"
#define STATS_NAME_IBSEND         "ibsend"
#define STATS_NAME_ISSEND         "issend"
#define STATS_NAME_RECV           "receive"
#define STATS_NAME_IRECV          "ireceive"
#define STATS_NAME_SEND_RECV      "send_receive"
#define STATS_NAME_PROBE          "ireceive"
#define STATS_NAME_IPROBE         "iprobe"
#define STATS_NAME_BCAST          "bcast"
#define STATS_NAME_SCATTER        "scatter"
#define STATS_NAME_GATHER         "gather"
#define STATS_NAME_ALLGATHER      "allgather"
#define STATS_NAME_ALLTOALL       "alltoall"
#define STATS_NAME_REDUCE         "reduce"
#define STATS_NAME_ALLREDUCE      "allreduce"
#define STATS_NAME_REDUCE_SCATTER "reduce_scatter"
#define STATS_NAME_SCAN           "scan"
#define STATS_NAME_EXSCAN         "exscan"
#define STATS_NAME_WAIT           "wait"
#define STATS_NAME_WAITALL        "waitall"
#define STATS_NAME_WAITANY        "waitany"
#define STATS_NAME_WAITSOME       "waitsome"
#define STATS_NAME_TEST           "test"
#define STATS_NAME_TESTALL        "testall"
#define STATS_NAME_TESTANY        "testany"
#define STATS_NAME_TESTSOME       "testsome"
#define STATS_NAME_MISC           "misc"
#define STATS_NAME_TOTAL          "total"

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

// Circular string buffer used for debugging output. Note that if the
// (total) output of a single print operation is larger than DEBUG_BUF_SIZE
// the debug output will not be printed correctly.

#ifdef TRACE_CALLS

#define DEBUG_BUF_SIZE 4096

static char debug_buf[DEBUG_BUF_SIZE+1];
static int debug_buf_pos = 0;

/***************************************************************************/
/*                        Conversion functions                             */
/***************************************************************************/

static char *copy_to_debug_buf(const char *tmp, int len)
{
   char *res;

   if (len > DEBUG_BUF_SIZE) {
      // Cannot store this string!
      WARN(0, "Debug buffer overflow!");
      return &debug_buf[debug_buf_pos];
   }

   if (debug_buf_pos + len > DEBUG_BUF_SIZE) {
      debug_buf_pos = 0;
   }

   res = strcpy(&debug_buf[debug_buf_pos], tmp);
   debug_buf_pos += len;
   return res;
}

static char *comm_to_string(MPI_Comm comm)
{
   int len = 0;
   char tmp[64];

   if (comm == MPI_COMM_WORLD) {
      len = sprintf(tmp, "<WORLD>");
      return copy_to_debug_buf(tmp, len+1);
   }

   if (comm == MPI_COMM_SELF) {
      len = sprintf(tmp, "<SELF>");
      return copy_to_debug_buf(tmp, len+1);
   }

   if (comm == MPI_COMM_NULL) {
      len = sprintf(tmp, "<NULL>");
      return copy_to_debug_buf(tmp, len+1);
   }

   len = sprintf(tmp, "<%d>", comm);
   return copy_to_debug_buf(tmp, len+1);
}

static char *request_to_string(MPI_Request r)
{
   int len;
   char tmp[64];

   if (r == MPI_REQUEST_NULL) {
      len = sprintf(tmp, "<NULL>");
      return copy_to_debug_buf(tmp, len+1);
   }

   len = sprintf(tmp, "<%d>", r);
   return copy_to_debug_buf(tmp, len+1);
}

static char *group_to_string(MPI_Group g)
{
   int len;
   char tmp[64];

   if (g == MPI_GROUP_EMPTY) {
      len = sprintf(tmp, "<EMPTY>");
      return copy_to_debug_buf(tmp, len+1);
   }

   if (g == MPI_GROUP_NULL) {
      len = sprintf(tmp, "<NULL>");
      return copy_to_debug_buf(tmp, len+1);
   }

   len = sprintf(tmp, "<%d>", g);
   return copy_to_debug_buf(tmp, len+1);
}

static char *type_to_string(MPI_Datatype type)
{
   int len = 0;
   char tmp[1024];

// FIXME!

//   int error = get_type_name(type, tmp, &len);

//   if (error != MPI_SUCCESS || len <= 0) {
//      len = sprintf(tmp, "<UNKNOWN>");
//   }

   tmp[0] = '\0';
   len = 0;

   return copy_to_debug_buf(tmp, len+1);
}

static char *op_to_string(MPI_Op o)
{
   int len;
   char tmp[64];

   // FIXME

   tmp[0] = '\0';
   len = 0;

   return copy_to_debug_buf(tmp, len+1);
}

static char *info_to_string(MPI_Info i)
{
   int len;
   char tmp[64];

   len = sprintf(tmp, "<%d>", i);
   return copy_to_debug_buf(tmp, len+1);
}

static char *file_to_string(MPI_File f)
{
   int len;
   char tmp[64];

   len = sprintf(tmp, "<%d>", f);
   return copy_to_debug_buf(tmp, len+1);
}

static char *win_to_string(MPI_Win w)
{
   int len;
   char tmp[64];

   len = sprintf(tmp, "<%d>", w);
   return copy_to_debug_buf(tmp, len+1);
}

static char *ranks_to_string(int *ranks, int n)
{
   int i;
   int len = 0;
   char buf[1024];

   len += sprintf(buf, "[");

   for (i=0;i<n;i++) {
      len += sprintf(&buf[len], "%d", ranks[i]);

      if (i != n-1) {
         len += sprintf(&buf[len], ",");
      }
   }

   len += sprintf(&buf[len], "]");
   return copy_to_debug_buf(buf, len+1);
}

static char *ranges_to_string(int range[][3], int n)
{
   int i;
   int len = 0;
   char buf[1024];

   len += sprintf(buf, "[");

   for (i=0;i<n;i++) {
      len += sprintf(&buf[len], "(%d,%d,%d)", range[i][0], range[i][1], range[i][2]);

      if (i != n-1) {
         len += sprintf(&buf[len], ",");
      }
   }

   len += sprintf(&buf[len], "]");
   return copy_to_debug_buf(buf, len+1);
}

#endif

/* ------------------------ Misc. utility function ------------------------ */

int MPI_Init ( int *argc, char ***argv )
{
   // NOTE: library will fail if any other MPI function is called before MPI_Init!
   init_logging();
   debug_buf[DEBUG_BUF_SIZE] = '\0';

#ifdef TRACE_CALLS
   INFO(0, "MPI_Init(int *argc=%p, char ***argv=%p)", argc, argv);
#endif // TRACE_CALLS

   int error = EMPI_Init(argc, argv);

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Init failed (%d)!", error);
   }
#endif // TRACE_ERRORS

   profile_init(statistic_names);

   return error;
}

int MPI_Initialized ( int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Initialized(int *flag=%p)", flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Initialized(flag);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Initialized failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int MPI_Abort ( MPI_Comm comm, int errorcode )
{
#ifdef TRACE_CALLS
   INFO(0, "MPI_Abort(MPI_Comm comm=%s, int errorcode=%d)", comm_to_string(comm), errorcode);
#endif // TRACE_CALLS

   int error = EMPI_Abort(comm, errorcode);

#if PROFILE_LEVEL > 0
   profile_finalize();
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Abort failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int MPI_Finalize ( void )
{
#ifdef TRACE_CALLS
   INFO(0, "MPI_Finalize()");
#endif // TRACE_CALLS

   int error = EMPI_Finalize();

#if PROFILE_LEVEL > 0
   profile_finalize();
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Finalize failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int MPI_Finalized ( int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Finalized(int *flag=%p)", flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Finalized(flag);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Finalized failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

double MPI_Wtime ( void )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Wtime( void )");
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   double result = EMPI_Wtime();

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

   return result;
}

int MPI_Get_processor_name ( char *name, int *resultlen )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Get_processor_name(char *name=%p, int *resultlen=%p)", name, resultlen);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Get_processor_name(name, resultlen);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Get_processor_name failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int MPI_Error_string ( int errorcode, char *string, int *resultlen )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Error_string(int errorcode=%d, char *string=%p, int *resultlen=%p)", errorcode, string, resultlen);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Error_string(errorcode, string, resultlen);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Error_string failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}




/* ------------------------ Communicators and groups ------------------------ */

int MPI_Comm_create ( MPI_Comm comm, MPI_Group g, MPI_Comm *newcomm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_create(MPI_Comm comm=%s, MPI_Group g=%s, MPI_Comm *newcomm=%p)", comm_to_string(comm), group_to_string(g), newcomm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Comm_create(comm, g, newcomm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int MPI_Comm_dup ( MPI_Comm comm, MPI_Comm *newcomm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_dup(MPI_Comm comm=%s, MPI_Comm *newcomm=%p)", comm_to_string(comm), newcomm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Comm_dup(comm, newcomm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_dup failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_free ( MPI_Comm *comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_free(MPI_Comm *comm=%p)", comm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Comm_free(comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_free failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int MPI_Comm_group ( MPI_Comm comm, MPI_Group *g )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_group(MPI_Comm comm=%s, MPI_Group *g=%p)", comm_to_string(comm), g);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Comm_group(comm, g);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_group failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_rank ( MPI_Comm comm, int *rank )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_rank(MPI_Comm comm=%s, int *rank=%p)", comm_to_string(comm), rank);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Comm_rank(comm, rank);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_rank failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_size ( MPI_Comm comm, int *size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_size(MPI_Comm comm=%s, int *size=%p)", comm_to_string(comm), size);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Comm_size(comm, size);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_size failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_split ( MPI_Comm comm, int color, int key, MPI_Comm *newcomm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_split(MPI_Comm comm=%s, int color=%d, int key=%d, MPI_Comm *newcomm=%p)", comm_to_string(comm), color, key, newcomm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Comm_split(comm, color, key, newcomm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_split failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int MPI_Group_range_incl ( MPI_Group g, int n, int ranges[][3], MPI_Group *newgroup )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Group_range_incl(MPI_Group g=%s, int n=%d, int ranges[][3]=%p, MPI_Group *newgroup=%p)", group_to_string(g), n, ranges, newgroup);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Group_range_incl(g, n, ranges, newgroup);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Group_range_incl failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int MPI_Group_range_excl ( MPI_Group g, int n, int ranges[][3], MPI_Group *newgroup )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Group_range_excl(MPI_Group g=%s, int n=%d, int ranges[][3]=%p, MPI_Group *newgroup=%p)", group_to_string(g), n, ranges, newgroup);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Group_range_excl(g, n, ranges, newgroup);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Group_range_excl failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int MPI_Group_translate_ranks ( MPI_Group group1, int n, int *ranks1, MPI_Group group2, int *ranks2 )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Group_translate_ranks(MPI_Group group1=%s, int n=%d, int *ranks1=%p, MPI_Group group2=%s, int *ranks2=%p)", group_to_string(group1), n, ranks1, group_to_string(group2), ranks2);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL


   int error = EMPI_Group_translate_ranks(group1, n, ranks1, group2, ranks2);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Group_translate_ranks failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Group_union ( MPI_Group group1, MPI_Group group2, MPI_Group *newgroup )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Group_union(MPI_Group group1=%s, MPI_Group group2=%s, MPI_Group *newgroup=%p)", group_to_string(group1), group_to_string(group2), newgroup);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Group_union(group1, group2, newgroup);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Group_union failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int MPI_Group_incl ( MPI_Group g, int n, int *ranks, MPI_Group *newgroup )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Group_incl(MPI_Group g=%s, int n=%d, int *ranks=%p, MPI_Group *newgroup=%p)", group_to_string(g), n, ranks, newgroup);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Group_incl(g, n, ranks, newgroup);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Group_incl failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


/* ------------------------ Collective communication ------------------------ */

int MPI_Allgather ( void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Allgather(void *sendbuf=%p, int sendcount=%d, MPI_Datatype sendtype=%s, void *recvbuf=%p, int recvcount=%d, MPI_Datatype recvtype=%s, MPI_Comm comm=%s)", sendbuf, sendcount, type_to_string(sendtype), recvbuf, recvcount, type_to_string(recvtype), comm_to_string(comm));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Allgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_ALLGATHER, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Allgather failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Allgatherv ( void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int *recvcounts, int *displs, MPI_Datatype recvtype, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Allgatherv(void *sendbuf=%p, int sendcount=%d, MPI_Datatype sendtype=%s, void *recvbuf=%p, int *recvcounts=%p, int *displs=%p, MPI_Datatype recvtype=%s, MPI_Comm comm=%s)", sendbuf, sendcount, type_to_string(sendtype), recvbuf, recvcounts, displs, type_to_string(recvtype), comm_to_string(comm));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Allgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_ALLGATHER, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Allgatherv failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int MPI_Allreduce ( void *sendbuf, void *recvbuf, int count, MPI_Datatype type, MPI_Op op, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Allreduce(void *sendbuf=%p, void *recvbuf=%p, int count=%d, MPI_Datatype type=%s, MPI_Op op=%s, MPI_Comm comm=%s)", sendbuf, recvbuf, count, type_to_string(type), op_to_string(op), comm_to_string(comm));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Allreduce(sendbuf, recvbuf, count, type, op, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_ALLREDUCE, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Allreduce failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Alltoall ( void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Alltoall(void *sendbuf=%p, int sendcount=%d, MPI_Datatype sendtype=%s, void *recvbuf=%p, int recvcount=%d, MPI_Datatype recvtype=%s, MPI_Comm comm=%s)", sendbuf, sendcount, type_to_string(sendtype), recvbuf, recvcount, type_to_string(recvtype), comm_to_string(comm));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Alltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_ALLTOALL, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Alltoall failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Alltoallv ( void *sendbuf, int *sendcnts, int *sdispls, MPI_Datatype sendtype, void *recvbuf, int *recvcnts, int *rdispls, MPI_Datatype recvtype, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Alltoallv(void *sendbuf=%p, int *sendcnts=%p, int *sdispls=%p, MPI_Datatype sendtype=%s, void *recvbuf=%p, int *recvcnts=%p, int *rdispls=%p, MPI_Datatype recvtype=%s, MPI_Comm comm=%s)", sendbuf, sendcnts, sdispls, type_to_string(sendtype), recvbuf, recvcnts, rdispls, type_to_string(recvtype), comm_to_string(comm));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Alltoallv(sendbuf, sendcnts, sdispls, sendtype, recvbuf, recvcnts, rdispls, recvtype, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_ALLTOALL, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Alltoallv failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Alltoallw ( void *sendbuf, int *sendcnts, int *sdispls, MPI_Datatype *sendtypes, void *recvbuf, int *recvcnts, int *rdispls, MPI_Datatype *recvtypes, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Alltoallw(void *sendbuf=%p, int *sendcnts=%p, int *sdispls=%p, MPI_Datatype *sendtypes=%p, void *recvbuf=%p, int *recvcnts=%p, int *rdispls=%p, MPI_Datatype *recvtypes=%p, MPI_Comm comm=%s)", sendbuf, sendcnts, sdispls, sendtypes, recvbuf, recvcnts, rdispls, recvtypes, comm_to_string(comm));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Alltoallw(sendbuf, sendcnts, sdispls, sendtypes, recvbuf, recvcnts, rdispls, recvtypes, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_ALLTOALL, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Alltoallw failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int MPI_Scatter ( void *sendbuf, int sendcnt, MPI_Datatype sendtype, void *recvbuf, int recvcnt, MPI_Datatype recvtype, int root, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Scatter(void *sendbuf=%p, int sendcnt=%d, MPI_Datatype sendtype=%s, void *recvbuf=%p, int recvcnt=%d, MPI_Datatype recvtype=%s, int root=%d, MPI_Comm comm=%s)", sendbuf, sendcnt, type_to_string(sendtype), recvbuf, recvcnt, type_to_string(recvtype), root, comm_to_string(comm));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Scatter(sendbuf, sendcnt, sendtype, recvbuf, recvcnt, recvtype, root, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_SCATTER, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Scatter failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Scatterv ( void *sendbuf, int *sendcnts, int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcnt, MPI_Datatype recvtype, int root, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Scatterv(void *sendbuf=%p, int *sendcnts=%p, int *displs=%p, MPI_Datatype sendtype=%s, void *recvbuf=%p, int recvcnt=%d, MPI_Datatype recvtype=%s, int root=%d, MPI_Comm comm=%s)", sendbuf, sendcnts, displs, type_to_string(sendtype), recvbuf, recvcnt, type_to_string(recvtype), root, comm_to_string(comm));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Scatterv(sendbuf, sendcnts, displs, sendtype, recvbuf, recvcnt, recvtype, root, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_SCATTER, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Scatterv failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int MPI_Barrier ( MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Barrier(MPI_Comm comm=%s)", comm_to_string(comm));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Barrier(comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_BARRIER, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Barrier failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Bcast ( void *buffer, int count, MPI_Datatype type, int root, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Bcast(void *buffer=%p, int count=%d, MPI_Datatype type=%s, int root=%d, MPI_Comm comm=%s)", buffer, count, type_to_string(type), root, comm_to_string(comm));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Bcast(buffer, count, type, root, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_BCAST, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Bcast failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int MPI_Gather ( void *sendbuf, int sendcnt, MPI_Datatype sendtype, void *recvbuf, int recvcnt, MPI_Datatype recvtype, int root, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Gather(void *sendbuf=%p, int sendcnt=%d, MPI_Datatype sendtype=%s, void *recvbuf=%p, int recvcnt=%d, MPI_Datatype recvtype=%s, int root=%d, MPI_Comm comm=%s)", sendbuf, sendcnt, type_to_string(sendtype), recvbuf, recvcnt, type_to_string(recvtype), root, comm_to_string(comm));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Gather(sendbuf, sendcnt, sendtype, recvbuf, recvcnt, recvtype, root, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_GATHER, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Gather failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Gatherv ( void *sendbuf, int sendcnt, MPI_Datatype sendtype, void *recvbuf, int *recvcnts, int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Gatherv(void *sendbuf=%p, int sendcnt=%d, MPI_Datatype sendtype=%s, void *recvbuf=%p, int *recvcnts=%p, int *displs=%p, MPI_Datatype recvtype=%s, int root=%d, MPI_Comm comm=%s)", sendbuf, sendcnt, type_to_string(sendtype), recvbuf, recvcnts, displs, type_to_string(recvtype), root, comm_to_string(comm));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Gatherv(sendbuf, sendcnt, sendtype, recvbuf, recvcnts, displs, recvtype, root, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_GATHER, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Gatherv failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Reduce ( void *sendbuf, void *recvbuf, int count, MPI_Datatype type, MPI_Op op, int root, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Reduce(void *sendbuf=%p, void *recvbuf=%p, int count=%d, MPI_Datatype type=%s, MPI_Op op=%s, int root=%d, MPI_Comm comm=%s)", sendbuf, recvbuf, count, type_to_string(type), op_to_string(op), root, comm_to_string(comm));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Reduce(sendbuf, recvbuf, count, type, op, root, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_REDUCE, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Reduce failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}



/* ------------------------ Send / Receive operations ------------------------ */

/*
int MPI_Ibsend ( void *buf, int count, MPI_Datatype type, int dest, int tag, MPI_Comm comm, MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Ibsend(void *buf=%p, int count=%d, MPI_Datatype type=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s, MPI_Request *r=%p)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm), r);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Ibsend(buf, count, type, dest, tag, comm, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_IBSEND, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Ibsend failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}
*/


int MPI_Irecv ( void *buf, int count, MPI_Datatype type, int source, int tag, MPI_Comm comm, MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Irecv(void *buf=%p, int count=%d, MPI_Datatype type=%s, int source=%d, int tag=%d, MPI_Comm comm=%s, MPI_Request *r=%p)", buf, count, type_to_string(type), source, tag, comm_to_string(comm), r);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Irecv(buf, count, type, source, tag, comm, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_IRECV, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Irecv failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Irsend ( void *buf, int count, MPI_Datatype type, int dest, int tag, MPI_Comm comm, MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Irsend(void *buf=%p, int count=%d, MPI_Datatype type=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s, MPI_Request *r=%p)", buf, count, type_to_string(type), dest, tag, comm_to_string(comm), r);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Irsend(buf, count, type, dest, tag, comm, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_IRSEND, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Irsend failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Isend ( void *buf, int count, MPI_Datatype type, int dest, int tag, MPI_Comm comm, MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Isend(void *buf=%p, int count=%d, MPI_Datatype type=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s, MPI_Request *r=%p)", buf, count, type_to_string(type), dest, tag, comm_to_string(comm), r);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Isend(buf, count, type, dest, tag, comm, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_ISEND, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Isend failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Rsend ( void *buf, int count, MPI_Datatype type, int dest, int tag, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Rsend(void *buf=%p, int count=%d, MPI_Datatype type=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s)", buf, count, type_to_string(type), dest, tag, comm_to_string(comm));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Rsend(buf, count, type, dest, tag, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_RSEND, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Rsend failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Send ( void *buf, int count, MPI_Datatype type, int dest, int tag, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Send(void *buf=%p, int count=%d, MPI_Datatype type=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s)", buf, count, type_to_string(type), dest, tag, comm_to_string(comm));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Send(buf, count, type, dest, tag, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_SEND, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Send failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Sendrecv ( void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *s )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Sendrecv(void *sendbuf=%p, int sendcount=%d, MPI_Datatype sendtype=%s, int dest=%d, int sendtag=%d, void *recvbuf=%p, int recvcount=%d, MPI_Datatype recvtype=%s, int source=%d, int recvtag=%d, MPI_Comm comm=%s, MPI_Status *s=%p)", sendbuf, sendcount, type_to_string(sendtype), dest, sendtag, recvbuf, recvcount, type_to_string(recvtype), source, recvtag, comm_to_string(comm), s);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Sendrecv(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, (EMPI_Status *)s);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_SEND_RECV, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Sendrecv failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int MPI_Ssend ( void *buf, int count, MPI_Datatype type, int dest, int tag, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Ssend(void *buf=%p, int count=%d, MPI_Datatype type=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s)", buf, count, type_to_string(type), dest, tag, comm_to_string(comm));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Ssend(buf, count, type, dest, tag, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_SSEND, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Ssend failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Recv ( void *buf, int count, MPI_Datatype type, int source, int tag, MPI_Comm comm, MPI_Status *s )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Recv(void *buf=%p, int count=%d, MPI_Datatype type=%s, int source=%d, int tag=%d, MPI_Comm comm=%s, MPI_Status *s=%p)", buf, count, type_to_string(type), source, tag, comm_to_string(comm), s);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Recv(buf, count, type, source, tag, comm, (EMPI_Status *)s);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_RECV, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Recv failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

/* ------------------------ Request and status handling ------------------------ */


int MPI_Wait ( MPI_Request *r, MPI_Status *s )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
   MPI_Comm comm = eMPI_COMM_SELF;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Wait(MPI_Request *r=%p, MPI_Status *s=%p)", r, s);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
   // We need a communicator to attach the statistics to.
   comm = request_get_comm(r, MPI_COMM_SELF);
#endif

   int error = EMPI_Wait(r, (EMPI_Status *)s);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_WAIT, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Wait failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int MPI_Waitall ( int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Waitall(int count=%d, MPI_Request array_of_requests[]=%p, MPI_Status array_of_statuses[]=%p)", count, array_of_requests, array_of_statuses);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
#ifndef IBIS_INTERCEPT
   profile_start = profile_start_ticks();
#endif // IBIS_INTERCEPT
#endif // PROFILE_LEVEL

   int error = EMPI_Waitall(count, array_of_requests, (EMPI_Status *)array_of_statuses);

#if PROFILE_LEVEL > 0
#ifndef IBIS_INTERCEPT
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_WAITALL, profile_end-profile_start);
#endif // IBIS_INTERCEPT
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Waitall failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int MPI_Request_free ( MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Request_free(MPI_Request *r=%p)", r);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Request_free(r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Request_free failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int MPI_Waitany ( int count, MPI_Request array_of_requests[], int *index, MPI_Status *s )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Waitany(int count=%d, MPI_Request array_of_requests[]=%p, int *index=%p, MPI_Status *s=%p)", count, array_of_requests, index, s);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Waitany(count, array_of_requests, index, (EMPI_Status *)s);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_WAITANY, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Waitany failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


/* ------------------------ Datatypes ------------------------ */


int MPI_Type_free ( MPI_Datatype *datatype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_free(MPI_Datatype *datatype=%p)", datatype);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_free(datatype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_free failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int MPI_Type_get_envelope ( MPI_Datatype type, int *num_integers, int *num_addresses, int *num_datatypes, int *combiner )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_get_envelope(MPI_Datatype type=%s, int *num_integers=%p, int *num_addresses=%p, int *num_datatypes=%p, int *combiner=%p)", type_to_string(type), num_integers, num_addresses, num_datatypes, combiner);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_get_envelope(type, num_integers, num_addresses, num_datatypes, combiner);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_get_envelope failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int MPI_Type_create_indexed_block ( int count, int blocklength, int array_of_displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_create_indexed_block(int count=%d, int blocklength=%d, int array_of_displacements[]=%p, MPI_Datatype oldtype=%s, MPI_Datatype *newtype=%p)", count, blocklength, array_of_displacements, type_to_string(oldtype), newtype);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_create_indexed_block(count, blocklength, array_of_displacements, oldtype, newtype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_create_indexed_block failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int MPI_Type_contiguous ( int count, MPI_Datatype old_type, MPI_Datatype *new_type_p )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_contiguous(int count=%d, MPI_Datatype old_type=%s, MPI_Datatype *new_type_p=%p)", count, type_to_string(old_type), new_type_p);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_contiguous(count, old_type, new_type_p);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_contiguous failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_commit ( MPI_Datatype *datatype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_commit(MPI_Datatype *datatype=%p)", datatype);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_commit(datatype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_commit failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int MPI_Type_get_name ( MPI_Datatype type, char *type_name, int *resultlen )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_get_name(MPI_Datatype type=%s, char *type_name=%p, int *resultlen=%p)", type_to_string(type), type_name, resultlen);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_get_name(type, type_name, resultlen);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_get_name failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

/* ------------------------ Files ------------------------ */

int MPI_File_set_view ( MPI_File MPI_fh, MPI_Offset disp, MPI_Datatype etype, MPI_Datatype filetype, char *datarep, MPI_Info info )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_set_view(MPI_File MPI_fh=%s, MPI_Offset disp=%p, MPI_Datatype etype=%s, MPI_Datatype filetype=%s, char *datarep=%p, MPI_Info info=%s)", file_to_string(MPI_fh), disp, type_to_string(etype), type_to_string(filetype), datarep, info_to_string(info));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_set_view(MPI_fh, disp, etype, filetype, datarep, info);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_set_view failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_open ( MPI_Comm comm, char *filename, int amode, MPI_Info info, MPI_File *fh )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_open(MPI_Comm comm=%s, char *filename=%p, int amode=%d, MPI_Info info=%s, MPI_File *fh=%p)", comm_to_string(comm), filename, amode, info_to_string(info), fh);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_open(comm, filename, amode, info, fh);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_open failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int MPI_File_close ( MPI_File *fh )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_close(MPI_File *fh=%p)", fh);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_close(fh);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_close failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int MPI_File_write_at ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype type, MPI_Status *s )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_write_at(MPI_File MPI_fh=%s, MPI_Offset offset=%p, void *buf=%p, int count=%d, MPI_Datatype type=%s, MPI_Status *s=%p)", file_to_string(MPI_fh), offset, buf, count, type_to_string(type), s);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_write_at(MPI_fh, offset, buf, count, type, (EMPI_Status *)s);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_write_at failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int MPI_File_read_at ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype type, MPI_Status *s )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_read_at(MPI_File MPI_fh=%s, MPI_Offset offset=%p, void *buf=%p, int count=%d, MPI_Datatype type=%s, MPI_Status *s=%p)", file_to_string(MPI_fh), offset, buf, count, type_to_string(type), s);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_read_at(MPI_fh, offset, buf, count, type, (EMPI_Status *)s);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_read_at failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int MPI_File_read_all ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype type, MPI_Status *s )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_read_all(MPI_File MPI_fh=%s, void *buf=%p, int count=%d, MPI_Datatype type=%s, MPI_Status *s=%p)", file_to_string(MPI_fh), buf, count, type_to_string(type), s);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_read_all(MPI_fh, buf, count, type, (EMPI_Status *)s);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_read_all failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_write_all ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype type, MPI_Status *s )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_write_all(MPI_File MPI_fh=%s, void *buf=%p, int count=%d, MPI_Datatype type=%s, MPI_Status *s=%p)", file_to_string(MPI_fh), buf, count, type_to_string(type), s);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_write_all(MPI_fh, buf, count, type, (EMPI_Status *)s);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_write_all failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}



/* ------------------------ Info ------------------------ */


int MPI_Info_create ( MPI_Info *info )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Info_create(MPI_Info *info=%p)", info);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL


   int error = EMPI_Info_create(info);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Info_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Info_delete ( MPI_Info info, char *key )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Info_delete(MPI_Info info=%s, char *key=%p)", info_to_string(info), key);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Info_delete(info, key);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Info_delete failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Info_set ( MPI_Info info, char *key, char *value )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Info_set(MPI_Info info=%s, char *key=%p, char *value=%p)", info_to_string(info), key, value);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Info_set(info, key, value);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Info_set failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int MPI_Info_free ( MPI_Info *info )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Info_free(MPI_Info *info=%p)", info);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Info_free(info);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Info_free failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}



/* ------------------------ Intercomm ------------------------ */


int MPI_Intercomm_create ( MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Intercomm_create(MPI_Comm local_comm=%s, int local_leader=%d, MPI_Comm peer_comm=%s, int remote_leader=%d, int tag=%d, MPI_Comm *newintercomm=%p)", comm_to_string(local_comm), local_leader, comm_to_string(peer_comm), remote_leader, tag, newintercomm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Intercomm_create(local_comm, local_leader, peer_comm, remote_leader, tag, newintercomm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(local_comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Intercomm_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Intercomm_merge ( MPI_Comm intercomm, int high, MPI_Comm *newintracomm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Intercomm_merge(MPI_Comm intercomm=%s, int high=%d, MPI_Comm *newintracomm=%p)", comm_to_string(intercomm), high, newintracomm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Intercomm_merge(intercomm, high, newintracomm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(intercomm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Intercomm_merge failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}






// HIERO



// Ignore the rest for now....

#if 0

int MPI_Type_create_f90_complex ( int p, int r, MPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_create_f90_complex(int p=%d, int r=%d, MPI_Datatype *newtype=%p)", p, r, newtype);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_create_f90_complex(p, r, newtype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_create_f90_complex failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_create_f90_integer ( int r, MPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_create_f90_integer(int r=%d, MPI_Datatype *newtype=%p)", r, newtype);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_create_f90_integer(r, newtype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_create_f90_integer failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_create_f90_real ( int p, int r, MPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_create_f90_real(int p=%d, int r=%d, MPI_Datatype *newtype=%p)", p, r, newtype);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_create_f90_real(p, r, newtype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_create_f90_real failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}




int MPI_Accumulate ( void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Op op, MPI_Win win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Accumulate(void *origin_addr=%p, int origin_count=%d, MPI_Datatype origin_datatype=%s, int target_rank=%d, MPI_Aint target_disp=%p, int target_count=%d, MPI_Datatype target_datatype=%s, MPI_Op op=%s, MPI_Win win=%s)", origin_addr, origin_count, type_to_string(origin_datatype), target_rank, (void *) target_disp, target_count, type_to_string(target_datatype), op_to_string(op), win_to_string(win));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Accumulate(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, op, win);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Accumulate failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Add_error_class ( int *errorclass )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Add_error_class(int *errorclass=%p)", errorclass);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Add_error_class(errorclass);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Add_error_class failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Add_error_code ( int errorclass, int *errorcode )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Add_error_code(int errorclass=%d, int *errorcode=%p)", errorclass, errorcode);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Add_error_code(errorclass, errorcode);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Add_error_code failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Add_error_string ( int errorcode, char *string )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Add_error_string(int errorcode=%d, char *string=%p)", errorcode, string);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Add_error_string(errorcode, string);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Add_error_string failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Address ( void *location, MPI_Aint *address )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Address(void *location=%p, MPI_Aint *address=%p)", location, address);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Address(location, address);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Address failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}



int MPI_Alloc_mem ( MPI_Aint size, MPI_Info info, void *baseptr )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Alloc_mem(MPI_Aint size=%p, MPI_Info info=%s, void *baseptr=%p)", (void *) size, info_to_string(info), baseptr);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Alloc_mem(size, info, baseptr);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Alloc_mem failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}




int MPI_Attr_delete ( MPI_Comm comm, int keyval )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Attr_delete(MPI_Comm comm=%s, int keyval=%d)", comm_to_string(comm), keyval);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Attr_delete(comm, keyval);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Attr_delete failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Attr_get ( MPI_Comm comm, int keyval, void *attr_value, int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Attr_get(MPI_Comm comm=%s, int keyval=%d, void *attr_value=%p, int *flag=%p)", comm_to_string(comm), keyval, attr_value, flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Attr_get(comm, keyval, attr_value, flag);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Attr_get failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Attr_put ( MPI_Comm comm, int keyval, void *attr_value )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Attr_put(MPI_Comm comm=%s, int keyval=%d, void *attr_value=%p)", comm_to_string(comm), keyval, attr_value);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Attr_put(comm, keyval, attr_value);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Attr_put failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}




int MPI_Bsend ( void *buf, int count, MPI_Datatype type, int dest, int tag, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Bsend(void *buf=%p, int count=%d, MPI_Datatype type=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Bsend(buf, count, type, dest, tag, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_BSEND, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Bsend failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Bsend_init ( void *buf, int count, MPI_Datatype type, int dest, int tag, MPI_Comm comm, MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Bsend_init(void *buf=%p, int count=%d, MPI_Datatype type=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s, MPI_Request *r=%p)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm), r);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Bsend_init(buf, count, type, dest, tag, comm, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Bsend_init failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Buffer_attach ( void *buffer, int size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Buffer_attach(void *buffer=%p, int size=%d)", buffer, size);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Buffer_attach(buffer, size);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Buffer_attach failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Buffer_detach ( void *buffer, int *size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Buffer_detach(void *buffer=%p, int *size=%p)", buffer, size);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Buffer_detach(buffer, size);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Buffer_detach failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Cancel ( MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Cancel(MPI_Request *r=%p)", r);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Cancel(r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Cancel failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Cart_coords ( MPI_Comm comm, int rank, int maxdims, int *coords )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Cart_coords(MPI_Comm comm=%s, int rank=%d, int maxdims=%d, int *coords=%p)", comm_to_string(comm), rank, maxdims, coords);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Cart_coords(comm, rank, maxdims, coords);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Cart_coords failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Cart_create ( MPI_Comm comm_old, int ndims, int *dims, int *periods, int reorder, MPI_Comm *comm_cart )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Cart_create(MPI_Comm comm_old=%s, int ndims=%d, int *dims=%p, int *periods=%p, int reorder=%d, MPI_Comm *comm_cart=%p)", comm_to_string(comm_old), ndims, dims, periods, reorder, comm_cart);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Cart_create(comm_old, ndims, dims, periods, reorder, comm_cart);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm_old, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Cart_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Cartdim_get ( MPI_Comm comm, int *ndims )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Cartdim_get(MPI_Comm comm=%s, int *ndims=%p)", comm_to_string(comm), ndims);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Cartdim_get(comm, ndims);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Cartdim_get failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Cart_get ( MPI_Comm comm, int maxdims, int *dims, int *periods, int *coords )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Cart_get(MPI_Comm comm=%s, int maxdims=%d, int *dims=%p, int *periods=%p, int *coords=%p)", comm_to_string(comm), maxdims, dims, periods, coords);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Cart_get(comm, maxdims, dims, periods, coords);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Cart_get failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Cart_map ( MPI_Comm comm_old, int ndims, int *dims, int *periods, int *newrank )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Cart_map(MPI_Comm comm_old=%s, int ndims=%d, int *dims=%p, int *periods=%p, int *newrank=%p)", comm_to_string(comm_old), ndims, dims, periods, newrank);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Cart_map(comm_old, ndims, dims, periods, newrank);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm_old, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Cart_map failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Cart_rank ( MPI_Comm comm, int *coords, int *rank )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Cart_rank(MPI_Comm comm=%s, int *coords=%p, int *rank=%p)", comm_to_string(comm), coords, rank);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Cart_rank(comm, coords, rank);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Cart_rank failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Cart_shift ( MPI_Comm comm, int direction, int displ, int *source, int *dest )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Cart_shift(MPI_Comm comm=%s, int direction=%d, int displ=%d, int *source=%p, int *dest=%p)", comm_to_string(comm), direction, displ, source, dest);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Cart_shift(comm, direction, displ, source, dest);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Cart_shift failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Cart_sub ( MPI_Comm comm, int *remain_dims, MPI_Comm *comm_new )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Cart_sub(MPI_Comm comm=%s, int *remain_dims=%p, MPI_Comm *comm_new=%p)", comm_to_string(comm), remain_dims, comm_new);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Cart_sub(comm, remain_dims, comm_new);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Cart_sub failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Close_port ( char *port_name )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Close_port(char *port_name=%p)", port_name);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Close_port(port_name);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Close_port failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_accept ( char *port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *newcomm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_accept(char *port_name=%p, MPI_Info info=%s, int root=%d, MPI_Comm comm=%s, MPI_Comm *newcomm=%p)", port_name, info_to_string(info), root, comm_to_string(comm), newcomm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Comm_accept(port_name, info, root, comm, newcomm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_accept failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_call_errhandler ( MPI_Comm comm, int errorcode )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_call_errhandler(MPI_Comm comm=%s, int errorcode=%d)", comm_to_string(comm), errorcode);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Comm_call_errhandler(comm, errorcode);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_call_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_compare ( MPI_Comm comm1, MPI_Comm comm2, int *result )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_compare(MPI_Comm comm1=%s, MPI_Comm comm2=%s, int *result=%p)", comm_to_string(comm1), comm_to_string(comm2), result);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Comm_compare(comm1, comm2, result);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm1, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_compare failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_connect ( char *port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *newcomm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_connect(char *port_name=%p, MPI_Info info=%s, int root=%d, MPI_Comm comm=%s, MPI_Comm *newcomm=%p)", port_name, info_to_string(info), root, comm_to_string(comm), newcomm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Comm_connect(port_name, info, root, comm, newcomm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_connect failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_create_errhandler ( MPI_Comm_errhandler_fn *function, MPI_Errhandler *errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_create_errhandler(MPI_Comm_errhandler_fn *function=%p, MPI_Errhandler *errhandler=%p)", function, errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Comm_create_errhandler(function, errhandler);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_create_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}




int MPI_Comm_create_keyval ( MPI_Comm_copy_attr_function *comm_copy_attr_fn, MPI_Comm_delete_attr_function *comm_delete_attr_fn, int *comm_keyval, void *extra_state )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_create_keyval(MPI_Comm_copy_attr_function *comm_copy_attr_fn=%p, MPI_Comm_delete_attr_function *comm_delete_attr_fn=%p, int *comm_keyval=%p, void *extra_state=%p)", comm_copy_attr_fn, comm_delete_attr_fn, comm_keyval, extra_state);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Comm_create_keyval(comm_copy_attr_fn, comm_delete_attr_fn, comm_keyval, extra_state);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_create_keyval failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_delete_attr ( MPI_Comm comm, int comm_keyval )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_delete_attr(MPI_Comm comm=%s, int comm_keyval=%d)", comm_to_string(comm), comm_keyval);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Comm_delete_attr(comm, comm_keyval);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_delete_attr failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_disconnect ( MPI_Comm *comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_disconnect(MPI_Comm *comm=%p)", comm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Comm_disconnect(comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_disconnect failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}



int MPI_Comm_free_keyval ( int *comm_keyval )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_free_keyval(int *comm_keyval=%p)", comm_keyval);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Comm_free_keyval(comm_keyval);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_free_keyval failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_get_attr ( MPI_Comm comm, int comm_keyval, void *attribute_val, int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_get_attr(MPI_Comm comm=%s, int comm_keyval=%d, void *attribute_val=%p, int *flag=%p)", comm_to_string(comm), comm_keyval, attribute_val, flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Comm_get_attr(comm, comm_keyval, attribute_val, flag);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_get_attr failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_get_errhandler ( MPI_Comm comm, MPI_Errhandler *errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_get_errhandler(MPI_Comm comm=%s, MPI_Errhandler *errhandler=%p)", comm_to_string(comm), errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Comm_get_errhandler(comm, errhandler);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_get_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_get_name ( MPI_Comm comm, char *comm_name, int *resultlen )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_get_name(MPI_Comm comm=%s, char *comm_name=%p, int *resultlen=%p)", comm_to_string(comm), comm_name, resultlen);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Comm_get_name(comm, comm_name, resultlen);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_get_name failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_get_parent ( MPI_Comm *parent )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_get_parent(MPI_Comm *parent=%p)", parent);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Comm_get_parent(parent);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_get_parent failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}




int MPI_Comm_join ( int fd, MPI_Comm *intercomm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_join(int fd=%d, MPI_Comm *intercomm=%p)", fd, intercomm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Comm_join(fd, intercomm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_join failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}



int MPI_Comm_remote_group ( MPI_Comm comm, MPI_Group *g )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_remote_group(MPI_Comm comm=%s, MPI_Group *g=%p)", comm_to_string(comm), g);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Comm_remote_group(comm, g);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_remote_group failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_remote_size ( MPI_Comm comm, int *size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_remote_size(MPI_Comm comm=%s, int *size=%p)", comm_to_string(comm), size);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Comm_remote_size(comm, size);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_remote_size failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_set_attr ( MPI_Comm comm, int comm_keyval, void *attribute_val )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_set_attr(MPI_Comm comm=%s, int comm_keyval=%d, void *attribute_val=%p)", comm_to_string(comm), comm_keyval, attribute_val);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Comm_set_attr(comm, comm_keyval, attribute_val);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_set_attr failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_set_errhandler ( MPI_Comm comm, MPI_Errhandler errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_set_errhandler(MPI_Comm comm=%s, MPI_Errhandler errhandler=%p)", comm_to_string(comm), (void *) errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Comm_set_errhandler(comm, errhandler);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_set_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_set_name ( MPI_Comm comm, char *comm_name )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_set_name(MPI_Comm comm=%s, char *comm_name=%p)", comm_to_string(comm), comm_name);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Comm_set_name(comm, comm_name);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_set_name failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}




int MPI_Comm_spawn ( char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[] )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_spawn(char *command=%p, char *argv[]=%p, int maxprocs=%d, MPI_Info info=%s, int root=%d, MPI_Comm comm=%s, MPI_Comm *intercomm=%p, int array_of_errcodes[]=%p)", command, argv, maxprocs, info_to_string(info), root, comm_to_string(comm), intercomm, array_of_errcodes);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Comm_spawn(command, argv, maxprocs, info, root, comm, intercomm, array_of_errcodes);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_spawn failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_spawn_multiple ( int count, char *array_of_commands[], char* *array_of_argv[], int array_of_maxprocs[], MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[] )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_spawn_multiple(int count=%d, char *array_of_commands[]=%p, char* *array_of_argv[]=%p, int array_of_maxprocs[]=%p, MPI_Info array_of_info[]=%p, int root=%d, MPI_Comm comm=%s, MPI_Comm *intercomm=%p, int array_of_errcodes[]=%p)", count, array_of_commands, array_of_argv, array_of_maxprocs, array_of_info, root, comm_to_string(comm), intercomm, array_of_errcodes);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Comm_spawn_multiple(count, array_of_commands, array_of_argv, array_of_maxprocs, array_of_info, root, comm, intercomm, array_of_errcodes);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_spawn_multiple failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}



int MPI_Comm_test_inter ( MPI_Comm comm, int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_test_inter(MPI_Comm comm=%s, int *flag=%p)", comm_to_string(comm), flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Comm_test_inter(comm, flag);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_test_inter failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Dims_create ( int nnodes, int ndims, int *dims )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Dims_create(int nnodes=%d, int ndims=%d, int *dims=%p)", nnodes, ndims, dims);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Dims_create(nnodes, ndims, dims);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Dims_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

/*
int MPI_Dist_graph_create_adjacent ( MPI_Comm comm_old, int indegree, int sources[], int sourceweights[], int outdegree, int destinations[], int destweights[], MPI_Info info, int reorder, MPI_Comm *comm_dist_graph )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Dist_graph_create_adjacent(MPI_Comm comm_old=%s, int indegree=%d, int sources[]=%p, int sourceweights[]=%p, int outdegree=%d, int destinations[]=%p, int destweights[]=%p, MPI_Info info=%s, int reorder=%d, MPI_Comm *comm_dist_graph=%p)", comm_to_string(comm_old), indegree, sources, sourceweights, outdegree, destinations, destweights, info_to_string(info), reorder, comm_dist_graph);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Dist_graph_create_adjacent(comm_old, indegree, sources, sourceweights, outdegree, destinations, destweights, info, reorder, comm_dist_graph);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm_old, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Dist_graph_create_adjacent failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}
*/
/*
int MPI_Dist_graph_create ( MPI_Comm comm_old, int n, int sources[], int degrees[], int destinations[], int weights[], MPI_Info info, int reorder, MPI_Comm *comm_dist_graph )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Dist_graph_create(MPI_Comm comm_old=%s, int n=%d, int sources[]=%p, int degrees[]=%p, int destinations[]=%p, int weights[]=%p, MPI_Info info=%s, int reorder=%d, MPI_Comm *comm_dist_graph=%p)", comm_to_string(comm_old), n, sources, degrees, destinations, weights, info_to_string(info), reorder, comm_dist_graph);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Dist_graph_create(comm_old, n, sources, degrees, destinations, weights, info, reorder, comm_dist_graph);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm_old, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Dist_graph_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}
*/
/*
int MPI_Dist_graph_neighbors_count ( MPI_Comm comm, int *indegree, int *outdegree, int *weighted )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Dist_graph_neighbors_count(MPI_Comm comm=%s, int *indegree=%p, int *outdegree=%p, int *weighted=%p)", comm_to_string(comm), indegree, outdegree, weighted);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Dist_graph_neighbors_count(comm, indegree, outdegree, weighted);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Dist_graph_neighbors_count failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}
*/
/*
int MPI_Dist_graph_neighbors ( MPI_Comm comm, int maxindegree, int sources[], int sourceweights[], int maxoutdegree, int destinations[], int destweights[] )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Dist_graph_neighbors(MPI_Comm comm=%s, int maxindegree=%d, int sources[]=%p, int sourceweights[]=%p, int maxoutdegree=%d, int destinations[]=%p, int destweights[]=%p)", comm_to_string(comm), maxindegree, sources, sourceweights, maxoutdegree, destinations, destweights);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Dist_graph_neighbors(comm, maxindegree, sources, sourceweights, maxoutdegree, destinations, destweights);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Dist_graph_neighbors failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}
*/

int MPI_Errhandler_create ( MPI_Handler_function *function, MPI_Errhandler *errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Errhandler_create(MPI_Handler_function *function=%p, MPI_Errhandler *errhandler=%p)", function, errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Errhandler_create(function, errhandler);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Errhandler_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Errhandler_free ( MPI_Errhandler *errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Errhandler_free(MPI_Errhandler *errhandler=%p)", errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Errhandler_free(errhandler);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Errhandler_free failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Errhandler_get ( MPI_Comm comm, MPI_Errhandler *errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Errhandler_get(MPI_Comm comm=%s, MPI_Errhandler *errhandler=%p)", comm_to_string(comm), errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Errhandler_get(comm, errhandler);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Errhandler_get failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Errhandler_set ( MPI_Comm comm, MPI_Errhandler errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Errhandler_set(MPI_Comm comm=%s, MPI_Errhandler errhandler=%p)", comm_to_string(comm), (void *) errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Errhandler_set(comm, errhandler);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Errhandler_set failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Error_class ( int errorcode, int *errorclass )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Error_class(int errorcode=%d, int *errorclass=%p)", errorcode, errorclass);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Error_class(errorcode, errorclass);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Error_class failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}



int MPI_Exscan ( void *sendbuf, void *recvbuf, int count, MPI_Datatype type, MPI_Op op, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Exscan(void *sendbuf=%p, void *recvbuf=%p, int count=%d, MPI_Datatype type=%s, MPI_Op op=%s, MPI_Comm comm=%s)", sendbuf, recvbuf, count, type_to_string(datatype), op_to_string(op), comm_to_string(comm));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Exscan(sendbuf, recvbuf, count, type, op, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_EXSCAN, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Exscan failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_call_errhandler ( MPI_File fh, int errorcode )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_call_errhandler(MPI_File fh=%s, int errorcode=%d)", file_to_string(fh), errorcode);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_call_errhandler(fh, errorcode);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_call_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}




int MPI_File_create_errhandler ( MPI_File_errhandler_fn *function, MPI_Errhandler *errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_create_errhandler(MPI_File_errhandler_fn *function=%p, MPI_Errhandler *errhandler=%p)", function, errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_create_errhandler(function, errhandler);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_create_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_delete ( char *filename, MPI_Info info )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_delete(char *filename=%p, MPI_Info info=%s)", filename, info_to_string(info));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_delete(filename, info);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_delete failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_get_amode ( MPI_File MPI_fh, int *amode )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_get_amode(MPI_File MPI_fh=%s, int *amode=%p)", file_to_string(MPI_fh), amode);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_get_amode(MPI_fh, amode);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_get_amode failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_get_atomicity ( MPI_File MPI_fh, int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_get_atomicity(MPI_File MPI_fh=%s, int *flag=%p)", file_to_string(MPI_fh), flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_get_atomicity(MPI_fh, flag);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_get_atomicity failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_get_byte_offset ( MPI_File MPI_fh, MPI_Offset offset, MPI_Offset *disp )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_get_byte_offset(MPI_File MPI_fh=%s, MPI_Offset offset=%p, MPI_Offset *disp=%p)", file_to_string(MPI_fh), (void *) offset, disp);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_get_byte_offset(MPI_fh, offset, disp);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_get_byte_offset failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_get_errhandler ( MPI_File file, MPI_Errhandler *errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_get_errhandler(MPI_File file=%s, MPI_Errhandler *errhandler=%p)", file_to_string(file), errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_get_errhandler(file, errhandler);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_get_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_get_group ( MPI_File MPI_fh, MPI_Group *g )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_get_group(MPI_File MPI_fh=%s, MPI_Group *g=%p)", file_to_string(MPI_fh), g);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_get_group(MPI_fh, g);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_get_group failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_get_info ( MPI_File MPI_fh, MPI_Info *info_used )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_get_info(MPI_File MPI_fh=%s, MPI_Info *info_used=%p)", file_to_string(MPI_fh), info_used);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_get_info(MPI_fh, info_used);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_get_info failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_get_position ( MPI_File MPI_fh, MPI_Offset *offset )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_get_position(MPI_File MPI_fh=%s, MPI_Offset *offset=%p)", file_to_string(MPI_fh), offset);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_get_position(MPI_fh, offset);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_get_position failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_get_position_shared ( MPI_File MPI_fh, MPI_Offset *offset )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_get_position_shared(MPI_File MPI_fh=%s, MPI_Offset *offset=%p)", file_to_string(MPI_fh), offset);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_get_position_shared(MPI_fh, offset);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_get_position_shared failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_get_size ( MPI_File MPI_fh, MPI_Offset *size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_get_size(MPI_File MPI_fh=%s, MPI_Offset *size=%p)", file_to_string(MPI_fh), size);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_get_size(MPI_fh, size);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_get_size failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_get_type_extent ( MPI_File MPI_fh, MPI_Datatype type, MPI_Aint *extent )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_get_type_extent(MPI_File MPI_fh=%s, MPI_Datatype type=%s, MPI_Aint *extent=%p)", file_to_string(MPI_fh), type_to_string(datatype), extent);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_get_type_extent(MPI_fh, type, extent);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_get_type_extent failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_get_view ( MPI_File MPI_fh, MPI_Offset *disp, MPI_Datatype *etype, MPI_Datatype *filetype, char *datarep )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_get_view(MPI_File MPI_fh=%s, MPI_Offset *disp=%p, MPI_Datatype *etype=%p, MPI_Datatype *filetype=%p, char *datarep=%p)", file_to_string(MPI_fh), disp, etype, filetype, datarep);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_get_view(MPI_fh, disp, etype, filetype, datarep);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_get_view failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_iread_at ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype type, MPIO_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_iread_at(MPI_File MPI_fh=%s, MPI_Offset offset=%p, void *buf=%p, int count=%d, MPI_Datatype type=%s, MPIO_Request *r=%p)", file_to_string(MPI_fh), (void *) offset, buf, count, type_to_string(datatype), r);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_iread_at(MPI_fh, offset, buf, count, type, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_iread_at failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_iread ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype type, MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_iread(MPI_File MPI_fh=%s, void *buf=%p, int count=%d, MPI_Datatype type=%s, MPI_Request *r=%p)", file_to_string(MPI_fh), buf, count, type_to_string(datatype), r);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_iread(MPI_fh, buf, count, type, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_iread failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_iread_shared ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype type, MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_iread_shared(MPI_File MPI_fh=%s, void *buf=%p, int count=%d, MPI_Datatype type=%s, MPI_Request *r=%p)", file_to_string(MPI_fh), buf, count, type_to_string(datatype), r);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_iread_shared(MPI_fh, buf, count, type, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_iread_shared failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_iwrite_at ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype type, MPIO_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_iwrite_at(MPI_File MPI_fh=%s, MPI_Offset offset=%p, void *buf=%p, int count=%d, MPI_Datatype type=%s, MPIO_Request *r=%p)", file_to_string(MPI_fh), (void *) offset, buf, count, type_to_string(datatype), r);
#endif // TRACE_CALLS


#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_iwrite_at(MPI_fh, offset, buf, count, type, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_iwrite_at failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_iwrite ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype type, MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_iwrite(MPI_File MPI_fh=%s, void *buf=%p, int count=%d, MPI_Datatype type=%s, MPI_Request *r=%p)", file_to_string(MPI_fh), buf, count, type_to_string(datatype), r);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_iwrite(MPI_fh, buf, count, type, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_iwrite failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_iwrite_shared ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype type, MPIO_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_iwrite_shared(MPI_File MPI_fh=%s, void *buf=%p, int count=%d, MPI_Datatype type=%s, MPIO_Request *r=%p)", file_to_string(MPI_fh), buf, count, type_to_string(datatype), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_iwrite_shared(MPI_fh, buf, count, type, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_iwrite_shared failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}




int MPI_File_preallocate ( MPI_File MPI_fh, MPI_Offset size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_preallocate(MPI_File MPI_fh=%s, MPI_Offset size=%p)", file_to_string(MPI_fh), (void *) size);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_preallocate(MPI_fh, size);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_preallocate failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_read_all_begin ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype type )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_read_all_begin(MPI_File MPI_fh=%s, void *buf=%p, int count=%d, MPI_Datatype type=%s)", file_to_string(MPI_fh), buf, count, type_to_string(datatype));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_read_all_begin(MPI_fh, buf, count, datatype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_read_all_begin failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_read_all_end ( MPI_File MPI_fh, void *buf, MPI_Status *s )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_read_all_end(MPI_File MPI_fh=%s, void *buf=%p, MPI_Status *s=%p)", file_to_string(MPI_fh), buf, s);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_read_all_end(MPI_fh, buf, (EMPI_Status *)s);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_read_all_end failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}




int MPI_File_read_at_all_begin ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype type )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_read_at_all_begin(MPI_File MPI_fh=%s, MPI_Offset offset=%p, void *buf=%p, int count=%d, MPI_Datatype type=%s)", file_to_string(MPI_fh), (void *) offset, buf, count, type_to_string(datatype));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_read_at_all_begin(MPI_fh, offset, buf, count, datatype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_read_at_all_begin failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_read_at_all_end ( MPI_File MPI_fh, void *buf, MPI_Status *s )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_read_at_all_end(MPI_File MPI_fh=%s, void *buf=%p, MPI_Status *s=%p)", file_to_string(MPI_fh), buf, s);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_read_at_all_end(MPI_fh, buf, (EMPI_Status *)s);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_read_at_all_end failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int MPI_File_read_at_all ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype type, MPI_Status *s )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_read_at_all(MPI_File MPI_fh=%s, MPI_Offset offset=%p, void *buf=%p, int count=%d, MPI_Datatype type=%s, MPI_Status *s=%p)", file_to_string(MPI_fh), (void *) offset, buf, count, type_to_string(datatype), s);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_read_at_all(MPI_fh, offset, buf, count, type, (EMPI_Status *)s);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_read_at_all failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}




int MPI_File_read ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype type, MPI_Status *s )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_read(MPI_File MPI_fh=%s, void *buf=%p, int count=%d, MPI_Datatype type=%s, MPI_Status *s=%p)", file_to_string(MPI_fh), buf, count, type_to_string(datatype), s);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_read(MPI_fh, buf, count, type, (EMPI_Status *)s);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_read failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_read_ordered_begin ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype type )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_read_ordered_begin(MPI_File MPI_fh=%s, void *buf=%p, int count=%d, MPI_Datatype type=%s)", file_to_string(MPI_fh), buf, count, type_to_string(datatype));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_read_ordered_begin(MPI_fh, buf, count, datatype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_read_ordered_begin failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_read_ordered_end ( MPI_File MPI_fh, void *buf, MPI_Status *s )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_read_ordered_end(MPI_File MPI_fh=%s, void *buf=%p, MPI_Status *s=%p)", file_to_string(MPI_fh), buf, s);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_read_ordered_end(MPI_fh, buf, (EMPI_Status *)s);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_read_ordered_end failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_read_ordered ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype type, MPI_Status *s )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_read_ordered(MPI_File MPI_fh=%s, void *buf=%p, int count=%d, MPI_Datatype type=%s, MPI_Status *s=%p)", file_to_string(MPI_fh), buf, count, type_to_string(datatype), s);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_read_ordered(MPI_fh, buf, count, type, (EMPI_Status *)s);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_read_ordered failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_read_shared ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype type, MPI_Status *s )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_read_shared(MPI_File MPI_fh=%s, void *buf=%p, int count=%d, MPI_Datatype type=%s, MPI_Status *s=%p)", file_to_string(MPI_fh), buf, count, type_to_string(datatype), s);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_read_shared(MPI_fh, buf, count, type, (EMPI_Status *)s);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_read_shared failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_seek ( MPI_File MPI_fh, MPI_Offset offset, int whence )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_seek(MPI_File MPI_fh=%s, MPI_Offset offset=%p, int whence=%d)", file_to_string(MPI_fh), (void *) offset, whence);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_seek(MPI_fh, offset, whence);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_seek failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_seek_shared ( MPI_File MPI_fh, MPI_Offset offset, int whence )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_seek_shared(MPI_File MPI_fh=%s, MPI_Offset offset=%p, int whence=%d)", file_to_string(MPI_fh), (void *) offset, whence);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_seek_shared(MPI_fh, offset, whence);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_seek_shared failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_set_atomicity ( MPI_File MPI_fh, int flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_set_atomicity(MPI_File MPI_fh=%s, int flag=%d)", file_to_string(MPI_fh), flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_set_atomicity(MPI_fh, flag);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_set_atomicity failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_set_errhandler ( MPI_File file, MPI_Errhandler errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_set_errhandler(MPI_File file=%s, MPI_Errhandler errhandler=%p)", file_to_string(file), (void *) errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_set_errhandler(file, errhandler);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_set_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_set_info ( MPI_File MPI_fh, MPI_Info info )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_set_info(MPI_File MPI_fh=%s, MPI_Info info=%s)", file_to_string(MPI_fh), info_to_string(info));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_set_info(MPI_fh, info);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_set_info failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_set_size ( MPI_File MPI_fh, MPI_Offset size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_set_size(MPI_File MPI_fh=%s, MPI_Offset size=%p)", file_to_string(MPI_fh), (void *) size);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_set_size(MPI_fh, size);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_set_size failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}



int MPI_File_sync ( MPI_File MPI_fh )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_sync(MPI_File MPI_fh=%s)", file_to_string(MPI_fh));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_sync(MPI_fh);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_sync failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_write_all_begin ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype type )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_write_all_begin(MPI_File MPI_fh=%s, void *buf=%p, int count=%d, MPI_Datatype type=%s)", file_to_string(MPI_fh), buf, count, type_to_string(datatype));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_write_all_begin(MPI_fh, buf, count, datatype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_write_all_begin failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_write_all_end ( MPI_File MPI_fh, void *buf, MPI_Status *s )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_write_all_end(MPI_File MPI_fh=%s, void *buf=%p, MPI_Status *s=%p)", file_to_string(MPI_fh), buf, s);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_write_all_end(MPI_fh, buf, (EMPI_Status *)s);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_write_all_end failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}




int MPI_File_write_at_all_begin ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype type )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_write_at_all_begin(MPI_File MPI_fh=%s, MPI_Offset offset=%p, void *buf=%p, int count=%d, MPI_Datatype type=%s)", file_to_string(MPI_fh), (void *) offset, buf, count, type_to_string(datatype));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_write_at_all_begin(MPI_fh, offset, buf, count, datatype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_write_at_all_begin failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_write_at_all_end ( MPI_File MPI_fh, void *buf, MPI_Status *s )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_write_at_all_end(MPI_File MPI_fh=%s, void *buf=%p, MPI_Status *s=%p)", file_to_string(MPI_fh), buf, s);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_write_at_all_end(MPI_fh, buf, (EMPI_Status *)s);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_write_at_all_end failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_write_at_all ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype type, MPI_Status *s )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_write_at_all(MPI_File MPI_fh=%s, MPI_Offset offset=%p, void *buf=%p, int count=%d, MPI_Datatype type=%s, MPI_Status *s=%p)", file_to_string(MPI_fh), (void *) offset, buf, count, type_to_string(datatype), s);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_write_at_all(MPI_fh, offset, buf, count, type, (EMPI_Status *)s);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_write_at_all failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}




int MPI_File_write ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype type, MPI_Status *s )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_write(MPI_File MPI_fh=%s, void *buf=%p, int count=%d, MPI_Datatype type=%s, MPI_Status *s=%p)", file_to_string(MPI_fh), buf, count, type_to_string(datatype), s);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_write(MPI_fh, buf, count, type, (EMPI_Status *)s);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_write failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_write_ordered_begin ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype type )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_write_ordered_begin(MPI_File MPI_fh=%s, void *buf=%p, int count=%d, MPI_Datatype type=%s)", file_to_string(MPI_fh), buf, count, type_to_string(datatype));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_write_ordered_begin(MPI_fh, buf, count, datatype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_write_ordered_begin failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_write_ordered_end ( MPI_File MPI_fh, void *buf, MPI_Status *s )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_write_ordered_end(MPI_File MPI_fh=%s, void *buf=%p, MPI_Status *s=%p)", file_to_string(MPI_fh), buf, s);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_write_ordered_end(MPI_fh, buf, (EMPI_Status *)s);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_write_ordered_end failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_write_ordered ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype type, MPI_Status *s )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_write_ordered(MPI_File MPI_fh=%s, void *buf=%p, int count=%d, MPI_Datatype type=%s, MPI_Status *s=%p)", file_to_string(MPI_fh), buf, count, type_to_string(datatype), s);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_write_ordered(MPI_fh, buf, count, type, (EMPI_Status *)s);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_write_ordered failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_write_shared ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype type, MPI_Status *s )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_write_shared(MPI_File MPI_fh=%s, void *buf=%p, int count=%d, MPI_Datatype type=%s, MPI_Status *s=%p)", file_to_string(MPI_fh), buf, count, type_to_string(datatype), s);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_File_write_shared(MPI_fh, buf, count, type, (EMPI_Status *)s);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_write_shared failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}



int MPI_Free_mem ( void *base )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Free_mem(void *base=%p)", base);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Free_mem(base);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Free_mem failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}



int MPI_Get_address ( void *location, MPI_Aint *address )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Get_address(void *location=%p, MPI_Aint *address=%p)", location, address);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Get_address(location, address);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Get_address failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Get_count ( MPI_Status *s, MPI_Datatype type, int *count )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Get_count(MPI_Status *s=%p, MPI_Datatype type=%s, int *count=%p)", s, type_to_string(datatype), count);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Get_count((EMPI_Status *)s, type, count);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Get_count failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Get_elements ( MPI_Status *s, MPI_Datatype type, int *elements )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Get_elements(MPI_Status *s=%p, MPI_Datatype type=%s, int *elements=%p)", s, type_to_string(datatype), elements);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Get_elements((EMPI_Status *)s, type, elements);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Get_elements failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Get ( void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Get(void *origin_addr=%p, int origin_count=%d, MPI_Datatype origin_datatype=%s, int target_rank=%d, MPI_Aint target_disp=%p, int target_count=%d, MPI_Datatype target_datatype=%s, MPI_Win win=%s)", origin_addr, origin_count, type_to_string(origin_datatype), target_rank, (void *) target_disp, target_count, type_to_string(target_datatype), win_to_string(win));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(origin_datatype);
   CHECK_TYPE(target_datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Get(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Get failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}




int MPI_Get_version ( int *version, int *subversion )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Get_version(int *version=%p, int *subversion=%p)", version, subversion);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Get_version(version, subversion);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Get_version failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Graph_create ( MPI_Comm comm_old, int nnodes, int *indx, int *edges, int reorder, MPI_Comm *comm_graph )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Graph_create(MPI_Comm comm_old=%s, int nnodes=%d, int *indx=%p, int *edges=%p, int reorder=%d, MPI_Comm *comm_graph=%p)", comm_to_string(comm_old), nnodes, indx, edges, reorder, comm_graph);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Graph_create(comm_old, nnodes, indx, edges, reorder, comm_graph);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm_old, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Graph_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Graphdims_get ( MPI_Comm comm, int *nnodes, int *nedges )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Graphdims_get(MPI_Comm comm=%s, int *nnodes=%p, int *nedges=%p)", comm_to_string(comm), nnodes, nedges);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Graphdims_get(comm, nnodes, nedges);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Graphdims_get failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Graph_get ( MPI_Comm comm, int maxindex, int maxedges, int *indx, int *edges )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Graph_get(MPI_Comm comm=%s, int maxindex=%d, int maxedges=%d, int *indx=%p, int *edges=%p)", comm_to_string(comm), maxindex, maxedges, indx, edges);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Graph_get(comm, maxindex, maxedges, indx, edges);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Graph_get failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Graph_map ( MPI_Comm comm_old, int nnodes, int *indx, int *edges, int *newrank )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Graph_map(MPI_Comm comm_old=%s, int nnodes=%d, int *indx=%p, int *edges=%p, int *newrank=%p)", comm_to_string(comm_old), nnodes, indx, edges, newrank);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Graph_map(comm_old, nnodes, indx, edges, newrank);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm_old, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Graph_map failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Graph_neighbors_count ( MPI_Comm comm, int rank, int *nneighbors )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Graph_neighbors_count(MPI_Comm comm=%s, int rank=%d, int *nneighbors=%p)", comm_to_string(comm), rank, nneighbors);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Graph_neighbors_count(comm, rank, nneighbors);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Graph_neighbors_count failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Graph_neighbors ( MPI_Comm comm, int rank, int maxneighbors, int *neighbors )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Graph_neighbors(MPI_Comm comm=%s, int rank=%d, int maxneighbors=%d, int *neighbors=%p)", comm_to_string(comm), rank, maxneighbors, neighbors);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Graph_neighbors(comm, rank, maxneighbors, neighbors);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Graph_neighbors failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Grequest_complete ( MPI_Request r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Grequest_complete(MPI_Request r=%s)", request_to_string(r));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Grequest_complete(r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Grequest_complete failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Grequest_start ( MPI_Grequest_query_function *query_fn, MPI_Grequest_free_function *free_fn, MPI_Grequest_cancel_function *cancel_fn, void *extra_state, MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Grequest_start(MPI_Grequest_query_function *query_fn=%p, MPI_Grequest_free_function *free_fn=%p, MPI_Grequest_cancel_function *cancel_fn=%p, void *extra_state=%p, MPI_Request *r=%p)", query_fn, free_fn, cancel_fn, extra_state, r);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Grequest_start(query_fn, free_fn, cancel_fn, extra_state, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Grequest_start failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Group_compare ( MPI_Group group1, MPI_Group group2, int *result )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Group_compare(MPI_Group group1=%s, MPI_Group group2=%s, int *result=%p)", group_to_string(group1), group_to_string(group2), result);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Group_compare(group1, group2, result);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Group_compare failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Group_difference ( MPI_Group group1, MPI_Group group2, MPI_Group *newgroup )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Group_difference(MPI_Group group1=%s, MPI_Group group2=%s, MPI_Group *newgroup=%p)", group_to_string(group1), group_to_string(group2), newgroup);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Group_difference(group1, group2, newgroup);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Group_difference failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Group_excl ( MPI_Group g, int n, int *ranks, MPI_Group *newgroup )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Group_excl(MPI_Group g=%s, int n=%d, int *ranks=%p, MPI_Group *newgroup=%p)", group_to_string(g), n, ranks, newgroup);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Group_excl(g, n, ranks, newgroup);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Group_excl failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Group_free ( MPI_Group *g )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Group_free(MPI_Group *g=%p)", g);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Group_free(g);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Group_free failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}



int MPI_Group_intersection ( MPI_Group group1, MPI_Group group2, MPI_Group *newgroup )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Group_intersection(MPI_Group group1=%s, MPI_Group group2=%s, MPI_Group *newgroup=%p)", group_to_string(group1), group_to_string(group2), newgroup);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Group_intersection(group1, group2, newgroup);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Group_intersection failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}





int MPI_Group_rank ( MPI_Group g, int *rank )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Group_rank(MPI_Group g=%s, int *rank=%p)", group_to_string(g), rank);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Group_rank(g, rank);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Group_rank failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Group_size ( MPI_Group g, int *size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Group_size(MPI_Group g=%s, int *size=%p)", group_to_string(g), size);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Group_size(g, size);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Group_size failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}






int MPI_Info_dup ( MPI_Info info, MPI_Info *newinfo )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Info_dup(MPI_Info info=%s, MPI_Info *newinfo=%p)", info_to_string(info), newinfo);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Info_dup(info, newinfo);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Info_dup failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}



int MPI_Info_get ( MPI_Info info, char *key, int valuelen, char *value, int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Info_get(MPI_Info info=%s, char *key=%p, int valuelen=%d, char *value=%p, int *flag=%p)", info_to_string(info), key, valuelen, value, flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Info_get(info, key, valuelen, value, flag);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Info_get failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Info_get_nkeys ( MPI_Info info, int *nkeys )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Info_get_nkeys(MPI_Info info=%s, int *nkeys=%p)", info_to_string(info), nkeys);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Info_get_nkeys(info, nkeys);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Info_get_nkeys failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Info_get_nthkey ( MPI_Info info, int n, char *key )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Info_get_nthkey(MPI_Info info=%s, int n=%d, char *key=%p)", info_to_string(info), n, key);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Info_get_nthkey(info, n, key);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Info_get_nthkey failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Info_get_valuelen ( MPI_Info info, char *key, int *valuelen, int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Info_get_valuelen(MPI_Info info=%s, char *key=%p, int *valuelen=%p, int *flag=%p)", info_to_string(info), key, valuelen, flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Info_get_valuelen(info, key, valuelen, flag);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Info_get_valuelen failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}





int MPI_Init_thread ( int *argc, char ***argv, int required, int *provided )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Init_thread(int *argc=%p, char ***argv=%p, int required=%d, int *provided=%p)", argc, argv, required, provided);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Init_thread(argc, argv, required, provided);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Init_thread failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Iprobe ( int source, int tag, MPI_Comm comm, int *flag, MPI_Status *s )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Iprobe(int source=%d, int tag=%d, MPI_Comm comm=%s, int *flag=%p, MPI_Status *s=%p)", source, tag, comm_to_string(comm), flag, s);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Iprobe(source, tag, comm, flag, (EMPI_Status *)s);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_IPROBE, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Iprobe failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}




int MPI_Issend ( void *buf, int count, MPI_Datatype type, int dest, int tag, MPI_Comm comm, MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Issend(void *buf=%p, int count=%d, MPI_Datatype type=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s, MPI_Request *r=%p)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Issend(buf, count, type, dest, tag, comm, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_ISSEND, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Issend failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Is_thread_main ( int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Is_thread_main(int *flag=%p)", flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Is_thread_main(flag);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Is_thread_main failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Keyval_create ( MPI_Copy_function *copy_fn, MPI_Delete_function *delete_fn, int *keyval, void *extra_state )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Keyval_create(MPI_Copy_function *copy_fn=%p, MPI_Delete_function *delete_fn=%p, int *keyval=%p, void *extra_state=%p)", copy_fn, delete_fn, keyval, extra_state);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Keyval_create(copy_fn, delete_fn, keyval, extra_state);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Keyval_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Keyval_free ( int *keyval )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Keyval_free(int *keyval=%p)", keyval);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Keyval_free(keyval);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Keyval_free failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Lookup_name ( char *service_name, MPI_Info info, char *port_name )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Lookup_name(char *service_name=%p, MPI_Info info=%s, char *port_name=%p)", service_name, info_to_string(info), port_name);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Lookup_name(service_name, info, port_name);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Lookup_name failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

/*
int MPI_Op_commutative ( MPI_Op op, int *commute )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Op_commutative(MPI_Op op=%s, int *commute=%p)", op_to_string(op), commute);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Op_commutative(op, commute);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Op_commutative failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}
*/

int MPI_Op_create ( MPI_User_function *function, int commute, MPI_Op *op )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Op_create(MPI_User_function *function=%p, int commute=%d, MPI_Op *op=%p)", function, commute, op);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Op_create(function, commute, op);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Op_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Open_port ( MPI_Info info, char *port_name )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Open_port(MPI_Info info=%s, char *port_name=%p)", info_to_string(info), port_name);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Open_port(info, port_name);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Open_port failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Op_free ( MPI_Op *op )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Op_free(MPI_Op *op=%p)", op);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Op_free(op);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Op_free failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Pack_external ( char *datarep, void *inbuf, int incount, MPI_Datatype type, void *outbuf, MPI_Aint outcount, MPI_Aint *position )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Pack_external(char *datarep=%p, void *inbuf=%p, int incount=%d, MPI_Datatype type=%s, void *outbuf=%p, MPI_Aint outcount=%p, MPI_Aint *position=%p)", datarep, inbuf, incount, type_to_string(datatype), outbuf, (void *) outcount, position);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Pack_external(datarep, inbuf, incount, type, outbuf, outcount, position);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Pack_external failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Pack_external_size ( char *datarep, int incount, MPI_Datatype type, MPI_Aint *size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Pack_external_size(char *datarep=%p, int incount=%d, MPI_Datatype type=%s, MPI_Aint *size=%p)", datarep, incount, type_to_string(datatype), size);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Pack_external_size(datarep, incount, type, size);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Pack_external_size failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Pack ( void *inbuf, int incount, MPI_Datatype type, void *outbuf, int outcount, int *position, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Pack(void *inbuf=%p, int incount=%d, MPI_Datatype type=%s, void *outbuf=%p, int outcount=%d, int *position=%p, MPI_Comm comm=%s)", inbuf, incount, type_to_string(datatype), outbuf, outcount, position, comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Pack(inbuf, incount, type, outbuf, outcount, position, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Pack failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Pack_size ( int incount, MPI_Datatype type, MPI_Comm comm, int *size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Pack_size(int incount=%d, MPI_Datatype type=%s, MPI_Comm comm=%s, int *size=%p)", incount, type_to_string(datatype), comm_to_string(comm), size);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Pack_size(incount, type, comm, size);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Pack_size failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Probe ( int source, int tag, MPI_Comm comm, MPI_Status *s )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Probe(int source=%d, int tag=%d, MPI_Comm comm=%s, MPI_Status *s=%p)", source, tag, comm_to_string(comm), s);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Probe(source, tag, comm, (EMPI_Status *)s);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_PROBE, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Probe failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Publish_name ( char *service_name, MPI_Info info, char *port_name )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Publish_name(char *service_name=%p, MPI_Info info=%s, char *port_name=%p)", service_name, info_to_string(info), port_name);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Publish_name(service_name, info, port_name);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Publish_name failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Put ( void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Put(void *origin_addr=%p, int origin_count=%d, MPI_Datatype origin_datatype=%s, int target_rank=%d, MPI_Aint target_disp=%p, int target_count=%d, MPI_Datatype target_datatype=%s, MPI_Win win=%s)", origin_addr, origin_count, type_to_string(origin_datatype), target_rank, (void *) target_disp, target_count, type_to_string(target_datatype), win_to_string(win));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(origin_datatype);
   CHECK_TYPE(target_datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Put(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Put failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Query_thread ( int *provided )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Query_thread(int *provided=%p)", provided);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Query_thread(provided);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Query_thread failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}



int MPI_Recv_init ( void *buf, int count, MPI_Datatype type, int source, int tag, MPI_Comm comm, MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Recv_init(void *buf=%p, int count=%d, MPI_Datatype type=%s, int source=%d, int tag=%d, MPI_Comm comm=%s, MPI_Request *r=%p)", buf, count, type_to_string(datatype), source, tag, comm_to_string(comm), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Recv_init(buf, count, type, source, tag, comm, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Recv_init failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


/*
int MPI_Reduce_local ( void *inbuf, void *inoutbuf, int count, MPI_Datatype type, MPI_Op op )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Reduce_local(void *inbuf=%p, void *inoutbuf=%p, int count=%d, MPI_Datatype type=%s, MPI_Op op=%s)", inbuf, inoutbuf, count, type_to_string(datatype), op_to_string(op));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Reduce_local(inbuf, inoutbuf, count, type, op);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Reduce_local failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}
*/
/*
int MPI_Reduce_scatter_block ( void *sendbuf, void *recvbuf, int recvcount, MPI_Datatype type, MPI_Op op, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Reduce_scatter_block(void *sendbuf=%p, void *recvbuf=%p, int recvcount=%d, MPI_Datatype type=%s, MPI_Op op=%s, MPI_Comm comm=%s)", sendbuf, recvbuf, recvcount, type_to_string(datatype), op_to_string(op), comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Reduce_scatter_block(sendbuf, recvbuf, recvcount, type, op, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Reduce_scatter_block failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}
*/

int MPI_Reduce_scatter ( void *sendbuf, void *recvbuf, int *recvcnts, MPI_Datatype type, MPI_Op op, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Reduce_scatter(void *sendbuf=%p, void *recvbuf=%p, int *recvcnts=%p, MPI_Datatype type=%s, MPI_Op op=%s, MPI_Comm comm=%s)", sendbuf, recvbuf, recvcnts, type_to_string(datatype), op_to_string(op), comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Reduce_scatter(sendbuf, recvbuf, recvcnts, type, op, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_REDUCE_SCATTER, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Reduce_scatter failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Register_datarep ( char *name, MPI_Datarep_conversion_function *read_conv_fn, MPI_Datarep_conversion_function *write_conv_fn, MPI_Datarep_extent_function *extent_fn, void *state )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Register_datarep(char *name=%p, MPI_Datarep_conversion_function *read_conv_fn=%p, MPI_Datarep_conversion_function *write_conv_fn=%p, MPI_Datarep_extent_function *extent_fn=%p, void *state=%p)", name, read_conv_fn, write_conv_fn, extent_fn, state);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Register_datarep(name, read_conv_fn, write_conv_fn, extent_fn, state);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Register_datarep failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}



int MPI_Request_get_status ( MPI_Request r, int *flag, MPI_Status *s )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Request_get_status(MPI_Request r=%s, int *flag=%p, MPI_Status *s=%p)", request_to_string(r), flag, s);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Request_get_status(r, flag, (EMPI_Status *)s);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Request_get_status failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}




int MPI_Rsend_init ( void *buf, int count, MPI_Datatype type, int dest, int tag, MPI_Comm comm, MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Rsend_init(void *buf=%p, int count=%d, MPI_Datatype type=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s, MPI_Request *r=%p)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Rsend_init(buf, count, type, dest, tag, comm, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Rsend_init failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Scan ( void *sendbuf, void *recvbuf, int count, MPI_Datatype type, MPI_Op op, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Scan(void *sendbuf=%p, void *recvbuf=%p, int count=%d, MPI_Datatype type=%s, MPI_Op op=%s, MPI_Comm comm=%s)", sendbuf, recvbuf, count, type_to_string(datatype), op_to_string(op), comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Scan(sendbuf, recvbuf, count, type, op, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_SCAN, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Scan failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}





int MPI_Send_init ( void *buf, int count, MPI_Datatype type, int dest, int tag, MPI_Comm comm, MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Send_init(void *buf=%p, int count=%d, MPI_Datatype type=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s, MPI_Request *r=%p)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Send_init(buf, count, type, dest, tag, comm, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Send_init failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}




int MPI_Sendrecv_replace ( void *buf, int count, MPI_Datatype type, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *s )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Sendrecv_replace(void *buf=%p, int count=%d, MPI_Datatype type=%s, int dest=%d, int sendtag=%d, int source=%d, int recvtag=%d, MPI_Comm comm=%s, MPI_Status *s=%p)", buf, count, type_to_string(datatype), dest, sendtag, source, recvtag, comm_to_string(comm), s);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Sendrecv_replace(buf, count, type, dest, sendtag, source, recvtag, comm, (EMPI_Status *)s);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_SEND_RECV, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Sendrecv_replace failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}




int MPI_Ssend_init ( void *buf, int count, MPI_Datatype type, int dest, int tag, MPI_Comm comm, MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Ssend_init(void *buf=%p, int count=%d, MPI_Datatype type=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s, MPI_Request *r=%p)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Ssend_init(buf, count, type, dest, tag, comm, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Ssend_init failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Startall ( int count, MPI_Request array_of_requests[] )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Startall(int count=%d, MPI_Request array_of_requests[]=%p)", count, array_of_requests);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Startall(count, array_of_requests);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Startall failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Start ( MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Start(MPI_Request *r=%p)", r);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Start(r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Start failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Status_set_cancelled ( MPI_Status *s, int flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Status_set_cancelled(MPI_Status *s=%p, int flag=%d)", s, flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Status_set_cancelled((EMPI_Status *)s, flag);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Status_set_cancelled failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Status_set_elements ( MPI_Status *s, MPI_Datatype type, int count )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Status_set_elements(MPI_Status *s=%p, MPI_Datatype type=%s, int count=%d)", s, type_to_string(datatype), count);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Status_set_elements((EMPI_Status *)s, type, count);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Status_set_elements failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Testall ( int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[] )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Testall(int count=%d, MPI_Request array_of_requests[]=%p, int *flag=%p, MPI_Status array_of_statuses[]=%p)", count, array_of_requests, flag, array_of_statuses);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Testall(count, array_of_requests, flag, (EMPI_Status *)array_of_statuses);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_TESTALL, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Testall failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Testany ( int count, MPI_Request array_of_requests[], int *index, int *flag, MPI_Status *s )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Testany(int count=%d, MPI_Request array_of_requests[]=%p, int *index=%p, int *flag=%p, MPI_Status *s=%p)", count, array_of_requests, index, flag, s);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Testany(count, array_of_requests, index, flag, (EMPI_Status *)s);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_TESTANY, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Testany failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Test_cancelled ( MPI_Status *s, int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Test_cancelled(MPI_Status *s=%p, int *flag=%p)", s, flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Test_cancelled((EMPI_Status *)s, flag);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Test_cancelled failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Test ( MPI_Request *r, int *flag, MPI_Status *s )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Test(MPI_Request *r=%p, int *flag=%p, MPI_Status *s=%p)", r, flag, s);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Test(r, flag, (EMPI_Status *)s);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_TEST, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Test failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Testsome ( int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[] )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Testsome(int incount=%d, MPI_Request array_of_requests[]=%p, int *outcount=%p, int array_of_indices[]=%p, MPI_Status array_of_statuses[]=%p)", incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Testsome(incount, array_of_requests, outcount, array_of_indices, (EMPI_Status *)array_of_statuses);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_TESTSOME, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Testsome failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Topo_test ( MPI_Comm comm, int *topo_type )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Topo_test(MPI_Comm comm=%s, int *topo_type=%p)", comm_to_string(comm), topo_type);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Topo_test(comm, topo_type);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Topo_test failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}



int MPI_Type_create_darray ( int size, int rank, int ndims, int array_of_gsizes[], int array_of_distribs[], int array_of_dargs[], int array_of_psizes[], int order, MPI_Datatype oldtype, MPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_create_darray(int size=%d, int rank=%d, int ndims=%d, int array_of_gsizes[]=%p, int array_of_distribs[]=%p, int array_of_dargs[]=%p, int array_of_psizes[]=%p, int order=%d, MPI_Datatype oldtype=%s, MPI_Datatype *newtype=%p)", size, rank, ndims, array_of_gsizes, array_of_distribs, array_of_dargs, array_of_psizes, order, type_to_string(oldtype), newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(oldtype);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_create_darray(size, rank, ndims, array_of_gsizes, array_of_distribs, array_of_dargs, array_of_psizes, order, oldtype, newtype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_create_darray failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_create_hindexed ( int count, int blocklengths[], MPI_Aint displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_create_hindexed(int count=%d, int blocklengths[]=%p, MPI_Aint displacements[]=%p, MPI_Datatype oldtype=%s, MPI_Datatype *newtype=%p)", count, blocklengths, displacements, type_to_string(oldtype), newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(oldtype);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_create_hindexed(count, blocklengths, displacements, oldtype, newtype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_create_hindexed failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_create_hvector ( int count, int blocklength, MPI_Aint stride, MPI_Datatype oldtype, MPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_create_hvector(int count=%d, int blocklength=%d, MPI_Aint stride=%p, MPI_Datatype oldtype=%s, MPI_Datatype *newtype=%p)", count, blocklength, (void *) stride, type_to_string(oldtype), newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(oldtype);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_create_hvector(count, blocklength, stride, oldtype, newtype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_create_hvector failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}



int MPI_Type_create_keyval ( MPI_Type_copy_attr_function *type_copy_attr_fn, MPI_Type_delete_attr_function *type_delete_attr_fn, int *type_keyval, void *extra_state )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_create_keyval(MPI_Type_copy_attr_function *type_copy_attr_fn=%p, MPI_Type_delete_attr_function *type_delete_attr_fn=%p, int *type_keyval=%p, void *extra_state=%p)", type_copy_attr_fn, type_delete_attr_fn, type_keyval, extra_state);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_create_keyval(type_copy_attr_fn, type_delete_attr_fn, type_keyval, extra_state);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_create_keyval failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_create_resized ( MPI_Datatype oldtype, MPI_Aint lb, MPI_Aint extent, MPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_create_resized(MPI_Datatype oldtype=%s, MPI_Aint lb=%p, MPI_Aint extent=%p, MPI_Datatype *newtype=%p)", type_to_string(oldtype), (void *) lb, (void *) extent, newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(oldtype);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_create_resized(oldtype, lb, extent, newtype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_create_resized failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_create_struct ( int count, int array_of_blocklengths[], MPI_Aint array_of_displacements[], MPI_Datatype array_of_types[], MPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_create_struct(int count=%d, int array_of_blocklengths[]=%p, MPI_Aint array_of_displacements[]=%p, MPI_Datatype array_of_types[]=%p, MPI_Datatype *newtype=%p)", count, array_of_blocklengths, array_of_displacements, array_of_types, newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(array_of_types[]);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_create_struct(count, array_of_blocklengths, array_of_displacements, array_of_types, newtype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_create_struct failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_create_subarray ( int ndims, int array_of_sizes[], int array_of_subsizes[], int array_of_starts[], int order, MPI_Datatype oldtype, MPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_create_subarray(int ndims=%d, int array_of_sizes[]=%p, int array_of_subsizes[]=%p, int array_of_starts[]=%p, int order=%d, MPI_Datatype oldtype=%s, MPI_Datatype *newtype=%p)", ndims, array_of_sizes, array_of_subsizes, array_of_starts, order, type_to_string(oldtype), newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(oldtype);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_create_subarray(ndims, array_of_sizes, array_of_subsizes, array_of_starts, order, oldtype, newtype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_create_subarray failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_delete_attr ( MPI_Datatype type, int type_keyval )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_delete_attr(MPI_Datatype type=%s, int type_keyval=%d)", type_to_string(type), type_keyval);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(type);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_delete_attr(type, type_keyval);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_delete_attr failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_dup ( MPI_Datatype type, MPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_dup(MPI_Datatype type=%s, MPI_Datatype *newtype=%p)", type_to_string(datatype), newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_dup(datatype, newtype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_dup failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_extent ( MPI_Datatype type, MPI_Aint *extent )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_extent(MPI_Datatype type=%s, MPI_Aint *extent=%p)", type_to_string(datatype), extent);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_extent(datatype, extent);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_extent failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}



int MPI_Type_free_keyval ( int *type_keyval )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_free_keyval(int *type_keyval=%p)", type_keyval);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_free_keyval(type_keyval);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_free_keyval failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_get_attr ( MPI_Datatype type, int type_keyval, void *attribute_val, int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_get_attr(MPI_Datatype type=%s, int type_keyval=%d, void *attribute_val=%p, int *flag=%p)", type_to_string(type), type_keyval, attribute_val, flag);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(type);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_get_attr(type, type_keyval, attribute_val, flag);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_get_attr failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_get_contents ( MPI_Datatype type, int max_integers, int max_addresses, int max_datatypes, int array_of_integers[], MPI_Aint array_of_addresses[], MPI_Datatype array_of_datatypes[] )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_get_contents(MPI_Datatype type=%s, int max_integers=%d, int max_addresses=%d, int max_datatypes=%d, int array_of_integers[]=%p, MPI_Aint array_of_addresses[]=%p, MPI_Datatype array_of_datatypes[]=%p)", type_to_string(datatype), max_integers, max_addresses, max_datatypes, array_of_integers, array_of_addresses, array_of_datatypes);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
   CHECK_TYPE(array_of_datatypes[]);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_get_contents(datatype, max_integers, max_addresses, max_datatypes, array_of_integers, array_of_addresses, array_of_datatypes);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_get_contents failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}



int MPI_Type_get_extent ( MPI_Datatype type, MPI_Aint *lb, MPI_Aint *extent )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_get_extent(MPI_Datatype type=%s, MPI_Aint *lb=%p, MPI_Aint *extent=%p)", type_to_string(datatype), lb, extent);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_get_extent(datatype, lb, extent);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_get_extent failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}



int MPI_Type_get_true_extent ( MPI_Datatype type, MPI_Aint *true_lb, MPI_Aint *true_extent )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_get_true_extent(MPI_Datatype type=%s, MPI_Aint *true_lb=%p, MPI_Aint *true_extent=%p)", type_to_string(datatype), true_lb, true_extent);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_get_true_extent(datatype, true_lb, true_extent);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_get_true_extent failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_hindexed ( int count, int blocklens[], MPI_Aint indices[], MPI_Datatype old_type, MPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_hindexed(int count=%d, int blocklens[]=%p, MPI_Aint indices[]=%p, MPI_Datatype old_type=%s, MPI_Datatype *newtype=%p)", count, blocklens, indices, type_to_string(old_type), newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(old_type);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_hindexed(count, blocklens, indices, old_type, newtype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_hindexed failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_hvector ( int count, int blocklen, MPI_Aint stride, MPI_Datatype old_type, MPI_Datatype *newtype_p )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_hvector(int count=%d, int blocklen=%d, MPI_Aint stride=%p, MPI_Datatype old_type=%s, MPI_Datatype *newtype_p=%p)", count, blocklen, (void *) stride, type_to_string(old_type), newtype_p);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(old_type);
   CHECK_TYPE(*newtype_p);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_hvector(count, blocklen, stride, old_type, newtype_p);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_hvector failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_indexed ( int count, int blocklens[], int indices[], MPI_Datatype old_type, MPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_indexed(int count=%d, int blocklens[]=%p, int indices[]=%p, MPI_Datatype old_type=%s, MPI_Datatype *newtype=%p)", count, blocklens, indices, type_to_string(old_type), newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(old_type);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_indexed(count, blocklens, indices, old_type, newtype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_indexed failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_lb ( MPI_Datatype type, MPI_Aint *displacement )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_lb(MPI_Datatype type=%s, MPI_Aint *displacement=%p)", type_to_string(datatype), displacement);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_lb(datatype, displacement);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_lb failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_match_size ( int typeclass, int size, MPI_Datatype *datatype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_match_size(int typeclass=%d, int size=%d, MPI_Datatype *datatype=%p)", typeclass, size, datatype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(*datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_match_size(typeclass, size, datatype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_match_size failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_set_attr ( MPI_Datatype type, int type_keyval, void *attribute_val )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_set_attr(MPI_Datatype type=%s, int type_keyval=%d, void *attribute_val=%p)", type_to_string(type), type_keyval, attribute_val);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(type);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_set_attr(type, type_keyval, attribute_val);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_set_attr failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_set_name ( MPI_Datatype type, char *type_name )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_set_name(MPI_Datatype type=%s, char *type_name=%p)", type_to_string(type), type_name);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(type);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_set_name(type, type_name);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_set_name failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_size ( MPI_Datatype type, int *size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_size(MPI_Datatype type=%s, int *size=%p)", type_to_string(datatype), size);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_size(datatype, size);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_size failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_struct ( int count, int blocklens[], MPI_Aint indices[], MPI_Datatype old_types[], MPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_struct(int count=%d, int blocklens[]=%p, MPI_Aint indices[]=%p, MPI_Datatype old_types[]=%p, MPI_Datatype *newtype=%p)", count, blocklens, indices, old_types, newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(old_types[]);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_struct(count, blocklens, indices, old_types, newtype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_struct failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_ub ( MPI_Datatype type, MPI_Aint *displacement )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_ub(MPI_Datatype type=%s, MPI_Aint *displacement=%p)", type_to_string(datatype), displacement);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_ub(datatype, displacement);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_ub failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_vector ( int count, int blocklength, int stride, MPI_Datatype old_type, MPI_Datatype *newtype_p )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_vector(int count=%d, int blocklength=%d, int stride=%d, MPI_Datatype old_type=%s, MPI_Datatype *newtype_p=%p)", count, blocklength, stride, type_to_string(old_type), newtype_p);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(old_type);
   CHECK_TYPE(*newtype_p);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Type_vector(count, blocklength, stride, old_type, newtype_p);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_vector failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Unpack_external ( char *datarep, void *inbuf, MPI_Aint insize, MPI_Aint *position, void *outbuf, int outcount, MPI_Datatype type )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Unpack_external(char *datarep=%p, void *inbuf=%p, MPI_Aint insize=%p, MPI_Aint *position=%p, void *outbuf=%p, int outcount=%d, MPI_Datatype type=%s)", datarep, inbuf, (void *) insize, position, outbuf, outcount, type_to_string(datatype));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Unpack_external(datarep, inbuf, insize, position, outbuf, outcount, datatype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Unpack_external failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Unpack ( void *inbuf, int insize, int *position, void *outbuf, int outcount, MPI_Datatype type, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Unpack(void *inbuf=%p, int insize=%d, int *position=%p, void *outbuf=%p, int outcount=%d, MPI_Datatype type=%s, MPI_Comm comm=%s)", inbuf, insize, position, outbuf, outcount, type_to_string(datatype), comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Unpack(inbuf, insize, position, outbuf, outcount, type, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Unpack failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Unpublish_name ( char *service_name, MPI_Info info, char *port_name )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Unpublish_name(char *service_name=%p, MPI_Info info=%s, char *port_name=%p)", service_name, info_to_string(info), port_name);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Unpublish_name(service_name, info, port_name);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Unpublish_name failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}







int MPI_Waitsome ( int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[] )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Waitsome(int incount=%d, MPI_Request array_of_requests[]=%p, int *outcount=%p, int array_of_indices[]=%p, MPI_Status array_of_statuses[]=%p)", incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Waitsome(incount, array_of_requests, outcount, array_of_indices, (EMPI_Status *)array_of_statuses);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_WAITSOME, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Waitsome failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_call_errhandler ( MPI_Win win, int errorcode )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_call_errhandler(MPI_Win win=%s, int errorcode=%d)", win_to_string(win), errorcode);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Win_call_errhandler(win, errorcode);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_call_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_complete ( MPI_Win win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_complete(MPI_Win win=%s)", win_to_string(win));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Win_complete(win);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_complete failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_create_errhandler ( MPI_Win_errhandler_fn *function, MPI_Errhandler *errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_create_errhandler(MPI_Win_errhandler_fn *function=%p, MPI_Errhandler *errhandler=%p)", function, errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Win_create_errhandler(function, errhandler);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_create_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_create ( void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_create(void *base=%p, MPI_Aint size=%p, int disp_unit=%d, MPI_Info info=%s, MPI_Comm comm=%s, MPI_Win *win=%p)", base, (void *) size, disp_unit, info_to_string(info), comm_to_string(comm), win);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Win_create(base, size, disp_unit, info, comm, win);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_create_keyval ( MPI_Win_copy_attr_function *win_copy_attr_fn, MPI_Win_delete_attr_function *win_delete_attr_fn, int *win_keyval, void *extra_state )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_create_keyval(MPI_Win_copy_attr_function *win_copy_attr_fn=%p, MPI_Win_delete_attr_function *win_delete_attr_fn=%p, int *win_keyval=%p, void *extra_state=%p)", win_copy_attr_fn, win_delete_attr_fn, win_keyval, extra_state);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Win_create_keyval(win_copy_attr_fn, win_delete_attr_fn, win_keyval, extra_state);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_create_keyval failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_delete_attr ( MPI_Win win, int win_keyval )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_delete_attr(MPI_Win win=%s, int win_keyval=%d)", win_to_string(win), win_keyval);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Win_delete_attr(win, win_keyval);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_delete_attr failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int MPI_Win_fence ( int assert, MPI_Win win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_fence(int assert=%d, MPI_Win win=%s)", assert, win_to_string(win));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Win_fence(assert, win);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_fence failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_free ( MPI_Win *win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_free(MPI_Win *win=%p)", win);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Win_free(win);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_free failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_free_keyval ( int *win_keyval )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_free_keyval(int *win_keyval=%p)", win_keyval);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Win_free_keyval(win_keyval);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_free_keyval failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_get_attr ( MPI_Win win, int win_keyval, void *attribute_val, int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_get_attr(MPI_Win win=%s, int win_keyval=%d, void *attribute_val=%p, int *flag=%p)", win_to_string(win), win_keyval, attribute_val, flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Win_get_attr(win, win_keyval, attribute_val, flag);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_get_attr failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_get_errhandler ( MPI_Win win, MPI_Errhandler *errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_get_errhandler(MPI_Win win=%s, MPI_Errhandler *errhandler=%p)", win_to_string(win), errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Win_get_errhandler(win, errhandler);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_get_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_get_group ( MPI_Win win, MPI_Group *g )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_get_group(MPI_Win win=%s, MPI_Group *g=%p)", win_to_string(win), g);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Win_get_group(win, g);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_get_group failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_get_name ( MPI_Win win, char *win_name, int *resultlen )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_get_name(MPI_Win win=%s, char *win_name=%p, int *resultlen=%p)", win_to_string(win), win_name, resultlen);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Win_get_name(win, win_name, resultlen);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_get_name failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_lock ( int lock_type, int rank, int assert, MPI_Win win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_lock(int lock_type=%d, int rank=%d, int assert=%d, MPI_Win win=%s)", lock_type, rank, assert, win_to_string(win));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Win_lock(lock_type, rank, assert, win);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_lock failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_post ( MPI_Group g, int assert, MPI_Win win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_post(MPI_Group g=%s, int assert=%d, MPI_Win win=%s)", group_to_string(g), assert, win_to_string(win));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Win_post(g, assert, win);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_post failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_set_attr ( MPI_Win win, int win_keyval, void *attribute_val )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_set_attr(MPI_Win win=%s, int win_keyval=%d, void *attribute_val=%p)", win_to_string(win), win_keyval, attribute_val);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Win_set_attr(win, win_keyval, attribute_val);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_set_attr failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_set_errhandler ( MPI_Win win, MPI_Errhandler errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_set_errhandler(MPI_Win win=%s, MPI_Errhandler errhandler=%p)", win_to_string(win), (void *) errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Win_set_errhandler(win, errhandler);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_set_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_set_name ( MPI_Win win, char *win_name )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_set_name(MPI_Win win=%s, char *win_name=%p)", win_to_string(win), win_name);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Win_set_name(win, win_name);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_set_name failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_start ( MPI_Group g, int assert, MPI_Win win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_start(MPI_Group g=%s, int assert=%d, MPI_Win win=%s)", group_to_string(g), assert, win_to_string(win));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Win_start(g, assert, win);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_start failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_test ( MPI_Win win, int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_test(MPI_Win win=%s, int *flag=%p)", win_to_string(win), flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Win_test(win, flag);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_test failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_unlock ( int rank, MPI_Win win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_unlock(int rank=%d, MPI_Win win=%s)", rank, win_to_string(win));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Win_unlock(rank, win);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_unlock failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_wait ( MPI_Win win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_wait(MPI_Win win=%s)", win_to_string(win));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Win_wait(win);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_wait failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

/*
int MPI_Group_comm_create ( MPI_Comm old_comm, MPI_Group g, int tag, MPI_Comm *new_comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Group_comm_create(MPI_Comm old_comm=%s, MPI_Group g=%s, int tag=%d, MPI_Comm *new_comm=%p)", comm_to_string(old_comm), group_to_string(g), tag, new_comm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = EMPI_Group_comm_create(old_comm, g, tag, new_comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(old_comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Group_comm_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}
*/

MPI_Comm MPI_Comm_f2c ( MPI_Fint comm )
{
   return EMPI_Comm_f2c(comm);
}


MPI_Group MPI_Group_f2c ( MPI_Fint g )
{
   return EMPI_Group_f2c(g);

}


MPI_Request MPI_Request_f2c ( MPI_Fint r )
{
   return EMPI_Request_f2c(r);

}


MPI_Info MPI_Info_f2c ( MPI_Fint info )
{

   return EMPI_Info_f2c(info);
}


MPI_File MPI_File_f2c ( MPI_Fint file )
{
   return EMPI_File_f2c(file);
}


MPI_Op MPI_Op_f2c ( MPI_Fint op )
{
   return EMPI_Op_f2c(op);
}


MPI_Win MPI_Win_f2c ( MPI_Fint Win )
{
   return EMPI_Win_f2c(Win);
}


MPI_Errhandler MPI_Errhandler_f2c ( MPI_Fint Errhandler )
{
   return EMPI_Errhandler_f2c(Errhandler);
}


MPI_Datatype MPI_Type_f2c ( MPI_Fint Type )
{
   return EMPI_Type_f2c(Type);
}


MPI_Fint MPI_Comm_c2f ( MPI_Comm comm )
{
   return EMPI_Comm_c2f(comm);
}


MPI_Fint MPI_Group_c2f ( MPI_Group g )
{
   return EMPI_Group_c2f(g);
}


MPI_Fint MPI_Request_c2f ( MPI_Request r )
{
   return EMPI_Request_c2f(r);
}


MPI_Fint MPI_Info_c2f ( MPI_Info info )
{
   return EMPI_Info_c2f(info);
}


MPI_Fint MPI_File_c2f ( MPI_File file )
{
   return EMPI_File_c2f(file);
}


MPI_Fint MPI_Op_c2f ( MPI_Op op )
{
   return EMPI_Op_c2f(op);
}


MPI_Fint MPI_Win_c2f ( MPI_Win Win )
{
   return EMPI_Win_c2f(Win);
}


MPI_Fint MPI_Errhandler_c2f ( MPI_Errhandler Errhandler )
{
   return EMPI_Errhandler_c2f(Errhandler);
}


MPI_Fint MPI_Type_c2f ( MPI_Datatype Type )
{
   return EMPI_Type_c2f(Type);
}


#endif // if 0
