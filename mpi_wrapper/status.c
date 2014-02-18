#include <stdio.h>
#include <stdlib.h>

#include "status.h"
#include "empi.h"

//struct s_status __EMPI_STATUS_CLEAN  = { EMPI_ANY_SOURCE, EMPI_ANY_TAG, 0, 0, 0, 0 };

void set_status(EMPI_Status *s, int source, int tag, int error, datatype *type, int count, int cancelled)
{
   if (s == EMPI_STATUS_IGNORE) {
      return;
   }

   s->MPI_SOURCE = source;
   s->MPI_TAG = tag;
   s->MPI_ERROR = error;
   s->count = count;
   s->cancelled = cancelled;
   s->type = type;
}

void clear_status(EMPI_Status *s)
{
   set_status(s, EMPI_ANY_SOURCE, EMPI_ANY_TAG, EMPI_SUCCESS, EMPI_DATATYPE_NULL, 0, FALSE);
}

void set_status_error(EMPI_Status *s, int source, int tag, int error, datatype *type)
{
   set_status(s, source, tag, error, type, 0, FALSE);
}

void set_status_count(EMPI_Status *s, datatype *type, int count)
{
   if (s == EMPI_STATUS_IGNORE) {
      return;
   }

   s->type = type;
   s->count = count;
}

void set_status_cancelled(EMPI_Status *s, int flag)
{
   if (s == EMPI_STATUS_IGNORE) {
      return;
   }

   s->cancelled = flag;
}

