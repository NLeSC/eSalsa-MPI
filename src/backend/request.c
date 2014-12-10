#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "settings.h"

#include "shared.h"
#include "types.h"
#include "request.h"
#include "logging.h"

static request *reqs[MAX_REQUESTS];

int init_request()
{
   int i;

   for (i=0;i<MAX_REQUESTS;i++) {
      reqs[i] = NULL;
   }

   return EMPI_SUCCESS;
}

//static int is_special(int index)
//{
//   return (index == FORTRAN_MPI_REQUEST_NULL);
//}

static int add_request(request *req)
{
   int i;

   for (i=0;i<MAX_REQUESTS;i++) {
      if (/*!is_special(i) &&*/ reqs[i] == NULL) {
         reqs[i] = req;
         return i;
      }
   }

   return -1;
}

request *create_request(int flags, void *buf, int count, datatype *type, int dest, int tag, communicator *c) {

   request *r = malloc(sizeof(request));

   if (r == NULL) {
      ERROR(1, "Failed to allocate request!\n");
      return NULL;
   }

   r->flags = flags;
   r->buf = buf;
   r->type = type;
   r->count = count;
   r->source_or_dest = dest;
   r->tag = tag;
   r->c = c;
   r->error = 0;
   r->req = MPI_REQUEST_NULL;
   r->handle = add_request(r);

   if (r->handle == -1) {
      ERROR(1, "Failed to store request!\n");
      free(r);
      return NULL;
   }

   r->sequence_nr = 0;
   r->fragment_buf = NULL;
   r->fragment_count = 0;

   return r;
}

void free_request(request *r)
{
   if (r != NULL) {
      if (r->req != MPI_REQUEST_NULL) {
          PMPI_Request_free(&(r->req));
      }

      reqs[r->handle] = NULL;

      free(r);
   }
}

request *handle_to_request(EMPI_Request handle)
{
   if (handle < 0 || handle >= MAX_REQUESTS) {
      ERROR(1, "handle_to_request(handle=%d) handle out of bounds!", handle);
      return NULL;
   }

   if (reqs[handle] == NULL) {
//      ERROR(1, "handle_to_request(handle=%d) request not found!", handle);
      return NULL;
   }

   return reqs[handle];
}

EMPI_Request request_to_handle(request *r)
{
   if (r == NULL) {
      ERROR(1, "request_to_handle(r=NULL) request is NULL!");
      return -1;
   }

   return r->handle;
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

EMPI_Comm request_get_comm(EMPI_Request *r, EMPI_Comm def)
{
   if (r == NULL) {
      WARN(1, "request_get_comm(r=NULL, def=%d) Request is NULL returning default!", def);
      return def;
   }

   request *req = handle_to_request(*r);

   if (req == NULL) {
      WARN(1, "request_get_comm(*r=%d, def=%d) Request not found, returning default!", *r, def);
      return def;
   }

   return communicator_to_handle(req->c);
}

