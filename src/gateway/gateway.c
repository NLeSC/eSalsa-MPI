#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
//#include <arpa/inet.h>
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
#include "linked_queue.h"
#include "socket_util.h"
#include "udt_util.h"

// #include "blocking_linked_queue.h"


//#define DETAILED_MESSAGE_INFO 1

#define MAX_MPI_RECEIVE_POLL 10
#define MAX_MPI_POLL_MISS 5
#define MAX_MPI_MESSAGE_FORWARD 1

#define MAX_MESSAGE_SIZE (8*1024*1024)
#define MAX_PENDING_ISEND_DATA 128*1024*1024

#define MAX_BLOCKED_QUEUE_SIZE 128*1024*1024

// #define MIN_INTERESTING_BUFFER_SIZE (512*1024)

// #define MAX_SINGLE_MPI_RECEIVE    (2*1024*1024)
//#define MAX_SINGLE_SOCKET_RECEIVE (8*1024*1024)
//#define MAX_SINGLE_SOCKET_SEND    (8*1024*1024)

//#define MAX_WAITING_BUFFERS  (3)
//#define MAX_MPI_RECEIVED_DATA_SIZE (STANDARD_BUFFER_SIZE)

#define MAX_LENGTH_CLUSTER_NAME 128
#define MAX_STREAMS 16

//#define MAX_EVENTS 32
//#define MAX_MPI_RECEIVE_SEQUENCE 16
//#define MAX_SOCKETS_RECEIVE_SEQUENCE 1
//#define MAX_SOCKETS_READ_SEQUENCE 2
//#define MAX_PENDING_DATA_SIZE (1024L*1024L*1024L)
//#define MAX_PENDING_DATA_MSG  (128)

#define RECEIVE_BUFFER_SIZE (32*1024*1024)
#define SEND_BUFFER_SIZE (32*1024*1024)

//#define STATE_RW (1)
//#define STATE_RO (0)

#define TYPE_SERVER           (0)
#define TYPE_ACCEPT           (1)
#define TYPE_GATEWAY_TCP      (2)
#define TYPE_GATEWAY_UDT      (3)
#define TYPE_COMPUTE_NODE_TCP (4)

#define STATE_ACCEPTING          0
#define STATE_READING_HANDSHAKE  1
#define STATE_WRITING_HANDSHAKE  2
#define STATE_IDLE               3
#define STATE_READ_HEADER        4
#define STATE_READ_PAYLOAD       5
#define STATE_TERMINATING        6
#define STATE_TERMINATED         7

// Error codes used internally.
#define CONNECT_OK                      0
//#define CONNECT_ERROR_CREATE_SOCKET     1
//#define CONNECT_ERROR_CONNECT           2
//#define CONNECT_ERROR_OPTIONS           3
//#define CONNECT_ERROR_BIND              4
//#define CONNECT_ERROR_LISTEN            5
//#define CONNECT_ERROR_ACCEPT            6
//#define CONNECT_ERROR_HOST              7
//#define CONNECT_ERROR_HOST_NOT_FOUND    8
//#define CONNECT_ERROR_ADDRESS_TYPE      9
//#define CONNECT_ERROR_SEND_FAILED      10
//#define CONNECT_ERROR_RECEIVE_FAILED   11

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

//typedef struct {
//   uint32_t size;
//   uint32_t start;
//   uint32_t end;
//   uint32_t messages;
//   unsigned char data[];
//} message_buffer;
//
//static bounded_array_list *waiting_mpi_buffers;
//static data_buffer *mpi_receive_buffer = NULL;

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

	// Receiver thread (only used in gateway-to-gateway connections).
	pthread_t receive_thread;

	// Sender thread (only used in gateway-to-gateway connections).
	pthread_t send_thread;

	// Current message that is being written.
	message_buffer *out_msg;

	// Current message that is read.
	message_buffer *in_msg;

	// Queue for the messages that need to written to the socket.
	linked_queue *output_queue;

	// A mutex to make the queue thread safe.
	pthread_mutex_t output_mutex;

	// A condition variable that allows dequeue to block.
	pthread_cond_t output_cond;

	// Queue for the messages that have been received from the socket (only in gateway-to-gateway TCP connections).
	linked_queue *input_queue;

} socket_info;

// A type to store gateway information.
typedef struct {
	unsigned long  ipv4;
	unsigned short port;
	unsigned short streams;
} gateway_address;

typedef struct {
	gateway_address info;
	socket_info **sockets;
	int stream_count;
	int next_output_stream;
	int next_input_stream;
} gateway_connection;

/*
typedef struct s_mpi_message {
	struct s_mpi_message *next;
	struct s_mpi_message *prev;
	MPI_Request r;
	int length;
	generic_message *message;
} mpi_message;
*/

//static MPI_Request *isend_requests;
//static MPI_Status *isend_statusses;
//static int *isend_requests_ready;

//static linked_queue **pending_isend_messages;
//static linked_queue **pending_isend_server_messages;

//static generic_message **current_isend_messages;

static size_t pending_isend_data;
static size_t max_pending_isend_data;

//static bool enqueue_block_warning = true;

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

//static MPI_Comm mpi_comm_application_only;
//MPI_Comm mpi_comm_gateways_only;
//extern MPI_Comm mpi_comm_gateway_and_application;

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

//static blocking_linked_queue *incoming_server_queue;

// The current message buffer used to receive messages for the server.
// static message_buffer *server_mpi_receive_buffer = NULL;

// The queue of message buffers waiting to be send to the server.
// static linked_queue *server_queue_out;

// The queue of incoming server messages.
//static linked_queue *server_queue_in;

// Message queue for incoming messages. They are parked here until they can be forwarded.
//static linked_queue *incoming_queue;

// Queue of pending MPI_Isends.
//static mpi_message *mpi_messages_head;
//static mpi_message *mpi_messages_tail;

//static int mpi_messages_count;
//static int mpi_messages_count_max;

// Used for pending receive operations
//static int pending_receive = 0;
//static int pending_receive_count = 0;
//static MPI_Request pending_receive_request;
//static generic_message *pending_receive_buffer = NULL;

// The PID of this process. Not valid on a gateway process.
//extern uint32_t my_pid;

//static uint64_t pending_data_messages;
//static uint64_t pending_data_size;

// Timing offset.
uint64_t gateway_start_time;

static uint64_t wa_send_data = 0;
static uint64_t wa_send_count = 0;

static uint64_t wa_received_data = 0;
static uint64_t wa_received_count = 0;

static pthread_mutex_t send_data_mutex;
static pthread_mutex_t received_data_mutex;

static uint64_t mpi_send_data = 0;
static uint64_t mpi_send_count = 0;

static uint64_t mpi_received_data = 0;
static uint64_t mpi_received_count = 0;

static socket_info **local_compute_nodes;

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

//static bool enqueue_message(message_buffer *m)
//{
//	int cluster, index, rank;
//	message_header mh;
//
//	message_buffer_peek(m, (unsigned char *)&mh, MESSAGE_HEADER_SIZE);
//
//	if (mh.dst_pid == server_pid) {
//		// This message contains a request for the server!
//		INFO(1, "ENQ SERVER REQUEST opcode=%d src=%d:%d length=%d", mh.opcode, GET_CLUSTER_RANK(mh.src_pid), GET_PROCESS_RANK(mh.src_pid), mh.length);
//
//		// FIXME: enqueue and socket state should be single atomic action!!!! - else we get race conditions!
//		if (!blocking_linked_queue_enqueue(server_info->output_queue, m, 0, -1)) {
//			return false;
//		}
//
//		// FIXME: only set when needed!!
//		socket_set_rw(epollfd, server_info->socketfd, server_info);
//		return true;
//	}
//
//	if (mh.src_pid == server_pid) {
//		// This message contains a reply from the server!
//		if (mh.dst_pid == my_pid) {
//			INFO(1, "RECEIVED SERVER REPLY for this gateway opcode=%d dst=%d:%d length=%d", mh.opcode, GET_CLUSTER_RANK(mh.dst_pid), GET_PROCESS_RANK(mh.dst_pid), mh.length);
//			// FIXME: handle this case -- its usually a finalize request!
//
//			if (mh.opcode == OPCODE_FINALIZE_REPLY) {
//				server_info->state = STATE_TERMINATING;
//				message_buffer_destroy(m);
//			} else {
//				ERROR(1, "Received unexpected server reply at gateway! opcode=%d dst=%d:%d length=%d",
//						mh.opcode, GET_CLUSTER_RANK(mh.dst_pid), GET_PROCESS_RANK(mh.dst_pid), mh.length);
//			}
//
//			return true;
//		}
//
//		INFO(1, "ENQ SERVER REPLY opcode=%d dst=%d:%d length=%d", mh.opcode, GET_CLUSTER_RANK(mh.dst_pid), GET_PROCESS_RANK(mh.dst_pid), mh.length);
//
//		rank = GET_PROCESS_RANK(mh.dst_pid);
//
//		if (mh.opcode == OPCODE_FINALIZE_REPLY) {
//			// This link will be terminating soon!
//			local_compute_nodes[rank]->state = STATE_TERMINATING;
//		}
//
//		// FIXME: enqueue and socket state should be single atomic action!!!! - else we get race conditions!
//		if (!blocking_linked_queue_enqueue(local_compute_nodes[rank]->output_queue, m, 0, -1)) {
//			return false;
//		}
//
//		// FIXME: only set when needed!!
//		socket_set_rw(epollfd, local_compute_nodes[rank]->socketfd, local_compute_nodes[rank]);
//		return true;
//	}
//
//	// This message must be forwarded to another gateway.
//	INFO(1, "ENQ DATA MESSAGE opcode=%d src=%d:%d dst=%d:%d length=%d", mh.opcode,
//			GET_CLUSTER_RANK(mh.src_pid), GET_PROCESS_RANK(mh.src_pid),
//			GET_CLUSTER_RANK(mh.dst_pid), GET_PROCESS_RANK(mh.dst_pid),
//			mh.length);
//
//	cluster = GET_CLUSTER_RANK(mh.dst_pid);
//
//	if (cluster < 0 || cluster >= cluster_count) {
//		ERROR(1, "Cluster index out of bounds! (cluster=%d)", cluster);
//		return false;
//	}
//
//	index = gateway_connections[cluster].next_output_stream;
//
//	// FIXME: may block -- is this acceptable ??!!
//
//	// FIXME: insert message buffer, NOT message BUG BUG BUG!!!
//
//	if (index < 0 || index > gateway_connections[cluster].stream_count) {
//		ERROR(1, "Stream index out of bounds %d ~ %d-%d", index, 0, gateway_connections[cluster].stream_count);
//	}
//
//	if (!blocking_linked_queue_enqueue(gateway_connections[cluster].sockets[index]->output_queue, m, 0, -1)) {
//		return false;
//	}
//
//	gateway_connections[cluster].next_output_stream = (index + 1) % gateway_connections[cluster].stream_count;
//	return true;
//}

static bool enqueue_message_at_socket_info(socket_info *info, message_buffer *m)
{
	bool result;
	int error = SOCKET_OK;

//	fprintf(stderr, "##### Enqueue for socket_info %d", info->socketfd);

	// Lock the queue first.
	pthread_mutex_lock(&info->output_mutex);

//	fprintf(stderr, "###### Storing message in queue of length %d (socket_info %d)", linked_queue_length(info->output_queue), info->socketfd);

	result = linked_queue_enqueue(info->output_queue, m, message_buffer_max_read(m));

	if (linked_queue_length(info->output_queue) == 1) {

		// We've just inserted a message into an empty queue, so set the socket to the correct mode (if needed),
		// and wake up any waiting threads.
		if (info->type == TYPE_SERVER || info->type == TYPE_COMPUTE_NODE_TCP) {
			error = socket_set_rw(epollfd, info->socketfd, info);
		}

//		fprintf(stderr, "##### Sending bcast to wake up threads! (socket_info %d)", info->socketfd);

		pthread_cond_broadcast(&info->output_cond);
	}

	// Unlock the queue.
	pthread_mutex_unlock(&info->output_mutex);

//	fprintf(stderr, "##### Enqueue done for socket_info %d", info->socketfd);

	return (error == SOCKET_OK) && result;
}

