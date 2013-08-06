#include "flags.h"

#ifdef IBIS_INTERCEPT

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <execinfo.h>

#include "mpi.h"
#include "generated_header.h"

#include "logging.h"
#include "types.h"
#include "request.h"
#include "communicator.h"
#include "group.h"
#include "messaging.h"
#include "wa_sockets.h"
#include "util.h"
#include "debugging.h"
#include "operation.h"
#include "profiling.h"

// The number of clusters and the rank of our cluster in this set.
uint32_t cluster_count;
uint32_t cluster_rank;

// The number of processes in our cluster and our process' rank in this set.
int local_count;
int local_rank;

// The pid of my machine.
uint32_t my_pid;

// The location of the server (hostname/ip and port)
static char *server;
static unsigned short port;

// The name of this cluster (must be unique).
static char *cluster_name;

// The size of each cluster, and the offset of each cluster in the
// total set of machines.
static int *cluster_sizes;
static int *cluster_offsets;

// Value of various Fortan MPI constants.
int FORTRAN_MPI_COMM_NULL;
int FORTRAN_MPI_GROUP_NULL;
int FORTRAN_MPI_REQUEST_NULL;
int FORTRAN_MPI_GROUP_EMPTY;
int FORTRAN_MPI_COMM_WORLD;
int FORTRAN_MPI_COMM_SELF;
int FORTRAN_MPI_OP_NULL;

int FORTRAN_FALSE;
int FORTRAN_TRUE;

/*****************************************************************************/
/*                      Initialization / Finalization                        */
/*****************************************************************************/

static int read_config_file()
{
   int  error;
   char *file;
   FILE *config;
   int retval = 1;
   char buffer[1024];

   file = getenv("MPIBIS_CONFIG");

   if (file == NULL) {
      WARN(0, "MPIBIS_CONFIG not set");
      file = "/home/jason/mpibis.config";
   } else {
      retval = 0;
   }

   INFO(0, "looking for config file %s", file);

   config = fopen(file, "r");

   if (config == NULL) {
      ERROR(1, "Failed to open config file %s", file);
      return 0;
   }

   INFO(0, "config file %s opened.", file);

   error = fscanf(config, "%s", buffer);

   if (error == EOF || error == 0) {
      fclose(config);
      ERROR(1, "Failed to read server adres from %s", file);
      return 0;
   }

   server = malloc(strlen(buffer+1));

   if (server == NULL) {
      fclose(config);
      ERROR(1, "Failed to allocate space for server adres %s", buffer);
      return 0;
   }

   strcpy(server, buffer);

   error = fscanf(config, "%hu", &port);

   if (error == EOF || error == 0) {
      fclose(config);
      ERROR(1, "Failed to read server port from %s", file);
      return 0;
   }

   error = fscanf(config, "%s", buffer);

   if (error == EOF || error == 0) {
      fclose(config);
      ERROR(1, "Failed to read cluster name from %s", file);
      return 0;
   }

   cluster_name = malloc(strlen(buffer)+1);

   if (cluster_name == NULL) {
      fclose(config);
      ERROR(1, "Failed to allocate space for cluster name %s", buffer);
      return 0;
   }

   strcpy(cluster_name, buffer);

   error = fscanf(config, "%d", &cluster_rank);

   if (error == EOF || error == 0) {
      fclose(config);
      ERROR(1, "Failed to read cluster rank from %s", file);
      return 0;
   }

   error = fscanf(config, "%d", &cluster_count);

   if (error == EOF || error == 0) {
      fclose(config);
      ERROR(1, "Failed to read cluster rank from %s", file);
      return 0;
   }

   fclose(config);
   return 1;
}

static int init_mpibis(int *argc, char ***argv)
{
   int i, changed, cnt, len;

   server = NULL;
   port = -1;

   cluster_name = NULL;
   cluster_count = 0;
   cluster_rank = -1;

   // Check sizes of opaque datatypes to make sure that we can replace them
   if (sizeof(MPI_Comm) < sizeof(communicator *)) {
      ERROR(0, "Cannot replace MPI_Comm, handle too small! (%lu < %lu)", sizeof(MPI_Comm), sizeof(communicator *));
      return 0;
   }

   if (sizeof(MPI_Group) < sizeof(group *)) {
      ERROR(0, "Cannot replace MPI_Group, handle too small! (%lu < %lu)", sizeof(MPI_Group), sizeof(group *));
      return 0;
   }

   if (sizeof(MPI_Request) < sizeof(request *)) {
      ERROR(0, "Cannot replace MPI_Request, handle too small! (%lu < %lu)", sizeof(MPI_Request), sizeof(request *));
      return 0;
   }

   // Check the value of COMM_WORLD. This may be a pointer, index or
   // something else. Whatever it is, we must make sure that its value
   // cannot be confused with one of our indexes.
   if (((long) MPI_COMM_WORLD >= 0) && ((long) MPI_COMM_WORLD < MAX_COMMUNICATORS)) {
      ERROR(0, "Cannot detect MPI_COMM_WORLD, as it is within my communicator range (%lu 0-%d)",
               (unsigned long)MPI_COMM_WORLD, MAX_COMMUNICATORS);
      return 0;
   }

   INFO(1, "MPI_COMM_WORLD detectable: %lu",(unsigned long) MPI_COMM_WORLD);

   // First try to read the configuration from an input file whose location is set in the environment.
   // This is needed since Fortran does not pass the command line arguments to the MPI library like C does.
   if (read_config_file() != 1) {
      return 0;
   }

   INFO(1, "MPI SIZES %lu %lu %lu %lu %lu", sizeof(MPI_Comm), sizeof(MPI_Group), sizeof(MPI_Request), sizeof(MPI_Fint), sizeof(int));

   // Next, parse the command line (possibly overwriting the config).
   i = 1;

   while ( i < (*argc) ){
      changed = 0;

      if ( strcmp((*argv)[i],"--wa-server") == 0 ) {
         if ( i+1 < *argc ){
            server = malloc(strlen((*argv)[i+1])+1);
            strcpy(server, (*argv)[i+1]);
            DELETE_ARG;
         } else {
            ERROR(1, "Missing option for --wa-server");
            return 0;
         }
         DELETE_ARG;

      } else if ( strcmp((*argv)[i],"--wa-server-port") == 0 ) {
         if ( i+1 < *argc ){
            port = (unsigned short) atoi((*argv)[i+1]);
            DELETE_ARG;
         } else {
            ERROR(1, "Missing option for --wa-server-port");
            return 0;
         }
         DELETE_ARG;

      } else if ( strcmp((*argv)[i],"--wa-cluster-name") == 0 ) {
         if ( i+1 < *argc ){

            len = strlen((*argv)[i+1]);

            if (len >= MAX_LENGTH_CLUSTER_NAME) {
               ERROR(1, "Cluster name too long (%d)", len);
               return 0;
            } else {
               cluster_name = malloc(len+1);
               strcpy(cluster_name, (*argv)[i+1]);
            }
            DELETE_ARG;
         } else {
            ERROR(1, 0, "Missing option for --wa-cluster-name");
            return 0;
         }
         DELETE_ARG;

      } else if ( strcmp((*argv)[i],"--wa-cluster-rank") == 0 ) {
         if ( i+1 < *argc ){
            cluster_rank = atoi((*argv)[i+1]);
            DELETE_ARG;
         } else {
            ERROR(1, "Missing option for --wa-cluster-rank");
            return 0;
         }
         DELETE_ARG;

      } else if ( strcmp((*argv)[i],"--wa-cluster-count") == 0 ) {
         if ( i+1 < *argc ){
            cluster_count = atoi((*argv)[i+1]);
            DELETE_ARG;
         } else {
            ERROR(1, 0, "Missing option for --wa-cluster-rank");
            return 0;
         }
         DELETE_ARG;
      }

      if ( !changed ) i++;
   }

   if (server == NULL || port <= 0) {
      ERROR(1, "WA server not (correctly) set (%s %d)!", server, port);
      return 0;
   }

   INFO(1, "WA server at %s %d", server, port);

   if (local_rank < 0 || local_count <= 0 || local_rank >= local_count) {
      ERROR(1, "Local cluster info not set correctly (%d, %d)!", local_rank, local_count);
      return 0;
   }

   if (cluster_name == NULL || cluster_rank < 0 || cluster_count <= 0 || cluster_rank >= cluster_count) {
      ERROR(1, "Cluster info not set correctly (%s, %d, %d)!", cluster_name, cluster_rank, cluster_count);
      return 0;
   }

   INFO(1, "I am %d of %d in cluster %s", local_rank, local_count, cluster_name);
   INFO(1, "Cluster %s is %d of %d clusters", cluster_name, cluster_rank, cluster_count);

   cluster_sizes = malloc(cluster_count * sizeof(int));
   cluster_offsets = malloc((cluster_count+1) * sizeof(int));

   my_pid = SET_PID(cluster_rank, local_rank);

   return 1;
}

static void init_constants()
{
   FORTRAN_MPI_COMM_NULL = PMPI_Comm_c2f(MPI_COMM_NULL);
   FORTRAN_MPI_COMM_WORLD = PMPI_Comm_c2f(MPI_COMM_WORLD);
   FORTRAN_MPI_COMM_SELF = PMPI_Comm_c2f(MPI_COMM_SELF);

   FORTRAN_MPI_GROUP_NULL = PMPI_Group_c2f(MPI_GROUP_NULL);
   FORTRAN_MPI_GROUP_EMPTY = PMPI_Group_c2f(MPI_GROUP_EMPTY);

   FORTRAN_MPI_REQUEST_NULL = PMPI_Request_c2f(MPI_REQUEST_NULL);

   FORTRAN_MPI_OP_NULL = PMPI_Op_c2f(MPI_OP_NULL);

   init_fortran_logical_(&FORTRAN_TRUE, &FORTRAN_FALSE);

   INFO(1, "MPI_COMM_NULL    = %p / %d", (void *)MPI_COMM_NULL, FORTRAN_MPI_COMM_NULL);
   INFO(1, "MPI_COMM_WORLD   = %p / %d", (void *)MPI_COMM_WORLD, FORTRAN_MPI_COMM_WORLD);
   INFO(1, "MPI_COMM_SELF    = %p / %d", (void *)MPI_COMM_SELF, FORTRAN_MPI_COMM_SELF);
   INFO(1, "MPI_GROUP_NULL   = %p / %d", (void *)MPI_GROUP_NULL, FORTRAN_MPI_GROUP_NULL);
   INFO(1, "MPI_GROUP_EMPTY  = %p / %d", (void *)MPI_GROUP_EMPTY, FORTRAN_MPI_GROUP_EMPTY);
   INFO(1, "MPI_REQUEST_NULL = %p / %d", (void *)MPI_REQUEST_NULL, FORTRAN_MPI_REQUEST_NULL);
   INFO(1, "MPI_OP_NULL      = %p / %d", (void *)MPI_OP_NULL, FORTRAN_MPI_OP_NULL);
   INFO(1, "FORTRAN_TRUE     = %d", FORTRAN_TRUE);
   INFO(1, "FORTRAN_FALSE    = %d", FORTRAN_FALSE);
}

