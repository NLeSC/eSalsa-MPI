#ifndef _OPERATION_H_
#define _OPERATION_H_

#include "flags.h"

#ifdef IBIS_INTERCEPT

#include "mpi.h"
#include "types.h"

struct s_operation {
     MPI_User_function *function;
     int commute;
     int index;
     MPI_Op op;
};

int init_operations();

operation *get_operation(MPI_Op op);
operation *get_operation_with_index(int index);

// void set_operation_ptr(MPI_Op *dst, operation *src);
// operation *create_operation(MPI_User_function *function, int commute);
// void free_operation(operation *op);

#endif // IBIS_INTERCEPT

#endif // _OPERATION_H_
