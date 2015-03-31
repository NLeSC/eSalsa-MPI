#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <errno.h>
#include <limits.h>
#include <fcntl.h>

#include "settings.h"
#include "logging.h"
#include "opcodes.h"
#include "generic_message.h"
//#include "linked_queue.h"
#include "socket_util.h"
#include "udt_util.h"
#include "fragments_buffer.h"


// We should now allocate a fixed amount of message fragments. We can either do this per processes (ie. 64 per process) or per
// data volume (ie. 16 GB in total), or use the minimum useful count of the two.
//
// Examples:
//
// Taking into account that the biggest message used in CESM is about 768 KB, and the avarage size is as low as 16~76 KB
// (depending on the node), a 1 MB buffer per connection should be enough for everyone ;-)
//
// With 16 KB fragments and 1 MB buffer per virtual connection (VC) we would get 64 packets per VC. Assuming each node has
// 8 VCs (used for the halo exchange) we would need 512 packets per node (8 MB). With 2K nodes there could be at most 1M packets
// in transit, using a total of 16GB.


//#define MAX_MESSAGE_SIZE (MAX_MESSAGE_PAYLOAD + (MESSAGE_HEADER_SIZE + 5*sizeof(int)))

#define EPOLL_TIMEOUT 500

#define MAX_LENGTH_CLUSTER_NAME 128

// Maximum consecutive write operations to perform on a socket before performing another epoll.
#define WRITE_OPS_LIMIT 1

// Maximum consecutive read operations to perform on a socket before performing another epoll.
#define READ_OPS_LIMIT 1

//#define MAX_STREAMS 16

//#define RECEIVE_BUFFER_SIZE (32*1024*1024)
//#define SEND_BUFFER_SIZE (32*1024*1024)

#define TYPE_SERVER           (0)
#define TYPE_ACCEPT           (1)
#define TYPE_GATEWAY_TCP      (2)
#define TYPE_GATEWAY_UDT      (3)
#define TYPE_COMPUTE_NODE_TCP (4)

//#define WIDE_AREA_PROTOCOL (TYPE_GATEWAY_TCP)

#define WIDE_AREA_PROTOCOL_TCP   0
#define WIDE_AREA_PROTOCOL_UDT   1

#define STATE_ACCEPTING          0
#define STATE_READING_HANDSHAKE  1
#define STATE_WRITING_HANDSHAKE  2
#define STATE_IDLE               3
#define STATE_READY              4
#define STATE_READ_HEADER        5
#define STATE_READ_PAYLOAD       6
#define STATE_TERMINATING        7
#define STATE_TERMINATED         8

// Error codes used internally.
#define CONNECT_OK              0
#define ERROR_HANDSHAKE_FAILED 12
#define ERROR_ALLOCATE         13
#define ERROR_CANNOT_FIND_IP   14
#define ERROR_GATEWAY          15
#define ERROR_ARGUMENT         16
#define ERROR_MUTEX            17
#define ERROR_SERVER           18
#define ERROR_CONFIG           19
#define ERROR_POLL   	       20
#define ERROR_ACCEPT           21
#define ERROR_CONNECTION       22


#ifdef DETAILED_TIMING
typedef struct {
	uint64_t starttime;
	uint64_t endtime;
	uint64_t size;
} timing;

timing write_timings[DETAILED_TIMING_COUNT];
int write_timing_count;

timing read_timings[DETAILED_TIMING_COUNT];
int read_timing_count;
#endif // DETAILED_TIMING


// A gateway request message which is used to inform the server
// of the contact information of a gateway.
typedef struct {
	int opcode;  // type of message
	int cluster; // rank of cluster
	int src;     // rank in cluster
	int count;   // ip adres count
} gateway_request_msg;

#define GATEWAY_REQUEST_SIZE (4*sizeof(int))

// A type to store all information related to a single socket connection.
// This includes the send and receive threads, plus output queue.
typedef struct {

	int socketfd;
	int type;
	int state;
	int index;

	bool done;

	// Receiver thread (only used in gateway-to-gateway connections).
	pthread_t receive_thread;

	// Sender thread (only used in gateway-to-gateway connections).
	pthread_t send_thread;


	/******* Current outgoing message *******/

	// Current message that is being written.
	unsigned char *out_msg;

	// Index of message that is being written.
	int out_msg_index;

	// Write position in message that is being written.
	size_t out_msg_pos;

	// Length of the message that is being written.
	size_t out_msg_length;

	// Total number of messages written.
	uint64_t out_msg_count;

	// Total number of bytes written.
	uint64_t out_msg_bytes_total;


	/******* Current incoming message *******/

	// Current message that is read.
	unsigned char *in_msg;

	// Index of message that is being read.
	int in_msg_index;

	// Read position in message that is being read.
	size_t in_msg_pos;

	// Length of the message that is being read.
	size_t in_msg_length;

	// Total number of messages read.
	uint64_t in_msg_count;

	// Total number of bytes read.
	uint64_t in_msg_bytes_total;

	// The number of bytes received on this socket_info that have not been forwarded yet (poor mans flow control).
	// size_t pending_ack_bytes;

	// A mutex to make the pending bytes thread safe.
	// pthread_mutex_t bytes_mutex;

	// A FIFO queue for the messages that need to written to the socket.
	int *output_queue;

	// Maximum number of elements in output queue.
	int max_output_queue_size;

	// Number of elements in output queue.
	int output_queue_size;

	// Head of output queue.
	int output_queue_head;

	// Tails of output queue.
	int output_queue_tail;

	// Queue for the messages that need to written to the socket.
	// linked_queue *output_queue;

	// A mutex to make the output queue thread safe.
	pthread_mutex_t output_mutex;

	// A condition variable that allows a dequeue on the output queue to block.
	pthread_cond_t output_cond;

	// Queue for the messages that have been received from the socket (only in gateway-to-gateway TCP connections).
	// linked_queue *input_queue;

} socket_info;

// A type to store gateway information.
typedef struct {
	unsigned long  ipv4;
	unsigned short port;
	unsigned short protocol;
	unsigned short streams;
} gateway_address;

typedef struct {
	gateway_address info;
	socket_info **sockets;
	int stream_count;
	int next_output_stream;
	int next_input_stream;
} gateway_connection;

// The name of this cluster (must be unique).
static char *cluster_name;
static gateway_address *gateway_addresses;
static gateway_connection *gateway_connections;

// The number of clusters and the rank of our cluster in this set.
uint32_t cluster_count;
uint32_t cluster_rank;

// The size of each cluster, and the offset of each cluster in the total set of machines.
int *cluster_sizes;
int *cluster_offsets;

// The number of application processes and gateways in this cluster.
uint32_t local_application_size;
uint32_t gateway_count;

uint32_t local_connections;
uint32_t pending_local_connections;

// The gateway rank of this gateway process. -1 if process is application process.
int gateway_rank;

// TODO: use multiple of these ?
// The file descriptor used to epoll the gateway connections.
static int epollfd;

// The filedescriptor of the socket connected to the 'server'.
static int serverfd = 0;

// The location of the server to which we connect.
static char *server_name;
static long server_ipv4;
static unsigned short server_port;

// The local port in our process to which the local compute nodes connect.
static unsigned short local_port;

// The PID of the server process. Used in communicator and group operations.
uint32_t server_pid;

// The PID of this gateway process. Used when the server has a message for us.
uint32_t my_pid;

// Socket info containing information on the address of the server.
static socket_info *server_info;

// Timing offset.
uint64_t gateway_start_time;
uint64_t application_start_time;

static uint64_t prev_deltat = 0;

static uint64_t prev_send_data = 0;
static uint64_t prev_send_count = 0;

static uint64_t prev_received_data = 0;
static uint64_t prev_received_count = 0;

static uint64_t wa_send_data = 0;
static uint64_t wa_send_count = 0;

static uint64_t wa_received_data = 0;
static uint64_t wa_received_count = 0;

static pthread_mutex_t send_data_mutex;
static pthread_mutex_t received_data_mutex;

// These represent the socket connections to the local compute nodes.
static socket_info **local_compute_nodes;

// Size of the message fragments (provided by the server).
static int fragment_size;

// The number of fragments in each fragment buffer (provided by the server).
static int fragment_count;

// We need -two- fragment buffers here, each containing "fragment_count" messages. One fragment buffer is used to receive messages
// from the local compute nodes that belong to this gateway, and also serves at the 'send buffer' when forwarding these messages
// to other gateways. The other fragment buffer is used to receive messages from remote gateways, and also serves as a
// 'send buffer' when forwarding these messages to the local compute nodes.
//
// The reason we need two is that using only one may result in deadlocks. For example: take two gateways each containing only one
// fragment buffer. It may happen that all messages in the fragment buffer of one of these gateways are filled by local messages.
// This gateway can then no longer receive any messages from another gateway, since there is no room left to receive these
// messages. If this happens on both gateways at the same time, neither can receive, so neither can send, so we have a deadlock.
// By using separate fragment buffers for incoming local and remote communication this can never occur.
//
// Note that there is no flow control (on this layer) on the gateway to gateway links. As a result, more messages can be send to
// a gateway than it can buffer (even when using a separate fragment buffer for receiving). When this happens, the gateway will
// simply stop receiving messages until there is buffer space available (and the flow control of the underlying TCP or UDT
// link will kick in). This will -NOT- cause a deadlock, since there is end-to-end flow control on the compute-node to
// compute-node streams. Therefore, all messages send by a compute node are guaranteed to have buffer space available on the
// receiver. As a result, a gateway is guaranteed to (eventually) be able to forward all data it receives to one of its
// compute-nodes and thus have more buffer space available.

// Fragment buffer containing the fragments used for receiving from local processes and sending to remote gateways.
static fragment_buffer *nodes_to_gateway_fragment_buffer;

// Fragment buffer containing the fragments used for receiving from a remote gateways and sending to local processes.
static fragment_buffer *gateway_to_nodes_fragment_buffer;

static uint64_t current_time_micros()
{
	uint64_t result;
	struct timeval t;

	gettimeofday(&t,NULL);

	result = (t.tv_sec * 1000000LU) + t.tv_usec;

	return result;
}

void store_sender_thread_stats(int index, uint64_t data, uint64_t count, uint64_t time)
{
	// Lock the send data first
	pthread_mutex_lock(&send_data_mutex);

	wa_send_data += data;
	wa_send_count += count;

	pthread_mutex_unlock(&send_data_mutex);
}

void retrieve_sender_thread_stats(uint64_t *data, uint64_t *count)
{
	// Lock the send data first
	pthread_mutex_lock(&send_data_mutex);

	*data = wa_send_data;
	*count = wa_send_count;

	pthread_mutex_unlock(&send_data_mutex);
}

void store_receiver_thread_stats(int index, uint64_t data, uint64_t count, uint64_t time)
{
	// Lock the send data first
	pthread_mutex_lock(&received_data_mutex);

	wa_received_data += data;
	wa_received_count += count;

	pthread_mutex_unlock(&received_data_mutex);

}

void retrieve_receiver_thread_stats(uint64_t *data, uint64_t *count)
{
	// Lock the send data first
	pthread_mutex_lock(&send_data_mutex);

	*data = wa_received_data;
	*count = wa_received_count;

	pthread_mutex_unlock(&send_data_mutex);
}

static void clear_socket_info_in_msg(socket_info *info)
{
	info->in_msg = NULL;
	info->in_msg_pos = 0;
	info->in_msg_length = 0;
	info->in_msg_index = -1;
	info->state = STATE_READY;
}

static void clear_socket_info_out_msg(socket_info *info)
{
	info->out_msg = NULL;
	info->out_msg_pos = 0;
	info->out_msg_length = 0;
	info->out_msg_index = -1;
}

