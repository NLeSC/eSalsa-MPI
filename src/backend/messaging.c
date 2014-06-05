#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/epoll.h>

#include <errno.h>
#include <limits.h>

#include "settings.h"

#include "util.h"
#include "empi.h"
#include "error.h"
#include "datatype.h"
#include "status.h"
#include "types.h"
#include "messaging.h"
#include "communicator.h"
#include "group.h"
#include "request.h"
#include "logging.h"
#include "gateway.h"
#include "opcodes.h"

// TODO: Cleanup these messages. They always have the comm and src fields!

// This message is used to request a comm-split at the server.
typedef struct {
    message_header header;
    int comm;    // communicator used
    int src;     // rank in current communicator
    int color;   // target new communicator
    int key;     // prefered rank in target communicator
} split_request_msg;

#define SPLIT_REQUEST_MSG_SIZE (MESSAGE_HEADER_SIZE + 4*sizeof(int))

// This message is used to request a comm-from-froup creation at the server.
typedef struct {
    message_header header;
    int comm;    // communicator used
    int src;     // rank in current communicator
    int size;    // number of ranks in group
    int members[]; // member ranks for group
} group_request_msg;

#define GROUP_REQUEST_MSG_SIZE (MESSAGE_HEADER_SIZE + 3*sizeof(int))

// This message is used to request a comm-dup at the server.
typedef struct {
    message_header header;
    int comm;    // communicator used
    int src;     // rank in current communicator
} dup_request_msg;

#define DUP_REQUEST_MSG_SIZE (MESSAGE_HEADER_SIZE + 2*sizeof(int))

// This message is used to request a comm-free at the server.
typedef struct {
    message_header header;
    int comm;    // communicator used
    int src;     // rank in current communicator
} free_request_msg;

#define FREE_REQUEST_MSG_SIZE (MESSAGE_HEADER_SIZE + 2*sizeof(int))

// This message is used to request an finalize at the server.
typedef struct {
    message_header header;
    int comm;    // communicator used
    int src;     // rank in current communicator
} finalize_request_msg;

#define FINALIZE_REQUEST_MSG_SIZE (MESSAGE_HEADER_SIZE + 2*sizeof(int))

// This the servers reply to a comm-split request.
typedef struct {
    message_header header;
//    int comm;           // communicator used
//    int src;            // source rank (unused)
    int newComm;        // communicator created
    int rank;           // rank in new communicator
    int size;           // size of new communicator
    int color;          // color for local split
    int key;            // key for local split
    int cluster_count;  // number of cluster in communicator
    int flags;          // flags of new communicator
    uint32_t payload[]; // Payload contains 3*cluster_count + 3*size values in this order:
                        //  cluster coordinations (cluster_count)
                        //  cluster sizes (cluster_count)
                        //  cluster ranks (cluster_count)
                        //  member pids   (size)
                        //  member cluster index (size)
                        //  local member ranks (size)
} split_reply_msg;

#define SPLIT_REPLY_MSG_SIZE (MESSAGE_HEADER_SIZE + 7*sizeof(int))

// This the servers reply to a group-to-comm request.
typedef struct {
    message_header header;
//    int comm;           // communicator used
//    int src;            // source rank (unused)
    int newComm;        // communicator created
    int rank;           // rank in new communicator
    int size;           // size of new communicator
    int type;           // type of group reply
    int cluster_count;  // number of clusters in communicator
    int flags;          // flags of new communicator
    uint32_t payload[]; // Payload contains 3*cluster_count + 3*size values in this order:
                        //  cluster coordinations (cluster_count)
                        //  cluster sizes (cluster_count)
                        //  cluster ranks (cluster_count)
                        //  member pids   (size)
                        //  member cluster index (size)
                        //  local member ranks (size)
} group_reply_msg;

#define GROUP_REPLY_MSG_SIZE (MESSAGE_HEADER_SIZE + 6*sizeof(int))

// This is the reply to a dup request.
typedef struct {
    message_header header;
//    int comm;    // communicator used
//    int src;     // source rank (unused)
    int newComm; // communicator created
} dup_reply_msg;

#define DUP_REPLY_MSG_SIZE (MESSAGE_HEADER_SIZE + 1*sizeof(int))

// This is the reply to a finalize request.
typedef struct {
    message_header header;
} finalize_reply_msg;

#define FINALIZE_REPLY_MSG_SIZE (MESSAGE_HEADER_SIZE)

// The number of clusters and the rank of our cluster in this set.
extern uint32_t cluster_count;
extern uint32_t cluster_rank;