static message_buffer *dequeue_message_from_socket_info(socket_info *info, int64_t timeout_usec)
{
	message_buffer *m;
	struct timespec alarm;
	struct timeval now;
	int error = SOCKET_OK;

//	fprintf(stderr, "##### Dequeue message from socket_info %d", info->socketfd);

	// Lock the queue first.
	pthread_mutex_lock(&(info->output_mutex));

//	fprintf(stderr, "##### Dequeue grabbed lock! (socket_info %d)", info->socketfd);

	m = linked_queue_dequeue(info->output_queue);

//	fprintf(stderr, "##### Dequeue got message %p (socket_info %d)", m, info->socketfd);

	while (m == NULL) {

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
	//			fprintf(stderr, "##### Dequeue got timeout!(socket_info %d)", info->socketfd);
				break;
			}

			if (error != 0) {
				WARN(1, "Failed to wait for message on output queue of socket %d", info->socketfd);
			}
		}

		m = linked_queue_dequeue(info->output_queue);
	}

	if (m == NULL && (info->type == TYPE_SERVER || info->type == TYPE_COMPUTE_NODE_TCP)) {
		// TODO: only set when needed, as this results in a system call!
		error = socket_set_ro(epollfd, info->socketfd, info);

		if (error != 0) {
			WARN(1, "Failed to wait for message on output queue of socket %d", info->socketfd);
		}
	}

	// Unlock the queue.
	pthread_mutex_unlock(&info->output_mutex);

//	fprintf(stderr, "##### Dequeue released lock! (socket_info %d)", info->socketfd);

	return m;
}

static bool enqueue_message_from_server(message_buffer *m)
{
	// Message from the server are destined for a local compute node or (rarely) for the gateway itself. Since the server link
	// is handled by a single epoll thread (together with all compute node links), only one of these calls is active at a time.
	int rank;
	message_header mh;

	message_buffer_peek(m, (unsigned char *)&mh, MESSAGE_HEADER_SIZE);

	if (mh.dst_pid == my_pid) {
		INFO(1, "RECEIVED SERVER REPLY for this gateway opcode=%d dst=%d:%d length=%d", mh.opcode, GET_CLUSTER_RANK(mh.dst_pid), GET_PROCESS_RANK(mh.dst_pid), mh.length);

		if (mh.opcode == OPCODE_FINALIZE_REPLY) {
			server_info->state = STATE_TERMINATING;
			message_buffer_destroy(m);
		} else {
			ERROR(1, "Received unexpected server reply at gateway! opcode=%d dst=%d:%d length=%d",
					mh.opcode, GET_CLUSTER_RANK(mh.dst_pid), GET_PROCESS_RANK(mh.dst_pid), mh.length);
		}

		return true;
	}

	INFO(1, "ENQ SERVER REPLY opcode=%d dst=%d:%d length=%d", mh.opcode, GET_CLUSTER_RANK(mh.dst_pid), GET_PROCESS_RANK(mh.dst_pid), mh.length);

	rank = GET_PROCESS_RANK(mh.dst_pid);

	if (mh.opcode == OPCODE_FINALIZE_REPLY) {
		// This link will be terminating soon!
		local_compute_nodes[rank]->state = STATE_TERMINATING;
	}

	return enqueue_message_at_socket_info(local_compute_nodes[rank], m);
}

static bool enqueue_message_from_compute_node(message_buffer *m)
{
	// Message from compute nodes are usually headed for the WA link, although they may also be destined for the server.
	// Since all compute node links are handled by a single epoll thread (together with the server link), only one of these calls
	// is active at a time.
	int cluster, index;
	message_header mh;

	message_buffer_peek(m, (unsigned char *)&mh, MESSAGE_HEADER_SIZE);

	if (mh.dst_pid == server_pid) {
		// This message contains a request for the server!
		INFO(1, "ENQ SERVER REQUEST opcode=%d src=%d:%d length=%d", mh.opcode, GET_CLUSTER_RANK(mh.src_pid), GET_PROCESS_RANK(mh.src_pid), mh.length);
		return enqueue_message_at_socket_info(server_info, m);
	}

	// This message must be forwarded to another gateway.
	INFO(1, "ENQ DATA MESSAGE TO WA opcode=%d src=%d:%d dst=%d:%d length=%d", mh.opcode,
			GET_CLUSTER_RANK(mh.src_pid), GET_PROCESS_RANK(mh.src_pid),
			GET_CLUSTER_RANK(mh.dst_pid), GET_PROCESS_RANK(mh.dst_pid),
			mh.length);

	cluster = GET_CLUSTER_RANK(mh.dst_pid);

	if (cluster < 0 || cluster >= cluster_count) {
		ERROR(1, "Cluster index out of bounds! (cluster=%d)", cluster);
		return false;
	}

	index = gateway_connections[cluster].next_output_stream;

//	if (index < 0 || index > gateway_connections[cluster].stream_count) {
//		ERROR(1, "Stream index out of bounds %d ~ %d-%d", index, 0, gateway_connections[cluster].stream_count);
//	}

	gateway_connections[cluster].next_output_stream = (index + 1) % gateway_connections[cluster].stream_count;

	return enqueue_message_at_socket_info(gateway_connections[cluster].sockets[index], m);
}

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

static int write_message(socket_info *info)
{
	// The socket is ready for writing, so write something!
	int error;

	// If there was no message in progress, we try to dequeue one.
	if (info->out_msg == NULL) {
		info->out_msg = dequeue_message_from_socket_info(info, 0);

		if (info->out_msg == NULL) {
			return 0;
		}
	}

	error = socket_send_mb(info->socketfd, info->out_msg, false);

	if (error != SOCKET_OK) {
		ERROR(1, "Failed to write message to compute node %d!", info->index);
		return ERROR_CONNECTION;
	}

	if (message_buffer_max_read(info->out_msg) == 0) {
		// Full message has been written, so destroy it and dequeue the next message.
		// If no message is available, the socket will be set to read only mode until more message arrive.
		message_buffer_destroy(info->out_msg);

		// TODO FIXME: Which is better ?
		// info->out_msg = dequeue_message_from_socket_info(info, 0);
		info->out_msg = NULL;
	}

	return 0;
}



static int read_message(socket_info *info, bool blocking, message_buffer **out, bool *disconnect)
{
	// The socket is ready for reading, so read something!
	int error;
	message_header mh;

	INFO(1, "XXX Receive of message from socket %d start=%d end=%d size=%d", info->socketfd, info->in_msg->start, info->in_msg->end,
			info->in_msg->size);

	error = socket_receive_mb(info->socketfd, info->in_msg, blocking);

	INFO(1, "XXX Receive of message from socket %d done! start=%d end=%d size=%d", info->socketfd, info->in_msg->start, info->in_msg->end,
			info->in_msg->size);

	if (error != SOCKET_OK) {

		if (error == SOCKET_DISCONNECT && info->state == STATE_TERMINATING) {
			// The connection was closed, as expected!
			*disconnect = true;
			return 0;
		}

		ERROR(1, "Failed to read message from compute node %d!", info->index);
		return ERROR_CONNECTION;
	}

	// NOTE: Assumption here is that the in_msg has exactly the size of a header or full message!!
	if (message_buffer_max_write(info->in_msg) == 0) {
		// Full message has been read, so see what we can do with it.

		if (info->state == STATE_READ_HEADER) {
			// Full header was read. Get the message size from this header, extend the buffer so the message will fit, read more.
			message_buffer_peek(info->in_msg, (unsigned char *)&mh, MESSAGE_HEADER_SIZE);

			if (mh.length == MESSAGE_HEADER_SIZE) {
				// Header only message, so we are done. Enqueue it.

				INFO(1, "XXX Enqueue SHORT message of size %d", info->in_msg->size);

				*out = info->in_msg;

				// enqueue_message(info->in_msg);
				// TODO: reuse old buffer?
				info->in_msg = message_buffer_create(MESSAGE_HEADER_SIZE);
			} else {
				// Message payload is still pending. Enlarge buffer and receive it.
				// TODO: replace buffer and reuse the header sized one ?

				INFO(1, "XXX Expanding message to size %d", mh.length);

				if (message_buffer_expand(info->in_msg, mh.length) != 0) {
					ERROR(1, "Failed to expand message buffer!");
					return ERROR_ALLOCATE;
				}
				info->state = STATE_READ_PAYLOAD;
			}
		} else { // info->state == STATE_READ_PAYLOAD
			// Full message was read, so queue it.

			INFO(1, "XXX Enqueue message of size %d", info->in_msg->size);

			*out = info->in_msg;
			// enqueue_message(info->in_msg);
			// TODO: reuse old buffer?

			info->in_msg = message_buffer_create(MESSAGE_HEADER_SIZE);
			info->state = STATE_READ_HEADER;
		}
	}

	*disconnect = false;
	return 0;
}







// Sender thread for sockets.
void* tcp_sender_thread(void *arg)
{
	socket_info *info;
	bool done;
	message_header *mh;
	int error;
	uint64_t last_write, now, data, count;

	info = (socket_info *) arg;
	done = false;

	now = last_write = current_time_micros();
	data = 0;
	count = 0;

// FIXME: BROKEN!!!

	// FIXME: how to stop ?
	while (!done) {
		// TODO: should use timed wait here
		mh = dequeue_message_from_socket_info(info, -1);

		if (mh != NULL) {

			error = socket_sendfully(info->socketfd, (unsigned char *)mh, mh->length);

			if (error != SOCKET_OK) {
				ERROR(1, "Failed to send message!");
				return NULL;
			}

			count++;
			data += mh->length;

			free(mh);

			now = current_time_micros();

			// write stats at most once per second.
			if (now > last_write + 1000000) {
				store_sender_thread_stats(info->index, data, count, now);
				last_write = now;
				count = 0;
				data = 0;
			}
		}
	}

	return NULL;
}

// Sender thread for sockets.
/*
void* udt_sender_thread(void *arg)
{
	socket_info *info;
	bool done;
	message_header *mh;
	int error;
	uint64_t last_write, now, data, count;
	UDT4_Statistics stats;

	info = (socket_info *) arg;
	done = false;

	now = last_write = current_time_micros();
	data = 0;
	count = 0;

	// FIXME: how to stop ?
	while (!done) {
		mh = blocking_linked_queue_dequeue(info->output_queue, -1);

		if (mh != NULL) {

			error = udt_sendfully(info->socketfd, (unsigned char *)mh, mh->length);

			if (error != SOCKET_OK) {
				ERROR(1, "Failed to send message!");
				return NULL;
			}

			count++;
			data += mh->length;

			free(mh);

			now = current_time_micros();

			// write stats at most once per second.
			if (now > last_write + 1000000) {
				store_sender_thread_stats(info->index, data, count, now);
				last_write = now;
				count = 0;
				data = 0;

				udt4_perfmon(info->socketfd, &stats, true);

				fprintf(stderr, "UDT4 SEND STATS estbw=%f sendrate=%f send=%ld sendloss=%d inflight=%d RTT=%f window=%d us=%f ACK=%d NACK=%d\n",
						stats.mbpsBandwidth, stats.mbpsSendRate, stats.pktSent, stats.pktSndLoss, stats.pktFlightSize,
						stats.msRTT, stats.pktCongestionWindow, stats.usPktSndPeriod, stats.pktRecvACK, stats.pktRecvNAK);


			}
		}
	}

	return NULL;
}
*/

void* udt_sender_thread(void *arg)
{
	size_t avail;
	socket_info *info;
	bool done;
	message_header *mh[1024];
	message_header *tmp;
	int error, msg, i;
	uint64_t last_write, now, data, count;
	UDT4_Statistics stats;

	message_buffer *m;
	unsigned char *p;

	info = (socket_info *) arg;
	done = false;

	now = last_write = current_time_micros();
	data = 0;
	count = 0;

	/*
	while (!done) {

		msg = blocking_linked_queue_bulk_dequeue(info->output_queue, (void **)mh, 1024, -1);

		for (i=0;i<msg;i++) {

			tmp = mh[i];

			if (mh != NULL) {

				error = udt_sendfully(info->socketfd, (unsigned char *)tmp, tmp->length);

				if (error != SOCKET_OK) {
					ERROR(1, "Failed to send message!");
					return NULL;
				}

				count++;
				data += tmp->length;

				free(tmp);
				mh[i] = NULL;

				now = current_time_micros();

				// write stats at most once per second.
				if (now > last_write + 1000000) {
					store_sender_thread_stats(info->index, data, count, now);
					last_write = now;
					count = 0;
					data = 0;

					udt4_perfmon(info->socketfd, &stats, true);

					fprintf(stderr, "UDT4 SEND STATS estbw=%f sendrate=%f send=%ld sendloss=%d inflight=%d RTT=%f window=%d us=%f ACK=%d NACK=%d\n",
							stats.mbpsBandwidth, stats.mbpsSendRate, stats.pktSent, stats.pktSndLoss, stats.pktFlightSize,
							stats.msRTT, stats.pktCongestionWindow, stats.usPktSndPeriod, stats.pktRecvACK, stats.pktRecvNAK);
				}
			}
		}
	}
*/

	//fprintf(stderr, "#### UDT Sender starting! socketfd=%d type=%d state=%d index=%d ", info->socketfd, info->type, info->state, info->index);

	while (!done) {
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

			message_buffer_destroy(m);

		} else {
//			fprintf(stderr, "#### UDT Sender got empty message .. will retry...");
		}
	}

	//INFO(1, "UDT Sender done!");

	return NULL;
}

