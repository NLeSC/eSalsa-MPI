#ifndef _TYPES_H_
#define _TYPES_H_

typedef struct s_msg_buffer message_buffer;
typedef struct s_communicator communicator;
typedef struct s_request request;
typedef struct s_group group;
typedef struct s_operation operation;
typedef struct s_status status;
typedef struct s_datatype datatype;

// NOTE: This is only sufficient if the maximum number of MPI processes is <= 2^16
typedef unsigned short t_pid;

#endif // _TYPES_H_
