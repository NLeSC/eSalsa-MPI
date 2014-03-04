#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <execinfo.h>

#include "../../include/settings.h"

#include "empi.h"
#include "mpi.h"
#include "error.h"
#include "datatype.h"
#include "status.h"
#include "file.h"
#include "info.h"

#include "types.h"
//#include "generated_header.h"

#include "logging.h"
#include "request.h"
#include "communicator.h"
#include "group.h"
#include "messaging.h"
#include "wa_sockets.h"
#include "util.h"
#include "debugging.h"
#include "operation.h"
//#include "profiling.h"


// The number of processes in our cluster and our process' rank in this set.
int local_count;
int local_rank;

/* The following external fields are provided by the WA implementation */

// The number of clusters and the rank of our cluster in this set.
extern uint32_t cluster_count;
extern uint32_t cluster_rank;

// The pid of my machine.
extern uint32_t my_pid;

// The name of this cluster (must be unique).
extern char *cluster_name;

// The size of each cluster, and the offset of each cluster in the
// total set of machines.
extern int *cluster_sizes;
extern int *cluster_offsets;


// Value of various Fortan MPI constants.
// DO WE NEED THESE ?

/*
int FORTRAN_MPI_COMM_NULL;
int FORTRAN_MPI_GROUP_NULL;
int FORTRAN_MPI_REQUEST_NULL;
int FORTRAN_MPI_GROUP_EMPTY;
int FORTRAN_MPI_COMM_WORLD;
int FORTRAN_MPI_COMM_SELF;
int FORTRAN_MPI_OP_NULL;
*/

int FORTRAN_FALSE;
int FORTRAN_TRUE;

static void init_constants()
{
   // FIXME: do we need these?
/*
   FORTRAN_MPI_COMM_NULL = PMPI_Comm_c2f(MPI_COMM_NULL);
   FORTRAN_MPI_COMM_WORLD = PMPI_Comm_c2f(MPI_COMM_WORLD);
   FORTRAN_MPI_COMM_SELF = PMPI_Comm_c2f(MPI_COMM_SELF);

   FORTRAN_MPI_GROUP_NULL = PMPI_Group_c2f(MPI_GROUP_NULL);
   FORTRAN_MPI_GROUP_EMPTY = PMPI_Group_c2f(MPI_GROUP_EMPTY);

   FORTRAN_MPI_REQUEST_NULL = PMPI_Request_c2f(MPI_REQUEST_NULL);

   FORTRAN_MPI_OP_NULL = PMPI_Op_c2f(MPI_OP_NULL);
*/

   init_fortran_logical_(&FORTRAN_TRUE, &FORTRAN_FALSE);

/*
   INFO(1, "MPI_COMM_NULL    = %p / %d", (void *)MPI_COMM_NULL, FORTRAN_MPI_COMM_NULL);
   INFO(1, "MPI_COMM_WORLD   = %p / %d", (void *)MPI_COMM_WORLD, FORTRAN_MPI_COMM_WORLD);
   INFO(1, "MPI_COMM_SELF    = %p / %d", (void *)MPI_COMM_SELF, FORTRAN_MPI_COMM_SELF);
   INFO(1, "MPI_GROUP_NULL   = %p / %d", (void *)MPI_GROUP_NULL, FORTRAN_MPI_GROUP_NULL);
   INFO(1, "MPI_GROUP_EMPTY  = %p / %d", (void *)MPI_GROUP_EMPTY, FORTRAN_MPI_GROUP_EMPTY);
   INFO(1, "MPI_REQUEST_NULL = %p / %d", (void *)MPI_REQUEST_NULL, FORTRAN_MPI_REQUEST_NULL);
   INFO(1, "MPI_OP_NULL      = %p / %d", (void *)MPI_OP_NULL, FORTRAN_MPI_OP_NULL);
*/
   INFO(1, "FORTRAN_TRUE     = %d", FORTRAN_TRUE);
   INFO(1, "FORTRAN_FALSE    = %d", FORTRAN_FALSE);
}

#define __EMPI_Init
int EMPI_Init(int *argc, char **argv[])
{
   int i=0;

   INFO(0, "Init MPI...");

   int status = PMPI_Init(argc, argv);

   if (status != MPI_SUCCESS) {
       ERROR(1, "Failed to initialize MPI (error=%d)", status);
       return EMPI_ERR_INTERN;
   }

   PMPI_Comm_rank(MPI_COMM_WORLD, &local_rank);
   PMPI_Comm_size(MPI_COMM_WORLD, &local_count);

   INFO(1, "START EMPI on %d of %d", local_rank, local_count);

   for (i=0;i<*argc;i++) {
      INFO(4, "argv[%d] = %s", i, (*argv)[i]);
   }

   init_constants();

   status = init_groups();

   if (status != EMPI_SUCCESS) {
      PMPI_Finalize();
      ERROR(1, "Failed to initialize groups! (error=%d)", status);
      return status;
   }

   status = init_request();

   if (status != EMPI_SUCCESS) {
      PMPI_Finalize();
      ERROR(1, "Failed to initialize requests! (error=%d)", status);
      return status;
   }

   status = init_datatypes();

   if (status != EMPI_SUCCESS) {
      PMPI_Finalize();
      ERROR(1, "Failed to initialize datatypes! (error=%d)", status);
      return status;
   }

   status = init_operations();

   if (status != EMPI_SUCCESS) {
      PMPI_Finalize();
      ERROR(1, "Failed to initialize operations! (error=%d)", status);
      return status;
   }

   status = init_files();

   if (status != EMPI_SUCCESS) {
      PMPI_Finalize();
      ERROR(1, "Failed to initialize files! (error=%d)", status);
      return status;
   }

   status = init_infos();

   if (status != EMPI_SUCCESS) {
      PMPI_Finalize();
      ERROR(1, "Failed to initialize infos! (error=%d)", status);
      return status;
   }

   status = wa_init(local_rank, local_count, argc, argv);

   if (status != EMPI_SUCCESS) {
      PMPI_Finalize();
      ERROR(1, "Failed to initialize WA link!");
      return EMPI_ERR_INTERN;
   }

   status = init_communicators(cluster_rank, cluster_count,
                                  cluster_sizes, cluster_offsets);

   if (status != EMPI_SUCCESS) {
      wa_finalize();
      PMPI_Finalize();
      ERROR(1, "Failed to initialize communicators! (error=%d)", status);
      return status;
   }

   return EMPI_SUCCESS;
}

#define __EMPI_Finalize
int EMPI_Finalize(void)
{
   int error;

   // We tell the system to shut down by terminating EMPI_COMM_WORLD.
   error = messaging_send_terminate_request(handle_to_communicator(EMPI_COMM_WORLD));

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to terminate EMPI_COMM_WORLD! (error=%d)", error);
      return error;
   }

   wa_finalize();

   return TRANSLATE_ERROR(PMPI_Finalize());
}

#define __EMPI_Abort
int EMPI_Abort(EMPI_Comm comm, int errorcode)
{
   communicator *c = handle_to_communicator(comm);

   return TRANSLATE_ERROR(PMPI_Abort(c->comm, errorcode));
}

#define __EMPI_Initialized
int EMPI_Initialized ( int *flag )
{
   return TRANSLATE_ERROR(PMPI_Initialized(flag));
}

#define __EMPI_Finalized
int EMPI_Finalized ( int *flag )
{
   return TRANSLATE_ERROR(PMPI_Finalized(flag));
}

#define __EMPI_Wtime
double EMPI_Wtime ( void )
{
   return PMPI_Wtime();
}

#define __EMPI_Error_string
int EMPI_Error_string ( int errorcode, char *string, int *resultlen )
{
   // FIXME: Use own error codes ?
   return TRANSLATE_ERROR(PMPI_Error_string(errorcode, string, resultlen));
}

#define __EMPI_Get_processor_name
int EMPI_Get_processor_name ( char *name, int *resultlen )
{
   return TRANSLATE_ERROR(PMPI_Get_processor_name(name, resultlen));
}


/*****************************************************************************/
/*                             Send / Receive                                */
/*****************************************************************************/

static int do_send(void* buf, int count, datatype *t, int dest, int tag, communicator *c)
{
   int error;

   if (rank_is_local(c, dest)) {
      error = TRANSLATE_ERROR(PMPI_Send(buf, count, t->type, get_local_rank(c, dest), tag, c->comm));
   } else {
      error = messaging_send(buf, count, t, dest, tag, c);
   }

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Rank %d (in cluster %d) failed to send data to %d (in cluster %d) (comm=%d, error=%d)!\n",
         c->global_rank, cluster_rank, dest, get_cluster_rank(c, dest), c->handle, error);
   }

   return error;
}

static int translate_status(communicator *c, datatype *t, status *s, MPI_Status *mstatus)
{
    int error, source, cancelled, count;

    count = 0;

    if (s == EMPI_STATUS_IGNORE) {
       return EMPI_SUCCESS;
    }

    if (mstatus == MPI_STATUS_IGNORE) {
       clear_status(s);
       return EMPI_SUCCESS;
    }

    source = get_global_rank(c, cluster_rank, mstatus->MPI_SOURCE);

    error = TRANSLATE_ERROR(PMPI_Test_cancelled(mstatus, &cancelled));

    if (error != MPI_SUCCESS) {
       return error;
    }

    if (!cancelled) {
       error = TRANSLATE_ERROR(PMPI_Get_count(mstatus, t->type, &count));

       if (error != EMPI_SUCCESS) {
          return error;
       }
    }

    set_status(s, source, mstatus->MPI_TAG, TRANSLATE_ERROR(mstatus->MPI_ERROR), t, count, cancelled);
    return EMPI_SUCCESS;
}

// FIXME: assumes source != MPI_ANY_SOURCE ?
static int do_recv(void *buf, int count, datatype *t, int source, int tag, status *s, communicator *c)
{
   int error;
   MPI_Status mstatus;

   if (source == EMPI_ANY_SOURCE) {
      FATAL("do_recv from EMPI_ANY_SOURCE not supported yet!");
      return EMPI_ERR_INTERN;
   }

   if (rank_is_local(c, source)) {
      // local recv
      error = TRANSLATE_ERROR(PMPI_Recv(buf, count, t->type, get_local_rank(c, source), tag, c->comm, &mstatus));

      if (error == EMPI_SUCCESS) {
         error = translate_status(c, t, s, &mstatus);
      }
   } else {
      // remote recv
      error = messaging_receive(buf, count, t, source, tag, s, c);
   }

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Rank %d (in cluster %d) failed to receive data from %d (in cluster %d) (comm=%d, error=%d)!\n", c->global_rank, cluster_rank, source, get_cluster_rank(c, source), c->handle, error);
   }

   return error;
}

#define __EMPI_Send
int EMPI_Send(void* buf, int count, EMPI_Datatype type, int dest, int tag, EMPI_Comm comm)
{
   communicator *c = handle_to_communicator(comm);
   datatype *t = handle_to_datatype(type);

   CHECK_COUNT(count);
   CHECK_DESTINATION(c, dest);

   return do_send(buf, count, t, dest, tag, c);
}

#define __EMPI_Ssend
int EMPI_Ssend ( void *buf, int count, EMPI_Datatype type, int dest, int tag, EMPI_Comm comm)
{
   communicator *c = handle_to_communicator(comm);
   datatype *t = handle_to_datatype(type);

   CHECK_COUNT(count);
   CHECK_DESTINATION(c, dest);

   if (comm_is_local(c)) {
     // simply perform a ssend in local cluster
     return TRANSLATE_ERROR(PMPI_Ssend(buf, count, t->type, dest, tag, c->comm));
   }

   if (rank_is_local(c, dest)) {
      // local send
     return TRANSLATE_ERROR(PMPI_Ssend(buf, count, t->type, get_local_rank(c, dest), tag, c->comm));
   } else {
     // remote send
     WARN(1, "Incorrect WA ssend implementation (in communicator %d)!", c->handle);
     return messaging_send(buf, count, t, dest, tag, c);
   }
}

#define __EMPI_Rsend
int EMPI_Rsend(void* buf, int count, EMPI_Datatype type, int dest, int tag, EMPI_Comm comm)
{
   // In rsend, it is the users responsibility to ensure that the
   // matching receive has already been posted -before- this rsend
   // is invoked. This allows MPI to assume all is well and proceed
   // in sending the message without additional handshakes. If the
   // receive has not been posted, however, the program is erroneous
   // and may crash.
   //
   // In this implementation we simply replace rsend by ssend, which
   // offers a stricter (less efficient) contract.
   return EMPI_Ssend(buf, count, type, dest, tag, comm);
}

#define __EMPI_Isend
int EMPI_Isend(void *buf, int count, EMPI_Datatype type, int dest, int tag, EMPI_Comm comm, EMPI_Request *req)
{
   int error, local, flags = REQUEST_FLAG_SEND;
   request *r;

   communicator *c = handle_to_communicator(comm);
   datatype *t = handle_to_datatype(type);

   CHECK_COUNT(count);
   CHECK_DESTINATION(c, dest);

   local = rank_is_local(c, dest);

   if (local) {
      flags |= REQUEST_FLAG_LOCAL;
   }

   r = create_request(flags, buf, count, t, dest, tag, c);

   if (r == NULL) {
      FATAL("Failed to create request!");
      return EMPI_ERR_INTERN;
   }

   if (local) {
      error = TRANSLATE_ERROR(PMPI_Isend(buf, count, t->type, get_local_rank(c, dest), tag, c->comm, &(r->req)));
   } else {
      error = messaging_send(buf, count, t, dest, tag, c);
   }

   if (error != EMPI_SUCCESS) {
      free_request(r);
      *req = EMPI_REQUEST_NULL;
      ERROR(1, "Failed to send data! (comm=%d, error=%d)", c->handle, error);
      return error;
   }

   // We stuff our own data into the users request pointer here...
   *req = r->handle;
   return EMPI_SUCCESS;
}

#define __EMPI_Irsend
int EMPI_Irsend(void *buf, int count, EMPI_Datatype type,
                int dest, int tag, EMPI_Comm comm, EMPI_Request *req)
{
   // As with rsend, we can simply replace this call with a normal isend.
   return EMPI_Isend(buf, count, type, dest, tag, comm, req);
}

