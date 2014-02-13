#ifndef _OPERATION_H_
#define _OPERATION_H_

#include "flags.h"
#include "empi.h"
#include "types.h"

#include "empi.h"

struct s_operation {
     EMPI_User_function *function;
     int commute;
     EMPI_Op handle;
};

int init_operations();

// Utility functions to convert handles to operations an vice-versa.
operation *handle_to_operation(EMPI_Op handle);
EMPI_Op operation_to_handle(operation *o);

// operation *get_operation(MPI_Op op);
// operation *get_operation_with_index(int index);
// void set_operation_ptr(MPI_Op *dst, operation *src);
// operation *create_operation(MPI_User_function *function, int commute);
// void free_operation(operation *op);

#endif // _OPERATION_H_
