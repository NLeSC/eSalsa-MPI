#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
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
#include "mpi.h"
#include "opcodes.h"
#include "message_queue.h"

#include "linked_queue.h"
#include "blocking_linked_queue.h"
#include "socket_util.h"
#include "udt_util.h"

//#define DETAILED_MESSAGE_INFO 1

#define MAX_MPI_RECEIVE_POLL 1
#define MAX_MPI_POLL_MISS 5
#define MAX_MPI_MESSAGE_FORWARD 1

#define MAX_MESSAGE_SIZE (8*1024*1024 + DATA_MESSAGE_SIZE)
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

#define TYPE_SERVER     (0)
#define TYPE_TCP_DATA   (1)
#define TYPE_UDT_DATA   (2)

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

#define CONNECT_ERROR_HANDSHAKE_FAILED 12
#define CONNECT_ERROR_ALLOCATE         13
#define CONNECT_ERROR_CANNOT_FIND_IP   14
#define CONNECT_ERROR_GATEWAY          15

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
	int index;

	pthread_t receive_thread;
	pthread_t send_thread;

	blocking_linked_queue *output_queue;
	blocking_linked_queue *input_queue;

	//   uint64_t in_bytes;
	//   uint64_t in_messages;

	//#ifdef DETAILED_TIMING
	//   uint64_t in_starttime;
	//#endif // DETAILED_TIMING
	//
	//   uint64_t out_bytes;
	//   uint64_t out_messages;
	//
	//#ifdef DETAILED_TIMING
	//   uint64_t out_starttime;
	//#endif // DETAILED_TIMING
	//
	//
	////   linked_queue *out_queue;

} socket_info;

// A type to store gateway information.
typedef struct {
	unsigned long  ipv4;
	unsigned short port;
	unsigned short streams;
} gateway_address;

typedef struct {
	gateway_address info;
	socket_info sockets[MAX_STREAMS];
	int stream_count;
	int next_output_stream;
	int next_input_stream;
} gateway_connection;

/*
typedef struct s_mpi_message {
	MPI_Request r;
	struct s_mpi_message *next;
	generic_message *message;
} mpi_message;
*/

static MPI_Request *isend_requests;
static MPI_Status *isend_statusses;
static int *isend_requests_ready;

static linked_queue **pending_isend_messages;
static linked_queue **pending_isend_server_messages;

static generic_message **current_isend_messages;

static size_t pending_isend_data;
static size_t max_pending_isend_data;

//static bool enqueue_block_warning = true;

// The name of this cluster (must be unique).
static char *cluster_name;
static gateway_address *gateway_addresses;
static gateway_connection *gateway_connections;

//static blocking_linked_queue *incoming_queue;

// The number of clusters and the rank of our cluster in this set.
uint32_t cluster_count;
uint32_t cluster_rank;

// The size of each cluster, and the offset of each cluster in the total set of machines.
int *cluster_sizes;
int *cluster_offsets;

// The number of application processes and gateways in this cluster.
uint32_t local_application_size;
uint32_t gateway_count;

// The gateway rank of this gateway process. -1 if process is application process.
int gateway_rank;

//static MPI_Comm mpi_comm_application_only;
MPI_Comm mpi_comm_gateways_only;
extern MPI_Comm mpi_comm_gateway_and_application;

// The file descriptor used to epoll the gateway connections.
// static int epollfd;

// The filedescriptor of the socket connected to the 'server'.
static int serverfd = 0;

// The hostname of the server
static char *server_name;
static long server_ipv4;
static unsigned short server_port;

// Socket info containing information on the address of the server.
static socket_info server_info;

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
//static mpi_message *mpi_messages;
//static int mpi_messages_count;
//static int mpi_messages_count_max;

// Used for pending receive operations
//static int pending_receive = 0;
//static int pending_receive_count = 0;
//static MPI_Request pending_receive_request;
//static generic_message *pending_receive_buffer = NULL;

// The PID of this process. Not valid on a gateway process.
extern uint32_t my_pid;

