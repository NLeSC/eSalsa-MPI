#include "flags.h"

#ifdef IBIS_INTERCEPT

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "shared.h"
#include "types.h"
#include "request.h"
#include "logging.h"

static request *reqs[MAX_REQUESTS];

int init_request()
{
   int i;

   FORTRAN_MPI_REQUEST_NULL = PMPI_Request_c2f(MPI_REQUEST_NULL);

   for (i=0;i<MAX_REQUESTS;i++) {
      reqs[i] = NULL;
   }

   return MPI_SUCCESS;
}

static int is_special(int index)
{
   return (index == FORTRAN_MPI_REQUEST_NULL);
}

static int add_request(request *req)
{
   int i;

   for (i=0;i<MAX_REQUESTS;i++) {
      if (!is_special(i) && reqs[i] == NULL) {
         reqs[i] = req;
         return i;
      }
   }

   return -1;
}

request *create_request(int flags, void *buf, int count, MPI_Datatype datatype, int dest, int tag, communicator *c) {

   request *r = malloc(sizeof(request));

   if (r == NULL) {
      ERROR(1, "Failed to allocate request!\n");
      return NULL;
   }

   r->flags = flags;
   r->buf = buf;
   r->type = datatype;
   r->count = count;
   r->source_or_dest = dest;
   r->tag = tag;
   r->c = c;
   r->error = 0;
   r->req = MPI_REQUEST_NULL;
   r->index = add_request(r);

   if (r->index == -1) {
      ERROR(1, "Failed to store request!\n");
      free(r);
      return NULL;
   }

   return r;
}

void free_request(request *r)
{
   if (r != NULL) {
      if (r->req != MPI_REQUEST_NULL) {
          PMPI_Request_free(&(r->req));
      }

      reqs[r->index] = NULL;

      free(r);
   }
}

request *get_request(MPI_Request r)
{
   request *res;

   if (r == MPI_REQUEST_NULL) {
      return NULL;
   }

   memcpy(&res, &r, sizeof(request *));
   return res;
}

request *get_request_with_index(int index)
{
   if (index < 0 || index >= MAX_REQUESTS) {
      ERROR(1, "Failed to find request %d (out of bounds)!\n", index);
      return NULL;
   }

   if (reqs[index] == NULL) {
      ERROR(1, "Failed to find request %d (empty slot)!\n", index);
      return NULL;
   }

   return reqs[index];
}

void set_request_ptr(MPI_Request *dst, request *src)
{
   memcpy(dst, &src, sizeof(request *));
}

int request_active(request *r)
{
   return (r->flags & REQUEST_FLAG_ACTIVE);
}

int request_local(request *r)
{
   return (r->flags & REQUEST_FLAG_LOCAL);
}

int request_persistent(request *r)
{
   return (r->flags & REQUEST_FLAG_PERSISTENT);
}

int request_send(request *r)
{
   return (r->flags & REQUEST_FLAG_SEND);
}

int request_receive(request *r)
{
   return (r->flags & REQUEST_FLAG_RECEIVE);
}

int request_completed(request *r)
{
   return (r->flags & REQUEST_FLAG_COMPLETED);
}

MPI_Comm request_get_mpi_comm(MPI_Request r, MPI_Comm def)
{
   MPI_Comm tmp;

   request *req = get_request(r);

   if (req == NULL) {
WARN(1, "req == NULL: Returning default comm!");
      return def;
   }

   set_communicator_ptr(&tmp, req->c);
   return tmp;
}

#endif // IBIS_INTERCEPT