#define __IMPI_Init
int IMPI_Init(int *argc, char **argv[])
{
   int i=0;

   INFO(0, "Init MPI...");

   int status = PMPI_Init(argc, argv);

   if (status == MPI_SUCCESS) {
      PMPI_Comm_rank(MPI_COMM_WORLD, &local_rank);
      PMPI_Comm_size(MPI_COMM_WORLD, &local_count);

      INFO(1, "START MPI WRAPPER on %d of %d", local_rank, local_count);

      for (i=0;i<*argc;i++) {
         INFO(4, "argv[%d] = %s", i, (*argv)[i]);
      }

      init_constants();

      status = init_groups();

      if (status != MPI_SUCCESS) {
         PMPI_Finalize();
         ERROR(1, "Failed to initialize groups! (error=%d)", status);
         return status;
      }

      status = init_request();

      if (status != MPI_SUCCESS) {
         PMPI_Finalize();
         ERROR(1, "Failed to initialize requests! (error=%d)", status);
         return status;

      }

      status = init_operations();

      if (status != MPI_SUCCESS) {
         PMPI_Finalize();
         ERROR(1, "Failed to initialize operations! (error=%d)", status);
         return status;
      }

      status = init_mpibis(argc, argv);

      if (status == 0) {
         PMPI_Finalize();
         ERROR(1, "Failed to initialize mpibis!");
         return MPI_ERR_INTERN;
      }

      status = wa_init(server, port, local_rank, local_count,
                   cluster_name, cluster_rank, cluster_count,
                   cluster_sizes, cluster_offsets);

      if (status == 0) {
         PMPI_Finalize();
         ERROR(1, "Failed to initialize wide area communication!");
         return MPI_ERR_INTERN;
      }

      status = init_communicators(cluster_rank, cluster_count,
                                  cluster_sizes, cluster_offsets);

      if (status != MPI_SUCCESS) {
         wa_finalize();
         PMPI_Finalize();
         ERROR(1, "Failed to initialize communicators! (error=%d)", status);
         return status;
      }
   }

   return MPI_SUCCESS;
}

#define __IMPI_Finalize
int IMPI_Finalize(void)
{
   int error;

   // We tell the system to shut down by terminating MPI_COMM_WORLD.
   error = messaging_send_terminate_request(get_communicator(MPI_COMM_WORLD));

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to terminate MPI_COMM_WORLD! (error=%d)", error);
      return error;
   }

   wa_finalize();

   return PMPI_Finalize();
}

#define __IMPI_Abort
int IMPI_Abort(MPI_Comm comm, int errorcode)
{
   communicator *c = get_communicator(comm);

   return PMPI_Abort(c->comm, errorcode);
}

/*****************************************************************************/
/*                             Send / Receive                                */
/*****************************************************************************/

static int do_send(void* buf, int count, MPI_Datatype datatype, int dest, int tag, communicator *c)
{
   int error;

   if (rank_is_local(c, dest)) {
      error = PMPI_Send(buf, count, datatype, get_local_rank(c, dest), tag, c->comm);
   } else {
      error = messaging_send(buf, count, datatype, dest, tag, c);
   }

   if (error != MPI_SUCCESS) {
      ERROR(1, "Rank %d (in cluster %d) failed to send data to %d (in cluster %d) (comm=%d, error=%d)!\n", c->global_rank, cluster_rank, dest, get_cluster_rank(c, dest), c->number, error);
   }

   return error;
}

// FIXME: assumes source != MPI_ANY_SOURCE ?
static int do_recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Status *status, communicator *c)
{
   int error;

   if (source == MPI_ANY_SOURCE) {
      FATAL("do_recv from MPI_ANY_SOURCE not supported yet!");
      return MPI_ERR_INTERN;
   }

   if (rank_is_local(c, source)) {
      // local recv
      error = PMPI_Recv(buf, count, datatype, get_local_rank(c, source), tag, c->comm, status);

      if (error == MPI_SUCCESS && status != MPI_STATUS_IGNORE) {
            status->MPI_SOURCE = get_global_rank(c, cluster_rank, status->MPI_SOURCE);
      }
   } else {
      // remote recv
      error = messaging_receive(buf, count, datatype, source, tag, status, c);
   }

   if (error != MPI_SUCCESS) {
      ERROR(1, "Rank %d (in cluster %d) failed to receive data from %d (in cluster %d) (comm=%d, error=%d)!\n", c->global_rank, cluster_rank, source, get_cluster_rank(c, source), c->number, error);
   }

   return error;
}

#define __IMPI_Send
int IMPI_Send(void* buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
{
   communicator *c = get_communicator(comm);

   CHECK_COUNT(count);
   CHECK_DESTINATION(c, dest);

   return do_send(buf, count, datatype, dest, tag, c);
}

#define __IMPI_Rsend
int IMPI_Rsend(void* buf, int count, MPI_Datatype datatype,
               int dest, int tag, MPI_Comm comm)
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
   return IMPI_Ssend(buf, count, datatype, dest, tag, comm);
}

#define __IMPI_Isend
int IMPI_Isend(void *buf, int count, MPI_Datatype datatype,
               int dest, int tag, MPI_Comm comm, MPI_Request *req)
{
   int error, local, flags = REQUEST_FLAG_SEND;
   request *r;

   communicator *c = get_communicator(comm);

   CHECK_COUNT(count);
   CHECK_DESTINATION(c, dest);

   local = rank_is_local(c, dest);

   if (local) {
      flags |= REQUEST_FLAG_LOCAL;
   }

   r = create_request(flags, buf, count, datatype, dest, tag, c);

   if (r == NULL) {
      FATAL("Failed to create request!");
      return MPI_ERR_INTERN;
   }

//INFO(3, "Isend local ? %d", local);

   if (local) {

//INFO(3, "Isend local buf=%p count=%d <type> dest=%d/%d tag=%d comm=%d", buf, count, dest, get_local_rank(c, dest), tag, c->number);

      error = PMPI_Isend(buf, count, datatype, get_local_rank(c, dest), tag, c->comm, &(r->req));

//INFO(3, "Isend local req=%p", r->req);

   } else {
      error = messaging_send(buf, count, datatype, dest, tag, c);
   }

   if (error != MPI_SUCCESS) {
      free_request(r);
      *req = MPI_REQUEST_NULL;
      ERROR(1, "Failed to send data! (comm=%d, error=%d)", c->number, error);
      return error;
   }

   // We stuff our own data into the users request pointer here...
   set_request_ptr(req, r);

   return MPI_SUCCESS;
}

#define __IMPI_Irsend
int IMPI_Irsend(void *buf, int count, MPI_Datatype datatype,
                int dest, int tag, MPI_Comm comm, MPI_Request *req)
{
   // As with rsend, we can simply replace this call with a normal isend.
   return IMPI_Isend(buf, count, datatype, dest, tag, comm, req);
}

#define __IMPI_Irecv
int IMPI_Irecv(void *buf, int count, MPI_Datatype datatype,
               int source, int tag, MPI_Comm comm, MPI_Request *req)
{
   int error, local, flags = REQUEST_FLAG_RECEIVE, local_source;
   request *r;

   // We first unpack the communicator.
   communicator *c = get_communicator(comm);

   CHECK_COUNT(count);
   CHECK_SOURCE(c, source);

   // Next, we check if the source is local or not.
   if (source == MPI_ANY_SOURCE) {
      // if source is any, the local flag is determined by the distribution of the communicator.
      local = comm_is_local(c);
//INFO(3, "Irec local ? %d (because comm is local)", local);
   } else {
      local = rank_is_local(c, source);
//INFO(3, "Irec local ? %d (because source is local)", local);
   }

   // Next, we create the request struct.
   if (local) {
      flags |= REQUEST_FLAG_LOCAL;
   }

   r = create_request(flags, buf, count, datatype, source, tag, c);

   if (r == NULL) {
      IERROR(1, "Failed to create request!");
      return MPI_ERR_INTERN;
   }

//INFO(3, "Created request %p", r);

   // Post the ireceive if it is local
   if (local == 1) {

      if (source == MPI_ANY_SOURCE) {
         local_source = MPI_ANY_SOURCE;
      } else {
         local_source = get_local_rank(c, source);
      }

      // If the source is guarenteed to be local, we directly use MPI.
      error = PMPI_Irecv(buf, count, datatype, local_source, tag, c->comm, &(r->req));

      if (error != MPI_SUCCESS) {
         ERROR(1, "IRecv failed! (comm=%d,error=%d)", c->number, error);
         free_request(r);
         *req = MPI_REQUEST_NULL;
         return error;
      }

//INFO(3, "Irec local request %p", r->req);
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
   set_request_ptr(req, r);
   return MPI_SUCCESS;
}

#define __IMPI_Recv
int IMPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
{
   int local, error;

   communicator *c = get_communicator(comm);

   CHECK_COUNT(count);
   CHECK_SOURCE(c, source);

   if (source == MPI_ANY_SOURCE) {
      // if source is any, the local flag is determined by the distribution of the communicator.
      local = comm_is_local(c);
   } else {
      local = rank_is_local(c, source);
   }

   if (local == 1) {
      error = PMPI_Recv(buf, count, datatype, source, tag, c->comm, status);

      if (error == MPI_SUCCESS && status != MPI_STATUS_IGNORE) {
         status->MPI_SOURCE = get_global_rank(c, cluster_rank, status->MPI_SOURCE);
      }

      return error;
   } else {
// FIXME: messaging_receive will block on the WA, which is NOT what we want!

      if (source == MPI_ANY_SOURCE) {
         IERROR(0, "MPI_Recv from MIXED MPI_ANY_SOURCE not implemented yet (buggerit!)");
         return MPI_ERR_RANK;
      }

      return messaging_receive(buf, count, datatype, source, tag, status, c);
   }
}


#define __IMPI_Sendrecv
int IMPI_Sendrecv(void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag,
                  void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag,
                  MPI_Comm comm, MPI_Status *status)
{
   int error;

   communicator *c = get_communicator(comm);

   CHECK_COUNT(sendcount);
   CHECK_DESTINATION(c, dest);

   CHECK_COUNT(recvcount);
   CHECK_SOURCE(c, source);

   if (comm_is_local(c)) {
     // simply perform a sendrecv in local cluster
     return PMPI_Sendrecv(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status);
   }

   // FIXME: Does this work if source == MPI_ANY_SOURCE ?

   // We need to perform a WA sendrecv. NOTE: It is still possble that both the send
   // and recv operations are local. We cannot directly use PMPI_Sendrecv, however, as
   // the dest and source ranks may be invalid locally.

   // Reverse the send and receive order on even and odd processors.
   if ((c->global_rank % 2) == 0) {

      error = do_send(sendbuf, sendcount, sendtype, dest, sendtag, c);

      if (error != MPI_SUCCESS) {
         return error;
      }

      error = do_recv(recvbuf, recvcount, recvtype, source, recvtag, status, c);

      if (error != MPI_SUCCESS) {
         return error;
      }

   } else {

      error = do_recv(recvbuf, recvcount, recvtype, source, recvtag, status, c);

      if (error != MPI_SUCCESS) {
         return error;
      }

      error = do_send(sendbuf, sendcount, sendtype, dest, sendtag, c);

      if (error != MPI_SUCCESS) {
         return error;
      }
   }

   return MPI_SUCCESS;
}

#define __IMPI_Ssend
int IMPI_Ssend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm )
{
   communicator *c = get_communicator(comm);

   CHECK_COUNT(count);
   CHECK_DESTINATION(c, dest);

   if (comm_is_local(c)) {
     // simply perform a ssend in local cluster
     return PMPI_Ssend(buf, count, datatype, dest, tag, c->comm);
   }

   if (rank_is_local(c, dest)) {
      // local send
     return PMPI_Ssend(buf, count, datatype, get_local_rank(c, dest), tag, c->comm);
   } else {
     // remote send
     WARN(1, "Incorrect WA ssend implementation (in communicator %d)!", c->number);
     return messaging_send(buf, count, datatype, dest, tag, c);
   }
}

