#ifndef __FLAGS_H_
#define __FLAGS_H_

// These flags can be use to configure eSalsa-MPI at compile time.
// Switch features off by commenting a define, or changing its value.

// Print every call ?
// #define TRACE_CALLS

// Print error and terminate when an MPI_ERROR is caught ?
#define TRACE_ERRORS

// Profiling level 0 = no profiling, 1 = course grained, 2 = fine grained
#define PROFILE_LEVEL 0

// Used detailed message info in gateway nodes ? (NOTE: May be expensive!)
// #define DETAILED_TIMING

// Used simple message info in gateway nodes ? (NOTE: Cheap but chatty!)
// #define SIMPLE_TIMING

// Print message info when gateway nodes terminate ?
// #define END_TIMING

// These constants determine various maxima used in the EMPI library.

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

// Maximum number of datatypes that can be created (at once, for each individual process).
#define MAX_DATATYPES 10240

// Maximum number of files that can be created (at once, for each individual process).
#define MAX_FILES 64

// Maximum number of info object that can be created (at once, for each individual process).
#define MAX_INFOS 64

// The largest message payload we support.
// #define MAX_MESSAGE_PAYLOAD (8*1024*1024)

// Size of detailed timing buffers (in messages) in gateway nodes.
#define DETAILED_TIMING_COUNT 8096

#endif