// The number of application processes and gateways in this cluster.
extern uint32_t local_application_size;
extern uint32_t gateway_count;

// The gateway rank of this gateway process. -1 if process is application process.
extern int gateway_rank;

// The rank of the gateway process handling my outgoing traffic. Always -1 (invalid) if process is gateway process.
static int my_gateway;

// The rank of the gateway process handling all server traffic. Always 0 if process is gateway process.
static int server_gateway;

// The PID of this process. Used when forwarding messages.
uint32_t my_pid;

// The PID of the master gateway. Not valid on a gateway process.
uint32_t server_pid;

// The size of each cluster, and the offset of each cluster in the
// total set of machines.
extern int *cluster_sizes;
extern int *cluster_offsets;

static MPI_Comm mpi_comm_application_only;

// Unused?
extern MPI_Comm mpi_comm_gateways_only;
MPI_Comm mpi_comm_gateway_and_application;

/*****************************************************************************/
/*                      Initialization / Finalization                        */
/*****************************************************************************/

int messaging_init(int rank, int size, int *adjusted_rank, int *adjusted_size, MPI_Comm *world, int *argc, char ***argv)
{
   int status, error;

   cluster_count = 0;
   cluster_rank = -1;

   // The last MPI process needs to read the config file, connect to the server,
   //  and get info on cluster ranks and sizes of all the clusters.
   if (rank == size-1) {
      status = master_gateway_init(rank, size, argc, argv);
   } else {
      status = EMPI_SUCCESS;
   }

   error = PMPI_Bcast(&status, 1, MPI_INT, size-1, MPI_COMM_WORLD);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to broadcast init status! (error=%d)", error);
      return EMPI_ERR_INTERN;
   }

   if (status != EMPI_SUCCESS) {
      ERROR(1, "Master gateway failed to initialize! (error=%d)", status);
      return EMPI_ERR_INTERN;
   }

   error = PMPI_Bcast(&cluster_count, 1, MPI_INT, size-1, MPI_COMM_WORLD);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to broadcast cluster count! (error=%d)", error);
      return EMPI_ERR_INTERN;
   }

   error = PMPI_Bcast(&cluster_rank, 1, MPI_INT, size-1, MPI_COMM_WORLD);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to broadcast cluster rank! (error=%d)", error);
      return EMPI_ERR_INTERN;
   }

   error = PMPI_Bcast(&local_application_size, 1, MPI_INT, size-1, MPI_COMM_WORLD);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to broadcast local application size! (error=%d)", error);
      return EMPI_ERR_INTERN;
   }

   error = PMPI_Bcast(&gateway_count, 1, MPI_INT, size-1, MPI_COMM_WORLD);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to broadcast gateway count! (error=%d)", error);
      return EMPI_ERR_INTERN;
   }

   if (rank != size-1) {
      cluster_sizes = malloc(cluster_count * sizeof(int));
      cluster_offsets = malloc((cluster_count+1) * sizeof(int));

      if (cluster_sizes == NULL || cluster_offsets == NULL) {
         // TODO: tell others
         ERROR(1, "Failed to allocate space for cluster info!");
         return EMPI_ERR_INTERN;
      }
   }

   error = PMPI_Bcast(cluster_sizes, cluster_count, MPI_INT, size-1, MPI_COMM_WORLD);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to send broadcast of cluster sizes! (error=%d)", error);
      return EMPI_ERR_INTERN;
   }

   error = PMPI_Bcast(cluster_offsets, cluster_count+1, MPI_INT, size-1, MPI_COMM_WORLD);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to send broadcast of cluster offsets! (error=%d)", error);
      return EMPI_ERR_INTERN;
   }

   // Everyone in this cluster now has the following info:
   //
   // Number of clusters
   // Rank of this cluster
   // Number of application processes in this cluster
   // Number of gateway processes in each cluster
   // Size of each cluster.
   // Offset of each cluster.
   //
   // We should now create three new communicators:
   //
   // mpi_comm_application_only : contains only the application
   //                             processes of this cluster
   //
   // mpi_comm_gateway_only     : contains only the gateway
   //                             processes of this cluster
   //
   // mpi_comm_gateway_and_application: dup of MPI_COMM_WORLD
   //                                   used for messages between
   //                                   the application and
   //                                   gateway processes.

   error = PMPI_Comm_dup(MPI_COMM_WORLD, &mpi_comm_gateway_and_application);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to create communicator! (error=%d)", error);
      return EMPI_ERR_INTERN;
   }

   *adjusted_size = local_application_size;

   if (rank >= local_application_size) {
      // I am a gateway process
      mpi_comm_application_only = MPI_COMM_NULL;
      *adjusted_rank = -1;
      my_gateway = -1;
      server_gateway = 0;
      gateway_rank = size-(rank+1);

      INFO(1, "I am gateway process %d of %d in cluster %d of %d", gateway_rank, gateway_count, cluster_rank, cluster_count);

      error = PMPI_Comm_split(MPI_COMM_WORLD, 0, gateway_rank, &mpi_comm_gateways_only);

   } else {
      // I am an application process.
      mpi_comm_gateways_only = MPI_COMM_NULL;
      *adjusted_rank = rank;
      // NOTE: No clue if this is a efficient distribution!
      my_gateway = local_application_size + (rank % gateway_count);
      server_gateway = size-1;
      gateway_rank = -1;

      INFO(1, "I am application process %d of %d in cluster %d of %d, my gateway is %d", *adjusted_rank, *adjusted_size, cluster_rank, cluster_count, my_gateway);

      error = PMPI_Comm_split(MPI_COMM_WORLD, 1, rank, &mpi_comm_application_only);
   }

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to split MPI_COMM_WORLD communicator! (error=%d)", error);
      return EMPI_ERR_INTERN;
   }

   // Generate my PID.
   my_pid = SET_PID(cluster_rank, rank);

   // The server PID is all 1's
   server_pid = 0xFFFFFFFF;

   // Return the new COMM world.
   *world = mpi_comm_application_only;

   if (gateway_rank >= 0) {
      generic_gateway_init(rank, size);
   }

   // Perform a barrier here, to ensure that the application processes
   // do not start before the gateway(s) are initialized!
   error = PMPI_Barrier(MPI_COMM_WORLD);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed wait for gateway processes! (error=%d)", error);
      return EMPI_ERR_INTERN;
   }

   return EMPI_SUCCESS;
}