/*****************************************************************************/
/*                             Waits / Polling                               */
/*****************************************************************************/

static void clear_status(MPI_Status *status)
{
   if (status != MPI_STATUS_IGNORE) {
      status->MPI_SOURCE = MPI_ANY_SOURCE;
      status->MPI_TAG = MPI_ANY_TAG;
      status->MPI_ERROR = 0;

      // FIXME: What datatype must we use in an empty status ?
      // The standard doesn't say!
      MPI_Status_set_elements(status, MPI_BYTE, 0);
   }
}

static int probe_request(MPI_Request *req, int blocking, int *flag, MPI_Status *status)
{
   int error = MPI_SUCCESS;
   MPI_Aint extent;

   request *r = get_request(*req);

   if (r == NULL) {

      DEBUG(1, "request=NULL, blocking=%d", blocking);

      clear_status(status);
      *flag = 1;
      return MPI_SUCCESS;
   }

/*
// FIXME: for performance debugging!!!

   if (r->c->number == 23 || r->c->number == 24 || r->c->number == 16) {

      error = PMPI_Type_extent(r->type, &extent);

      if (error != MPI_SUCCESS) {
         extent = 0;
      }

      STACKTRACE(0, "in wait/test for comm %d count %d bytes %d", r->c->number, r->count, r->count*extent);
   }

// END FIXME: for performance debugging!!!
*/

   INFO(1, "request=(index=%d, flags=%d, srcdest=%d, count=%d, tag=%d type=%s) blocking=%d",
	r->index, r->flags, r->source_or_dest, r->count, r->tag, type_to_string(r->type), blocking);

   // We don't support persistent request yet!
   if (request_persistent(r)) {
      FATAL(0, "Persistent requests not supported yet! (MPI_Test)");
      clear_status(status);
      free_request(r);
      *flag = 1;
      *req = MPI_REQUEST_NULL;
      return MPI_ERR_REQUEST;

   } else if (request_local(r)) {
      // Pure local request, so we ask MPI.

      INFO(2, "request=LOCAL blocking=%d %p", blocking, r->req);

      if (blocking) {
         r->error = PMPI_Wait(&(r->req), status);
         *flag = 1;
      } else {
         r->error = PMPI_Test(&(r->req), flag, status);
      }

      if (*flag == 1) {
         // We must translate local source rank to global rank.

      INFO(2, "translate local rank=%d to global rank", status->MPI_SOURCE);

         status->MPI_SOURCE = get_global_rank(r->c, cluster_rank, status->MPI_SOURCE);
      }

   } else if (request_send(r)) {

      INFO(2, "request=WA_SEND blocking=%d", blocking);

      // Non-persistent WA send should already have finished.
      status->MPI_SOURCE = r->source_or_dest;
      status->MPI_TAG = r->tag;
      status->MPI_ERROR = MPI_SUCCESS;
      r->error = MPI_SUCCESS;
      *flag = 1;

   } else if (r->source_or_dest != MPI_ANY_SOURCE) {

      INFO(2, "request=WA_RECEIVE blocking=%d", blocking);

      // It was a non-persistent remote receive request, so probe the
      // WA link (will do nothing if the request was already completed).
      messaging_probe_receive(r, blocking);

      if (request_completed(r)) {
         *flag = 1;
         if (r->error == MPI_SUCCESS) {
            r->error = messaging_finalize_receive(r, status);
         } else {
            status->MPI_SOURCE = r->source_or_dest;
            status->MPI_TAG = r->tag;
            status->MPI_ERROR = r->error;
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

            r->error = PMPI_Iprobe(MPI_ANY_SOURCE, r->tag, r->c->comm, flag, MPI_STATUS_IGNORE);

            if (r->error != MPI_SUCCESS) {
               IERROR(1, "IProbe from MPI_ANY_SOURCE failed! ()");
               return MPI_ERR_INTERN;
            }

            if (*flag) {

               DEBUG(3, "request=WA_RECEIVE_ANY performing LOCAL receive");

               // A message is available locally, so receiving it!
               r->error = PMPI_Recv(r->buf, r->count, r->type, MPI_ANY_SOURCE, r->tag, r->c->comm, status);
               r->flags |= REQUEST_FLAG_COMPLETED;

               // We must translate local source rank to global rank.
               status->MPI_SOURCE = get_global_rank(r->c, cluster_rank, status->MPI_SOURCE);

            } else {

               DEBUG(3, "request=WA_RECEIVE_ANY performing WA probe");

               // No local message was found (yet), so try the WA link.
               // NOTE: we should poll here, so blocking is set to 0,
               // ignoring the value of the parameter.
               r->error = messaging_probe_receive(r, 0 /*blocking*/);

               if (request_completed(r)) {
                  DEBUG(3, "request=WA_RECEIVE_ANY performed WA receive");

                  if (r->error == MPI_SUCCESS) {
                     r->error = messaging_finalize_receive(r, status);
                  } else {
                     status->MPI_SOURCE = r->source_or_dest;
                     status->MPI_TAG = r->tag;
                     status->MPI_ERROR = r->error;
                  }
                  *flag = 1;
               }
            }

         } while (blocking && (*flag == 0) && (r->error == MPI_SUCCESS));
      }
   }

   if (*flag == 1) {

   DEBUG(1, "received data %d", ((int *)r->buf)[0]);

      error = r->error;
      free_request(r);
      *req = MPI_REQUEST_NULL;
   }

   DEBUG(1, "done error=%d");

   return error;
}

#define __IMPI_Test
int IMPI_Test(MPI_Request *req, int *flag, MPI_Status *status)
{
   int error = probe_request(req, 0, flag, status);

   if (error != MPI_SUCCESS) {
      IERROR(1, "Probe request failed!");
   }

   return error;
}

#define __IMPI_Wait
int IMPI_Wait(MPI_Request *req, MPI_Status *status)
{
   int flag = 0;

   int error = probe_request(req, 1, &flag, status);

   if (error != MPI_SUCCESS) {
      IERROR(1, "Probe request failed!");
   }

   return error;
}

#define __IMPI_Waitany
int IMPI_Waitany(int count, MPI_Request *array_of_requests, int *index, MPI_Status *status)
{
   int i;
   int error;
   int undef;
   int flag = 0;

   while (1) {

      undef = 0;

      for (i=0;i<count;i++) {
         if (array_of_requests[i] != MPI_REQUEST_NULL) {
            error = IMPI_Test(&array_of_requests[i], &flag, status);

            if (error != MPI_SUCCESS) {
               *index = i;
               return MPI_ERR_IN_STATUS;
            }

            if (flag) {
               *index = i;
               return MPI_SUCCESS;
            }

         } else {
            undef++;
         }
      }

      if (undef == count) {
         // All requests where MPI_REQUEST_NULL
         *index = MPI_UNDEFINED;
         clear_status(status);
         return MPI_SUCCESS;
      }
   }

   // unreachable!
   return MPI_ERR_INTERN;
}

#define __IMPI_Waitall
/*
int IMPI_Waitall(int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses)
{
   int i, error;
   int errors = 0;

   DEBUG(0, "Wating for %d requests", count);

   for (i=0;i<count;i++) {
      if (array_of_requests[i] != MPI_REQUEST_NULL) {

         DEBUG(1, "checking %d of %d request=%s", i, count, request_to_string(array_of_requests[i]));

         error = IMPI_Wait(&array_of_requests[i], &array_of_statuses[i]);

         // NOTE: fall back to MPI_Wait for profiling purposes!
//         error = MPI_Wait(&array_of_requests[i], &array_of_statuses[i]);

         DEBUG(1, "done %d of %d request=%s -> error=%d", i, count, request_to_string(array_of_requests[i]), error);

         if (error != MPI_SUCCESS) {
            errors++;
         }
      }
   }

   DEBUG(0, "count=%d error=%d", count, errors);

   if (errors != 0) {
      return MPI_ERR_IN_STATUS;
   }

   return MPI_SUCCESS;;
}
*/