#define __EMPI_Irecv
int EMPI_Irecv(void *buf, int count, EMPI_Datatype type,
               int source, int tag, EMPI_Comm comm, EMPI_Request *req)
{
   int error, local, flags = REQUEST_FLAG_RECEIVE, local_source;
   request *r;

   // We first unpack the communicator.
   communicator *c = handle_to_communicator(comm);
   datatype *t = handle_to_datatype(type);

   CHECK_COUNT(count);
   CHECK_SOURCE(c, source);

   // Next, we check if the source is local or not.
   if (source == EMPI_ANY_SOURCE) {
      // if source is any, the local flag is determined by the distribution of the communicator.
      local = comm_is_local(c);
   } else {
      local = rank_is_local(c, source);
   }

   // Next, we create the request struct.
   if (local) {
      flags |= REQUEST_FLAG_LOCAL;
   }

   r = create_request(flags, buf, count, t, source, tag, c);

   if (r == NULL) {
      IERROR(1, "Failed to create request!");
      return EMPI_ERR_INTERN;
   }

   // Post the ireceive if it is local
   if (local == 1) {

      if (source == EMPI_ANY_SOURCE) {
         local_source = EMPI_ANY_SOURCE;
      } else {
         local_source = get_local_rank(c, source);
      }

      // If the source is guarenteed to be local, we directly use MPI.
      error = TRANSLATE_ERROR(PMPI_Irecv(buf, count, t->type, local_source, tag, c->comm, &(r->req)));

      if (error != EMPI_SUCCESS) {
         ERROR(1, "IRecv failed! (comm=%d,error=%d)", c->handle, error);
         free_request(r);
         *req = EMPI_REQUEST_NULL;
         return error;
      }
   }

/****
   } else if (source != MPI_ANY_SOURCE) {
      // If the source is guarenteed to be remote, we directly use the WA link.
      error = messaging_probe_receive(r, 0);

   } else { // local == 0 && source == MPI_ANY_SOURCE

      // If the source may be local or remote, we must poll both. Start with local MPI.
      // FIXME: fixed order may cause starvation ?
      error = PMPI_Iprobe(MPI_ANY_SOURCE, tag, c->comm, &flag, MPI_STATUS_IGNORE);

      if (error != MPI_SUCCESS) {
         IERROR(1, "IProbe from MPI_ANY_SOURCE failed!");
         return MPI_ERR_INTERN;
      }

      if (flag) {
         // A message is available locally, so receiving it!
         r->error = PMPI_Recv(buf, count, datatype, MPI_ANY_SOURCE, tag, c->comm, MPI_STATUS_IGNORE);
         r->flags |= REQUEST_FLAG_COMPLETED;
      } else {
         // No local message was found (yet), so try the WA link.
         error = messaging_probe_receive(r, 0);
      }
   }

***/

   // We stuff our own data into the users request pointer here...
   *req = request_to_handle(r);
   return EMPI_SUCCESS;
}

#define __EMPI_Recv
int EMPI_Recv(void *buf, int count, EMPI_Datatype type, int source, int tag, EMPI_Comm comm, EMPI_Status *s)
{
   int local, error;
   MPI_Status mstatus;

   communicator *c = handle_to_communicator(comm);
   datatype *t = handle_to_datatype(type);

   CHECK_COUNT(count);
   CHECK_SOURCE(c, source);

   if (source == EMPI_ANY_SOURCE) {
      // if source is any, the local flag is determined by the distribution of the communicator.
      local = comm_is_local(c);
   } else {
      local = rank_is_local(c, source);
   }

   if (local == 1) {
      error = TRANSLATE_ERROR(PMPI_Recv(buf, count, t->type, source, tag, c->comm, &mstatus));

      if (error == EMPI_SUCCESS && s != EMPI_STATUS_IGNORE) {
         error = translate_status(c, t, s, &mstatus);
      }

      return error;
   } else {

// FIXME: messaging_receive will block on the WA, which is NOT what we want!

      if (source == EMPI_ANY_SOURCE) {
         IERROR(0, "MPI_Recv from MIXED MPI_ANY_SOURCE not implemented yet (buggerit!)");
         return EMPI_ERR_RANK;
      }

      return messaging_receive(buf, count, t, source, tag, s, c);
   }
}


#define __EMPI_Sendrecv
int EMPI_Sendrecv(void *sendbuf, int sendcount, EMPI_Datatype sendtype, int dest, int sendtag,
                  void *recvbuf, int recvcount, EMPI_Datatype recvtype, int source, int recvtag,
                  EMPI_Comm comm, EMPI_Status *s)
{
   // FIXME: This implementation id BS!!
   // Reimplement using isend/irecv/wait!
   int error;

   MPI_Status mstatus;

   datatype *stype = handle_to_datatype(sendtype);
   datatype *rtype = handle_to_datatype(recvtype);
   communicator *c = handle_to_communicator(comm);

   CHECK_COUNT(sendcount);
   CHECK_DESTINATION(c, dest);

   CHECK_COUNT(recvcount);
   CHECK_SOURCE(c, source);

   if (comm_is_local(c)) {
     // simply perform a sendrecv in local cluster
     error = TRANSLATE_ERROR(PMPI_Sendrecv(sendbuf, sendcount, stype->type, dest, sendtag, recvbuf, recvcount, rtype->type, source, recvtag, c->comm, &mstatus));

     if (error == MPI_SUCCESS && s != EMPI_STATUS_IGNORE) {
        error = translate_status(c, rtype, s, &mstatus);
     }

     return error;
   }

   // FIXME: Does this work if source == MPI_ANY_SOURCE ?
   // FIXME: Is this correct ???
   // FIXME: Shouldn't we just translate this to a isend / irecv / wait ?

   // We need to perform a WA sendrecv. NOTE: It is still possible that both the send
   // and recv operations are local. We cannot directly use MPI_Sendrecv, however, as
   // the dest and source ranks may be invalid locally.

   // Reverse the send and receive order on even and odd processors.
   // FIXME: -- WHY ?

// FIXME: THIS CANNOT BE RIGHT!!

   IERROR(0, "MPI_Sendrecv WA not implemented correctly FIXME!! (buggerit!)");

   if ((c->global_rank % 2) == 0) {

      error = do_send(sendbuf, sendcount, stype, dest, sendtag, c);

      if (error != EMPI_SUCCESS) {
         return error;
      }

      error = do_recv(recvbuf, recvcount, rtype, source, recvtag, s, c);

      if (error != EMPI_SUCCESS) {
         return error;
      }

   } else {

      error = do_recv(recvbuf, recvcount, rtype, source, recvtag, s, c);

      if (error != EMPI_SUCCESS) {
         return error;
      }

      error = do_send(sendbuf, sendcount, stype, dest, sendtag, c);

      if (error != EMPI_SUCCESS) {
         return error;
      }
   }

   return EMPI_SUCCESS;
}

/*****************************************************************************/
/*                             Waits / Polling                               */
/*****************************************************************************/

static int probe_request(EMPI_Request *req, int blocking, int *flag, EMPI_Status *s)
{
   int error = EMPI_SUCCESS;
//   MPI_Aint extent;
   MPI_Status mstatus;

   request *r = handle_to_request(*req);

   if (r == NULL) {

      DEBUG(1, "request=NULL, blocking=%d", blocking);

      clear_status(s);
      *flag = 1;
      return EMPI_SUCCESS;
   }

/*
// FIXME: for performance debugging!!!

   if (r->c->handle == 23 || r->c->handle == 24 || r->c->handle == 16) {

      error = PMPI_Type_extent(r->type, &extent);

      if (error != MPI_SUCCESS) {
         extent = 0;
      }

      STACKTRACE(0, "in wait/test for comm %d count %d bytes %d", r->c->handle, r->count, r->count*extent);
   }

// END FIXME: for performance debugging!!!
*/

   INFO(1, "request=(index=%d, flags=%d, srcdest=%d, count=%d, tag=%d type=%s) blocking=%d",
	r->handle, r->flags, r->source_or_dest, r->count, r->tag, r->type, blocking);

   // We don't support persistent request yet!
   if (request_persistent(r)) {
      FATAL(0, "Persistent requests not supported yet! (MPI_Test)");
      clear_status(s);
      free_request(r);
      *flag = 1;
      *req = EMPI_REQUEST_NULL;
      return EMPI_ERR_REQUEST;

   } else if (request_local(r)) {
      // Pure local request, so we ask MPI.

      INFO(2, "request=LOCAL blocking=%d %p", blocking, r->req);

      if (blocking) {
         r->error = TRANSLATE_ERROR(PMPI_Wait(&(r->req), &mstatus));
         *flag = 1;
      } else {
         r->error = TRANSLATE_ERROR(PMPI_Test(&(r->req), flag, &mstatus));
      }

      if (*flag == 1) {
         // We must translate local source rank to global rank.
         INFO(2, "translate local rank=%d to global rank", s->MPI_SOURCE);
         error = translate_status(r->c, r->type, s, &mstatus);
      }

   } else if (request_send(r)) {

      INFO(2, "request=WA_SEND blocking=%d", blocking);

      // Non-persistent WA send should already have finished.
      set_status(s, r->source_or_dest, r->tag, EMPI_SUCCESS, r->type, r->count, FALSE);
      r->error = EMPI_SUCCESS;
      *flag = 1;

   } else if (r->source_or_dest != MPI_ANY_SOURCE) {

      INFO(2, "request=WA_RECEIVE blocking=%d", blocking);

      // It was a non-persistent remote receive request, so probe the
      // WA link (will do nothing if the request was already completed).
      messaging_probe_receive(r, blocking);

      if (request_completed(r)) {
         *flag = 1;
         if (r->error == EMPI_SUCCESS) {
            r->error = messaging_finalize_receive(r, s);
         } else {
            set_status_error(s, r->source_or_dest, r->tag, r->error, r->type);
         }
      }

   } else {
      // It was a non-persistent mixed receive request, so we must probe
      // the local network and the WA link.

      INFO(2, "request=WA_RECEIVE_ANY blocking=%d", blocking);

      if (request_completed(r)) {
         DEBUG(3, "request=WA_RECEIVE_ANY already completed by source=%d tag=%d count=%d", r->source_or_dest, r->tag, r->count);
         *flag = 1;
      } else {
         do {
            // Probe locally first
            DEBUG(3, "request=WA_RECEIVE_ANY performing LOCAL probe for ANY, %d", r->tag);

            r->error = TRANSLATE_ERROR(PMPI_Iprobe(MPI_ANY_SOURCE, r->tag, r->c->comm, flag, MPI_STATUS_IGNORE));

            if (r->error != EMPI_SUCCESS) {
               IERROR(1, "IProbe from MPI_ANY_SOURCE failed! ()");
               return EMPI_ERR_INTERN;
            }

            if (*flag) {

               DEBUG(3, "request=WA_RECEIVE_ANY performing LOCAL receive");

               // A message is available locally, so receiving it!
               r->error = TRANSLATE_ERROR(PMPI_Recv(r->buf, r->count, r->type->type, MPI_ANY_SOURCE, r->tag, r->c->comm, &mstatus));
               r->flags |= REQUEST_FLAG_COMPLETED;

               // We must translate local source rank to global rank.
               error = translate_status(r->c, r->type, s, &mstatus);

            } else {

               DEBUG(3, "request=WA_RECEIVE_ANY performing WA probe");

               // No local message was found (yet), so try the WA link.
               // NOTE: we should poll here, so blocking is set to 0,
               // ignoring the value of the parameter.
               r->error = messaging_probe_receive(r, 0);

               if (request_completed(r)) {
                  DEBUG(3, "request=WA_RECEIVE_ANY performed WA receive");

                  if (r->error == EMPI_SUCCESS) {
                     r->error = messaging_finalize_receive(r, s);
                  } else {
                     set_status_error(s, r->source_or_dest, r->tag, r->error, r->type);
                  }
                  *flag = 1;
               }
            }

         } while (blocking && (*flag == 0) && (r->error == EMPI_SUCCESS));
      }
   }

   if (*flag == 1) {

      DEBUG(1, "received data %d", ((int *)r->buf)[0]);

      error = r->error;
      free_request(r);
      *req = EMPI_REQUEST_NULL;
   }

   DEBUG(1, "done error=%d");

   return error;
}

#define __EMPI_Test
int EMPI_Test(EMPI_Request *req, int *flag, EMPI_Status *s)
{
   int error = probe_request(req, 0, flag, s);

   if (error != EMPI_SUCCESS) {
      IERROR(1, "Probe request failed!");
   }

   return error;
}

#define __EMPI_Wait
int EMPI_Wait(EMPI_Request *req, EMPI_Status *s)
{
   int flag = 0;

   int error = probe_request(req, 1, &flag, s);

   if (error != EMPI_SUCCESS) {
      IERROR(1, "Probe request failed!");
   }

   return error;
}

#define __EMPI_Waitany
int EMPI_Waitany(int count, EMPI_Request *array_of_requests, int *index, EMPI_Status *s)
{
   int i;
   int error;
   int undef;
   int flag = 0;

   while (1) {

      undef = 0;

      for (i=0;i<count;i++) {
         if (array_of_requests[i] != EMPI_REQUEST_NULL) {
            error = EMPI_Test(&array_of_requests[i], &flag, s);

            if (error != EMPI_SUCCESS) {
               *index = i;
               return EMPI_ERR_IN_STATUS;
            }

            if (flag) {
               *index = i;
               return EMPI_SUCCESS;
            }

         } else {
            undef++;
         }
      }

      if (undef == count) {
         // All requests where MPI_REQUEST_NULL
         *index = EMPI_UNDEFINED;
         clear_status(s);
         return EMPI_SUCCESS;
      }
   }

   // unreachable!
   return EMPI_ERR_INTERN;
}

#define __EMPI_Waitall
int EMPI_Waitall(int count, EMPI_Request *array_of_requests, EMPI_Status *array_of_statuses)
{
   int i, error;
   int errors = 0;
   int done = 0;
   int flag;

   while (done < count) {

      DEBUG(0, "Waiting for %d of %d requests", count-done, count);

      for (i=0;i<count;i++) {
         if (array_of_requests[i] != EMPI_REQUEST_NULL) {

            DEBUG(1, "checking %d of %d request=%s", i, count, array_of_requests[i]);

            flag = 0;

            error = EMPI_Test(&array_of_requests[i], &flag, &array_of_statuses[i]);

            DEBUG(1, "test %d of %d request=%s -> flag=%d error=%d", i, count, array_of_requests[i], flag, error);

            if (flag == 1) {
              done++;
              array_of_requests[i] = EMPI_REQUEST_NULL;
            }

            if (error != EMPI_SUCCESS) {
               errors++;
            }
         }
      }
   }

   DEBUG(0, "count=%d error=%d", count, errors);

   if (errors != 0) {
      return EMPI_ERR_IN_STATUS;
   }

   return EMPI_SUCCESS;
}


#define __EMPI_Request_free
int EMPI_Request_free(EMPI_Request *r)
{
   int error = EMPI_SUCCESS;
   request *req = NULL;

   // Check for special values of *r and r.
   if (r == NULL || *r == EMPI_REQUEST_NULL) {
      return EMPI_SUCCESS;
   }

   // Retrieve the request struct using the handle.
   req = handle_to_request(*r);

   if (req == NULL) {
      IERROR(0, "Request not found!\n");
      return EMPI_ERR_REQUEST;
   }

   // Also free the assosiated MPI_Request
   if (req->req != MPI_REQUEST_NULL) {
      error = TRANSLATE_ERROR(PMPI_Request_free(&(req->req)));
   }

   free_request(req);
   *r = EMPI_REQUEST_NULL;

   return error;
}