/*****************************************************************************/
/*                             Utility functions                             */
/*****************************************************************************/

int match_message(data_message *m, int comm, int source, int tag)
{
   int result = ((comm == m->comm) &&
                 (source == EMPI_ANY_SOURCE || source == m->source) &&
                 (tag == EMPI_ANY_TAG || tag == m->tag));

   DEBUG(5, "MATCH_MESSAGE: (comm=%d source=%d [any=%d] tag=%d [any=%d]) == (m.comm=%d m.source=%d m.tag=%d) => %d",
        comm, source, EMPI_ANY_SOURCE, tag, EMPI_ANY_TAG,
        m->comm, m->source, m->tag, result);

   return result;
}


/*****************************************************************************/
/*                         Application Communication                         */
/*****************************************************************************/

static data_message *create_data_message(int opcode, int dst_pid, int comm, int source, int dest, int tag, int count, int data_size)
{
   data_message *m;

   m = malloc(DATA_MESSAGE_SIZE + data_size);

   if (m == NULL) {
       ERROR(1, "Failed to allocate message buffer!\n");
       return NULL;
   }

   m->header.opcode = opcode;
   m->header.src_pid = my_pid;
   m->header.dst_pid = dst_pid;
   m->header.length = DATA_MESSAGE_SIZE + data_size;

   m->comm = comm;
   m->source = source;
   m->dest = dest;
   m->tag = tag;
   m->count = count;

   return m;
}

static void free_data_message(data_message *m)
{
   if (m == NULL) {
      return;
   }

   free(m);
}

// Unpack a message from a data_message;
static int unpack_message(void *buf, int count, datatype *t, communicator *c, data_message *m, EMPI_Status *s)
{
   int error = 0;
   int position = 0;

   error = TRANSLATE_ERROR(PMPI_Unpack(m->payload, m->header.length-DATA_MESSAGE_SIZE, &position, buf, count, t->type, c->comm));

   if (error == EMPI_SUCCESS) {
      set_status(s, m->source, m->tag, error, t, count, FALSE);
   }

   free_data_message(m);
   return error;
}