int IMPI_Waitall(int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses)
{
   int i, error;
   int errors = 0;
   int done = 0;
   int flag;

   while (done < count) { 

      DEBUG(0, "Waiting for %d of %d requests", count-done, count);

      for (i=0;i<count;i++) {
         if (array_of_requests[i] != MPI_REQUEST_NULL) {

            DEBUG(1, "checking %d of %d request=%s", i, count, request_to_string(array_of_requests[i]));

            flag = 0;

            error = IMPI_Test(&array_of_requests[i], &flag, &array_of_statuses[i]);

            DEBUG(1, "test %d of %d request=%s -> flag=%d error=%d", i, count, request_to_string(array_of_requests[i]), flag, error);

            if (flag == 1) { 
              done++;
              array_of_requests[i] = MPI_REQUEST_NULL;
            }

            if (error != MPI_SUCCESS) {
               errors++;
            }

         } 
      }
   }

   DEBUG(0, "count=%d error=%d", count, errors);

   if (errors != 0) {
      return MPI_ERR_IN_STATUS;
   }

   return MPI_SUCCESS;;
}


#define __IMPI_Request_free
int IMPI_Request_free(MPI_Request *r)
{
   int error = MPI_SUCCESS;
   request *req = NULL;

//INFO(2, "Free request %p", r);

   if (r == NULL || *r == MPI_REQUEST_NULL) {
      return MPI_SUCCESS;
   }

   req = get_request(*r);

   if (req == NULL) {
      IERROR(0, "Request not found!\n");
      return MPI_ERR_REQUEST;
   }

//INFO(2, "req->req %p", req->req);

   if (req->req != MPI_REQUEST_NULL) {

//INFO(2, "Forwarding request free!");

      error = PMPI_Request_free(&(req->req));
   }

//INFO(2, "Free request struct");

   free_request(req);
   *r = MPI_REQUEST_NULL;

   return error;
}

#define __IMPI_Request_get_status
int IMPI_Request_get_status(MPI_Request req, int *flag, MPI_Status *status )
{
   int error = MPI_SUCCESS;
   request *r = get_request(req);

   if (r == NULL) {
      clear_status(status);
      *flag = 1;
      return MPI_SUCCESS;
   }

   // We don't support persistent request yet!
   if (request_persistent(r)) {
      IERROR(0, "persistent requests not supported yet! (MPI_Test)");
      clear_status(status);
      *flag = 1;
      return MPI_ERR_REQUEST;
   }

   if (request_local(r)) {
      // Pure local request, so we ask MPI.
      error = PMPI_Request_get_status(r->req, flag, status);

      if (error == MPI_SUCCESS && status != MPI_STATUS_IGNORE) {
         status->MPI_SOURCE = get_global_rank(r->c, cluster_rank, status->MPI_SOURCE);
      }

      return error;
  }

   // It was a WA or mixed request.
   // Non-persistent send should already have finished.
   if (request_send(r)) {
      status->MPI_SOURCE = r->source_or_dest;
      status->MPI_TAG = r->tag;
      status->MPI_ERROR = MPI_SUCCESS;
      *flag = 1;
      return MPI_SUCCESS;
   }

   // Non-persistent receive may not have completed yet.
   *flag = request_completed(r);

   if (!(*flag)) {
// FIXME: should also probe local here ??
      messaging_probe_receive(r, 0);
   }

   if (request_completed(r)) {
      *flag = 1;
      if (r->error == MPI_SUCCESS) {
         r->error = messaging_finalize_receive(r, status);
      } else {
         status->MPI_SOURCE = r->source_or_dest;
         status->MPI_TAG = r->tag;
         status->MPI_ERROR = r->error;
      }
   }

   if (*flag) {
      error = r->error;
   }

   return error;
}

/*****************************************************************************/
/*                              Collectives                                  */
/*****************************************************************************/

#define __IMPI_Barrier
int IMPI_Barrier(MPI_Comm comm)
{
   int error, i;
   char buffer = 42;

   communicator *c = get_communicator(comm);

   if (comm_is_local(c)) {
     // simply perform a barrier in local cluster
     return PMPI_Barrier(c->comm);
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

   error = PMPI_Barrier(c->comm);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Local barrier failed! (comm=%d, error=%d)", c->number, error);
      return MPI_ERR_INTERN;
   }

   // Perform the WA barrier (coordinators only)
   if (c->global_rank == c->coordinators[0]) {

//INFO(1, "IMPI_Barrier WA BARRIER", "I am coord[0]");

      // Coordinator 0 first receives from all others....
      for (i=1;i<c->cluster_count;i++) {

//INFO(1, "IMPI_Barrier WA BARRIER", "Receiving from coord[i]=%d", c->coordinators[i]);

         error = messaging_receive(&buffer, 1, MPI_BYTE, c->coordinators[i], BARRIER_TAG, MPI_STATUS_IGNORE, c);

         if (error != MPI_SUCCESS) {
            ERROR(1, "WA receive failed! (comm=%d, error=%d)", c->number, error);
            return MPI_ERR_INTERN;
         }
      }

//INFO(1, "IMPI_Barrier WA BARRIER", "Bcast result from coord[0]");

      // ... then bcasts reply.
      error = messaging_bcast(&buffer, 1, MPI_BYTE, c->coordinators[0], c);

      if (error != MPI_SUCCESS) {
         ERROR(1, "IMPI_Barrier: WA bcast failed! (comm=%d, error=%d)", c->number, error);
         return MPI_ERR_INTERN;
      }

   } else {

      for (i=1;i<c->cluster_count;i++) {
         if (c->global_rank == c->coordinators[i]) {

//INFO(1, "IMPI_Barrier WA BARRIER", "I am coord[%d]=%d", i, c->coordinators[i]);

            // All other coordinators first send to coordinator 0...

//INFO(1, "IMPI_Barrier WA BARRIER", "Sending to coord[0]");

            error = messaging_send(&buffer, 1, MPI_BYTE, c->coordinators[0], BARRIER_TAG, c);

            if (error != MPI_SUCCESS) {
               ERROR(1, "IMPI_Barrier: WA send failed! (comm=%d, error=%d)", c->number, error);
               return MPI_ERR_INTERN;
            }

///INFO(1, "IMPI_Barrier WA BARRIER", "Receiving BCAST");

            // Then wait for reply.
            error = messaging_bcast_receive(&buffer, 1, MPI_BYTE, c->coordinators[0], c);

            if (error != MPI_SUCCESS) {
               ERROR(1, "IMPI_Barrier: WA bcast receive failed (error=%d)!", error);
               return MPI_ERR_INTERN;
            }
         }
      }
   }

//INFO(1, "IMPI_Barrier WA BARRIER", "LOCAL BARRIER(2)");

   // Perform the second local barrier
   error = PMPI_Barrier(c->comm);

   if (error != MPI_SUCCESS) {
      ERROR(1, "IMPI_Barrier: local barrier failed! (2) (comm=%d, error=%d)", c->number, error);
      return MPI_ERR_INTERN;
   }

// INFO(1, "IMPI_Barrier WA BARRIER", "DONE!");

   return MPI_SUCCESS;
}

#define __IMPI_Bcast
int IMPI_Bcast(void* buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
{
   int error;

   communicator *c = get_communicator(comm);

   if (comm_is_local(c)) {

DEBUG(1, "Local BCAST");

     // simply perform a bcast in local cluster
     return PMPI_Bcast(buffer, count, datatype, root, c->comm);
   }

   // We need to perform a WA BCAST.

DEBUG(1, "WA BCAST root=%d grank=%d gsize=%d lrank=%d lsize=%d", root, c->global_rank, c->global_size, c->local_rank, c->local_size);

   // If we are root we first send the data to the server and then bcast locally.
   if (c->global_rank == root) {

DEBUG(1, "WA BCAST I am root");

      error = messaging_bcast(buffer, count, datatype, root, c);

      if (error != MPI_SUCCESS) {
         ERROR(1, "Root %d failed to broadcast! (comm=%d, error=%d)", root, c->number, error);
         return error;
      }

      return PMPI_Bcast(buffer, count, datatype, c->local_rank, c->comm);
   }


DEBUG(1, "WA BCAST I am NOT root");

   // Check if we are in the same cluster as the root. If so, just receive its bcast.
   if (rank_is_local(c, root)) {

DEBUG(1, "WA BCAST Root is local (grank=%d lrank=%d)", root, get_local_rank(c, root));

      return PMPI_Bcast(buffer, count, datatype, get_local_rank(c, root), c->comm);
   }

DEBUG(1, "WA BCAST Root is remote (%d)", root);

   // If we are in a different cluster from the root and we are the local coordinator
   // we first receive the WA bcast and then forward this bcast locally
   if (c->global_rank == c->my_coordinator) {

DEBUG(1, "WA BCAST I am coordinator (grank=%d lrank=%d) -- doing receive", c->global_rank, c->my_coordinator);

      error = messaging_bcast_receive(buffer, count, datatype, root, c);

      if (error != MPI_SUCCESS) {
         ERROR(1, "Local root failed to receive broadcast! (comm=%d, error=%d)", c->number, root);
         return error;
      }
   }

DEBUG(1, "WA BCAST Local bcast to finish up (coordinator grank=%d lrank=%d)", c->my_coordinator, get_local_rank(c, c->my_coordinator));

   return PMPI_Bcast(buffer, count, datatype, get_local_rank(c, c->my_coordinator), c->comm);
}


static int WA_Gatherv(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                      void *recvbuf, int *recvcounts, int *displs, MPI_Datatype recvtype,
                      int root, communicator *c)
{
   int error, i;
   MPI_Aint extent;

   // We implement a WA gather using simple send and receive primitives. This could be optimized by using
   // Async send and receives. NOTE: optimizing using message combining between clusters is harder than it
   // seems, since only the root knows exactly how much data each process sends...

   if (c->global_rank == root) {

      error = PMPI_Type_extent(recvtype, &extent);

      if (error != MPI_SUCCESS) {
         ERROR(1, "Failed to retrieve data size! (comm=%d, error=%d)", c->number, error);
         return error;
      }

      for (i=0;i<c->global_size;i++) {
         if (i == root) {
            memcpy(recvbuf + (displs[i] * extent), sendbuf, (recvcounts[i]*extent));
         } else {
            error = do_recv(recvbuf + (displs[i] * extent), recvcounts[i], recvtype, i, GATHERV_TAG, MPI_STATUS_IGNORE, c);

            if (error != MPI_SUCCESS) {
               ERROR(1, "Failed to receive data from %d for gather! (comm=%d, error=%d)", i, c->number, error);
               return error;
            }
         }
      }

   } else {
      error = do_send(sendbuf, sendcount, sendtype, root, GATHERV_TAG, c);

      if (error != MPI_SUCCESS) {
         ERROR(1, "Failed to send data from %d to root for gatherv! (comm=%d, error=%)", c->global_rank, c->number, error);
         return error;
      }
   }

   return MPI_SUCCESS;
}


#define __IMPI_Gatherv
int IMPI_Gatherv(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                 void *recvbuf, int *recvcounts, int *displs, MPI_Datatype recvtype,
                 int root, MPI_Comm comm)
{
   communicator *c = get_communicator(comm);

   CHECK_COUNT(sendcount);

   if (comm_is_local(c)) {
     // simply perform a gatherv in local cluster
     return PMPI_Gatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, c->comm);
   }

   return WA_Gatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, root, c);
}