static void set_done_at_socket_info(socket_info *info)
{
	// Lock the queue first.
	pthread_mutex_lock(&(info->output_mutex));

	info->done = true;

	// Tell any listners that something has happened.
	pthread_cond_broadcast(&(info->output_cond));

	// Unlock the queue.
	pthread_mutex_unlock(&(info->output_mutex));
}

static int enqueue_message_at_socket_info(socket_info *info, int index)
{
	int *tmp;
	int i;
	int error = 0;

//	fprintf(stderr, "##### Enqueue for socket_info %d", info->socketfd);

	// Lock the queue first.
	pthread_mutex_lock(&info->output_mutex);

	if (info->done) {
		FATAL("Cannot enqueue message after done is set!");
	}

//	fprintf(stderr, "###### Storing message in queue of length %d (socket_info %d)", linked_queue_length(info->output_queue), info->socketfd);

	if (info->max_output_queue_size == info->output_queue_size) {
		// The output queue is full, so we need to resize it!
		tmp = info->output_queue;

		INFO(1, "RESIZE OUTPUT QUEUE %d FROM %d TO %d", info->socketfd, info->max_output_queue_size, 2*info->max_output_queue_size);

		info->output_queue = malloc(2 * info->max_output_queue_size * sizeof(int));

		if (info->output_queue == NULL) {
			FATAL("Failed to create output queue for socket info!");
		}

		if (info->output_queue_head == 0) {
			INFO(1, "RESIZE USES LINEAR COPY");
			// The elements are stored -in order- (e.g.: (H) 0, 1, 2, ... N-1 (T)) so we can use memcpy to copy to the new buffer space.
			memcpy(info->output_queue, tmp, info->max_output_queue_size * sizeof(int));
		} else {
			INFO(1, "RESIZE REORDER COPY %d %d", info->output_queue_head, info->output_queue_tail);

			// The elements are stored -out of order- (e.g.: ..., N-2, N-1 (T), (H) 0, 1, 2, ... ) so we can -not- use memcpy!
			for (i=0;i<info->max_output_queue_size;i++) {
				info->output_queue[i] = tmp[(info->output_queue_head + i) % info->max_output_queue_size];
			}

		}

		free(tmp);

		info->output_queue_head = 0;
		info->output_queue_tail = info->max_output_queue_size;
		info->max_output_queue_size *= 2;

		INFO(1, "AFTER RESIZE %d head %d tail %d size %d", info->socketfd, info->output_queue_head, info->output_queue_tail, info->output_queue_size);

	}

	INFO(1, "ENQ %d index %d at position %d size %d", info->socketfd, index, info->output_queue_tail, info->output_queue_size);

	info->output_queue[info->output_queue_tail] = index;
	info->output_queue_tail = (info->output_queue_tail + 1) % info->max_output_queue_size;
	info->output_queue_size += 1;

	// FIXME: ensure this only needs to be done at 1 ?
	if (info->output_queue_size == 1) {

		// We've just inserted a message into an empty queue, so set the socket to the correct mode (if needed),
		// and wake up any waiting threads. We need do do this inside the lock to prevent the message from disappearing before
		// we have updated the message state.
		if (info->type == TYPE_SERVER || info->type == TYPE_COMPUTE_NODE_TCP) {
			error = socket_set_rw(epollfd, info->socketfd, info);
		}

//		fprintf(stderr, "##### Sending bcast to wake up threads! (socket_info %d)", info->socketfd);

		// FIXME: is this alway needed ?
		pthread_cond_broadcast(&info->output_cond);
	}

	// Unlock the queue.
	pthread_mutex_unlock(&info->output_mutex);

//	fprintf(stderr, "##### Enqueue done for socket_info %d", info->socketfd);

	return error;
}

/*
static void add_ack_bytes(message_header *mh)
{
	int rank;
	socket_info *info;
	size_t toAck = 0;
	message_buffer *m;
	ack_message *ack;

	if (mh->src_pid == server_pid) {
		return;
	}

	if (GET_CLUSTER_RANK(mh->src_pid) != cluster_rank) {
		return;
	}

	rank = GET_PROCESS_RANK(mh->src_pid);

	if (rank < 0 || rank >= local_application_size) {
		WARN(1, "Failed to release bytes!");
		return;
	}

	info = local_compute_nodes[rank];

	if (info->type != TYPE_COMPUTE_NODE_TCP) {
		return;
	}

	pthread_mutex_lock(&info->bytes_mutex);

	info->pending_ack_bytes += mh->length;

	if (info->pending_ack_bytes >= MAX_MESSAGE_SIZE/2) {
		toAck = info->pending_ack_bytes;
		info->pending_ack_bytes	= 0;

		DEBUG(1, "Sending ack %ld to compute node %d\n", toAck, rank);
	}

	pthread_mutex_unlock(&info->bytes_mutex);

	if (toAck > 0) {
		// We must send an ACK message to mh->src_pid
		m = message_buffer_create(ACK_MESSAGE_SIZE);
		ack = (ack_message *)message_buffer_direct_write_access(m, ACK_MESSAGE_SIZE);

		ack->header.opcode = OPCODE_ACK;
		ack->header.dst_pid = mh->src_pid;
		ack->header.src_pid = my_pid;
		ack->header.length = ACK_MESSAGE_SIZE;
		ack->bytes = toAck;

		// FIXME: should enqueu at head, not tail!
		enqueue_message_at_socket_info(info, m);
	}
}
*/

static int dequeue_message_from_socket_info(socket_info *info, int64_t timeout_usec)
{
	struct timespec alarm;
	struct timeval now;
	int error;

//	fprintf(stderr, "##### Dequeue message from socket_info %d", info->socketfd);

	if (info->out_msg != NULL) {
		FATAL("INTERNAL ERROR: extra dequeue on socket info %d ?", info->socketfd);
	}

	info->out_msg_index = -1;

	// Lock the queue first.
	pthread_mutex_lock(&(info->output_mutex));

	INFO(2, "Dequeue message from socket info socketfd=%d output_queue_size=%d ", info->socketfd, info->output_queue_size);

//	fprintf(stderr, "##### Dequeue grabbed lock! (socket_info %d)", info->socketfd);

	while (info->output_queue_size == 0) {

		if (info->done) {
			// We are done. No more messages will be queued. Set the socket to read only before returning!.
			if (info->type == TYPE_COMPUTE_NODE_TCP || info->type == TYPE_SERVER) {
				error = socket_set_ro(epollfd, info->socketfd, info);

				if (error != 0) {
					WARN(1, "Failed to set socket %d to read only", info->socketfd);
				}
			}

			pthread_mutex_unlock(&info->output_mutex);
			return 2;
		}

		if (timeout_usec == 0) {
			// We don't wait for an element to appear, but return immediately!
			break;
		}

		if (timeout_usec < 0) {
			// negative timeout, so perform a blocking wait.
			error = pthread_cond_wait(&info->output_cond, &info->output_mutex);

			if (error != 0) {
				WARN(1, "Failed to wait for message on output queue of socket %d", info->socketfd);
			}
		} else {
			// positive timeout, so perform a timed wait.

			//		fprintf(stderr, "##### Dequeue will sleep for %ld usec (socket_info %d)", timeout_usec, info->socketfd);

			gettimeofday(&now, NULL);
			alarm.tv_sec = now.tv_sec + (timeout_usec / 1000000);
			alarm.tv_nsec = (now.tv_usec + (timeout_usec % 1000000)) * 1000;

			if (alarm.tv_nsec >= 1000000000) {
				alarm.tv_sec++;
				alarm.tv_nsec -= 1000000000;
			}

			error = pthread_cond_timedwait(&info->output_cond, &info->output_mutex, &alarm);

			//	fprintf(stderr, "##### Dequeue woke up!(socket_info %d)", info->socketfd);

			if (error == ETIMEDOUT) {
				// If the timeout expired we return.
			//	fprintf(stderr, "##### Dequeue got timeout!(socket_info %d)", info->socketfd);
				break;
			}

			if (error != 0) {
				WARN(1, "Failed to wait for message on output queue of socket %d", info->socketfd);
			}
		}
	}

	if (info->output_queue_size > 0) {
		// A message is available in the queue.

		info->out_msg_index = info->output_queue[info->output_queue_head];

		INFO(1, "DEQ %d from postion %d index %d size %d", info->socketfd, info->output_queue_head, info->out_msg_index, info->output_queue_size);

		info->output_queue_size--;

		if (info->output_queue_size == 0) {
			// Queue is empty now!
			info->output_queue_head = 0;
			info->output_queue_tail = 0;
		} else {
			info->output_queue_head = (info->output_queue_head + 1) % info->max_output_queue_size;
		}

		INFO(3, "Dequeued message %d queue size now %d", info->out_msg_index, info->output_queue_size);

	} else {

		// No messages available, so set the socket to read-only! We need to do this inside the lock of the queue to prevent
		// queue when we don't expect it.

		if (info->type == TYPE_COMPUTE_NODE_TCP || info->type == TYPE_SERVER) {
			error = socket_set_ro(epollfd, info->socketfd, info);

			if (error != 0) {
				WARN(1, "Failed to set socket %d to read only", info->socketfd);
			}
		}
	}

	// Unlock the queue.
	pthread_mutex_unlock(&info->output_mutex);

	if (info->out_msg_index != -1) {

		// We've managed to get a message, so update some fields.
		if (info->type == TYPE_COMPUTE_NODE_TCP) {

			INFO(3, "Dequeued message for compute node - deq message data from g2n buffer");

			info->out_msg = fragment_buffer_get_fragment(gateway_to_nodes_fragment_buffer, info->out_msg_index);
			info->out_msg_pos = 0;
			info->out_msg_length = ((generic_message *) info->out_msg)->length;

		} else { // if (info->type == TYPE_SERVER || info->type == TYPE_GATEWAY_TCP || info->type == TYPE_GATEWAY_UDT)

			INFO(3, "Dequeued message for server or gateway - deq message data from n2g buffer");

			info->out_msg = fragment_buffer_get_fragment(nodes_to_gateway_fragment_buffer, info->out_msg_index);
			info->out_msg_pos = 0;
			info->out_msg_length = ((generic_message *) info->out_msg)->length;

		}

		// Sanity check
		if (info->out_msg_length < GENERIC_MESSAGE_HEADER_SIZE || info->out_msg_length > fragment_size) {
			FATAL("Invalid message was dequeued! length=%d", info->out_msg_length);
		}

		return 0;
	}


	return 1;
}