// Process at most one incoming message from MPI.
static int probe_mpi_data_message(data_message **message, int blocking)
{
   int error, flag, count;
   MPI_Status status;
   unsigned char *buffer;

   // NOTE: We (i)probe for a DATA message from a gateway here. Since there may be multiple
   //       gateways, we use MPI_ANY_SOURCE instead of a specific source rank.

   if (blocking) {

      error = PMPI_Probe(MPI_ANY_SOURCE, TAG_FORWARDED_DATA_MSG, mpi_comm_gateway_and_application, &status);

      if (error != MPI_SUCCESS) {
         ERROR(1, "Failed to probe MPI! (error=%d)", error);
         return TRANSLATE_ERROR(error);
      }

      flag = 1;

   } else {
      error = PMPI_Iprobe(MPI_ANY_SOURCE, TAG_FORWARDED_DATA_MSG, mpi_comm_gateway_and_application, &flag, &status);

      if (error != MPI_SUCCESS) {
         ERROR(1, "Failed to probe MPI! (error=%d)", error);
         return TRANSLATE_ERROR(error);
      }
   }

   if (!flag) {
      return EMPI_SUCCESS;
   }

   error = PMPI_Get_count(&status, MPI_BYTE, &count);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to receive size of MPI message! (error=%d)", error);
      return TRANSLATE_ERROR(error);
   }

   buffer = malloc(count);

   if (buffer == NULL) {
      ERROR(1, "Failed to allocate space for MPI message! (error=%d)", error);
      return EMPI_ERR_INTERN;
   }

   // NOTE: Blocking receive should NOT block, as the probe already told us a message is waiting.
   error = PMPI_Recv(buffer, count, MPI_BYTE, status.MPI_SOURCE, status.MPI_TAG, mpi_comm_gateway_and_application, MPI_STATUS_IGNORE);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to receive after (i)probe! source=%d tag=%d count=%d (error=%d)", status.MPI_SOURCE, status.MPI_TAG, count, error);
      return TRANSLATE_ERROR(error);
   }

   *message = (data_message *)buffer;
   return EMPI_SUCCESS;
}

static int do_send(int opcode, void* buf, int count, datatype *t, int dest, int tag, communicator* c)
{
   // We have already checked the various parameters, so all we have to so is send the lot!
   int bytes, error, tmp;
   data_message *m;

   tmp = 0;

   DEBUG(1, "Forwarding message to gateway %d", my_gateway);

   // Get the data size
   error = TRANSLATE_ERROR(PMPI_Pack_size(count, t->type, c->comm, &bytes));

   if (error != EMPI_SUCCESS) {
      return error;
   }

   DEBUG(2, "Message size is %d bytes", bytes);

   // Allocate a data message, and fill the header.
   m = create_data_message(opcode, get_pid(c, dest), c->handle, c->global_rank, dest, tag, count, bytes);

   if (m == NULL) {
      ERROR(1, "Failed to allocate MPI message buffer!");
      return EMPI_ERR_INTERN;
   }

   DEBUG(2, "Packing message into send buffer");

   // Copy the data to the message (TODO: is this needed ??)
   error = TRANSLATE_ERROR(PMPI_Pack(buf, count, t->type, &(m->payload), bytes, &tmp, c->comm));

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to pack MPI message into message buffer!");
      return error;
   }

   DEBUG(1, "Forwarding message to gateway %d", my_gateway);

   // Send the message to the gateway.
   error = TRANSLATE_ERROR(PMPI_Send(m, DATA_MESSAGE_SIZE + bytes, MPI_BYTE, my_gateway, TAG_DATA_MSG, mpi_comm_gateway_and_application));

   // Free the message buffer.
   free_data_message(m);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to forward message to gateway %d!", my_gateway);
   }

   // Return the error if needed.
   return error;
}

// Send a wide area message.
int messaging_send(void* buf, int count, datatype *t, int dest, int tag, communicator* c)
{
   return do_send(OPCODE_DATA, buf, count, t, dest, tag, c);
}


// Receive a wide area message.
int messaging_receive(void *buf, int count, datatype *t, int source, int tag, EMPI_Status *status, communicator* c)
{
   int error;
   data_message *m;

   m = find_pending_message(c, source, tag);

   while (m == NULL) {

      error = probe_mpi_data_message(&m, 1);

      if (error != EMPI_SUCCESS) {
         ERROR(1, "Failed to probe for MPI DATA message (error=%d)", error);
         return error;
      }

      DEBUG(5, "Message received from opcode=%d src_pid=%d dst_pid=%d length=%d comm=%d source=%d dest=%d tag=%d count=%d",
                    m->header.opcode, m->header.src_pid, m->header.dst_pid, m->header.length,
                    m->comm, m->source, m->dest, m->tag, m->count);

      if (!match_message(m, c->handle, source, tag)) {
         DEBUG(5, "No match. Storing message");
         store_message(m);
         m = NULL;
      }
   }

   error = unpack_message(buf, count, t, c, m, status);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to unpack MPI DATA message (error=%d)", error);
   }

   return error;
}