#define __EMPI_Request_get_status
int EMPI_Request_get_status(EMPI_Request req, int *flag, EMPI_Status *s)
{
   int error = EMPI_SUCCESS;
   MPI_Status mstatus;

   if (s != EMPI_STATUS_IGNORE) {
      clear_status(s);
   }

   if (req == EMPI_REQUEST_NULL) {
      *flag = 1;
      return EMPI_SUCCESS;
   }

   request *r = handle_to_request(req);

   if (r == NULL) {
      return EMPI_ERR_REQUEST;
   }

   // We don't support persistent request yet!
   if (request_persistent(r)) {
      IERROR(0, "persistent requests not supported yet! (MPI_Test)");
      *flag = 1;
      return EMPI_ERR_REQUEST;
   }

   if (request_local(r)) {
      // Pure local request, so we ask MPI.
      error = TRANSLATE_ERROR(PMPI_Request_get_status(r->req, flag, &mstatus));

      if (error == EMPI_SUCCESS && s != EMPI_STATUS_IGNORE) {
         error = translate_status(r->c, r->type, s, &mstatus);
      }

      return error;
  }

   // It was a WA or mixed request.
   // Non-persistent send should already have finished.
   if (request_send(r)) {
      set_status(s, r->source_or_dest, r->tag, EMPI_SUCCESS, r->type, r->count, FALSE);
      *flag = 1;
      return EMPI_SUCCESS;
   }

   // Non-persistent receive may not have completed yet.
   *flag = request_completed(r);

   if (!(*flag)) {
// FIXME: should also probe local here ??
      messaging_probe_receive(r, 0);
   }

   if (request_completed(r)) {
      *flag = 1;
      if (r->error == EMPI_SUCCESS) {
         r->error = messaging_finalize_receive(r, s);
      } else {
         set_status_error(s, r->source_or_dest, r->tag, r->error, r->type);
      }
   }

   if (*flag) {
      error = r->error;
   }

   return error;
}

#define __EMPI_Get_count
int EMPI_Get_count(EMPI_Status *s, EMPI_Datatype t, int *result)
{
   // MPI standard: EMPI_STATUS_IGNORE is not allowed.
   if (s == EMPI_STATUS_IGNORE) {
      return EMPI_ERR_ARG;
   }

   // MPI standard: A datatype with size 0 should return 0 if count is 0, or MPI_UNDEFINED otherwise.
   if (s->type == EMPI_DATATYPE_NULL) {
      if (t == EMPI_DATATYPE_NULL) {
         if (s->count == 0) {
            *result = 0;
         } else {
            *result = EMPI_UNDEFINED;
         }

         return EMPI_SUCCESS;
      }

      return EMPI_ERR_TYPE;
   }

   // MPI standard: The provide datatype should match the message.
   if (s->type != t) {
      return EMPI_ERR_TYPE;
   }

   *result = s->count;
   return EMPI_SUCCESS;
}


#define __EMPI_Get_elements
int EMPI_Get_elements(EMPI_Status *s, EMPI_Datatype t, int *result)
{
   // NOTE: As we don't support derived datatypes, EMPI_Get_count and
   // EMPI_Get_elements should return the same result!
   return EMPI_Get_count(s, t, result);
}


#define __EMPI_Get_elements
int EMPI_Status_set_elements(EMPI_Status *s, EMPI_Datatype t, int count)
{
   // MPI standard: EMPI_STATUS_IGNORE behaviour is unclear!!
   if (s == EMPI_STATUS_IGNORE) {
      return MPI_ERR_ARG;
   }

   datatype *type = handle_to_datatype(t);

   if (type == NULL) {
      ERROR(1, "Datatype %d not found!", t);
      return MPI_ERR_TYPE;
   }

   set_status_count(s, type, count);

   return EMPI_SUCCESS;
}


#define __EMPI_Status_set_cancelled
int EMPI_Status_set_cancelled(EMPI_Status *s, int flag)
{
   // MPI standard: EMPI_STATUS_IGNORE behaviour is unclear!!
   if (s == EMPI_STATUS_IGNORE) {
      return MPI_ERR_ARG;
   }

   set_status_cancelled(s, flag);

   return EMPI_SUCCESS;
}

/*****************************************************************************/
/*                              Collectives                                  */
/*****************************************************************************/

#define __EMPI_Barrier
int EMPI_Barrier(EMPI_Comm comm)
{
   int error, i;
   char buffer = 42;
   datatype *type_byte;

   communicator *c = handle_to_communicator(comm);

   if (comm_is_local(c)) {
     // simply perform a barrier in local cluster
     return TRANSLATE_ERROR(PMPI_Barrier(c->comm));
   }

   // We need to perform a WA barrier. This consists of three steps:
   //
   // 1) local barrier, to ensure all local processes have entered the call
   // 2) WA barrier (implemented using flat tree on coordinators) to ensure all
   //    clusters have entered the call
   // 3) local barrier, to ensure all local processes wait until their coordinator
   //    has finished the WA barrier.

//INFO(1, "WA BARRIER grank=%d lrank=%d coord[0]=%d", c->global_rank, c->local_rank, c->coordinators[0]);

   // Perform the first local barrier

//INFO(1, "LOCAL BARRIER(1)");

   type_byte = handle_to_datatype(EMPI_BYTE);

   if (type_byte == NULL) {
      ERROR(1, "Datatype %d not found!", EMPI_BYTE);
      return MPI_ERR_INTERN;
   }

   error = TRANSLATE_ERROR(PMPI_Barrier(c->comm));

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Local barrier failed! (comm=%d, error=%d)", c->handle, error);
      return EMPI_ERR_INTERN;
   }

   // Perform the WA barrier (coordinators only)
   if (c->global_rank == c->coordinators[0]) {

//INFO(1, "EMPI_Barrier WA BARRIER", "I am coord[0]");

      // Coordinator 0 first receives from all others....
      for (i=1;i<c->cluster_count;i++) {

//INFO(1, "EMPI_Barrier WA BARRIER", "Receiving from coord[i]=%d", c->coordinators[i]);

         error = messaging_receive(&buffer, 1, type_byte, c->coordinators[i], BARRIER_TAG, EMPI_STATUS_IGNORE, c);

         if (error != EMPI_SUCCESS) {
            ERROR(1, "WA receive failed! (comm=%d, error=%d)", c->handle, error);
            return EMPI_ERR_INTERN;
         }
      }

//INFO(1, "EMPI_Barrier WA BARRIER", "Bcast result from coord[0]");

      // ... then bcasts reply.
      error = messaging_bcast(&buffer, 1, type_byte, c->coordinators[0], c);

      if (error != EMPI_SUCCESS) {
         ERROR(1, "EMPI_Barrier: WA bcast failed! (comm=%d, error=%d)", c->handle, error);
         return EMPI_ERR_INTERN;
      }

   } else {

      for (i=1;i<c->cluster_count;i++) {
         if (c->global_rank == c->coordinators[i]) {

//INFO(1, "EMPI_Barrier WA BARRIER", "I am coord[%d]=%d", i, c->coordinators[i]);

            // All other coordinators first send to coordinator 0...

//INFO(1, "EMPI_Barrier WA BARRIER", "Sending to coord[0]");

            error = messaging_send(&buffer, 1, type_byte, c->coordinators[0], BARRIER_TAG, c);

            if (error != EMPI_SUCCESS) {
               ERROR(1, "EMPI_Barrier: WA send failed! (comm=%d, error=%d)", c->handle, error);
               return EMPI_ERR_INTERN;
            }

///INFO(1, "EMPI_Barrier WA BARRIER", "Receiving BCAST");

            // Then wait for reply.
            error = messaging_bcast_receive(&buffer, 1, type_byte, c->coordinators[0], c);

            if (error != EMPI_SUCCESS) {
               ERROR(1, "EMPI_Barrier: WA bcast receive failed (error=%d)!", error);
               return EMPI_ERR_INTERN;
            }
         }
      }
   }

//INFO(1, "EMPI_Barrier WA BARRIER", "LOCAL BARRIER(2)");

   // Perform the second local barrier
   error = TRANSLATE_ERROR(PMPI_Barrier(c->comm));

   if (error != EMPI_SUCCESS) {
      ERROR(1, "EMPI_Barrier: local barrier failed! (2) (comm=%d, error=%d)", c->handle, error);
      return EMPI_ERR_INTERN;
   }

// INFO(1, "EMPI_Barrier WA BARRIER", "DONE!");

   return EMPI_SUCCESS;
}

#define __EMPI_Bcast
int EMPI_Bcast(void* buffer, int count, EMPI_Datatype type, int root, EMPI_Comm comm)
{
   int error;

   communicator *c = handle_to_communicator(comm);
   datatype *t = handle_to_datatype(type);

   if (comm_is_local(c)) {

DEBUG(1, "Local BCAST");

     // simply perform a bcast in local cluster
     return TRANSLATE_ERROR(PMPI_Bcast(buffer, count, t->type, root, c->comm));
   }

   // We need to perform a WA BCAST.

DEBUG(1, "WA BCAST root=%d grank=%d gsize=%d lrank=%d lsize=%d", root, c->global_rank, c->global_size, c->local_rank, c->local_size);

   // If we are root we first send the data to the server and then bcast locally.
   if (c->global_rank == root) {

DEBUG(1, "WA BCAST I am root");

      error = messaging_bcast(buffer, count, t, root, c);

      if (error != EMPI_SUCCESS) {
         ERROR(1, "Root %d failed to broadcast! (comm=%d, error=%d)", root, c->handle, error);
         return error;
      }

      return TRANSLATE_ERROR(PMPI_Bcast(buffer, count, t->type, c->local_rank, c->comm));
   }

DEBUG(1, "WA BCAST I am NOT root");

   // Check if we are in the same cluster as the root. If so, just receive its bcast.
   if (rank_is_local(c, root)) {

DEBUG(1, "WA BCAST Root is local (grank=%d lrank=%d)", root, get_local_rank(c, root));

      return TRANSLATE_ERROR(PMPI_Bcast(buffer, count, t->type, get_local_rank(c, root), c->comm));
   }

DEBUG(1, "WA BCAST Root is remote (%d)", root);

   // If we are in a different cluster from the root and we are the local coordinator
   // we first receive the WA bcast and then forward this bcast locally
   if (c->global_rank == c->my_coordinator) {

DEBUG(1, "WA BCAST I am coordinator (grank=%d lrank=%d) -- doing receive", c->global_rank, c->my_coordinator);

      error = messaging_bcast_receive(buffer, count, t, root, c);

      if (error != EMPI_SUCCESS) {
         ERROR(1, "Local root failed to receive broadcast! (comm=%d, error=%d)", c->handle, root);
         return error;
      }
   }

DEBUG(1, "WA BCAST Local bcast to finish up (coordinator grank=%d lrank=%d)", c->my_coordinator, get_local_rank(c, c->my_coordinator));

   return TRANSLATE_ERROR(PMPI_Bcast(buffer, count, t->type, get_local_rank(c, c->my_coordinator), c->comm));
}


static int WA_Gatherv(void *sendbuf, int sendcount, datatype* sendtype,
                      void *recvbuf, int *recvcounts, int *displs, datatype *recvtype,
                      int root, communicator *c)
{
   int error, i;
   MPI_Aint extent;

   // We implement a WA gather using simple send and receive primitives. This could be optimized by using
   // Async send and receives. NOTE: optimizing using message combining between clusters is harder than it
   // seems, since only the root knows exactly how much data each process sends...

   if (c->global_rank == root) {

      error = TRANSLATE_ERROR(PMPI_Type_extent(recvtype->type, &extent));

      if (error != EMPI_SUCCESS) {
         ERROR(1, "Failed to retrieve data size! (comm=%d, error=%d)", c->handle, error);
         return error;
      }

      for (i=0;i<c->global_size;i++) {
         if (i == root) {
            memcpy(recvbuf + (displs[i] * extent), sendbuf, (recvcounts[i]*extent));
         } else {
            error = do_recv(recvbuf + (displs[i] * extent), recvcounts[i], recvtype, i, GATHERV_TAG, EMPI_STATUS_IGNORE, c);

            if (error != EMPI_SUCCESS) {
               ERROR(1, "Failed to receive data from %d for gather! (comm=%d, error=%d)", i, c->handle, error);
               return error;
            }
         }
      }

   } else {
      error = do_send(sendbuf, sendcount, sendtype, root, GATHERV_TAG, c);

      if (error != EMPI_SUCCESS) {
         ERROR(1, "Failed to send data from %d to root for gatherv! (comm=%d, error=%)", c->global_rank, c->handle, error);
         return error;
      }
   }

   return EMPI_SUCCESS;
}


#define __EMPI_Gatherv
int EMPI_Gatherv(void *sendbuf, int sendcount, EMPI_Datatype sendtype,
                 void *recvbuf, int *recvcounts, int *displs, EMPI_Datatype recvtype,
                 int root, EMPI_Comm comm)
{
   communicator *c = handle_to_communicator(comm);
   datatype *stype = handle_to_datatype(sendtype);
   datatype *rtype = handle_to_datatype(recvtype);

   CHECK_COUNT(sendcount);

   if (comm_is_local(c)) {
     // simply perform a gatherv in local cluster
     return TRANSLATE_ERROR(PMPI_Gatherv(sendbuf, sendcount, stype->type, recvbuf, recvcounts, displs, rtype->type, root, c->comm));
   }

   return WA_Gatherv(sendbuf, sendcount, stype, recvbuf, recvcounts, displs, rtype, root, c);
}

#define __EMPI_Gather
int EMPI_Gather(void* sendbuf, int sendcount, EMPI_Datatype sendtype,
                void* recvbuf, int recvcount, EMPI_Datatype recvtype,
                int root, EMPI_Comm comm)
{
   int i, error;
   int *displs = NULL;
   int *counts = NULL;

   communicator *c = handle_to_communicator(comm);
   datatype *stype = handle_to_datatype(sendtype);
   datatype *rtype = handle_to_datatype(recvtype);

   if (comm_is_local(c)) {
     // simply perform a gatherv in local cluster
     return TRANSLATE_ERROR(PMPI_Gather(sendbuf, sendcount, stype->type, recvbuf, recvcount, rtype->type, root, c->comm));
   }

   // We implement a WA Gather using Gatherv
   // NOTE: the receive related parameters are only valid on root!

   if (c->global_rank == root) {

      displs = malloc(c->global_size * sizeof(int));

      if (displs == NULL) {
         ERROR(0, "Failed to allocate local buffer! (comm=%d)", c->handle);
         return EMPI_ERR_INTERN;
      }

      counts = malloc(c->global_size * sizeof(int));

      if (counts == NULL) {
         ERROR(0, "Failed to allocate local buffer! (comm=%d)", c->handle);
         free(displs);
         return EMPI_ERR_INTERN;
      }

      for (i=0;i<c->global_size;i++) {
         displs[i] = i*recvcount;
         counts[i] = recvcount;
      }
   }

   error = WA_Gatherv(sendbuf, sendcount, stype, recvbuf, counts, displs, rtype, root, c);

   if (c->global_rank == root) {
      free(displs);
      free(counts);
   }

   return error;
}

static int get_count_sums(communicator *c, int *recvcounts, int *sums, int *offsets)
{
   int i, sum = 0;

   for (i=0;i<c->cluster_count;i++) {
      sums[i] = 0;
   }

   for (i=0;i<c->global_size;i++) {
      sums[c->member_cluster_index[i]] += recvcounts[i];
      sum += recvcounts[i];
   }

   offsets[0] = 0;

   for (i=1;i<c->cluster_count;i++) {
      offsets[i] = offsets[i-1] + sums[i-1];
   }

   return sum;
}