static bool enqueue_message_from_server(socket_info *info)
{
	// Message from the server are destined for a local compute node or (rarely) for the gateway itself. Since the server link
	// is handled by a single epoll thread (together with all compute node links), only one of these calls is active at a time.
	int rank;

	bool result;
	generic_message *m = (generic_message *) info->in_msg;

	if (m->dst_pid == my_pid) {
		INFO(1, "RECEIVED SERVER REPLY for this gateway length=%d index=%d", m->length, info->in_msg_index);

		if ((m->length - GENERIC_MESSAGE_HEADER_SIZE) != 8 || (((uint32_t *)&m->payload)[0] != OPCODE_FINALIZE_REPLY)) {
			ERROR(1, "RECEIVED UNEXPECTED SERVER REPLY for magic=%d:%d flag=%d opcode=%d src=%d:%d dst=%d:%d length=%d index=%d",
					GET_MAGIC0(m->flags),
					GET_MAGIC1(m->flags),
					GET_FLAGS(m->flags),
					GET_OPCODE(m->flags),
					GET_CLUSTER_RANK(m->src_pid),
					GET_PROCESS_RANK(m->src_pid),
					GET_CLUSTER_RANK(m->dst_pid),
					GET_PROCESS_RANK(m->dst_pid),
					m->length, info->in_msg_index);
		}

		// We've received a finalize command from the server. This can only happen if all processes have called an MPI_Finalize,
		// after which they have notified the server that they want to quit. Once all request are in, the server replies by
		// sending a OPCODE_FINALIZE_REPLY to the gateways. So we are now 100% sure that all compute nodes are idle and waiting.
		// In addition, all other gateway nodes will also receive a OPCODE_FINALIZE_REPLY at some moment in time.
		//
		// Conclusion: it is now safe to close all connections to the compute nodes, but not yet to the other gateways since they
		//             may not have received OPCODE_FINALIZE_REPLY yet.

		return false;
	}

	INFO(1, "RECEIVED SERVER REPLY for magic=%d:%d flag=%d opcode=%d src=%d:%d dst=%d:%d length=%d index=%d",
			GET_MAGIC0(m->flags),
			GET_MAGIC1(m->flags),
			GET_FLAGS(m->flags),
			GET_OPCODE(m->flags),
			GET_CLUSTER_RANK(m->src_pid),
			GET_PROCESS_RANK(m->src_pid),
			GET_CLUSTER_RANK(m->dst_pid),
			GET_PROCESS_RANK(m->dst_pid),
			m->length, info->in_msg_index);

	rank = GET_PROCESS_RANK(m->dst_pid);

/* FIXME: reimplement!

	if (mh.opcode == OPCODE_FINALIZE_REPLY) {
		// This link will be terminating soon!
		local_compute_nodes[rank]->state = STATE_TERMINATING;
	}

*/
	result = enqueue_message_at_socket_info(local_compute_nodes[rank], info->in_msg_index);

	clear_socket_info_in_msg(info);

	return result;
}

static void enqueue_message_from_compute_node(socket_info *info)
{
	// Message from compute nodes are usually headed for the WA link, although they may also be destined for the server.
	// Since all compute node links are handled by a single epoll thread (together with the server link), only one of these calls
	// is active at a time.
	int cluster, index;
	socket_info *dest;

	generic_message *m = (generic_message *) info->in_msg;

	if (m->dst_pid == server_pid) {
		// This message contains a request for the server!
		INFO(1, "ENQ SERVER REQUEST src=%d:%d length=%d",GET_CLUSTER_RANK(m->src_pid), GET_PROCESS_RANK(m->src_pid), m->length);
		dest = server_info;

	} else {
		// This message must be forwarded to another gateway.
		INFO(1, "ENQ DATA MESSAGE TO WA src=%d:%d dst=%d:%d length=%d",
				GET_CLUSTER_RANK(m->src_pid), GET_PROCESS_RANK(m->src_pid),
				GET_CLUSTER_RANK(m->dst_pid), GET_PROCESS_RANK(m->dst_pid),
				m->length);

		INFO(1, "ENQ DATA MESSAGE TO WA at index %d magic=%d:%d flag=%d opcode=%d src=%d:%d dst=%d:%d seq=%d ack=%d length=%d",
						    info->in_msg_index,
							GET_MAGIC0(m->flags),
							GET_MAGIC1(m->flags),
							GET_FLAGS(m->flags),
							GET_OPCODE(m->flags),
							GET_CLUSTER_RANK(m->src_pid),
							GET_PROCESS_RANK(m->src_pid),
							GET_CLUSTER_RANK(m->dst_pid),
							GET_PROCESS_RANK(m->dst_pid),
							m->transmit_seq,
							m->ack_seq,
							m->length);

		cluster = GET_CLUSTER_RANK(m->dst_pid);

		if (cluster < 0 || cluster >= cluster_count) {
			FATAL("Cluster index out of bounds! (cluster=%d)", cluster);
		}

		// NOTE: to prevent message reordering, we always send fragments from the same sender via the same stream!
		index = GET_PROCESS_RANK(m->src_pid) % gateway_connections[cluster].stream_count;
		dest = gateway_connections[cluster].sockets[index];
	}

	enqueue_message_at_socket_info(dest, info->in_msg_index);

	clear_socket_info_in_msg(info);
}

static int enqueue_message_from_wa_link(socket_info *info)
{
	// Message from the WA link are headed to a compute node. (FIXME: they may also be heading for another WA link ?)
	// Since multiple WA receive threads may be used, several of these calls may be active at a time.
	int rank, error;

	generic_message *m = (generic_message *) info->in_msg;

	INFO(1, "ENQ DATA MESSAGE FROM WA at index %d magic=%d:%d flag=%d opcode=%d src=%d:%d dst=%d:%d seq=%d ack=%d length=%d",
						    info->in_msg_index,
							GET_MAGIC0(m->flags),
							GET_MAGIC1(m->flags),
							GET_FLAGS(m->flags),
							GET_OPCODE(m->flags),
							GET_CLUSTER_RANK(m->src_pid),
							GET_PROCESS_RANK(m->src_pid),
							GET_CLUSTER_RANK(m->dst_pid),
							GET_PROCESS_RANK(m->dst_pid),
							m->transmit_seq,
							m->ack_seq,
							m->length);

	rank = GET_PROCESS_RANK(m->dst_pid);

	error = enqueue_message_at_socket_info(local_compute_nodes[rank], info->in_msg_index);

	clear_socket_info_in_msg(info);

	return error;
}

/*
static bool enqueue_message_from_wa_link(message_buffer *m)
{
	// Messages from the WA link are always destined for a local compute node.
	// Depening on the implementation, multiple threads may be used to receive WA messages, so multiple of these calls may be
	// active at the same time.
	int rank;
	message_header mh;

	message_buffer_peek(m, (unsigned char *)&mh, MESSAGE_HEADER_SIZE);

	INFO(1, "ENQ DATA MESSAGE FROM WA opcode=%d src=%d:%d dst=%d:%d length=%d", mh.opcode,
			GET_CLUSTER_RANK(mh.src_pid), GET_PROCESS_RANK(mh.src_pid),
			GET_CLUSTER_RANK(mh.dst_pid), GET_PROCESS_RANK(mh.dst_pid),
			mh.length);

	rank = GET_PROCESS_RANK(mh.dst_pid);

	return enqueue_message_at_socket_info(local_compute_nodes[rank], m);
}
*/

static int write_message(socket_info *info, bool blocking)
{
	// The socket is ready for writing, so write something!
	int status;
	ssize_t written;
	size_t bytes_left;
	int64_t timeout;
	generic_message *m;

	// If there was no message in progress, we try to dequeue one.
	if (info->out_msg == NULL) {

		if (info->type == TYPE_GATEWAY_TCP || info->type == TYPE_GATEWAY_UDT) {
			// Dequeue with 1 sec. wait.
			timeout = 1000*1000;
		} else {
			// Non-blocking dequeue.
			timeout = 0;
		}

		status = dequeue_message_from_socket_info(info, timeout);

		if (status != 0) {
			// No message was dequeued, so return status which is either 1 ("wouldblock") or 2 ("done")
			return status;
		}

		m = (generic_message *)info->out_msg;

		INFO(1, "DEQ DATA MESSAGE at index %d magic=%d:%d flag=%d opcode=%d src=%d:%d dst=%d:%d seq=%d ack=%d length=%d",
								    info->out_msg_index,
									GET_MAGIC0(m->flags),
									GET_MAGIC1(m->flags),
									GET_FLAGS(m->flags),
									GET_OPCODE(m->flags),
									GET_CLUSTER_RANK(m->src_pid),
									GET_PROCESS_RANK(m->src_pid),
									GET_CLUSTER_RANK(m->dst_pid),
									GET_PROCESS_RANK(m->dst_pid),
									m->transmit_seq,
									m->ack_seq,
									m->length);

	}

	bytes_left = info->out_msg_length - info->out_msg_pos;

	if (info->type == TYPE_COMPUTE_NODE_TCP) {
		INFO(1, "Writing message to compute node %d socket=%d timeout=%ld type=%d bytes=%ld bytes_left=%ld!",
				info->index, info->socketfd, timeout, info->type, info->out_msg_length, bytes_left);

	} else if (info->type == TYPE_GATEWAY_TCP || info->type == TYPE_GATEWAY_UDT) {
		INFO(1, "Writing message to gateway %d socket=%d timeout=%ld type=%d bytes=%ld bytes_left=%ld!",
						info->index, info->socketfd, timeout, info->type, info->out_msg_length, bytes_left);
	}

	written = socket_send(info->socketfd, info->out_msg + info->out_msg_pos, bytes_left, blocking);

	if (written < 0) {
		ERROR(1, "Failed to write message to compute node %d!", info->index);
		return -1;
	}

	info->out_msg_pos += written;
	bytes_left -= written;

	INFO(1, "WROTE %d position now %ld", written, info->out_msg_pos);

	if (bytes_left == 0) {
		// Full message has been written, so free it and clear the out_msg. The next write will queue a new message.

		info->out_msg_count++;
		info->out_msg_bytes_total += info->out_msg_length;

		INFO(1, "Message was completely written count=%ld totalbytes=%ld!", info->out_msg_count, info->out_msg_bytes_total);

		if (info->type == TYPE_COMPUTE_NODE_TCP) {
			fragment_buffer_push_free_fragment(gateway_to_nodes_fragment_buffer, info->out_msg_index);
		} else { // if (info->type == TYPE_SERVER || info->type == TYPE_GATEWAY_TCP || info->type == TYPE_GATEWAY_UDT)
			fragment_buffer_push_free_fragment(nodes_to_gateway_fragment_buffer, info->out_msg_index);
		}

		clear_socket_info_out_msg(info);
		return 0;
	} else {
		INFO(1, "Message was partly written!");
		return 1; // "wouldblock"
	}
}

static int read_message(socket_info *info, bool blocking)
{
	// A socket is ready for reading, so read something!
	int status;
	int64_t timeout;
	ssize_t bytes_read, bytes_needed;
	fragment_buffer *fb;

//	INFO(1, "XXX Receive of message from socket %d", info->socketfd);

	bytes_needed = -1;
	fb = NULL;
	timeout = 0;

	switch (info->state) {
	case STATE_READY:

		// No message fragment available yet, so try to get one. If we fail, we'll simply return 1 (for "wouldblock"). Make sure
		// that we do get the fragment from the appropriate fragment buffer.
		switch (info->type) {
		case TYPE_COMPUTE_NODE_TCP:
			timeout = 0;
			fb = nodes_to_gateway_fragment_buffer;
			break;

		case TYPE_GATEWAY_TCP:
		case TYPE_GATEWAY_UDT:
			timeout = 1000*1000;
			fb = gateway_to_nodes_fragment_buffer;
			break;

		case TYPE_SERVER:
			timeout = 0;
			fb = gateway_to_nodes_fragment_buffer;
			break;

		default:
			FATAL("INTERNAL ERROR: unknown socket_info type %d in socket info %d", info->type, info->socketfd);
		}

		status = fragment_buffer_pop_free_fragment_wait(fb, &(info->in_msg_index), timeout);

		if (status == 0) {
			// We have a buffer.
			info->in_msg = fragment_buffer_get_fragment(fb, info->in_msg_index);
			info->in_msg_pos = 0;
			info->in_msg_length = GENERIC_MESSAGE_HEADER_SIZE;
			info->state = STATE_READ_HEADER;

			// We need to read the message fragment header first.
			bytes_needed = GENERIC_MESSAGE_HEADER_SIZE;

		} else if (status == 1) {
			// We are out of buffer space, so return 1 ("wouldblock")
			return 1;

		} else if (status == -1) {
			FATAL("INTERNAL ERROR: Failed to reserve message fragment for socket info %d", info->socketfd);
		}

		break;

	case STATE_READ_HEADER:
	case STATE_READ_PAYLOAD:

		// The length field contains a valid size, so compute how many bytes we need to read.
		bytes_needed = info->in_msg_length - info->in_msg_pos;
		break;

	case STATE_TERMINATING:

		// We are terminating, so we no more data but need to read a byte to terminate the socket.
		bytes_needed = 1;
		break;

	default:
		FATAL("INTERNAL ERROR: invalid state while reading for socket info %d", info->socketfd);
	}

	while (bytes_needed > 0) {

		bytes_read = socket_receive(info->socketfd, info->in_msg + info->in_msg_pos, bytes_needed, blocking);

//		INFO(1, "XXX Receive of message from socket %d done! pos=%ld needed=%ld read=%ld", info->socketfd, info->in_msg_pos,
//				bytes_needed, bytes_read);

		if (bytes_read > 0) {

			info->in_msg_pos += bytes_read;
			bytes_needed -= bytes_read;

			if (bytes_needed == 0 && info->state == STATE_READ_HEADER) {
				// Complete header has been read, switch to reading payload
				info->state = STATE_READ_PAYLOAD;
				info->in_msg_length = ((generic_message *) info->in_msg)->length;
				bytes_needed = info->in_msg_length - GENERIC_MESSAGE_HEADER_SIZE;
			}

		} else if (bytes_read == 0) {
			// No bytes where read, so return 1 ("wouldblock")
			return 1;

		} else { // if (bytes_read < 0)

			// The connection was lost.
			if (bytes_read == SOCKET_DISCONNECT) {
				return 2;
			}

			ERROR(1, "Failed to read message from compute node %d!", info->index);
			return -1;
		}
	}

	info->in_msg_bytes_total += info->in_msg_length;
	info->in_msg_count++;

	// We have read the complete message
	return 0;
}


