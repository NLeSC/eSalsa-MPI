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
#include "socket_util.h"
#include "request_queue.h"
#include "generic_message.h"
#include "message_buffer.h"

#define DEFAULT_GATEWAY_IN_BUFFER (8*1024*1024)
#define DEFAULT_GATEWAY_OUT_BUFFER (8*1024*1024)

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
	int newComm; // communicator created
} dup_reply_msg;

#define DUP_REPLY_MSG_SIZE (MESSAGE_HEADER_SIZE + 1*sizeof(int))

// This is the reply to a finalize request.
typedef struct {
	message_header header;
} finalize_reply_msg;

#define FINALIZE_REPLY_MSG_SIZE (MESSAGE_HEADER_SIZE)


// The number of clusters and the rank of our cluster in this set.
uint32_t cluster_count;
uint32_t cluster_rank;

// The number of application processes and gateways in this cluster.
//extern uint32_t local_application_size;
//extern uint32_t gateway_count;

// The gateway rank of this gateway process. -1 if process is application process.
//extern int gateway_rank;

// The rank of the gateway process handling my outgoing traffic. Always -1 (invalid) if process is gateway process.
//static int my_gateway;

// The rank of the gateway process handling all server traffic. Always 0 if process is gateway process.
//static int server_gateway;

// The PID of this process. Used when forwarding messages.
uint32_t my_pid;

// The PID of the server process. Used in communicator and group operations.
uint32_t server_pid;

// The size of each cluster, and the offset of each cluster in the
// total set of machines.
int *cluster_sizes;
int *cluster_offsets;

//static MPI_Comm mpi_comm_application_only;

// Unused?
//extern MPI_Comm mpi_comm_gateways_only;
//MPI_Comm mpi_comm_gateway_and_application;


// All information about the gateway. Note that gateway_name is only valid on rank 0.
static char *gateway_name;
static long gateway_ipv4;
static unsigned short gateway_port;

// The socket connected to the gateway.
static int gatewayfd;

static message_buffer *gateway_in_buffer;
static message_buffer *gateway_out_buffer;

static request_queue *send_request_queue;
static request_queue *receive_request_queue;

/*****************************************************************************/
/*                      Initialization / Finalization                        */
/*****************************************************************************/

static int read_config_file()
{
	int  error;
	char *file;
	FILE *config;
	char buffer[1024];

	file = getenv("EMPI_LOCAL_CONFIG");

	if (file == NULL) {
		WARN(0, "EMPI_LOCAL_CONFIG not set!");
		return EMPI_ERR_NO_SUCH_FILE;
	}

	INFO(0, "Looking for config file %s", file);

	config = fopen(file, "r");

	if (config == NULL) {
		ERROR(1, "Failed to open config file %s", file);
		return EMPI_ERR_NO_SUCH_FILE;
	}

	INFO(0, "Config file %s opened.", file);

	// Read the cluster name
	error = fscanf(config, "%s", buffer);

	if (error == EOF || error == 0) {
		fclose(config);
		ERROR(1, "Failed to read cluster name from %s", file);
		return EMPI_ERR_UNKNOWN;
	}

	//	cluster_name = malloc(strlen(buffer)+1);
	//
	//	if (cluster_name == NULL) {
	//		fclose(config);
	//		ERROR(1, "Failed to allocate space for cluster name %s", buffer);
	//		return EMPI_ERR_INTERN;
	//	}
	//
	//	strcpy(cluster_name, buffer);

	// Read the server address
	error = fscanf(config, "%s", buffer);

	if (error == EOF || error == 0) {
		fclose(config);
		ERROR(1, "Failed to read server address from %s", file);
		return EMPI_ERR_INTERN;
	}

	gateway_name = malloc(strlen(buffer+1));

	if (gateway_name == NULL) {
		fclose(config);
		ERROR(1, "Failed to allocate space for server address %s", buffer);
		return EMPI_ERR_INTERN;
	}

	strcpy(gateway_name, buffer);

	// Read the server port
	error = fscanf(config, "%hu", &gateway_port);

	if (error == EOF || error == 0) {
		fclose(config);
		ERROR(1, "Failed to read server port from %s", file);
		return EMPI_ERR_INTERN;
	}

	fclose(config);
	return 0;
}