#define __EMPI_Allgatherv
int EMPI_Allgatherv(void *sendbuf, int sendcount, EMPI_Datatype sendtype,
                    void *recvbuf, int *recvcounts,
                    int *displs, EMPI_Datatype recvtype, EMPI_Comm comm)
{
   int tmp, sum, error, i, offset;
   int *sums;
   int *offsets;
   unsigned char *buffer;

   MPI_Aint extent;

   communicator *c = handle_to_communicator(comm);
   datatype *stype = handle_to_datatype(sendtype);
   datatype *rtype = handle_to_datatype(recvtype);

   if (comm_is_local(c)) {
     // simply perform an allgatherv in local cluster
     return TRANSLATE_ERROR(PMPI_Allgatherv(sendbuf, sendcount, stype->type, recvbuf, recvcounts, displs, rtype->type, c->comm));
   }

   // We need to perform a WA Allgatherv.
   error = TRANSLATE_ERROR(PMPI_Type_extent(rtype->type, &extent));

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to retrieve data size! (comm=%d, error=%d)", c->handle, error);
      return error;
   }

   INFO(2, "Performing WA allgatherv");

   // First retrieve the data element size
   // FIXME: use size?

   // Next, get the number of elements sent per cluster, the offsets in the buffer, and the total number of elements.
   sums = malloc(c->cluster_count * sizeof(int));

   if (sums == NULL) {
      ERROR(1, "Failed to allocated space for local sums! (comm=%d)", c->handle);
      return EMPI_ERR_INTERN;
   }

   offsets = malloc(c->cluster_count * sizeof(int));

   if (offsets == NULL) {
      ERROR(1, "Failed to allocated space for local offsets! (comm=%d)", c->handle);
      return EMPI_ERR_INTERN;
   }

   sum = get_count_sums(c, recvcounts, sums, offsets);

INFO(2, "Sum = %d", sum);

for (i=0;i<c->cluster_count;i++) {
   INFO(2, "recvcounts[%d]=%d displs[%d]=%d sums[%d]=%d offsets[%d]=%d ", i, recvcounts[i], i, displs[i], i, sums[i], i, offsets[i]);
}

   // Allocate a buffer large enough to receive all data
   buffer = malloc(sum * extent);

   if (buffer == NULL) {
      ERROR(1, "Failed to allocated space for local buffer! (comm=%d)", c->handle);
      return EMPI_ERR_INTERN;
   }

   if (c->global_rank == c->my_coordinator) {

      // I am the local coordinator!

      // First, receive all local data
      offset = offsets[c->member_cluster_index[c->global_rank]];

      for (i=0;i<c->global_size;i++) {

         if (rank_is_local(c, i)) {

            if (i == c->global_rank) {
INFO(2, "LOCAL COPY: offset=%d extent=%d count=%d", offset, extent, recvcounts[i]);
               memcpy(buffer + (offset*extent), sendbuf, recvcounts[i]*extent);
            } else {
INFO(2, "LOCAL RECEIVE: offset=%d extent=%d count=%d", offset, extent, recvcounts[i]);

               error = TRANSLATE_ERROR(PMPI_Recv(buffer + (offset * extent), recvcounts[i], rtype->type, get_local_rank(c, i), ALLGATHERV_TAG, c->comm, MPI_STATUS_IGNORE));

               if (error != EMPI_SUCCESS) {
                  ERROR(1, "Failed to receive data from %d for gather! (comm=%d, error=%d)", i, c->handle, error);
                  return error;
               }
            }

            offset += recvcounts[i];
         }
      }

INFO(2, "LOCAL RECEIVE: DONE  offset=%d");

      // Next, exchange data with other cluster coordinators using a WA BCAST.
      for (i=0;i<c->cluster_count;i++) {

         if (c->coordinators[i] == c->global_rank) {
            // bcast the local result to all other coordinators
            error = messaging_bcast(buffer + (offsets[i]*extent), sums[i], rtype, c->global_rank, c);

            if (error != EMPI_SUCCESS) {
               ERROR(1, "Failed to send bcast from %d for gather! (comm=%d, error=%d)", c->global_rank, c->handle, error);
               return error;
            }
         } else {
            error = messaging_bcast_receive(buffer + (offsets[i]*extent), sums[i], rtype, c->coordinators[i], c);

            if (error != EMPI_SUCCESS) {
               ERROR(1, "Failed to receive bcast on %d for gather! (comm=%d, error=%d)", c->global_rank, c->handle, error);
               return error;
            }
         }
      }

   } else {

      // I am NOT a coordinator, so just send my data to the local coordinator.

      INFO(2, "LOCAL SEND: count=%d", sendcount);

      error = TRANSLATE_ERROR(PMPI_Send(sendbuf, sendcount, stype->type, get_local_rank(c, c->my_coordinator), ALLGATHERV_TAG, c->comm));

      if (error != EMPI_SUCCESS) {
         ERROR(1, "Failed to send data from %d to local root for gatherv! (comm=%d, error=%d)", c->global_rank, c->handle, error);
         return error;
      }

   }

   // Bcast the resulting data locally
   error = TRANSLATE_ERROR(PMPI_Bcast(buffer, sum, rtype->type, get_local_rank(c, c->my_coordinator), c->comm));

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Local broadcast of result failed! (comm=%d, error=%d)", c->handle, error);
      return error;
   }

   // We have now collected all data in "buffer". This data is grouped by cluster and relative global rank. We now need to copy it to the destination buffer.
   for (i=0;i<c->global_size;i++) {
      tmp = c->member_cluster_index[i];

      memcpy(recvbuf + (displs[i] * extent), buffer + (offsets[tmp] * extent), recvcounts[i] * extent);

      offsets[tmp] += recvcounts[i];
   }

   // Free all temp buffers
   free(offsets);
   free(sums);
   free(buffer);
   return EMPI_SUCCESS;
}

#define __EMPI_Allgather
int EMPI_Allgather(void* sendbuf, int sendcount, EMPI_Datatype sendtype,
                   void* recvbuf, int recvcount, EMPI_Datatype recvtype,
                   EMPI_Comm comm)
{
   int *displs;
   int *recvcounts;
   int i, error;

   communicator *c = handle_to_communicator(comm);
   datatype *stype = handle_to_datatype(sendtype);
   datatype *rtype = handle_to_datatype(recvtype);

   if (comm_is_local(c)) {
     // simply perform an allgather in local cluster
     return TRANSLATE_ERROR(PMPI_Allgather(sendbuf, sendcount, stype->type, recvbuf, recvcount, rtype->type, c->comm));
   }

INFO(1, "sendcount=%d recvcount=%d", sendcount, recvcount);

   // We implement an Allgather on top of an Allgatherv.
   displs = malloc(c->global_size * sizeof(int));

   if (displs == NULL) {
      ERROR(0, "Failed to allocate local buffer! (comm=%d)", c->handle);
      return EMPI_ERR_INTERN;
   }

   recvcounts = malloc(c->global_size * sizeof(int));

   if (recvcounts == NULL) {
      ERROR(0, "Failed to allocate local buffer! (comm=%d)", c->handle);
      free(displs);
      return EMPI_ERR_INTERN;
   }

   for (i=0;i<c->global_size;i++) {
      displs[i] = i*recvcount;
      recvcounts[i] = recvcount;
      INFO(1, "displs[%d]=%d recvcounts[%d]=%d", i, displs[i], i, recvcounts[i]);
   }

//   error = WA_Gatherv(sendbuf, sendcount, stype, recvbuf, recvcounts, displs, rtype, root, c);
   error = EMPI_Allgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);

   free(displs);
   free(recvcounts);

   return error;
}


static int WA_Scatterv(void *sendbuf, int *sendcounts, int *displs, datatype* sendtype,
                       void *recvbuf, int recvcount, datatype *recvtype, int root, communicator *c)
{
   int i, error;
   MPI_Aint extent;

   // FIXME: can be optimized by message combining and async sends!

   // We implement a WA Scatterv using simple send/receive primitives
   if (c->global_rank == root) {

      // First retrieve the data element size
      // FIXME: use size?
      error = TRANSLATE_ERROR(PMPI_Type_extent(sendtype->type, &extent));

      if (error != EMPI_SUCCESS) {
         ERROR(1, "Failed to retrieve data size! (comm=%d, error=%d)", c->handle, error);
         return error;
      }

      for (i=0;i<c->global_size;i++) {

         if (i == c->global_rank) {
//INFO(1, "LOCAL COPY");
            memcpy(recvbuf, sendbuf + (displs[i]*extent), recvcount*extent);
         } else {
//INFO(1, "DO send to %d", i);
            error = do_send(sendbuf + (displs[i]*extent), sendcounts[i], sendtype, i, SCATTERV_TAG, c);
//INFO(1, "DONE send to %d", i);

            if (error != EMPI_SUCCESS) {
               ERROR(1, "WA_Scatterv: Root %d (in cluster %d) failed to send data to %d (in cluster %d) (in communicator %d)!\n", root, get_cluster_rank(c, root), i, get_cluster_rank(c, i), c->handle);
               return error;
            }
         }
      }

   } else {

//INFO(1, "DO receive from %d", root);

      error = do_recv(recvbuf, recvcount, recvtype, root, SCATTERV_TAG, EMPI_STATUS_IGNORE, c);

//INFO(1, "DONE receive from %d", root);

      if (error != EMPI_SUCCESS) {
         ERROR(1, "WA_Scatterv: Process %d (in cluster %d) failed to receive data from root %d (in cluster %d) (in communicator %d)!\n", c->global_rank, cluster_rank, root, get_cluster_rank(c, root), c->handle);
         return error;
      }
   }

   return EMPI_SUCCESS;
}

#define __EMPI_Scatter
int EMPI_Scatter(void* sendbuf, int sendcount, EMPI_Datatype sendtype,
                 void* recvbuf, int recvcount, EMPI_Datatype recvtype,
                 int root, EMPI_Comm comm)
{
   int i, error;
   int *displs = NULL;
   int *sendcounts = NULL;

   communicator *c = handle_to_communicator(comm);
   datatype *stype = handle_to_datatype(sendtype);
   datatype *rtype = handle_to_datatype(recvtype);

   if (comm_is_local(c)) {

//INFO(1, "local scatter");

     // simply perform a scatter in local cluster
     return TRANSLATE_ERROR(PMPI_Scatter(sendbuf, sendcount, stype->type, recvbuf, recvcount, rtype->type, root, c->comm));
   }

//INFO(1, "WA scatter");

   // We implement a WA Scatter using the WA Scatterv
   if (c->global_rank == root) {

      displs = malloc(c->global_size * sizeof(int));

      if (displs == NULL) {
         ERROR(1, "Failed to allocate buffer! (comm=%d)", c->handle);
         return EMPI_ERR_INTERN;
      }

      sendcounts = malloc(c->global_size * sizeof(int));

      if (sendcounts == NULL) {
         ERROR(1, "Failed to allocate buffer! (comm=%d)", c->handle);
         free(displs);
         return EMPI_ERR_INTERN;
      }

      for (i=0;i<c->global_size;i++) {
         sendcounts[i] = sendcount;
         displs[i] = sendcount * i;
      }
   }

   error = WA_Scatterv(sendbuf, sendcounts, displs, stype, recvbuf, recvcount, rtype, root, c);

   if (c->global_rank == root) {
      free(displs);
      free(sendcounts);
   }

   return error;
}

#define __EMPI_Scatterv
int EMPI_Scatterv(void* sendbuf, int *sendcounts, int *displs, EMPI_Datatype sendtype,
                  void* recvbuf, int recvcount, EMPI_Datatype recvtype,
                  int root, EMPI_Comm comm)
{
   communicator *c;
   datatype *stype;
   datatype *rtype;

   c = handle_to_communicator(comm);

   if (c == NULL) {
      ERROR(1, "Communicator %d not found!", comm);
      return MPI_ERR_COMM;
   }

   stype = handle_to_datatype(sendtype);

   if (stype == NULL) {
      ERROR(1, "Datatype %d not found!", sendtype);
      return MPI_ERR_TYPE;
   }

   rtype = handle_to_datatype(recvtype);

   if (rtype == NULL) {
      ERROR(1, "Datatype %d not found!", recvtype);
      return MPI_ERR_TYPE;
   }

   if (comm_is_local(c)) {
     // simply perform a scatterv in local cluster
     return TRANSLATE_ERROR(PMPI_Scatterv(sendbuf, sendcounts, displs, stype->type, recvbuf, recvcount, rtype->type, root, c->comm));
   }

   return WA_Scatterv(sendbuf, sendcounts, displs, stype, recvbuf, recvcount, rtype, root, c);
}

#define __EMPI_Reduce
int EMPI_Reduce(void* sendbuf, void* recvbuf, int count, EMPI_Datatype type, EMPI_Op op, int root, EMPI_Comm comm)
{
   int local_root, root_in_cluster, i, error;
   unsigned char *buffer = NULL;
   MPI_Aint extent;
   communicator *c;
   datatype *t;
   operation *o;

   c = handle_to_communicator(comm);

   if (c == NULL) {
      ERROR(1, "Communicator %d not found!", comm);
      return EMPI_ERR_COMM;
   }

   t = handle_to_datatype(type);

   if (t == NULL) {
      ERROR(1, "Datatype %d not found!", type);
      return EMPI_ERR_TYPE;
   }

   o = handle_to_operation(op);

   if (o == NULL) {
      ERROR(1, "Operation %d not found!", op);
      return EMPI_ERR_OP;
   }

   if (comm_is_local(c)) {

INFO(2, "Local reduce");
     // simply perform a reduce in local cluster
     return TRANSLATE_ERROR(PMPI_Reduce(sendbuf, recvbuf, count, t->type, o->op, root, c->comm));
   }

INFO(2, "WA reduce");

   // We need to perform a WA Reduce. We do this by performing a reduce
   // to our local cluster coordinator. This result is then forwarded to the
   // root, which merges all partial results locally.

   if (rank_is_local(c, root)) {
      local_root = root;
      root_in_cluster = 1;
   } else {
      local_root = c->my_coordinator;
      root_in_cluster = 0;
   }

INFO(2, "Local root is %d (root=%d)", local_root, root);

   if (c->global_rank == root || c->global_rank == c->my_coordinator) {

INFO(2, "I am root or coordinator (rank=%d root=%d coordinator=%d)", c->global_rank, root, c->my_coordinator);

// FIXME: use size own type info ???

      error = TRANSLATE_ERROR(PMPI_Type_extent(t->type, &extent));

      if (error != EMPI_SUCCESS) {
         ERROR(1, "Failed to retrieve data size for reduce! (comm=%d, error=%d)", c->handle, error);
         return error;
      }

      buffer = malloc(count * extent);

      if (buffer == NULL) {
         ERROR(1, " Failed to allocate buffer space for WA reduce! (comm=%d, error=%d)", c->handle, error);
         return EMPI_ERR_INTERN;
      }
   } else {
INFO(2, "I am NOT root NOR coordinator (rank=%d root=%d coordinator=%d)", c->global_rank, root, c->my_coordinator);
   }

//INFO(1, "JASON EMPI_REDUCE", "START LOCAL REDUCE root=%d lroot=%d grank=%d lrank=%d count=%d sbuf[0]=%d rbuf[0]=%d\n",
//                       root, local_root, c->global_rank, c->local_rank, count, ((int *)sendbuf)[0], ((int *)recvbuf)[0]);


INFO(2, "Starting local reduce sendbuf=%p buffer=%p count=%d root=%d get_local_rank(root)=%d",
                  sendbuf, buffer, count, local_root, get_local_rank(c, local_root));

   error = TRANSLATE_ERROR(PMPI_Reduce(sendbuf, buffer, count, t->type, o->op, get_local_rank(c, local_root), c->comm));

INFO(2, "Local reduce OK");

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to perform local reduce in communicator! (comm=%d, error=%d)", c->handle, error);
      return error;
   }

   if (c->global_rank == root) {
      // The global root now receive the partial result from each cluster
      // coordinator (except from his own cluster).

      // Copy local result to target buffer
      memcpy(recvbuf, buffer, count * extent);

      // Receive partial results from remote coordinators
      for (i=0;i<c->cluster_count;i++) {
         if (c->global_rank != c->coordinators[i] && !(rank_is_local(c, c->coordinators[i]))) {

INFO(2, "Receiving REMOTE result from coordinator[%d]=%d", i, c->coordinators[i]);

            error = messaging_receive(buffer, count, t, c->coordinators[i], REDUCE_TAG, EMPI_STATUS_IGNORE, c);

            if (error != EMPI_SUCCESS) {
               ERROR(1, "Root %d failed to receive local reduce result from coordinator %d! (comm=%d, error=%d)",
		        c->global_rank, c->coordinators[i], c->handle, error);
               return error;
            }

            // FIXME: no error checking here ??
            (*(o->function))((void*)buffer, recvbuf, &count, &type);
         }
      }
   } else if (root_in_cluster == 0 && c->global_rank == c->my_coordinator) {
      // The local coordinator now sends the partial result to the global root.

INFO(2, "Sending LOCAL result to REMOTE root=%d", root);

      error = messaging_send(buffer, count, t, root, REDUCE_TAG, c);

      if (error != EMPI_SUCCESS) {
         ERROR(1, "Local coordinator %d failed to send local reduce result to root! (comm=%d, error=%d)",
		c->global_rank, c->handle, error);
         return error;
      }
   }