static uint64_t pending_data_messages;
static uint64_t pending_data_size;

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

	// FIXME: how to stop ?
	while (!done) {
		mh = blocking_linked_queue_dequeue(info->output_queue, -1);

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


// Sender thread for socket connected to server.
void* server_sender_thread(void *arg)
{
	socket_info *info;
	bool done;
	message_header *mh;
	int error;

	info = (socket_info *) arg;
	done = false;

	// FIXME: how to stop ?
	while (!done) {
		mh = blocking_linked_queue_dequeue(info->output_queue, -1);

		if (mh != NULL) {
			error = socket_sendfully(info->socketfd, (unsigned char *)mh, ntohl(mh->length));

			free(mh);

			if (error != SOCKET_OK) {
				ERROR(1, "Failed to send message!");
				return NULL;
			}
		}
	}

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

		blocking_linked_queue_enqueue(info->input_queue, buffer, mh.length, -1);

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

		blocking_linked_queue_enqueue(info->input_queue, buffer, mh.length, -1);

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
// Receiver thread for sockets.
void* server_receiver_thread(void *arg)
{
	int len, error;
	socket_info *info;
	bool done;
	message_header mh;
	void *buffer;

	info = (socket_info *) arg;
	done = false;

	// FIXME: how to stop ?
	while (!done) {

		error = socket_receivefully(info->socketfd, (unsigned char *)&mh, MESSAGE_HEADER_SIZE);

		if (error != SOCKET_OK) {
			ERROR(1, "Failed to receive header!");
			return NULL;
		}

		len = htonl(mh.length);

		if (len < 0 || len > MAX_MESSAGE_SIZE) {
			ERROR(1, "Receive invalid message header length=%d from server!\n", len);
			return NULL;
		}

		buffer = malloc(len);

		if (buffer == NULL) {
			ERROR(1, "Failed to allocate space for message of length=%d from server!\n", len);
			return NULL;
		}

		memcpy(buffer, &mh, MESSAGE_HEADER_SIZE);

		error = socket_receivefully(info->socketfd, buffer + MESSAGE_HEADER_SIZE, len-MESSAGE_HEADER_SIZE);

		if (error != SOCKET_OK) {
			ERROR(1, "Failed to receive message payload from server!");
			return NULL;
		}

		blocking_linked_queue_enqueue(info->input_queue, buffer, len, -1);
	}

	return NULL;
}


//static message_buffer *create_message_buffer()
//{
//   message_buffer *tmp = malloc(sizeof(message_buffer) + 2*MAX_MESSAGE_SIZE);
//
//   tmp->size = 2*MAX_MESSAGE_SIZE;
//   tmp->start = 0;
//   tmp->end = 0;
//   tmp->messages = 0;
//
//   return tmp;
//}

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


/*****************************************************************************/
/*                      Initialization / Finalization                        */
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
		return 0;
	}

	INFO(0, "Looking for config file %s", file);

	config = fopen(file, "r");

	if (config == NULL) {
		ERROR(1, "Failed to open config file %s", file);
		return 0;
	}

	INFO(0, "Config file %s opened.", file);

	// Read the cluster name
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

	// Read the server address
	error = fscanf(config, "%s", buffer);

	if (error == EOF || error == 0) {
		fclose(config);
		ERROR(1, "Failed to read server adres from %s", file);
		return 0;
	}

	server_name = malloc(strlen(buffer+1));

	if (server_name == NULL) {
		fclose(config);
		ERROR(1, "Failed to allocate space for server adres %s", buffer);
		return 0;
	}

	strcpy(server_name, buffer);

	// Read the server port
	error = fscanf(config, "%hu", &server_port);

	if (error == EOF || error == 0) {
		fclose(config);
		ERROR(1, "Failed to read server port from %s", file);
		return 0;
	}

	fclose(config);
	return 1;
}

static int init_cluster_info(int *argc, char ***argv)
{
	int i, changed, cnt, len;

	cluster_name = NULL;
	server_name = NULL;
	server_port = -1;

	// First try to read the configuration from an input file whose location is set in the environment.
	// This is needed since Fortran does not pass the command line arguments to the MPI library like C does.
	if (read_config_file() == 1) {
		return 1;
	}

	// Next, parse the command line (possibly overwriting the config).
	i = 1;

	while ( i < (*argc) ){
		changed = 0;

		if ( strcmp((*argv)[i],"--wa-server") == 0 ) {
			if ( i+1 < *argc ){
				server_name = malloc(strlen((*argv)[i+1])+1);
				strcpy(server_name, (*argv)[i+1]);
				DELETE_ARG;
			} else {
				ERROR(1, "Missing option for --wa-server");
				return 0;
			}
			DELETE_ARG;

		} else if ( strcmp((*argv)[i],"--wa-server-port") == 0 ) {
			if ( i+1 < *argc ){
				server_port = (unsigned short) atoi((*argv)[i+1]);
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
			} else {
				ERROR(1, 0, "Missing option for --wa-cluster-name");
				return 0;
			}
			DELETE_ARG;
		}

		if ( !changed ) i++;
	}

	return 1;
}

