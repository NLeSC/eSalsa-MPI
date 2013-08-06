#ifndef _TYPES_H_
#define _TYPES_H_

#include "flags.h"

#ifdef IBIS_INTERCEPT

#include "mpi.h"

typedef struct s_msg_buffer message_buffer;
typedef struct s_communicator communicator;
typedef struct s_request  request;
typedef struct s_group group;
typedef struct s_operation operation;

// NOTE: This is only sufficient if the maximum number of MPI processes is <= 2^16
typedef unsigned short t_pid;

#endif // IBIS_INTERCEPT

#endif // _TYPE_H_