INFO(2, "MPI_Reduce DONE");

   return EMPI_SUCCESS;
}

#define __EMPI_Accumulate
int EMPI_Accumulate (void *origin_addr, int origin_count, EMPI_Datatype origin_datatype,
                     int target_rank, MPI_Aint target_disp, int target_count, EMPI_Datatype target_datatype,
                     EMPI_Op op, EMPI_Win win)
{
   ERROR(1, "Operation not implemented: EMPI_Accumulate");
   return EMPI_ERR_INTERN;
/*
   operation *o = get_operation(op);

   if (o == NULL) {
      ERROR(1, "Operation not found!");
      return MPI_ERR_OP;
   }

   return PMPI_Accumulate(origin_addr, origin_count, origin_datatype,
                          target_rank, target_disp, target_count, target_datatype,
                          o->op, win);
*/
}

#define __EMPI_Allreduce
int EMPI_Allreduce(void* sendbuf, void* recvbuf, int count,
                         EMPI_Datatype type, EMPI_Op op, EMPI_Comm comm)
{
   // FIXME: Assumes operation is commutative!
   int error, i;
   MPI_Aint extent;
   char *buffer;
   communicator *c;
   datatype *t;
   operation *o;

   c = handle_to_communicator(comm);

   if (c == NULL) {
      ERROR(1, "Communicator %d not found!", comm);
      return EMPI_ERR_COMM;
   }

   t = handle_to_datatype(type);

   if (t == NULL) {
      ERROR(1, "Datatype %d not found!", type);
      return EMPI_ERR_TYPE;
   }

   o = handle_to_operation(op);

   if (o == NULL) {
      ERROR(1, "Operation %d not found!", op);
      return EMPI_ERR_OP;
   }

   if (comm_is_local(c)) {
     // simply perform an allreduce in local cluster
     return TRANSLATE_ERROR(PMPI_Allreduce(sendbuf, recvbuf, count, t->type, o->op, c->comm));
   }

   // We need to perform a WA Allreduce. We do this by performing a reduce
   // to our local cluster coordinator. This result is then broadcast to the
   // other cluster coordinators, which merge the results locally. The result
   // of this local merge is then broadcast in each local cluster.
   // NOTE: this does assume the operation is commutative!

//   INFO(1, "START LOCAL REDUCE grank=%d lrank=%d count=%d sbuf[1]=%d rbuf[1]=%d\n",
//                       c->global_rank, c->local_rank, count, ((int *)sendbuf)[1], ((int *)recvbuf)[1]);

   error = TRANSLATE_ERROR(PMPI_Reduce(sendbuf, recvbuf, count, t->type, o->op, get_local_rank(c, c->my_coordinator), c->comm));

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to perform local allreduce! (comm=%d, error=%d)", c->handle, error);
      return error;
   }

//   INFO(1, "RESULT LOCAL REDUCE grank=%d lrank=%d count=%d sbuf[1]=%d rbuf[1]=%d\n",
//                        c->global_rank, c->local_rank, count, ((int *)sendbuf)[1], ((int *)recvbuf)[1]);

   // The local cluster coordinator shares the result with all other cluster coordinators.

   if (c->global_rank == c->my_coordinator) {

//  INFO(1, "JASON ALLREDUCE WA", "LOCAL ROOT!\n");

      // FIXME: use size?
      error = TRANSLATE_ERROR(PMPI_Type_extent(t->type, &extent));

      if (error != EMPI_SUCCESS) {
         ERROR(1, "Failed to retrieve data size for allreduce! (comm=%d, error=%d)", c->handle, error);
         return error;
      }

      buffer = malloc(count * extent);

      if (buffer == NULL) {
         ERROR(1, "Failed to allocate buffer space for WA Allreduce! (comm=%d, error=%d)", c->handle, error);
         return EMPI_ERR_INTERN;
      }

//  INFO(1, "JASON ALLREDUCE WA", "FIXME: WA BCAST with CRAP performance!!\n");

      // FIXME: If this BCAST blocks then we're dead!
//      INFO(1, "WA BAST SEND grank=%d lrank=%d count=%d buf[1]=%d\n", c->global_rank, c->local_rank, count, ((int*)recvbuf)[1]);
      error = messaging_bcast(recvbuf, count, t, c->global_rank, c);

      if (error != EMPI_SUCCESS) {
         ERROR(1, "Local root %d failed to bcast local allreduce result! (comm=%d, error=%d)", c->global_rank, c->handle, error);
         return error;
      }

      for (i=0;i<c->cluster_count;i++) {

         if (c->coordinators[i] != c->global_rank) {

//        INFO(1, "WA BAST RECV i=%d grank=%d lrank=%d count=%d from=%d\n", i, c->global_rank, c->local_rank, count, c->coordinators[i]);

            error = messaging_bcast_receive(buffer, count, t, c->coordinators[i], c);

            if (error != EMPI_SUCCESS) {
               ERROR(1, "Local root %d failed to bcast local allreduce result! (comm=%d, error=%d)", c->global_rank, c->handle, error);
               return error;
            }

//        INFO(1, "CALLING REDUCE OP buf[1]=%d revcbuf[1]=%d count=%d\n", ((int *)buffer)[1], ((int *)recvbuf)[1], count);

            (*(o->function))((void*)buffer, recvbuf, &count, &type);

//        INFO(1, "RESULT REDUCE OP buf[1]=%d revcbuf[1]=%d count=%d\n", ((int *)buffer)[1], ((int *)recvbuf)[1], count);
         }
      }
   }

//   INFO(1, "LOCAL BAST grank=%d lrank=%d count=%d buf[1]=%d\n", c->global_rank, c->local_rank, count, ((int*)recvbuf)[1]);

   return TRANSLATE_ERROR(PMPI_Bcast(recvbuf, count, t->type, 0, c->comm));
}

#define __EMPI_Scan
int EMPI_Scan(void* sendbuf, void* recvbuf, int count,
              EMPI_Datatype type, EMPI_Op op, EMPI_Comm comm)
{
   int i, tmp_cluster, error;
   MPI_Aint extent;
   unsigned char *buffer;
   communicator *c;
   datatype *t;
   operation *o;

   c = handle_to_communicator(comm);

   if (c == NULL) {
      ERROR(1, "Communicator %d not found!", comm);
      return EMPI_ERR_COMM;
   }

   t = handle_to_datatype(type);

   if (t == NULL) {
      ERROR(1, "Datatype %d not found!", type);
      return EMPI_ERR_TYPE;
   }

   o = handle_to_operation(op);

   if (o == NULL) {
      ERROR(1, "Operation %d not found!", op);
      return EMPI_ERR_OP;
   }

   if (comm_is_local(c)) {
     // simply perform a scan in local cluster
     return TRANSLATE_ERROR(PMPI_Scan(sendbuf, recvbuf, count, t->type, o->op, c->comm));
   }

   // We implement a WA Scan using simple send/receive primitives

   // First retrieve the data element size
   // FIXME: use size?
   error = TRANSLATE_ERROR(PMPI_Type_extent(t->type, &extent));

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to retrieve send data size! (comm=%d, error=%d)", c->handle, error);
      return error;
   }

   // Allocate a temp buffer.
   buffer = malloc(count * extent);

   if (buffer == NULL) {
      ERROR(1, "Failed to allocate temporary buffer! (comm=%d, error=%d)", c->handle, error);
      return error;
   }

   // Step 1: copy send buffer to receive buffer.
   memcpy(recvbuf, sendbuf, count * extent);

   // Step 2: loop over all ranks, and send/(receive+reduce) data where needed.
   for (i=0;i<c->global_size;i++) {

      tmp_cluster = GET_CLUSTER_RANK(c->members[i]);

      if (i > c->global_rank) {
         error = do_send(sendbuf, count, t, i, SCAN_TAG, c);

         if (error != EMPI_SUCCESS) {
            ERROR(1, "Rank %d (in cluster %d) failed to send data to %d (in cluster %d)! (comm=%d, error=%d)", c->global_rank, cluster_rank, i, tmp_cluster, c->handle, error);
            return error;
         }

      } else if (i < c->global_rank) {

         // Must receive from i.
         error = do_recv(buffer, count, t, i, SCAN_TAG, EMPI_STATUS_IGNORE, c);

         if (error != EMPI_SUCCESS) {
            ERROR(1, "Rank %d (in cluster %d) failed to receive data from %d (in cluster %d)! (comm=%d, error=%d)", c->global_rank, cluster_rank, i, tmp_cluster, c->handle, error);
            return error;
         }

         // FIXME: no error handling ?
         (*(o->function))((void*)buffer, recvbuf, &count, &type);
      }
   }

   free(buffer);
   return EMPI_SUCCESS;
}

static int WA_Alltoallw(void *sendbuf, int *sendcounts, int *sdispls, datatype **sendtype,
                        void *recvbuf, int *recvcounts, int *rdispls, datatype **recvtype,
                        communicator *c)
{
   ERROR(1, "INTERNAL ERROR: WA_Alltoallw not implemented yet!");
   return EMPI_ERR_INTERN;
}

static int WA_Alltoallv(void *sendbuf, int *sendcounts, int *sdispls, datatype *sendtype,
                        void *recvbuf, int *recvcounts, int *rdispls, datatype *recvtype,
                        communicator *c)
{
   int i, j, error;
   MPI_Aint sextent, rextent;

   // We implement a WA Alltoallv using simple send/receive primitives

   // First retrieve the data element size
   // FIXME: use size?
   error = TRANSLATE_ERROR(PMPI_Type_extent(sendtype->type, &sextent));

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to retrieve send data size! (comm=%d, error=%d)", c->handle, error);
      return error;
   }

   // First retrieve the data element size
   // FIXME: use size?
   error = TRANSLATE_ERROR(PMPI_Type_extent(recvtype->type, &rextent));

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to retrieve receive data size! (comm=%d, error=%d)", c->handle, error);
      return error;
   }

   for (i=0;i<c->global_size;i++) {

      if (i == c->global_rank) {
         // We should receive from all others (including self)

INFO(1, "I am master %d", i);

         for (j=0;j<c->global_size;j++) {

            if (j == c->global_rank) {

INFO(1, "local copy index=%d displ=%d, count=%d", j, rdispls[j], recvcounts[j]);

               // receive from self is a simple memcopy.
               memcpy(recvbuf + (rextent * rdispls[j]), sendbuf + (sextent * sdispls[j]), recvcounts[i]*rextent);

            } else {

INFO(1, "receive index=%d displ=%d, count=%d", j, rdispls[j], recvcounts[j]);

               // receive from others.
               error = do_recv(recvbuf + (rextent * rdispls[j]), recvcounts[j], recvtype, j, ALLTOALLV_TAG, EMPI_STATUS_IGNORE, c);

               if (error != EMPI_SUCCESS) {
                  ERROR(1, "Rank %d (in cluster %d) failed to receive data from %d (in cluster %d)! (comm=%d, error=%d)", c->global_rank, cluster_rank, j, get_cluster_rank(c, j), c->handle, error);
                  return error;
               }
            }
         }
      } else {
         // We should send to one other.

INFO(1, "send index=%d displ=%d, count=%d", i, sdispls[i], sendcounts[i]);

         error = do_send(sendbuf + (sdispls[i] * sextent), sendcounts[i], sendtype, i, ALLTOALLV_TAG, c);

         if (error != EMPI_SUCCESS) {
            ERROR(1, "Rank %d (in cluster %d) failed to send data to %d (in cluster %d) (comm=%d, error=%d)", c->global_rank, cluster_rank, i, get_cluster_rank(c, i), c->handle, error);
            return error;
         }
      }
   }

   return EMPI_SUCCESS;
}