/*

// Sender thread for socket connected to server.
void* server_sender_thread(void *arg)
{
	socket_info *info;
	bool done;
	int error;
	message_buffer *m;

	info = (socket_info *) arg;
	done = false;

	// FIXME: how to stop ?
	while (!done) {
		m = blocking_linked_queue_dequeue(info->output_queue, -1);

		if (m != NULL) {

			INFO(1, "Sending message to server of size %d", message_buffer_max_read(m));

			error = socket_send_mb(info->socketfd, m, true);

//			error = socket_sendfully(info->socketfd, (unsigned char *)mh, mh->length);
//
//			free(mh);

			message_buffer_destroy(m);

			if (error != SOCKET_OK) {
				ERROR(1, "Failed to send message!");
				return NULL;
			}
		}
	}

	return NULL;
}
*/


// Receiver thread for sockets.
void* tcp_receiver_thread(void *arg)
{
	socket_info *info;
	bool done;
	message_header mh;
	void *buffer;
	int error;
	uint64_t last_write, now, data, count;
	message_buffer *m;


	info = (socket_info *) arg;
	done = false;

	now = last_write = current_time_micros();
	data = 0;
	count = 0;

	// FIXME: how to stop ?
	while (!done) {

		error = socket_receivefully(info->socketfd, (unsigned char *)&mh, MESSAGE_HEADER_SIZE);

		if (error != SOCKET_OK) {
			ERROR(1, "Failed to receive header!");
			return NULL;
		}

		if (mh.length < 0 || mh.length > MAX_MESSAGE_SIZE) {
			ERROR(1, "Receive invalid message header length=%d!\n", mh.length);
			return NULL;
		}

		buffer = malloc(mh.length);

		if (buffer == NULL) {
			ERROR(1, "Failed to allocate space for message of length=%d!\n", mh.length);
			return NULL;
		}

		memcpy(buffer, &mh, MESSAGE_HEADER_SIZE);

		error = socket_receivefully(info->socketfd, buffer + MESSAGE_HEADER_SIZE, mh.length-MESSAGE_HEADER_SIZE);

		if (error != SOCKET_OK) {
			ERROR(1, "Failed to receive message payload!");
			return NULL;
		}

		m = message_buffer_wrap(buffer, mh.length);
		m->end = mh.length;

		// FIXME: WILL LEAK??
		enqueue_message_from_wa_link(m);

		data += mh.length;
		count++;
		now = current_time_micros();

		// write stats at most once per second.
		if (now > last_write + 1000000) {
			store_receiver_thread_stats(info->index, data, count, now);
			last_write = now;
			count = 0;
			data = 0;
		}
	}

	return NULL;
}