// Broadcast a message to all cluster coordinators in the communicator.
int messaging_bcast(void* buf, int count, datatype *t, int root, communicator* c)
{
   int i, error;

   for (i=0;i<cluster_count;i++) {
      if (i != cluster_rank) {
         error = do_send(OPCODE_COLLECTIVE_BCAST, buf, count, t, c->coordinators[i], BCAST_TAG, c);

         if (error != EMPI_SUCCESS) {
            ERROR(1, "Failed to send bcast to cluster coordinators!");
            return error;
         }
      }
   }

   return EMPI_SUCCESS;
}

// Receive a broadcast message on the cluster coordinators.
int messaging_bcast_receive(void *buf, int count, datatype *t, int root, communicator* c)
{
   return messaging_receive(buf, count, t, root, BCAST_TAG, EMPI_STATUS_IGNORE, c);
}

// Probe if a message is available.
int messaging_probe_receive(request *r, int blocking)
{
   int error;
   data_message *m;

   if ((r->flags & REQUEST_FLAG_COMPLETED)) {
      return EMPI_SUCCESS;
   }

   r->message = find_pending_message(r->c, r->source_or_dest, r->tag);

   if (r->message != NULL) {
      r->flags |= REQUEST_FLAG_COMPLETED;
      return EMPI_SUCCESS;
   }

   r->error = EMPI_SUCCESS;

   do {
      m = NULL;

      error = probe_mpi_data_message(&m, blocking);

      if (error != EMPI_SUCCESS) {
         ERROR(1, "Failed to probe for MPI DATA message (error=%d)", error);
         r->error = error;
         r->flags |= REQUEST_FLAG_COMPLETED;
         return error;
      }

      if (m != NULL) {
         // We've received a message. Let's see if it matches our requirements...
         if (match_message(m, r->c->handle, r->source_or_dest, r->tag)) {
            r->message = m;
            r->flags |= REQUEST_FLAG_COMPLETED;
            return EMPI_SUCCESS;
         }

         DEBUG(5, "No match. Storing message");
         store_message(m);
      }

   } while (blocking);

   return EMPI_SUCCESS;
}

// Finalize a pending receive request.
int messaging_finalize_receive(request *r, EMPI_Status *status)
{
   if (!(r->flags & REQUEST_FLAG_COMPLETED)) {
      return EMPI_ERR_INTERN;
   }

   if (r->flags & REQUEST_FLAG_UNPACKED) {
      return EMPI_SUCCESS;
   }

   r->error = unpack_message(r->buf, r->count, r->type, r->c, r->message, status);
   r->flags |= REQUEST_FLAG_UNPACKED;

   return r->error;
}


/*****************************************************************************/
/*               Server Communication on Application Process                 */
/*****************************************************************************/

static int *copy_int_array(uint32_t *src, int offset, int len)
{
   int i;
   int *result = malloc(len * sizeof(int));

   if (result == NULL) {
      ERROR(1, "Failed to allocate space for int []!");
      return NULL;
   }

   for (i=0;i<len;i++) {
      result[i] = ntohl(src[offset+i]);
   }

   return result;
}

static int send_server_request(unsigned char *data, int len)
{
   int error;

   DEBUG(1, "Sending server message to %d of length %d", server_gateway, len);

   error = PMPI_Send(data, len, MPI_BYTE, server_gateway, TAG_SERVER_REQUEST, mpi_comm_gateway_and_application);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to send server request to gateway! (error=%d)", error);
      return TRANSLATE_ERROR(error);
   }

   return EMPI_SUCCESS;
}

static int receive_server_message(unsigned char *data, int len)
{
   int error;

   DEBUG(1, "Waiting for server message from %d", server_gateway);

   error = PMPI_Recv(data, len, MPI_BYTE, server_gateway, TAG_SERVER_REPLY, mpi_comm_gateway_and_application, MPI_STATUS_IGNORE);

   DEBUG(1, "Received server message (error=%d)", error);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to receive server reply! (error=%d)", error);
      return TRANSLATE_ERROR(error);
   }

   return EMPI_SUCCESS;
}

static int receive_server_message_varlen(unsigned char **data, int *len)
{
   int error;
   MPI_Status status;

   error = PMPI_Probe(server_gateway, TAG_SERVER_REPLY, mpi_comm_gateway_and_application, &status);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to probe server reply (varlen)! (error=%d)", error);
      return TRANSLATE_ERROR(error);
   }

   error = PMPI_Get_count(&status, MPI_BYTE, len);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to retrieve size of receive server reply (varlen)! (error=%d)", error);
      return TRANSLATE_ERROR(error);
   }

   *data = malloc(*len);

   if (*data == NULL) {
      ERROR(1, "Failed to allocate space for server reply (varlen)!");
      return MPI_ERR_INTERN;
   }

   return receive_server_message(*data, *len);
}