// Sender thread for sockets.
void* tcp_sender_thread(void *arg)
{
	bool more;
	int status;
	socket_info *info;
	// message_buffer *m;
	//message_header *mh;

	uint64_t last_write, now, data, count;

	info = (socket_info *) arg;
	more = true;

	now = last_write = current_time_micros();
	data = 0;
	count = 0;

	while (more) {

		status = write_message(info, true);

		if (status == -1) {
			ERROR(1, "Failed to send message!");
			more = false;
		} else if (status == 2) {
			INFO(1, "TCP sender thread done");
			more = false;
		}

		now = current_time_micros();

		// write stats at most once per second.
		if (now > last_write + 1000000) {
			store_sender_thread_stats(info->index, info->out_msg_bytes_total-data, info->out_msg_count-count, now);
			data = info->out_msg_bytes_total;
			count = info->out_msg_count;
			last_write = now;
		}
	}

	// Make sure all statistics are saved when we quit.
	store_sender_thread_stats(info->index, info->out_msg_bytes_total-data, info->out_msg_count-count, now);

	return NULL;
}

void* udt_sender_thread(void *arg)
{
	/*
	size_t avail;
	socket_info *info;
	bool more;
	// message_header *tmp;
	int error; // , msg, i;
//	uint64_t last_write, now, data, count;
//	UDT4_Statistics stats;

	message_buffer *m;
	unsigned char *p;

	info = (socket_info *) arg;
	more = true;

	//now = last_write = current_time_micros();
	//data = 0;
	//count = 0;

	while (more) {
		m = dequeue_message_from_socket_info(info, 1000000);

		if (m != NULL) {

			avail = message_buffer_max_read(m);

		//	fprintf(stderr, "#### UDT Sender sending message of %ld bytes", avail);

			p = message_buffer_direct_read_access(m, avail);

			error = udt_sendfully(info->socketfd, p, avail);

			if (error != SOCKET_OK) {
				ERROR(1, "Failed to send message!");
				return NULL;
			}

			add_ack_bytes((message_header *)m->data);

			if (((message_header *)m->data)->opcode == OPCODE_FINALIZE) {
				more = false;
			}

			message_buffer_destroy(m);

		} else {
//			fprintf(stderr, "#### UDT Sender got empty message .. will retry...");
		}
	}

	//INFO(1, "UDT Sender done!");
*/
	return NULL;
}

// Receiver thread for sockets.
void* tcp_receiver_thread(void *arg)
{
	socket_info *info;
	bool more;
	int error;
	uint64_t last_read, now, data, count;

	info = (socket_info *) arg;
	more = true;

	now = last_read = current_time_micros();
	data = 0;
	count = 0;

	while (more) {

		error = read_message(info, true); // FIXME: should use timeout!

		switch (error) {
		case 0: // Have a complete message, so queue it.
			enqueue_message_from_wa_link(info);
			break;

		case 1:
			// Would block -> no message!
			break;

		case 2: // Disconnect
//			if (info->state == STATE_TERMINATING) {
				// Disconnect was expected!
//				INFO(1, "Disconnecting WA link %d", info->index);
				// socket_remove_from_epoll(epollfd, info->socketfd);
//				close(info->socketfd);
//				info->state = STATE_TERMINATED;



//			} else {
			WARN(1, "Unexpected termination of link to gateway %d", info->index);
//			}
//			shutdown(info->socketfd, SHUT_RD);
			more = false;
			break;

		default: // Also case -1, error
			ERROR(1, "Failed to handle event on connection to gateway %d (error=%d)", info->index, error);
			more = false;
		}

		// Write some statistics (at most once per second).
		now = current_time_micros();

		if (now > last_read + 1000000) {
			store_receiver_thread_stats(info->index, info->in_msg_bytes_total-data, info->in_msg_count-count, now);
			last_read = now;
			data = info->in_msg_bytes_total;
			count = info->in_msg_count;
		}
	}

	// Make sure all statistics are saved when we quit.
	store_receiver_thread_stats(info->index, info->in_msg_bytes_total-data, info->in_msg_count-count, now);

	return NULL;
}

// Receiver thread for sockets.
void* udt_receiver_thread(void *arg)
{
	/*
	socket_info *info;
	bool more;
	message_header mh;
	void *buffer;
	int error;
	uint64_t last_write, now, data, count;
	UDT4_Statistics stats;

	message_buffer *m;

	info = (socket_info *) arg;
	more = true;

	now = last_write = current_time_micros();
	data = 0;
	count = 0;

	while (more) {

		error = udt_receivefully(info->socketfd, (unsigned char *)&mh, MESSAGE_HEADER_SIZE);

		if (error != SOCKET_OK) {
			ERROR(1, "Failed to receive header!");
			return NULL;
		}

		if (mh.length < 0 || mh.length > MAX_MESSAGE_SIZE) {
			ERROR(1, "Receive invalid message header length=%d!\n", mh.length);
			return NULL;
		}

		if (mh.opcode == OPCODE_FINALIZE) {
			more = false;
			break;
		}

		buffer = malloc(mh.length);

		if (buffer == NULL) {
			ERROR(1, "Failed to allocate space for message of length=%d!\n", mh.length);
			return NULL;
		}

		memcpy(buffer, &mh, MESSAGE_HEADER_SIZE);

		error = udt_receivefully(info->socketfd, buffer + MESSAGE_HEADER_SIZE, mh.length-MESSAGE_HEADER_SIZE);

		if (error != SOCKET_OK) {
			ERROR(1, "Failed to receive message payload!");
			return NULL;
		}

		m = message_buffer_wrap(buffer, mh.length, false);
		m->end = mh.length;

		// FIXME: WILL LEAK??
		enqueue_message_from_wa_link(m);

//		blocking_linked_queue_enqueue(info->input_queue, buffer, mh.length, -1);

		data += mh.length;
		count++;
		now = current_time_micros();

		// write stats at most once per second.
		if (now > last_write + 1000000) {
			store_receiver_thread_stats(info->index, data, count, now);
			last_write = now;
			count = 0;
			data = 0;

			udt4_perfmon(info->socketfd, &stats, true);

			fprintf(stderr, "UDT4 RCV STATS estbw=%f rcvrate=%f received=%ld receiveloss=%d RTT=%f window=%d ACK=%d NACK=%d\n",
					stats.mbpsBandwidth, stats.mbpsRecvRate, stats.pktRecv, stats.pktRcvLoss, stats.msRTT,
					stats.pktCongestionWindow, stats.pktSentACK, stats.pktSentNAK);

		}
	}
*/
	// FIXME: close link
	return NULL;
}

static socket_info *create_socket_info(int socketfd, int type, int state)
{
	int error;
	socket_info *info;

	info = (socket_info *) malloc(sizeof(socket_info));

	if (info == NULL) {
		FATAL("Failed to create socket info!");
	}

	INFO(1, "Creating socket info socketfd=%d type=%d state=%d pointer=%p", socketfd, type, state, info);

	info->socketfd = socketfd;
	info->type = type;
	info->state = state;
	info->index = -1;
	info->done = false;

	// info->pending_ack_bytes = 0;

	info->in_msg = NULL;
	info->in_msg_index = -1;
	info->in_msg_pos = 0;
	info->in_msg_length = 0;
	info->in_msg_count = 0;
	info->in_msg_bytes_total = 0;

	info->out_msg = NULL;
	info->out_msg_index = -1;
	info->out_msg_pos = 0;
	info->out_msg_length = 0;
	info->out_msg_count = 0;
	info->out_msg_bytes_total = 0;

	// Remove this ??
	// info->input_queue = NULL;

	// info->output_queue = linked_queue_create(-1);
	info->output_queue = malloc(8 * sizeof(int));

	if (info->output_queue == NULL) {
		FATAL("Failed to create socket info!");
	}

	info->max_output_queue_size = 8;
	info->output_queue_size = 0;
	info->output_queue_head = 0;
	info->output_queue_tail = 0;

	error = pthread_cond_init(&(info->output_cond), NULL);

	if (error != 0) {
		FATAL("Failed to create socket info!");
	}

	error = pthread_mutex_init(&(info->output_mutex), NULL);

	if (error != 0) {
		FATAL("Failed to create socket info!");
	}

//	error = pthread_mutex_init(&(info->bytes_mutex), NULL);

//	if (error != 0) {
		//FATAL("Failed to create socket info!");
	//}

	return info;
}

static int init_socket_info_threads(socket_info *info)
{
	int error;

	if (info->type == TYPE_GATEWAY_TCP) {

		INFO(1, "Starting gateway TCP receiver thread!");

		error = pthread_create(&(info->receive_thread), NULL, &tcp_receiver_thread, info);

		if (error != 0) {
			ERROR(1, "Failed to create TCP receive thread for socket %d!", info->socketfd);
			return ERROR_ALLOCATE;
		}

		INFO(1, "Starting gateway TCP sender thread!");

		error = pthread_create(&(info->send_thread), NULL, &tcp_sender_thread, info);

		if (error != 0) {
			ERROR(1, "Failed to create TCP sender thread for socket %d!", info->socketfd);
			return ERROR_ALLOCATE;
		}

 	} else if (info->type == TYPE_GATEWAY_UDT) {

		INFO(1, "Starting gateway UDT receiver thread!");

		error = pthread_create(&(info->receive_thread), NULL, &udt_receiver_thread, info);

		if (error != 0) {
			ERROR(1, "Failed to create UDT receive thread for socket %d!", info->socketfd);
			return ERROR_ALLOCATE;
		}

		INFO(1, "Starting gateway UDT sender thread!");

		error = pthread_create(&(info->send_thread), NULL, &udt_sender_thread, info);

		if (error != 0) {
			ERROR(1, "Failed to create UDT sender thread for socket %d!", info->socketfd);
			return ERROR_ALLOCATE;
		}
	}

	return 0;
}

static int start_gateway_threads(int index)
{
    int i, status;

    // We do not need to add a connection to ourselves!
    if (index == cluster_rank) {
       return CONNECT_OK;
    }

    for (i=0;i<gateway_connections[index].stream_count;i++) {

    	INFO(1, "Starting gatways thread %d.%d", index, i);

    	status = init_socket_info_threads(gateway_connections[index].sockets[i]);

    	if (status != CONNECT_OK) {
    		ERROR(1, "Failed to start threads for gateway %d.%d!", index, i);
    		return status;
    	}
    }

    return CONNECT_OK;
}

