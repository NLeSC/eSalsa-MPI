#ifndef _FILES_H_
#define _FILES_H_

#include "types.h"
#include "empi.h"
#include <mpi.h>

struct s_file {
     EMPI_File handle;
     communicator *comm;
     MPI_File file;
};

int init_files();

// Utility functions to convert handles to operations an vice-versa.
file *handle_to_file(EMPI_File handle);
EMPI_File file_to_handle(file *i);

file *add_file(communicator *comm, MPI_File f);
void remove_file(EMPI_File f);

// operation *get_operation(MPI_Op op);
// operation *get_operation_with_index(int index);
// void set_operation_ptr(MPI_Op *dst, operation *src);
// operation *create_operation(MPI_User_function *function, int commute);
// void free_operation(operation *op);

#endif // _OPERATION_H_