#define __EMPI_Alltoall
int EMPI_Alltoall(void *sendbuf, int sendcount, EMPI_Datatype sendtype,
                  void *recvbuf, int recvcount, EMPI_Datatype recvtype,
                  EMPI_Comm comm)
{
   int i, error;
   int *senddispls;
   int *sendcounts;
   int *recvdispls;
   int *recvcounts;
   communicator *c;
   datatype *stype;
   datatype *rtype;

   c = handle_to_communicator(comm);

   if (c == NULL) {
      ERROR(1, "Communicator %d not found!", comm);
      return EMPI_ERR_COMM;
   }

   stype = handle_to_datatype(sendtype);

   if (stype == NULL) {
      ERROR(1, "Datatype %d not found!", sendtype);
      return EMPI_ERR_TYPE;
   }

   rtype = handle_to_datatype(recvtype);

   if (rtype == NULL) {
      ERROR(1, "Datatype %d not found!", recvtype);
      return EMPI_ERR_TYPE;
   }

   if (comm_is_local(c)) {
     // simply perform an all-to-all in local cluster
     return TRANSLATE_ERROR(PMPI_Alltoall(sendbuf, sendcount, stype->type, recvbuf, recvcount, rtype->type, c->comm));
   }

   // We implement a WA Alltoall using send/receive
   senddispls = malloc(c->global_size * sizeof(int));

   if (senddispls == NULL) {
      ERROR(1, "Failed to allocate buffer! (comm=%d)", c->handle);
      return EMPI_ERR_INTERN;
   }

   sendcounts = malloc(c->global_size * sizeof(int));

   if (sendcounts == NULL) {
      ERROR(1, "Failed to allocate buffer! (comm=%d)", c->handle);
      free(senddispls);
      return EMPI_ERR_INTERN;
   }

   recvdispls = malloc(c->global_size * sizeof(int));

   if (recvdispls == NULL) {
      ERROR(1, "Failed to allocate buffer! (comm=%d)", c->handle);
      free(senddispls);
      free(sendcounts);
      return EMPI_ERR_INTERN;
   }

   recvcounts = malloc(c->global_size * sizeof(int));

   if (recvcounts == NULL) {
      ERROR(1, "Failed to allocate buffer! (comm=%d)", c->handle);
      free(senddispls);
      free(sendcounts);
      free(recvdispls);
      return EMPI_ERR_INTERN;
   }

   for (i=0;i<c->global_size;i++) {
      sendcounts[i] = sendcount;
      senddispls[i] = sendcount * i;
      recvcounts[i] = recvcount;
      recvdispls[i] = recvcount * i;
   }

   error = WA_Alltoallv(sendbuf, sendcounts, senddispls, stype,
                       recvbuf, recvcounts, recvdispls, rtype, c);

   free(senddispls);
   free(sendcounts);
   free(recvdispls);
   free(recvcounts);

   return error;
}

#define __EMPI_Alltoallv
int EMPI_Alltoallv(void *sendbuf, int *sendcounts, int *sdispls, EMPI_Datatype sendtype,
                    void *recvbuf, int *recvcounts, int *rdispls, EMPI_Datatype recvtype,
                    EMPI_Comm comm)
{
   communicator *c;
   datatype *stype;
   datatype *rtype;

   c = handle_to_communicator(comm);

   if (c == NULL) {
      ERROR(1, "Communicator %d not found!", comm);
      return EMPI_ERR_COMM;
   }

   stype = handle_to_datatype(sendtype);

   if (stype == NULL) {
      ERROR(1, "Datatype %d not found!", sendtype);
      return EMPI_ERR_TYPE;
   }

   rtype = handle_to_datatype(recvtype);

   if (rtype == NULL) {
      ERROR(1, "Datatype %d not found!", recvtype);
      return EMPI_ERR_TYPE;
   }

   if (comm_is_local(c)) {
     // simply perform an all-to-all in local cluster
     return TRANSLATE_ERROR(PMPI_Alltoallv(sendbuf, sendcounts, sdispls, stype->type, recvbuf, recvcounts, rdispls, rtype->type, c->comm));
   }

   return WA_Alltoallv(sendbuf, sendcounts, sdispls, stype, recvbuf, recvcounts, rdispls, rtype, c);
}


#define __EMPI_Alltoallw
int EMPI_Alltoallw(void *sendbuf, int *sendcounts, int *sdispls, EMPI_Datatype *sendtypes,
                    void *recvbuf, int *recvcounts, int *rdispls, EMPI_Datatype *recvtypes,
                    EMPI_Comm comm)
{
   int i, error;
   communicator *c;

   datatype *type;

   datatype **empi_send_types;
   datatype **empi_recv_types;

   MPI_Datatype *mpi_send_types;
   MPI_Datatype *mpi_recv_types;

   c = handle_to_communicator(comm);

   if (c == NULL) {
      ERROR(1, "Communicator %d not found!", comm);
      return EMPI_ERR_COMM;
   }

   if (comm_is_local(c)) {
      // simply perform an all-to-all in local cluster
      mpi_send_types = malloc(c->global_size * sizeof(MPI_Comm));

      if (mpi_send_types == NULL) {
         ERROR(1, "Failed to allocate space for MPI types!");
         return EMPI_ERR_INTERN;
      }

      mpi_recv_types = malloc(c->global_size * sizeof(MPI_Comm));

      if (mpi_recv_types == NULL) {
         ERROR(1, "Failed to allocate space for MPI types!");
         free(mpi_send_types);
         return EMPI_ERR_INTERN;
      }

      for (i=0;i<c->global_size;i++) {

         type = handle_to_datatype(sendtypes[i]);

         if (type == NULL) {
            ERROR(1, "Datatype %d not found!", sendtypes[i]);
            free(mpi_send_types);
            free(mpi_recv_types);
            return EMPI_ERR_TYPE;
         }

         mpi_send_types[i] = type->type;
      }

      for (i=0;i<c->global_size;i++) {

         type = handle_to_datatype(recvtypes[i]);

         if (type == NULL) {
            ERROR(1, "Datatype %d not found!", recvtypes[i]);
            free(mpi_send_types);
            free(mpi_recv_types);
            return EMPI_ERR_TYPE;
         }

         mpi_recv_types[i] = type->type;
      }

      error = TRANSLATE_ERROR(PMPI_Alltoallw(sendbuf, sendcounts, sdispls, mpi_send_types, recvbuf, recvcounts, rdispls, mpi_recv_types, c->comm));

      free(mpi_send_types);
      free(mpi_recv_types);

   } else {

      // simply perform an all-to-all in local cluster
      empi_send_types = malloc(c->global_size * sizeof(datatype *));

      if (empi_send_types == NULL) {
         ERROR(1, "Failed to allocate space for EMPI types!");
         return EMPI_ERR_INTERN;
      }

      empi_recv_types = malloc(c->global_size * sizeof(datatype *));

      if (empi_recv_types == NULL) {
         ERROR(1, "Failed to allocate space for EMPI types!");
         free(empi_send_types);
         return EMPI_ERR_INTERN;
      }

      for (i=0;i<c->global_size;i++) {

         empi_send_types[i] = handle_to_datatype(sendtypes[i]);

         if (empi_send_types[i] == NULL) {
            ERROR(1, "Datatype %d not found!", sendtypes[i]);
            free(empi_send_types);
            free(empi_recv_types);
            return EMPI_ERR_TYPE;
         }
      }

      for (i=0;i<c->global_size;i++) {

         empi_recv_types[i] = handle_to_datatype(recvtypes[i]);

         if (empi_recv_types[i] == NULL) {
            ERROR(1, "Datatype %d not found!", recvtypes[i]);
            free(empi_send_types);
            free(empi_recv_types);
            return EMPI_ERR_TYPE;
         }
      }


      error = WA_Alltoallw(sendbuf, sendcounts, sdispls, empi_send_types, recvbuf, recvcounts, rdispls, empi_recv_types, c);

      free(empi_send_types);
      free(empi_recv_types);
   }

   return error;
}

/*****************************************************************************/
/*                         Communicators and Groups                          */
/*****************************************************************************/



#define __EMPI_Comm_size
int EMPI_Comm_size(EMPI_Comm comm, int *size)
{
   communicator *c = handle_to_communicator(comm);

   if (c == NULL) {
      ERROR(1, "Communicator %d not found!", comm);
      return EMPI_ERR_COMM;
   }

//   INFO(1, "MPI_Comm_size", "Retrieve size from %d: local(%d %d) | global(%d %d)", c->handle, c->local_rank, c->local_size, c->global_rank, c->global_size);

   *size = c->global_size;

   return EMPI_SUCCESS;
}

#define __EMPI_Comm_rank
int EMPI_Comm_rank(EMPI_Comm comm, int *rank)
{
   communicator *c = handle_to_communicator(comm);

   if (c == NULL) {
      ERROR(1, "Communicator %d not found!", comm);
      return EMPI_ERR_COMM;
   }

//   INFO(1, "MPI_Comm_rank", "Retrieve rank from %d: local(%d %d) | global(%d %d)", c->handle, c->local_rank, c->local_size, c->global_rank, c->global_size);

   *rank = c->global_rank;

   return EMPI_SUCCESS;
}

static int *copy_int_array(int *src, int size)
{
   int *tmp = malloc(size * sizeof(int));

   if (tmp == NULL) {
      return NULL;
   }

   return memcpy(tmp, src, size * sizeof(int));
}

#define __EMPI_Comm_dup
int EMPI_Comm_dup(EMPI_Comm comm, EMPI_Comm *newcomm)
{
   int error;
   dup_reply reply;
   MPI_Comm tmp_com;
   uint32_t *members;
   int *coordinators;
   int *cluster_sizes;
   int *cluster_ranks;
   uint32_t *member_cluster_index;
   uint32_t *local_ranks;

   communicator *dup;

   communicator *c = handle_to_communicator(comm);

   if (c == NULL) {
      ERROR(1, "Communicator %d not found!", comm);
      return EMPI_ERR_COMM;
   }

   error = messaging_send_dup_request(c);

   if (error != EMPI_SUCCESS) {
      IERROR(1, "MPI_Comm_dup send failed! (comm=%d, error=%d)", c->handle, error);
      return error;
   }

   error = messaging_receive_dup_reply(&reply);

   if (error != EMPI_SUCCESS) {
      IERROR(1, "MPI_Comm_dup receive failed! (comm=%d, error=%d)", c->handle, error);
      return error;
   }

   error = TRANSLATE_ERROR(PMPI_Comm_dup(c->comm, &tmp_com));

   if (error != EMPI_SUCCESS) {
      IERROR(1, "MPI_Comm_dup local dup failed! (comm=%d, error=%d)", c->handle, error);
      return error;
   }

   members = (uint32_t *) copy_int_array((int *)c->members, c->global_size);

   if (members == NULL) {
      IERROR(1, "MPI_Comm_dup member copy failed! (comm=%d)", c->handle);
      return error;
   }

   coordinators = copy_int_array(c->coordinators, c->cluster_count);

   if (coordinators == NULL) {
      IERROR(1, "MPI_Comm_dup coordinator copy failed! (comm=%d)", c->handle);
      free(members);
      return error;
   }

   cluster_sizes = copy_int_array(c->cluster_sizes, c->cluster_count);

   if (cluster_sizes == NULL) {
      IERROR(1, "MPI_Comm_dup cluster_sizes copy failed! (comm=%d)", c->handle);
      free(members);
      free(coordinators);
      return error;
   }

   cluster_ranks = copy_int_array(c->cluster_ranks, c->cluster_count);

   if (cluster_ranks == NULL) {
      IERROR(1, "MPI_Comm_dup cluster_ranks copy failed! (comm=%d)", c->handle);
      free(members);
      free(coordinators);
      free(cluster_sizes);
      return error;
   }

   member_cluster_index = (uint32_t *) copy_int_array((int*)c->member_cluster_index, c->global_size);

   if (member_cluster_index == NULL) {
      IERROR(1, "MPI_Comm_dup member_cluster_index copy failed! (comm=%d)", c->handle);
      free(members);
      free(coordinators);
      free(cluster_sizes);
      free(cluster_ranks);
      return error;
   }

   local_ranks = (uint32_t *) copy_int_array((int*)c->local_ranks, c->global_size);

   if (local_ranks == NULL) {
      IERROR(1, "MPI_Comm_dup local_ranks copy failed! (comm=%d)", c->handle);
      free(members);
      free(coordinators);
      free(cluster_sizes);
      free(cluster_ranks);
      free(member_cluster_index);
      return error;
   }

   error = create_communicator(reply.newComm, tmp_com,
                 c->local_rank, c->local_size,
                 c->global_rank, c->global_size,
                 c->cluster_count, coordinators, cluster_sizes,
                 c->flags, members,
                 cluster_ranks, member_cluster_index, local_ranks,
                 &dup);

   if (error != EMPI_SUCCESS) {
      IERROR(1, "MPI_Comm_dup create communicator failed! (comm=%d, error=%d)", c->handle, error);
      return error;
   }

   *newcomm = dup->handle;

   STACKTRACE(0, "COMM_DUP %d -> %d", c->handle, dup->handle);

   return EMPI_SUCCESS;
}

static int local_comm_create(communicator *c, group *g, MPI_Comm *newcomm)
{
   int i, error, local_count;
   MPI_Group orig_group;
   MPI_Group new_group;
   int *local_members;
   communicator *world;

   // We first need to split the local part of the group from our local communicator.

   // Collect the local members.
   local_count = 0;
   local_members = malloc(g->size * sizeof(int));

   if (local_members == NULL) {
      IERROR(1, "Failed to allocate memory for local group members! (comm=%d)", c->handle);
      return EMPI_ERR_INTERN;
   }

   for (i=0;i<g->size;i++) {
      if (GET_CLUSTER_RANK(g->members[i]) == cluster_rank) {
// FIXME: is this correct ?????
         local_members[local_count++] = (int) GET_PROCESS_RANK(g->members[i]);
      }
   }

   // If local_count == 0 then we do not need to split the local communicator.
   if (local_count == 0) {
      free(local_members);
      *newcomm = MPI_COMM_NULL;
      return EMPI_SUCCESS;
   }

   // If local_count > 0 we need to create a new (local) communicator.
   // Since we known the local ranks relative to WORLD, we'll use that
   // communicator as a basis.

   world = handle_to_communicator(EMPI_COMM_WORLD);

   error = TRANSLATE_ERROR(PMPI_Comm_group(world->comm, &orig_group));

   if (error != EMPI_SUCCESS) {
      IERROR(1, "Failed to split local group! (comm=%d, error=%d)", c->handle, error);
      free(local_members);
      return error;
   }

   error = TRANSLATE_ERROR(PMPI_Group_incl(orig_group, local_count, local_members, &new_group));

   free(local_members);

   if (error != EMPI_SUCCESS) {
      IERROR(1, "Failed to perform local group include! (comm=%d, error=%d)", c->handle, error);
      return error;
   }

   // Check if we are part of the new communicator.
   if (new_group == MPI_GROUP_NULL) {
      *newcomm = MPI_COMM_NULL;
      return EMPI_SUCCESS;
   }

   // HACK: we use the local communicator to create the new one here ?
   error = TRANSLATE_ERROR(PMPI_Comm_create(c->comm, new_group, newcomm));

   if (error != EMPI_SUCCESS) {
      IERROR(1, "Failed to create local communicator! (comm=%d, error=%d)", c->handle, error);
      return error;
   }

   return EMPI_SUCCESS;
}