static int receive_gateway_ready_opcode(int index)
{
	int status;
	int opcode;
	int socketfd;

	status = 0;
	socketfd = gateway_connections[index].sockets[0]->socketfd;

	if (gateway_connections[index].sockets[0]->type == TYPE_GATEWAY_TCP) {
		status = socket_receivefully(socketfd, (unsigned char *) &opcode, 4);
	} else if (gateway_connections[index].sockets[0]->type == TYPE_GATEWAY_UDT) {
		status = udt_receivefully(socketfd, (unsigned char *) &opcode, 4);
	} else {
		FATAL("Unknown gateway protocol (%d)!", gateway_connections[index].sockets[0]->type);
	}

	if (status != SOCKET_OK) {
		ERROR(1, "Handshake with gateway failed! (%d)", status);
		return ERROR_HANDSHAKE_FAILED;
	}

	return 0;
}

static int send_gateway_ready_opcode(int index)
{
	int status = 0;
	int opcode = OPCODE_GATEWAY_READY;
	int socketfd = gateway_connections[index].sockets[0]->socketfd;

	if (gateway_connections[index].sockets[0]->type == TYPE_GATEWAY_TCP) {
		status = socket_sendfully(socketfd, (unsigned char *) &opcode, 4);
	} else if (gateway_connections[index].sockets[0]->type == TYPE_GATEWAY_UDT) {
		status = udt_sendfully(socketfd, (unsigned char *) &opcode, 4);
	} else {
		FATAL("Unknown gateway protocol (%d)!", gateway_connections[index].sockets[0]->type);
	}

	if (status != SOCKET_OK) {
		ERROR(1, "Handshake with gateway failed! (%d)", status);
		return ERROR_HANDSHAKE_FAILED;
	}

	return 0;
}

static int connect_to_gateways(int crank, int local_port)
{
	int remoteIndex, i, s;
	int status, socket;

	if (crank == cluster_rank) {
		// I must initiate the connection!
		for (i=cluster_rank+1;i<cluster_count;i++) {
			remoteIndex = i*gateway_count + gateway_rank;

			gateway_connections[i].stream_count = gateway_addresses[remoteIndex].streams;
			gateway_connections[i].sockets = malloc(gateway_connections[i].stream_count * sizeof(socket_info *));
			gateway_connections[i].next_output_stream = 0;
			gateway_connections[i].next_input_stream = 0;

			// gateway_connections[i].mpi_receive_buffer = create_message_buffer();

			for (s=0;s<gateway_addresses[remoteIndex].streams;s++) {

				INFO(2, "Connecting to gateway stream %d.%d.%d -> index = %d", i, gateway_rank, s, remoteIndex);

				if (gateway_addresses[remoteIndex].protocol == WIDE_AREA_PROTOCOL_TCP) {
					// Create a path to the target gateway.
					status = socket_connect(gateway_addresses[remoteIndex].ipv4, gateway_addresses[remoteIndex].port + s, -1, -1,
							&socket);

					if (status != CONNECT_OK) {
						ERROR(1, "Failed to connect!");
						return status;
					}

					gateway_connections[i].sockets[s] = create_socket_info(socket, TYPE_GATEWAY_TCP, STATE_READY);

				} else if (gateway_addresses[remoteIndex].protocol == WIDE_AREA_PROTOCOL_UDT) {
					status = udt_connect(gateway_addresses[remoteIndex].ipv4, gateway_addresses[remoteIndex].port + s, -1, -1,
							&socket);

					if (status != CONNECT_OK) {
						ERROR(1, "Failed to connect!");
						return status;
					}

					gateway_connections[i].sockets[s] = create_socket_info(socket, TYPE_GATEWAY_UDT, STATE_READY);
				}

				INFO(1, "Created connection to remote gateway stream %d.%d.%d socket = %d!", i, gateway_rank, s, socket);
			}
		}

		for (i=cluster_rank+1;i<cluster_count;i++) {
			send_gateway_ready_opcode(i);
		}

	} else if (crank < cluster_rank) {
		// I must accept the connection!
		remoteIndex = crank*gateway_count + gateway_rank;

		gateway_connections[crank].stream_count = gateway_addresses[remoteIndex].streams;
		gateway_connections[crank].sockets = malloc(gateway_connections[crank].stream_count * sizeof(socket_info *));
		gateway_connections[crank].next_output_stream = 0;
		gateway_connections[crank].next_input_stream = 0;

		for (s=0;s<gateway_addresses[remoteIndex].streams;s++) {

			INFO(2, "Accepting from gateway stream %d.%d.%d -> index = %d", crank, gateway_rank, s, remoteIndex);

			if (gateway_addresses[remoteIndex].protocol == WIDE_AREA_PROTOCOL_TCP) {
				// Create a path to the target gateway.
				status = socket_accept_one(local_port + s, gateway_addresses[remoteIndex].ipv4, -1, -1, &socket);

				if (status != CONNECT_OK) {
					ERROR(1, "Failed to accept!");
					return status;
				}

				gateway_connections[crank].sockets[s] = create_socket_info(socket, TYPE_GATEWAY_TCP, STATE_READY);

			} else if (gateway_addresses[remoteIndex].protocol == WIDE_AREA_PROTOCOL_UDT) {
				status = udt_accept(local_port + s, gateway_addresses[remoteIndex].ipv4, -1, -1, &socket);

				if (status != CONNECT_OK) {
					ERROR(1, "Failed to accept!");
					return status;
				}

				gateway_connections[crank].sockets[s] = create_socket_info(socket, TYPE_GATEWAY_UDT, STATE_READY);
			}

			INFO(1, "Accepted connection from remote gateway %d.%d.%d socket = %d!", crank, gateway_rank, s, socket);
		}

		// Once the connection has been established, we must
		// wait until the gateway ready opcode is received.
		// This ensures that the gateway that initiated the
		// connection has finished completely (needed to prevent
		// race conditions during connection setup).
		receive_gateway_ready_opcode(crank);
	}

	return CONNECT_OK;
}

static int connect_gateways()
{
	int i, local_port, status;

	// NOTE: Each gateway connects to -one- other gateway in every other cluster.
	gateway_connections = malloc(sizeof(gateway_connection) * cluster_count);

	if (gateway_connections == NULL) {
		ERROR(1, "Failed to allocate space for gateway connections!");
		return ERROR_ALLOCATE;
	}

	memset(gateway_connections, 0, sizeof(gateway_connections) * cluster_count);

	local_port = gateway_addresses[cluster_rank*gateway_count + gateway_rank].port;

	// We connect the gateways in two phases here.

	// First, we do an SMPD-style all-to-all connection setup.
	for (i=0;i<cluster_count;i++) {
		status = connect_to_gateways(i, local_port);

		if (status != CONNECT_OK) {
			ERROR(1, "Failed to connect to gateway %d (error=%d)", i, status);
			return status;
		}
	}

	// Second, we start the send and receive threads for all connections.
	for (i=0;i<cluster_count;i++) {

		if (i != cluster_rank) {
			status = start_gateway_threads(i);

			if (status != CONNECT_OK) {
				ERROR(1, "Failed to add gateway %d to epoll (error=%d)", i, status);
				return status;
			}
		}
	}

	return 0;
}


#ifdef DETAILED_TIMING
static void flush_read_timings()
{
	int i;

	for (i=0;i<read_timing_count;i++) {
		printf("READ TIMINGS GATEWAY %d.%d %d %ld %ld %ld\n",
				cluster_rank, gateway_rank, i,
				read_timings[i].starttime,
				read_timings[i].endtime,
				read_timings[i].size);
	}

	read_timing_count = 0;
	//   fflush(stdout);
}

static void store_read_timings(uint64_t starttime, uint64_t endtime, uint64_t size)
{
	if (read_timing_count >= DETAILED_TIMING_COUNT) {
		flush_read_timings();
	}

	read_timings[read_timing_count].starttime = starttime;
	read_timings[read_timing_count].endtime = endtime;
	read_timings[read_timing_count].size = size;
	read_timing_count++;
}
#endif // DETAILED_TIMING


#ifdef DETAILED_TIMING
static void flush_write_timings()
{
	int i;

	for (i=0;i<write_timing_count;i++) {
		printf("WRITE TIMINGS GATEWAY %d.%d %d %ld %ld %ld\n",
				cluster_rank, gateway_rank, i,
				write_timings[i].starttime,
				write_timings[i].endtime,
				write_timings[i].size);
	}

	write_timing_count = 0;
	//   fflush(stdout);
}

static void store_write_timings(uint64_t starttime, uint64_t endtime, uint64_t size)
{
	if (write_timing_count >= DETAILED_TIMING_COUNT) {
		flush_write_timings();
	}

	write_timings[write_timing_count].starttime = starttime;
	write_timings[write_timing_count].endtime = endtime;
	write_timings[write_timing_count].size = size;
	write_timing_count++;

}
#endif // DETAILED_TIMING

void cleanup()
{
	int opcode, error;

	// The master gateway needs to tell the server that this cluster is ready to stop.
	// Only after all master gateways in all clusters are ready will the server reply and
	// are we allowed to continue.

	error = socket_set_blocking(serverfd);

	if (error != SOCKET_OK) {
		WARN(1, "Failed to set server socket to BLOCKING mode! (error=%d)", error);
	}

	opcode = OPCODE_CLOSE_LINK;

	error = socket_sendfully(serverfd, (unsigned char*) &opcode, 4);

	if (error != SOCKET_OK) {
		WARN(1, "Failed to send OPCODE_CLOSE_LINK to server! (error=%d)", error);
	}

	error = socket_receivefully(serverfd, (unsigned char*) &opcode, 4);

	if (error != SOCKET_OK) {
		WARN(1, "Failed to receive OPCODE_CLOSE_LINK from server! (error=%d)", error);
	}


	// We are now sure that everyone agrees that we should stop. We can now disconnect all
	// socket connections without producing unexpected EOFs on the other side!
	// disconnect_gateways();

	// Finally close the epoll socket and serverfd socket.
	close(epollfd);
	close(serverfd);

	// TODO: free lots of data structures!
}

/*
static void print_gateway_statistics(uint64_t deltat)
{
	int i,j;


	for (i=0;i<cluster_count;i++) {
		if (i != cluster_rank) {
			for (j=0;j<gateway_connections[i].stream_count;j++) {
				printf("STATS FOR GATEWAY STREAM %d.%d.%d TO %d.%d.%d AFTER %ld.%03ld IN %ld %ld OUT %ld %ld PENDING %ld %ld\n",
						cluster_rank, gateway_rank, j, i, gateway_rank, j, sec, millis,
						gateway_connections[i].sockets[j].in_messages,
						gateway_connections[i].sockets[j].in_bytes,
						gateway_connections[i].sockets[j].out_messages,
						gateway_connections[i].sockets[j].out_bytes,
						pending_data_messages, pending_data_size);
			}
		}
	}

#ifdef DETAILED_TIMING
	flush_write_timings();
	flush_read_timings();
#endif

	fflush(stdout);
}
*/



