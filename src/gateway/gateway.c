#include <stdio.h>
#include <unistd.h>
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
#include "linked_queue.h"
#include "socket_util.h"
#include "udt_util.h"

#define MAX_MESSAGE_SIZE (MAX_MESSAGE_PAYLOAD + (MESSAGE_HEADER_SIZE + 5*sizeof(int)))

#define MAX_LENGTH_CLUSTER_NAME 128
#define MAX_STREAMS 16

#define RECEIVE_BUFFER_SIZE (32*1024*1024)
#define SEND_BUFFER_SIZE (32*1024*1024)

#define TYPE_SERVER           (0)
#define TYPE_ACCEPT           (1)
#define TYPE_GATEWAY_TCP      (2)
#define TYPE_GATEWAY_UDT      (3)
#define TYPE_COMPUTE_NODE_TCP (4)

#define WIDE_AREA_PROTOCOL (TYPE_GATEWAY_TCP)

#define STATE_ACCEPTING          0
#define STATE_READING_HANDSHAKE  1
#define STATE_WRITING_HANDSHAKE  2
#define STATE_IDLE               3
#define STATE_READ_HEADER        4
#define STATE_READ_PAYLOAD       5
#define STATE_TERMINATING        6
#define STATE_TERMINATED         7

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

	// Receiver thread (only used in gateway-to-gateway connections).
	pthread_t receive_thread;

	// Sender thread (only used in gateway-to-gateway connections).
	pthread_t send_thread;

	// Current message that is being written.
	message_buffer *out_msg;

	// Current message that is read.
	message_buffer *in_msg;

	// The number of bytes received on this socket_info that have not been forwarded yet (poor mans flow control).
	size_t pending_ack_bytes;

	// A mutex to make the pending bytes thread safe.
	pthread_mutex_t bytes_mutex;

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
	ssize_t written;

	// If there was no message in progress, we try to dequeue one.
	if (info->out_msg == NULL) {
		info->out_msg = dequeue_message_from_socket_info(info, 0);

		if (info->out_msg == NULL) {
			return 0;
		}
	}

	written = socket_send_mb(info->socketfd, info->out_msg, false);

	if (written < 0) {
		ERROR(1, "Failed to write message to compute node %d!", info->index);
		return ERROR_CONNECTION;
	}

	if (message_buffer_max_read(info->out_msg) == 0) {
		// Full message has been written, so destroy it and dequeue the next message.
		// If no message is available, the socket will be set to read only mode until more message arrive.

		if (info->type == TYPE_SERVER) {
			add_ack_bytes((message_header *)info->out_msg->data);
		}

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
	ssize_t bytes_read;
	message_header mh;

	INFO(1, "XXX Receive of message from socket %d start=%d end=%d size=%d", info->socketfd, info->in_msg->start, info->in_msg->end,
			info->in_msg->size);

	bytes_read = socket_receive_mb(info->socketfd, info->in_msg, 0, blocking);

	INFO(1, "XXX Receive of message from socket %d done! start=%d end=%d size=%d", info->socketfd, info->in_msg->start, info->in_msg->end,
			info->in_msg->size);

	if (bytes_read < 0) {

		if (bytes_read == SOCKET_DISCONNECT && info->state == STATE_TERMINATING) {
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
	bool done;
	ssize_t written;
	socket_info *info;
	message_buffer *m;

	uint64_t last_write, now, data, count;

	info = (socket_info *) arg;
	done = false;

	now = last_write = current_time_micros();
	data = 0;
	count = 0;

	while (!done) {
		m = dequeue_message_from_socket_info(info, 1000000);

		if (m != NULL) {

			written = socket_send_mb(info->socketfd, m, true);

			if (written < 0) {
				ERROR(1, "Failed to send message!");
				return NULL;
			}

			count++;
			data += written;

//			// HACK -- flow control on gateway - client link
//			mh = (message_header *) m->data;
//			release_bytes(GET_PROCESS_RANK(mh->src_pid), avail);

			add_ack_bytes((message_header *)m->data);

			message_buffer_destroy(m);

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

		m = message_buffer_wrap(buffer, mh.length, false);
		m->end = mh.length;

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

	info->pending_ack_bytes = 0;

	info->in_msg = NULL;
	info->out_msg = NULL;

	info->input_queue = NULL;
	info->output_queue = linked_queue_create(-1);

	error = pthread_cond_init(&(info->output_cond), NULL);

	if (error != 0) {
		FATAL("Failed to create socket info!");
	}

	error = pthread_mutex_init(&(info->output_mutex), NULL);

	if (error != 0) {
		FATAL("Failed to create socket info!");
	}

	error = pthread_mutex_init(&(info->bytes_mutex), NULL);

	if (error != 0) {
		FATAL("Failed to create socket info!");
	}

	return info;
}

static int init_socket_info_threads(socket_info *info)
{
	int error;

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
	int socketfd;

	status = 0;
	socketfd = gateway_connections[index].sockets[0]->socketfd;

	if (gateway_connections[index].sockets[0]->type == TYPE_GATEWAY_TCP) {
		status = socket_receivefully(socketfd, (unsigned char *) &opcode, 4);
	} else if (gateway_connections[index].sockets[0]->type == TYPE_GATEWAY_UDT) {
		status = udt_receivefully(socketfd, (unsigned char *) &opcode, 4);
	} else {
		FATAL("Unknown gateway protocol!");
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
		FATAL("Unknown gateway protocol!");
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

				if (WIDE_AREA_PROTOCOL == TYPE_GATEWAY_TCP) {
					// Create a path to the target gateway.
					status = socket_connect(gateway_addresses[remoteIndex].ipv4, gateway_addresses[remoteIndex].port + s, -1, -1,
							&socket);
				} else if (WIDE_AREA_PROTOCOL == TYPE_GATEWAY_UDT) {
					status = udt_connect(gateway_addresses[remoteIndex].ipv4, gateway_addresses[remoteIndex].port + s, -1, -1,
							&socket);
				}

				if (status != CONNECT_OK) {
					ERROR(1, "Failed to connect!");
					return status;
				}

				gateway_connections[i].sockets[s] = create_socket_info(socket, WIDE_AREA_PROTOCOL, STATE_IDLE);
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

			if (WIDE_AREA_PROTOCOL == TYPE_GATEWAY_TCP) {
				// Create a path to the target gateway.
				status = socket_accept_one(local_port + s, gateway_addresses[remoteIndex].ipv4, -1, -1, &socket);
			} else if (WIDE_AREA_PROTOCOL == TYPE_GATEWAY_UDT) {
				status = udt_accept(local_port + s, gateway_addresses[remoteIndex].ipv4, -1, -1, &socket);
			}

			if (status != CONNECT_OK) {
				ERROR(1, "Failed to accept!");
				return status;
			}

			gateway_connections[crank].sockets[s] = create_socket_info(socket, WIDE_AREA_PROTOCOL, STATE_IDLE);
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

	if (index != cluster_rank) {
		for (s=0;s<gateway_connections[index].stream_count;s++) {

			if (WIDE_AREA_PROTOCOL == TYPE_GATEWAY_TCP) {
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


static int read_handshake_compute_node(socket_info *info)
{
	int error;
	uint32_t *msg;
	uint32_t size;
	ssize_t bytes_read;

	if (info->state != STATE_READING_HANDSHAKE) {
		ERROR(1, "Socket %d in invalid state %d", info->socketfd, info->state);
		return ERROR_HANDSHAKE_FAILED;
	}

//	error = perform_non_blocking_read(info);
	bytes_read = socket_receive_mb(info->socketfd, info->in_msg, 0, false);

	if (bytes_read < 0) {
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
	ssize_t written;

	if (info->state != STATE_WRITING_HANDSHAKE) {
		ERROR(1, "Socket %d in invalid state %d", info->socketfd, info->state);
		return ERROR_HANDSHAKE_FAILED;
	}

//	error = perform_non_blocking_write(info);

	written = socket_send_mb(info->socketfd, info->out_msg, false);

	if (written < 0) {
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

					// We only read data from this compute node if it hasn't got too much data waiting to be send.
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


