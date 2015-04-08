#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
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
#include "virtual_connection.h"

#define INITIAL_ACTIVE_CONNECTIONS_SIZE 4

// Available sequence numbers is 2^31
#define SEQUENCE_NUMBERS (0x80000000)

// The size of the send window.
#define MAX_PENDING_BYTES (128*1024*1024)

// The point at which an ACK is send (2=halfway buffer, 4=quarter buffer, etc).
#define ACK_POINT (8)

// Size of the node cache in the receive request queue (shared by all).
#define RECEIVE_REQUEST_QUEUE_CACHE_SIZE (16)

// Size of the node cache in send request queue (per virtual connection).
#define SEND_REQUEST_QUEUE_CACHE_SIZE (4)

// A generic server message.
typedef struct {
	int opcode;  // opcode of operation
	int length; // length of the entire message, including this header. (NOTE: may be much larger than fragment_size)
} server_message;

#define SERVER_MSG_SIZE (2*sizeof(int))

// This message is used to request a comm-split at the server.
typedef struct {
	server_message header;
	int comm;    // communicator used
	int src;     // rank in current communicator
	int color;   // target new communicator
	int key;     // prefered rank in target communicator
} split_request_msg;

#define SPLIT_REQUEST_MSG_SIZE (SERVER_MSG_SIZE + 4*sizeof(int))

// This message is used to request a comm-from-group creation at the server.
typedef struct {
	server_message header;
	int comm;    // communicator used
	int src;     // rank in current communicator
	int size;    // number of ranks in group
	int members[]; // member ranks for group
} group_request_msg;

#define GROUP_REQUEST_MSG_SIZE (SERVER_MSG_SIZE + 3*sizeof(int))

// This message is used to request a comm-dup at the server.
typedef struct {
	server_message header;
	int comm;    // communicator used
	int src;     // rank in current communicator
} dup_request_msg;

#define DUP_REQUEST_MSG_SIZE (SERVER_MSG_SIZE + 2*sizeof(int))

// This message is used to request a comm-free at the server.
typedef struct {
	server_message header;
	int comm;    // communicator used
	int src;     // rank in current communicator
} free_request_msg;

#define FREE_REQUEST_MSG_SIZE (SERVER_MSG_SIZE + 2*sizeof(int))

// This message is used to request an finalize at the server.
typedef struct {
	server_message header;
	int comm;    // communicator used
	int src;     // rank in current communicator
} finalize_request_msg;

#define FINALIZE_REQUEST_MSG_SIZE (SERVER_MSG_SIZE + 2*sizeof(int))