static void print_gateway_statistics(uint64_t deltat)
{
	uint64_t sec, millis, dt;
        uint64_t receive_data, receive_count, send_data, send_count;
        uint64_t delta_receive_data, delta_receive_count, delta_send_data, delta_send_count;
        double gbit_in, gbit_out;
        uint64_t prate_in, prate_out;

	sec = deltat / 1000000UL;
	millis = (deltat % 1000000UL) / 1000UL;

	dt = deltat - prev_deltat;

	// Retrieve stats for sending and receiving
	retrieve_receiver_thread_stats(&receive_data, &receive_count);
	retrieve_sender_thread_stats(&send_data, &send_count);

        delta_receive_data = receive_data - prev_received_data;
        delta_receive_count = receive_count - prev_received_count;

        delta_send_data = send_data - prev_send_data;
        delta_send_count = send_count - prev_send_count;

        gbit_in =  (delta_receive_data * 8) / (1000.0*dt);
        gbit_out = (delta_send_data * 8) / (1000.0*dt);

        prate_in = delta_receive_count / (dt / 1000000.0);
        prate_out = delta_send_count / (dt / 1000000.0);

	printf("STATS FOR GATEWAY %d AFTER %ld.%03ld - WA_IN_TOT %ld bytes %ld packets WA_OUT_TOT %ld bytes %ld packets -- WA_IN %f gbit/s %ld pckt/s WA_OUT %f gbit/s %ld pckt/s\n",
			cluster_rank, sec, millis,
			receive_data, receive_count,
			send_data, send_count,
			gbit_in, prate_in,
			gbit_out, prate_out);

        prev_received_data = receive_data;
        prev_received_count = receive_count;

        prev_send_data = send_data;
        prev_send_count = send_count;

	prev_deltat = deltat;

	fflush(stdout);
}

/*****************************************************************************/
/*                      Reading configuration files                          */
/*****************************************************************************/

static int read_config_file()
{
	int  error;
	char *file;
	FILE *config;
	char buffer[1024];

	file = getenv("EMPI_GATEWAY_CONFIG");

	if (file == NULL) {
		WARN(0, "EMPI_GATEWAY_CONFIG not set!");
		return ERROR_CONFIG;
	}

	INFO(0, "Looking for config file %s", file);

	config = fopen(file, "r");

	if (config == NULL) {
		ERROR(1, "Failed to open config file %s", file);
		return ERROR_CONFIG;
	}

	INFO(0, "Config file %s opened.", file);

	// Read the cluster name
	error = fscanf(config, "%s", buffer);

	if (error == EOF || error == 0) {
		fclose(config);
		ERROR(1, "Failed to read cluster name from %s", file);
		return ERROR_CONFIG;
	}

	cluster_name = malloc(strlen(buffer)+1);

	if (cluster_name == NULL) {
		fclose(config);
		ERROR(1, "Failed to allocate space for cluster name %s", buffer);
		return ERROR_CONFIG;
	}

	strcpy(cluster_name, buffer);

	// Read the server address
	error = fscanf(config, "%s", buffer);

	if (error == EOF || error == 0) {
		fclose(config);
		ERROR(1, "Failed to read server address from %s", file);
		return ERROR_CONFIG;
	}

	server_name = malloc(strlen(buffer+1));

	if (server_name == NULL) {
		fclose(config);
		ERROR(1, "Failed to allocate space for server address %s", buffer);
		return ERROR_CONFIG;
	}

	strcpy(server_name, buffer);

	// Read the server port
	error = fscanf(config, "%hu", &server_port);

	if (error == EOF || error == 0) {
		fclose(config);
		ERROR(1, "Failed to read server port from %s", file);
		return ERROR_CONFIG;
	}

	// Read the server port
	error = fscanf(config, "%hu", &local_port);

	if (error == EOF || error == 0) {
		fclose(config);
		ERROR(1, "Failed to read local port from %s", file);
		return ERROR_CONFIG;
	}

	fclose(config);
	return 0;
}

static int init_cluster_info(int argc, char **argv)
{
	int i, len, error;

	cluster_name = NULL;
	server_name = NULL;
	server_port = -1;

	// First try to read the configuration from an input file whose location is set in the environment.
	// This is needed since Fortran does not pass the command line arguments to the MPI library like C does.
	error = read_config_file();

	if (error != 0) {
		return error;
	}

	// Next, parse the command line (possibly overwriting the config).
	i = 1;

	while ( i < argc ){

		if ( strcmp(argv[i],"--server") == 0 ) {
			if ( i+1 < argc ){
				server_name = malloc(strlen(argv[i+1])+1);
				strcpy(server_name, argv[i+1]);
				i++;
			} else {
				ERROR(1, "Missing option for --server");
				return ERROR_ARGUMENT;
			}

		} else if ( strcmp(argv[i],"--server-port") == 0 ) {
			if ( i+1 < argc ){
				server_port = (unsigned short) atoi(argv[i+1]);
				i++;
			} else {
				ERROR(1, "Missing option for --server-port");
				return ERROR_ARGUMENT;
			}

		} else if ( strcmp(argv[i],"--cluster-name") == 0 ) {
			if ( i+1 < argc ){

				len = strlen(argv[i+1]);

				if (len >= MAX_LENGTH_CLUSTER_NAME) {
					ERROR(1, "Cluster name too long (%d)", len);
					return ERROR_ARGUMENT;
				} else {
					cluster_name = malloc(len+1);
					strcpy(cluster_name, argv[i+1]);
					i++;
				}
			} else {
				ERROR(1, 0, "Missing option for --cluster-name");
				return ERROR_ARGUMENT;
			}
		} else if ( strcmp(argv[i],"--local-port") == 0 ) {
			if ( i+1 < argc ){
				local_port = (unsigned short) atoi(argv[i+1]);
				i++;
			} else {
				ERROR(1, "Missing option for --local-port");
				return ERROR_ARGUMENT;
			}
		}

		i++;
	}

	return 0;
}

/*****************************************************************************/
/*                   Initial communication with server                       */
/*****************************************************************************/

static int handshake()
{
	// A handshake consist of a single RPC that sends the name of this cluster
	// to the server and gets a cluster rank and clusters count as a reply (or an error).
	int error;

	// The maximum size of the handshake message is (2*4 + MAX_LENGTH_CLUSTER_NAME) bytes
	unsigned char message[2*4+MAX_LENGTH_CLUSTER_NAME];
	unsigned int *message_i;
	unsigned int reply[6];
	unsigned int opcode;

	message_i = (unsigned int *) message;

	memset(message, 0, 2*4+MAX_LENGTH_CLUSTER_NAME);

	message_i[0] = OPCODE_HANDSHAKE;
	message_i[1] = strlen(cluster_name);

	strcpy((char*) (message+(2*4)), cluster_name);

	error = socket_sendfully(serverfd, message, 2*4+strlen(cluster_name));

	if (error != SOCKET_OK) {
		ERROR(1, "Handshake with server failed! (%d)", error);
		return ERROR_HANDSHAKE_FAILED;
	}

	error = socket_receivefully(serverfd, (unsigned char *)&opcode, 4);

	if (error != SOCKET_OK) {
		ERROR(1, "Handshake with server failed! (%d)", error);
		return ERROR_HANDSHAKE_FAILED;
	}

	if (opcode != OPCODE_HANDSHAKE_ACCEPTED) {
		ERROR(1, "Server refused handshake! (%d)", opcode);
		return ERROR_HANDSHAKE_FAILED;
	}

	error = socket_receivefully(serverfd, (unsigned char *)reply, 6*4);

	if (error != SOCKET_OK) {
		ERROR(1, "Handshake with server failed! (%d)", error);
		return ERROR_HANDSHAKE_FAILED;
	}

	cluster_rank           = reply[0];
	cluster_count          = reply[1];
	local_application_size = reply[2];
	gateway_count          = reply[3];
	fragment_size          = reply[4];
	fragment_count         = reply[5];

	INFO(1, "Received following configuration from server:");
	INFO(1, "  Cluster rank  : %d", cluster_rank);
	INFO(1, "  Cluster count : %d", cluster_count);
	INFO(1, "  Local   size  : %d", local_application_size);
	INFO(1, "  Gateway count : %d", gateway_count);
	INFO(1, "  Fragment size : %d", fragment_size);
	INFO(1, "  Fragment count: %d", fragment_count);

	if (cluster_count == 0 || cluster_count >= MAX_CLUSTERS) {
		ERROR(1, "Cluster count out of bounds! (%d)", cluster_count);
		return ERROR_HANDSHAKE_FAILED;
	}

	if (cluster_rank >= cluster_count) {
		ERROR(1, "Cluster rank out of bounds! (%d >= %d)", cluster_rank, cluster_count);
		return ERROR_HANDSHAKE_FAILED;
	}

	if (local_application_size == 0 || local_application_size >= MAX_PROCESSES_PER_CLUSTER) {
		ERROR(1, "Local application size out of bounds! (%d)", local_application_size);
		return ERROR_HANDSHAKE_FAILED;
	}

	if (gateway_count == 0) {
		ERROR(1, "Local gateway count too low! (%d)", gateway_count);
		return ERROR_HANDSHAKE_FAILED;
	}

	return 0;
}

static int receive_cluster_sizes()
{
	int error, i;

	error = socket_receivefully(serverfd, (unsigned char*) cluster_sizes, cluster_count * 4);

	if (error != SOCKET_OK) {
		ERROR(1, "Handshake with server failed! (%d)", error);
		return ERROR_HANDSHAKE_FAILED;
	}

	cluster_offsets[0] = 0;

	for (i=1;i<=cluster_count;i++) {
		cluster_offsets[i] = cluster_offsets[i-1] + cluster_sizes[i-1];
	}

	for (i=0;i<cluster_count;i++) {
		INFO(0, "Cluster %d %d %d", i, cluster_sizes[i], cluster_offsets[i]);
	}

	INFO(0, "Total size %d", cluster_offsets[cluster_count]);

	return 0;
}

static int send_gateway_info(int rank, struct in_addr *ip4ads, int ip4count)
{
	int error;
	gateway_request_msg req;

	req.opcode  = OPCODE_GATEWAY_INFO;
	req.cluster = cluster_rank;
	req.src     = rank;
	req.count   = ip4count;

	error = socket_sendfully(serverfd, (unsigned char *) &req, GATEWAY_REQUEST_SIZE);

	if (error != SOCKET_OK) {
		ERROR(1, "Failed to send cluster info header to server!");
		return ERROR_HANDSHAKE_FAILED;
	}

	error = socket_sendfully(serverfd, (unsigned char *) ip4ads, ip4count * sizeof(struct in_addr));

	if (error != SOCKET_OK) {
		ERROR(1, "Failed to send cluster info data to server!");
		return ERROR_HANDSHAKE_FAILED;
	}

	return 0;
}

static int receive_gateway(int index)
{
	int error;
	unsigned long ipv4;
	unsigned short port;
	unsigned short protocol;
	unsigned short streams;

	error = socket_receivefully(serverfd, (unsigned char*) &ipv4, 4);

	if (error != SOCKET_OK) {
		ERROR(1, "Receive of gateway info failed! (ipv4, error=%d)", error);
		return ERROR_HANDSHAKE_FAILED;
	}

	error = socket_receivefully(serverfd, (unsigned char*) &port, 2);

	if (error != SOCKET_OK) {
		ERROR(1, "Receive of gateway info failed! (port, error=%d)", error);
		return ERROR_HANDSHAKE_FAILED;
	}

	error = socket_receivefully(serverfd, (unsigned char*) &protocol, 2);

	if (error != SOCKET_OK) {
		ERROR(1, "Receive of gateway info failed! (protocol, error=%d)", error);
		return ERROR_HANDSHAKE_FAILED;
	}

	error = socket_receivefully(serverfd, (unsigned char*) &streams, 2);

	if (error != SOCKET_OK) {
		ERROR(1, "Receive of gateway info failed! (stream, error=%d)", error);
		return ERROR_HANDSHAKE_FAILED;
	}

	gateway_addresses[index].ipv4 = ipv4;
	gateway_addresses[index].port = port;
	gateway_addresses[index].protocol = protocol;
	gateway_addresses[index].streams = streams;

	WARN(2, "Received gateway info %d address=%d port %d protocol %d streams %d", index, ipv4, port, protocol, streams);

	return 0;
}

static int receive_gateway_info()
{
	int i,j, error;

	for (i=0;i<cluster_count;i++) {
		for (j=0;j<gateway_count;j++) {

			INFO(2, "Receiving gateway info for gateway %d/%d", i, j);

			error = receive_gateway(i*gateway_count+j);

			if (error != 0) {
				return error;
			}
		}
	}

	INFO(2, "All gateway info received!");

	return 0;
}