#define __EMPI_Comm_create
int EMPI_Comm_create(EMPI_Comm mc, EMPI_Group mg, EMPI_Comm *newcomm)
{
   int error, local_rank, local_size;
   group_reply reply;
   MPI_Comm tmp_comm;
   communicator *result;

   // Retrieve our communicator.
   communicator *c = handle_to_communicator(mc);

   if (c == NULL) {
      ERROR(1, "Communicator %d not found!", mc);
      return EMPI_ERR_COMM;
   }

   // Retrieve our group.
   group *g = handle_to_group(mg);

   if (g == NULL) {
      ERROR(1, "Group %d not found! (comm=%d)", mg, c->handle);
      return EMPI_ERR_GROUP;
   }

   // Request the create at the server
   error = messaging_send_group_request(c, g);

   if (error != EMPI_SUCCESS) {
      IERROR(1, "Failed to send comm_create! (comm=%d, error=%d)", c->handle, error);
      return error;
   }

   // Wait for the reply.
   error = messaging_receive_group_reply(&reply);

   if (error != EMPI_SUCCESS) {
      IERROR(1, "Failed to receive comm_create! (comm=%d, error=%d)", c->handle, error);
      return error;
   }

   // If the type of the new communicator is GROUP_TYPE_IDLE we can simply return MPI_COMM_NULL.
   if (reply.type == GROUP_TYPE_IDLE) {
      *newcomm = EMPI_COMM_NULL;
      return EMPI_SUCCESS;
   }

   // Otherwise, we may have to create a local communicator.
   error = local_comm_create(c, g, &tmp_comm);

   if (error != EMPI_SUCCESS) {
      IERROR(1, "Failed to create local communicator! (comm=%d, error=%d)", c->handle, error);
      return error;
   }

   // Create the communicator if necessary.
   if (tmp_comm != MPI_COMM_NULL) {
      error = TRANSLATE_ERROR(PMPI_Comm_rank(tmp_comm, &local_rank));

      if (error != EMPI_SUCCESS) {
         IERROR(1, "Failed to retrieve rank of local communicator! (comm=%d, error=%d)", c->handle, error);
         return error;
      }

      error = TRANSLATE_ERROR(PMPI_Comm_size(tmp_comm, &local_size));

      if (error != EMPI_SUCCESS) {
         IERROR(1, "Failed to retrieve size of local communicator! (comm=%d, error=%d)", c->handle, error);
         return error;
      }

      error = create_communicator(reply.newComm, tmp_comm,
                 local_rank, local_size, reply.rank, reply.size,
                 reply.cluster_count, reply.coordinators, reply.cluster_sizes,
                 reply.flags, reply.members,
                 reply.cluster_ranks, reply.member_cluster_index, reply.local_ranks,
                 &result);

      if (error != EMPI_SUCCESS) {
         IERROR(1, "Failed to create new communicator! (comm=%d, error=%d)", c->handle, error);
         return error;
      }

      *newcomm = result->handle;

   } else {
      *newcomm = EMPI_COMM_NULL;
   }

   STACKTRACE(0, "COMM_CREATE %d -> %d", c->handle, result->handle);

   return EMPI_SUCCESS;
}

#define __EMPI_Comm_split
int EMPI_Comm_split(EMPI_Comm comm, int color, int key, EMPI_Comm *newcomm)
{
   int error, local_rank, local_size;
   MPI_Comm tmp;
   comm_reply reply;
   communicator *result;

   // We first forward the split request to the server to split the
   // virtual communicator.
   communicator *c = handle_to_communicator(comm);

   if (c == NULL) {
      ERROR(1, "Communicator %d not found!", comm);
      return EMPI_ERR_COMM;
   }

   // Translate the color from MPI_UNDEFINED to -1 if needed.
   if (color == MPI_UNDEFINED) {
      color = -1;
      key = -1;
   }

   error = messaging_send_comm_request(c, color, key);

   if (error != EMPI_SUCCESS) {
      IERROR(1, "Failed to send comm_split! (comm=%d, error=%d)", c->handle, error);
      return error;
   }

   error = messaging_receive_comm_reply(&reply);

   if (error != EMPI_SUCCESS) {
      IERROR(1, "Failed to receive comm_split! (comm=%d, error=%d)", c->handle, error);
      return error;
   }

   // The reply from the server will tell us how to split the
   // local communicator. Note that we may get a -1 as a color
   // which we need to translate to an MPI_UNDEFINED first.
   if (reply.color < 0) {
      reply.color = EMPI_UNDEFINED;
      reply.key = 0;
   }

   tmp = MPI_COMM_NULL;

   error = TRANSLATE_ERROR(PMPI_Comm_split(c->comm, reply.color, reply.key, &tmp));

   if (error != EMPI_SUCCESS) {
      IERROR(1, "Failed to perform local comm_split! (comm=%d, error=%d)", c->handle, error);
      return error;
   }

   // If a new communicator was returned (that is, color != -1) we
   // have to register the new virtual communicator locally.
   if (tmp != MPI_COMM_NULL) {

      error = TRANSLATE_ERROR(PMPI_Comm_rank(tmp, &local_rank));

      if (error != EMPI_SUCCESS) {
         IERROR(1, "Failed to retrieve rank of local communicator! (comm=%d, error=%d)", c->handle, error);
         return error;
      }

      error = TRANSLATE_ERROR(PMPI_Comm_size(tmp, &local_size));

      if (error != EMPI_SUCCESS) {
         IERROR(1, "Failed to retrieve size of local communicator! (comm=%d, error=%d)", c->handle, error);
         return error;
      }

      error = create_communicator(reply.newComm, tmp,
                 local_rank, local_size, reply.rank, reply.size,
                 reply.cluster_count, reply.coordinators, reply.cluster_sizes,
                 reply.flags, reply.members,
                 reply.cluster_ranks, reply.member_cluster_index, reply.local_ranks,
                 &result);

      if (error != EMPI_SUCCESS) {
         IERROR(1, "Failed to create new communicator! (comm=%d, error=%d)", c->handle, error);
         return error;
      }

      *newcomm = result->handle;
   } else {
      *newcomm = EMPI_COMM_NULL;
   }

   STACKTRACE(0, "COMM_SPLIT %d -> %d", c->handle, result->handle);

   return EMPI_SUCCESS;
}

#define __EMPI_Comm_group
int EMPI_Comm_group(EMPI_Comm comm, EMPI_Group *g)
{
   int error;
   group *res;

   communicator *c = handle_to_communicator(comm);

   if (c == NULL) {
      ERROR(1, "Communicator %d not found!", comm);
      return EMPI_ERR_COMM;
   }

   error = group_comm_group(c, &res);

   if (error == EMPI_SUCCESS) {
      *g =  res->handle;
   } else {
      ERROR(1, "Failed to create group! (comm=%d, error=%d)", c->handle, error);
   }

   return error;
}

#define __EMPI_Comm_free
int EMPI_Comm_free ( EMPI_Comm *comm )
{
   if (*comm == EMPI_COMM_WORLD) {
      // ignored
      return EMPI_SUCCESS;
   }

   if (*comm == EMPI_COMM_NULL) {
      ERROR(1, "Communicator is MPI_COMM_NULL!");
      return EMPI_ERR_COMM;
   }

   communicator *c = handle_to_communicator(*comm);

   if (c == NULL) {
      ERROR(1, "Communicator %d not found!", *comm);
      return EMPI_ERR_COMM;
   }

/*
   Ignored for now, as the spec implies that this is an asynchronous operation!

   error = free_communicator(c);
   comm = MPI_COMM_NULL;
   return error;
*/

   WARN(1, "Ignoring MPI_Comm_free on communicator %d!\n", c->handle);

   *comm = EMPI_COMM_NULL;

   return EMPI_SUCCESS;
}

#define __EMPI_Group_rank
int EMPI_Group_rank(EMPI_Group mg, int *rank)
{
   group *g = handle_to_group(mg);

   if (g == NULL) {
      ERROR(1, "Group %d not found!", mg);
      return EMPI_ERR_GROUP;
   }

   return group_rank(g, rank);
}

#define __EMPI_Group_size
int EMPI_Group_size(EMPI_Group mg, int *size)
{
   group *g = handle_to_group(mg);

   if (g == NULL) {
      ERROR(1, "Group %d not found!", mg);
      return EMPI_ERR_GROUP;
   }

   return group_size(g, size);
}

#define __EMPI_Group_incl
int EMPI_Group_incl(EMPI_Group mg, int n, int *ranks, EMPI_Group *newgroup)
{
   group *res;
   group *g;

   g = handle_to_group(mg);

   if (g == NULL) {
      ERROR(1, "Group %d not found!", mg);
      return EMPI_ERR_GROUP;
   }

   int error = group_incl(g, n, ranks, &res);

   if (error == EMPI_SUCCESS) {
      *newgroup = res->handle;
   } else {
      ERROR(1, "Failed to include group!");
   }

   return error;
}

#define __EMPI_Group_range_incl
int EMPI_Group_range_incl(EMPI_Group mg, int n, int ranges[][3], EMPI_Group *newgroup)
{
   group *res;
   group *g;

   g = handle_to_group(mg);

   if (g == NULL) {
      ERROR(1, "Group %d not found!", mg);
      return EMPI_ERR_GROUP;
   }

   int error = group_range_incl(g, n, ranges, &res);

   if (error == EMPI_SUCCESS) {
      *newgroup = res->handle;
   } else {
      ERROR(1, "Failed to include group range!");
   }

   return error;
}

#define __EMPI_Group_range_excl
int EMPI_Group_range_excl(EMPI_Group mg, int n, int ranges[][3], EMPI_Group *newgroup)
{
   group *res;
   group *g;

   g = handle_to_group(mg);

   if (g == NULL) {
      ERROR(1, "Group %d not found!", mg);
      return EMPI_ERR_GROUP;
   }

   int error = group_range_excl(g, n, ranges, &res);

   if (error == EMPI_SUCCESS) {
      *newgroup = res->handle;
   } else {
      ERROR(1, "Failed to include group range!");
   }

   return error;
}


#define __EMPI_Group_union
int EMPI_Group_union(EMPI_Group group1, EMPI_Group group2, EMPI_Group *newgroup)
{
   int error;
   group *in1;
   group *in2;
   group *out;

   in1 = handle_to_group(group1);

   if (in1 == NULL) {
      ERROR(1, "Group %d not found!", group1);
      return EMPI_ERR_GROUP;
   }

   in2 = handle_to_group(group2);

   if (in2 == NULL) {
      ERROR(1, "Group %d not found!", group2);
      return EMPI_ERR_GROUP;
   }

   error = group_union(in1, in2, &out);

   if (error == EMPI_SUCCESS) {
      *newgroup = out->handle;
   } else {
      ERROR(1, "Failed to perform group union!");
   }

   return error;
}

#define __EMPI_Group_translate_ranks
int EMPI_Group_translate_ranks(EMPI_Group group1, int n, int *ranks1,
                                     EMPI_Group group2, int *ranks2)
{
   int i, j, pid, rank;
   group *in1;
   group *in2;

   in1 = handle_to_group(group1);

   if (in1 == NULL) {
      ERROR(1, "Group %d not found!", group1);
      return EMPI_ERR_GROUP;
   }

   in2 = handle_to_group(group2);

   if (in2 == NULL) {
      ERROR(1, "Group %d not found!", group2);
      return EMPI_ERR_GROUP;
   }


//INFO(1, "EMPI_Group_translate_ranks DEBUG", "group1 %d group2 %d", in1->size, in2->size);

   for (i=0;i<n;i++) {

      rank = ranks1[i];

      if (rank < 0 || rank >= in1->size) {
         ERROR(1, "Incorrect rank!");
         return EMPI_ERR_RANK;
      }

      pid = in1->members[rank];

//INFO(1, "EMPI_Group_translate_ranks DEBUG", "rank %d pid %d", rank, pid);

      ranks2[i] = EMPI_UNDEFINED;

      for (j=0;i<in2->size;j++) {
         if (in2->members[j] == pid) {

//INFO(1, "EMPI_Group_translate_ranks DEBUG", "found pid %d at %d", pid, j);

            ranks2[i] = j;
            break;
         }
      }
   }

   return EMPI_SUCCESS;
}

/*****************************************************************************/
/*                                Datatypes                                  */
/*****************************************************************************/

#define __EMPI_Type_free
int EMPI_Type_free ( EMPI_Datatype *type )
{
   int error;
   datatype *t;

   t = handle_to_datatype(*type);

   if (t == NULL) {
      ERROR(1, "Datatype %d not found!", *type);
      return EMPI_ERR_TYPE;
   }

   error = TRANSLATE_ERROR(PMPI_Type_free(&(t->type)));

   free_datatype(t);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to free datatype %d! (error = %d)", type, error);
   }

   *type = EMPI_DATATYPE_NULL;
   return error;
}

#define __EMPI_Type_get_envelope
int EMPI_Type_get_envelope ( EMPI_Datatype type, int *num_integers, int *num_addresses, int *num_datatypes, int *combiner )
{
   datatype *t;

   t = handle_to_datatype(type);

   if (t == NULL) {
      ERROR(1, "Datatype %d not found!", type);
      return EMPI_ERR_TYPE;
   }

   return TRANSLATE_ERROR(PMPI_Type_get_envelope (t->type, num_integers, num_addresses, num_datatypes, combiner ));
}

#define __EMPI_Type_create_indexed_block
int EMPI_Type_create_indexed_block ( int count, int blocklength, int array_of_displacements[], EMPI_Datatype oldtype, EMPI_Datatype *newtype )
{
   int error;
   datatype *t, *new_t;
   MPI_Datatype mtype;

   t = handle_to_datatype(oldtype);

   if (t == NULL) {
      ERROR(1, "Datatype %d not found!", oldtype);
      return EMPI_ERR_TYPE;
   }

   error = TRANSLATE_ERROR(PMPI_Type_create_indexed_block( count, blocklength, array_of_displacements, t->type, &mtype ));

   if (error != EMPI_SUCCESS) {
      ERROR(1, "MPI_Type_create_indexed_block failed %d!", error);
      return error;
   }

   new_t = add_datatype(mtype);

   if (new_t == NULL) {
      ERROR(1, "Failed to add datatype!");
      return EMPI_ERR_INTERN;
   }

   *newtype = new_t->handle;

   return EMPI_SUCCESS;
}

#define __EMPI_Type_contiguous
int EMPI_Type_contiguous ( int count, EMPI_Datatype old_type, EMPI_Datatype *new_type )
{
   int error;
   datatype *t, *new_t;
   MPI_Datatype mtype;

   t = handle_to_datatype(old_type);

   if (t == NULL) {
      ERROR(1, "Datatype %d not found!", old_type);
      return EMPI_ERR_TYPE;
   }

   error = TRANSLATE_ERROR(PMPI_Type_contiguous( count, t->type, &mtype ));

   if (error != EMPI_SUCCESS) {
      ERROR(1, "MPI_Type_contiguous failed %d!", error);
      return error;
   }

   new_t = add_datatype(mtype);

   if (new_t == NULL) {
      ERROR(1, "Failed to add datatype!");
      return EMPI_ERR_INTERN;
   }

   *new_type = new_t->handle;

   return EMPI_SUCCESS;
}

#define __EMPI_Type_commit
int EMPI_Type_commit ( EMPI_Datatype *type )
{
   // NOTE: there is no disctiction between committed and uncommitted types in EMPI.
   // This will result in 'type leaks', where intermediate types are never released.
   // Since most MPI applications define 'zero to a few' new types we ignore this
   // problem for now.
   int error;
   datatype *t;

   t = handle_to_datatype(*type);

   if (t == NULL) {
      ERROR(1, "Datatype %d not found!", *type);
      return EMPI_ERR_TYPE;
   }

   error = TRANSLATE_ERROR(PMPI_Type_commit(&t->type));

   if (error != EMPI_SUCCESS) {
      ERROR(1, "MPI_Type_commit failed %d!", error);
   }

   return error;
}