#define __IMPI_Gather
int IMPI_Gather(void* sendbuf, int sendcount, MPI_Datatype sendtype,
                void* recvbuf, int recvcount, MPI_Datatype recvtype,
                int root, MPI_Comm comm)
{
   int i, error;
   int *displs = NULL;
   int *counts = NULL;

   communicator *c = get_communicator(comm);

   if (comm_is_local(c)) {
     // simply perform a gatherv in local cluster
     return PMPI_Gather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, c->comm);
   }

   // We implement a WA Gather using Gatherv
   // NOTE: the receive related parameters are only valid on root!

   if (c->global_rank == root) {

      displs = malloc(c->global_size * sizeof(int));

      if (displs == NULL) {
         ERROR(0, "Failed to allocate local buffer! (comm=%d)", c->number);
         return MPI_ERR_INTERN;
      }

      counts = malloc(c->global_size * sizeof(int));

      if (counts == NULL) {
         ERROR(0, "Failed to allocate local buffer! (comm=%d)", c->number);
         free(displs);
         return MPI_ERR_INTERN;
      }

      for (i=0;i<c->global_size;i++) {
         displs[i] = i*recvcount;
         counts[i] = recvcount;
      }
   }

   error = WA_Gatherv(sendbuf, sendcount, sendtype, recvbuf, counts, displs, recvtype, root, c);

   if (c->global_rank == root) {
      free(displs);
      free(counts);
   }

   return error;
}


#define __IMPI_Allgather
int IMPI_Allgather(void* sendbuf, int sendcount, MPI_Datatype sendtype,
                   void* recvbuf, int recvcount, MPI_Datatype recvtype,
                   MPI_Comm comm)
{
   int *displs;
   int *recvcounts;
   int i, error;

   communicator *c = get_communicator(comm);

   if (comm_is_local(c)) {
     // simply perform an allgather in local cluster
     return PMPI_Allgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, c->comm);
   }

INFO(1, "sendcount=%d recvcount=%d", sendcount, recvcount);

   // We implements a Allgather on top of a Allgatherv.
   displs = malloc(c->global_size * sizeof(int));

   if (displs == NULL) {
      ERROR(0, "Failed to allocate local buffer! (comm=%d)", c->number);
      return MPI_ERR_INTERN;
   }

   recvcounts = malloc(c->global_size * sizeof(int));

   if (recvcounts == NULL) {
      ERROR(0, "Failed to allocate local buffer! (comm=%d)", c->number);
      free(displs);
      return MPI_ERR_INTERN;
   }

   for (i=0;i<c->global_size;i++) {
      displs[i] = i*recvcount;
      recvcounts[i] = recvcount;
INFO(1, "displs[%d]=%d recvcounts[%d]=%d", i, displs[i], i, recvcounts[i]);
   }

   error = IMPI_Allgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);

   free(displs);
   free(recvcounts);

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

#define __IMPI_Allgatherv
int IMPI_Allgatherv(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                    void *recvbuf, int *recvcounts,
                    int *displs, MPI_Datatype recvtype, MPI_Comm comm)
{
   int tmp, sum, error, i, offset;
   int *sums;
   int *offsets;
   unsigned char *buffer;

   MPI_Aint extent;

   communicator *c = get_communicator(comm);

   if (comm_is_local(c)) {
     // simply perform an allgatherv in local cluster
     return PMPI_Allgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, c->comm);
   }

   // We need to perform a WA Allgatherv.

   error = PMPI_Type_extent(recvtype, &extent);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to retrieve data size! (comm=%d, error=%d)", c->number, error);
      return error;
   }

INFO(2, "Performing WA allgatherv");

   // First retrieve the data element size
   // FIXME: use size?

   // Next, get the number of elements sent per cluster, the offsets in the buffer, and the total number of elements.
   sums = malloc(c->cluster_count * sizeof(int));

   if (sums == NULL) {
      ERROR(1, "Failed to allocated space for local sums! (comm=%d)", c->number);
      return MPI_ERR_INTERN;
   }

   offsets = malloc(c->cluster_count * sizeof(int));

   if (offsets == NULL) {
      ERROR(1, "Failed to allocated space for local offsets! (comm=%d)", c->number);
      return MPI_ERR_INTERN;
   }

   sum = get_count_sums(c, recvcounts, sums, offsets);

INFO(2, "Sum = %d", sum);

for (i=0;i<c->cluster_count;i++) {
   INFO(2, "recvcounts[%d]=%d displs[%d]=%d sums[%d]=%d offsets[%d]=%d ", i, recvcounts[i], i, displs[i], i, sums[i], i, offsets[i]);
}

   // Allocate a buffer large enough to receive all data
   buffer = malloc(sum * extent);

   if (buffer == NULL) {
      ERROR(1, "Failed to allocated space for local buffer! (comm=%d)", c->number);
      return MPI_ERR_INTERN;
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

               error = PMPI_Recv(buffer + (offset * extent), recvcounts[i], recvtype, get_local_rank(c, i), ALLGATHERV_TAG, c->comm, MPI_STATUS_IGNORE);

               if (error != MPI_SUCCESS) {
                  ERROR(1, "Failed to receive data from %d for gather! (comm=%d, error=%d)", i, c->number, error);
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
            error = messaging_bcast(buffer + (offsets[i]*extent), sums[i], recvtype, c->global_rank, c);

            if (error != MPI_SUCCESS) {
               ERROR(1, "Failed to send bcast from %d for gather! (comm=%d, error=%d)", c->global_rank, c->number, error);
               return error;
            }
         } else {
            error = messaging_bcast_receive(buffer + (offsets[i]*extent), sums[i], recvtype, c->coordinators[i], c);

            if (error != MPI_SUCCESS) {
               ERROR(1, "Failed to receive bcast on %d for gather! (comm=%d, error=%d)", c->global_rank, c->number, error);
               return error;
            }
         }
      }

   } else {

      // I am NOT a coordinator, so just send my data to the local coordinator.

      INFO(2, "LOCAL SEND: count=%d", sendcount);

      error = PMPI_Send(sendbuf, sendcount, sendtype, get_local_rank(c, c->my_coordinator), ALLGATHERV_TAG, c->comm);

      if (error != MPI_SUCCESS) {
         ERROR(1, "Failed to send data from %d to local root for gatherv! (comm=%d, error=%d)", c->global_rank, c->number, error);
         return error;
      }

   }

   // Bcast the resulting data locally
   error = PMPI_Bcast(buffer, sum, recvtype, get_local_rank(c, c->my_coordinator), c->comm);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Local broadcast of result failed! (comm=%d, error=%d)", c->number, error);
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
   return MPI_SUCCESS;
}


static int WA_Scatterv(void *sendbuf, int *sendcounts, int *displs, MPI_Datatype sendtype,
                       void *recvbuf, int recvcount, MPI_Datatype recvtype,
                       int root, communicator *c)
{
   int i, error;
   MPI_Aint extent;

   // FIXME: can be optimized by message combining and async sends!

   // We implement a WA Scatterv using simple send/receive primitives
   if (c->global_rank == root) {

      // First retrieve the data element size
      // FIXME: use size?
      error = PMPI_Type_extent(sendtype, &extent);

      if (error != MPI_SUCCESS) {
         ERROR(1, "Failed to retrieve data size! (comm=%d, error=%d)", c->number, error);
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

            if (error != MPI_SUCCESS) {
               ERROR(1, "WA_Scatterv: Root %d (in cluster %d) failed to send data to %d (in cluster %d) (in communicator %d)!\n", root, get_cluster_rank(c, root), i, get_cluster_rank(c, i), c->number);
               return error;
            }
         }
      }

   } else {

//INFO(1, "DO receive from %d", root);

      error = do_recv(recvbuf, recvcount, recvtype, root, SCATTERV_TAG, MPI_STATUS_IGNORE, c);

//INFO(1, "DONE receive from %d", root);

      if (error != MPI_SUCCESS) {
         ERROR(1, "WA_Scatterv: Process %d (in cluster %d) failed to receive data from root %d (in cluster %d) (in communicator %d)!\n", c->global_rank, cluster_rank, root, get_cluster_rank(c, root), c->number);
         return error;
      }
   }

   return MPI_SUCCESS;
}


#define __IMPI_Scatter
int IMPI_Scatter(void* sendbuf, int sendcount, MPI_Datatype sendtype,
                 void* recvbuf, int recvcount, MPI_Datatype recvtype,
                 int root, MPI_Comm comm)
{
   int i, error;
   int *displs = NULL;
   int *sendcounts = NULL;

   communicator *c = get_communicator(comm);

   if (comm_is_local(c)) {

//INFO(1, "local scatter");

     // simply perform a scatter in local cluster
     return PMPI_Scatter(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, root, c->comm);
   }

//INFO(1, "WA scatter");

   // We implement a WA Scatter using the WA Scatterv
   if (c->global_rank == root) {

      displs = malloc(c->global_size * sizeof(int));

      if (displs == NULL) {
         ERROR(1, "Failed to allocate buffer! (comm=%d)", c->number);
         return MPI_ERR_INTERN;
      }

      sendcounts = malloc(c->global_size * sizeof(int));

      if (sendcounts == NULL) {
         ERROR(1, "Failed to allocate buffer! (comm=%d)", c->number);
         free(displs);
         return MPI_ERR_INTERN;
      }

      for (i=0;i<c->global_size;i++) {
         sendcounts[i] = sendcount;
         displs[i] = sendcount * i;
      }
   }

   error = WA_Scatterv(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, c);

   if (c->global_rank == root) {
      free(displs);
      free(sendcounts);
   }

   return error;
}

#define __IMPI_Scatterv
int IMPI_Scatterv(void* sendbuf, int *sendcounts, int *displs, MPI_Datatype sendtype,
                  void* recvbuf, int recvcount, MPI_Datatype recvtype,
                  int root, MPI_Comm comm)
{
   communicator *c = get_communicator(comm);

   if (comm_is_local(c)) {
     // simply perform a scatterv in local cluster
     return PMPI_Scatterv(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, c->comm);
   }

   return WA_Scatterv(sendbuf, sendcounts, displs, sendtype, recvbuf, recvcount, recvtype, root, c);
}