int messaging_comm_split_send(communicator* c, int color, int key)
{
   int error;
   split_request_msg req;

   req.header.opcode  = htonl(OPCODE_SPLIT);
   req.header.src_pid = htonl(my_pid);
   req.header.dst_pid = htonl(server_pid);
   req.header.length  = htonl(SPLIT_REQUEST_MSG_SIZE);
   req.comm  = htonl(c->handle);
   req.src   = htonl(c->global_rank);
   req.color = htonl(color);
   req.key   = htonl(key);

   error = send_server_request((unsigned char *)&req, SPLIT_REQUEST_MSG_SIZE);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "INTERNAL ERROR: failed to send COMM FREE request to server! (error=%d)", error);
      return error;
   }

   return EMPI_SUCCESS;
}

int messaging_comm_split_receive(comm_reply *reply)
{
   // Since operations on communicators are collective operations, we can
   // assume here that the reply has not be received yet.

   // Since we do not know the size of the reply message in advance,
   // we let the receive allocate the data for us.
   split_reply_msg *msg;
   int len, error;

   error = receive_server_message_varlen((unsigned char **)&msg, &len);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "INTERNAL ERROR: failed to receive server reply! (error=%d)", error);
      return error;
   }

   // Sanity check on the opcode
   if (OPCODE_SPLIT_REPLY != ntohl(msg->header.opcode)) {
      ERROR(1, "INTERNAL ERROR: unexpected message opcode %d", msg->header.opcode);
      return EMPI_ERR_INTERN;
   }

//   reply->comm = ntohl(msg->comm);
//   reply->src = ntohl(msg->src);
   reply->newComm = ntohl(msg->newComm);
   reply->rank = ntohl(msg->rank);
   reply->size = ntohl(msg->size);
   reply->color = ntohl(msg->color);
   reply->key = ntohl(msg->key);
   reply->cluster_count = ntohl(msg->cluster_count);
   reply->flags = ntohl(msg->flags);

   DEBUG(1, "*Received comm reply (newComm=%d rank=%d size=%d color=%d key=%d cluster_count=%d flag=%d)", 
           reply->newComm, reply->rank, reply->size, reply->color, reply->key, reply->cluster_count, reply->flags);

   reply->coordinators = copy_int_array(msg->payload, 0, reply->cluster_count);

   if (reply->coordinators == NULL) {
      ERROR(1, "Failed to allocate or receive coordinators");
      return EMPI_ERR_INTERN;
   }

   reply->cluster_sizes = copy_int_array(msg->payload, reply->cluster_count, reply->cluster_count);

   if (reply->cluster_sizes == NULL) {
      ERROR(1, "Failed to allocate or receive cluster sizes");
      return EMPI_ERR_INTERN;
   }

   reply->cluster_ranks = copy_int_array(msg->payload, 2*reply->cluster_count, reply->cluster_count);

   if (reply->cluster_ranks == NULL) {
      ERROR(1, "Failed to allocate or receive cluster ranks");
      return EMPI_ERR_INTERN;
   }

   if (reply->size > 0) {

      reply->members = (uint32_t *) copy_int_array(msg->payload, 3*reply->cluster_count, reply->size);

      if (reply->members == NULL) {
         ERROR(1, "Failed to allocate or receive communicator members");
         return EMPI_ERR_INTERN;
      }

      reply->member_cluster_index = (uint32_t *) copy_int_array(msg->payload, 3*reply->cluster_count + reply->size, reply->size);

      if (reply->member_cluster_index == NULL) {
         ERROR(1, "Failed to allocate or receive communicator member cluster index");
         return EMPI_ERR_INTERN;
      }

      reply->local_ranks = (uint32_t *) copy_int_array(msg->payload, 3*reply->cluster_count + 2*reply->size, reply->size);

      if (reply->local_ranks == NULL) {
         ERROR(1, "Failed to allocate or receive communicator member local ranks");
         return EMPI_ERR_INTERN;
      }
   } else {
      reply->members = NULL;
      reply->member_cluster_index = NULL;
      reply->local_ranks  = NULL;
   }

   free(msg);

   return EMPI_SUCCESS;
}