static void init_socket_info(socket_info *info, int socketfd, int type, size_t max_out, size_t max_in)
{
	info->socketfd = socketfd;
	info->type = type;
	info->output_queue = blocking_linked_queue_create(-1, max_out);
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

static int init_socket_info_threads(socket_info *info)
{
	int error;

	if (info->type == TYPE_SERVER) {
		error = pthread_create(&info->receive_thread, NULL, &server_receiver_thread, info);

		if (error != 0) {
			fprintf(stderr, "Failed to create server receive thread for socket %d!\n", info->socketfd);
			return CONNECT_ERROR_ALLOCATE;
		}

		error = pthread_create(&info->send_thread, NULL, &server_sender_thread, info);

		if (error != 0) {
			fprintf(stderr, "Failed to create server sender thread for socket %d!\n", info->socketfd);
			return CONNECT_ERROR_ALLOCATE;
		}

	} else if (info->type == TYPE_TCP_DATA) {
		error = pthread_create(&info->receive_thread, NULL, &tcp_receiver_thread, info);

		if (error != 0) {
			fprintf(stderr, "Failed to create TCP receive thread for socket %d!\n", info->socketfd);
			return CONNECT_ERROR_ALLOCATE;
		}

		error = pthread_create(&info->send_thread, NULL, &tcp_sender_thread, info);

		if (error != 0) {
			fprintf(stderr, "Failed to create TCP sender thread for socket %d!\n", info->socketfd);
			return CONNECT_ERROR_ALLOCATE;
		}

 	} else if (info->type == TYPE_UDT_DATA) {
		error = pthread_create(&info->receive_thread, NULL, &udt_receiver_thread, info);

		if (error != 0) {
			fprintf(stderr, "Failed to create UDT receive thread for socket %d!\n", info->socketfd);
			return CONNECT_ERROR_ALLOCATE;
		}

		error = pthread_create(&info->send_thread, NULL, &udt_sender_thread, info);

		if (error != 0) {
			fprintf(stderr, "Failed to create UDT sender thread for socket %d!\n", info->socketfd);
			return CONNECT_ERROR_ALLOCATE;
		}
	}

	return CONNECT_OK;
}

static int start_gateway_threads(int index)
{
    int i, status;

    // We do not need to add a connection to ourselves!
    if (index == cluster_rank) {
       return CONNECT_OK;
    }

    for (i=0;i<gateway_connections[index].stream_count;i++) {

    	status = init_socket_info_threads(&gateway_connections[index].sockets[i]);

    	if (status != CONNECT_OK) {
    		ERROR(1, "Failed to start threads for gateway %d.%d!", index, i);
    		return status;
    	}
    }

    return CONNECT_OK;
}

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

	message_i[0] = htonl(OPCODE_HANDSHAKE);
	message_i[1] = htonl(strlen(cluster_name));

	strcpy((char*) (message+(2*4)), cluster_name);

	error = socket_sendfully(serverfd, message, 2*4+strlen(cluster_name));

	if (error != SOCKET_OK) {
		ERROR(1, "Handshake with server failed! (%d)", error);
		return CONNECT_ERROR_HANDSHAKE_FAILED;
	}

	error = socket_receivefully(serverfd, (unsigned char *)&opcode, 4);

	if (error != SOCKET_OK) {
		ERROR(1, "Handshake with server failed! (%d)", error);
		return CONNECT_ERROR_HANDSHAKE_FAILED;
	}

	opcode = ntohl(opcode);

	if (opcode != OPCODE_HANDSHAKE_ACCEPTED) {
		ERROR(1, "Server refused handshake! (%d)", opcode);
		return CONNECT_ERROR_HANDSHAKE_FAILED;
	}

	error = socket_receivefully(serverfd, (unsigned char *)reply, 4*4);

	if (error != SOCKET_OK) {
		ERROR(1, "Handshake with server failed! (%d)", error);
		return CONNECT_ERROR_HANDSHAKE_FAILED;
	}

	cluster_rank           = ntohl(reply[0]);
	cluster_count          = ntohl(reply[1]);
	local_application_size = ntohl(reply[2]);
	gateway_count          = ntohl(reply[3]);

	INFO(1, "Received following configuration from server:");
	INFO(1, "  Cluster rank   : %d", cluster_rank);
	INFO(1, "  Cluster count  : %d", cluster_count);
	INFO(1, "  Local app size : %d", local_application_size);
	INFO(1, "  Gateway counte : %d", gateway_count);

	if (cluster_count == 0 || cluster_count >= MAX_CLUSTERS) {
		ERROR(1, "Cluster count out of bounds! (%d)", cluster_count);
		return CONNECT_ERROR_HANDSHAKE_FAILED;
	}

	if (cluster_rank >= cluster_count) {
		ERROR(1, "Cluster rank out of bounds! (%d >= %d)", cluster_rank, cluster_count);
		return CONNECT_ERROR_HANDSHAKE_FAILED;
	}

	if (local_application_size == 0 || local_application_size >= MAX_PROCESSES_PER_CLUSTER) {
		ERROR(1, "Local application size out of bounds! (%d)", local_application_size);
		return CONNECT_ERROR_HANDSHAKE_FAILED;
	}

	if (gateway_count == 0) {
		ERROR(1, "Local gateway count too low! (%d)", gateway_count);
		return CONNECT_ERROR_HANDSHAKE_FAILED;
	}

	return CONNECT_OK;
}