#define __IMPI_Reduce
int IMPI_Reduce(void* sendbuf, void* recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm)
{
   int local_root, root_in_cluster, i, error;
   unsigned char *buffer = NULL;
   MPI_Aint extent;

   communicator *c = get_communicator(comm);

   operation *o = get_operation(op);

   if (o == NULL) {
      ERROR(1, "Operation not found!");
      return MPI_ERR_OP;
   }

   if (comm_is_local(c)) {

INFO(2, "Local reduce");

     // simply perform a reduce in local cluster
     return PMPI_Reduce(sendbuf, recvbuf, count, datatype, o->op, root, c->comm);
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

      // FIXME: use size?
      error = PMPI_Type_extent(datatype, &extent);

      if (error != MPI_SUCCESS) {
         ERROR(1, "Failed to retrieve data size for reduce! (comm=%d, error=%d)", c->number, error);
         return error;
      }

      buffer = malloc(count * extent);

      if (buffer == NULL) {
         ERROR(1, " Failed to allocate buffer space for WA reduce! (comm=%d, error=%d)", c->number, error);
         return MPI_ERR_INTERN;
      }
   } else {
INFO(2, "I am NOT root NOR coordinator (rank=%d root=%d coordinator=%d)", c->global_rank, root, c->my_coordinator);
   }

//INFO(1, "JASON IMPI_REDUCE", "START LOCAL REDUCE root=%d lroot=%d grank=%d lrank=%d count=%d sbuf[0]=%d rbuf[0]=%d\n",
//                       root, local_root, c->global_rank, c->local_rank, count, ((int *)sendbuf)[0], ((int *)recvbuf)[0]);


INFO(2, "Starting local reduce sendbuf=%p buffer=%p count=%d root=%d get_local_rank(root)=%d",
                  sendbuf, buffer, count, local_root, get_local_rank(c, local_root));

   error = PMPI_Reduce(sendbuf, buffer, count, datatype, o->op, get_local_rank(c, local_root), c->comm);

INFO(2, "Local reduce OK");

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to perform local reduce in communicator! (comm=%d, error=%d)", c->number, error);
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

            error = messaging_receive(buffer, count, datatype, c->coordinators[i], REDUCE_TAG, MPI_STATUS_IGNORE, c);

            if (error != MPI_SUCCESS) {
               ERROR(1, "Root %d failed to receive local reduce result from coordinator %d! (comm=%d, error=%d)",
		        c->global_rank, c->coordinators[i], c->number, error);
               return error;
            }

            // FIXME: no error checking here ??
            (*(o->function))((void*)buffer, recvbuf, &count, &datatype);
         }
      }
   } else if (root_in_cluster == 0 && c->global_rank == c->my_coordinator) {
      // The local coordinator now sends the partial result to the global root.

INFO(2, "Sending LOCAL result to REMOTE root=%d", root);

      error = messaging_send(buffer, count, datatype, root, REDUCE_TAG, c);

      if (error != MPI_SUCCESS) {
         ERROR(1, "Local coordinator %d failed to send local reduce result to root! (comm=%d, error=%d)",
		c->global_rank, c->number, error);
         return error;
      }
   }

INFO(2, "MPI_Reduce DONE");

   return MPI_SUCCESS;
}

#define __IMPI_Accumulate
int IMPI_Accumulate (void *origin_addr, int origin_count, MPI_Datatype origin_datatype,
                     int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype,
                     MPI_Op op, MPI_Win win)
{
   operation *o = get_operation(op);

   if (o == NULL) {
      ERROR(1, "Operation not found!");
      return MPI_ERR_OP;
   }

   return PMPI_Accumulate(origin_addr, origin_count, origin_datatype,
                          target_rank, target_disp, target_count, target_datatype,
                          o->op, win);
}

#define __IMPI_Allreduce
int IMPI_Allreduce(void* sendbuf, void* recvbuf, int count,
                         MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
   // FIXME: Assumes operation is commutative!
   int error, i;
   MPI_Aint extent;
   char *buffer;

   communicator *c = get_communicator(comm);

   operation *o = get_operation(op);

   if (o == NULL) {
      ERROR(1, "Operation not found!");
      return MPI_ERR_OP;
   }

   if (comm_is_local(c)) {
     // simply perform an allreduce in local cluster
     return PMPI_Allreduce(sendbuf, recvbuf, count, datatype, o->op, c->comm);
   }

   // We need to perform a WA Allreduce. We do this by performing a reduce
   // to our local cluster coordinator. This result is then broadcast to the
   // other cluster coordinators, which merge the results locally. The result
   // of this local merge is then broadcast in each local cluster.
   // NOTE: this does assume the operation is commutative!

//   INFO(1, "START LOCAL REDUCE grank=%d lrank=%d count=%d sbuf[1]=%d rbuf[1]=%d\n",
//                       c->global_rank, c->local_rank, count, ((int *)sendbuf)[1], ((int *)recvbuf)[1]);

   error = PMPI_Reduce(sendbuf, recvbuf, count, datatype, o->op, get_local_rank(c, c->my_coordinator), c->comm);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to perform local allreduce! (comm=%d, error=%d)", c->number, error);
      return error;
   }

//   INFO(1, "RESULT LOCAL REDUCE grank=%d lrank=%d count=%d sbuf[1]=%d rbuf[1]=%d\n",
//                        c->global_rank, c->local_rank, count, ((int *)sendbuf)[1], ((int *)recvbuf)[1]);

   // The local cluster coordinator shares the result with all other cluster coordinators.

   if (c->global_rank == c->my_coordinator) {

//  INFO(1, "JASON ALLREDUCE WA", "LOCAL ROOT!\n");

      // FIXME: use size?
      error = PMPI_Type_extent(datatype, &extent);

      if (error != MPI_SUCCESS) {
         ERROR(1, "Failed to retrieve data size for allreduce! (comm=%d, error=%d)", c->number, error);
         return error;
      }

      buffer = malloc(count * extent);

      if (buffer == NULL) {
         ERROR(1, "Failed to allocate buffer space for WA Allreduce! (comm=%d, error=%d)", c->number, error);
         return MPI_ERR_INTERN;
      }

//  INFO(1, "JASON ALLREDUCE WA", "FIXME: WA BCAST with CRAP performance!!\n");

      // FIXME: If this BCAST blocks then we're dead!
//      INFO(1, "WA BAST SEND grank=%d lrank=%d count=%d buf[1]=%d\n", c->global_rank, c->local_rank, count, ((int*)recvbuf)[1]);
      error = messaging_bcast(recvbuf, count, datatype, c->global_rank, c);

      if (error != MPI_SUCCESS) {
         ERROR(1, "Local root %d failed to bcast local allreduce result! (comm=%d, error=%d)", c->global_rank, c->number, error);
         return error;
      }

      for (i=0;i<c->cluster_count;i++) {

         if (c->coordinators[i] != c->global_rank) {

//        INFO(1, "WA BAST RECV i=%d grank=%d lrank=%d count=%d from=%d\n", i, c->global_rank, c->local_rank, count, c->coordinators[i]);

            error = messaging_bcast_receive(buffer, count, datatype, c->coordinators[i], c);

            if (error != MPI_SUCCESS) {
               ERROR(1, "Local root %d failed to bcast local allreduce result! (comm=%d, error=%d)", c->global_rank, c->number, error);
               return error;
            }

//        INFO(1, "CALLING REDUCE OP buf[1]=%d revcbuf[1]=%d count=%d\n", ((int *)buffer)[1], ((int *)recvbuf)[1], count);

            (*(o->function))((void*)buffer, recvbuf, &count, &datatype);

//        INFO(1, "RESULT REDUCE OP buf[1]=%d revcbuf[1]=%d count=%d\n", ((int *)buffer)[1], ((int *)recvbuf)[1], count);
         }
      }
   }

//   INFO(1, "LOCAL BAST grank=%d lrank=%d count=%d buf[1]=%d\n", c->global_rank, c->local_rank, count, ((int*)recvbuf)[1]);

   return PMPI_Bcast(recvbuf, count, datatype, 0, c->comm);
}

#define __IMPI_Scan
int IMPI_Scan(void* sendbuf, void* recvbuf, int count,
              MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
   int i, tmp_cluster, error;
   MPI_Aint extent;
   unsigned char *buffer;

   communicator *c = get_communicator(comm);
   operation *o = get_operation(op);

   if (o == NULL) {
      ERROR(1, "Operation not found!");
      return MPI_ERR_OP;
   }

   if (comm_is_local(c)) {
     // simply perform a scan in local cluster
     return PMPI_Scan(sendbuf, recvbuf, count, datatype, o->op, c->comm);
   }

   // We implement a WA Scan using simple send/receive primitives

   // First retrieve the data element size
   // FIXME: use size?
   error = PMPI_Type_extent(datatype, &extent);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to retrieve send data size! (comm=%d, error=%d)", c->number, error);
      return error;
   }

   // Allocate a temp buffer.
   buffer = malloc(count * extent);

   if (buffer == NULL) {
      ERROR(1, "Failed to allocate temporary buffer! (comm=%d, error=%d)", c->number, error);
      return error;
   }

   // Step 1: copy send buffer to receive buffer.
   memcpy(recvbuf, sendbuf, count * extent);

   // Step 2: loop over all ranks, and send/(receive+reduce) data where needed.
   for (i=0;i<c->global_size;i++) {

      tmp_cluster = GET_CLUSTER_RANK(c->members[i]);

      if (i > c->global_rank) {
         error = do_send(sendbuf, count, datatype, i, SCAN_TAG, c);

         if (error != MPI_SUCCESS) {
            ERROR(1, "Rank %d (in cluster %d) failed to send data to %d (in cluster %d)! (comm=%d, error=%d)", c->global_rank, cluster_rank, i, tmp_cluster, c->number, error);
            return error;
         }

      } else if (i < c->global_rank) {

         // Must receive from i.
         error = do_recv(buffer, count, datatype, i, SCAN_TAG, MPI_STATUS_IGNORE, c);

         if (error != MPI_SUCCESS) {
            ERROR(1, "Rank %d (in cluster %d) failed to receive data from %d (in cluster %d)! (comm=%d, error=%d)", c->global_rank, cluster_rank, i, tmp_cluster, c->number, error);
            return error;
         }

         // FIXME: no error handling ?
         (*(o->function))((void*)buffer, recvbuf, &count, &datatype);
      }
   }

   free(buffer);
   return MPI_SUCCESS;
}