int messaging_comm_create_send(communicator* c, group *g)
{
   int i, msgsize, error;
   group_request_msg *req;

   msgsize = GROUP_REQUEST_MSG_SIZE + (g->size*sizeof(int));

DEBUG(1, "Sending group request of size %d", msgsize);

   req = (group_request_msg *) malloc(msgsize);

   if (req == NULL) {
      ERROR(1, "Failed to allocate space COMM_CREATE request!");
      return EMPI_ERR_INTERN;
   }

   req->header.opcode  = htonl(OPCODE_GROUP);
   req->header.src_pid = htonl(my_pid);
   req->header.dst_pid = htonl(server_pid);
   req->header.length  = htonl(msgsize);

   req->comm = htonl(c->handle);
   req->src = htonl(c->global_rank);
   req->size = htonl(g->size);

   for (i=0;i<g->size;i++) {
      req->members[i] = htonl(g->members[i]);
   }

DEBUG(1, "Group request header %d %d %d %d %d", ntohl(req->header.opcode), ntohl(req->header.length), ntohl(req->comm), ntohl(req->src), ntohl(req->size));

   error = send_server_request((unsigned char *)req, msgsize);

   free(req);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to send COMM_CREATE request!");
   }

   return error;
}

int messaging_comm_create_receive(group_reply *reply)
{
   // Since operations on communicators are collective operations, we can
   // assume here that the reply has not be received yet.

   // Since we do not know the size of the reply message in advance,
   // we let the receive allocate the data for us.
   group_reply_msg *msg;
   int len, error;

   error = receive_server_message_varlen((unsigned char **)&msg, &len);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "INTERNAL ERROR: failed to receive server reply! (error=%d)", error);
      return error;
   }

   // Sanity check on the opcode
   if (OPCODE_GROUP_REPLY != ntohl(msg->header.opcode)) {
      ERROR(1, "INTERNAL ERROR: unexpected message opcode %d", msg->header.opcode);
      return EMPI_ERR_INTERN;
   }

//   reply->comm = ntohl(msg->comm);
//   reply->src = ntohl(msg->src);
   reply->newComm = ntohl(msg->newComm);
   reply->rank = ntohl(msg->rank);
   reply->size = ntohl(msg->size);
   reply->type = ntohl(msg->type);
   reply->cluster_count = ntohl(msg->cluster_count);
   reply->flags = ntohl(msg->flags);

   DEBUG(1, "*Received group reply (newComm=%d rank=%d size=%d type=%d cluster_count=%d flag=%d)",
           reply->newComm, reply->rank, reply->size, reply->type, reply->cluster_count, reply->flags);

   if (reply->type == GROUP_TYPE_ACTIVE) {

      reply->coordinators = copy_int_array(msg->payload, 0, reply->cluster_count);

      if (reply->coordinators == NULL) {
         ERROR(1, "Failed to allocate or receive coordinators");
         return EMPI_ERR_INTERN;
      }

      reply->cluster_sizes = copy_int_array(msg->payload, reply->cluster_count, reply->cluster_count);

      if (reply->cluster_sizes == NULL) {
         ERROR(1, "Failed to allocate or receive cluster sizes");
         return EMPI_ERR_INTERN;
      }

      reply->cluster_ranks = copy_int_array(msg->payload, 2*reply->cluster_count, reply->cluster_count);

      if (reply->cluster_ranks == NULL) {
         ERROR(1, "Failed to allocate or receive cluster ranks");
         return EMPI_ERR_INTERN;
      }

      if (reply->size > 0) {

         reply->members = (uint32_t *) copy_int_array(msg->payload, 3*reply->cluster_count, reply->size);

         if (reply->members == NULL) {
            ERROR(1, "Failed to allocate or receive communicator members");
            return EMPI_ERR_INTERN;
         }

         reply->member_cluster_index = (uint32_t *) copy_int_array(msg->payload, 3*reply->cluster_count + reply->size, reply->size);

         if (reply->member_cluster_index == NULL) {
            ERROR(1, "Failed to allocate or receive communicator member cluster index");
            return EMPI_ERR_INTERN;
         }

         reply->local_ranks = (uint32_t *) copy_int_array(msg->payload, 3*reply->cluster_count + 2*reply->size, reply->size);

         if (reply->local_ranks == NULL) {
            ERROR(1, "Failed to allocate or receive communicator member local ranks");
            return EMPI_ERR_INTERN;
         }
      } else {
         reply->members = NULL;
         reply->member_cluster_index = NULL;
         reply->local_ranks  = NULL;
      }
   } else {
      reply->coordinators = NULL;
      reply->cluster_sizes = NULL;
      reply->cluster_ranks = NULL;
      reply->members = NULL;
      reply->member_cluster_index = NULL;
      reply->local_ranks  = NULL;
   }

   free(msg);

   return EMPI_SUCCESS;
}