// This the servers reply to a comm-split request.
typedef struct {
	server_message header;
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

#define SPLIT_REPLY_MSG_SIZE (SERVER_MSG_SIZE + 7*sizeof(int))

// This the servers reply to a group-to-comm request.
typedef struct {
	server_message header;
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

#define GROUP_REPLY_MSG_SIZE (SERVER_MSG_SIZE + 6*sizeof(int))

// This is the reply to a dup request.
typedef struct {
	server_message header;
	int newComm; // communicator created
} dup_reply_msg;

#define DUP_REPLY_MSG_SIZE (SERVER_MSG_SIZE + 1*sizeof(int))

// This is the reply to a finalize request.
typedef struct {
	server_message header;
} finalize_reply_msg;

#define FINALIZE_REPLY_MSG_SIZE (SERVER_MSG_SIZE)

// One virtual connection for each process in this application. Note that these will be created on demand, so with applications
// with limited number of connections per process the overhead is small.
static virtual_connection **connections;

// Array containing virtual connections that have data to send.
static virtual_connection **active_connections;
static int active_connections_size;
static int next_active_connection;

// The number of clusters and the rank of our cluster in this set.
uint32_t cluster_count;
uint32_t cluster_rank;

// The size of each cluster, and the offset of each cluster in the total set of machines.
int *cluster_sizes;
int *cluster_offsets;

// The total number of processes in this MPI run.
static uint32_t total_size;

// The PID of this process. Used when forwarding messages.
uint32_t my_pid;

// The PID of the server process. Used in communicator and group operations.
uint32_t server_pid;

// All information about the gateway. Note that gateway_name is only valid on rank 0.
static char *gateway_name;
static long gateway_ipv4;
static unsigned short gateway_port;

// The socket connected to the gateway.
static int gatewayfd;

// Queue of pending receive requests.
static request_queue *receive_request_queue;

// Fragment that is currently being received from the gateway.
static generic_message *receive_fragment;
static size_t receive_fragment_pos;

// Fragment that is currently being send to the gateway.
static generic_message *send_fragment;
static size_t send_fragment_pos;
static size_t send_fragment_connection_index;

// Maximum fragment size.
static size_t fragment_size;

/*****************************************************************************/
/*                      Initialization / Finalization                        */
/*****************************************************************************/

static int read_config_file() {
	int error;
	char *file;
	FILE *config;
	char buffer[1024];

	file = getenv("EMPI_APPLICATION_CONFIG");

	if (file == NULL) {
		WARN(0, "EMPI_APPLICATION_CONFIG not set!");
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

	gateway_name = malloc(strlen(buffer + 1));

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

int messaging_init(int rank, int size, int *argc, char ***argv) {
	int error, i;
	uint32_t msg[4];

	active_connections = calloc(INITIAL_ACTIVE_CONNECTIONS_SIZE, sizeof(virtual_connection *));
	active_connections_size = INITIAL_ACTIVE_CONNECTIONS_SIZE;
	next_active_connection = 0;

	receive_request_queue = request_queue_create(RECEIVE_REQUEST_QUEUE_CACHE_SIZE);

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

		error = socket_connect(gateway_ipv4, gateway_port, 0, 0, &gatewayfd);

		if (error != SOCKET_OK) {
			ERROR(1, "Failed to connect to gateway! (error=%d)", error);
			return EMPI_ERR_GATEWAY;
		}

		msg[0] = OPCODE_HANDSHAKE;
		msg[1] = rank;
		msg[2] = size;

		error = socket_sendfully(gatewayfd, (unsigned char *) &msg, 3 * sizeof(uint32_t));

		if (error != SOCKET_OK) {
			ERROR(1, "Failed to complete handshake with gateway! (error=%d)", error);
			return EMPI_ERR_GATEWAY;
		}

		error = socket_receivefully(gatewayfd, (unsigned char *) &msg, 4 * sizeof(uint32_t));

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
		fragment_size = msg[3];
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

	error = PMPI_Bcast(&fragment_size, 1, MPI_INT, 0, MPI_COMM_WORLD);

	if (error != MPI_SUCCESS) {
		ERROR(1, "Failed to broadcast fragment_size! (error=%d)", error);
		return EMPI_ERR_INTERN;
	}

	if (rank == 0) {
		WARN(1, "Size of this cluster %d", size);
		WARN(1, "Rank of this cluster %d", cluster_rank);
		WARN(1, "Number of clusters %d", cluster_count);
		WARN(1, "Message fragmentation size %d", fragment_size);
	}

	// Allocate space for send / receive fragments
	receive_fragment = (generic_message *) malloc(fragment_size);
	receive_fragment_pos = 0;

	send_fragment = (generic_message *) malloc(fragment_size);
	send_fragment_connection_index = -1;
	send_fragment_pos = 0;

	// Allocate space for the cluster information
	cluster_sizes = malloc(cluster_count * sizeof(int));
	cluster_offsets = malloc((cluster_count + 1) * sizeof(int));

	if (cluster_sizes == NULL || cluster_offsets == NULL) {
		ERROR(1, "Failed to allocate space for cluster info!");
		return EMPI_ERR_INTERN;
	}

	// Rank 0 receives cluster information from the gateway
	if (rank == 0) {

		error = socket_receivefully(gatewayfd, (unsigned char *) cluster_sizes, cluster_count * sizeof(int));

		if (error != SOCKET_OK) {
			ERROR(1, "Failed to receive cluster sizes from gateway! (error=%d)",
					error);
			return EMPI_ERR_GATEWAY;
		}

		error = socket_receivefully(gatewayfd,
				(unsigned char *) cluster_offsets,
				(cluster_count + 1) * sizeof(int));

		if (error != SOCKET_OK) {
			ERROR(1,
					"Failed to receive cluster offsets from gateway! (error=%d)",
					error);
			return EMPI_ERR_GATEWAY;
		}
	}

	// Tell everyone about the cluster sizes and offsets.
	error = PMPI_Bcast(cluster_sizes, cluster_count, MPI_INT, 0, MPI_COMM_WORLD);

	if (error != MPI_SUCCESS) {
		ERROR(1, "Failed to broadcast cluster sizes! (error=%d)", error);
		return EMPI_ERR_INTERN;
	}

	error = PMPI_Bcast(cluster_offsets, (cluster_count + 1), MPI_INT, 0, MPI_COMM_WORLD);

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

	// Allocate space for all virtual connections.
	//
	// We allocate one slot here for each process in this application. Each slot is initially set to NULL. On first communication
	// with a specific process a "virtual_connection" struct is allocated and stored in the slot. This is only done for -remote-
	// processes (on another cluster). Slots representing local processes will not be used.
	//
	// TODO: possible room for optimization is to only allocate slots for remote processes, but this doesn't save much space...
	//
	total_size = 0;

	for (i = 0; i < cluster_count; i++) {
		total_size += cluster_sizes[i];
	}

	connections = malloc((total_size + 1) * sizeof(virtual_connection *));

	for (i = 0; i < total_size+1; i++) {
		connections[i] = NULL;
	}

	// Generate my PID.
	my_pid = SET_PID(cluster_rank, rank);

	// The server PID is all 1's
	server_pid = 0xFFFFFFFF;

	// Create a special virtual connection to the server.
	connections[total_size] = virtual_connection_create(total_size, server_pid, -1, 1);

	// Next, all other ranks connect to the gateway
	if (rank != 0) {

		error = socket_connect(gateway_ipv4, gateway_port, 0, 0, &gatewayfd);

		if (error != SOCKET_OK) {
			ERROR(1, "Failed to connect to gateway! (error=%d)", error);
			return EMPI_ERR_GATEWAY;
		}

		msg[0] = OPCODE_HANDSHAKE;
		msg[1] = rank;
		msg[2] = size;

		error = socket_sendfully(gatewayfd, (unsigned char *) &msg, 3 * sizeof(uint32_t));

		if (error != SOCKET_OK) {
			ERROR(1, "Failed to complete handshake with gateway! (error=%d)", error);
			return EMPI_ERR_GATEWAY;
		}

		error = socket_receivefully(gatewayfd, (unsigned char *) &msg, 1 * sizeof(uint32_t));

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

	// Next, rank 0 waits for the all clear from the gateway.
	if (rank == 0) {

		error = socket_receivefully(gatewayfd, (unsigned char *) &msg, 1 * sizeof(uint32_t));

		if (error != SOCKET_OK) {
			ERROR(1, "Failed to complete handshake with gateway! (error=%d)", error);
			return EMPI_ERR_GATEWAY;
		}

		if (msg[0] != OPCODE_GATEWAY_READY) {
			ERROR(1, "Gateway did not give all clear!");
			close(gatewayfd);
			return EMPI_ERR_GATEWAY;
		}
	}

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

int match_message(data_message *m, int comm, int source, int tag) {
	int result = ((comm == m->comm)
			&& (source == EMPI_ANY_SOURCE || source == m->source)
			&& (tag == EMPI_ANY_TAG || tag == m->tag));

	DEBUG(5, "MATCH_MESSAGE: (comm=%d source=%d [any=%d] tag=%d [any=%d]) == (m.comm=%d m.source=%d m.tag=%d) => %d",
			comm, source, EMPI_ANY_SOURCE, tag, EMPI_ANY_TAG,
			m->comm, m->source, m->tag, result);

	return result;
}

/*****************************************************************************/
/*                         Active connection utils                           */
/*****************************************************************************/

static virtual_connection *select_active_connection() {
	int i;
	uint32_t pending;
	virtual_connection *vc;

	//WARN(3, "Selecting active connection %d", next_active_connection);

	for (i = 0; i < next_active_connection; i++) {

		vc = active_connections[i];

		//WARN(3, "Testing VC %d as active %d", vc->index, i);

		if (vc->force_ack) {
			// This connection must send an ACK (and maybe some data).
			//WARN(1, "VC %d selected as active (needs ACK)", vc->index);
			return vc;
		}

		if (vc->send_position < vc->send_length) {
			// This connection has some data to send, but do we have credits to send it ?

			if (vc->sliding_window_size == -1) {
				// No sliding window used, so we can send as much as we like.
				//WARN(1, "VC %d selected as active (no FC)", vc->index);
				return vc;
			}

			// Calculate the number of pending message fragments. Make sure to wrap around where needed.
			pending = ((vc->transmit_sequence + SEQUENCE_NUMBERS)
					- vc->acknowledged_sequence_received) % SEQUENCE_NUMBERS;

			if (pending <= vc->sliding_window_size) {
				// There is room in the send window for this fragment, so return the connections.
				//WARN(1, "VC %d selected as active", vc->index);

				return vc;
			}
			// HIERO
			//else {
			//WARN(1, "VC %d has run out of it window pending=%d window=%d tx=%d rx=%d", vc->index, pending,
					//vc->sliding_window_size, vc->transmit_sequence, vc->acknowledged_sequence_received);
			//}
			// Not enough credits, so try the next connection....

		} else {
			// Should not happen!
			FATAL("INTERNAL ERROR: Found active connection without data to send!");
		}
	}

	return NULL;
}

static int copy_fragment_to_send(virtual_connection *vc, generic_message *m) {

	size_t avail = 0;
//	uint32_t cluster, process;

	if (vc->send_message != NULL) {
		// The virtual connection has a data message waiting, so calculate the number of bytes that need to be send.
		avail = vc->send_length - vc->send_position;

		if (avail == 0) {
			// Should not happen!
			FATAL("Failed to copy active fragment from virtual connection: no data left in message!");
		}

		// If there are too many bytes we fragment the message.
		if (avail > (fragment_size - GENERIC_MESSAGE_HEADER_SIZE)) {
			avail = (fragment_size - GENERIC_MESSAGE_HEADER_SIZE);
		}

	} else if (vc->force_ack) {
		// The virtual connection has no data message waiting, but must send an ack for flow control.
		avail = 0;
		vc->send_opcode = OPCODE_ACK;
	} else {
		// Should not happen!
		FATAL("INTERNAL ERROR: selected virtual connection %d with no data to send!", vc->index);
	}

	// Write the header fields of the message fragment.
	m->flags = SET_FLAGS_FIELD(0, vc->send_opcode);
	m->src_pid = my_pid;
	m->dst_pid = vc->peer_pid;
	m->transmit_seq = vc->transmit_sequence;
	m->ack_seq = vc->receive_sequence;
	m->length = GENERIC_MESSAGE_HEADER_SIZE + avail;

	if (avail > 0) {
		INFO(1, "copy_fragment_to_send peer=%d:%d length=%d offset=%d", GET_CLUSTER_RANK(vc->peer_pid),
				GET_PROCESS_RANK(vc->peer_pid), avail, vc->send_position);

		// Copy the data to the fragment
		memcpy(&(m->payload), vc->send_message + vc->send_position, avail);

		// Update the position.
		vc->send_position += avail;
	} else {
		DEBUG(1, "Send ack to %d:%d with sequence number tx=%d rx=%d", GET_CLUSTER_RANK(vc->peer_pid),
				GET_PROCESS_RANK(vc->peer_pid), m->transmit_seq, m->ack_seq);
	}

	// Update the sliding window administration (even if no flow control is used).
	vc->transmit_sequence = (vc->transmit_sequence + 1) % SEQUENCE_NUMBERS;
	vc->acknowledged_sequence_sent = vc->receive_sequence;

//	if (vc->force_ack) {
//		FATAL("HIERO!");
//	}

	vc->force_ack = false;
	return 0;
}

static int select_fragment_to_send() {
	// NOTE: by varying how we handle the active connections here, we can influence how the messages are send.
	//
	// Current implementation is FIFO until somebody blocks. This way message fragments belonging to the same message are likely
	// to be send one after the other.
	//
	// An alternative is to use a round robin approach (dequeue from head, send 1 fragment, enqueue at tail). This will balance
	// the available bandwidth over the active connections.

	virtual_connection *tmp = select_active_connection();

	if (tmp == NULL) {
		// No active connection found. Either no one is sending, or no on has credits to send.
		//WARN(1, "No active VC found!");
		return 1;
	}

	//WARN(1, "Found active VC %d!", tmp->index);

	send_fragment_connection_index = tmp->index;
	send_fragment_pos = 0;

	return copy_fragment_to_send(tmp, send_fragment);
}

static int add_active_sender(virtual_connection *vc) {
	virtual_connection **tmp;

	if (vc->active_sender) {
		// Already active!
		//WARN(2, "VC %d already active -- cannot add!", vc->index);
		return 1;
	}

	//WARN(2, "Adding VC %d as active sender %d!", vc->index, next_active_connection);

	vc->active_sender = true;

	if (next_active_connection == active_connections_size) {
		// We have run out of space in the active connection array, so resize!
		tmp = active_connections;

		// FIXME: change to INFO when done
		//WARN(1, "Increasing size of active connection list from %d to %d",
		//		active_connections_size, active_connections_size * 2);

		active_connections = calloc(2 * active_connections_size,
				sizeof(virtual_connection *));

		if (active_connections == NULL) {
			ERROR(0, "Failed to allocate space for active connection index!");
			return -1;
		}

		active_connections_size = active_connections_size * 2;

		memcpy(active_connections, tmp,
				active_connections_size * sizeof(virtual_connection *));

		free(tmp);
	}

	active_connections[next_active_connection] = vc;
	next_active_connection++;
	return 0;
}

static int remove_active_sender(virtual_connection *vc) {
	int i;

	if (!vc->active_sender) {
		// Not active!
		return 1;
	}

	for (i = 0; i < next_active_connection; i++) {
		if (active_connections[i] == vc) {
			// Found it, so swap with last one and decrease the counter. Also works if list has only one element.
			next_active_connection--;
			active_connections[i] = active_connections[next_active_connection];
			active_connections[next_active_connection] = NULL;
			vc->active_sender = false;
			return 0;
		}
	}

	// Should not happen!
	ERROR(0, "INTERNAL ERROR: Active connection %d not found in list!", index);
	return -1;
}

/*****************************************************************************/
/*                         Application Communication                         */
/*****************************************************************************/

static void free_data_message(data_message *m) {
	if (m == NULL) {
		return;
	}

	free(m);
}

// Unpack a message from a data_message;
static int unpack_message(void *buf, int count, datatype *t, communicator *c,
		data_message *m, EMPI_Status *s) {
	int error = 0;
	int position = 0;

	error =
			TRANSLATE_ERROR(
					PMPI_Unpack(m->payload, m->length-DATA_MESSAGE_SIZE, &position, buf, count, t->type, c->comm));

	if (error == EMPI_SUCCESS) {
		set_status(s, m->source, m->tag, error, t, count, FALSE);
	}

	free_data_message(m);
	return error;
}

static int receive_message_fragment() {
	// This function reads -at most- a single message from the network. Depending on the blocking parameter, it will continue
	// reading until the complete message is received, or return as soon as the network is empty.
	//
	// Each message may contain a fragment of a larger data message, which essentialy is a serialized MPI message with a header.
	//
	// We currently assume that a single source send -one- message at a time, and that messages from a single source are -NOT-
	// reordered by the network. This implies that a sequence of fragments from a single source arrive in the correct order, and
	// that fragments from multiple messages from the same source so not mix.
	//
	// HOWEVER: since multiple sources may send to the same receiver simultaneously, we may get a mix of fragments from different
	// senders. After receiving a message, we therefore have to 'demultiplex' it to the correct 'stream'.
	//
	// NOTE: that we also make a copy here. We first receive the complete message. Then use the header to select the correct
	// destination stream, and copy the payload to the destination buffer. There is room from optimization here...
	//
	// NOTE: the 'no message reordering assumption' has implications for the implementation of the gateway processes!

//	int i;
	size_t to_read;
	ssize_t bytes_read;

	//WARN(1, "START READ at position %ld", receive_fragment_pos);

	// NOTE: We receive at most one message here.
	while (true) {

		// Check if there is enough data available for a message header.
		if (receive_fragment_pos >= GENERIC_MESSAGE_HEADER_SIZE) {

			// There should be at least a message header in the buffer!
//			WARN(1, "GOT WA MESSAGE opcode=%d src=%d:%d dst=%d:%d length=%d", receive_fragment->header.opcode,
//					GET_CLUSTER_RANK(receive_fragment->header.src_pid), GET_PROCESS_RANK(receive_fragment->header.src_pid),
//					GET_CLUSTER_RANK(receive_fragment->header.dst_pid), GET_PROCESS_RANK(receive_fragment->header.dst_pid),
//					receive_fragment->header.length);

			if (receive_fragment_pos == receive_fragment->length) {
				// There is a complete message in the buffer!
				INFO(1, "MESSAGE RECV COMPLETE for magic=%d:%d flag=%d opcode=%d src=%d:%d dst=%d:%d seq=%d ack=%d length=%d",
						GET_MAGIC0(receive_fragment->flags),
						GET_MAGIC1(receive_fragment->flags),
						GET_FLAGS(receive_fragment->flags),
						GET_OPCODE(receive_fragment->flags),
						GET_CLUSTER_RANK(receive_fragment->src_pid),
						GET_PROCESS_RANK(receive_fragment->src_pid),
						GET_CLUSTER_RANK(receive_fragment->dst_pid),
						GET_PROCESS_RANK(receive_fragment->dst_pid),
						receive_fragment->transmit_seq,
						receive_fragment->ack_seq,
						receive_fragment->length);
				return 0;
			}

			to_read = receive_fragment->length - receive_fragment_pos;
		} else {
			to_read = GENERIC_MESSAGE_HEADER_SIZE - receive_fragment_pos;
		}

//		WARN(1, "WILL READ %d to position %ld", to_read, receive_fragment_pos);

		//WARN(2, "MESSAGE INCOMPLETE pos=%d len=%d toread=%d", receive_fragment_pos, receive_fragment->length, to_read);

		// Not enough data for a complete message, so we poll the socket and attempt to receive some data.
		bytes_read = socket_receive(gatewayfd, ((unsigned char *) receive_fragment) + receive_fragment_pos, to_read, false);

//		WARN(2, "READ %d bytes", bytes_read);

		if (bytes_read > 0) {
			receive_fragment_pos += bytes_read;

//			WARN(1, "READ %d position now %ld", bytes_read, receive_fragment_pos);
		} else if (bytes_read == 0) {
			// We give up if we fail to read any data and we are in non-blocking mode.
			return 1;
		} else { // bytes_read < 0
			ERROR(1, "Failed to read gateway message!");
			return EMPI_ERR_GATEWAY;
		}

		//INFO(2, "READ %d bytes", bytes_read);

		//for (i=0;i<bytes_read;i++) {
//			INFO(2, " byte[%d]=%d", i, *(((unsigned char *) receive_fragment) + receive_fragment_pos + i));
		//}


	}

	// Unreachable
	return 1;
}

static void process_ack_message() {
	// We've received an ACK message for a certain virtual connection.
	int pid, cluster, process, index;
	virtual_connection *vc;

	// First find the index of the virtual connection we are interested in.
	pid = receive_fragment->src_pid;

	cluster = GET_CLUSTER_RANK(pid);
	process = GET_PROCESS_RANK(pid);

	index = cluster_offsets[cluster] + process;

	vc = connections[index];

	DEBUG(1, "Received ack for %d:%d with sequence number tx=%d rx=%d expected tx=%d rx=%d", cluster, process,
			receive_fragment->transmit_seq,
			receive_fragment->ack_seq,
			vc->transmit_sequence,
			vc->receive_sequence);

	if (receive_fragment->transmit_seq != vc->receive_sequence) {
		FATAL("INTERNAL ERROR: Received message fragment out of order %d != %d !", receive_fragment->transmit_seq,
				vc->receive_sequence);
	}

	// Save the ACK contained in the message. This will allow us to send some data later.
	vc->acknowledged_sequence_received = receive_fragment->ack_seq;

	// Increase the sequence number of the expected packet.
	vc->receive_sequence = (vc->receive_sequence + 1) % SEQUENCE_NUMBERS;
}

static void deliver_data_message(data_message *m) {

	int position;
	request *req;
	unsigned char *buffer;

	INFO(4, "Data message received from src=%d dst=%d length=%d comm=%d tag=%d count=%d",
			m->source, m->dest, m->length, m->comm, m->tag, m->count);

	req = request_queue_dequeue_matching(receive_request_queue, m->comm, m->source, m->tag);

	if (req == NULL) {
		store_message(m);
		return;
	}

	// This is the right message, so unpack it into the application buffer.
	INFO(5, "Found matching request. Directly unpacking message to application buffer!");

	// Get read access to the message
	buffer = &(m->payload[0]);

	position = 0;

	req->error = PMPI_Unpack(buffer, m->length - DATA_MESSAGE_SIZE, &position, req->buf, req->count, req->type->type,
			req->c->comm);

	req->message_source = m->source;
	req->message_tag = m->tag;
	req->message_count = m->count;
	req->flags |= REQUEST_FLAG_COMPLETED;

	free_data_message(m);
}

static int create_message_to_receive(virtual_connection *vc, uint32_t opcode, size_t length) {

	if (vc->receive_message != NULL) {
		FATAL("INTERNAL ERROR: Attempt do overwrite received data message!");
	}

	vc->receive_message = malloc(length);

	if (vc->receive_message == NULL) {
		FATAL("Failed to allocate space for received data message!");
	}

	vc->receive_position = 0;
	vc->receive_length = length;
	vc->receive_opcode = opcode;

	return 0;
}

static void process_data_fragment() {

	// We've received an DATA message for a certain virtual connection.
	int pid, cluster, process, index;
	uint32_t window_used;
	virtual_connection *vc;
	data_message *dm;

	// First find the index of the virtual connection we are interested in.
	pid = receive_fragment->src_pid;

	cluster = GET_CLUSTER_RANK(pid);
	process = GET_PROCESS_RANK(pid);

	index = cluster_offsets[cluster] + process;

	INFO(1, "Received data message from %d:%d", cluster, process);

	// Next, check if the virtual connection already exists. If not we create one on the fly.
	if (connections[index] == NULL) {

		// No active connection to cluster:process yet, so create it. FIXME: change to INFO once debugged.
		INFO(0, "Creating virtual connection to %d:%d", cluster, process);

		connections[index] = virtual_connection_create(index, pid,
				MAX_PENDING_BYTES / fragment_size, SEND_REQUEST_QUEUE_CACHE_SIZE);

		if (connections[index] == NULL) {
			FATAL("Failed to create virtual connection to destination %d:%d", cluster, process);
		}
	}

	vc = connections[index];

	// Next, we check the sequence number of the message fragment to make sure we are not receiving out-of-order.
	if (receive_fragment->transmit_seq != vc->receive_sequence) {
		FATAL("INTERNAL ERROR: Received message fragment out of order %d != %d !", receive_fragment->transmit_seq,
				vc->receive_sequence);
	}

	// Next, we check if the virtual connection has an active receive message. If not we create one.
	if (vc->receive_message == NULL) {
		dm = (data_message *) &(receive_fragment->payload);
		create_message_to_receive(vc, OPCODE_DATA, dm->length);
	}

	// Next, we update the receive sequence number, wrapping around if needed.
	vc->receive_sequence = (vc->receive_sequence + 1) % SEQUENCE_NUMBERS;

	// We also save the ACK contained in the message. This will allow us to send some data later.
	vc->acknowledged_sequence_received = receive_fragment->ack_seq;

	// Check if we have exhausted the sliding window. If so, we may need to force the send of an ACK.
	window_used = ((vc->receive_sequence + SEQUENCE_NUMBERS) - vc->acknowledged_sequence_sent) % SEQUENCE_NUMBERS;

INFO(1, "Windows used on VC %d window_used=%d last seq rec=%d last ack sent=%d",
		vc->index, window_used, vc->receive_sequence, vc->acknowledged_sequence_sent);

// FIXME TODO FIXME TODO!!!
	if (window_used >= vc->sliding_window_size / ACK_POINT) {
// FIXME: too late ? should ack earlier, as it will take a round trip for the next message to come in!
		// Force an ack.

INFO(1, "Will FORCE ACK on VC %d window_used=%d last seq rec=%d last ack sent=%d",
		vc->index, window_used, vc->receive_sequence, vc->acknowledged_sequence_sent);

		vc->force_ack = true;
		vc->acknowledged_sequence_sent = vc->receive_sequence;

		add_active_sender(vc);
	}

	// Next we copy the fragment payload to the data message
	memcpy(vc->receive_message + vc->receive_position, &(receive_fragment->payload),
			receive_fragment->length-GENERIC_MESSAGE_HEADER_SIZE);

	vc->receive_position += receive_fragment->length-GENERIC_MESSAGE_HEADER_SIZE;

	// Check if the data message is complete
	if (vc->receive_position == vc->receive_length) {

		// Message complete, so deliver it.
		deliver_data_message((data_message *) vc->receive_message);

		vc->receive_message = NULL;
		vc->receive_length = 0;
		vc->receive_opcode = 0;
		vc->receive_position = 0;
	} else {
		INFO(4, "Data message NOT delivered %d %d!", vc->receive_position, vc->receive_length);
	}
}

static void process_server_fragment() {

	// FIXME: merge with process data fragment!!!!

	// We've received an SERVER message reply.
	virtual_connection *vc;
	server_message *sm;

//	WARN(1, "Received server reply");

	// Get the virtual connection to the server
	vc = connections[total_size];

	// Next, we check the sequence number of the message fragment to make sure we are not receiving out-of-order.
	if (receive_fragment->transmit_seq != vc->receive_sequence) {
		FATAL("INTERNAL ERROR: Received message fragment out of order!");
	}

	// Next, we check if the virtual connection to the server has an active receive message.
	if (vc->receive_message == NULL) {
		// No active receive message, so create one now!
		sm = (server_message *) &(receive_fragment->payload);
		create_message_to_receive(vc, OPCODE_SERVER, sm->length);
	}

	// Next we copy the fragment payload to the data message
	memcpy(vc->receive_message + vc->receive_position,
			&(receive_fragment->payload), receive_fragment->length-GENERIC_MESSAGE_HEADER_SIZE);

	// Next, we update the receive sequence number, wrapping around if needed.
	vc->receive_sequence = (vc->receive_sequence + 1) % SEQUENCE_NUMBERS;

	// We also save the ACK contained in the message. This will allow us to send some data later.
	vc->acknowledged_sequence_received = receive_fragment->ack_seq;

	/* No flow control on server link, so we do not check the window for necessary acks ...

	 // Check if we have exhausted the sliding window. If so, we may need to force the send of an ACK.
	 window_used = ((vc->receive_sequence + SEQUENCE_NUMBERS) - vc->acknowledged_sequence_sent) % SEQUENCE_NUMBERS;

	 if (window_used >= vc->sliding_window_size) {
	 // Force an ack.
	 vc->force_ack = true;
	 add_active_sender(vc);
	 }

	 */

	vc->receive_position += receive_fragment->length;
}

static int poll_receive() {

	int result;
	int opcode;

	while (true) {

		result = receive_message_fragment();

		if (result == 1) {
			// No (complete) message fragment was received.
			return 1;
		} else if (result == -1) {
			ERROR(1, "Failed to probe for WA message");
			return -1;
		}

		opcode = GET_OPCODE(receive_fragment->flags);

		switch (opcode) {
		case OPCODE_DATA:
			// We've received a data message fragment, so forward it to the demux and defrag layer.
			process_data_fragment();
			break;
		case OPCODE_SERVER:
			// We've received a server message fragment, so forward it to the demux and defrag layer.
			process_server_fragment();
			break;
		case OPCODE_ACK:
			// We've received an ACK message, so process it.
			process_ack_message();
			break;
		default:
			// We've received an unknown message type, so complain about it!
			FATAL("Received generic message with unknown opcode: %d!", opcode);
			return -1;
		}

		receive_fragment_pos = 0;

		// FIXME: keep going ?
		return 0;
	}

	// Unreachable
	return -1;
}

static int create_data_message_to_send(virtual_connection *vc) {
	int bytes, error, tmp;
	data_message *m;
	request *req;

	req = vc->current_send_request;

	// FIXME FIXME FIXME: This MPI_Pack always makes a copy, but it is only required for non-contiguous data types.
	// However, since we assume the wide area network is the bottleneck, not the local system, we are OK with this for now.

	// Get the data size
	error = PMPI_Pack_size(req->count, req->type->type, req->c->comm, &bytes);

	if (error != MPI_SUCCESS) {
		ERROR(1, "Failed to determine size of data message (error=%d)!", error);
		return -1;
	}

	DEBUG(2, "Data message size is %d bytes", bytes);

	m = (data_message *) malloc(DATA_MESSAGE_SIZE + bytes);

	if (m == NULL) {
		ERROR(1, "Failed to allocate buffer needed to pack data message!");
		return -1;
	}

	m->length = DATA_MESSAGE_SIZE + bytes;
	m->comm = req->c->handle;
	m->source = req->c->global_rank;
	m->dest = req->source_or_dest;
	m->tag = req->tag;
	m->count = req->count;

	// Next, copy the message payload
	DEBUG(2, "Packing payload into send message");

	// Copy the data to the message
	tmp = 0;
	error = PMPI_Pack(req->buf, req->count, req->type->type, &(m->payload),	bytes, &tmp, req->c->comm);

	if (error != MPI_SUCCESS) {
		ERROR(1, "Failed to pack payload into data message! (error=%d)", TRANSLATE_ERROR(error));
		return -1;
	}

	vc->send_message = (unsigned char *) m;
	vc->send_length = m->length;
	vc->send_opcode = OPCODE_DATA;

	return 0;
}

static void finish_active_message_fragment() {
	virtual_connection *tmp;

	// We check if this was the last fragment of a larger data message.
	tmp = connections[send_fragment_connection_index];

	if (tmp->send_position == tmp->send_length) {
		// We've finished writing this message.

		if (send_fragment_connection_index == total_size) {

INFO(1, "finish_active_message_fragment SERVER");

			// this is the server link, so only one send at a time and no request used.
			tmp->send_message = NULL;
			tmp->send_length = 0;
			remove_active_sender(tmp);

		} else {

INFO(1, "finish_active_message_fragment COMPUTE NODE");

			// Check if there was a send request (will be NULL for a forced ACK)
			if (tmp->current_send_request != NULL) {
				// complete request
				tmp->current_send_request->flags |= REQUEST_FLAG_COMPLETED;

				// Free data message
				free(tmp->send_message);
				tmp->send_position = 0;
				tmp->send_length = 0;
			}

			// Dequeue next message if possible.
			tmp->current_send_request = request_queue_dequeue(tmp->send_request_queue);

			if (tmp->current_send_request != NULL) {
				// We have another send request, so prepare the data message!
				create_data_message_to_send(tmp);
			} else {
				// No more send requests, so remove this connection from the list of active connections.
				tmp->send_message = NULL;
				tmp->send_length = 0;
				remove_active_sender(tmp);
			}
		}
	}

	// We always clear the active fragment info, even if there is another fragment or message to send. We do this to ensure that
	// select_message_fragment() will check the sliding window for this virtual connection.
	send_fragment_connection_index = -1;
	send_fragment_pos = 0;
}

static int write_active_fragment() {

	int written;

	while (true) {

		if (send_fragment_pos == send_fragment->length) {
			// Fragment is written completely, so return 'done'.
			INFO(1, "MESSAGE SEND COMPLETE for magic=%d:%d flag=%d opcode=%d src=%d:%d dst=%d:%d seq=%d ack=%d length=%d",
									GET_MAGIC0(send_fragment->flags),
									GET_MAGIC1(send_fragment->flags),
									GET_FLAGS(send_fragment->flags),
									GET_OPCODE(send_fragment->flags),
									GET_CLUSTER_RANK(send_fragment->src_pid),
									GET_PROCESS_RANK(send_fragment->src_pid),
									GET_CLUSTER_RANK(send_fragment->dst_pid),
									GET_PROCESS_RANK(send_fragment->dst_pid),
									send_fragment->transmit_seq,
									send_fragment->ack_seq,
									send_fragment->length);

			return 0;
		}

		// Write as much of the fragment as we can.
		written = socket_send(gatewayfd, ((unsigned char *) send_fragment) + send_fragment_pos,
				send_fragment->length - send_fragment_pos, false);

		if (written > 0) {
			// Successfull, so update position.
			send_fragment_pos += written;

//			WARN(1, "WROTE %d bytes, position now %ld length %d", written, send_fragment_pos, send_fragment->length);

		} else if (written == 0) {
			// No data written, so return 'wouldblock'
//			WARN(1, "WROTE 0 bytes, return at position %ld length %d", written, send_fragment_pos, send_fragment->length);

			return 1;
		} else { // if (written < 0) {
			// Write failed so return 'error'
			ERROR(1, "Failed to write message fragment");
			return -1;
		}
	}
}

static int poll_send() {

	int error;

	while (true) {

		if (send_fragment_connection_index != -1) {

			// There is an active fragment, so write it.
			error = write_active_fragment();

			if (error == 1) {
				// Could not write any more data, so return 'wouldblock'
				return 1;

			} else if (error == 0) {
				// We've finished writing, so finish the active fragment and update the virtual connection it belongs to.
				finish_active_message_fragment();

			} else if (error == -1) {
				// Failed to write to network. Error already printed.
				return -1;
			}

		} else {

			error = select_fragment_to_send();

			if (error == 1) {
				// No more fragments available.
				return 0;
			} else if (error == -1) {
				FATAL("INTERNAL ERROR: Failed to select message fragment!");
			}
		}
	}
}

static int messaging_send_server(server_message *sm) {
	int error;

	// Any communication with the server is blocking, so we can simply insert the message into the correct virtual connection,
	// and the wait until it is send.

	connections[total_size]->send_message = (unsigned char *) sm;
	connections[total_size]->send_length = sm->length;
	connections[total_size]->send_position = 0;
	connections[total_size]->send_opcode = OPCODE_SERVER;

	add_active_sender(connections[total_size]);

	// Keep polling the network until the data is send!
	while (connections[total_size]->send_message != NULL) {

		error = messaging_poll();

		if (error != 0) {
			ERROR(1, "Failed to send message to server!");
			return EMPI_ERR_INTERN;
		}
	}

	return EMPI_SUCCESS;
}

static int messaging_send_nonblocking(void* buf, int count, datatype *t, int dest, int tag, communicator* c, request *req) {

	// This is a non-blocking send, which we implement by queuing the message and poking the queue to send some data.
	int error, pid, cluster, process, index;

	// First find the index of the virtual connection we are interested in.
	pid = get_pid(req->c, req->source_or_dest);

	cluster = GET_CLUSTER_RANK(pid);
	process = GET_PROCESS_RANK(pid);
	index = cluster_offsets[cluster] + process;

	//WARN(1, "messaging_send_nonblocking to %d:%d = index %d", cluster, process, index);

	// Next, check if the virtual connection already exists. If not we create one on the fly.
	if (connections[index] == NULL) {
		// No active connection to cluster:process yet, so create it. FIXME: change to INFO once debugged.
		//WARN(0, "Creating virtual connection to %d:%d", cluster, process);

		connections[index] = virtual_connection_create(index, pid,
				MAX_PENDING_BYTES / fragment_size, SEND_REQUEST_QUEUE_CACHE_SIZE);

		if (connections[index] == NULL) {
			ERROR(1, "Failed to create virtual connection to destination %d:%d", cluster, process);
			return EMPI_ERR_INTERN;
		}
	}

	// Next, we check if the virtual connection has an active send request.
	if (connections[index]->current_send_request == NULL) {
		//WARN(1, "VC %d has no active send", index);

		// No active request, so our request is active now!
		connections[index]->current_send_request = req;
		create_data_message_to_send(connections[index]);

		add_active_sender(connections[index]);
	} else {
		//WARN(1, "VC %d has active send -- will queue message!", index);

		// There is a request active already, so queue this request for later.
		error = virtual_connection_enqueue_send(connections[index], req);

		if (error != 0) {
			ERROR(1, "Failed to send enqueue request virtual connection to destination %d:%d", cluster, process);
			return EMPI_ERR_INTERN;
		}
	}

	// Next, poke the network and hope we send some data.
	error = messaging_poll();

	if (error != 0) {
		ERROR(1, "Failed to process pending send operation!");
		return MPI_ERR_INTERN;
	}

	return EMPI_SUCCESS;
}

static int messaging_send_blocking(void* buf, int count, datatype *t, int dest, int tag, communicator* c) {
	// This is a blocking send, which we implement using a non-blocking send followed by a wait.
	int error;
	request *req;

	// First create a request for this send.
	req = create_request(REQUEST_FLAG_SEND, buf, count, t, dest, tag, c);

	// Then send or queue it.
	error = messaging_send_nonblocking(buf, count, t, dest, tag, c, req);

	if (error != EMPI_SUCCESS) {
		free_request(req);
		return error;
	}

	// Keep polling the network until our send has completed.
	while (!request_completed(req)) {
		error = messaging_poll();

		if (error != 0) {
			ERROR(1, "Failed to process pending send operation!");
			return MPI_ERR_INTERN;
		}
	}

	free_request(req);

	return EMPI_SUCCESS;
}

// Send a wide area DATA message.
int messaging_send(void* buf, int count, datatype *t, int dest, int tag,
		communicator* c, request *req, bool needs_ack) {
	//WARN(1, "Forwarding message to gateway");

	if (needs_ack) {
		WARN(1, "Message ACK not implemented yet (needed for MPI_Ssend and friends)");
	}

	if (req == NULL) {
		return messaging_send_blocking(buf, count, t, dest, tag, c);
	} else {
		return messaging_send_nonblocking(buf, count, t, dest, tag, c, req);
	}
}

// Blocking receive for a wide area message.
int messaging_receive(void *buf, int count, datatype *t, int source, int tag,
		EMPI_Status *status, communicator* c) {
	int error;
	data_message *m;
	request *req;

	// First we check the receive queue of the target communicator. This queue contains messages for which no request was
	// pending when they where received.
	m = find_pending_message(c, source, tag);

	if (m != NULL) {
		// Found a (still packed) message in the queue, so unpack it.
		error = unpack_message(buf, count, t, c, m, status);

		if (error != EMPI_SUCCESS) {
			ERROR(1, "Failed to unpack data message (error=%d)", error);
		}

		return error;
	}

	// No message available yet, so create a request and queue it.
	req = create_request(REQUEST_FLAG_RECEIVE, buf, count, t, source, tag, c);

	messaging_post_receive(req);

	// Now start polling the network until our request is completed.
	while (!request_completed(req)) {
		error = messaging_poll();

		if (error == -1) {
			ERROR(1, "Failed to probe for WA message");
			return EMPI_ERR_INTERN;
		}
	}

	// Request completed, so free it request.
	free_request(req);

	return EMPI_SUCCESS;
}

// Broadcast a message to all cluster coordinators in the communicator.
int messaging_bcast(void* buf, int count, datatype *t, int root,
		communicator* c) {
	int i, error;

	for (i = 0; i < cluster_count; i++) {
		if (i != cluster_rank) {
			error = messaging_send(buf, count, t, c->coordinators[i], BCAST_TAG,
					c, NULL, false);
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
int messaging_bcast_receive(void *buf, int count, datatype *t, int root,
		communicator* c) {
	return messaging_receive(buf, count, t, root, BCAST_TAG, EMPI_STATUS_IGNORE,
			c);
}

/*
 * Test if a message has been received and queued that can fulfill the given request.
 * Only returns if such a message is found. but does not dequeue or unpack the message.
 */
int messaging_peek_receive_queue(request *r) {

	if ((r->flags & REQUEST_FLAG_COMPLETED)) {
		return -1;
	}

	if (have_matching_message(r->c, r->source_or_dest, r->tag)) {
		return 0;
	}

	return 1;
}

/* Test if a message has been received and queued that can fulfill the given request.
 * If so, unpack the message to the application buffer.
 */
int messaging_poll_receive_queue(request *r) {
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

		r->error = TRANSLATE_ERROR(PMPI_Unpack(&(m->payload[0]), m->length-DATA_MESSAGE_SIZE, &position, r->buf, r->count,
				r->type->type, r->c->comm));

		r->flags |= REQUEST_FLAG_COMPLETED;
		free_data_message(m);
		return 0;
	}

	return 1;
}

/* Test if a message has been received and queued that can fulfill the given request. If not, queue the request. */
void messaging_post_receive(request *r) {
	int result;

	result = messaging_poll_receive_queue(r);

	if (result == 0) {
		// A message was found!
		return;
	}

	// No message was found, so enqueue the message.
	if (!request_queue_enqueue(receive_request_queue, r)) {
		FATAL("Failed to post receive request!");
	}
}

int messaging_poll() {

	int error;

	error = poll_send();

	if (error == -1) {
		ERROR(1, "Failed to send WA message");
		return -1;
	}

	error = poll_receive();

	if (error == -1) {
		ERROR(1, "Failed to receive WA message");
		return -1;
	}

	return 0;
}

/*****************************************************************************/
/*               Server Communication on Application Process                 */
/*****************************************************************************/

static int wait_for_server_reply(int opcode) {

	int error;
	server_message *m;

	// Wait until a message arrives.
	while (connections[total_size]->receive_message == NULL) {
		// No message received yet...
		error = messaging_poll();

		if (error != 0) {
			ERROR(1, "Failed to send message to server!");
			return EMPI_ERR_INTERN;
		}
	}

	// Wait until the message is complete
	while (connections[total_size]->receive_position < connections[total_size]->receive_length) {
		error = messaging_poll();

		if (error != 0) {
			ERROR(1, "Failed to send message to server!");
			return EMPI_ERR_INTERN;
		}
	}

	if (connections[total_size]->receive_opcode != OPCODE_SERVER) {
		ERROR(1, "Did not receive server reply!");
		return EMPI_ERR_INTERN;
	}

	m = (server_message *) connections[total_size]->receive_message;

	if (m->opcode != opcode) {
		ERROR(1, "Received server reply with wrong opcode!");
		return EMPI_ERR_INTERN;
	}

	return EMPI_SUCCESS;
}

static int *copy_int_array(int *src, int offset, int length) {

	int *tmp = malloc(length * sizeof(int));

	if (tmp == NULL) {
		ERROR(1, "Failed to allocate int[%ld]!", length * sizeof(int));
		return NULL;
	}

	memcpy(tmp, src + offset, length * sizeof(int));

	return tmp;
}

int messaging_comm_split_send(communicator* c, int color, int key) {
	int error;
	split_request_msg *req;

	req = malloc(SPLIT_REQUEST_MSG_SIZE);

	if (req == NULL) {
		ERROR(1, "Failed to allocate space for SPLIT REQUEST message!");
		return EMPI_ERR_INTERN;
	}

	req->header.opcode = OPCODE_SPLIT;

	req->header.length = SPLIT_REQUEST_MSG_SIZE;
	req->comm = c->handle;
	req->src = c->global_rank;
	req->color = color;
	req->key = key;

	error = messaging_send_server((server_message *) req);

	free(req);

	return error;
}

int messaging_comm_split_receive(comm_reply *reply) {
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
	int i;

	int error;
	split_reply_msg *msg;

	error = wait_for_server_reply(OPCODE_SPLIT_REPLY);

	if (error != EMPI_SUCCESS) {
		return error;
	}

	msg = (split_reply_msg *) connections[total_size]->receive_message;

//	fprintf(stderr, "SPLIT MESG %d\n", msg->header.length);

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

//	WARN(1, "*Received comm reply (newComm=%d rank=%d size=%d color=%d key=%d cluster_count=%d flag=%d)",
//			reply->newComm, reply->rank, reply->size, reply->color, reply->key, reply->cluster_count, reply->flags);

	reply->coordinators = copy_int_array((int *) &(msg->payload[0]), 0, reply->cluster_count);

	if (reply->coordinators == NULL) {
		ERROR(1, "Failed to allocate or receive coordinators");
		return EMPI_ERR_INTERN;
	}

//	fprintf(stderr, "Comm(%d) coordinators [", reply->newComm);
//
//	for (i=0;i<reply->cluster_count;i++) {
//		fprintf(stderr, " %d ", reply->coordinators[i]);
//	}
//
//	fprintf(stderr, "]\n");

	reply->cluster_sizes = copy_int_array((int *) &(msg->payload[0]), reply->cluster_count, reply->cluster_count);

	if (reply->cluster_sizes == NULL) {
		ERROR(1, "Failed to allocate or receive cluster sizes");
		return EMPI_ERR_INTERN;
	}

//	fprintf(stderr, "Comm(%d) cluster sizes [", reply->newComm);
//
//	for (i=0;i<reply->cluster_count;i++) {
//		fprintf(stderr, " %d ", reply->cluster_sizes[i]);
//	}
//
//	fprintf(stderr, "]\n");

	reply->cluster_ranks = copy_int_array((int *) &(msg->payload[0]),
			2 * reply->cluster_count, reply->cluster_count);

	if (reply->cluster_ranks == NULL) {
		ERROR(1, "Failed to allocate or receive cluster ranks");
		return EMPI_ERR_INTERN;
	}

//	fprintf(stderr, "Comm(%d) cluster ranks [", reply->newComm);
//
//	for (i=0;i<reply->cluster_count;i++) {
//		fprintf(stderr, " %d ", reply->cluster_ranks[i]);
//	}
//
//	fprintf(stderr, "]\n");

	if (reply->size > 0) {

		reply->members = (uint32_t *) copy_int_array((int *) &(msg->payload[0]), 3 * reply->cluster_count, reply->size);

		if (reply->members == NULL) {
			ERROR(1, "Failed to allocate or receive communicator members");
			return EMPI_ERR_INTERN;
		}

//		fprintf(stderr, "Comm(%d) members [", reply->newComm);
//
//		for (i=0;i<reply->size;i++) {
//			fprintf(stderr, " %d ", reply->members[i]);
//		}
//
//		fprintf(stderr, "]\n");

		reply->member_cluster_index = (uint32_t *) copy_int_array((int *) &(msg->payload[0]),
				3 * reply->cluster_count + reply->size, reply->size);

		if (reply->member_cluster_index == NULL) {
			ERROR(1, "Failed to allocate or receive communicator member cluster index");
			return EMPI_ERR_INTERN;
		}

//		fprintf(stderr, "Comm(%d) member_cluster_index [", reply->newComm);
//
//		for (i=0;i<reply->size;i++) {
//			fprintf(stderr, " %d ", reply->member_cluster_index[i]);
//		}
//
//		fprintf(stderr, "]\n");


		reply->local_ranks = (uint32_t *) copy_int_array((int *) &(msg->payload[0]),
				3 * reply->cluster_count + 2 * reply->size, reply->size);

		if (reply->local_ranks == NULL) {
			ERROR(1, "Failed to allocate or receive communicator member local ranks");
			return EMPI_ERR_INTERN;
		}

//		fprintf(stderr, "Comm(%d) local_ranks [", reply->newComm);
//
//		for (i=0;i<reply->size;i++) {
//			fprintf(stderr, " %d ", reply->local_ranks[i]);
//		}
//
//		fprintf(stderr, "]\n");

	} else {
		reply->members = NULL;
		reply->member_cluster_index = NULL;
		reply->local_ranks = NULL;
	}

	connections[total_size]->receive_message = NULL;
	connections[total_size]->receive_opcode = 0;
	connections[total_size]->receive_length = 0;
	connections[total_size]->receive_position = 0;
	return EMPI_SUCCESS;
}

int messaging_comm_create_send(communicator* c, group *g) {
	int i, msgsize, error;
	group_request_msg *req;

	msgsize = GROUP_REQUEST_MSG_SIZE + (g->size * sizeof(int));

	DEBUG(1, "Sending group request of size %d", msgsize);

	req = (group_request_msg *) malloc(GROUP_REQUEST_MSG_SIZE + (g->size * sizeof(int)));

	if (req == NULL) {
		ERROR(1, "Failed to allocate space COMM_CREATE request!");
		return EMPI_ERR_INTERN;
	}

	req->header.opcode = OPCODE_GROUP;
	req->header.length = msgsize;

	req->comm = c->handle;
	req->src = c->global_rank;
	req->size = g->size;

	for (i = 0; i < g->size; i++) {
		req->members[i] = g->members[i];
	}

//	WARN(1, "Group request header %d %d %d %d %d", req->header.opcode, req->header.length, req->comm, req->src, req->size);

	error = messaging_send_server((server_message *) req);

	free(req);

	return error;
}

int messaging_comm_create_receive(group_reply *reply) {
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

	error = wait_for_server_reply(OPCODE_GROUP_REPLY);

	if (error != EMPI_SUCCESS) {
		return error;
	}

	msg = (group_reply_msg *) connections[total_size]->receive_message;

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

		reply->coordinators = copy_int_array((int *) &(msg->payload[0]), 0,
				reply->cluster_count);

		if (reply->coordinators == NULL) {
			ERROR(1, "Failed to allocate or receive coordinators");
			return EMPI_ERR_INTERN;
		}

		reply->cluster_sizes = copy_int_array((int *) &(msg->payload),
				reply->cluster_count, reply->cluster_count);

		if (reply->cluster_sizes == NULL) {
			ERROR(1, "Failed to allocate or receive cluster sizes");
			return EMPI_ERR_INTERN;
		}

		reply->cluster_ranks = copy_int_array((int *) &(msg->payload[0]),
				2 * reply->cluster_count, reply->cluster_count);

		if (reply->cluster_ranks == NULL) {
			ERROR(1, "Failed to allocate or receive cluster ranks");
			return EMPI_ERR_INTERN;
		}

		if (reply->size > 0) {

			reply->members = (uint32_t *) copy_int_array(
					(int *) &(msg->payload[0]), 3 * reply->cluster_count,
					reply->size);

			if (reply->members == NULL) {
				ERROR(1, "Failed to allocate or receive communicator members");
				return EMPI_ERR_INTERN;
			}

			reply->member_cluster_index = (uint32_t *) copy_int_array(
					(int *) &(msg->payload[0]),
					3 * reply->cluster_count + reply->size, reply->size);

			if (reply->member_cluster_index == NULL) {
				ERROR(1,
						"Failed to allocate or receive communicator member cluster index");
				return EMPI_ERR_INTERN;
			}

			reply->local_ranks = (uint32_t *) copy_int_array(
					(int *) &(msg->payload[0]),
					3 * reply->cluster_count + 2 * reply->size, reply->size);

			if (reply->local_ranks == NULL) {
				ERROR(1,
						"Failed to allocate or receive communicator member local ranks");
				return EMPI_ERR_INTERN;
			}
		} else {
			reply->members = NULL;
			reply->member_cluster_index = NULL;
			reply->local_ranks = NULL;
		}
	} else {
		reply->coordinators = NULL;
		reply->cluster_sizes = NULL;
		reply->cluster_ranks = NULL;
		reply->members = NULL;
		reply->member_cluster_index = NULL;
		reply->local_ranks = NULL;
	}

	connections[total_size]->receive_message = NULL;
	connections[total_size]->receive_opcode = 0;
	connections[total_size]->receive_length = 0;
	connections[total_size]->receive_position = 0;

	return EMPI_SUCCESS;
}

int messaging_comm_dup_send(communicator* c) {
	int error;
	dup_request_msg *req;

	req = malloc(DUP_REQUEST_MSG_SIZE);

	if (req == NULL) {
		ERROR(1, "Failed to allocate space for DUP REQUEST!");
		return EMPI_ERR_INTERN;
	}

	req->header.opcode = OPCODE_DUP;
	req->header.length = DUP_REQUEST_MSG_SIZE;
	req->comm = c->handle;
	req->src = c->global_rank;

	error = messaging_send_server((server_message *) req);

	free(req);

	return error;
}

int messaging_comm_dup_receive(dup_reply *reply) {
	// Since operations on communicators are collective operations, we can
	// assume here that the reply has not be received yet.
	int error;
	dup_reply_msg *msg;

	error = wait_for_server_reply(OPCODE_DUP_REPLY);

	if (error != EMPI_SUCCESS) {
		return error;
	}

	msg = (dup_reply_msg *) connections[total_size]->receive_message;

	if (msg == NULL) {
		ERROR(1, "Failed to get direct read access to buffer!");
		return MPI_ERR_INTERN;
	}

	reply->newComm = msg->newComm;

	DEBUG(1, "*Received dup reply (newComm=%d)", reply->newComm);

	connections[total_size]->receive_message = NULL;
	connections[total_size]->receive_opcode = 0;
	connections[total_size]->receive_length = 0;
	connections[total_size]->receive_position = 0;

	return EMPI_SUCCESS;
}

int messaging_comm_free_send(communicator* c) {
	int error;
	free_request_msg *req;

	req = malloc(FREE_REQUEST_MSG_SIZE);

	if (req == NULL) {
		ERROR(1, "Failed to allocate space for FREE REQUEST!");
		return EMPI_ERR_INTERN;
	}

	req->header.opcode = OPCODE_FREE;
	req->header.length = FREE_REQUEST_MSG_SIZE;

	req->comm = c->handle;
	req->src = c->global_rank;

	error = messaging_send_server((server_message *) req);

	free(req);

	return error;
}

int messaging_finalize() {
	// A finalize is an collective operation. It must be performed by all application processes.

	int error;
	finalize_request_msg *req;
	communicator *c;
	unsigned char buffer;

	c = handle_to_communicator(0);

	INFO(1, "Sending finalize request %d:%d %ld %ld ", c->handle, c->global_rank, sizeof(finalize_request_msg), FINALIZE_REQUEST_MSG_SIZE);

	req = (finalize_request_msg *) malloc(FINALIZE_REQUEST_MSG_SIZE);

	if (req == NULL) {
		ERROR(1, "Failed to allocate space for FINALIZE REQUEST");
		return EMPI_ERR_INTERN;
	}

	req->header.opcode = OPCODE_FINALIZE;
	req->header.length = FINALIZE_REQUEST_MSG_SIZE;

	req->comm = c->handle;
	req->src = c->global_rank;

	error = messaging_send_server((server_message *) req);

	free(req);

	if (error != EMPI_SUCCESS) {
		ERROR(1, "Failed to send FINALIZE REQUEST");
		return EMPI_ERR_INTERN;
	}

	INFO(1, "Finalize request %d:%d send", c->handle, c->global_rank);DEBUG(1, "Receiving finalize reply %d:%d send",
			c->handle, c->global_rank);

	// Wait for the gateway to disconnect.
	error = socket_receive(gatewayfd, &buffer, 1, true);

	// error = wait_for_server_reply(OPCODE_FINALIZE_REPLY);

	if (error != SOCKET_DISCONNECT) {
		WARN(1, "Failed to cleanly disconnect from gateway error=%d", error);
	}

	//connections[total_size]->receive_message = NULL;
	//connections[total_size]->receive_opcode = 0;
	//connections[total_size]->receive_length = 0;
	//connections[total_size]->receive_position = 0;

	INFO(1, "Gateway disconnected as expected");

	// Do we need this barrier ?
	// error = PMPI_Barrier(MPI_COMM_WORLD);
	error = PMPI_Finalize();

	if (error != MPI_SUCCESS) {
		ERROR(1, "INTERNAL ERROR: failed to perform FINALIZE of local MPI! (error=%d)", error);
		return TRANSLATE_ERROR(error);
	}

	return EMPI_SUCCESS;
}