int messaging_init(int rank, int size, int *argc, char ***argv)
{
	int error;
	uint32_t msg[3];

	// FIXME: hardcoded size!
	gateway_in_buffer  = message_buffer_create(DEFAULT_GATEWAY_IN_BUFFER);
	gateway_out_buffer = message_buffer_create(DEFAULT_GATEWAY_OUT_BUFFER);

	// FIXME: hardcoded cache size!
	send_request_queue = request_queue_create(10);
	receive_request_queue = request_queue_create(10);

	// The first MPI process reads the config file which contains the location of the local gateway.
	if (rank == 0) {
		error = read_config_file();

		if (error != EMPI_SUCCESS) {
			return error;
		}

		// Convert the gateway hostname to an IPv4 address.
		error = socket_get_ipv4_address(gateway_name, &gateway_ipv4);

		if (error != SOCKET_OK) {
			ERROR(1, "Failed to retrieve server address!");
			return EMPI_ERR_GATEWAY;
		}
	}

	// Make sure every one knows about the gateway!
	error = PMPI_Bcast(&gateway_ipv4, 1, MPI_LONG, 0, MPI_COMM_WORLD);

	if (error != MPI_SUCCESS) {
		ERROR(1, "Failed to broadcast gateway IP! (error=%d)", error);
		return EMPI_ERR_INTERN;
	}

	error = PMPI_Bcast(&gateway_port, 1, MPI_SHORT, 0, MPI_COMM_WORLD);

	if (error != MPI_SUCCESS) {
		ERROR(1, "Failed to broadcast gateway port! (error=%d)", error);
		return EMPI_ERR_INTERN;
	}

	// All MPI processes now know the gateway location.
	// Rank 0 will connect to gateway first and gets information about the number of participating clusters, their sizes,
	// offsets, etc.
	if (rank == 0) {

		error = socket_connect(gateway_ipv4, gateway_port, -1, -1, &gatewayfd);

		if (error != SOCKET_OK) {
			ERROR(1, "Failed to connect to gateway! (error=%d)", error);
			return EMPI_ERR_GATEWAY;
		}

		msg[0] = OPCODE_HANDSHAKE;
		msg[1] = rank;
		msg[2] = size;

		error = socket_sendfully(gatewayfd, (unsigned char *)&msg, 3 * sizeof(uint32_t));

		if (error != SOCKET_OK) {
			ERROR(1, "Failed to complete handshake with gateway! (error=%d)", error);
			return EMPI_ERR_GATEWAY;
		}

		error = socket_receivefully(gatewayfd, (unsigned char *)&msg, 3 * sizeof(uint32_t));

		if (error != SOCKET_OK) {
			ERROR(1, "Failed to complete handshake with gateway! (error=%d)", error);
			return EMPI_ERR_GATEWAY;
		}

		if (msg[0] != OPCODE_HANDSHAKE_ACCEPTED) {
			ERROR(1, "Gateway did not accept handshake! (error=%d)", error);
			close(gatewayfd);
			return EMPI_ERR_GATEWAY;
		}

		cluster_rank = msg[1];
		cluster_count = msg[2];
	}

	// Tell everyone about how many clusters there are.
	error = PMPI_Bcast(&cluster_rank, 1, MPI_INT, 0, MPI_COMM_WORLD);

	if (error != MPI_SUCCESS) {
		ERROR(1, "Failed to broadcast cluster rank! (error=%d)", error);
		return EMPI_ERR_INTERN;
	}

	error = PMPI_Bcast(&cluster_count, 1, MPI_INT, 0, MPI_COMM_WORLD);

	if (error != MPI_SUCCESS) {
		ERROR(1, "Failed to broadcast cluster count! (error=%d)", error);
		return EMPI_ERR_INTERN;
	}

	// Allocate space for the cluster information
	cluster_sizes = malloc(cluster_count * sizeof(int));
	cluster_offsets = malloc((cluster_count+1) * sizeof(int));

	if (cluster_sizes == NULL || cluster_offsets == NULL) {
		// TODO: tell others
		ERROR(1, "Failed to allocate space for cluster info!");
		return EMPI_ERR_INTERN;
	}

	// Rank 0 receives cluster information from the gateway
	if (rank == 0) {

		error = socket_receivefully(gatewayfd, (unsigned char *)cluster_sizes, cluster_count * sizeof(int));

		if (error != SOCKET_OK) {
			ERROR(1, "Failed to receive cluster sizes from gateway! (error=%d)", error);
			return EMPI_ERR_GATEWAY;
		}

		error = socket_receivefully(gatewayfd, (unsigned char *)cluster_offsets, (cluster_count+1) * sizeof(int));

		if (error != SOCKET_OK) {
			ERROR(1, "Failed to receive cluster offsets from gateway! (error=%d)", error);
			return EMPI_ERR_GATEWAY;
		}
	}

	// Tell everyone about the cluster sizes and offsets.
	error = PMPI_Bcast(cluster_sizes, cluster_count, MPI_INT, 0, MPI_COMM_WORLD);

	if (error != MPI_SUCCESS) {
		ERROR(1, "Failed to broadcast cluster sizes! (error=%d)", error);
		return EMPI_ERR_INTERN;
	}

	error = PMPI_Bcast(cluster_offsets, (cluster_count+1), MPI_INT, 0, MPI_COMM_WORLD);

	if (error != MPI_SUCCESS) {
		ERROR(1, "Failed to broadcast cluster offsets! (error=%d)", error);
		return EMPI_ERR_INTERN;
	}

	// Everyone in this cluster now has the following info:
	//
	// Number of clusters
	// Rank of this cluster
	// Size of each cluster.
	// Offset of each cluster.

	// Next, all other ranks connect to the gateway

	if (rank != 0) {

		error = socket_connect(gateway_ipv4, gateway_port, -1, -1, &gatewayfd);

		if (error != SOCKET_OK) {
			ERROR(1, "Failed to connect to gateway! (error=%d)", error);
			return EMPI_ERR_GATEWAY;
		}

		msg[0] = OPCODE_HANDSHAKE;
		msg[1] = rank;
		msg[2] = size;

		error = socket_sendfully(gatewayfd, (unsigned char *)&msg, 3 * sizeof(uint32_t));

		if (error != SOCKET_OK) {
			ERROR(1, "Failed to complete handshake with gateway! (error=%d)", error);
			return EMPI_ERR_GATEWAY;
		}

		error = socket_receivefully(gatewayfd, (unsigned char *)&msg, 1 * sizeof(uint32_t));

		if (error != SOCKET_OK) {
			ERROR(1, "Failed to complete handshake with gateway! (error=%d)", error);
			return EMPI_ERR_GATEWAY;
		}

		if (msg[0] != OPCODE_HANDSHAKE_ACCEPTED) {
			ERROR(1, "Gateway did not accept handshake! (error=%d)", error);
			close(gatewayfd);
			return EMPI_ERR_GATEWAY;
		}
	}

	// Generate my PID.
	my_pid = SET_PID(cluster_rank, rank);

	// The server PID is all 1's
	server_pid = 0xFFFFFFFF;

	// Perform a barrier here, to ensure that the application processes are initialized.
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

//static data_message *create_data_message(int opcode, int dst_pid, int comm, int source, int dest, int tag, int count, int data_size)
//{
//   data_message *m;
//
//   m = malloc(DATA_MESSAGE_SIZE + data_size);
//
//   if (m == NULL) {
//       ERROR(1, "Failed to allocate message buffer!\n");
//       return NULL;
//   }
//
//   m->header.opcode = opcode;
//   m->header.src_pid = my_pid;
//   m->header.dst_pid = dst_pid;
//   m->header.length = DATA_MESSAGE_SIZE + data_size;
//
//   m->comm = comm;
//   m->source = source;
//   m->dest = dest;
//   m->tag = tag;
//   m->count = count;
//
//   return m;
//}

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

/*
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
 */

static int probe_gateway_message(message_header *mh, bool blocking)
{
	int error;
	size_t tmp;

	// NOTE: we may receive any combination of message here.
	do {
		// First we poll the gateway socket and attempt to receive some data.
		error = socket_receive_mb(gatewayfd, gateway_in_buffer, false);

		if (error != SOCKET_OK) {
			ERROR(1, "Failed to read gateway message!");
			return EMPI_ERR_GATEWAY;
		}

		// Next, we check if there is enough data available for a message header.
		if (message_buffer_max_read(gateway_in_buffer) >= MESSAGE_HEADER_SIZE) {
			// There should be at least a message header in the buffer!
			tmp = message_buffer_peek(gateway_in_buffer, (unsigned char *)mh, MESSAGE_HEADER_SIZE);

			if (tmp != MESSAGE_HEADER_SIZE) {
				// Should not happen!
				ERROR(1, "Failed to peek at gateway_in_buffer!");
				return EMPI_ERR_INTERN;
			}

			if (message_buffer_max_read(gateway_in_buffer) >= mh->length) {
				// There is a complete message in the buffer!
				return 0;
			}

			// No complete message yet. Check if the message will fit in the buffer.
			if (message_buffer_max_write(gateway_in_buffer) < mh->length) {

				if (mh->length > message_buffer_size(gateway_in_buffer)) {
					// The message is larger than the buffer!
					ERROR(1, "Failed to receive wide area message as it is larger than receive buffer!");
					return -1;
				}

				// We need to compact the buffer or the message will not fit!
				message_buffer_compact(gateway_in_buffer);
			}
		}

	} while (blocking);

	return 1;
}

/*
static int probe_gateway_message(data_message **message, bool blocking)
{
   int error, flag, count;
   MPI_Status status;
   generic_message *msg;
   size_t tmp;
   message_header mh;

   // NOTE: we may receive any combination of message here.
   do {
	   // First we poll the gateway socket and attempt to receive some data.
	   message_buffer_receive(gatewayfd, gateway_in_buffer, false);

	   // Next, we check if there is enough data available for a message header.
	   if (message_buffer_max_read(gateway_in_buffer) >= MESSAGE_HEADER_SIZE) {
		   // There should be at least a message header in the buffer!

		   tmp = message_buffer_peek(gateway_in_buffer, &mh, MESSAGE_HEADER_SIZE);

		   if (tmp != MESSAGE_HEADER_SIZE) {
			   // Should not happen!
			   ERROR(1, "Failed to peek at gateway_in_buffer!");
			   return EMPI_ERR_INTERN;
		   }

		   if (message_buffer_max_read(gateway_in_buffer) >= mh.length) {
			   // There is a complete message in the buffer!
			   msg = malloc(mh.length);

			   if (msg == NULL) {
				   ERROR(1, "Failed to allocate space for message!");
				   return EMPI_ERR_INTERN;
			   }

			   tmp = message_buffer_read(gateway_in_buffer, (unsigned char *)msg, mh.length);

			   if (tmp != mh.length) {
				   // Should not happen!
				   ERROR(1, "Failed to receive from gateway_in_buffer!");
				   return EMPI_ERR_INTERN;
			   }

 *message = msg;
			   return EMPI_SUCCESS;
		   }

		   // When we arrive here, there is not enough data available for the message.
		   if (message_buffer_max_write(gateway_in_buffer) < mh.length) {
			   // We need to compact the buffer or the message will not fit!
			   message_buffer_compact(gateway_in_buffer);
		   }
	   }

	   // If we arrive here, there is not enough data available for the message header and/or message payload.
	   // We either give up, or try another round depending on the value of "blocking".

   } while (blocking);

   return EMPI_SUCCESS;
}
 */

static int ensure_space(size_t space, bool blocking)
{
	int error;
	size_t avail, used, size;

	if (space > gateway_out_buffer->size) {
		// If (space > size) the message will never fit!
		return -1;
	}

	avail = message_buffer_max_write(gateway_out_buffer);

	while (avail < space) {

		// There is not enough space in the buffer for the message. Try to push out some data.
		error = socket_send_mb(gatewayfd, gateway_out_buffer, false);

		if (error != SOCKET_OK) {
			ERROR(1, "Failed to write data to gateway!");
			return -1;
		}

		avail = message_buffer_max_write(gateway_out_buffer);

		if (avail >= space) {
			// There's enough space now.
			return 0;
		}

		// Still not enough space. Maybe compacting the buffer will help ?
		used = message_buffer_max_read(gateway_out_buffer);
		size = message_buffer_size(gateway_out_buffer);

		if (size - used > space) {
			// Compacting gives us enough space!
			message_buffer_compact(gateway_out_buffer);
			return 0;
		}

		if (!blocking) {
			// In non-blocking mode we give up here!
			return 1;
		}
	}

	// Enough space in the buffer!
	return 0;
}

static int do_send1(int opcode, void* buf, int count, datatype *t, int dest, int tag, communicator* c, bool blocking)
{
	int bytes, error, tmp;
	data_message *m;
	unsigned char *buffer;

	// Get the data size
	error = PMPI_Pack_size(count, t->type, c->comm, &bytes);

	if (error != MPI_SUCCESS) {
		ERROR(1, "Failed to determine size of data (error=%d)!", error);
		return -1;
	}

	DEBUG(2, "Message size is %d bytes", bytes);

	error = ensure_space(DATA_MESSAGE_SIZE + bytes, blocking);

	if (blocking) {
		if (error != 0) {
			ERROR(1, "Failed to write message to send buffer (error=%d)!", error);
			return -1;
		}
	} else {
		if (error == -1) {
			ERROR(1, "Failed to write message to send buffer (error=%d)!", error);
			return -1;
		}

		if (error == 1) {
			return 1;
		}
	}

	// We have enough space is the gateway_out_buffer to write the message
	m = (data_message *) message_buffer_direct_write_access(gateway_out_buffer, DATA_MESSAGE_SIZE);

	m->header.opcode = opcode;
	m->header.src_pid = my_pid;
	m->header.dst_pid = get_pid(c, dest);
	m->header.length = DATA_MESSAGE_SIZE + bytes;

	m->comm = c->handle;
	m->source = c->global_rank;
	m->dest = dest;
	m->tag = tag;
	m->count = count;

	// Next, copy the message payload
	DEBUG(2, "Packing message into send buffer");

	buffer = message_buffer_direct_write_access(gateway_out_buffer, bytes);

	// Copy the data to the message
	error = PMPI_Pack(buf, count, t->type, buffer, bytes, &tmp, c->comm);

	if (error != MPI_SUCCESS) {
		ERROR(1, "Failed to pack MPI message into message buffer! (error=%d)",  TRANSLATE_ERROR(error));
		return -1;
	}

	DEBUG(1, "Written message to gateway buffer!",);

	return 0;
}

static int process_pending_sends(bool blocking)
{
	int error;
	request *req;

	req = request_queue_peek(send_request_queue);

	while (req != NULL) {

		error = do_send1(OPCODE_DATA, req->buf, req->count, req->type, req->source_or_dest, req->tag, req->c, blocking);

		if (error != 0) {
			return error;
		}

		// Request has been send succesfully
		req->flags |= REQUEST_FLAG_COMPLETED;

		request_queue_dequeue(send_request_queue);
		req = request_queue_peek(send_request_queue);
	}

	return 0;
}


static int do_send(int opcode, void* buf, int count, datatype *t, int dest, int tag, communicator* c, request *req, bool needs_ack)
{
	// We have already checked the various parameters, so all we have to so is send the lot!
	int error;

	DEBUG(1, "Forwarding message to gateway");

	if (needs_ack) {
		WARN(1, "Message ACK not implemented yet (needed for MPI_Ssend and friends)");
	}

	if (req == NULL) {
		// This is a blocking send, so we must first process all pending non-blocking sends to prevent message reordering.
		error = process_pending_sends(true);

		if (error != 0) {
			ERROR(1, "Failed to process pending send operation!");
			return MPI_ERR_INTERN;
		}

		error = do_send1(opcode, buf, count, t, dest, tag, c, true);

		if (error != 0) {
			ERROR(1, "Failed to send message");
			return MPI_ERR_INTERN;
		}

		return EMPI_SUCCESS;

	} else {
		error = process_pending_sends(false);

		if (error != -1) {
			ERROR(1, "Failed to process pending send operation!");
			return MPI_ERR_INTERN;
		}

		if (error == 1) {
			// We ran out of buffer space while processing pending sends, so simply enqueue the current request.
			request_queue_enqueue(send_request_queue, req);
			return EMPI_SUCCESS;
		}

		error = do_send1(opcode, buf, count, t, dest, tag, c, false);

		if (error != -1) {
			ERROR(1, "Failed to send message");
			return MPI_ERR_INTERN;
		}

		if (error == 1) {
			// We ran out of buffer space while trying to send, so simply enqueue the current request.
			request_queue_enqueue(send_request_queue, req);
			return EMPI_SUCCESS;
		}

		// We copied the message to the send buffer, so update the reques.
		req->flags |= REQUEST_FLAG_COMPLETED;
		return EMPI_SUCCESS;
	}
}

// Send a wide area DATA message.
int messaging_send(void* buf, int count, datatype *t, int dest, int tag, communicator* c, request *req, bool needs_ack)
{
	return do_send(OPCODE_DATA, buf, count, t, dest, tag, c, req, needs_ack);
}

static generic_message *read_generic_message(size_t length)
{
	generic_message *m = malloc(length);

	if (m == NULL) {
		ERROR(1, "Failed to allocate space for server message!");
		return NULL;
	}

	message_buffer_read(gateway_in_buffer, (unsigned char *)m, length);

	return m;
}


/*

static int do_recv1(void *buf, int count, datatype *t, int *source, int *tag, int *count, int *error, communicator* c, bool blocking)
{
	int position, result;
	message_header mh;
	data_message *m;
	generic_message *gm;
	unsigned char *buffer;

	m = find_pending_message(c, *source, *tag);

	if (m != NULL) {
		// Found message in the queue, so unpack it.
 *error = PMPI_Unpack(buffer, m->header.length-DATA_MESSAGE_SIZE, &position, buf, count, t->type, c->comm);
 *source = m->source;
 *tag = m->tag;
 *count = m->count;
		return 0;
	}

	// No message yet, so start polling!
	while (true) {

		result = probe_gateway_message(&mh, blocking);

		if (result == -1) {
			ERROR(1, "Failed to probe for WA message (error=%d)", error);
			return -1;
		}

		if (result == 1) {
			return 1;
		}

		if (mh->opcode != OPCODE_DATA) {
			// We've run into a server message. Should never happen ?
			FATAL(1, "Unexpected server message!");
			return -1;
		}

		// There is a data message in the gateway_in_buffer. Check if this is the message we are looking for.
		m = (data_message *) message_buffer_direct_read_access(gateway_in_buffer, 0);

		DEBUG(4, "Message received from opcode=%d src_pid=%d dst_pid=%d length=%d comm=%d source=%d dest=%d tag=%d count=%d",
				m->header.opcode, m->header.src_pid, m->header.dst_pid, m->header.length,
				m->comm, m->source, m->dest, m->tag, m->count);

		if (match_message(m, c->handle, *source, *tag)) {
			// This is the right message, so unpack it into the application buffer.
			DEBUG(5, "Match. Directly unpacking message to application buffer!");

			// Skip the header.
			message_buffer_skip(gateway_in_buffer, DATA_MESSAGE_SIZE);

			buffer = message_buffer_direct_read_access(gateway_in_buffer, m->header.length-DATA_MESSAGE_SIZE);

 *error = PMPI_Unpack(buffer, m->header.length-DATA_MESSAGE_SIZE, &position, buf, count, t->type, c->comm);
 *source = m->source;
 *tag = m->tag;
 *count = m->count;
			return 0;

		} else {
			if (blocking) {
				// Wrong message, to receive and queue it.
				DEBUG(5, "No match. Copying and storing message");
				m = (data_message *) read_generic_message(mh->length);
				store_message(m);
				m = NULL;
			} else {
				return 1;
			}
		}
	}

	// Unreachable!
	return -1;
}
 */

static int do_recv(int opcode, void *buf, int count, datatype *t, int source, int tag, EMPI_Status *status, communicator* c)
{
	// do a blocking receive.
	//
	// We must first check if there is a matching message on the queue.
	//
	// If not, we try to receive a matching message.
	//
	// HOWEVER: there may be pending receive requests on the receive queue that match the same message, and therefore have
	// right of way.
	//
	// ALSO: any non-matching messages should be compared against the receive queue before storing them in the message queue.

	int error, position;
	message_header mh;
	data_message *m;
	unsigned char *buffer;
	request *req;

	// First we check the receive queue.
	m = find_pending_message(c, source, tag);

	if (m != NULL) {
		// Found message in the queue, so unpack it.
		error = unpack_message(buf, count, t, c, m, status);

		if (error != EMPI_SUCCESS) {
			ERROR(1, "Failed to unpack MPI DATA message (error=%d)", error);
		}

		return error;
	}

	// No message yet, so start polling the network!
	while (true) {

		error = probe_gateway_message(&mh, true);

		if (error != 0) {
			ERROR(1, "Failed to probe for WA message (error=%d)", error);
			return error;
		}

		if (mh.opcode != OPCODE_DATA) {
			// We've run into a server message. Should not happen!
			FATAL("Received unexpected server message!");
		}

		// There is a data message in the gateway_in_buffer. Check if there is a matching message in the receive queue.
		m = (data_message *) message_buffer_direct_read_access(gateway_in_buffer, 0);

		DEBUG(4, "Message received from opcode=%d src_pid=%d dst_pid=%d length=%d comm=%d source=%d dest=%d tag=%d count=%d",
				m->header.opcode, m->header.src_pid, m->header.dst_pid, m->header.length,
				m->comm, m->source, m->dest, m->tag, m->count);

		req = request_queue_dequeue_matching(receive_request_queue, m->comm, m->source, m->tag);

		if (req != NULL) {
			// Found a matching pending receive
			DEBUG(5, "Matched pending receive. Directly unpacking message to application buffer!");

			// Skip the header.
			message_buffer_skip(gateway_in_buffer, DATA_MESSAGE_SIZE);

			buffer = message_buffer_direct_read_access(gateway_in_buffer, m->header.length-DATA_MESSAGE_SIZE);

			position = 0;

			req->error = PMPI_Unpack(buffer, m->header.length-DATA_MESSAGE_SIZE, &position, req->buf, req->count, req->type->type, req->c->comm);
			req->message_source = m->source;
			req->message_tag = m->tag;
			req->message_count = m->count;
			req->flags |= REQUEST_FLAG_COMPLETED;

		} else {

			// No pending receive request found, so see if we match the message!
			if (match_message(m, c->handle, source, tag)) {
				// This is the right message, so unpack it into the application buffer.
				DEBUG(5, "Match. Directly unpacking message to application buffer!");

				// Skip the header.
				message_buffer_skip(gateway_in_buffer, DATA_MESSAGE_SIZE);

				buffer = message_buffer_direct_read_access(gateway_in_buffer, m->header.length-DATA_MESSAGE_SIZE);

				error = PMPI_Unpack(buffer, m->header.length-DATA_MESSAGE_SIZE, &position, buf, count, t->type, c->comm);

				if (error == MPI_SUCCESS) {
					set_status(status, m->source, m->tag, error, t, count, FALSE);
				} else {
					ERROR(1, "Failed to unpack MPI DATA message (error=%d)", error);
					return TRANSLATE_ERROR(error);
				}

				return EMPI_SUCCESS;

			} else {
				// Nobody wants this message, so receive and queue it for later!
				DEBUG(5, "No match. Copying and storing message");
				m = (data_message *) read_generic_message(mh.length);
				store_message(m);
				m = NULL;
			}
		}
	}

	// Unreachable!
	return MPI_ERR_INTERN;
}


// Receive a wide area message.
int messaging_receive(void *buf, int count, datatype *t, int source, int tag, EMPI_Status *status, communicator* c)
{
	return do_recv(OPCODE_DATA, buf, count, t, source, tag, status, c);
}

// Broadcast a message to all cluster coordinators in the communicator.
int messaging_bcast(void* buf, int count, datatype *t, int root, communicator* c)
{
	int i, error;

	for (i=0;i<cluster_count;i++) {
		if (i != cluster_rank) {
			error = messaging_send(buf, count, t, c->coordinators[i], BCAST_TAG, c, NULL, false);
			// error = do_send(OPCODE_COLLECTIVE_BCAST, buf, count, t, c->coordinators[i], BCAST_TAG, c);

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

int messaging_peek_receive_queue(request *r)
{
	if ((r->flags & REQUEST_FLAG_COMPLETED)) {
		return -1;
	}

	if (have_matching_message(r->c, r->source_or_dest, r->tag)) {
		return 0;
	}

	return 1;
}

int messaging_poll_receive_queue(request *r)
{
	int position;
	data_message *m;

	if ((r->flags & REQUEST_FLAG_COMPLETED)) {
		return 0;
	}

	m = find_pending_message(r->c, r->source_or_dest, r->tag);

	if (m != NULL) {
		position = 0;
		r->message_source = m->source;
		r->message_tag = m->tag;
		r->message_count = m->count;
		r->error = TRANSLATE_ERROR(PMPI_Unpack(&(m->payload[0]), m->header.length-DATA_MESSAGE_SIZE, &position, r->buf, r->count, r->type->type, r->c->comm));
		r->flags |= REQUEST_FLAG_COMPLETED;
		free_data_message(m);
		return 0;
	}

	return 1;
}

static int messaging_poll_receive(request *r)
{
	int result, position;
	data_message *m;
	message_header mh;
	unsigned char *buffer;

	if ((r->flags & REQUEST_FLAG_COMPLETED)) {
		return 0;
	}

	// See if there is a message on the network
	result = probe_gateway_message(&mh, false);

	if (result == -1) {
		ERROR(1, "Failed to probe for WA message");
		return -1;
	}

	if (result == 1) {
		// No message was found on the network, so return.
		return 1;
	}

	if (mh.opcode != OPCODE_DATA) {
		// We've run into a server message. Should never happen ?
		FATAL("Unexpected server message!");
	}

	// There is a data message in the gateway_in_buffer. Check if this is the message we are looking for.
	m = (data_message *) message_buffer_direct_read_access(gateway_in_buffer, 0);

	DEBUG(4, "Message received from opcode=%d src_pid=%d dst_pid=%d length=%d comm=%d source=%d dest=%d tag=%d count=%d",
			m->header.opcode, m->header.src_pid, m->header.dst_pid, m->header.length,
			m->comm, m->source, m->dest, m->tag, m->count);

	if (!match_message(m, r->c->handle, r->source_or_dest, r->tag)) {
		// A different message was found on the network so return.
		return 1;
	}

	// This is the right message, so unpack it into the application buffer.
	DEBUG(5, "Match. Directly unpacking message to application buffer!");

	// Skip the header.
	message_buffer_skip(gateway_in_buffer, DATA_MESSAGE_SIZE);

	buffer = message_buffer_direct_read_access(gateway_in_buffer, m->header.length-DATA_MESSAGE_SIZE);

	position = 0;

	r->error = TRANSLATE_ERROR(PMPI_Unpack(buffer, m->header.length-DATA_MESSAGE_SIZE, &position, r->buf, r->count, r->type->type, r->c->comm));
	r->message_source = m->source;
	r->message_tag = m->tag;
	r->message_count = m->count;
	r->flags |= REQUEST_FLAG_COMPLETED;
	return 0;
}

int messaging_post_receive(request *r)
{
	int result;

	result = messaging_poll_receive_queue(r);

	if (result == 0) {
		// A message was found!
		return EMPI_SUCCESS;
	}

	result = messaging_poll_receive(r);

	if (result == 0) {
		// A message was found!
		return EMPI_SUCCESS;
	}

	if (result == -1) {
		ERROR(1, "Failed to probe for WA message");
		return EMPI_ERR_INTERN;
	}

	// No message was found, so enqueue the message.
	request_queue_enqueue(receive_request_queue, r);
	return EMPI_SUCCESS;
}

static int process_pending_receives(bool copy)
{
	int result, position;
	message_header mh;
	data_message *m;
	request *req;
	unsigned char *buffer;

	// See if there is a message on the network
	result = probe_gateway_message(&mh, false);

	if (result == -1) {
		ERROR(1, "Failed to probe for WA message");
		return -1;
	}

	if (result == 1) {
		// No message was found on the network, so return.
		return 1;
	}

	if (mh.opcode != OPCODE_DATA) {
		// We've run into a server message. Should never happen ?
		FATAL("Unexpected server message!");
	}

	// There is a data message in the gateway_in_buffer. Check if this is the message we are looking for.
	m = (data_message *) message_buffer_direct_read_access(gateway_in_buffer, 0);

	DEBUG(4, "Message received from opcode=%d src_pid=%d dst_pid=%d length=%d comm=%d source=%d dest=%d tag=%d count=%d",
			m->header.opcode, m->header.src_pid, m->header.dst_pid, m->header.length,
			m->comm, m->source, m->dest, m->tag, m->count);

	req = request_queue_dequeue_matching(receive_request_queue, m->comm, m->source, m->tag);

	if (req == NULL) {
		// No matching receive pending. Copy and queue the message if desired!
		if (copy) {
			DEBUG(5, "No match. Copying and storing message");
			m = (data_message *) read_generic_message(mh.length);
			store_message(m);
		}
		return 1;
	}

	// This is the right message, so unpack it into the application buffer.
	DEBUG(5, "Match. Directly unpacking message to application buffer!");

	// Skip the header.
	message_buffer_skip(gateway_in_buffer, DATA_MESSAGE_SIZE);

	buffer = message_buffer_direct_read_access(gateway_in_buffer, m->header.length-DATA_MESSAGE_SIZE);

	position = 0;

	req->error = PMPI_Unpack(buffer, m->header.length-DATA_MESSAGE_SIZE, &position, req->buf, req->count, req->type->type, req->c->comm);
	req->message_source = m->source;
	req->message_tag = m->tag;
	req->message_count = m->count;
	req->flags |= REQUEST_FLAG_COMPLETED;
	return 0;
}

int messaging_poll_sends(bool blocking)
{
	// See if we can push the send queue a bit!
	process_pending_sends(blocking);
	return EMPI_SUCCESS;
}

int messaging_poll_receives()
{
	process_pending_receives(true);
	return EMPI_SUCCESS;
}

int messaging_poll()
{
	process_pending_sends(false);
	process_pending_receives(false);
	return EMPI_SUCCESS;
}

// Probe if a message is available.
/*
int messaging_probe_receive(request *r, int blocking)
{
   int error;
   message_header mh;
   data_message *m;

   if ((r->flags & REQUEST_FLAG_COMPLETED)) {
      return EMPI_SUCCESS;
   }

   r->message = find_pending_message(r->c, r->source_or_dest, r->tag);

   if (r->message != NULL) {
	   r->error = unpack_message(r->buf, r->count, r->type, r->c, r->message, status);
	   r->flags |= REQUEST_FLAG_COMPLETED;
	   return EMPI_SUCCESS;
   }

   r->error = EMPI_SUCCESS;

   do {
      m = NULL;

      error = probe_gateway_message(&mh, blocking);

      if (error == -1) {
    	 ERROR(1, "Failed to receive WA message!");
    	 return EMPI_ERR_INTERN;
      }

      if (mh->opcode != OPCODE_DATA) {
    	  // We've run into a server message. This should not happen!
    	  FATAL(1, "Unexpected server message!");
      }





      if (mh->opcode != OPCODE_DATA) {





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
}*/

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

static int wait_for_server_reply(int opcode, message_header *mh)
{
	int error, position;
	data_message *m;
	request *req;
	unsigned char *buffer;

	while (true) {

		error = probe_gateway_message(mh, true);

		if (error != 0) {
			ERROR(1, "Failed to probe for WA message (error=%d)", error);
			return error;
		}

		if (mh->opcode == OPCODE_DATA) {
			// We've run into a data message. See is there is pending receive request for it...

			m = (data_message *) message_buffer_direct_read_access(gateway_in_buffer, 0);

			DEBUG(4, "Message received from opcode=%d src_pid=%d dst_pid=%d length=%d comm=%d source=%d dest=%d tag=%d count=%d",
					m->header.opcode, m->header.src_pid, m->header.dst_pid, m->header.length,
					m->comm, m->source, m->dest, m->tag, m->count);

			req = request_queue_dequeue_matching(receive_request_queue, m->comm, m->source, m->tag);

			if (req != NULL) {
				// Found a matching pending receive
				DEBUG(5, "Matched pending receive. Directly unpacking message to application buffer!");

				// Skip the header.
				message_buffer_skip(gateway_in_buffer, DATA_MESSAGE_SIZE);

				buffer = message_buffer_direct_read_access(gateway_in_buffer, m->header.length-DATA_MESSAGE_SIZE);

				position = 0;

				req->error = PMPI_Unpack(buffer, m->header.length-DATA_MESSAGE_SIZE, &position, req->buf, req->count, req->type->type, req->c->comm);
				req->message_source = m->source;
				req->message_tag = m->tag;
				req->message_count = m->count;
				req->flags |= REQUEST_FLAG_COMPLETED;
			} else {
				// No body is interresed, so receive and store for later!
				DEBUG(5, "No match. Copying and storing message");
				m = (data_message *) read_generic_message(mh->length);
				store_message(m);
			}

		} else if (mh->opcode != opcode) {
			// This should never happen!
			ERROR(1, "Received server reply with WRONG opcode (got %d expected %d)", mh->opcode, opcode);
			return EMPI_ERR_INTERN;
		} else {
			// We have found our message!
			return EMPI_SUCCESS;
		}
	}

	// Unreachable!
	return EMPI_ERR_INTERN;
}

static int *receive_int_array(int length)
{
	message_buffer *m;
	int error;
	int *tmp = malloc(length * sizeof(int));

	if (tmp == NULL) {
		ERROR(1, "Failed to allocate int[%ld]!", length * sizeof(int));
		return NULL;
	}

	m = message_buffer_wrap((unsigned char *)tmp, length * sizeof(int));

	if (m == NULL) {
		ERROR(1, "Failed to wrap int[%ld]!", length * sizeof(int));
		return NULL;
	}

	error = socket_receive_mb(gatewayfd, m, true);

	if (error != SOCKET_OK) {
		message_buffer_destroy(m);
		free(tmp);
		ERROR(1, "Failed to receive int[%ld]!", length * sizeof(int));
		return NULL;
	}

	return tmp;
}

int messaging_comm_split_send(communicator* c, int color, int key)
{
	int error;
	split_request_msg *req;

	error = ensure_space(SPLIT_REQUEST_MSG_SIZE, true);

	if (error != 0) {
		ERROR(1, "Failed to write SPLIT REQUEST to send buffer (error=%d)!", error);
		return EMPI_ERR_INTERN;
	}

	req = (split_request_msg *) message_buffer_direct_write_access(gateway_out_buffer, SPLIT_REQUEST_MSG_SIZE);

	if (req == NULL) {
		ERROR(1, "Failed to directly access buffer to write SPLIT REQUEST");
		return EMPI_ERR_INTERN;
	}

	req->header.opcode  = OPCODE_SPLIT;
	req->header.src_pid = my_pid;
	req->header.dst_pid = server_pid;
	req->header.length  = SPLIT_REQUEST_MSG_SIZE;
	req->comm  = c->handle;
	req->src   = c->global_rank;
	req->color = color;
	req->key   = key;

	// Since this split is a collective operation, we must completely write the send buffer here!
	error = socket_send_mb(gatewayfd, gateway_out_buffer, true);

	if (error != SOCKET_OK) {
		ERROR(1, "Failed to flush send buffer!");
		return EMPI_ERR_INTERN;
	}

	return EMPI_SUCCESS;
}

int messaging_comm_split_receive(comm_reply *reply)
{
	// We must now receive a split reply message of variable length:
	//
	//    message_header header;
	//    int newComm;        // communicator created
	//    int rank;           // rank in new communicator
	//    int size;           // size of new communicator
	//    int color;          // color for local split
	//    int key;            // key for local split
	//    int cluster_count;  // number of cluster in communicator
	//    int flags;          // flags of new communicator
	//    uint32_t cluster_coordinators[cluster_count] //  cluster coordinations
	//    uint32_t cluster_sizes[cluster_count]        //  cluster sizes
	//    uint32_t cluster_ranks[cluster_count]        //  cluster ranks
	//    uint32_t member_pids[size]                   //  member pids
	//    uint32_t member_cindex[size]                 //  member cluster index
	//    uint32_t member_ranks[size]                  //  member local ranks
	//
	// Since this is the result of a collective operation on a communicator (and this reply is sent in response to our own
	// request), we can assume here that the reply has not be received yet. Nor can there be any other -server- messages in the
	// receive buffer. It is possible, however, that -data- messages are ahead in the receive buffer.

	int error;
	split_reply_msg *msg;
	message_header mh;

	error = wait_for_server_reply(OPCODE_SPLIT_REPLY, &mh);

	if (error != EMPI_SUCCESS) {
		return error;
	}

	msg = (split_reply_msg *) message_buffer_direct_read_access(gateway_in_buffer, SPLIT_REPLY_MSG_SIZE);

	if (msg == NULL) {
		ERROR(1, "Failed to get direct read access to buffer!");
		return MPI_ERR_INTERN;
	}

	reply->newComm = msg->newComm;
	reply->rank = msg->rank;
	reply->size = msg->size;
	reply->color = msg->color;
	reply->key = msg->key;
	reply->cluster_count = msg->cluster_count;
	reply->flags = msg->flags;

	DEBUG(1, "*Received comm reply (newComm=%d rank=%d size=%d color=%d key=%d cluster_count=%d flag=%d)",
			reply->newComm, reply->rank, reply->size, reply->color, reply->key, reply->cluster_count, reply->flags);

	reply->coordinators = receive_int_array(reply->cluster_count);

	if (reply->coordinators == NULL) {
		ERROR(1, "Failed to allocate or receive coordinators");
		return EMPI_ERR_INTERN;
	}

	reply->cluster_sizes = receive_int_array(reply->cluster_count);

	if (reply->cluster_sizes == NULL) {
		ERROR(1, "Failed to allocate or receive cluster sizes");
		return EMPI_ERR_INTERN;
	}

	reply->cluster_ranks = receive_int_array(reply->cluster_count);

	if (reply->cluster_ranks == NULL) {
		ERROR(1, "Failed to allocate or receive cluster ranks");
		return EMPI_ERR_INTERN;
	}

	if (reply->size > 0) {

		reply->members = (uint32_t *) receive_int_array(reply->size);

		if (reply->members == NULL) {
			ERROR(1, "Failed to allocate or receive communicator members");
			return EMPI_ERR_INTERN;
		}

		reply->member_cluster_index = (uint32_t *) receive_int_array(reply->size);

		if (reply->member_cluster_index == NULL) {
			ERROR(1, "Failed to allocate or receive communicator member cluster index");
			return EMPI_ERR_INTERN;
		}

		reply->local_ranks = (uint32_t *) receive_int_array(reply->size);

		if (reply->local_ranks == NULL) {
			ERROR(1, "Failed to allocate or receive communicator member local ranks");
			return EMPI_ERR_INTERN;
		}
	} else {
		reply->members = NULL;
		reply->member_cluster_index = NULL;
		reply->local_ranks  = NULL;
	}

	return EMPI_SUCCESS;
}


int messaging_comm_create_send(communicator* c, group *g)
{
	int i, msgsize, error;
	group_request_msg *req;

	msgsize = GROUP_REQUEST_MSG_SIZE + (g->size*sizeof(int));

	DEBUG(1, "Sending group request of size %d", msgsize);

	error = ensure_space(msgsize, true);

	if (error != 0) {
		ERROR(1, "Failed to write COMM CREATE REQUEST to send buffer (error=%d)!", error);
		return EMPI_ERR_INTERN;
	}

	req = (group_request_msg *) message_buffer_direct_write_access(gateway_out_buffer, msgsize);

	if (req == NULL) {
		ERROR(1, "Failed to allocate space COMM_CREATE request!");
		return EMPI_ERR_INTERN;
	}

	req->header.opcode  = OPCODE_GROUP;
	req->header.src_pid = my_pid;
	req->header.dst_pid = server_pid;
	req->header.length  = msgsize;

	req->comm = c->handle;
	req->src = c->global_rank;
	req->size = g->size;

	for (i=0;i<g->size;i++) {
		req->members[i] = g->members[i];
	}

	DEBUG(1, "Group request header %d %d %d %d %d", req->header.opcode, req->header.length, req->comm, req->src, req->size);

	// Since this is a collective operation, we must completely write the send buffer here!
	error = socket_send_mb(gatewayfd, gateway_out_buffer, true);

	if (error != SOCKET_OK) {
		ERROR(1, "Failed to flush send buffer!");
		return EMPI_ERR_INTERN;
	}

	return EMPI_SUCCESS;
}



int messaging_comm_create_receive(group_reply *reply)
{
	// What we need to receive an group reply of variable length:
	//
	//    message_header header;
	//    int newComm;             // communicator created
	//    int rank;                // rank in new communicator
	//    int size;                // size of new communicator
	//    int type;                // type of group reply
	//    int cluster_count;       // number of clusters in communicator
	//    int flags;               // flags of new communicator
	//    uint32_t cluster_coordinatiors[cluster_count] //  cluster coordinations
	//    uint32_t cluster_sizes[cluster_count]         //  cluster sizes
	//    uint32_t cluster_ranks[cluster_count]         //  cluster ranks
	//    uint32_t member_pids[size]                    //  member pids
	//    uint32_t member_cindex[size]                  //  member cluster index
	//    uint32_t member_ranks[size]                   //  member local ranks
	//
	// Since this is the result of a collective operation on a communicator (and this reply is sent in response to our own
	// request), we can assume here that the reply has not be received yet. Nor can there be any other -server- messages in the
	// receive buffer. It is possible, however, that -data- messages are ahead in the receive buffer.

	int error;
	group_reply_msg *msg;
	message_header mh;

	error = wait_for_server_reply(OPCODE_GROUP_REPLY, &mh);

	if (error != EMPI_SUCCESS) {
		return error;
	}

	msg = (group_reply_msg *) message_buffer_direct_read_access(gateway_in_buffer, GROUP_REPLY_MSG_SIZE);

	if (msg == NULL) {
		ERROR(1, "Failed to get direct read access to buffer!");
		return MPI_ERR_INTERN;
	}

	reply->newComm = msg->newComm;
	reply->rank = msg->rank;
	reply->size = msg->size;
	reply->type = msg->type;
	reply->cluster_count = msg->cluster_count;
	reply->flags = msg->flags;

	DEBUG(1, "*Received group reply (newComm=%d rank=%d size=%d type=%d cluster_count=%d flag=%d)",
			reply->newComm, reply->rank, reply->size, reply->type, reply->cluster_count, reply->flags);

	if (reply->type == GROUP_TYPE_ACTIVE) {

		reply->coordinators = receive_int_array(reply->cluster_count);

		if (reply->coordinators == NULL) {
			ERROR(1, "Failed to allocate or receive coordinators");
			return EMPI_ERR_INTERN;
		}

		reply->cluster_sizes = receive_int_array(reply->cluster_count);

		if (reply->cluster_sizes == NULL) {
			ERROR(1, "Failed to allocate or receive cluster sizes");
			return EMPI_ERR_INTERN;
		}

		reply->cluster_ranks = receive_int_array(reply->cluster_count);

		if (reply->cluster_ranks == NULL) {
			ERROR(1, "Failed to allocate or receive cluster ranks");
			return EMPI_ERR_INTERN;
		}

		if (reply->size > 0) {

			reply->members = (uint32_t *) receive_int_array(reply->size);

			if (reply->members == NULL) {
				ERROR(1, "Failed to allocate or receive communicator members");
				return EMPI_ERR_INTERN;
			}

			reply->member_cluster_index = (uint32_t *) receive_int_array(reply->size);

			if (reply->member_cluster_index == NULL) {
				ERROR(1, "Failed to allocate or receive communicator member cluster index");
				return EMPI_ERR_INTERN;
			}

			reply->local_ranks = (uint32_t *) receive_int_array(reply->size);

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

	return EMPI_SUCCESS;
}

int messaging_comm_dup_send(communicator* c)
{
	int error;
	dup_request_msg *req;

	error = ensure_space(DUP_REQUEST_MSG_SIZE, true);

	if (error != 0) {
		ERROR(1, "Failed to write DUP REQUEST to send buffer (error=%d)!", error);
		return EMPI_ERR_INTERN;
	}

	req = (dup_request_msg *) message_buffer_direct_write_access(gateway_out_buffer, DUP_REQUEST_MSG_SIZE);

	req->header.opcode  = OPCODE_DUP;
	req->header.src_pid = my_pid;
	req->header.dst_pid = server_pid;
	req->header.length  = DUP_REQUEST_MSG_SIZE;
	req->comm           = c->handle;
	req->src            = c->global_rank;

	// Since this is a collective operation, we must completely write the send buffer here!
	error = socket_send_mb(gatewayfd, gateway_out_buffer, true);

	if (error != SOCKET_OK) {
		ERROR(1, "Failed to flush send buffer!");
		return EMPI_ERR_INTERN;
	}

	return EMPI_SUCCESS;
}

int messaging_comm_dup_receive(dup_reply *reply)
{
	// Since operations on communicators are collective operations, we can
	// assume here that the reply has not be received yet.
	int error;
	dup_reply_msg *msg;
	message_header mh;

	error = wait_for_server_reply(OPCODE_DUP_REPLY, &mh);

	if (error != EMPI_SUCCESS) {
		return error;
	}

	msg = (dup_reply_msg *) message_buffer_direct_read_access(gateway_in_buffer, DUP_REPLY_MSG_SIZE);

	if (msg == NULL) {
		ERROR(1, "Failed to get direct read access to buffer!");
		return MPI_ERR_INTERN;
	}

	reply->newComm = msg->newComm;

	DEBUG(1, "*Received dup reply (newComm=%d)", reply->newComm);

	return EMPI_SUCCESS;
}


int messaging_comm_free_send(communicator* c)
{
	int error;
	free_request_msg *req;

	error = ensure_space(FREE_REQUEST_MSG_SIZE, true);

	if (error != 0) {
		ERROR(1, "Failed to write FREE REQUEST to send buffer (error=%d)!", error);
		return EMPI_ERR_INTERN;
	}

	req = (free_request_msg *) message_buffer_direct_write_access(gateway_out_buffer, FREE_REQUEST_MSG_SIZE);

	if (req == NULL) {
		ERROR(1, "Failed to directly access buffer to write FREE REQUEST");
		return EMPI_ERR_INTERN;
	}

	req->header.opcode  = OPCODE_FREE;
	req->header.src_pid = my_pid;
	req->header.dst_pid = server_pid;
	req->header.length  = FREE_REQUEST_MSG_SIZE;
	req->comm           = c->handle;
	req->src            = c->global_rank;

	// Since this is a collective operation, we must completely write the send buffer here!
	error = socket_send_mb(gatewayfd, gateway_out_buffer, true);

	if (error != SOCKET_OK) {
		ERROR(1, "Failed to flush send buffer!");
		return EMPI_ERR_INTERN;
	}

	return EMPI_SUCCESS;
}

int messaging_finalize()
{
	// A finalize is an collective operation. It must be performed by all
	// application processes.

	int error, count;
	finalize_request_msg *req;
	finalize_reply_msg msg;
	message_header mh;

	communicator *c;

	c = handle_to_communicator(0);

	DEBUG(1, "Sending finalize request %d:%d", c->handle, c->global_rank);

	error = ensure_space(FINALIZE_REQUEST_MSG_SIZE, true);

	if (error != 0) {
		ERROR(1, "Failed to write FINALIZE REQUEST to send buffer (error=%d)!", error);
		return EMPI_ERR_INTERN;
	}

	req = (finalize_request_msg *) message_buffer_direct_write_access(gateway_out_buffer, FINALIZE_REQUEST_MSG_SIZE);

	if (req == NULL) {
		ERROR(1, "Failed to directly access buffer to write FINALIZE REQUEST");
		return EMPI_ERR_INTERN;
	}

	req->header.opcode  = OPCODE_FINALIZE;
	req->header.src_pid = my_pid;
	req->header.dst_pid = server_pid;
	req->header.length  = FINALIZE_REQUEST_MSG_SIZE;
	req->comm           = c->handle;
	req->src            = c->global_rank;

	// Since this is a collective operation, we must completely write the send buffer here!
	error = socket_send_mb(gatewayfd, gateway_out_buffer, true);

	if (error != SOCKET_OK) {
		ERROR(1, "Failed to flush send buffer!");
		return EMPI_ERR_INTERN;
	}

	DEBUG(1, "Finalize request %d:%d send", c->handle, c->global_rank);
	DEBUG(1, "Receiving finalize reply %d:%d send", c->handle, c->global_rank);

	error = wait_for_server_reply(OPCODE_FINALIZE_REPLY, &mh);

	if (error != EMPI_SUCCESS) {
		return error;
	}

	count = message_buffer_read(gateway_in_buffer, (unsigned char *) &msg, FINALIZE_REPLY_MSG_SIZE);

	if (count != FINALIZE_REPLY_MSG_SIZE) {
		ERROR(1, "INTERNAL ERROR: failed to receive FINALIZE reply from server! (error=%d)", error);
		return MPI_ERR_INTERN;
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