int messaging_comm_dup_send(communicator* c)
{
   int error;
   dup_request_msg req;

   req.header.opcode  = htonl(OPCODE_DUP);
   req.header.src_pid = htonl(my_pid);
   req.header.dst_pid = htonl(server_pid);
   req.header.length  = htonl(DUP_REQUEST_MSG_SIZE);
   req.comm           = htonl(c->handle);
   req.src            = htonl(c->global_rank);

   error = send_server_request((unsigned char *)&req, FREE_REQUEST_MSG_SIZE);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "INTERNAL ERROR: failed to send COMM FREE request to server! (error=%d)", error);
   }

   return error;
}

int messaging_comm_free_send(communicator* c)
{
   int error;
   free_request_msg req;

   req.header.opcode  = htonl(OPCODE_FREE);
   req.header.src_pid = htonl(my_pid);
   req.header.dst_pid = htonl(server_pid);
   req.header.length  = htonl(FREE_REQUEST_MSG_SIZE);
   req.comm           = htonl(c->handle);
   req.src            = htonl(c->global_rank);

   error = send_server_request((unsigned char *)&req, FREE_REQUEST_MSG_SIZE);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "INTERNAL ERROR: failed to send COMM FREE request to server! (error=%d)", error);
   }

   return error;
}


int messaging_comm_dup_receive(dup_reply *reply)
{
   // Since operations on communicators are collective operations, we can
   // assume here that the reply has not be received yet.
   int error;
   dup_reply_msg msg;

   // Note: this is a blocking receive, as we cannot continue
   // with the until application until this message is received!
   error = receive_server_message((unsigned char *)&msg, DUP_REPLY_MSG_SIZE);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "INTERNAL ERROR: failed to receive server reply! (error=%d)", error);
      return error;
   }

   // Sanity check on the opcode
   if (OPCODE_DUP_REPLY != ntohl(msg.header.opcode)) {
      ERROR(1, "INTERNAL ERROR: unexpected message opcode %d", msg.header.opcode);
      return EMPI_ERR_INTERN;
   }

//   reply->comm = ntohl(msg.comm);
//   reply->src = ntohl(msg.src);
   reply->newComm = ntohl(msg.newComm);

   DEBUG(1, "*Received dup reply (newComm=%d)", reply->newComm);

   return EMPI_SUCCESS;
}

int messaging_finalize()
{
   // A finalize is an collective operation. It must be performed by all
   // application processes.

   int error;
   finalize_request_msg req;
   finalize_reply_msg msg;

   communicator *c;

   c = handle_to_communicator(0);

DEBUG(1, "Sending finalize request %d:%d", c->handle, c->global_rank);

   req.header.opcode  = htonl(OPCODE_FINALIZE);
   req.header.src_pid = htonl(my_pid);
   req.header.dst_pid = htonl(server_pid);
   req.header.length  = htonl(FINALIZE_REQUEST_MSG_SIZE);
   req.comm           = htonl(c->handle);
   req.src            = htonl(c->global_rank);

   // Send the request to the server.
   error = send_server_request((unsigned char *)&req, FINALIZE_REQUEST_MSG_SIZE);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "INTERNAL ERROR: failed to send FINALIZE request to server! (error=%d)", error);
      return error;
   }

DEBUG(1, "Finalize request %d:%d send", c->handle, c->global_rank);
DEBUG(1, "Receiving finalize reply %d:%d send", c->handle, c->global_rank);

   error = receive_server_message((unsigned char *)&msg, FINALIZE_REPLY_MSG_SIZE);

   if (error != MPI_SUCCESS) {
      ERROR(1, "INTERNAL ERROR: failed to receive FINALIZE reply from server! (error=%d)", error);
      return TRANSLATE_ERROR(error);
   }

DEBUG(1, "Received finalize reply %d:%d send", c->handle, c->global_rank);

   error = PMPI_Barrier(MPI_COMM_WORLD);


   error = PMPI_Finalize();

   if (error != MPI_SUCCESS) {
      ERROR(1, "INTERNAL ERROR: failed to perform FINALIZE of local MPI! (error=%d)", error);
      return TRANSLATE_ERROR(error);
   }

   return EMPI_SUCCESS;
}


