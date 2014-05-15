#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "settings.h"

#include "empi.h"
#include "debugging.h"
#include "logging.h"

#include "communicator.h"


/*
#ifdef CATCH_DERIVED_TYPES

#define PRIMITIVE_TYPES 38

static MPI_Datatype primitive_types[PRIMITIVE_TYPES] = {
   MPI_CHAR,
   MPI_SHORT,
   MPI_INT,
   MPI_LONG,
   MPI_UNSIGNED_CHAR,
   MPI_UNSIGNED_SHORT,
   MPI_UNSIGNED,
   MPI_UNSIGNED_LONG,
   MPI_FLOAT,
   MPI_DOUBLE,
   MPI_LONG_DOUBLE,
   MPI_BYTE,
   MPI_PACKED,
   MPI_INTEGER,
   MPI_REAL,
   MPI_DOUBLE_PRECISION,
   MPI_COMPLEX,
   MPI_LOGICAL,
   MPI_CHARACTER,
   MPI_BYTE,
   MPI_PACKED,
   MPI_FLOAT_INT,
   MPI_DOUBLE_INT,
   MPI_LONG_INT,
   MPI_2INT,
   MPI_SHORT_INT,
   MPI_LONG_DOUBLE_INT,
   MPI_2REAL,
   MPI_2DOUBLE_PRECISION,
   MPI_2INTEGER,
   MPI_DOUBLE_COMPLEX,
   MPI_INTEGER1,
   MPI_INTEGER2,
   MPI_INTEGER4,
   MPI_REAL4,
   MPI_REAL8,
   MPI_LONG_LONG_INT,
   MPI_INTEGER8
};

void catch_derived_datatype(MPI_Datatype datatype)
{
   int i;

   for (i=0;i<PRIMITIVE_TYPES;i++) {
      if (datatype == primitive_types[i]) {
         return;
      }
   }

   FATAL("Attempt to communicate using a derived datatype!");
}

#endif // CATCH_DERIVED_TYPES
*/


#ifdef CHECK_PARAMETERS

void check_count(int count)
{
   if (count < 0) {
      FATAL("Invalid count %d", count);
   }
}

void check_source(communicator *c, int source)
{
   if (source != EMPI_ANY_SOURCE && (source < 0 || source >= c->global_size)) {
      FATAL("Invalid source rank %d for communicator %d", source, c->number);
   }
}

void check_destination(communicator *c, int dest)
{
   if (dest < 0 || dest >= c->global_size) { \
      FATAL("Invalid destination rank %d for communicator %d", dest, c->number);
   }
}

#endif // CHECK_PARAMETERS