static int WA_Alltoallv(void *sendbuf, int *sendcounts, int *sdispls, MPI_Datatype sendtype,
                        void *recvbuf, int *recvcounts, int *rdispls, MPI_Datatype recvtype,
                        communicator *c)
{
   int i, j, error;
   MPI_Aint sextent, rextent;

   // We implement a WA Alltoallv using simple send/receive primitives

   // First retrieve the data element size
   // FIXME: use size?
   error = PMPI_Type_extent(sendtype, &sextent);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to retrieve send data size! (comm=%d, error=%d)", c->number, error);
      return error;
   }

   // First retrieve the data element size
   // FIXME: use size?
   error = PMPI_Type_extent(recvtype, &rextent);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to retrieve receive data size! (comm=%d, error=%d)", c->number, error);
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
               error = do_recv(recvbuf + (rextent * rdispls[j]), recvcounts[j], recvtype, j, ALLTOALLV_TAG, MPI_STATUS_IGNORE, c);

               if (error != MPI_SUCCESS) {
                  ERROR(1, "Rank %d (in cluster %d) failed to receive data from %d (in cluster %d)! (comm=%d, error=%d)", c->global_rank, cluster_rank, j, get_cluster_rank(c, j), c->number, error);
                  return error;
               }
            }
         }
      } else {
         // We should send to one other.

INFO(1, "send index=%d displ=%d, count=%d", i, sdispls[i], sendcounts[i]);

         error = do_send(sendbuf + (sdispls[i] * sextent), sendcounts[i], sendtype, i, ALLTOALLV_TAG, c);

         if (error != MPI_SUCCESS) {
            ERROR(1, "Rank %d (in cluster %d) failed to send data to %d (in cluster %d) (comm=%d, error=%d)", c->global_rank, cluster_rank, i, get_cluster_rank(c, i), c->number, error);
            return error;
         }
      }
   }

   return MPI_SUCCESS;
}

#define __IMPI_Alltoall
int IMPI_Alltoall(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                  void *recvbuf, int recvcount, MPI_Datatype recvtype,
                  MPI_Comm comm)
{
   int i, error;
   int *senddispls;
   int *sendcounts;
   int *recvdispls;
   int *recvcounts;

   communicator *c = get_communicator(comm);

   if (comm_is_local(c)) {
     // simply perform an all-to-all in local cluster
     return PMPI_Alltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, c->comm);
   }

   // We implement a WA Alltoall using send/receive
   senddispls = malloc(c->global_size * sizeof(int));

   if (senddispls == NULL) {
      ERROR(1, "Failed to allocate buffer! (comm=%d)", c->number);
      return MPI_ERR_INTERN;
   }

   sendcounts = malloc(c->global_size * sizeof(int));

   if (sendcounts == NULL) {
      ERROR(1, "Failed to allocate buffer! (comm=%d)", c->number);
      free(senddispls);
      return MPI_ERR_INTERN;
   }

   recvdispls = malloc(c->global_size * sizeof(int));

   if (recvdispls == NULL) {
      ERROR(1, "Failed to allocate buffer! (comm=%d)", c->number);
      free(senddispls);
      free(sendcounts);
      return MPI_ERR_INTERN;
   }

   recvcounts = malloc(c->global_size * sizeof(int));

   if (recvcounts == NULL) {
      ERROR(1, "Failed to allocate buffer! (comm=%d)", c->number);
      free(senddispls);
      free(sendcounts);
      free(recvdispls);
      return MPI_ERR_INTERN;
   }

   for (i=0;i<c->global_size;i++) {
      sendcounts[i] = sendcount;
      senddispls[i] = sendcount * i;
      recvcounts[i] = recvcount;
      recvdispls[i] = recvcount * i;
   }

   error = WA_Alltoallv(sendbuf, sendcounts, senddispls, sendtype,
                       recvbuf, recvcounts, recvdispls, recvtype, c);

   free(senddispls);
   free(sendcounts);
   free(recvdispls);
   free(recvcounts);

   return error;
}

#define __IMPI_Alltoallv
int IMPI_Alltoallv(void *sendbuf, int *sendcounts, int *sdispls, MPI_Datatype sendtype,
                   void *recvbuf, int *recvcounts, int *rdispls, MPI_Datatype recvtype,
                   MPI_Comm comm)
{
   communicator *c = get_communicator(comm);

   if (comm_is_local(c)) {
     // simply perform an all-to-all in local cluster
     return PMPI_Alltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, c->comm);
   }

   return WA_Alltoallv(sendbuf, sendcounts, sdispls, sendtype, recvbuf, recvcounts, rdispls, recvtype, c);
}

/*****************************************************************************/
/*                         Communicators and Groups                          */
/*****************************************************************************/

#define __IMPI_Comm_size
int IMPI_Comm_size(MPI_Comm comm, int *size)
{
   communicator *c = get_communicator(comm);

//   INFO(1, "MPI_Comm_size", "Retrieve size from %d: local(%d %d) | global(%d %d)", c->number, c->local_rank, c->local_size, c->global_rank, c->global_size);

   *size = c->global_size;

   return MPI_SUCCESS;
}

#define __IMPI_Comm_rank
int IMPI_Comm_rank(MPI_Comm comm, int *rank)
{
   communicator *c = get_communicator(comm);

//   INFO(1, "MPI_Comm_rank", "Retrieve rank from %d: local(%d %d) | global(%d %d)", c->number, c->local_rank, c->local_size, c->global_rank, c->global_size);

   *rank = c->global_rank;

   return MPI_SUCCESS;
}

static int *copy_int_array(int *src, int size)
{
   int *tmp = malloc(size * sizeof(int));

   if (tmp == NULL) {
      return NULL;
   }

   return memcpy(tmp, src, size * sizeof(int));
}

#define __IMPI_Comm_dup
int IMPI_Comm_dup(MPI_Comm comm, MPI_Comm *newcomm)
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

   communicator *c = get_communicator(comm);

   error = messaging_send_dup_request(c);

   if (error != MPI_SUCCESS) {
      IERROR(1, "MPI_Comm_dup send failed! (comm=%d, error=%d)", c->number, error);
      return error;
   }

   error = messaging_receive_dup_reply(&reply);

   if (error != MPI_SUCCESS) {
      IERROR(1, "MPI_Comm_dup receive failed! (comm=%d, error=%d)", c->number, error);
      return error;
   }

   error = PMPI_Comm_dup(c->comm, &tmp_com);

   if (error != MPI_SUCCESS) {
      IERROR(1, "MPI_Comm_dup local dup failed! (comm=%d, error=%d)", c->number, error);
      return error;
   }

   members = (uint32_t *) copy_int_array((int *)c->members, c->global_size);

   if (members == NULL) {
      IERROR(1, "MPI_Comm_dup member copy failed! (comm=%d)", c->number);
      return error;
   }

   coordinators = copy_int_array(c->coordinators, c->cluster_count);

   if (coordinators == NULL) {
      IERROR(1, "MPI_Comm_dup coordinator copy failed! (comm=%d)", c->number);
      free(members);
      return error;
   }

   cluster_sizes = copy_int_array(c->cluster_sizes, c->cluster_count);

   if (cluster_sizes == NULL) {
      IERROR(1, "MPI_Comm_dup cluster_sizes copy failed! (comm=%d)", c->number);
      free(members);
      free(coordinators);
      return error;
   }

   cluster_ranks = copy_int_array(c->cluster_ranks, c->cluster_count);

   if (cluster_ranks == NULL) {
      IERROR(1, "MPI_Comm_dup cluster_ranks copy failed! (comm=%d)", c->number);
      free(members);
      free(coordinators);
      free(cluster_sizes);
      return error;
   }

   member_cluster_index = (uint32_t *) copy_int_array((int*)c->member_cluster_index, c->global_size);

   if (member_cluster_index == NULL) {
      IERROR(1, "MPI_Comm_dup member_cluster_index copy failed! (comm=%d)", c->number);
      free(members);
      free(coordinators);
      free(cluster_sizes);
      free(cluster_ranks);
      return error;
   }

   local_ranks = (uint32_t *) copy_int_array((int*)c->local_ranks, c->global_size);

   if (local_ranks == NULL) {
      IERROR(1, "MPI_Comm_dup local_ranks copy failed! (comm=%d)", c->number);
      free(members);
      free(coordinators);
      free(cluster_sizes);
      free(cluster_ranks);
      free(member_cluster_index);
      return error;
   }

   error = create_communicator(tmp_com, reply.newComm,
                 c->local_rank, c->local_size,
                 c->global_rank, c->global_size,
                 c->cluster_count, coordinators, cluster_sizes,
                 c->flags, members,
                 cluster_ranks, member_cluster_index, local_ranks,
                 &dup);

   if (error != MPI_SUCCESS) {
      IERROR(1, "MPI_Comm_dup create communicator failed! (comm=%d, error=%d)", c->number, error);
      return error;
   }

   set_communicator_ptr(newcomm, dup);

   STACKTRACE(0, "COMM_DUP %d -> %d", c->number, dup->number);

   return MPI_SUCCESS;
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
      IERROR(1, "Failed to allocate memory for local group members! (comm=%d)", c->number);
      return MPI_ERR_INTERN;
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
      return MPI_SUCCESS;
   }

   // If local_count > 0 we need to create a new (local) communicator.
   // Since we known the local ranks relative to WORLD, we'll use that
   // communicator as a basis.

   world = get_communicator_with_index(FORTRAN_MPI_COMM_WORLD);

   error = PMPI_Comm_group(world->comm, &orig_group);

   if (error != MPI_SUCCESS) {
      IERROR(1, "Failed to split local group! (comm=%d, error=%d)", c->number, error);
      free(local_members);
      return error;
   }

   error = PMPI_Group_incl(orig_group, local_count, local_members, &new_group);

   free(local_members);

   if (error != MPI_SUCCESS) {
      IERROR(1, "Failed to perform local group include! (comm=%d, error=%d)", c->number, error);
      return error;
   }

   // Check if we are part of the new communicator.
   if (new_group == MPI_GROUP_NULL) {
      *newcomm = MPI_COMM_NULL;
      return MPI_SUCCESS;
   }

   // HACK: we use the local communicator to create the new one here ?
   error = PMPI_Comm_create(c->comm, new_group, newcomm);

   if (error != MPI_SUCCESS) {
      IERROR(1, "Failed to create local communicator! (comm=%d, error=%d)", c->number, error);
      return error;
   }

   return MPI_SUCCESS;
}