static int receive_cluster_sizes()
{
	int error, i;

	error = socket_receivefully(serverfd, (unsigned char*) cluster_sizes, cluster_count * 4);

	if (error != SOCKET_OK) {
		ERROR(1, "Handshake with server failed! (%d)", error);
		return CONNECT_ERROR_HANDSHAKE_FAILED;
	}

	for (i=0;i<cluster_count;i++) {
		cluster_sizes[i] = ntohl(cluster_sizes[i]);
	}

	cluster_offsets[0] = 0;

	for (i=1;i<=cluster_count;i++) {
		cluster_offsets[i] = cluster_offsets[i-1] + cluster_sizes[i-1];
	}

	for (i=0;i<cluster_count;i++) {
		INFO(0, "Cluster %d %d %d", i, cluster_sizes[i], cluster_offsets[i]);
	}

	INFO(0, "Total size %d", cluster_offsets[cluster_count]);

	return CONNECT_OK;
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
		return CONNECT_ERROR_HANDSHAKE_FAILED;
	}

	error = socket_receivefully(serverfd, (unsigned char*) &port, 2);

	if (error != SOCKET_OK) {
		ERROR(1, "Receive of gateway info failed! (%d)", error);
		return CONNECT_ERROR_HANDSHAKE_FAILED;
	}

	error = socket_receivefully(serverfd, (unsigned char*) &streams, 2);

	if (error != SOCKET_OK) {
		ERROR(1, "Receive of gateway info failed! (%d)", error);
		return CONNECT_ERROR_HANDSHAKE_FAILED;
	}

	gateway_addresses[index].ipv4 = ipv4;
	gateway_addresses[index].port = ntohs(port);
	gateway_addresses[index].streams = ntohs(streams);

	return CONNECT_OK;
}

static int receive_gateway_info()
{
	int i,j, error;

	for (i=0;i<cluster_count;i++) {
		for (j=0;j<gateway_count;j++) {

			INFO(2, "Receiving gateway info for gateway %d/%d", i, j);

			error = receive_gateway(i*gateway_count+j);

			if (error != CONNECT_OK) {
				ERROR(1, "Failed to receive gateway info!");
				return CONNECT_ERROR_HANDSHAKE_FAILED;
			}
		}
	}

	INFO(2, "All gateway info received!");

	return CONNECT_OK;
}

static int send_gateway_info(int rank, struct in_addr *ip4ads, int ip4count)
{
	int error;
	gateway_request_msg req;

	req.opcode  = htonl(OPCODE_GATEWAY_INFO);
	req.cluster = htonl(cluster_rank);
	req.src     = htonl(rank);
	req.count   = htonl(ip4count);

	error = socket_sendfully(serverfd, (unsigned char *) &req, GATEWAY_REQUEST_SIZE);

	if (error != SOCKET_OK) {
		ERROR(1, "Failed to send cluster info header to server!");
		return EMPI_ERR_INTERN;
	}

	error = socket_sendfully(serverfd, (unsigned char *) ip4ads, ip4count * sizeof(struct in_addr));

	if (error != SOCKET_OK) {
		ERROR(1, "Failed to send cluster info data to server!");
		return EMPI_ERR_INTERN;
	}

	return CONNECT_OK;
}

static int receive_gateway_ready_opcode(int index)
{
	int status;
	int opcode;
	int socketfd = gateway_connections[index].sockets[0].socketfd;

	if (gateway_connections[index].sockets[0].type == TYPE_TCP_DATA) {
		status = socket_receivefully(socketfd, (unsigned char *) &opcode, 4);
	} else if (gateway_connections[index].sockets[0].type == TYPE_UDT_DATA) {
		status = udt_receivefully(socketfd, (unsigned char *) &opcode, 4);
	}

	if (status != SOCKET_OK) {
		ERROR(1, "Handshake with gateway failed! (%d)", status);
		return CONNECT_ERROR_GATEWAY;
	}

	return CONNECT_OK;
}

static int send_gateway_ready_opcode(int index)
{
	int status;
	int opcode = OPCODE_GATEWAY_READY;
	int socketfd = gateway_connections[index].sockets[0].socketfd;

	if (gateway_connections[index].sockets[0].type == TYPE_TCP_DATA) {
		status = socket_sendfully(socketfd, (unsigned char *) &opcode, 4);
	} else if (gateway_connections[index].sockets[0].type == TYPE_UDT_DATA) {
		status = udt_sendfully(socketfd, (unsigned char *) &opcode, 4);
	}

	if (status != SOCKET_OK) {
		ERROR(1, "Handshake with gateway failed! (%d)", status);
		return CONNECT_ERROR_HANDSHAKE_FAILED;
	}

	return CONNECT_OK;
}

