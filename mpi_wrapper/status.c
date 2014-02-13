#include <stdio.h>
#include <stdlib.h>

#include "status.h"
#include "empi.h"

struct s_status __EMPI_STATUS_CLEAN  = { EMPI_ANY_SOURCE, EMPI_ANY_TAG, 0, 0, 0, 0 };

int clear_status(EMPI_Status *s)
{
   if (s == NULL) {
      return EMPI_ERR_OTHER;
   }

   *s = __EMPI_STATUS_CLEAN;

   return EMPI_SUCCESS;
}