static int all_clear_barrier()
{
	int error, opcode;

	opcode = OPCODE_GATEWAY_READY;

	error = socket_sendfully(serverfd, (unsigned char *) &opcode, sizeof(int));

	if (error != SOCKET_OK) {
		ERROR(1, "Failed to send OPCODE_GATEWAY_READY to server!");
		return ERROR_HANDSHAKE_FAILED;
	}

	error = socket_receivefully(serverfd, (unsigned char *) &opcode, sizeof(int));

	if (error != SOCKET_OK) {
		ERROR(1, "Failed to send OPCODE_GATEWAY_READY from server!");
		return ERROR_HANDSHAKE_FAILED;
	}

	return 0;
}

static int send_all_clear()
{
	int error, opcode;

	opcode = OPCODE_GATEWAY_READY;

	error = socket_sendfully(local_compute_nodes[0]->socketfd, (unsigned char *) &opcode, sizeof(int));

	if (error != SOCKET_OK) {
		ERROR(1, "Failed to send OPCODE_GATEWAY_READY to compute node 0!");
		return ERROR_HANDSHAKE_FAILED;
	}

	return 0;
}


/*****************************************************************************/
/*                Initial communication with compute nodes                   */
/*****************************************************************************/


static int read_handshake_compute_node(socket_info *info)
{
	int error;
	uint32_t *msg;
	uint32_t size;
	ssize_t bytes_read, bytes_needed;
	size_t offset;

	if (info->state != STATE_READING_HANDSHAKE) {
		ERROR(1, "Socket %d in invalid state %d", info->socketfd, info->state);
		return ERROR_HANDSHAKE_FAILED;
	}

	bytes_needed = info->in_msg_length - info->in_msg_pos;

	bytes_read = socket_receive(info->socketfd, info->in_msg + info->in_msg_pos, bytes_needed, false);

	if (bytes_read < 0) {
		ERROR(1, "Failed to read handshake message of compute node!");
		return ERROR_HANDSHAKE_FAILED;
	}

	info->in_msg_pos += bytes_read;
	bytes_needed -= bytes_read;

	if (bytes_needed == 0) {
		// Full message has been read. Process it and send reply back!
		msg = (uint32_t *) info->in_msg;

		if (msg[0] != OPCODE_HANDSHAKE) {
			// Invalid handshake!
			close(info->socketfd);
			return ERROR_HANDSHAKE_FAILED;
		}

		// Retrieve the rank of the connecting machine.
		info->index = msg[1];
		size = msg[2];

		free(info->in_msg);
		info->in_msg = NULL;
		info->in_msg_length = 0;
		info->in_msg_pos = 0;

		INFO(2, "Received handshake from compute node %d of %d socketfd=%d type=%d state=%d", info->index, size, info->socketfd, info->type, info->state);

		if (size != local_application_size) {
			ERROR(1, "Size reported by compute node (%d) does not match configured size (%d)!", size, local_application_size);
			close(info->socketfd);
			return ERROR_HANDSHAKE_FAILED;
		}

		if (info->index < 0 || info->index >= local_application_size) {
			ERROR(1, "Rank reported by compute node (%d) out of bounds (0-%d)!", info->index, local_application_size);
			close(info->socketfd);
			return ERROR_HANDSHAKE_FAILED;
		}

		if (local_compute_nodes[info->index] != NULL) {
			ERROR(1, "Rank reported by compute node (%d) already in use!", info->index);
			close(info->socketfd);
			return ERROR_HANDSHAKE_FAILED;
		}

		local_compute_nodes[info->index] = info;

		if (info->index == 0) {
			// We send the compute node with rank 0 all information about the clusters.
			info->out_msg = malloc(4 * sizeof(uint32_t) + (2*cluster_count+1) * sizeof(int));
			info->out_msg_pos = 0;
			info->out_msg_length = 4 * sizeof(uint32_t) + (2*cluster_count+1) * sizeof(int);

			msg = (uint32_t *) info->out_msg;

			msg[0] = OPCODE_HANDSHAKE_ACCEPTED;
			msg[1] = cluster_rank;
			msg[2] = cluster_count;
			msg[3] = fragment_size;

			offset = 4 * sizeof(uint32_t);

			memcpy(info->out_msg + offset, (unsigned char *)cluster_sizes, cluster_count * sizeof(int));

			offset += cluster_count * sizeof(int);

			memcpy(info->out_msg + offset, (unsigned char *)cluster_offsets, (cluster_count+1) * sizeof(int));

		} else {
			// We send the compute node with rank != 0 a simple opcode.
			info->out_msg = malloc(sizeof(uint32_t));
			info->out_msg_pos = 0;
			info->out_msg_length = sizeof(uint32_t);

			msg = (uint32_t *) info->out_msg;
			msg[0] = OPCODE_HANDSHAKE_ACCEPTED;
		}

		info->state = STATE_WRITING_HANDSHAKE;

		error = socket_set_wo(epollfd, info->socketfd, info);

		if (error != 0) {
			return error;
		}
	}

	return 0;
}

static int write_handshake_compute_node(socket_info *info)
{
	int error;
	ssize_t written, bytes_left;

	if (info->state != STATE_WRITING_HANDSHAKE) {
		ERROR(1, "Socket %d in invalid state %d", info->socketfd, info->state);
		return -1; // error
	}

//	error = perform_non_blocking_write(info);

	bytes_left = info->out_msg_length - info->out_msg_pos;

	written = socket_send(info->socketfd, info->out_msg + info->out_msg_pos, bytes_left, false);

	if (written < 0) {
		ERROR(1, "Failed to write handshake message to compute node!");
		return -1; // error
	}

	info->out_msg_pos += written;
	bytes_left -= written;

	if (bytes_left == 0) {
		// Full message has been written. Socket will be idle until all nodes have connected.
		free(info->out_msg);
		info->out_msg = NULL;
		info->out_msg_length = 0;
		info->out_msg_pos = 0;
		info->state = STATE_IDLE;

		error = socket_set_idle(epollfd, info->socketfd, info);

		if (error != 0) {
			ERROR(1, "Failed to set socket to idle state!");
			return -1; // error
		}

		INFO(2, "Handshake with compute node %d of %d completed!", info->index, local_application_size);

		pending_local_connections--;
		local_connections++;

		return 0; // done
	}

	return 1; // wouldblock
}


static int accept_new_connection(socket_info *info)
{
	int error, new_socket;
	socket_info *new_info;

	new_socket = accept(info->socketfd, NULL, NULL);

	if (new_socket == -1) {
		ERROR(1, "Failed to accept socket!");
		return ERROR_ACCEPT;
	}

	error = socket_set_non_blocking(new_socket);

	if (error != 0) {
		ERROR(1, "Failed to set socket to non-blocking!");
		return error;
	}

	new_info = create_socket_info(new_socket, TYPE_COMPUTE_NODE_TCP, STATE_READING_HANDSHAKE);

	if (new_info == NULL) {
		return ERROR_ALLOCATE;
	}

	// Allocate a temporary message buffer.
	new_info->in_msg = malloc(3 * sizeof(uint32_t));

	if (new_info->in_msg == NULL) {
		ERROR(1, "Failed to allocate handshake message!");
		return ERROR_ALLOCATE;
	}

	new_info->in_msg_length = 3 * sizeof(uint32_t);

	error = socket_add_to_epoll(epollfd, new_socket, new_info);

	if (error != 0) {
		ERROR(1, "Failed to add listen socket to epoll set!");
		return error;
	}

	pending_local_connections++;

	return 0;
}

static int accept_local_connections()
{
	int listenfd, error, count, i;
	struct epoll_event events[10];
	socket_info *info, *listen_info;

	local_connections = 0;
	pending_local_connections = 0;

	// Allocate space for the local connection info.
	local_compute_nodes = calloc(local_application_size, sizeof(socket_info *));

	if (local_compute_nodes == NULL) {
		WARN(1, "Failed to allocate space for local compute node connection info!");
		close(serverfd);
		return ERROR_ALLOCATE;
	}

	// memset(local_compute_nodes, 0, local_application_size * sizeof(socket_info *));

	// open local server socket at local_port
	error = socket_listen(local_port, 0, 0, 100, &listenfd);

	if (error != SOCKET_OK) {
		WARN(1, "Failed to open socket to listen for local connections!");
		return error;
	}

	error = socket_set_non_blocking(listenfd);

	if (error != 0) {
		close(listenfd);
		ERROR(1, "Failed to set listen socket to non-blocking!");
		return error;
	}

	listen_info = create_socket_info(listenfd, TYPE_ACCEPT, STATE_ACCEPTING);

	if (listen_info == NULL) {
		return ERROR_ALLOCATE;
	}

	error = socket_add_to_epoll(epollfd, listenfd, listen_info);

	if (error != 0) {
		close(listenfd);
		ERROR(1, "Failed to add listen socket to epoll set!");
		return error;
	}

	INFO(1, "Gateway accepting connections from %d local compute nodes on port %d", local_application_size, local_port);

	while (local_connections < local_application_size) {

		count = epoll_wait(epollfd, events, 10, 1000);

		if (count < 0) {
			close(listenfd);
			ERROR(1, "Failed to wait for socket event!");
			return ERROR_POLL;
		}

		if (count > 0) {
			for (i=0;i<count;i++) {

				info = events[i].data.ptr;

				if ((events[i].events & (EPOLLERR | EPOLLHUP)) != 0) {
					// Error or hang up occurred in file descriptor!
					ERROR(1, "Unexpected error on socket %d", info->socketfd);
					return ERROR_POLL;
				}

				if (info->state == STATE_ACCEPTING) {
					error = accept_new_connection(info);
				} else if (info->state == STATE_READING_HANDSHAKE) {
					// do connection handshake with compute node
					error = read_handshake_compute_node(info);
				} else if (info->state == STATE_WRITING_HANDSHAKE) {
					// do connection handshake with compute node
					error = write_handshake_compute_node(info);
				} else {
					ERROR(1, "Socket %d in invalid state %d", info->socketfd, info->state);
					return ERROR_HANDSHAKE_FAILED;
				}

				if (error != 0) {
					return error;
				}
			}
		}

		INFO(2, "Gateway now has %d pending and %d completed local connections", pending_local_connections, local_connections);
	}

	// Remove the accept file descriptor from the epoll set.
	socket_remove_from_epoll(epollfd, listenfd);

	close(listenfd);
	free(listen_info);

	INFO(2, "All %d local compute are connected", local_application_size);

	// Switching all sockets to the local compute nodes to read mode.
	for (i=0;i<local_application_size;i++) {
		error = socket_set_ro(epollfd, local_compute_nodes[i]->socketfd, local_compute_nodes[i]);

		if (error != SOCKET_OK) {
			ERROR(1, "Failed to set connection to compute node %d to read mode!", i);
			return ERROR_CONNECTION;
		}

		local_compute_nodes[i]->state = STATE_READY;
		local_compute_nodes[i]->in_msg = NULL;
	}

	return 0;
}


/*****************************************************************************/
/*                      Initialization of gateway                            */
/*****************************************************************************/

