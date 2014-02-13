#include "flags.h"

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "shared.h"
#include "types.h"
#include "logging.h"
#include "operation.h"
#include "reduce_ops.h"

static operation *ops[MAX_OPERATIONS];
static int next_operation = 0;

static int init_operation(EMPI_Op handle, EMPI_User_function *function, int commute)
{
   if (handle < 0 || handle > MAX_OPERATIONS) {
      IERROR(1, "Failed to initialize operations -- handle %d out of bounds!\n", handle);
      return EMPI_ERR_INTERN;
   }

   if (ops[handle] != NULL) {
      IERROR(1, "Failed to initialize operations -- handle %d already in use!\n", handle);
      return EMPI_ERR_INTERN;
   }

   ops[handle] = malloc(sizeof(operation));

   if (ops[handle] == NULL) {
      IERROR(1, "Failed to initialize operations -- cannot allocate operation %d!\n", handle);
      return EMPI_ERR_INTERN;
   }

   ops[handle]->function = function;
   ops[handle]->commute = commute;
   ops[handle]->handle = handle;

   if (handle >= next_operation) {
      next_operation = handle+1;
   }

   return EMPI_SUCCESS;
}

int init_operations()
{
   int i, error;

   for (i=0;i<MAX_OPERATIONS;i++) {
      ops[i] = NULL;
   }

   error = init_operation(EMPI_OP_NULL, NULL, 1);
   if (error != EMPI_SUCCESS) return error;

   error = init_operation(EMPI_MAX, MAGPIE_MAX, 1);
   if (error != EMPI_SUCCESS) return error;

   error = init_operation(EMPI_MIN, MAGPIE_MIN, 1);
   if (error != EMPI_SUCCESS) return error;

   error = init_operation(EMPI_SUM, MAGPIE_SUM, 1);
   if (error != EMPI_SUCCESS) return error;

   error = init_operation(EMPI_PROD, MAGPIE_PROD, 1);
   if (error != EMPI_SUCCESS) return error;

   error = init_operation(EMPI_MAXLOC, MAGPIE_MAXLOC, 1);
   if (error != EMPI_SUCCESS) return error;

   error = init_operation(EMPI_MINLOC, MAGPIE_MINLOC, 1);
   if (error != EMPI_SUCCESS) return error;

   error = init_operation(EMPI_BOR, MAGPIE_BOR, 1);
   if (error != EMPI_SUCCESS) return error;

   error = init_operation(EMPI_BAND, MAGPIE_BAND, 1);
   if (error != EMPI_SUCCESS) return error;

   error = init_operation(EMPI_BXOR, MAGPIE_BXOR, 1);
   if (error != EMPI_SUCCESS) return error;

   error = init_operation(EMPI_LOR, MAGPIE_LOR, 1);
   if (error != EMPI_SUCCESS) return error;

   error = init_operation(EMPI_LAND, MAGPIE_LAND, 1);
   if (error != EMPI_SUCCESS) return error;

   error = init_operation(EMPI_LXOR, MAGPIE_LXOR, 1);
   if (error != EMPI_SUCCESS) return error;

   return EMPI_SUCCESS;
}

operation *handle_to_operation(EMPI_Op handle)
{
   if (handle < 0 || handle >= MAX_OPERATIONS) {
      ERROR(1, "handle_to_operation(handle=%d) Handle out of bounds!\n", handle);
      return NULL;
   }

   return ops[handle];
}

EMPI_Op operation_to_handle(operation *o)
{
   if (o == NULL) {
      ERROR(1, "operation_to_handle(o=NULL) Operation is NULL!\n");
      return -1;
   }

   return o->handle;
}