#define __IMPI_Comm_create
int IMPI_Comm_create(MPI_Comm mc, MPI_Group mg, MPI_Comm *newcomm)
{
   int error, local_rank, local_size;
   group_reply reply;
   MPI_Comm tmp_comm;
   communicator *result;

   // Retrieve our communicator.
   communicator *c = get_communicator(mc);

   // Retrieve our group.
   group *g = get_group(mg);

   if (g == NULL) {
      ERROR(1, "Group not found! (comm=%d)", c->number);
      return MPI_ERR_GROUP;
   }

   // Request the create at the server
   error = messaging_send_group_request(c, g);

   if (error != MPI_SUCCESS) {
      IERROR(1, "Failed to send comm_create! (comm=%d, error=%d)", c->number, error);
      return error;
   }

   // Wait for the reply.
   error = messaging_receive_group_reply(&reply);

   if (error != MPI_SUCCESS) {
      IERROR(1, "Failed to receive comm_create! (comm=%d, error=%d)", c->number, error);
      return error;
   }

   // If the type of the new communicator is GROUP_TYPE_IDLE we can simply return MPI_COMM_NULL.
   if (reply.type == GROUP_TYPE_IDLE) {
      *newcomm = MPI_COMM_NULL;
      return MPI_SUCCESS;
   }

   // Otherwise, we may have to create a local communicator.
   error = local_comm_create(c, g, &tmp_comm);

   if (error != MPI_SUCCESS) {
      IERROR(1, "Failed to create local communicator! (comm=%d, error=%d)", c->number, error);
      return error;
   }

   // Create the communicator if necessary.
   if (tmp_comm != MPI_COMM_NULL) {
      PMPI_Comm_rank(tmp_comm, &local_rank);
      PMPI_Comm_size(tmp_comm, &local_size);

      error = create_communicator(tmp_comm, reply.newComm,
                 local_rank, local_size, reply.rank, reply.size,
                 reply.cluster_count, reply.coordinators, reply.cluster_sizes,
                 reply.flags, reply.members,
                 reply.cluster_ranks, reply.member_cluster_index, reply.local_ranks,
                 &result);

      if (error != MPI_SUCCESS) {
         IERROR(1, "Failed to create new communicator! (comm=%d, error=%d)", c->number, error);
         return error;
      }

      set_communicator_ptr(newcomm, result);

   } else {
      *newcomm = MPI_COMM_NULL;
   }

   STACKTRACE(0, "COMM_CREATE %d -> %d", c->number, result->number);

   return MPI_SUCCESS;
}

#define __IMPI_Comm_split
int IMPI_Comm_split(MPI_Comm comm, int color, int key, MPI_Comm *newcomm)
{
   int error, local_rank, local_size;
   MPI_Comm tmp;
   comm_reply reply;
   communicator *result;

   // We first forward the split request to the server to split the
   // virtual communicator.
   communicator *c = get_communicator(comm);

   // Translate the color from MPI_UNDEFINED to -1 if needed.
   if (color == MPI_UNDEFINED) {
      color = -1;
      key = -1;
   }

   error = messaging_send_comm_request(c, color, key);

   if (error != MPI_SUCCESS) {
      IERROR(1, "Failed to send comm_split! (comm=%d, error=%d)", c->number, error);
      return error;
   }

   error = messaging_receive_comm_reply(&reply);

   if (error != MPI_SUCCESS) {
      IERROR(1, "Failed to receive comm_split! (comm=%d, error=%d)", c->number, error);
      return error;
   }

   // The reply from the server will tell us how to split the
   // local communicator. Note that we may get a -1 as a color
   // which we need to translate to an MPI_UNDEFINED first.
   if (reply.color < 0) {
      reply.color = MPI_UNDEFINED;
      reply.key = 0;
   }

   tmp = MPI_COMM_NULL;

   error = PMPI_Comm_split(c->comm, reply.color, reply.key, &tmp);

   if (error != MPI_SUCCESS) {
      IERROR(1, "Failed to perform local comm_split! (comm=%d, error=%d)", c->number, error);
      return error;
   }

   // If a new communicator was returned (that is, color != -1) we
   // have to register the new virtual communicator locally.
   if (tmp != MPI_COMM_NULL) {
      PMPI_Comm_rank(tmp, &local_rank);
      PMPI_Comm_size(tmp, &local_size);

      error = create_communicator(tmp, reply.newComm,
                 local_rank, local_size, reply.rank, reply.size,
                 reply.cluster_count, reply.coordinators, reply.cluster_sizes,
                 reply.flags, reply.members,
                 reply.cluster_ranks, reply.member_cluster_index, reply.local_ranks,
                 &result);

      if (error != MPI_SUCCESS) {
         IERROR(1, "Failed to create new communicator! (comm=%d, error=%d)", c->number, error);
         return error;
      }

      set_communicator_ptr(newcomm, result);
   } else {
      *newcomm = MPI_COMM_NULL;
   }

   STACKTRACE(0, "COMM_SPLIT %d -> %d", c->number, result->number);

   return MPI_SUCCESS;
}

#define __IMPI_Comm_group
int IMPI_Comm_group(MPI_Comm comm, MPI_Group *g)
{
   int error;
   group *res;

   communicator *c = get_communicator(comm);

   error = group_comm_group(c, &res);

   if (error == MPI_SUCCESS) {
      set_group_ptr(g, res);
   } else {
      ERROR(1, "Failed to create group! (comm=%d, error=%d)", c->number, error);
   }

   return error;
}

#define __IMPI_Comm_free
int IMPI_Comm_free ( MPI_Comm *comm )
{
   if (*comm == MPI_COMM_WORLD) {
      // ignored
      return MPI_SUCCESS;
   }

   if (*comm == MPI_COMM_NULL) {
      ERROR(1, "Communicator is MPI_COMM_NULL!");
      return MPI_ERR_COMM;
   }

   communicator *c = get_communicator(*comm);

/*
   Ignored for now, as the spec implies that this is an asynchronous operation!

   error = free_communicator(c);
   comm = MPI_COMM_NULL;
   return error;
*/

   WARN(1, "Ignoring MPI_Comm_free on communicator %d!\n", c->number);

   *comm = MPI_COMM_NULL;

   return MPI_SUCCESS;
}

#define __IMPI_Group_rank
int IMPI_Group_rank(MPI_Group g, int *rank)
{
   return group_rank(get_group(g), rank);
}

#define __IMPI_Group_size
int IMPI_Group_size(MPI_Group g, int *size)
{
   return group_size(get_group(g), size);
}

#define __IMPI_Group_incl
int IMPI_Group_incl(MPI_Group g, int n, int *ranks, MPI_Group *newgroup)
{
   group *res;

   int error = group_incl(get_group(g), n, ranks, &res);

   if (error == MPI_SUCCESS) {
      set_group_ptr(newgroup, res);
   } else {
      ERROR(1, "Failed to include group!");
   }

   return error;
}

#define __IMPI_Group_range_incl
int IMPI_Group_range_incl(MPI_Group g, int n, int ranges[][3], MPI_Group *newgroup)
{
   group *res;

   int error = group_range_incl(get_group(g), n, ranges, &res);

   if (error == MPI_SUCCESS) {
      set_group_ptr(newgroup, res);
   } else {
      ERROR(1, "Failed to include group range!");
   }

   return error;
}

#define __IMPI_Group_union
int IMPI_Group_union(MPI_Group group1, MPI_Group group2, MPI_Group *newgroup)
{
   int error;
   group *in1;
   group *in2;
   group *out;

   in1 = get_group(group1);
   in2 = get_group(group2);

   if (in1 == NULL || in2 == NULL) {
      ERROR(1, "Failed to find group!");
      return MPI_ERR_GROUP;
   }

   error = group_union(in1, in2, &out);

   if (error == MPI_SUCCESS) {
      set_group_ptr(newgroup, out);
   } else {
      ERROR(1, "Failed to perform group union!");
   }

   return error;
}

#define __IMPI_Group_translate_ranks
int IMPI_Group_translate_ranks(MPI_Group group1, int n, int *ranks1,
                                     MPI_Group group2, int *ranks2)
{
   int i, j, pid, rank;
   group *in1;
   group *in2;

   in1 = get_group(group1);
   in2 = get_group(group2);

//INFO(1, "IMPI_Group_translate_ranks DEBUG", "group1 %d group2 %d", in1->size, in2->size);

   if (in1 == NULL || in2 == NULL) {
      ERROR(1, "Group not found!");
      return MPI_ERR_GROUP;
   }

   for (i=0;i<n;i++) {

      rank = ranks1[i];

      if (rank < 0 || rank >= in1->size) {
         ERROR(1, "Incorrect rank!");
         return MPI_ERR_RANK;
      }

      pid = in1->members[rank];

//INFO(1, "IMPI_Group_translate_ranks DEBUG", "rank %d pid %d", rank, pid);

      ranks2[i] = MPI_UNDEFINED;

      for (j=0;i<in2->size;j++) {
         if (in2->members[j] == pid) {

//INFO(1, "IMPI_Group_translate_ranks DEBUG", "found pid %d at %d", pid, j);

            ranks2[i] = j;
            break;
         }
      }
   }

   return MPI_SUCCESS;
}

/*****************************************************************************/
/*                                Utilities                                  */
/*****************************************************************************/

/* ========== C <-> Fortran conversions ============ */

#define __IMPI_Comm_f2c
MPI_Comm IMPI_Comm_f2c(MPI_Fint comm)
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

   communicator *tmp = get_communicator_with_index(comm);
   set_communicator_ptr(&res, tmp);
   return res;
}

#define __IMPI_Comm_c2f
MPI_Fint IMPI_Comm_c2f(MPI_Comm comm)
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

   communicator *tmp = get_communicator(comm);
   return tmp->number;
}

#define __IMPI_Group_f2c
MPI_Group IMPI_Group_f2c(MPI_Fint g)
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

#define __IMPI_Group_c2f
MPI_Fint IMPI_Group_c2f(MPI_Group g)
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

#define __IMPI_Request_f2c
MPI_Request IMPI_Request_f2c(MPI_Fint req)
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

#define __IMPI_Request_c2f
MPI_Fint IMPI_Request_c2f(MPI_Request req)
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

/*
// We don't need these if we don't support user defines ops yet!

#define __IMPI_Op_f2c
MPI_Op IMPI_Op_f2c(MPI_Fint op)
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


#define __IMPI_Op_c2f
MPI_Fint IMPI_Op_c2f(MPI_Op op) 
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
#include "generated_impi.c"

#endif // IBIS_INTERCEPT




