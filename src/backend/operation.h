#ifndef _OPERATION_H_
#define _OPERATION_H_

#include "types.h"
#include "empi.h"
#include <mpi.h>

struct s_operation {
     EMPI_Op handle;
     EMPI_User_function *function;
     int commute;
     MPI_Op op;
};

int init_operations();

// Utility functions to convert handles to operations an vice-versa.
operation *handle_to_operation(EMPI_Op handle);
EMPI_Op operation_to_handle(operation *o);
operation *create_operation(EMPI_User_function *function, MPI_Op op, int commute);

// operation *get_operation(MPI_Op op);
// operation *get_operation_with_index(int index);
// void set_operation_ptr(MPI_Op *dst, operation *src);
// void free_operation(operation *op);

#endif // _OPERATION_H_