int EMPI_Type_get_name ( EMPI_Datatype type, char *type_name, int *resultlen )
{
   int error;
   datatype *t;

   t = handle_to_datatype(type);

   if (t == NULL) {
      ERROR(1, "Datatype %d not found!", type);
      return EMPI_ERR_TYPE;
   }

   error = TRANSLATE_ERROR(PMPI_Type_get_name(t->type, type_name, resultlen));

   if (error != EMPI_SUCCESS) {
      ERROR(1, "MPI_Type_get_name failed %d!", error);
   }

   return error;
}

/*****************************************************************************/
/*                                  Files                                    */
/*****************************************************************************/

int EMPI_File_open ( EMPI_Comm comm, char *filename, int amode, EMPI_Info inf, EMPI_File *fh )
{
   int error;

   MPI_File mpi_file;
   MPI_Info mpi_info;

   communicator *c;
   info *i;
   file *f;

   c = handle_to_communicator(comm);

   if (c == NULL) {
      ERROR(1, "Communicator %d not found!", comm);
      return EMPI_ERR_COMM;
   }

   if (comm_is_local(c)) {
     // simply perform a file open local cluster

      if (inf == EMPI_INFO_NULL) {
         mpi_info = MPI_INFO_NULL;
      } else {
         i = handle_to_info(inf);

         if (i == NULL) {
            ERROR(1, "Info %d not found!", inf);
            return EMPI_ERR_INFO;
         }

         mpi_info = i->info;
      }

      error = TRANSLATE_ERROR(PMPI_File_open(c->comm, filename, amode, mpi_info, &mpi_file));

      if (error != EMPI_SUCCESS) {
         ERROR(1, "Failed to open file %s!", filename);
         return error;
      }

      f = add_file(c, mpi_file);

      if (f == NULL) {
         ERROR(1, "Failed to add file handle!");
         // TODO: we are leaking an MPI_File here!
         return EMPI_ERR_INTERN;
      }

      *fh = file_to_handle(f);
      return EMPI_SUCCESS;
   }

   ERROR(1, "NOT IMPLEMENTED: EMPI_File_open with wide area communicator!");
   return EMPI_ERR_INTERN;
}

int EMPI_File_close ( EMPI_File *fh )
{
   int error;
   file *f;

   f = handle_to_file(*fh);

   if (f == NULL) {
      ERROR(1, "File %d not found!", *fh);
      return EMPI_ERR_FILE;
   }

   error = TRANSLATE_ERROR(PMPI_File_close(&(f->file)));

   remove_file(*fh);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to close file %d!", *fh);
   }

   *fh = EMPI_FILE_NULL;

   return error;
}

int EMPI_File_read_all ( EMPI_File fh, void *buf, int count, EMPI_Datatype type, EMPI_Status *stat )
{
   int error;
   file *f;
   datatype *t;
   MPI_Status mstat;

   f = handle_to_file(fh);

   if (f == NULL) {
      ERROR(1, "File %d not found!", fh);
      return EMPI_ERR_FILE;
   }

   t = handle_to_datatype(type);

   if (t == NULL) {
      ERROR(1, "Datatype %d not found!", type);
      return EMPI_ERR_TYPE;
   }

   if (stat == EMPI_STATUS_IGNORE) {
      error = TRANSLATE_ERROR(PMPI_File_read_all(f->file, buf, count, t->type, MPI_STATUS_IGNORE));
   } else {
      error = TRANSLATE_ERROR(PMPI_File_read_all(f->file, buf, count, t->type, &mstat));
   }

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to read file %d!", fh);
      return error;
   }

   if (stat != EMPI_STATUS_IGNORE) {
      translate_status(f->comm, t, stat, &mstat);
   }

   return error;
}

int EMPI_File_read_at ( EMPI_File fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype type, EMPI_Status *stat )
{
   int error;
   file *f;
   datatype *t;
   MPI_Status mstat;

   f = handle_to_file(fh);

   if (f == NULL) {
      ERROR(1, "File %d not found!", fh);
      return EMPI_ERR_FILE;
   }

   t = handle_to_datatype(type);

   if (t == NULL) {
      ERROR(1, "Datatype %d not found!", type);
      return EMPI_ERR_TYPE;
   }

   if (stat == EMPI_STATUS_IGNORE) {
      error = TRANSLATE_ERROR(PMPI_File_read_at(f->file, (MPI_Offset) offset, buf, count, t->type, MPI_STATUS_IGNORE));
   } else {
      error = TRANSLATE_ERROR(PMPI_File_read_at(f->file, (MPI_Offset) offset, buf, count, t->type, &mstat));
   }

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to read file %d!", fh);
      return error;
   }

   if (stat != EMPI_STATUS_IGNORE) {
      translate_status(f->comm, t, stat, &mstat);
   }

   return error;
}

int EMPI_File_write_at ( EMPI_File fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype type, EMPI_Status *stat )
{
   int error;
   file *f;
   datatype *t;
   MPI_Status mstat;

   f = handle_to_file(fh);

   if (f == NULL) {
      ERROR(1, "File %d not found!", fh);
      return EMPI_ERR_FILE;
   }

   t = handle_to_datatype(type);

   if (t == NULL) {
      ERROR(1, "Datatype %d not found!", type);
      return EMPI_ERR_TYPE;
   }

   if (stat == EMPI_STATUS_IGNORE) {
      error = TRANSLATE_ERROR(PMPI_File_write_at(f->file, (MPI_Offset) offset, buf, count, t->type, MPI_STATUS_IGNORE));
   } else {
      error = TRANSLATE_ERROR(PMPI_File_write_at(f->file, (MPI_Offset) offset, buf, count, t->type, &mstat));
   }

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to write file %d!", fh);
      return error;
   }

   if (stat != EMPI_STATUS_IGNORE) {
      translate_status(f->comm, t, stat, &mstat);
   }

   return error;
}

int EMPI_File_set_view ( EMPI_File fh, EMPI_Offset disp, EMPI_Datatype etype, EMPI_Datatype filetype, char *datarep, EMPI_Info inf )
{
   int error;
   file *f;
   info *i;
   datatype *t1, *t2;
   MPI_Info mpi_info;

   f = handle_to_file(fh);

   if (f == NULL) {
      ERROR(1, "File %d not found!", fh);
      return EMPI_ERR_FILE;
   }

   t1 = handle_to_datatype(etype);

   if (t1 == NULL) {
      ERROR(1, "Datatype %d not found!", etype);
      return EMPI_ERR_TYPE;
   }

   t2 = handle_to_datatype(filetype);

   if (t2 == NULL) {
      ERROR(1, "Datatype %d not found!", filetype);
      return EMPI_ERR_TYPE;
   }

   if (inf == EMPI_INFO_NULL) {
      mpi_info = MPI_INFO_NULL;
   } else {
      i = handle_to_info(inf);

      if (i == NULL) {
         ERROR(1, "Info %d not found!", inf);
         return EMPI_ERR_INFO;
      }

      mpi_info = i->info;
   }

   error = TRANSLATE_ERROR(PMPI_File_set_view(f->file, (MPI_Offset) disp, t1->type, t2->type, datarep, mpi_info));

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to set file view %d!", fh);
   }

   return error;
}

int EMPI_File_write_all ( EMPI_File fh, void *buf, int count, EMPI_Datatype type, EMPI_Status *stat )
{
   int error;
   file *f;
   datatype *t;
   MPI_Status mstat;

   f = handle_to_file(fh);

   if (f == NULL) {
      ERROR(1, "File %d not found!", fh);
      return EMPI_ERR_FILE;
   }

   t = handle_to_datatype(type);

   if (t == NULL) {
      ERROR(1, "Datatype %d not found!", type);
      return EMPI_ERR_TYPE;
   }

   if (stat == EMPI_STATUS_IGNORE) {
      error = TRANSLATE_ERROR(PMPI_File_write_all(f->file, buf, count, t->type, MPI_STATUS_IGNORE));
   } else {
      error = TRANSLATE_ERROR(PMPI_File_write_all(f->file, buf, count, t->type, &mstat));
   }

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to write file %d!", fh);
      return error;
   }

   if (stat != EMPI_STATUS_IGNORE) {
      translate_status(f->comm, t, stat, &mstat);
   }

   return error;
}

/*****************************************************************************/
/*                                   Info                                    */
/*****************************************************************************/

int EMPI_Info_create ( EMPI_Info *ih )
{
   int error;
   MPI_Info mpi_info;
   info *i;

   error = TRANSLATE_ERROR(PMPI_Info_create(&mpi_info));

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to create MPI_Info!");
      return error;
   }

   i = add_info(mpi_info);

   if (i == NULL) {
      ERROR(1, "Failed to create EMPI_Info handle!");
      PMPI_Info_free(&mpi_info);
      return EMPI_ERR_INTERN;
   }

   *ih = info_to_handle(i);
   return EMPI_SUCCESS;
}

int EMPI_Info_delete ( EMPI_Info inf, char *key )
{
   int error;
   info *i;

   i = handle_to_info(inf);

   if (i != NULL) {
      ERROR(1, "Failed to retrieve info %d!", inf);
      return EMPI_ERR_INFO;
   }

   error = TRANSLATE_ERROR(PMPI_Info_delete(i->info, key));

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to delete key %s from info %d!", key, inf);
   }

   return error;
}

int EMPI_Info_set ( EMPI_Info inf, char *key, char *value )
{
   int error;
   info *i;

   i = handle_to_info(inf);

   if (i != NULL) {
      ERROR(1, "Failed to retrieve info %d!", inf);
      return EMPI_ERR_INFO;
   }

   error = TRANSLATE_ERROR(PMPI_Info_set(i->info, key, value));

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to set key %s to value %s for info %d!", key, value, inf);
   }

   return error;
}

int EMPI_Info_free ( EMPI_Info *inf )
{
   int error;
   info *i;

   i = handle_to_info(*inf);

   if (i != NULL) {
      ERROR(1, "Failed to retrieve info %d!", *inf);
      return EMPI_ERR_INFO;
   }

   error = TRANSLATE_ERROR(PMPI_Info_free(&(i->info)));

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to free info %d!", *inf);
   }

   remove_info(*inf);
   *inf = EMPI_INFO_NULL;
   return error;
}

/*****************************************************************************/
/*                                Intercomm                                  */
/*****************************************************************************/


int EMPI_Intercomm_create ( EMPI_Comm local_comm, int local_leader, EMPI_Comm peer_comm, int remote_leader, int tag, EMPI_Comm *newintercomm )
{
   ERROR(1, "NOT IMPLEMENTED!");
   return EMPI_ERR_INTERN;
}

int EMPI_Intercomm_merge ( EMPI_Comm intercomm, int high, EMPI_Comm *newintracomm )
{
   ERROR(1, "NOT IMPLEMENTED!");
   return EMPI_ERR_INTERN;
}


/*****************************************************************************/
/*                                Utilities                                  */
/*****************************************************************************/

/* ========== C <-> Fortran conversions ============ */

/*
#define __EMPI_Comm_f2c
MPI_Comm EMPI_Comm_f2c(MPI_Fint comm)
{
   MPI_Comm res;

   if (comm == FORTRAN_MPI_COMM_WORLD) {
      return MPI_COMM_WORLD;
   }

   if (comm == FORTRAN_MPI_COMM_SELF) {
      return MPI_COMM_SELF;
   }

   if (comm == FORTRAN_MPI_COMM_NULL) {
      return MPI_COMM_NULL;
   }

FIXME!!

   communicator *tmp = handle_to_communicator(comm);
   set_communicator_ptr(&res, tmp);
   return res;
}

#define __EMPI_Comm_c2f
MPI_Fint EMPI_Comm_c2f(MPI_Comm comm)
{
   if (comm == MPI_COMM_WORLD) {
      return FORTRAN_MPI_COMM_WORLD;
   }

   if (comm == MPI_COMM_SELF) {
      return FORTRAN_MPI_COMM_SELF;
   }

   if (comm == MPI_COMM_NULL) {
      return FORTRAN_MPI_COMM_NULL;
   }

   communicator *tmp = handle_to_communicator(comm);
   return tmp->number;
}

#define __EMPI_Group_f2c
MPI_Group EMPI_Group_f2c(MPI_Fint g)
{
   MPI_Group res;

   if (g == FORTRAN_MPI_GROUP_NULL) {
      return MPI_GROUP_NULL;
   }

   if (g == FORTRAN_MPI_GROUP_EMPTY) {
      return MPI_GROUP_EMPTY;
   }

   group *tmp = get_group_with_index(g);

   if (tmp == NULL) {
      ERROR(1, "MPI_Group_f2c(group=%d) group not found!", g);
      return MPI_GROUP_NULL;
   }

   set_group_ptr(&res, tmp);
   return res;
}

#define __EMPI_Group_c2f
MPI_Fint EMPI_Group_c2f(MPI_Group g)
{
   if (g == MPI_GROUP_NULL) {
      return FORTRAN_MPI_GROUP_NULL;
   }

   if (g == MPI_GROUP_EMPTY) {
      return FORTRAN_MPI_GROUP_EMPTY;
   }

   group *tmp = get_group(g);

   if (tmp == NULL) {
      ERROR(1, "MPI_Group_c2f(group=%p) group not found!", (void *)g);
      return FORTRAN_MPI_GROUP_NULL;
   }

   return tmp->index;
}

#define __EMPI_Request_f2c
MPI_Request EMPI_Request_f2c(MPI_Fint req)
{
   MPI_Request r;

   if (req == FORTRAN_MPI_REQUEST_NULL) {
      return MPI_REQUEST_NULL;
   }

   request *tmp = get_request_with_index(req);

   if (tmp == NULL) {
      ERROR(1, "MPI_Request_f2c(req=%d) request not found!", req);
      return MPI_REQUEST_NULL;
   }

   set_request_ptr(&r, tmp);
   return r;
}

#define __EMPI_Request_c2f
MPI_Fint EMPI_Request_c2f(MPI_Request req)
{
   if (req == MPI_REQUEST_NULL) {
      return FORTRAN_MPI_REQUEST_NULL;
   }

   request *tmp = get_request(req);

   if (tmp == NULL) {
      ERROR(1, "MPI_Request_c2f(req=%p) request not found!", req);
      return FORTRAN_MPI_REQUEST_NULL;
   }

   return tmp->index;
}

// We don't need these if we don't support user defines ops yet!

#define __EMPI_Op_f2c
MPI_Op EMPI_Op_f2c(MPI_Fint op)
{
   MPI_Op o;

   if (op == FORTRAN_MPI_OP_NULL) {
      return MPI_OP_NULL;
   }

   operation *tmp = get_operation_with_index(op);

   if (tmp == NULL) {
      ERROR(1, "MPI_Op_f2c(req=%d) operation not found!", op);
      return MPI_OP_NULL;
   }

   set_operation_ptr(&o, tmp);
   return o;
}


#define __EMPI_Op_c2f
MPI_Fint EMPI_Op_c2f(MPI_Op op)
{
   operation *tmp = get_operation(op);

   if (tmp == NULL) {
      ERROR(1, "MPI_Op_c2f(op=%p) op not found!", op);
      return FORTRAN_MPI_OP_NULL;
   }

   return tmp->index;
}
*/

// Include the generated impi implementation which contains
// default implementations for all MPI calls.
//#include "generated_impi.c"





