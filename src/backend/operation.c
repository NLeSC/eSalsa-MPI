#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "../../include/settings.h"

#include "shared.h"
#include "types.h"
#include "logging.h"
#include "operation.h"
#include "reduce_ops.h"

static operation *ops[MAX_OPERATIONS];
static int next_operation = 0;

static int init_operation(EMPI_Op handle, EMPI_User_function *function, MPI_Op op, int commute)
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
   ops[handle]->op = op;

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

   error = init_operation(EMPI_OP_NULL, NULL, MPI_OP_NULL, 1);
   if (error != EMPI_SUCCESS) return error;

   error = init_operation(EMPI_MAX, EMPI_REDUCE_MAX, MPI_MAX, 1);
   if (error != EMPI_SUCCESS) return error;

   error = init_operation(EMPI_MIN, EMPI_REDUCE_MIN, MPI_MIN, 1);
   if (error != EMPI_SUCCESS) return error;

   error = init_operation(EMPI_SUM, EMPI_REDUCE_SUM, MPI_SUM, 1);
   if (error != EMPI_SUCCESS) return error;

   error = init_operation(EMPI_PROD, EMPI_REDUCE_PROD, MPI_PROD, 1);
   if (error != EMPI_SUCCESS) return error;

   error = init_operation(EMPI_MAXLOC, EMPI_REDUCE_MAXLOC, MPI_MAXLOC, 1);
   if (error != EMPI_SUCCESS) return error;

   error = init_operation(EMPI_MINLOC, EMPI_REDUCE_MINLOC, MPI_MINLOC, 1);
   if (error != EMPI_SUCCESS) return error;

   error = init_operation(EMPI_BOR, EMPI_REDUCE_BOR, MPI_BOR, 1);
   if (error != EMPI_SUCCESS) return error;

   error = init_operation(EMPI_BAND, EMPI_REDUCE_BAND, MPI_BAND, 1);
   if (error != EMPI_SUCCESS) return error;

   error = init_operation(EMPI_BXOR, EMPI_REDUCE_BXOR, MPI_BXOR, 1);
   if (error != EMPI_SUCCESS) return error;

   error = init_operation(EMPI_LOR, EMPI_REDUCE_LOR, MPI_LOR, 1);
   if (error != EMPI_SUCCESS) return error;

   error = init_operation(EMPI_LAND, EMPI_REDUCE_LAND, MPI_LAND, 1);
   if (error != EMPI_SUCCESS) return error;

   error = init_operation(EMPI_LXOR, EMPI_REDUCE_LXOR, MPI_LXOR, 1);
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

operation *create_operation(EMPI_User_function *function, MPI_Op op, int commute)
{
   int handle, error;

   handle = next_operation;
   next_operation++;

   error = init_operation(handle, function, op, commute);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "create_operation failed(%d)!\n", error);
      return NULL;
   }

   return ops[handle];
}


