#ifndef _DATATYPE_H_
#define _DATATYPE_H_

#include <stdlib.h>

#include "flags.h"
#include "empi.h"
#include "mpi.h"

struct s_datatype {
   int handle;
   int size;
   MPI_Datatype type;
};


int init_datatypes(void);
datatype *handle_to_datatype(EMPI_Datatype handle);
EMPI_Datatype datatype_to_handle(datatype *d);
int is_derived_datatype(EMPI_Datatype type);

#endif