static int connect_to_gateways(int crank, int local_port)
{
	int remoteIndex, i, s;
	int status, socket;

	// FIXME: hardcoded data transfer type. Should be in server config!

	int type = TYPE_UDT_DATA;

	if (crank == cluster_rank) {
		// I must initiate the connection!
		for (i=cluster_rank+1;i<cluster_count;i++) {
			remoteIndex = i*gateway_count + gateway_rank;

			gateway_connections[i].stream_count = gateway_addresses[remoteIndex].streams;
			gateway_connections[i].next_output_stream = 0;
			gateway_connections[i].next_input_stream = 0;

			// gateway_connections[i].mpi_receive_buffer = create_message_buffer();

			for (s=0;s<gateway_addresses[remoteIndex].streams;s++) {

				INFO(2, "Connecting to gateway stream %d.%d.%d -> index = %d", i, gateway_rank, s, remoteIndex);

				if (type == TYPE_TCP_DATA) {
					// Create a path to the target gateway.
					status = socket_connect(gateway_addresses[remoteIndex].ipv4, gateway_addresses[remoteIndex].port + s, -1, -1,
							&socket);
				} else if (type == TYPE_UDT_DATA) {
					status = udt_connect(gateway_addresses[remoteIndex].ipv4, gateway_addresses[remoteIndex].port + s, -1, -1,
							&socket);
				}

				if (status != CONNECT_OK) {
					ERROR(1, "Failed to connect!");
					return status;
				}

				init_socket_info(&(gateway_connections[i].sockets[s]), socket, type, MAX_BLOCKED_QUEUE_SIZE,
						MAX_BLOCKED_QUEUE_SIZE);

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
		gateway_connections[crank].next_output_stream = 0;
		gateway_connections[crank].next_input_stream = 0;

		for (s=0;s<gateway_addresses[remoteIndex].streams;s++) {

			INFO(2, "Accepting from gateway stream %d.%d.%d -> index = %d", crank, gateway_rank, s, remoteIndex);

			if (type == TYPE_TCP_DATA) {
				// Create a path to the target gateway.
				status = socket_accept(local_port + s, gateway_addresses[remoteIndex].ipv4, -1, -1, &socket);
			} else if (type == TYPE_UDT_DATA) {
				status = udt_accept(local_port + s, gateway_addresses[remoteIndex].ipv4, -1, -1, &socket);
			}

			if (status != CONNECT_OK) {
				ERROR(1, "Failed to accept!");
				return status;
			}

			init_socket_info(&(gateway_connections[crank].sockets[s]), socket, type, MAX_BLOCKED_QUEUE_SIZE,
					MAX_BLOCKED_QUEUE_SIZE);

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
		return CONNECT_ERROR_ALLOCATE;
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

		status = start_gateway_threads(i);

		if (status != CONNECT_OK) {
			ERROR(1, "Failed to add gateway %d to epoll (error=%d)", i, status);
			return status;
		}
	}

	return CONNECT_OK;
}

static int disconnect_gateway(int index)
{
	int s;

	if (index != cluster_rank) {
		for (s=0;s<gateway_connections[index].stream_count;s++) {
			close(gateway_connections[index].sockets[s].socketfd);
		}
	}

	return CONNECT_OK;
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

	return CONNECT_OK;
}

int master_gateway_init(int rank, int size, int *argc, char ***argv)
{
	int status;

	cluster_count = 0;
	cluster_rank = -1;

//	mpi_messages = NULL;
//	mpi_messages_count = 0;
//	mpi_messages_count_max = 0;

	INFO(1, "Initializing master gateway");

	// Create a queue for incoming server messages. No limit on length.
//	incoming_server_queue = blocking_linked_queue_create(-1, 0);

	// Read the cluster name and server location from a file.
	status = init_cluster_info(argc, argv);

	if (status == 0) {
		WARN(1, "Failed to initialize EMPI wide area implementation (MPI-based)!");
		return EMPI_ERR_INTERN;
	}

	if (cluster_name == NULL || server_name == NULL || server_port <= 0 || server_port > 65535) {
		WARN(1, "Cluster info not correctly set (name=%s server=%d port=%d)!", cluster_name, server_name, server_port);
		return EMPI_ERR_INTERN;
	}

	status = socket_get_ipv4_address(server_name, &server_ipv4);

	if (status != SOCKET_OK) {
		WARN(1, "Failed to retrieve server address!");
		return EMPI_ERR_INTERN;
	}

	INFO(1, "I am master gateway in cluster %s. Server at %s %d", cluster_name, server_name, server_port);

	// At least this last process should be a gateway, although there may be more.
	// Therefore, we let this process connect to the server to get information on the others.
	status = socket_connect(server_ipv4, server_port, -1, -1, &serverfd);

	if (status != SOCKET_OK) {
		WARN(1, "Failed to connect to server!");
		return EMPI_ERR_INTERN;
	}

	init_socket_info(&server_info, serverfd, TYPE_SERVER, 0, 0);

	// Ask the server for information on the cluster count and number of gateways per cluster.
	status = handshake();

	if (status != CONNECT_OK) {
		WARN(1, "Failed to perform handshake with server!");
		close(serverfd);
		return EMPI_ERR_INTERN;
	}

	if ((local_application_size + gateway_count) != size) {
		WARN(1, "Cluster size mismatch. Expected %d + %d, but got %d!", local_application_size, gateway_count, size);
		close(serverfd);
		return EMPI_ERR_INTERN;
	}

	cluster_sizes = malloc(cluster_count * sizeof(int));
	cluster_offsets = malloc((cluster_count+1) * sizeof(int));

	if (cluster_sizes == NULL || cluster_offsets == NULL) {
		WARN(1, "Failed to allocate space for cluster info!");
		close(serverfd);
		return EMPI_ERR_INTERN;
	}

	status = receive_cluster_sizes();

	if (status != CONNECT_OK) {
		WARN(1, "Failed to receive cluster sizes!");
		close(serverfd);
		return EMPI_ERR_INTERN;
	}

	return EMPI_SUCCESS;
}

int generic_gateway_init(int rank, int size)
{
	int status, error, src, dst, ip4count, i;
	struct in_addr *ip4ads;

	// I am one of the gateways.
	INFO(1, "I am one of the gateways -- performing generic gateway init!");

	gateway_start_time = current_time_micros();

	error = pthread_mutex_init(&send_data_mutex, NULL);

	if (error != 0) {
		ERROR(1, "Failed to init mutex!");
		return EMPI_ERR_INTERN;
	}

	error = pthread_mutex_init(&received_data_mutex, NULL);

	if (error != 0) {
		ERROR(1, "Failed to init mutex!");
		return EMPI_ERR_INTERN;
	}

//	mpi_messages = NULL;
//	mpi_messages_count = 0;
//	mpi_messages_count_max = 0;

	pending_isend_data = 0;
	max_pending_isend_data = MAX_PENDING_ISEND_DATA;

	isend_requests = malloc(local_application_size * sizeof(MPI_Request));

	if (isend_requests == NULL) {
		ERROR(1, "Failed to allocate isend_request array!");
		return EMPI_ERR_INTERN;
	}

	isend_statusses = malloc(local_application_size * sizeof(MPI_Status));

	if (isend_statusses == NULL) {
		ERROR(1, "Failed to allocate isend_statusses!");
		return EMPI_ERR_INTERN;
	}

	isend_requests_ready = malloc(local_application_size * sizeof(int));

	if (isend_requests_ready == NULL) {
		ERROR(1, "Failed to allocate isend_request_ready array!");
		return EMPI_ERR_INTERN;
	}

	pending_isend_messages = malloc(local_application_size * sizeof(linked_queue *));

	if (pending_isend_messages == NULL) {
		ERROR(1, "Failed to allocate pending_isend_messages array!");
		return EMPI_ERR_INTERN;
	}

	pending_isend_server_messages = malloc(local_application_size * sizeof(linked_queue *));

	if (pending_isend_server_messages == NULL) {
		ERROR(1, "Failed to allocate pending_isend_server_messages array!");
		return EMPI_ERR_INTERN;
	}

	current_isend_messages = malloc(local_application_size * sizeof(generic_message *));

	if (current_isend_messages == NULL) {
		ERROR(1, "Failed to allocate current_isend_messages array!");
		return EMPI_ERR_INTERN;
	}

	for (i=0;i<local_application_size;i++) {
		isend_requests[i] = MPI_REQUEST_NULL;
		isend_requests_ready[i] = 0;
		current_isend_messages[i] = NULL;

		pending_isend_messages[i] = linked_queue_create(-1);

		if (pending_isend_messages[i] == NULL) {
			ERROR(1, "Failed to allocate pending_isend_messages[%d] queue!", i);
			return EMPI_ERR_INTERN;
		}

		pending_isend_server_messages[i] = linked_queue_create(-1);

		if (pending_isend_messages[i] == NULL) {
			ERROR(1, "Failed to allocate pending_isend_messages[%d] queue!", i);
			return EMPI_ERR_INTERN;
		}
	}

#ifdef DETAILED_TIMING
	write_timing_count = 0;
	read_timing_count = 0;
#endif // DETAILED_TIMING

	// Create an fd for polling.
	//   epollfd = epoll_create1(0);

	// Init the incoming message queue.
//	incoming_queue = blocking_linked_queue_create(-1, -1);

	// Retrieve the local IPv4 addresses.
	status = get_local_ips(&ip4ads, &ip4count);

	if (status != SOCKET_OK) {
		ERROR(1, "Failed to retrieve local addresses! (error=%d)", status);
		return EMPI_ERR_INTERN;
	}

	if (gateway_rank == 0) {

		INFO(1, "Sending my IP information to the server");

		// Send my IPv4 address to the server.
		status = send_gateway_info(0, ip4ads, ip4count);

		free(ip4ads);

		if (status != CONNECT_OK) {
			ERROR(1, "Failed to forward gateway address info from gateway %d to server! (error=%d)", size-1, status);
			close(serverfd);
			return EMPI_ERR_INTERN;
		}

		// If there are more gateways in this cluster, receive their IPs and forward them as well.
		if (gateway_count > 1) {

			for (src=1;src<gateway_count;src++) {

				INFO(1, "Receiving IP information from gateway %d", src);

				error = PMPI_Recv(&ip4count, 1, MPI_INT, src, 0, mpi_comm_gateways_only, MPI_STATUS_IGNORE);

				if (error != MPI_SUCCESS) {
					ERROR(1, "Failed to receive gateway address info from gateways %d! (error=%d)", src, error);
					close(serverfd);
					return EMPI_ERR_INTERN;
				}

				ip4ads = malloc(ip4count * sizeof(struct in_addr));

				if (ip4ads == NULL) {
					ERROR(1, "Failed to allocate space for remote gateway addresses!");
					close(serverfd);
					return EMPI_ERR_INTERN;
				}

				error = PMPI_Recv(ip4ads, ip4count * sizeof(struct in_addr), MPI_BYTE, src, 0, mpi_comm_gateways_only, MPI_STATUS_IGNORE);

				if (error != MPI_SUCCESS) {
					ERROR(1, "Failed to receive gateway address info from gateways %d! (error=%d)", src, error);
					close(serverfd);
					return EMPI_ERR_INTERN;
				}

				INFO(1, "Sending IP information from gateway %d to server", src);

				status = send_gateway_info(src, ip4ads, ip4count);

				if (status != CONNECT_OK) {
					ERROR(1, "Failed to forward gateway address info from gateway %d to server! (error=%d)", src, status);
					close(serverfd);
					return EMPI_ERR_INTERN;
				}

				free(ip4ads);
			}
		}

	} else {

		INFO(1, "Sending my IP information to gateway 0 of my cluster");

		// If I'm a gateway, but not gateway 0, I should forward my IPs to gateway 0.
		error = PMPI_Send(&ip4count, 1, MPI_INT, 0, 0, mpi_comm_gateways_only);

		if (error != MPI_SUCCESS) {
			ERROR(1, "Failed to send gateway address info from gateway %d! (error=%d)", gateway_rank, error);
			return EMPI_ERR_INTERN;
		}

		error = PMPI_Send(ip4ads, ip4count * sizeof(struct in_addr), MPI_BYTE, 0, 0, mpi_comm_gateways_only);

		if (error != MPI_SUCCESS) {
			ERROR(1, "Failed to send gateway address info from gateways %d! (error=%d)", rank, error);
			return EMPI_ERR_INTERN;
		}

		free(ip4ads);
	}

	// All gateway info from this cluster is now send to the server. We allocate space for gateway info for all clusters.
	gateway_addresses = malloc(sizeof(gateway_address) * cluster_count * gateway_count);

	if (gateway_addresses == NULL) {
		ERROR(1, "Failed to allocate space for gateway addresses!");
		return CONNECT_ERROR_HANDSHAKE_FAILED;
	}

	memset(gateway_addresses, 0, sizeof(gateway_address) * cluster_count * gateway_count);

	// Next, the gateway with rank 0 receives all global gateway info from the server.
	if (gateway_rank == 0) {

		INFO(1, "Receiving IP information on all gateways from server");

		status = receive_gateway_info();

		if (status != CONNECT_OK) {
			ERROR(1, "Failed to receive cluster sizes!");
			close(serverfd);
			return EMPI_ERR_INTERN;
		}

		// If there are more gateways in our cluster we also forward the gateway info to them.
		if (gateway_count > 1) {
			// Send the gateway info to the other gateways in this cluster.
			for (dst=1;dst<gateway_count;dst++) {

				INFO(1, "Sending IP information on all gateways to gateway %d", dst);

				error = PMPI_Send(gateway_addresses, sizeof(gateway_address) * cluster_count * gateway_count, MPI_BYTE, dst, 1, mpi_comm_gateways_only);

				if (error != MPI_SUCCESS) {
					ERROR(1, "Failed to send gateway addresses to other gateways! (error=%d)", error);
					close(serverfd);
					return EMPI_ERR_INTERN;
				}
			}
		}

		// We are done talking to the server for now. Start the send and receive threads for the server connection for any future
		// asynchronous communication that may be needed.
		init_socket_info_threads(&server_info);

	} else {

		INFO(1, "Receiving IP information on all gateways from gateway 0");

		// Receive the gateway info from gateway 0 in this cluster.
		error = PMPI_Recv(gateway_addresses, sizeof(gateway_address) * cluster_count * gateway_count, MPI_BYTE, 0, 1, mpi_comm_gateways_only, MPI_STATUS_IGNORE);

		if (error != MPI_SUCCESS) {
			ERROR(1, "Failed to send gateway addresses to other gateways! (error=%d)", error);
			return EMPI_ERR_INTERN;
		}
	}

	INFO(1, "All IP information received -- connecting to other gateways!");

	// Now connect all gateway processes to each other. This assumes a direct connection is possible between each pair of gateways!
	error = connect_gateways();

	if (error != CONNECT_OK) {
		ERROR(1, "Failed to connect gateways! (error=%d)", error);
		return EMPI_ERR_INTERN;
	}

	return EMPI_SUCCESS;
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

/*
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
 */

static int process_gateway_message(generic_message *m, bool *done)
{
	// NOTE: server messages are external and therefore in network byte order!
	int opcode = ntohl(m->header.opcode);

	DEBUG(1, "Received gateway message with opcode %d", opcode);

	free(m);

	if (opcode != OPCODE_FINALIZE_REPLY) {
		ERROR(1, "Failed to process gateway message with unknown opcode %d!", opcode);
		return EMPI_ERR_INTERN;
	}

	INFO(1, "Gateway receive FINALIZE message from server!");

	*done = true;

	return EMPI_SUCCESS;
}

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

/*
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
*/

static bool enqueue_will_block(int cluster)
{
	// Check if the is enough space to enqueue a message for destination "cluster"
	int index = gateway_connections[cluster].next_output_stream;
	return blocking_linked_queue_enqueue_will_block(gateway_connections[cluster].sockets[index].output_queue);
}

static bool enqueue_message(int cluster, void *message, int size, int timeout_usec)
{
	// We queue the messages at the destination connection in a round robin fashion.
	int index = gateway_connections[cluster].next_output_stream;

	if (!blocking_linked_queue_enqueue(gateway_connections[cluster].sockets[index].output_queue, message, size, timeout_usec)) {
		return false;
	}

	gateway_connections[cluster].next_output_stream = (index + 1) % gateway_connections[cluster].stream_count;

	return true;
}

static generic_message *dequeue_message(int cluster, int timeout_usec)
{
	generic_message *tmp;

	// We dequeue the messages at the source connection in a round robin fashion.
	int index = gateway_connections[cluster].next_input_stream;

	tmp = blocking_linked_queue_dequeue(gateway_connections[cluster].sockets[index].input_queue, timeout_usec);

	if (tmp == NULL) {
		return NULL;
	}

	gateway_connections[cluster].next_input_stream = (index + 1) % gateway_connections[cluster].stream_count;

	return tmp;
}

/*
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
*/

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

static generic_message *pending_message = NULL;
static int pending_message_count = 0;

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
	   fprintf(stderr, "NOT FORWARDING! Limit of %ld has been reached (%ld)\n", max_pending_isend_data, pending_isend_data);
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

void cleanup()
{
	int opcode, error;

	// The master gateway needs to tell the server that this cluster is ready to stop.
	// Only after all master gateways in all clusters are ready will the server reply and
	// are we allowed to continue.
	if (gateway_rank == 0) {

/*

 FIXME: fix cleanup!!

 		error = set_socket_blocking(serverfd);

		if (error != EMPI_SUCCESS) {
			WARN(1, "Failed to set server socket to BLOCKING mode! (error=%d)", error);
		}

		opcode = htonl(OPCODE_CLOSE_LINK);

		error = socket_sendfully(serverfd, (unsigned char*) &opcode, 4);

		if (error != EMPI_SUCCESS) {
			WARN(1, "Failed to send OPCODE_CLOSE_LINK to server! (error=%d)", error);
		}

		error = socket_receivefully(serverfd, (unsigned char*) &opcode, 4);

		if (error != EMPI_SUCCESS) {
			WARN(1, "Failed to receive OPCODE_CLOSE_LINK from server! (error=%d)", error);
		}
*/
		close(serverfd);
	}

	// Do another barrier to ensure that all gateways in this cluster wait for the master
	// gateway and thereby all other gateways in all other clusters.
	error = PMPI_Barrier(mpi_comm_gateways_only);

	if (error != EMPI_SUCCESS) {
		WARN(1, "Failed to wait for other gateways other clusters! (error=%d)", error);
	}

	// We are now sure that everyone agrees that we should stop. We can now disconnect all
	// socket connections without producing unexpected EOFs on the other side!
	disconnect_gateways();

	// Finally close the epoll socket.
	// close(epollfd);
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

