#ifndef _DEBUGGING_H_
#define _DEBUGGING_H_

#include <stdlib.h>

#include "flags.h"
#include "communicator.h"

// Macro to catch derived datatypes.
#ifdef CATCH_DERIVED_TYPES
#define CHECK_TYPE(T) (catch_derived_datatype(T))

void catch_derived_datatype(MPI_Datatype type);

#else
#define CHECK_TYPE(T)
#endif

// Macros to check parameters
#ifdef CHECK_PARAMETERS
#define CHECK_COUNT(C)         (check_count(C))
#define CHECK_SOURCE(C,R)      (check_source(C, R))
#define CHECK_DESTINATION(C,R) (check_destination(C, R))

void check_count(int count);
void check_source(communicator *c, int rank);
void check_destination(communicator *c, int rank);
#else
#define CHECK_COUNT(C)
#define CHECK_SOURCE(C,R)
#define CHECK_DESTINATION(C,R)
#endif

#endif