static int gateway_init(int argc, char **argv)
{
	int status, error, ip4count;
	struct in_addr *ip4ads;

#ifdef DETAILED_TIMING
	write_timing_count = 0;
	read_timing_count = 0;
#endif // DETAILED_TIMING

	INFO(1, "Initializing gateway");

	gateway_start_time = current_time_micros();

	// The server PID is all 1's
	server_pid = 0xFFFFFFFF;

	// Create an fd for polling.
	epollfd = epoll_create1(0);

	cluster_count = 0;
	cluster_rank = -1;

	error = pthread_mutex_init(&send_data_mutex, NULL);

	if (error != 0) {
		ERROR(1, "Failed to init mutex!");
		return ERROR_MUTEX;
	}

	error = pthread_mutex_init(&received_data_mutex, NULL);

	if (error != 0) {
		ERROR(1, "Failed to init mutex!");
		return ERROR_MUTEX;
	}

	// Read the cluster name and server location from a file.
	error = init_cluster_info(argc, argv);

	if (error != 0) {
		WARN(1, "Failed to initialize gateway!");
		return ERROR_ARGUMENT;
	}

	if (cluster_name == NULL || server_name == NULL || server_port <= 0 || server_port > 65535) {
		WARN(1, "Cluster info not correctly set (name=%s server=%d port=%d)!", cluster_name, server_name, server_port);
		return ERROR_ARGUMENT;
	}

	// Get the IP of the server.
	status = socket_get_ipv4_address(server_name, &server_ipv4);

	if (status != SOCKET_OK) {
		WARN(1, "Failed to retrieve server address!");
		return ERROR_ARGUMENT;
	}

	INFO(2, "I am gateway in cluster %s. Server at %s %d", cluster_name, server_name, server_port);

	// Connect to the server.
	status = socket_connect(server_ipv4, server_port, 0, 0, &serverfd);

	if (status != SOCKET_OK) {
		WARN(1, "Failed to connect to server!");
		return ERROR_SERVER;
	}

	server_info = create_socket_info(serverfd, TYPE_SERVER, STATE_IDLE);

	// Ask the server for information on the cluster count and number of gateways per cluster.
	status = handshake();

	if (status != 0) {
		close(serverfd);
		return status;
	}

	INFO(2, "Expecting %d application and %d gateway processes at this site.", local_application_size, gateway_count);

	if (gateway_count != 1) {
		WARN(1, "Only 1 gateway per site supported in this version!");
		return ERROR_ARGUMENT;
	}

	// Receive sizes of all clusters.
	cluster_sizes = malloc(cluster_count * sizeof(int));
	cluster_offsets = malloc((cluster_count+1) * sizeof(int));

	if (cluster_sizes == NULL || cluster_offsets == NULL) {
		WARN(1, "Failed to allocate space for cluster info!");
		close(serverfd);
		return ERROR_ALLOCATE;
	}

	status = receive_cluster_sizes();

	if (status != 0) {
		close(serverfd);
		return status;
	}

	// Retrieve the local IPv4 addresses.
	status = socket_get_local_ips(&ip4ads, &ip4count);

	if (status != SOCKET_OK) {
		ERROR(1, "Failed to retrieve local addresses! (error=%d)", status);
		return ERROR_CANNOT_FIND_IP;
	}

	INFO(2, "Sending my IP information to the server");

	// Send my IPv4 address to the server.
	status = send_gateway_info(0, ip4ads, ip4count);

	free(ip4ads);

	if (status != 0) {
		close(serverfd);
		return status;
	}

	// All gateway info from this cluster is now send to the server. We allocate space for gateway info for all clusters.
	// gateway_addresses = malloc(sizeof(gateway_address) * cluster_count * gateway_count);
	gateway_addresses = calloc(cluster_count * gateway_count, sizeof(gateway_address));

	if (gateway_addresses == NULL) {
		ERROR(1, "Failed to allocate space for gateway addresses!");
		return ERROR_ALLOCATE;
	}

	// memset(gateway_addresses, 0, sizeof(gateway_address) * cluster_count * gateway_count);

	// Next, Receives all global gateway info from the server.
	INFO(2, "Receiving IP information on all gateways from server");

	status = receive_gateway_info();

	if (status != 0) {
		close(serverfd);
		return status;
	}

	// Generate my PID. All ones, except for the cluster rank.
	my_pid = SET_PID(cluster_rank, 0xFFFFFF);

	INFO(2, "All IP information received -- creating message buffers");

	// Init the fragment buffers here.
	nodes_to_gateway_fragment_buffer = fragment_buffer_create(fragment_size, fragment_count);

	if (nodes_to_gateway_fragment_buffer == NULL) {
		FATAL("Failed to create nodes_to_gateway_fragment_buffer!");
	}

	gateway_to_nodes_fragment_buffer = fragment_buffer_create(fragment_size, fragment_count);

	if (gateway_to_nodes_fragment_buffer == NULL) {
		FATAL("Failed to create gateway_to_nodes_fragment_buffer!");
	}

	INFO(2, "Connecting to other gateways");

	// Now connect all gateway processes to each other. This assumes a direct connection is possible between each pair of gateways!
	status = connect_gateways();

	if (status != 0) {
		return status;
	}

	INFO(2, "All gateways connected -- accepting connections from local compute nodes");

	// Accept all connections from the local compute nodes.
	status = accept_local_connections();

	if (status != 0) {
		close(serverfd);
		return status;
	}

	INFO(2, "All local compute nodes connected -- performing -all clear barrier- with server");

	status = all_clear_barrier();

	if (status != 0) {
		close(serverfd);
		return status;
	}

	INFO(2, "All gateways and their compute nodes are ready -- sending the all clear local compute node 0");

	status = send_all_clear();

	if (status != 0) {
		close(serverfd);
		return status;
	}

	// Accept all connections from the local compute nodes.

	// We are done talking to the server for now. We add the serverfd to the epollfd, and perform any additional
	// communication with the server in an asynchronous fashion.

	INFO(2, "Adding server connection to epoll set");

	clear_socket_info_in_msg(server_info);
	clear_socket_info_out_msg(server_info);

	error = socket_set_non_blocking(server_info->socketfd);

	if (error != 0) {
		ERROR(1, "Failed to set server socket connection to non-blocking!");
		return error;
	}

	error = socket_add_to_epoll(epollfd, serverfd, server_info);

	if (error != 0) {
		ERROR(1, "Failed to add server socket connection to epoll set!");
		return error;
	}

	error = socket_set_ro(epollfd, server_info->socketfd, server_info);

	if (error != 0) {
		ERROR(1, "Failed to set server socket connection to RO!");
		return error;
	}

	INFO(2, "Gateway initialized!");

	return 0;
}

//static void handle_incoming_message(socket_info *info)
//{
//	// Read produced a message.
//	if (info->type == TYPE_SERVER) {
//		enqueue_message_from_server(info);
//	} else {
//		enqueue_message_from_compute_node(info);
//	}
//}

static void shutdown_compute_node(int index)
{
	socket_info *info;

	info = local_compute_nodes[index];

	close(info->socketfd);

	// TODO: cleanup socket_info and print statistics ?
}

static void shutdown_compute_node_connections()
{
	int i;

	for (i=0;i<local_application_size;i++) {
		shutdown_compute_node(i);
	}
}

static int shutdown_gateway_connection(int index)
{
	int i;
	gateway_connection *gw;
	void *result;

	if (index != cluster_rank) {

		gw = &gateway_connections[index];

		// Tell the sender threads to stop
		for (i=0;i<gw->stream_count;i++) {
			set_done_at_socket_info(gw->sockets[i]);
		}

		// Wait until the sender threads have stopped.
		for (i=0;i<gw->stream_count;i++) {
			pthread_join(gw->sockets[i]->send_thread, &result);
		}

		// Close the sockets. This will upset the receiver ...
		for (i=0;i<gw->stream_count;i++) {
			if (gw->sockets[i]->type == TYPE_GATEWAY_TCP) {
				// close the sending part of the socket, but leave the receiveing part open!
				shutdown(gw->sockets[i]->socketfd, SHUT_WR);
			} else if (gw->sockets[i]->type == TYPE_GATEWAY_UDT) {
				udt4_close(gw->sockets[i]->socketfd);
			} else {
				FATAL("INTERNAL ERROR: Unknown socket type in gateway connection %d.%d", index, i);
			}
		}

		// Wait until the receiver threads have stopped.
		for (i=0;i<gw->stream_count;i++) {
			pthread_join(gw->sockets[i]->receive_thread, &result);
		}

		// Close the sockets. This will upset the receiver ...
		for (i=0;i<gw->stream_count;i++) {
			if (gw->sockets[i]->type == TYPE_GATEWAY_TCP) {
				// close the entire socket
				close(gw->sockets[i]->socketfd);
			} else if (gw->sockets[i]->type == TYPE_GATEWAY_UDT) {
				// ignored
			} else {
				FATAL("INTERNAL ERROR: Unknown socket type in gateway connection %d.%d", index, i);
			}
		}
	}

	return 0;
}

static int shutdown_gateway_connections()
{
	int i, status;

	for (i=0;i<cluster_count;i++) {
		status = shutdown_gateway_connection(i);

		if (status != CONNECT_OK) {
			WARN(1, "Failed to disconnect to gateway %d (error=%d)", i, status);
		}
	}

	return 0;
}


static int process_messages()
{
	int i, error, count, ops;
	struct epoll_event *events;
	socket_info *info;
	uint32_t event;
	uint64_t now, last_print;
	bool keep_going = true;

	application_start_time = current_time_micros();
	last_print = application_start_time;

	// Allocate space for the events, one per compute node plus one for the server.
	events = malloc((local_application_size+1) * sizeof(struct epoll_event));

	if (events == NULL) {
		ERROR(1, "Failed to allocate space for epoll events!");
		return ERROR_ALLOCATE;
	}

	while (keep_going) {

		count = epoll_wait(epollfd, events, local_application_size+1, EPOLL_TIMEOUT);

		if (count < 0) {
			ERROR(1, "Failed to wait for socket event!");
			return ERROR_POLL;
		}

INFO(1, "GATEWAY epoll returned %d events", count);

		if (count > 0) {

			for (i=0;i<count;i++) {

				info = events[i].data.ptr;
				event = events[i].events;

				if ((event & (EPOLLERR | EPOLLHUP)) != 0) {
					// Error or hang up occurred in this socket!
					ERROR(1, "Unexpected error/hangup on connection to compute node %d", info->index);
					return ERROR_POLL;
				}

				if (event & EPOLLOUT) {

					// This socket has room to write some data.
					error = write_message(info, false);
//					ops = 1;
//
//					// Keep writing wile we have data and there is buffer space in TCP.
//					while (error == 0 && ops < WRITE_OPS_LIMIT) {
//						error = write_message(info, false);
//						ops++;
//					}

					if (error == -1) {
						ERROR(1, "Failed to handle write event on connection to compute node %d (error=%d)", i, error);
						return ERROR_CONNECTION;
					}
				}

				if (event & EPOLLIN) {
					// This socket has room to read some data.
//					error = 0;
//					ops = 0;
//
//					// Keep reading while there is more data to read.
//					while (error == 0 && ops < READ_OPS_LIMIT) {
					error = read_message(info, false);
//						ops++;

					if (error == 0) {
						if (info->type == TYPE_SERVER) {
							keep_going = enqueue_message_from_server(info);
						} else {
							enqueue_message_from_compute_node(info);
						}
					} else if (error == -1 || error == 2) {
						// Unexpected disconnect: this should not happen!
						ERROR(1, "Unexpected termination of link to compute node %d", info->index);
						return ERROR_CONNECTION;
					}
				}
			}
		}

		// Check if we need to print anything...
		now = current_time_micros();

		if (now >= last_print + 1000000) {
			print_gateway_statistics(now-application_start_time);
			last_print = now;
		}
	}

	// Disconnect all compute nodes
	shutdown_compute_node_connections();

	// Disconnect all gateways
	shutdown_gateway_connections();

	// We should now have 1 active connection left (to the server)
	close(epollfd);
	close(serverfd);

	return 0;
}


int main(int argc, char **argv)
{
	int error;

	init_logging();

	error = gateway_init(argc, argv);

	if (error != 0) {
		return error;
	}

	error = process_messages();

	if (error != 0) {
		return error;
	}

    return 0;
}