// Receiver thread for sockets.
void* udt_receiver_thread(void *arg)
{
	socket_info *info;
	bool done;
	message_header mh;
	void *buffer;
	int error;
	uint64_t last_write, now, data, count;
	UDT4_Statistics stats;

	message_buffer *m;

	info = (socket_info *) arg;
	done = false;

	now = last_write = current_time_micros();
	data = 0;
	count = 0;

	// FIXME: how to stop ?
	while (!done) {

		error = udt_receivefully(info->socketfd, (unsigned char *)&mh, MESSAGE_HEADER_SIZE);

		if (error != SOCKET_OK) {
			ERROR(1, "Failed to receive header!");
			return NULL;
		}

		if (mh.length < 0 || mh.length > MAX_MESSAGE_SIZE) {
			ERROR(1, "Receive invalid message header length=%d!\n", mh.length);
			return NULL;
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

		m = message_buffer_wrap(buffer, mh.length);
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

	return NULL;
}

/*

// Receiver thread for sockets.
void* server_receiver_thread(void *arg)
{
	int error;
	socket_info *info;
	bool done;
//	message_header mh;
//	void *buffer;

	info = (socket_info *) arg;
	done = false;

	// FIXME: how to stop ?
	while (!done) {

		error = read_message(info, true);

		if (error != SOCKET_OK) {
			ERROR(1, "Failed to message!");
			return NULL;
		}
	}

	return NULL;
}
*/


/*
static message_buffer *create_message_buffer()
{
   message_buffer *tmp = malloc(sizeof(message_buffer) + MAX_MESSAGE_SIZE);

   tmp->size = MAX_MESSAGE_SIZE;
   tmp->start = 0;
   tmp->end = 0;
   tmp->messages = 0;

   return tmp;
}

static  void release_message_buffer(message_buffer *mb)
{
	// TODO: cache these ?
	free(mb);
}
*/

//static mpi_message *create_mpi_message(generic_message *m)
//{
//	// FIXME: used dynamic array ?
//	mpi_message *tmp = malloc(sizeof(mpi_message));
//
//	if (tmp == NULL) {
//		return NULL;
//	}
//
//	tmp->next = NULL;
//	tmp->message = m;
//	return tmp;
//}

static socket_info *create_socket_info(int socketfd, int type, int state)
{
	int error;
	socket_info *info;

	info = (socket_info *) malloc(sizeof(socket_info));

	if (info == NULL) {
		ERROR(1, "Failed to allocate socket_info!");
		return NULL;
	}

	INFO(1, "Creating socket info socketfd=%d type=%d state=%d pointer=%p", socketfd, type, state, info);

	info->socketfd = socketfd;
	info->type = type;
	info->state = state;
	info->index = -1;

	info->in_msg = NULL;
	info->out_msg = NULL;

	info->input_queue = NULL;
	info->output_queue = linked_queue_create(-1);

	error = pthread_cond_init(&(info->output_cond), NULL);

	if (error != 0) {
		linked_queue_destroy(info->output_queue);
		free(info);
		return NULL;
	}

	error = pthread_mutex_init(&(info->output_mutex), NULL);

	if (error != 0) {
		pthread_cond_destroy(&(info->output_cond));
		linked_queue_destroy(info->output_queue);
		free(info);
		return NULL;
	}

	return info;
}

/*
static void init_socket_info(socket_info *info, int socketfd, int type, size_t max_out, size_t max_in)
{
	info->socketfd = socketfd;
	info->type = type;
	info->input_queue = blocking_linked_queue_create(-1, max_in);

	//
	//	//   info->state = STATE_RO;
	//
	//	info->in_bytes = 0;
	//	info->out_bytes = 0;
	//
	//	info->in_messages = 0;
	//	info->out_messages = 0;
	//
	//	info->socket_read_buffer = create_message_buffer();
	//	info->socket_write_buffer = create_message_buffer();
}
*/

static int init_socket_info_threads(socket_info *info)
{
	int error;
/*
	if (info->type == TYPE_SERVER) {
		error = pthread_create(&info->receive_thread, NULL, &server_receiver_thread, info);

		if (error != 0) {
			ERROR(1, "Failed to create server receive thread for socket %d!", info->socketfd);
			return ERROR_ALLOCATE;
		}

		error = pthread_create(&info->send_thread, NULL, &server_sender_thread, info);

		if (error != 0) {
			ERROR(1, "Failed to create server sender thread for socket %d!", info->socketfd);
			return ERROR_ALLOCATE;
		}

	} else
*/
	if (info->type == TYPE_GATEWAY_TCP) {
		error = pthread_create(&(info->receive_thread), NULL, &tcp_receiver_thread, info);

		if (error != 0) {
			ERROR(1, "Failed to create TCP receive thread for socket %d!", info->socketfd);
			return ERROR_ALLOCATE;
		}

		error = pthread_create(&(info->send_thread), NULL, &tcp_sender_thread, info);

		if (error != 0) {
			ERROR(1, "Failed to create TCP sender thread for socket %d!", info->socketfd);
			return ERROR_ALLOCATE;
		}

 	} else if (info->type == TYPE_GATEWAY_UDT) {
		error = pthread_create(&(info->receive_thread), NULL, &udt_receiver_thread, info);

		if (error != 0) {
			ERROR(1, "Failed to create UDT receive thread for socket %d!", info->socketfd);
			return ERROR_ALLOCATE;
		}

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
	int socketfd = gateway_connections[index].sockets[0]->socketfd;

	if (gateway_connections[index].sockets[0]->type == TYPE_GATEWAY_TCP) {
		status = socket_receivefully(socketfd, (unsigned char *) &opcode, 4);
	} else if (gateway_connections[index].sockets[0]->type == TYPE_GATEWAY_UDT) {
		status = udt_receivefully(socketfd, (unsigned char *) &opcode, 4);
	}

	if (status != SOCKET_OK) {
		ERROR(1, "Handshake with gateway failed! (%d)", status);
		return ERROR_HANDSHAKE_FAILED;
	}

	return 0;
}

static int send_gateway_ready_opcode(int index)
{
	int status;
	int opcode = OPCODE_GATEWAY_READY;
	int socketfd = gateway_connections[index].sockets[0]->socketfd;

	if (gateway_connections[index].sockets[0]->type == TYPE_GATEWAY_TCP) {
		status = socket_sendfully(socketfd, (unsigned char *) &opcode, 4);
	} else if (gateway_connections[index].sockets[0]->type == TYPE_GATEWAY_UDT) {
		status = udt_sendfully(socketfd, (unsigned char *) &opcode, 4);
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

	// FIXME: hardcoded data transfer type. Should be in server config!
	int type = TYPE_GATEWAY_UDT;

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

				if (type == TYPE_GATEWAY_TCP) {
					// Create a path to the target gateway.
					status = socket_connect(gateway_addresses[remoteIndex].ipv4, gateway_addresses[remoteIndex].port + s, -1, -1,
							&socket);
				} else if (type == TYPE_GATEWAY_UDT) {
					status = udt_connect(gateway_addresses[remoteIndex].ipv4, gateway_addresses[remoteIndex].port + s, -1, -1,
							&socket);
				}

				if (status != CONNECT_OK) {
					ERROR(1, "Failed to connect!");
					return status;
				}

				gateway_connections[i].sockets[s] = create_socket_info(socket, type, STATE_IDLE);
//				init_socket_info(&(gateway_connections[i].sockets[s]), socket, type, MAX_BLOCKED_QUEUE_SIZE,
//						MAX_BLOCKED_QUEUE_SIZE);

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

			if (type == TYPE_GATEWAY_TCP) {
				// Create a path to the target gateway.
				status = socket_accept_one(local_port + s, gateway_addresses[remoteIndex].ipv4, -1, -1, &socket);
			} else if (type == TYPE_GATEWAY_UDT) {
				status = udt_accept(local_port + s, gateway_addresses[remoteIndex].ipv4, -1, -1, &socket);
			}

			if (status != CONNECT_OK) {
				ERROR(1, "Failed to accept!");
				return status;
			}

			gateway_connections[crank].sockets[s] = create_socket_info(socket, type, STATE_IDLE);
//
//			init_socket_info(&(gateway_connections[crank].sockets[s]), socket, type, MAX_BLOCKED_QUEUE_SIZE,
//					MAX_BLOCKED_QUEUE_SIZE);

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

//static int set_gateway_to_nonblocking(int index)
//{
//   int i, status;
//
//   DEBUG(1, "Adding gateway %d to epoll", index);
//
//   // We do not need to add a connection to ourselves!
//   if (index == cluster_rank) {
//      return 0;
//   }
//
//   for (i=0;i<gateway_connections[index].stream_count;i++) {
//      status = set_socket_non_blocking(gateway_connections[index].sockets[i].socketfd);
//
//      if (status != EMPI_SUCCESS) {
//         ERROR(1, "Failed to set socket to non-blocking mode!");
//         return status;
//      }
//
////      status = add_socket_to_epoll(gateway_connections[index].sockets[i].socketfd, &(gateway_connections[index].sockets[i]));
//
////      if (status != EMPI_SUCCESS) {
////         ERROR(1, "Failed to add socket to epoll set!");
////         return status;
////      }
//   }
//
//   return 0;
//}

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

static int disconnect_gateway(int index)
{
	int s;

	// FIXME! HARD CODED
	int type = TYPE_GATEWAY_UDT;

	if (index != cluster_rank) {
		for (s=0;s<gateway_connections[index].stream_count;s++) {

			if (type == TYPE_GATEWAY_TCP) {
				close(gateway_connections[index].sockets[s]->socketfd);
			} else { // if (type == TYPE_GATEWAY_UDT)
				udt4_close(gateway_connections[index].sockets[s]->socketfd);
			}
		}
	}

	return 0;
}

static int disconnect_gateways()
{
	int i, status;

	for (i=0;i<cluster_count;i++) {
		status = disconnect_gateway(i);

		if (status != CONNECT_OK) {
			WARN(1, "Failed to disconnect to gateway %d (error=%d)", i, status);
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

/***
static int nonblock_write_message(message_buffer *buffer, int socketfd, bool *wouldblock, int *written_data) {

	ssize_t avail, tmp;

	avail = buffer->end - buffer->start;

	while (avail > 0) {

		tmp = write(socketfd, &(buffer->data[buffer->start]), avail);

		if (tmp == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				// Cannot write any more data. Socket is full.
				*wouldblock = true;
				return EMPI_SUCCESS;
			} else {
				ERROR(1, "Failed to write message to socket! %d %d %d %d (error=%d)", socketfd, buffer->start, buffer->end, avail, errno);
				return EMPI_ERR_INTERN;
			}
		}

		buffer->start += tmp;
		avail -= tmp;
		written_data += tmp;
	}

	// We ran out of data to write, so return!
	return EMPI_SUCCESS;
}

/*
static int write_socket_messages_for_cluster(int cluster, int *wouldblock, int *nodata)
{
   // FIXME: assumes single stream!
   int avail, my_wouldblock, written_data, error;
   message_buffer *tmp;
   socket_info *info;

   // FIXME: HACK!
   if (cluster < 0) {
      info = server_info;
   } else {
      info = &(gateway_connection[cluster].sockets[0]);
   }

   my_wouldblock = 0;
   my_nodata = 0;
   written_data = 0;

   error = nonblock_write_message(info->socket_write_buffer, info->socket_fd, &my_wouldblock, &written_data);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to write message to socket! (error=%d)", errno);
      return error;
   }

   if (my_wouldblock == 1) {
 *wouldblock++;
      return EMPI_SUCCESS;
   }

   // Socket ran out of data to write. Reset buffer.
   reset_message_buffer(info->socket_write_buffer);

   // Next, attempt to swap buffers.

   // FIXME: HACK!
   if (cluster < 0) {
      tmp = server_mpi_receive_buffer;
   } else {
      tmp = gateway_connection[cluster].mpi_receive_buffer;
   }

   if (tmp->end > 0) {
      // There is data available it the mpi receive buffer!
      // FIXME: HACK!
      if (cluster < 0) {
         server_mpi_receive_buffer = info->socket_write_buffer;
      } else {
         gateway_connection[cluster].mpi_receive_buffer = info->socket_write_buffer;
      }

      info->socket_write_buffer = tmp;

      error = nonblock_write_message(info->socket_write_buffer, info->socket_fd, &my_wouldblock, &written_data);

      if (error != EMPI_SUCCESS) {
         ERROR(1, "Failed to write message to socket! (error=%d)", errno);
         return error;
      }

      if (my_wouldblock == 1) {
 *wouldblock++;
         return EMPI_SUCCESS;
      }
   }

 *nodata++;
   return EMPI_SUCCESS;
}


// FIXME: should split this!!! Extract buffer and socket like in read, and lift swap to higher level!
/*
static int write_socket_messages_for_cluster(int cluster, int *wouldblock, int *nodata)
{
   // FIXME: assumes single stream!
   int avail, my_wouldblock, written_data, error;
   message_buffer *tmp;
   socket_info *info;

   // FIXME: HACK!
   if (cluster < 0) {
      info = server_info;
   } else {
      info = &(gateway_connection[cluster].sockets[0]);
   }

   my_wouldblock = 0;
   my_nodata = 0;
   written_data = 0;

   error = nonblock_write_message(info->socket_write_buffer, info->socket_fd, &my_wouldblock, &written_data);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to write message to socket! (error=%d)", errno);
      return error;
   }

   if (my_wouldblock == 1) {
 *wouldblock++;
      return EMPI_SUCCESS;
   }

   // Socket ran out of data to write. Reset buffer.
   reset_message_buffer(info->socket_write_buffer);

   // Next, attempt to swap buffers.

   // FIXME: HACK!
   if (cluster < 0) {
      tmp = server_mpi_receive_buffer;
   } else {
      tmp = gateway_connection[cluster].mpi_receive_buffer;
   }

   if (tmp->end > 0) {
      // There is data available it the mpi receive buffer!
      // FIXME: HACK!
      if (cluster < 0) {
         server_mpi_receive_buffer = info->socket_write_buffer;
      } else {
         gateway_connection[cluster].mpi_receive_buffer = info->socket_write_buffer;
      }

      info->socket_write_buffer = tmp;

      error = nonblock_write_message(info->socket_write_buffer, info->socket_fd, &my_wouldblock, &written_data);

      if (error != EMPI_SUCCESS) {
         ERROR(1, "Failed to write message to socket! (error=%d)", errno);
         return error;
      }

      if (my_wouldblock == 1) {
 *wouldblock++;
         return EMPI_SUCCESS;
      }
   }

 *nodata++;
   return EMPI_SUCCESS;
}
 */
/*
static inline void reset_message_buffer(message_buffer *buffer)
{
	buffer->start = buffer->end = 0;
}

/*
static int write_socket_messages()
{
	int idle, error, written_data, i;
	bool wouldblock;
	message_buffer *tmp;
	socket_info *info;

	written_data = 0;

	do {
		idle = 0;

		for (i=0;i<cluster_count;i++) {
			if (i != cluster_rank) {

				info = &(gateway_connections[i].sockets[0]);

				wouldblock = false;

				error = nonblock_write_message(info->socket_write_buffer, info->socketfd, &wouldblock, &written_data);

				if (error != EMPI_SUCCESS) {
					ERROR(1, "Failed to write message to socket! (error=%d)", errno);
					return error;
				}

				if (wouldblock) {
					// nonblock_write_message returned because the socket was full.
					idle++;
				} else {
					// nonblock_write_message returned because it ran out of data to send.
					reset_message_buffer(info->socket_write_buffer);

					// Check if there is data available in the gateway mpi receive buffer
					if (gateway_connections[i].mpi_receive_buffer->end > 0) {
						// There is, so swap buffers.
						tmp = info->socket_write_buffer;
						info->socket_write_buffer = gateway_connections[i].mpi_receive_buffer;
						gateway_connections[i].mpi_receive_buffer = tmp;
					} else {
						// There isn't, so add one to the idle count.
						idle++;
					}
				}
			}
		}

	} while (idle < (cluster_count-1) && written_data < MAX_SINGLE_SOCKET_SEND);

	return EMPI_SUCCESS;
}

/*
static int nonblock_read_message(message_buffer *buffer, int socketfd, bool *wouldblock, int *read_data)
{
	size_t avail, tmp;

	avail = buffer->size - buffer->end;

	while (avail > 0) {

		tmp = read(socketfd, &(buffer->data[buffer->end]), avail);

		if (tmp == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				// Cannot read any more data. Socket is empty

				*wouldblock = true;
				return EMPI_SUCCESS;
			} else {
				ERROR(1, "Failed to read message from socket %d %d %d %d ! (error=%d)", socketfd, buffer->end, buffer->size, avail, errno);
				return EMPI_ERR_INTERN;
			}
		} else if (tmp == 0) {
			ERROR(1, "Unexpected EOF on socket %d! avail=%ld", socketfd, avail);
			return EMPI_ERR_INTERN;
		}

		buffer->end += tmp;
		avail -= tmp;
		*read_data += tmp;
	}

	return EMPI_SUCCESS;
}

/*
static int read_socket_messages_for_cluster(int cluster, int *wouldblock, int *nospace)
{
   // FIXME: assumes single stream!
   int my_wouldblock, read_data, error;

   socket_info *info = &(gateway_connection[cluster].sockets[0]);

   my_wouldblock = 0;
   read_data = 0;

   error = nonblock_read_message(info->socket_read_buffer, info->socket_fd, &my_wouldblock, &read_data);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to read message from socket! (error=%d)", errno);
      return error;
   }

   if (my_wouldblock == 1) {
 *wouldblock++;
      return EMPI_SUCCESS;
   }

   // Attempt to flush buffer, and retry read_message
   error = message_buffer_flush(info->socket_read_buffer);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to flush socket read message buffer! (error=%d)", errno);
      return error;
   }

 *nospace++;
   return EMPI_SUCCESS;
}
***/

/*
static int process_gateway_message(generic_message *m, bool *done)
{
	// NOTE: server messages are external and therefore in network byte order!
	int opcode = m->header.opcode;

	DEBUG(1, "Received gateway message with opcode %d", opcode);

	free(m);

	if (opcode != OPCODE_FINALIZE_REPLY) {
		ERROR(1, "Failed to process gateway message with unknown opcode %d!", opcode);
		return 1;
	}

	INFO(1, "Gateway receive FINALIZE message from server!");

	*done = true;

	return 0;
}
*/

/***
static int forward_mpi_message_with_isend(int rank)
{
	int error, tag, length;
	generic_message *tmp;

	// First check if there are any server messages
	tmp = linked_queue_dequeue(pending_isend_server_messages[rank]);

	if (tmp != NULL) {
		tag = TAG_SERVER_REPLY;
		length = ntohl(tmp->header.length);
	} else {
		// Next, check for data messages
		tag = TAG_FORWARDED_DATA_MSG;
		tmp = linked_queue_dequeue(pending_isend_messages[rank]);

		if (tmp != NULL) {
			length = tmp->header.length;
			mpi_send_data += length;
			mpi_send_count++;
		}
	}

	if (tmp == NULL) {
		// queue is empty.
		return EMPI_SUCCESS;
	}

	current_isend_messages[rank] = tmp;

	error = PMPI_Isend(tmp, length, MPI_BYTE, rank, tag, mpi_comm_gateway_and_application, &isend_requests[rank]);

	if (error != MPI_SUCCESS) {
		ERROR(1, "Failed to perform Isend to %d! (error=%d)", rank, error);
		return TRANSLATE_ERROR(error);
	}

	pending_isend_data -= length;
	return EMPI_SUCCESS;
}

static int forward_mpi_server_message(generic_message *m)
{
	int cluster, rank;

	cluster = GET_CLUSTER_RANK(ntohl(m->header.dst_pid));
	rank = GET_PROCESS_RANK(ntohl(m->header.dst_pid));

	DEBUG(1, "Forwarding message to MPI %d:%d tag=%d len-%d", cluster, rank, m->header.length);

	if (cluster != cluster_rank) {
		ERROR(1, "Received message for wrong cluster! dst=%d me=%d!", cluster, cluster_rank);
		return EMPI_ERR_INTERN;
	}

	// Queue the message at the pensing message queue for the destination.
	if (!linked_queue_enqueue(pending_isend_server_messages[rank], m, ntohl(m->header.length))) {
		ERROR(1, "Failed to enqueue message for isend to %d", rank);
		return EMPI_ERR_INTERN;
	}

	pending_isend_data += ntohl(m->header.length);

	// If there is already a pending isend, there is nothing else we can do.
	if (isend_requests[rank] != MPI_REQUEST_NULL) {
		//fprintf(stderr, " isend_requests[%d] = ACTIVE\n", rank);
		return EMPI_SUCCESS;
	}

	//fprintf(stderr, " isend_requests[%d] = MPI_REQUEST_NULL\n", rank);

	return forward_mpi_message_with_isend(rank);

}

static int forward_mpi_data_message(generic_message *m)
{
	int cluster, rank;

	cluster = GET_CLUSTER_RANK(m->header.dst_pid);
	rank = GET_PROCESS_RANK(m->header.dst_pid);

	DEBUG(1, "Forwarding message to MPI %d:%d tag=%d len-%d", cluster, rank, m->header.length);

	if (cluster != cluster_rank) {
		ERROR(1, "Received message for wrong cluster! dst=%d me=%d!", cluster, cluster_rank);
		return EMPI_ERR_INTERN;
	}

	// Queue the message at the pending message queue for the destination.
	if (!linked_queue_enqueue(pending_isend_messages[rank], m, m->header.length)) {
		ERROR(1, "Failed to enqueue message for isend to %d", rank);
		return EMPI_ERR_INTERN;
	}

	pending_isend_data += m->header.length;

	//fprintf(stderr, "pending_isend_data = %ld\n", pending_isend_data);

	// If there is already a pending isend, there is nothing else we can do.
	if (isend_requests[rank] != MPI_REQUEST_NULL) {
		//fprintf(stderr, " isend_requests[%d] = ACTIVE\n", rank);
		return EMPI_SUCCESS;
	}

	//fprintf(stderr, " isend_requests[%d] = MPI_REQUEST_NULL\n", rank);

	return forward_mpi_message_with_isend(rank);
}

static int poll_mpi_requests()
{
	int error, outcount, i, rank;
	generic_message *m;

	error = PMPI_Testsome(local_application_size, isend_requests, &outcount, isend_requests_ready, isend_statusses);

	if (error != MPI_SUCCESS) {
		ERROR(1, "Failed to test statusses of MPI_Isend! (error=%d)", error);
		return TRANSLATE_ERROR(error);
	}

	if (outcount == 0 || outcount == MPI_UNDEFINED) {
		// None of the isend requests need attention.
		return EMPI_SUCCESS;
	}

	for (i=0;i<outcount;i++) {

		rank = isend_requests_ready[i];

//		if (isend_statusses[rank].MPI_ERROR != MPI_SUCCESS) {
//			// should not happen ??
//			ERROR(1, "Isend to %d failed! (error=%d)", rank, isend_statusses[rank].MPI_ERROR);
//			return TRANSLATE_ERROR(isend_statusses[rank].MPI_ERROR);
//		}

		m = current_isend_messages[rank];
		free(m);

		error = forward_mpi_message_with_isend(rank);

		if (error != EMPI_SUCCESS) {
			return error;
		}
	}

	return EMPI_SUCCESS;
}
***/

/*
static void enqueue_mpi_message(mpi_message *m)
{
	pending_isend_data += m->length;

	if (mpi_messages_count == 0) {
		m->next = m->prev = NULL;
		mpi_messages_head = mpi_messages_tail = m;
		mpi_messages_count = 1;
		return;
	}

	m->next = NULL;
	m->prev = mpi_messages_tail;
	mpi_messages_tail->next = m;
	mpi_messages_tail = m;
	mpi_messages_count++;

	if (mpi_messages_count > mpi_messages_count_max) {
		mpi_messages_count_max = mpi_messages_count;
	}
}

static int forward_mpi_message_with_isend(generic_message *m, int length, int rank, int tag)
{
	int error;
	mpi_message *tmp;

	tmp = malloc(sizeof(mpi_message));
	tmp->message = m;
	tmp->length = length;

	error = PMPI_Isend(m, length, MPI_BYTE, rank, tag, mpi_comm_gateway_and_application, &(tmp->r));

	if (error != MPI_SUCCESS) {
		ERROR(1, "Failed to perform Isend to %d! (error=%d)", rank, error);
		return TRANSLATE_ERROR(error);
	}

	enqueue_mpi_message(tmp);
	return EMPI_SUCCESS;
}

static int forward_mpi_server_message(generic_message *m)
{
	int cluster, rank, length, tag;

	cluster = GET_CLUSTER_RANK(ntohl(m->header.dst_pid));
	rank = GET_PROCESS_RANK(ntohl(m->header.dst_pid));

	DEBUG(1, "Forwarding message to MPI %d:%d tag=%d len-%d", cluster, rank, m->header.length);

	if (cluster != cluster_rank) {
		ERROR(1, "Received message for wrong cluster! dst=%d me=%d!", cluster, cluster_rank);
		return EMPI_ERR_INTERN;
	}

	length = ntohl(m->header.length);
	tag = TAG_SERVER_REPLY;

	return forward_mpi_message_with_isend(m, length, rank, tag);
}

static int forward_mpi_data_message(generic_message *m)
{
	int cluster, rank, length, tag;

	cluster = GET_CLUSTER_RANK(m->header.dst_pid);
	rank = GET_PROCESS_RANK(m->header.dst_pid);

	DEBUG(1, "Forwarding message to MPI %d:%d tag=%d len-%d", cluster, rank, m->header.length);

	if (cluster != cluster_rank) {
		ERROR(1, "Received message for wrong cluster! dst=%d me=%d!", cluster, cluster_rank);
		return EMPI_ERR_INTERN;
	}

	tag = TAG_FORWARDED_DATA_MSG;
	length = m->header.length;

	mpi_send_data += length;
	mpi_send_count++;

	return forward_mpi_message_with_isend(m, length, rank, tag);
}

static int poll_mpi_requests()
{
	int error, flag;
	mpi_message *tmp, *curr;
	MPI_Status status;

	curr = mpi_messages_head;

	while (curr != NULL) {

		flag = 0;

		error = PMPI_Test(&(curr->r), &flag, &status);

		if (error != MPI_SUCCESS) {
			ERROR(1, "Failed to test status of MPI_Isend! (error=%d)", error);
			return TRANSLATE_ERROR(error);
		}

		if (flag) {
			// Operation has finished, so remove element
			pending_isend_data -= curr->length;
			tmp = curr;
			mpi_messages_count--;

			if (mpi_messages_count == 0) {
				// We are removing the only element
				mpi_messages_head = mpi_messages_tail = NULL;
				curr = NULL;
			} else if (curr->prev == NULL) {
				// We are removing the head
				mpi_messages_head = curr->next;
				mpi_messages_head->prev = NULL;
				curr = curr->next;
			} else if (curr->next == NULL) {
				// We are removing the tail
				mpi_messages_tail = curr->prev;
				mpi_messages_tail->next = NULL;
				curr = NULL;
			} else {
				// We are removing some element in the middle.
				curr->prev->next = curr->next;
				curr->next->prev = curr->prev;
				curr = curr->next;
			}

			free(tmp->message);
			free(tmp);
		} else {
			curr = curr->next;
		}
	}

	return EMPI_SUCCESS;
}
*/


/***
static int process_socket_buffer(socket_info *info, int *done)
{
	message_buffer *buffer;
	generic_message *m; //, *c;
	int avail, len, pid, error;
	bool stop = false;

	buffer = info->socket_read_buffer;

	avail = buffer->end - buffer->start;

	//fprintf(stderr, "PROCESSING SOCKET BUFFER of socket %d %d %d %d %d\n", info->socketfd, buffer->start, buffer->end, buffer->size, avail);

	while (!stop) {

		len = sizeof(generic_message);

		if (avail < len) {
			// There's no full message header in the buffer yet.
			//fprintf(stderr, "No complete header in buffer %d\n", avail);
			stop = true;
			break;
		}

		// These is at least a message header in the buffer.
		m = (generic_message *) &(buffer->data[buffer->start]);

		if (info->type == TYPE_SERVER) {
			len = ntohl(m->header.length);
			pid = ntohl(m->header.dst_pid);
		} else {
			len = m->header.length;
			pid = m->header.dst_pid;
		}

		DEBUG(2, "Got message of len %d at offset %d", len, buffer->start);

		if (avail < len) {
			// There's no full message in the buffer yet.
			//fprintf(stderr, "No complete message in buffer %d\n", avail);
			stop = true;
			break;
		}

		// We have a full message, so copy it and queue to copy for sending with MPI later.
		// EEP EEP EEP!!!!

		//fprintf(stderr, "Copying and queueing message of len %d\n", len);

		//    Replaced by immediate send!
		//    c = malloc(len);
		//    memcpy(c, m, len);
		//    linked_queue_enqueue(queue, c);

		if (info->type == TYPE_SERVER) {

			if (pid == my_pid) {
				// This message is indented for me!
				error = process_gateway_message(m, done);

				if (*done == 1) {
					stop = true;
					break;
				}
			} else {
				error = forward_mpi_message(m, pid, len, TAG_SERVER_REPLY);
			}

			if (error != MPI_SUCCESS) {
				ERROR(1, "Failed to process gateway message! (error=%d)", error);
				return error;
			}
		} else {
			error = forward_mpi_message(m, pid, len, TAG_FORWARDED_DATA_MSG);

			if (error != MPI_SUCCESS) {
				ERROR(1, "Failed to forward message to MPI! (error=%d)", error);
				return error;
			}
		}

		// Update some statistics.
		info->in_bytes += len;
		info->in_messages++;

		// Update the buffer start position and available data.
		buffer->start += len;
		avail -= len;
	}

	if (avail == 0) {
		// buffer is empty
		//fprintf(stderr, "Finished extraction BUFFER IS EMPTY\n");
		reset_message_buffer(buffer);
		return EMPI_SUCCESS;
	}

	///fprintf(stderr, "Finished extraction BUFFER HAS REMAINING DATA %d %d\n", buffer->start, avail);

	// There is some data left in the buffer, but not enough for a complete
	// message. We must check if the complete message will fit into the rest
	// of the buffer.

	if (buffer->start + len >= buffer->size) {

		//fprintf(stderr, "Moving data in buffer 0 %d %d %d\n", buffer->start, buffer->end, buffer->size);

		// It does not fit, so we need to copy the existing data to the start of the buffer.
		if (avail < buffer->start) {
			// No overlapping copy, so memcpy is safe.
			memcpy(&(buffer->data[0]), &(buffer->data[buffer->start]), avail);
		} else {
			// Overlapping copy, so must use memmove.
			memmove(&(buffer->data[0]), &(buffer->data[buffer->start]), avail);
		}

		buffer->start = 0;
		buffer->end = avail;
	} else {
		//the rest of the message fits, so no copy and just receive in the remaining buffer space.
		//fprintf(stderr, "NOT moving data in buffer 0 %d %d %d\n", buffer->start, buffer->end, buffer->size);
	}

	return EMPI_SUCCESS;
}
*/
/*
static int read_socket_messages()
{
	int idle, error, read_data, i, done;
	bool wouldblock;
	socket_info *info;

	read_data = 0;

	do {
		idle = 0;

		for (i=0;i<cluster_count;i++) {
			if (i != cluster_rank) {

				info = &(gateway_connections[i].sockets[0]);

				wouldblock = false;

				error = nonblock_read_message(info->socket_read_buffer, info->socketfd, &wouldblock, &read_data);

				if (error != EMPI_SUCCESS) {
					ERROR(1, "Failed to read message from socket! (error=%d)", errno);
					return error;
				}

				if (wouldblock) {
					idle++;
				}

				error = process_socket_buffer(info, &done);

				if (error != EMPI_SUCCESS) {
					ERROR(1, "Failed to flush socket read message buffer! (error=%d)", errno);
					return error;
				}
			}
		}

	} while (read_data < MAX_SINGLE_SOCKET_RECEIVE && idle < (cluster_count-1));

	return EMPI_SUCCESS;
}
*/
/*
static int write_socket_server_messages()
{
	message_buffer *tmp;
	int error, written_data;
	bool wouldblock;

	written_data = 0;
	wouldblock = false;

	error = nonblock_write_message(server_info.socket_write_buffer, server_info.socketfd, &wouldblock, &written_data);

	if (error != EMPI_SUCCESS) {
		ERROR(1, "Failed to write message to socket! (error=%d)", errno);
		return error;
	}

	if (wouldblock) {
		// nonblock_write_message returned because the socket was full.
		return EMPI_SUCCESS;
	}

	// nonblock_write_message returned because it ran out of data to send.
	reset_message_buffer(server_info.socket_write_buffer);

	// Check if there is data available in the gateway mpi receive buffer
	if (server_mpi_receive_buffer->end > 0) {
		// There is, so swap buffers.
		tmp = server_info.socket_write_buffer;
		server_info.socket_write_buffer = server_mpi_receive_buffer;
		server_mpi_receive_buffer = tmp;
	}

	return EMPI_SUCCESS;
}
*/
/*
static int read_socket_server_messages(int *done)
{
	int error, read_data;
	bool wouldblock;

	read_data = 0;
	wouldblock = false;

	error = nonblock_read_message(server_info.socket_read_buffer, server_info.socketfd, &wouldblock, &read_data);

	if (error != EMPI_SUCCESS) {
		ERROR(1, "Failed to read message from socket! (error=%d)", errno);
		return error;
	}

	return process_socket_buffer(&server_info, done);
}
*/
/*
static int process_socket_messages(int *done)
{
	int error;

	//   fprintf(stderr, "write socket messages\n");

	error = write_socket_messages();

	if (error != EMPI_SUCCESS) {
		ERROR(1, "Failed to write socket messages! (error=%d)", errno);
		return error;
	}

	//   fprintf(stderr, "read socket messages\n");

	error = read_socket_messages();

	if (error != EMPI_SUCCESS) {
		ERROR(1, "Failed to read socket messages! (error=%d)", errno);
		return error;
	}

	// The master gateway must also handle server communication
	if (gateway_rank == 0) {

		error = write_socket_server_messages();

		if (error != EMPI_SUCCESS) {
			ERROR(1, "Failed to write socket server messages! (error=%d)", errno);
			return error;
		}

		error = read_socket_server_messages(done);

		if (error != EMPI_SUCCESS) {
			ERROR(1, "Failed to read socket server messages! (error=%d)", errno);
			return error;
		}
	}

	return EMPI_SUCCESS;
}
*/

/*
static int receive_message(int count, MPI_Status *status, message_buffer *buffer)
{
#ifdef DETAILED_MESSAGE_INFO
	message_header *m;
	uint64_t time;
#endif

	// SANITY CHECKS!
	//   if (buffer == NULL) {
	//      ERROR(1, "Receive got invalid buffer (NULL)!");
	//   }

	//   if (buffer->size != STANDARD_BUFFER_SIZE ||
	//       buffer->start < 0 || buffer->start > STANDARD_BUFFER_SIZE || buffer->start > buffer->end ||
	//       buffer->end < 0 || buffer->end >= STANDARD_BUFFER_SIZE) {
	//      ERROR(1, "Receive got invalid buffer (size=%d, start=%d end=%d messages=%d)!", buffer->size, buffer->start, buffer->end, buffer->messages);
	//   }

	// NOTE: Blocking receive should NOT block, as the probe already told us a message is waiting -- IS THIS TRUE???
	int error = PMPI_Recv(&(buffer->data[buffer->end]), count, MPI_BYTE, status->MPI_SOURCE, status->MPI_TAG, mpi_comm_gateway_and_application, MPI_STATUS_IGNORE);

	if (error != MPI_SUCCESS) {
		ERROR(1, "Failed to receive MPI server message! source=%d tag=%d count=%d (error=%d)", status->MPI_SOURCE, status->MPI_TAG, count, error);
		return TRANSLATE_ERROR(error);
	}

#ifdef DETAILED_MESSAGE_INFO

	time = current_time_micros() - gateway_start_time;

	m = (message_header *) &(buffer->data[buffer->end]);

	fprintf(stderr, "DETAILED_MESSAGE_INFO MPI_MESSAGE_RECEIVED FROM %d %d TO %d %d TAG %d SIZE %d PAYLOAD %ld TIMESTAMP %ld\n",
			GET_CLUSTER_RANK(m->src_pid), GET_PROCESS_RANK(m->src_pid),
			GET_CLUSTER_RANK(m->dst_pid), GET_PROCESS_RANK(m->dst_pid),
			status->MPI_TAG, count, (count - sizeof(data_message)), time);
#endif

	buffer->end += count;
	buffer->messages += 1;

	// FIXME: sanity check!!!
	if (buffer->end > buffer->size) {
		ERROR(1, "Receivebuffer overflow!!!");
	}

	return EMPI_SUCCESS;
}
*/

/*
static int receive_server_reply(int *done)
{
	int error, flag, count;
	unsigned char *buffer;
	MPI_Status status;

	error = PMPI_Iprobe(MPI_ANY_SOURCE, TAG_SERVER_REPLY, mpi_comm_gateway_and_application, &flag, &status);

	if (error != MPI_SUCCESS) {
		ERROR(1, "Failed to probe MPI for TAG_SERVER_REQUEST! (error=%d)", error);
		return TRANSLATE_ERROR(error);
	}

	if (!flag) {
		return EMPI_SUCCESS;
	}

	DEBUG(1, "Incoming MPI server_request!");

	error = PMPI_Get_count(&status, MPI_BYTE, &count);

	if (error != MPI_SUCCESS) {
		ERROR(1, "Failed to receive size of MPI data message! (error=%d)", error);
		return TRANSLATE_ERROR(error);
	}

	buffer = malloc(count);

	if (buffer == NULL) {
		ERROR(1, "Failed to allocate space for MPI server message!");
		return EMPI_ERR_INTERN;
	}

	DEBUG(2, "Receiving MPI server reply from source %d, with tag %d and size %d", status.MPI_SOURCE, status.MPI_TAG, count);

	// NOTE: Blocking receive should NOT block, as the probe already told us a message is waiting -- IS THIS TRUE???
	error = PMPI_Recv(buffer, count, MPI_BYTE, status.MPI_SOURCE, status.MPI_TAG, mpi_comm_gateway_and_application, MPI_STATUS_IGNORE);

	if (error != MPI_SUCCESS) {
		DEBUG(1, "Failed to receive MPI server message! source=%d tag=%d count=%d (error=%d)", status.MPI_SOURCE, status.MPI_TAG, count, error);
		return TRANSLATE_ERROR(error);
	}

	// This is a message from the server forwarded to me via MPI by the master gateway.
	return process_gateway_message((generic_message *)buffer, done);
}
***/

//static bool enqueue_will_block(int cluster)
//{
//	// Check if the is enough space to enqueue a message for destination "cluster"
//	int index = gateway_connections[cluster].next_output_stream;
//	return blocking_linked_queue_enqueue_will_block(gateway_connections[cluster].sockets[index]->output_queue);
//}

//static bool enqueue_message(int cluster, void *message, int size, int timeout_usec)
//{
//	// We queue the messages at the destination connection in a round robin fashion.
//	int index = gateway_connections[cluster].next_output_stream;
//
//	if (!blocking_linked_queue_enqueue(gateway_connections[cluster].sockets[index]->output_queue, message, size, timeout_usec)) {
//		return false;
//	}
//
//	gateway_connections[cluster].next_output_stream = (index + 1) % gateway_connections[cluster].stream_count;
//
//	return true;
//}




/*
static generic_message *dequeue_message(int cluster, int timeout_usec)
{
	generic_message *tmp;

	// We dequeue the messages at the source connection in a round robin fashion.
	int index = gateway_connections[cluster].next_input_stream;



	tmp = blocking_linked_queue_dequeue(gateway_connections[cluster].sockets[index]->input_queue, timeout_usec);

	if (tmp == NULL) {
		return NULL;
	}

	gateway_connections[cluster].next_input_stream = (index + 1) % gateway_connections[cluster].stream_count;

	return tmp;
}
*/

/***
static int receive_from_mpi(int *done, int *got_message)
{
	int flag, count, error, pid;
	MPI_Status status;
	generic_message *buffer;

	error = PMPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, mpi_comm_gateway_and_application, &flag, &status);

	if (error != MPI_SUCCESS) {
		ERROR(1, "Failed to probe MPI for ANY message! (error=%d)", error);
		return TRANSLATE_ERROR(error);
	}

	if (!flag) {
		// No messages.
		return EMPI_SUCCESS;
	}

	DEBUG(1, "Incoming MPI message from %d with tag %d!", status.MPI_SOURCE, status.MPI_TAG);

	error = PMPI_Get_count(&status, MPI_BYTE, &count);

	if (error != MPI_SUCCESS) {
		ERROR(1, "Failed to get size of MPI message from %d with tag %d! (error=%d)", status.MPI_SOURCE, status.MPI_TAG, error);
		return TRANSLATE_ERROR(error);
	}

	if (count < 0 || count > MAX_MESSAGE_SIZE) {
		ERROR(1, "Received MPI message with invalid size %d from %d with tag %d! (error=%d)", count, status.MPI_SOURCE,
				status.MPI_TAG);
		return EMPI_ERR_INTERN;
	}

	buffer = malloc(count);

	// NOTE: Blocking receive should NOT block, as the probe already told us a message is waiting -- IS THIS TRUE???
	error = PMPI_Recv(buffer, count, MPI_BYTE, status.MPI_SOURCE, status.MPI_TAG, mpi_comm_gateway_and_application,
			MPI_STATUS_IGNORE);

	if (error != MPI_SUCCESS) {
		ERROR(1, "Failed to receive MPI server message! source=%d tag=%d count=%d (error=%d)", status.MPI_SOURCE,
				status.MPI_TAG, count, error);
		return TRANSLATE_ERROR(error);
	}

	// Check the tag to see what type of message we have received. Options are:
	//
	// TAG_DATA_MSG: data message that need to be forwarded to remote gateway
	// TAG_SERVER_REQUEST: server request that needs to be forwarded to server


	switch (status.MPI_TAG) {
	case TAG_DATA_MSG:

		if (!enqueue_message(GET_CLUSTER_RANK(buffer->header.dst_pid), buffer, count, -1)) {
			ERROR(1, "Failed to enqueue message to gateway %d! (error=%d)", GET_CLUSTER_RANK(buffer->header.dst_pid), error);
			return error;
		}
		break;

	case TAG_SERVER_REQUEST:
		if (gateway_rank != 0) {
			ERROR(1, "Received MPI message with TAG_SERVER_REQUEST on invalid gateway! source=%d tag=%d count=%d",
					status.MPI_SOURCE, status.MPI_TAG, count);
			return EMPI_ERR_INTERN;
		}
		blocking_linked_queue_enqueue(server_info.output_queue, buffer, count, -1);
		break;

	case TAG_SERVER_REPLY:

		pid = ntohl(buffer->header.dst_pid);

		if (pid != my_pid) {
			ERROR(1, "Received invalid server reply via MPI! source=%d tag=%d count=%d", status.MPI_SOURCE, status.MPI_TAG, count);
			return EMPI_ERR_INTERN;
		}

		// This message is indented for me!
		error = process_gateway_message(buffer, done);

		if (error != EMPI_SUCCESS) {
			ERROR(1, "Failed to process server reply for gateway! (error=%d)", error);
			return error;
		}
		break;

	default:
		// should not happen ?
		ERROR(1, "Received MPI message with invalid TAG! source=%d tag=%d count=%d", status.MPI_SOURCE, status.MPI_TAG, count);
		return EMPI_ERR_INTERN;
	}

	return EMPI_SUCCESS;
}
***/

/*
static int receive_server_message_from_mpi(bool *got_message)
{
	int flag, count, error;
	MPI_Status status;
	generic_message *buffer;

	*got_message = false;

	error = PMPI_Iprobe(MPI_ANY_SOURCE, TAG_SERVER_REQUEST, mpi_comm_gateway_and_application, &flag, &status);

	if (error != MPI_SUCCESS) {
		ERROR(1, "Failed to probe MPI for ANY server message! (error=%d)", error);
		return TRANSLATE_ERROR(error);
	}

	if (!flag) {
		// No messages.
		return EMPI_SUCCESS;
	}

	DEBUG(1, "Incoming MPI server message from %d!", status.MPI_SOURCE);

	error = PMPI_Get_count(&status, MPI_BYTE, &count);

	if (error != MPI_SUCCESS) {
		ERROR(1, "Failed to get size of MPI server message from %d! (error=%d)", status.MPI_SOURCE, error);
		return TRANSLATE_ERROR(error);
	}

	if (count < 0 || count > MAX_MESSAGE_SIZE) {
		ERROR(1, "Received MPI server message with invalid size %d from %d! (error=%d)", count, status.MPI_SOURCE);
		return EMPI_ERR_INTERN;
	}

	buffer = malloc(count);

	// NOTE: Blocking receive should NOT block, as the probe already told us a message is waiting -- IS THIS TRUE???
	error = PMPI_Recv(buffer, count, MPI_BYTE, status.MPI_SOURCE, TAG_SERVER_REQUEST, mpi_comm_gateway_and_application,
			MPI_STATUS_IGNORE);

	if (error != MPI_SUCCESS) {
		ERROR(1, "Failed to receive MPI server message! source=%d count=%d (error=%d)", status.MPI_SOURCE, count, error);
		return TRANSLATE_ERROR(error);
	}

	// FIXME: remove extra gateways!
	if (gateway_rank != 0) {
		ERROR(1, "Received MPI message with TAG_SERVER_REQUEST on invalid gateway! source=%d tag=%d count=%d",
				status.MPI_SOURCE, status.MPI_TAG, count);
		return EMPI_ERR_INTERN;
	}

	blocking_linked_queue_enqueue(server_info.output_queue, buffer, count, -1);

	return EMPI_SUCCESS;
}
*/

//static generic_message *pending_message = NULL;
//static int pending_message_count = 0;

/***
static int receive_data_message_from_mpi(int cluster, message_buffer *buf, bool *got_message, bool* no_space)
{
	int flag, count, error;
	MPI_Status status;

	*got_message = false;
	*no_space = false;

	error = PMPI_Iprobe(MPI_ANY_SOURCE, TAG_DATA_MSG + cluster, mpi_comm_gateway_and_application, &flag, &status);

	if (error != MPI_SUCCESS) {
		ERROR(1, "Failed to probe MPI for ANY data message! (error=%d)", error);
		return TRANSLATE_ERROR(error);
	}

	if (!flag) {
		// No messages.
		return EMPI_SUCCESS;
	}

	*got_message = true;

	DEBUG(1, "Incoming MPI data message from %d!", status.MPI_SOURCE);

	error = PMPI_Get_count(&status, MPI_BYTE, &count);

	if (error != MPI_SUCCESS) {
		ERROR(1, "Failed to get size of MPI data message from %d with! (error=%d)", status.MPI_SOURCE, error);
		return TRANSLATE_ERROR(error);
	}

	if (count < 0 || count > MAX_MESSAGE_SIZE) {
		ERROR(1, "Received MPI data message with invalid size %d from %d! (error=%d)", count, status.MPI_SOURCE);
		return EMPI_ERR_INTERN;
	}

	if (buf->end + count > buf->size) {
		*no_space = true;
		return EMPI_SUCCESS;
	}

	// NOTE: Blocking receive should NOT block, as the probe already told us a message is waiting -- IS THIS TRUE???
	error = PMPI_Recv(&(buf->data) + buf->end, count, MPI_BYTE, status.MPI_SOURCE, TAG_DATA_MSG + cluster,
			mpi_comm_gateway_and_application, MPI_STATUS_IGNORE);

	if (error != MPI_SUCCESS) {
		ERROR(1, "Failed to receive MPI data message! source=%d count=%d (error=%d)", status.MPI_SOURCE, count, error);
		return TRANSLATE_ERROR(error);
	}

	buf->end += count;
	mpi_received_data += count;
	mpi_received_count++;

	return EMPI_SUCCESS;
}
***/
/*
static int receive_data_message_from_mpi(int cluster, bool *got_message, bool *would_block)
{
	int flag, count, error;
	MPI_Status status;
	generic_message *buffer;

	*got_message = false;
	*would_block = false;

//	if (pending_message != NULL) {
//
//		if (!enqueue_message(cluster, pending_message, pending_message_count, 0)) {
//			*would_block = true;
//			return EMPI_SUCCESS;
//		}
//
//		pending_message = NULL;
//		pending_message_count = 0;
//	}

	error = PMPI_Iprobe(MPI_ANY_SOURCE, TAG_DATA_MSG + cluster, mpi_comm_gateway_and_application, &flag, &status);

	if (error != MPI_SUCCESS) {
		ERROR(1, "Failed to probe MPI for ANY data message! (error=%d)", error);
		return TRANSLATE_ERROR(error);
	}

	if (!flag) {
		// No messages.
		return EMPI_SUCCESS;
	}

	DEBUG(1, "Incoming MPI data message from %d!", status.MPI_SOURCE);

	error = PMPI_Get_count(&status, MPI_BYTE, &count);

	if (error != MPI_SUCCESS) {
		ERROR(1, "Failed to get size of MPI data message from %d with! (error=%d)", status.MPI_SOURCE, error);
		return TRANSLATE_ERROR(error);
	}

	if (count < 0 || count > MAX_MESSAGE_SIZE) {
		ERROR(1, "Received MPI data message with invalid size %d from %d! (error=%d)", count, status.MPI_SOURCE);
		return EMPI_ERR_INTERN;
	}

	buffer = malloc(count);

	// NOTE: Blocking receive should NOT block, as the probe already told us a message is waiting -- IS THIS TRUE???
	error = PMPI_Recv(buffer, count, MPI_BYTE, status.MPI_SOURCE, TAG_DATA_MSG + cluster, mpi_comm_gateway_and_application,
			MPI_STATUS_IGNORE);

	if (error != MPI_SUCCESS) {
		ERROR(1, "Failed to receive MPI data message! source=%d count=%d (error=%d)", status.MPI_SOURCE, count, error);
		return TRANSLATE_ERROR(error);
	}

	// Check the tag to see what type of message we have received. Options are:
	//
	// TAG_DATA_MSG: data message that need to be forwarded to remote gateway
	// TAG_SERVER_REQUEST: server request that needs to be forwarded to server

	if (!enqueue_message(cluster, buffer, count, -1)) {
//		*would_block = true;
//		pending_message= buffer;
//		pending_message_count = count;
//		return EMPI_SUCCESS;
		return EMPI_ERR_INTERN;
	}

	mpi_received_data += count;
	mpi_received_count++;

	*got_message = true;

	return EMPI_SUCCESS;
}


// Forward all data messages received from other gateways to their destinations using MPI.
static int forward_data_messages_to_mpi(int cluster)
{
   int error, count;
   generic_message *m;

   // We stop dequeuing messages from the TCP receive threads if we have
   // too much data waiting to be forwarded to the local MPI clients.
   // This will (eventually) cause the local receive thread to stop receiving,
   // and the remote send thread to stop sending. As a result, the remote gateway
   // will stop accepting data from the MPI clients
   //
   // FIXME: we may get head of line queuing problems here!!! Only point-to-point
   // (read end-to-end) flow control can prevent this....

   if (pending_isend_data > max_pending_isend_data) {
//	   fprintf(stderr, "EEP! Limit of %ld has been reached (%ld)\n", max_pending_isend_data, pending_isend_data);
	   return EMPI_SUCCESS;
   }

   m = dequeue_message(cluster, 0);

   count = 0;

   while (m != NULL) {

      error = forward_mpi_data_message(m);

      if (error != MPI_SUCCESS) {
         ERROR(1, "Failed to forward message to MPI! (error=%d)", error);
         return error;
      }

      count++;

      if (count >= MAX_MPI_MESSAGE_FORWARD) {
    	  break;
      }

      m = dequeue_message(cluster, 0);
   }

   return EMPI_SUCCESS;
}

static int forward_server_messages_to_mpi(bool *done)
{
	int error;
	generic_message *m;

	m = blocking_linked_queue_dequeue(server_info.input_queue, 0);

	while (m != NULL) {

		if (ntohl(m->header.dst_pid) == my_pid) {
			// This message is indented for me!
			error = process_gateway_message(m, done);

			if (error != MPI_SUCCESS) {
				ERROR(1, "Failed to process gateway message! (error=%d)", error);
				return error;
			}
		} else {
			error = forward_mpi_server_message(m);

			if (error != MPI_SUCCESS) {
				ERROR(1, "Failed to forward server message to MPI! (error=%d)", error);
				return error;
			}
		}

		m = blocking_linked_queue_dequeue(server_info.input_queue, 0);
	}

	return EMPI_SUCCESS;
}
*/

/***
static int receive_data_messages_from_mpi(int cluster)
{
	bool got_message, nospace;
	int error, miss, i;

	miss = 0;

	message_buffer *buf = create_message_buffer();

	// Third, receive messages from MPI
	for (i=0;i<MAX_MPI_RECEIVE_POLL;i++) {

//		if (enqueue_will_block(cluster)) {
//
//			if (enqueue_block_warning) {
//				WARN(1, "Will not receive for cluster %d since enqueue would block!");
//				enqueue_block_warning = false;
//			}
			//return EMPI_SUCCESS;
		//}

//		enqueue_block_warning = true;
		got_message = false;

		error = receive_data_message_from_mpi(cluster, buf, &got_message, &nospace);

		if (error != EMPI_SUCCESS) {
			ERROR(1, "Failed to receive MPI data messages! (error=%d)", error);
			return error;
		}

		if (!got_message) {
			// No message was received
			miss++;

			if (miss > MAX_MPI_POLL_MISS) {
				break;
			}
		} else if (nospace) {
			// We received a message, but there was not enough space in the buffer to receive it!
			if (!enqueue_buffer(cluster, buf, -1)) {
				ERROR(1, "Failed to enqueue buffer!");
				return EMPI_ERR_INTERN;
			}

			// Since we managed to fill a buffer, we decided not ro receive more
			return EMPI_SUCCESS;
		}

		// else, we keep going
	}

	if (buf->end != 0) {
		// The buffer is not empty, so queue it!
		if (!enqueue_buffer(cluster, buf, -1)) {
			ERROR(1, "Failed to enqueue buffer!");
			return EMPI_ERR_INTERN;
		}
	} else {
		// The buffer was empty, so release it!
		release_message_buffer(buf);
	}

	return EMPI_SUCCESS;
}
***/

/*
static int receive_data_messages_from_mpi(int cluster)
{
	bool got_message, would_block;
	int error, miss, i;

	miss = 0;

	// Third, receive messages from MPI
	for (i=0;i<MAX_MPI_RECEIVE_POLL;i++) {

//		if (enqueue_will_block(cluster)) {
//
//			if (enqueue_block_warning) {
//				WARN(1, "Will not receive for cluster %d since enqueue would block!");
//				enqueue_block_warning = false;
//			}
			//return EMPI_SUCCESS;
		//}

//		enqueue_block_warning = true;
		got_message = false;

		error = receive_data_message_from_mpi(cluster, &got_message, &would_block);

		if (error != EMPI_SUCCESS) {
			ERROR(1, "Failed to receive MPI data messages! (error=%d)", error);
			return error;
		}

		if (would_block)  {
			break;
		}

		if (!got_message) {
			miss++;
		}

		if (miss > MAX_MPI_POLL_MISS) {
			break;
		}
	}

	return EMPI_SUCCESS;
}


static int receive_server_messages_from_mpi()
{
	bool got_message;
	int error;

	got_message = false;

	do {
		error = receive_server_message_from_mpi(&got_message);

		if (error != EMPI_SUCCESS) {
			ERROR(1, "Failed to receive MPI server messages! (error=%d)", error);
			return error;
		}
	} while (got_message);

	return EMPI_SUCCESS;
}


// Process the MPI messages.
static int process_mpi_messages(bool *done)
{
	int i, error;

	// First, attempt to push data messages into MPI.
	for (i=0;i<cluster_count;i++) {
		if (i != cluster_rank) {
			error = forward_data_messages_to_mpi(i);

			if (error != EMPI_SUCCESS) {
				ERROR(1, "Failed to forward MPI data messages from cluster %d! (error=%d)", i, error);
				return error;
			}
		}
	}

	if (gateway_rank == 0) {
		// Next handle any server messages.
		error = forward_server_messages_to_mpi(done);

		if (error != EMPI_SUCCESS) {
			ERROR(1, "Failed to forward MPI server messages! (error=%d)", error);
			return error;
		}

		error = receive_server_messages_from_mpi();

		if (error != EMPI_SUCCESS) {
			return error;
		}
	}

	for (i=0;i<cluster_count;i++) {
		if (i != cluster_rank) {
			error = receive_data_messages_from_mpi(i);

			if (error != EMPI_SUCCESS) {
				return error;
			}
		}
	}

	// Fourth, poll to see if any pending send messages have finished.
	error = poll_mpi_requests();

	if (error != EMPI_SUCCESS) {
		ERROR(1, "Failed to poll MPI send requests! (error=%d)", error);
		return error;
	}

	return EMPI_SUCCESS;
}

*/


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

	close(serverfd);

	// We are now sure that everyone agrees that we should stop. We can now disconnect all
	// socket connections without producing unexpected EOFs on the other side!
	disconnect_gateways();

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
	uint64_t sec, millis, receive_data, receive_count, send_data, send_count;

	sec = deltat / 1000000UL;
	millis = (deltat % 1000000UL) / 1000UL;

	// Retrieve stats for sending and receiving
	retrieve_receiver_thread_stats(&receive_data, &receive_count);
	retrieve_sender_thread_stats(&send_data, &send_count);

	printf("STATS FOR GATEWAY %d AFTER %ld.%03ld - WA_IN %ld %ld MPI_OUT %ld %ld - MPI_IN %ld %ld WA_OUT %ld %ld\n",
			cluster_rank, sec, millis,
			receive_data, receive_count, mpi_send_data, mpi_send_count,
			mpi_received_data, mpi_received_count, send_data, send_count);

	fflush(stdout);
}

/*
int messaging_run_gateway(int rank, int size, int empi_size)
{
	int error;
	bool done = false;

//#ifdef SIMPLE_TIMING
	uint64_t start, last;
	uint64_t current;
//#endif

	pending_data_messages = 0;
	pending_data_size = 0;

//#ifdef SIMPLE_TIMING

	start = current_time_micros();
	last = start;

//#endif // SIMPLE_TIMING

	fprintf(stderr, "GATEWAY %d.%d starting!\n", cluster_rank, gateway_rank);

	while (!done) {

		//      fprintf(stderr, "process MPI messages\n");

		error = process_mpi_messages(&done);

		if (error != EMPI_SUCCESS) {
			ERROR(1, "Failed to receive MPI message!");
			return error;
		}

//		if (done) {
//			break;
//		}

		//      fprintf(stderr, "process socket messages\n");

//		error = process_socket_messages(&done);
//
//		if (error != EMPI_SUCCESS) {
//			ERROR(1, "Failed to handle gateway messages!");
//			return error;
//		}

//		if (done == 1) {
//			break;
//		}

//#ifdef SIMPLE_TIMING
		current = current_time_micros();

		// Check if a second has passed
		if ((current - last) > 1000000LU) {
			last = current;
			print_gateway_statistics(current-start);
		}
//#endif // SIMPLE_TIMING
	}

	// We need a barrier to ensure everybody (application and gateway nodes) is ready to finalize.
	// TODO: -- DO WE NEED THIS ??
	error = PMPI_Barrier(MPI_COMM_WORLD);

	if (error != EMPI_SUCCESS) {
		ERROR(1, "Failed to wait for temination! (error=%d)", error);
		return error;
	}

	cleanup();

	// Print final statistics about the communcation with other gateways.

//#ifdef END_TIMING
	current = current_time_micros();

	print_gateway_statistics(current-start);

	printf("GATEWAY %d.%d finished after %ld usec\n", cluster_rank, gateway_rank, (current-start));
//#endif // END_TIMING

	error = PMPI_Finalize();

	if (error != EMPI_SUCCESS) {
		ERROR(1, "Failed to finalize MPI! (error=%d)", error);
		return error;
	}

	return EMPI_SUCCESS;
}
*/

/*****************************************************************************/
/*                      Reading configuration files                          */
/*****************************************************************************/

static int read_config_file()
{
	int  error;
	char *file;
	FILE *config;
	char buffer[1024];

	file = getenv("EMPI_CONFIG");

	if (file == NULL) {
		WARN(0, "EMPI_CONFIG not set!");
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

	// The maximum size of the handshake message is
	//  (2*4 + MAX_LENGTH_CLUSTER_NAME) bytes
	unsigned char message[2*4+MAX_LENGTH_CLUSTER_NAME];
	unsigned int *message_i;
	unsigned int reply[4];
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

	error = socket_receivefully(serverfd, (unsigned char *)reply, 4*4);

	if (error != SOCKET_OK) {
		ERROR(1, "Handshake with server failed! (%d)", error);
		return ERROR_HANDSHAKE_FAILED;
	}

	cluster_rank           = reply[0];
	cluster_count          = reply[1];
	local_application_size = reply[2];
	gateway_count          = reply[3];

	INFO(1, "Received following configuration from server:");
	INFO(1, "  Cluster rank  : %d", cluster_rank);
	INFO(1, "  Cluster count : %d", cluster_count);
	INFO(1, "  Local   size  : %d", local_application_size);
	INFO(1, "  Gateway count : %d", gateway_count);

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
	unsigned short streams;

	error = socket_receivefully(serverfd, (unsigned char*) &ipv4, 4);

	if (error != SOCKET_OK) {
		ERROR(1, "Receive of gateway info failed! (%d)", error);
		return ERROR_HANDSHAKE_FAILED;
	}

	error = socket_receivefully(serverfd, (unsigned char*) &port, 2);

	if (error != SOCKET_OK) {
		ERROR(1, "Receive of gateway info failed! (%d)", error);
		return ERROR_HANDSHAKE_FAILED;
	}

	error = socket_receivefully(serverfd, (unsigned char*) &streams, 2);

	if (error != SOCKET_OK) {
		ERROR(1, "Receive of gateway info failed! (%d)", error);
		return ERROR_HANDSHAKE_FAILED;
	}

	gateway_addresses[index].ipv4 = ipv4;
	gateway_addresses[index].port = port;
	gateway_addresses[index].streams = streams;

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

	opcode  = OPCODE_GATEWAY_READY;

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

/*****************************************************************************/
/*                Initial communication with compute nodes                   */
/*****************************************************************************/


/*
static int perform_non_blocking_read(socket_info2 *info)
{
	int error;
	size_t bytes_read;

	error = socket_receive(info->socketfd, info->current_in_msg + info->current_in_position,
			info->current_in_length - info->current_in_position, false, &bytes_read);

	if (error != SOCKET_OK) {
		return error;
	}

	info->current_in_position += bytes_read;
	return 0;
}

static int perform_non_blocking_write(socket_info2 *info)
{
	int error;
	size_t bytes_written;

	error = socket_send(info->socketfd, info->current_out_msg + info->current_out_position,
			info->current_out_length - info->current_out_position, false, &bytes_written);

	if (error != SOCKET_OK) {
		return error;
	}

	info->current_out_position += bytes_written;
	return 0;

}
*/

/*
static int perform_non_blocking_read(socket_info2 *info)
{
	int error;
	size_t bytes_read;

	error = socket_receive(info->socketfd, info->in_msg, false);

	if (error != SOCKET_OK) {
		return error;
	}

	return 0;
}


static int perform_non_blocking_write(socket_info2 *info)
{
	int error;
	size_t bytes_written;

	error = socket_send(info->socketfd, info->current_out_msg + info->current_out_position,
			info->current_out_length - info->current_out_position, false, &bytes_written);

	if (error != SOCKET_OK) {
		return error;
	}

	info->current_out_position += bytes_written;
	return 0;

}
*/

static int read_handshake_compute_node(socket_info *info)
{
	int error;
	uint32_t *msg;
	uint32_t size;

	if (info->state != STATE_READING_HANDSHAKE) {
		ERROR(1, "Socket %d in invalid state %d", info->socketfd, info->state);
		return ERROR_HANDSHAKE_FAILED;
	}

//	error = perform_non_blocking_read(info);
	error = socket_receive_mb(info->socketfd, info->in_msg, false);

	if (error != SOCKET_OK) {
		ERROR(1, "Failed to read handshake message of compute node!");
		return ERROR_HANDSHAKE_FAILED;
	}

	if (message_buffer_max_write(info->in_msg) == 0) {
		// Full message has been read. Process it and send reply back!
		msg = (uint32_t *) message_buffer_direct_read_access(info->in_msg, 3 * sizeof(uint32_t));

		if (msg[0] != OPCODE_HANDSHAKE) {
			// Invalid handshake!
			close(info->socketfd);
			return ERROR_HANDSHAKE_FAILED;
		}

		// Retrieve the rank of the connecting machine.
		info->index = msg[1];
		size = msg[2];

		message_buffer_destroy(info->in_msg);
		info->in_msg = NULL;

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
			info->out_msg = message_buffer_create(3 * sizeof(uint32_t) + (2*cluster_count+1) * sizeof(int));

			msg = (uint32_t *) message_buffer_direct_write_access(info->out_msg, 3 * sizeof(uint32_t));

			msg[0] = OPCODE_HANDSHAKE_ACCEPTED;
			msg[1] = cluster_rank;
			msg[2] = cluster_count;

			error = message_buffer_write(info->out_msg, (unsigned char *)cluster_sizes, cluster_count * sizeof(int));

			if (error != cluster_count * sizeof(int)) {
				ERROR(1, "Failed to create reply for compute node 0!");
				close(info->socketfd);
				return ERROR_HANDSHAKE_FAILED;
			}

			error = message_buffer_write(info->out_msg, (unsigned char *)cluster_offsets, (cluster_count+1) * sizeof(int));

			if (error != (cluster_count+1) * sizeof(int)) {
				ERROR(1, "Failed to create reply for compute node 0!");
				close(info->socketfd);
				return ERROR_HANDSHAKE_FAILED;
			}
		} else {
			// We send the compute node with rank != 0 a simple opcode.
			info->out_msg = message_buffer_create(sizeof(uint32_t));
			msg = (uint32_t *) message_buffer_direct_write_access(info->out_msg, 1 * sizeof(uint32_t));
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

	if (info->state != STATE_WRITING_HANDSHAKE) {
		ERROR(1, "Socket %d in invalid state %d", info->socketfd, info->state);
		return ERROR_HANDSHAKE_FAILED;
	}

//	error = perform_non_blocking_write(info);

	error = socket_send_mb(info->socketfd, info->out_msg, false);

	if (error != SOCKET_OK) {
		ERROR(1, "Failed to write handshake message to compute node!");
		return ERROR_HANDSHAKE_FAILED;
	}

	if (message_buffer_max_read(info->out_msg) == 0) {
		// Full message has been written. Socket will be idle until all nodes have connected.
		info->state = STATE_IDLE;

		message_buffer_destroy(info->out_msg);
		info->out_msg = NULL;

		error = socket_set_idle(epollfd, info->socketfd, info);

		if (error != 0) {
			return error;
		}

		INFO(2, "Handshake with compute node %d of %d completed!", info->index, local_application_size);

		pending_local_connections--;
		local_connections++;
	}

	return 0;
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

	new_info->in_msg = message_buffer_create(3 * sizeof(uint32_t));

	if (new_info->in_msg == NULL) {
		ERROR(1, "Failed to allocate handshake message!");
		return ERROR_ALLOCATE;
	}

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
	local_compute_nodes = malloc(local_application_size * sizeof(socket_info *));

	if (local_compute_nodes == NULL) {
		WARN(1, "Failed to allocate space for local compute node connection info!");
		close(serverfd);
		return ERROR_ALLOCATE;
	}

	memset(local_compute_nodes, 0, local_application_size * sizeof(socket_info *));

	// open local server socket at local_port
	error = socket_listen(local_port, -1, -1, 100, &listenfd);

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

				if ((events[i].events & (EPOLLERR | EPOLLHUP)) != 0) {
					// Error or hang up occurred in file descriptor!
					ERROR(1, "Unexpected error on socket %d", info->socketfd);
					return ERROR_POLL;
				}

				info = events[i].data.ptr;

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

		local_compute_nodes[i]->state = STATE_READ_HEADER;
		local_compute_nodes[i]->in_msg = message_buffer_create(MESSAGE_HEADER_SIZE);
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

	pending_isend_data = 0;
	max_pending_isend_data = MAX_PENDING_ISEND_DATA;

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
	status = socket_connect(server_ipv4, server_port, -1, -1, &serverfd);

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
	gateway_addresses = malloc(sizeof(gateway_address) * cluster_count * gateway_count);

	if (gateway_addresses == NULL) {
		ERROR(1, "Failed to allocate space for gateway addresses!");
		return ERROR_ALLOCATE;
	}

	memset(gateway_addresses, 0, sizeof(gateway_address) * cluster_count * gateway_count);

	// Next, Receives all global gateway info from the server.
	INFO(2, "Receiving IP information on all gateways from server");

	status = receive_gateway_info();

	if (status != 0) {
		close(serverfd);
		return status;
	}

	// Generate my PID. All ones, except for the cluster rank.
	my_pid = SET_PID(cluster_rank, 0xFFFFFF);

	INFO(2, "All IP information received -- connecting to other gateways");

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

	// We are done talking to the server for now. We add the serverfd to the epollfdf, and perform any additional
	// communication with the server in an asynchronous fashion.

	INFO(2, "Adding server connection to epoll set");

	server_info->in_msg = message_buffer_create(MESSAGE_HEADER_SIZE);
	server_info->state = STATE_READ_HEADER;

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

static int process_messages()
{
	int i, error, count, active_connections;
	struct epoll_event *events;
	socket_info *info;
	message_buffer *m;
	bool disconnect;
	uint32_t event;

	// Allocate space for the events, one per compute node.
	events = malloc(local_application_size * sizeof(struct epoll_event));

	if (events == NULL) {
		ERROR(1, "Failed to allocate space for epoll events!");
		return ERROR_ALLOCATE;
	}

	active_connections = local_application_size + 1;

	while (active_connections > 1) {

		count = epoll_wait(epollfd, events, local_application_size, 1000);

		if (count < 0) {
			ERROR(1, "Failed to wait for socket event!");
			return ERROR_POLL;
		}

		if (count > 0) {
			for (i=0;i<count;i++) {

				info = events[i].data.ptr;
				event = events[i].events;

				if ((event & (EPOLLERR | EPOLLHUP)) != 0) {
					// Error or hang up occurred in file descriptor!
					ERROR(1, "Unexpected error/hangup on connection to compute node %d", info->index);
					return ERROR_POLL;
				}

				if (event & EPOLLOUT) {
					error = write_message(info);

					if (error != 0) {
						ERROR(1, "Failed to handle event on connection to compute node %d (error=%d)", i, error);
						return ERROR_CONNECTION;
					}
				}

				if (event & EPOLLIN) {
					m = NULL;

					error = read_message(info, false, &m, &disconnect);

					if (error != 0) {
						ERROR(1, "Failed to handle event on connection to compute node %d (error=%d)", i, error);
						return ERROR_CONNECTION;
					}

					if (m != NULL) {
						// Read produced a message.
						if (info->type == TYPE_SERVER) {
							enqueue_message_from_server(m);
						} else {
							enqueue_message_from_compute_node(m);
						}
					}

					if (disconnect) {
						if (info->state == STATE_TERMINATING) {
							// Disconnect was expected!
							INFO(1, "Disconnecting compute node %d", info->index);
							socket_remove_from_epoll(epollfd, info->socketfd);
							close(info->socketfd);
							info->state = STATE_TERMINATED;
							active_connections--;
						} else {
							ERROR(1, "Unexpected termination of link to compute node %d", info->index);
							return ERROR_CONNECTION;
						}
					}
				}
			}
		}
	}

	// We should now have 1 active connection left (to the server)
	cleanup();

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


