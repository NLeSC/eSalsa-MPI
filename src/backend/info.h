#ifndef _INFOS_H_
#define _INFOS_H_

#include "types.h"
#include "empi.h"
#include <mpi.h>

struct s_info {
   EMPI_Info handle;
   MPI_Info info;
};

int init_infos();

// Utility functions to convert handles to operations an vice-versa.
info *handle_to_info(EMPI_Info handle);
EMPI_Info info_to_handle(info *i);
info *add_info(MPI_Info inf);
void remove_info(EMPI_Info inf);


// operation *get_operation(MPI_Op op);
// operation *get_operation_with_index(int index);
// void set_operation_ptr(MPI_Op *dst, operation *src);
// operation *create_operation(MPI_User_function *function, int commute);
// void free_operation(operation *op);

#endif // _OPERATION_H_
