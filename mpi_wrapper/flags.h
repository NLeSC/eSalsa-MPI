#ifndef __FLAGS_H_
#define __FLAGS_H_

// These flags determine how much of the MPIbis library will be used.
// Switch features off by commenting a define.

// Print every call ?
// #define TRACE_CALLS

// Print error and terminate when an MPI_ERROR is caught ?
#define TRACE_ERRORS

// Profiling level 0 = no profiling, 1 = course grained, 2 = fine grained
#define PROFILE_LEVEL 0

// Print error when a derived datatype is send (which is not supported yet).
//#define CATCH_DERIVED_TYPES

// Switch on Macros used to check function parameters
//#define CHECK_PARAMETERS

// Is error code translation needed between EMPI and MPI calls ?
//#ifdef NEED_ERROR_TRANSLATION

// These constants determine various maxima used in the MPIbis library.

// Maximum number of MPI processes allowed per cluster (2^24).
#define MAX_PROCESSES_PER_CLUSTER 16777216

// Maximum number of clusters allowed (2^8).
#define MAX_CLUSTERS 256

// Macro to filter out cluster ID.
#define GET_CLUSTER_RANK(X) ((X & 0xFF000000) >> 24)

// Macro to filter out process ID.
#define GET_PROCESS_RANK(X) (X & 0x00FFFFFF)

// Macro to create process ID.
#define SET_PID(X, Y) ((X & 0xFF) << 24 | (Y & 0xFFFFFF))

// Maximum number of communicators that can be created (in total, shared by all processes).
#define MAX_COMMUNICATORS 1024

// Maximum number of groups that can be created (at once, for each individual process).
#define MAX_GROUPS 1024

// Maximum number of requests that can be created (at once, for each individual process).
#define MAX_REQUESTS 1024

// Maximum number of operations that can be created (at once, for each individual process).
#define MAX_OPERATIONS 256

#endif
