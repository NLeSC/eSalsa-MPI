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

//#define DETAILED_MESSAGE_INFO 1

#define MAX_POLL_MISS_COUNT (100)

#define MAX_MESSAGE_SIZE (8*1024*1024)

#define MIN_INTERESTING_BUFFER_SIZE (512*1024)

// #define MAX_SINGLE_MPI_RECEIVE    (2*1024*1024)
#define MAX_SINGLE_SOCKET_RECEIVE (8*1024*1024)
#define MAX_SINGLE_SOCKET_SEND    (8*1024*1024)

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

#define STATE_RW (1)
#define STATE_RO (0)

#define TYPE_SERVER (0)
#define TYPE_DATA   (1)

// Error codes used internally.
#define CONNECT_OK                      0
#define CONNECT_ERROR_CREATE_SOCKET     1
#define CONNECT_ERROR_CONNECT           2
#define CONNECT_ERROR_OPTIONS           3
#define CONNECT_ERROR_BIND              4
#define CONNECT_ERROR_LISTEN            5
#define CONNECT_ERROR_ACCEPT            6
#define CONNECT_ERROR_HOST              7
#define CONNECT_ERROR_HOST_NOT_FOUND    8
#define CONNECT_ERROR_ADDRESS_TYPE      9
#define CONNECT_ERROR_SEND_FAILED      10
#define CONNECT_ERROR_RECEIVE_FAILED   11
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

typedef struct {
   uint32_t size;
   uint32_t start;
   uint32_t end;
   uint32_t messages;
   unsigned char data[];
} message_buffer;

//static bounded_array_list *waiting_mpi_buffers;
//static data_buffer *mpi_receive_buffer = NULL;

// A gateway request message whcih is used to inform the server
// of the contact information of a gateway.
typedef struct {
   int opcode;  // type of message
   int cluster; // rank of cluster
   int src;     // rank in cluster
   int count;   // ip adres count
} gateway_request_msg;

#define GATEWAY_REQUEST_SIZE (4*sizeof(int))

// A type to store socket information
typedef struct {
   int socketfd;
   int type;

//    int state;
//   int cluster;
//   generic_message *in;
//   uint32_t inpos;
//   uint32_t incount;

   uint64_t in_bytes;
   uint64_t in_messages;

#ifdef DETAILED_TIMING
   uint64_t in_starttime;
#endif // DETAILED_TIMING

//   message_queue *in_queue;
//   generic_message *out;
//   uint32_t outpos;
//   uint32_t outcount;

   uint64_t out_bytes;
   uint64_t out_messages;

#ifdef DETAILED_TIMING
   uint64_t out_starttime;
#endif // DETAILED_TIMING

   message_buffer *socket_read_buffer;
   message_buffer *socket_write_buffer;

//   linked_queue *out_queue;

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

   message_buffer *mpi_receive_buffer;

} gateway_connection;

typedef struct s_mpi_message {
   MPI_Request r;
   struct s_mpi_message *next;
   //generic_message *message;
   unsigned char data[];
} mpi_message;

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

// The current message buffer used to receive messages for the server.
static message_buffer *server_mpi_receive_buffer = NULL;

// The queue of message buffers waiting to be send to the server.
// static linked_queue *server_queue_out;

// The queue of incoming server messages.
//static linked_queue *server_queue_in;

// Message queue for incoming messages. They are parked here until they can be forwarded.
//static linked_queue *incoming_queue;

// Queue of pending MPI_Isends.
static mpi_message *mpi_messages;

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

static message_buffer *create_message_buffer()
{
   message_buffer *tmp = malloc(sizeof(message_buffer) + 2*MAX_MESSAGE_SIZE);

   tmp->size = 2*MAX_MESSAGE_SIZE;
   tmp->start = 0;
   tmp->end = 0;
   tmp->messages = 0;

   return tmp;
}

static mpi_message *create_mpi_message(generic_message *source, int len)
{
   mpi_message *tmp;

   if (len  < 0) {
      return NULL;
   }

   tmp = malloc(sizeof(mpi_message) + len);

   if (tmp == NULL) {
      return NULL;
   }

   tmp->next = NULL;
   memcpy(&(tmp->data[0]), source, len);
   return tmp;
}

static uint64_t current_time_micros()
{
   uint64_t result;
   struct timeval t;

   gettimeofday(&t,NULL);

   result = (t.tv_sec * 1000000LU) + t.tv_usec;

   return result;
}

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

static int get_server_address()
{
   struct addrinfo *result;
   int error;

   /* resolve the server name into a list of addresses */
   error = getaddrinfo(server_name, NULL, NULL, &result);

   if (error != 0) {
      ERROR(1, "getaddrinfo failed: %s", gai_strerror(error));
      return CONNECT_ERROR_HOST_NOT_FOUND;
   }

   if (result->ai_family == AF_INET) {
      DEBUG(1, "Got inet4");
   } else {
      ERROR(1, 0, "Got unsupported address type!");
      return CONNECT_ERROR_ADDRESS_TYPE;
   }

   server_ipv4 = ((struct sockaddr_in *)(result->ai_addr))->sin_addr.s_addr;

   freeaddrinfo(result);

   return CONNECT_OK;
}

static void init_socket_info(socket_info *info, int socketfd, int type)
{
   info->socketfd = socketfd;
   info->type = type;

//   info->state = STATE_RO;

   info->in_bytes = 0;
   info->out_bytes = 0;

   info->in_messages = 0;
   info->out_messages = 0;

   info->socket_read_buffer = create_message_buffer();
   info->socket_write_buffer = create_message_buffer();
}

/*****************************************************************************/
/*                          Socket operations                                */
/*****************************************************************************/

static int set_socket_non_blocking(int socketfd)
{
   int error;

   int flags = fcntl (socketfd, F_GETFL, 0);

   if (flags == -1) {
      ERROR(1, "Failed to get socket flags! (error=%d)", errno);
      return EMPI_ERR_INTERN;
   }

   flags |= O_NONBLOCK;

   error = fcntl (socketfd, F_SETFL, flags);

   if (error == -1) {
      ERROR(1, "Failed to set socket to NON_BLOCKING mode! (error=%d)", errno);
      return EMPI_ERR_INTERN;
   }

   return EMPI_SUCCESS;
}

static int set_socket_blocking(int socketfd)
{
   int error;

   int flags = fcntl (socketfd, F_GETFL, 0);

   if (flags == -1) {
      ERROR(1, "Failed to get socket flags! (error=%d)", errno);
      return EMPI_ERR_INTERN;
   }

   flags ^= O_NONBLOCK;

   error = fcntl (socketfd, F_SETFL, flags);

   if (error == -1) {
      ERROR(1, "Failed to set socket to BLOCKING mode! (error=%d)", errno);
      return EMPI_ERR_INTERN;
   }

   return EMPI_SUCCESS;
}

/*
static int add_socket_to_epoll(int socketfd, void *data)
{
   int error;
   struct epoll_event event;

   DEBUG(1, "Adding socket %d to epoll %d", socketfd, epollfd);

   event.data.ptr = data;
   event.events = EPOLLIN;

   error = epoll_ctl (epollfd, EPOLL_CTL_ADD, socketfd, &event);

   if (error == -1) {
      ERROR(1, "Failed to add socket to epoll (error=%d %s)", errno, strerror(errno));
      return EMPI_ERR_INTERN;
   }

   return EMPI_SUCCESS;
}
*/
/*
static int set_socket_in_epoll_to_rw(int socketfd, void *data)
{
   int error;
   struct epoll_event event;

   DEBUG(1, "Setting socket %d to RW in epoll %d", socketfd, epollfd);

   event.data.ptr = data;
   event.events = EPOLLIN | EPOLLOUT;

   error = epoll_ctl (epollfd, EPOLL_CTL_MOD, socketfd, &event);

   if (error != 0) {
      ERROR(1, "Failed to set socket to RW (%d %d error=%d %s )", epollfd, socketfd, errno, strerror(errno));
      return EMPI_ERR_INTERN;
   }

   return EMPI_SUCCESS;
}

static int set_socket_in_epoll_to_ro(int socketfd, void *data)
{
   int error;
   struct epoll_event event;

   DEBUG(1, "Setting socket %d to RO in epoll %d", socketfd, epollfd);

   event.data.ptr = data;
   event.events = EPOLLIN;

   error = epoll_ctl (epollfd, EPOLL_CTL_MOD, socketfd, &event);

   if (error != 0) {
      ERROR(1, "Failed to set socket to RO (error=%d %d)", error, strerror(errno));
      return EMPI_ERR_INTERN;
   }

   return EMPI_SUCCESS;
}
*/

/*****************************************************************************/
/*                      Send / Receive operations                            */
/*****************************************************************************/

static int socket_sendfully(int socketfd, unsigned char *buffer, size_t len)
{
   size_t w = 0;
   ssize_t tmp = 0;

   while (w < len) {
      tmp = write(socketfd, buffer+w, len-w);

      if (tmp < 0) {
         ERROR(1, "socket_sendfully failed! (%s)", strerror(errno));
         return CONNECT_ERROR_SEND_FAILED;
      } else {
         w += tmp;
      }
   }

   return CONNECT_OK;
}

static int socket_receivefully(int socketfd, unsigned char *buffer, size_t len)
{
   size_t r = 0;
   ssize_t tmp = 0;

   while (r < len) {
      tmp = read(socketfd, buffer+r, len-r);

      if (tmp < 0) {
         ERROR(1, "socket_receivefully failed! (%s)", strerror(errno));
         return CONNECT_ERROR_RECEIVE_FAILED;
      } else {
         r += tmp;
      }
   }

   return CONNECT_OK;
}

static int set_socket_options(int socket)
{
   int rcvbuf, sndbuf, flag, error;

   // Set send and receive buffer size
   rcvbuf = RECEIVE_BUFFER_SIZE;

   error = setsockopt(socket, SOL_SOCKET, SO_RCVBUF, (char *)&rcvbuf, sizeof(int));

   if (error != 0) {
      WARN(1, "Failed to set SO_RCVBUF!");
   }

   sndbuf = SEND_BUFFER_SIZE;

   error = setsockopt(socket, SOL_SOCKET, SO_SNDBUF, (char *)&sndbuf, sizeof(int));

   if (error != 0) {
      WARN(1, "Failed to set SO_SNDBUF!");
   }

   // Set TCP nodelay
//   flag = 1;

//   error = setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));

//   if (error != 0) {
//      WARN(1, "Failed to set TCP_NODELAY!");
//   }

   return CONNECT_OK;
}


static int socket_connect(unsigned long ipv4, unsigned short port, int *socketfd)
{
   struct sockaddr_in address;
   socklen_t addrlen;
   int error;
   int connected = 0;
   int attempts = 0;
   char ipstring[INET_ADDRSTRLEN+1];

   address.sin_family = AF_INET;
   address.sin_addr.s_addr = ipv4;
   address.sin_port = htons(port);

   inet_ntop(AF_INET, &(address.sin_addr), ipstring, INET_ADDRSTRLEN);

   INFO(2, "Connecting to %s:%d", ipstring, port);

   *socketfd = socket(AF_INET, SOCK_STREAM, 0);

   if (*socketfd < 0) {
      ERROR(1, "Failed to create socket!");
      return CONNECT_ERROR_CREATE_SOCKET;
   }

   error = set_socket_options(*socketfd);

   if (error != 0) {
      close(*socketfd);
      ERROR(1, "Failed to set socket options!");
      return CONNECT_ERROR_OPTIONS;
   }

   addrlen = sizeof(struct sockaddr_in);

   while (attempts < 1000) {

      error = connect(*socketfd, (struct sockaddr *)&address, addrlen);

      if (error != 0) {
         WARN(1, "Failed to connect to %s:%d (error = %d) -- will retry!", ipstring, port, error);
         sleep(1);
      } else {
         connected = 1;
         break;
      }
   }

   if (connected == 0) {
      close(*socketfd);
      ERROR(1, "Failed to connect to %s:%d", ipstring, port);
      return CONNECT_ERROR_CONNECT;
   }

   INFO(1, "Created connection to %s:%d", ipstring, port);

   return CONNECT_OK;
}

static int socket_accept(unsigned short local_port, uint32_t expected_host, int *socketfd)
{
   int sd, new_socket;
   int error;
   uint32_t host;
   struct sockaddr_in address;
   int flag = 1;
   socklen_t addrlen;
   char buffer[INET_ADDRSTRLEN+1];

   sd = socket(AF_INET, SOCK_STREAM, 0);

   if (sd < 0) {
      ERROR(1, "Failed to create socket!");
      return CONNECT_ERROR_CREATE_SOCKET;
   }

   address.sin_family = AF_INET;
   address.sin_addr.s_addr = INADDR_ANY;
   address.sin_port = htons(local_port);

   INFO(2, "Accepting connection from host %s on port %d", inet_ntop(AF_INET, &expected_host, buffer, INET_ADDRSTRLEN+1), local_port);

   error = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *) &flag, sizeof(int));

   if (error != 0) {
      close(sd);
      ERROR(1, "Failed to set SO_REUSEADDR on server socket!");
      return CONNECT_ERROR_BIND;
   }

   error = set_socket_options(sd);

   if (error != 0) {
      close(sd);
      ERROR(1, "Failed to set socket options!");
      return CONNECT_ERROR_OPTIONS;
   }

   error = bind(sd,(struct sockaddr *)&address, sizeof(address));

   if (error != 0) {
      close(sd);
      ERROR(1, "Failed to bind socket to port %d!", local_port);
      return CONNECT_ERROR_BIND;
   }

   error = listen(sd, 1);

   if (error != 0) {
      close(sd);
      ERROR(1, "Failed to listen to socket on port %d!", local_port);
      return CONNECT_ERROR_LISTEN;
   }

   addrlen = sizeof(struct sockaddr_in);

   new_socket = accept(sd, (struct sockaddr *)&address, &addrlen);

   close(sd);

   if (new_socket < 0) {
      ERROR(1, "Failed to accept socket connection on port %d!", local_port);
      return CONNECT_ERROR_ACCEPT;
   }

   host = address.sin_addr.s_addr;

   if (expected_host == host) {
     INFO(1, "Received connection from expected host %s:%d!", inet_ntop(AF_INET, &(address.sin_addr), buffer, INET_ADDRSTRLEN+1), ntohs(address.sin_port));
   } else {
      close(new_socket);
      ERROR(1, "Received connection from unexpected host %s (%d != %d)!", inet_ntop(AF_INET, &host, buffer, INET_ADDRSTRLEN+1), host, expected_host);
      return CONNECT_ERROR_HOST;
   }

   *socketfd = new_socket;

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

   if (error != CONNECT_OK) {
      ERROR(1, "Handshake with server failed! (%d)", error);
      return CONNECT_ERROR_HANDSHAKE_FAILED;
   }

   error = socket_receivefully(serverfd, (unsigned char *)&opcode, 4);

   if (error != CONNECT_OK) {
      ERROR(1, "Handshake with server failed! (%d)", error);
      return CONNECT_ERROR_HANDSHAKE_FAILED;
   }

   opcode = ntohl(opcode);

   if (opcode != OPCODE_HANDSHAKE_ACCEPTED) {
      ERROR(1, "Server refused handshake! (%d)", opcode);
      return CONNECT_ERROR_HANDSHAKE_FAILED;
   }

   error = socket_receivefully(serverfd, (unsigned char *)reply, 4*4);

   if (error != CONNECT_OK) {
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

   if (error != CONNECT_OK) {
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

static int get_local_ips(struct in_addr **ip4ads, int *ip4count)
{
   int count, index, status;
   struct in_addr *output;
   struct ifaddrs *addresses;
   struct ifaddrs *current;
   char host[NI_MAXHOST];

   INFO(1, "Retrieving local IP addresses.");

   addresses = NULL;

   getifaddrs(&addresses);

   if (addresses == NULL) {
      ERROR(1, "Failed to find valid IPv4 address on this gateway!");
      return CONNECT_ERROR_CANNOT_FIND_IP;
   }

   index = 0;
   count = 0;
   current = addresses;

   while (current != NULL) {

      if (current->ifa_addr != NULL && strcmp(current->ifa_name, "lo") != 0) {

         if (current->ifa_addr->sa_family==AF_INET) {

            // This is an IP4 address
            status = getnameinfo(current->ifa_addr, sizeof(struct sockaddr_in), host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

            if (status == 0) {
               INFO(2, "Found device %s with IP %s", current->ifa_name, host);
            }

            count++;

         } else if (current->ifa_addr->sa_family==AF_INET6) {
            // This is an IP6 address -- ignored for now!
            // ipv6++;
         }
      }

      current = current->ifa_next;
   }

   if (count == 0) {
      ERROR(1, "Failed to find valid IPv4 address on this gateway!");
      freeifaddrs(addresses);
      return CONNECT_ERROR_CANNOT_FIND_IP;
   }

   output = malloc(count * sizeof(struct in_addr));

   if (output == NULL) {
      ERROR(1, "Failed to allocate space for IPv4 addresses on this gateway!");
      freeifaddrs(addresses);
      return CONNECT_ERROR_ALLOCATE;
   }

   current = addresses;

   while (current != NULL) {

      if (current->ifa_addr == NULL) {
         continue;
      }

      if (current->ifa_addr != NULL && strcmp(current->ifa_name, "lo") != 0) {
         if (current->ifa_addr->sa_family==AF_INET) {
            // This is an IP4 address
            output[index] = ((struct sockaddr_in *) current->ifa_addr)->sin_addr;
            index++;
         }
      }

      current = current->ifa_next;
   }

   freeifaddrs(addresses);

   *ip4ads = output;
   *ip4count = count;

   return 0;
}

static int receive_gateway(int index)
{
   int error;
   unsigned long ipv4;
   unsigned short port;
   unsigned short streams;

   error = socket_receivefully(serverfd, (unsigned char*) &ipv4, 4);

   if (error != CONNECT_OK) {
      ERROR(1, "Receive of gateway info failed! (%d)", error);
      return CONNECT_ERROR_HANDSHAKE_FAILED;
   }

   error = socket_receivefully(serverfd, (unsigned char*) &port, 2);

   if (error != CONNECT_OK) {
      ERROR(1, "Receive of gateway info failed! (%d)", error);
      return CONNECT_ERROR_HANDSHAKE_FAILED;
   }

   error = socket_receivefully(serverfd, (unsigned char*) &streams, 2);

   if (error != CONNECT_OK) {
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

   if (error != CONNECT_OK) {
      ERROR(1, "Failed to send cluster info header to server!");
      return EMPI_ERR_INTERN;
   }

   error = socket_sendfully(serverfd, (unsigned char *) ip4ads, ip4count * sizeof(struct in_addr));

   if (error != CONNECT_OK) {
      ERROR(1, "Failed to send cluster info data to server!");
      return EMPI_ERR_INTERN;
   }

   return CONNECT_OK;
}

static int receive_gateway_ready_opcode(int socketfd)
{
   int status;
   int opcode;

   status = socket_receivefully(socketfd, (unsigned char *) &opcode, 4);

   if (status == -1) {
      ERROR(1, "Handshake with gateway failed! (%d)", status);
      return CONNECT_ERROR_GATEWAY;
   }

   return CONNECT_OK;
}

static int send_gateway_ready_opcode(int socketfd)
{
   int status;
   int opcode = OPCODE_GATEWAY_READY;

   status = socket_sendfully(socketfd, (unsigned char *) &opcode, 4);

   if (status != CONNECT_OK) {
      ERROR(1, "Handshake with gateway failed! (%d)", status);
      return CONNECT_ERROR_HANDSHAKE_FAILED;
   }

   return CONNECT_OK;
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
         gateway_connections[i].mpi_receive_buffer = create_message_buffer();

         for (s=0;s<gateway_addresses[remoteIndex].streams;s++) {

            INFO(2, "Connecting to gateway stream %d.%d.%d -> index = %d", i, gateway_rank, s, remoteIndex);

            // Create a path to the target gateway.
            status = socket_connect(gateway_addresses[remoteIndex].ipv4, gateway_addresses[remoteIndex].port + s, &socket);

            if (status != CONNECT_OK) {
               ERROR(1, "Failed to connect!");
               return status;
            }

            init_socket_info(&(gateway_connections[i].sockets[s]), socket, TYPE_DATA);

            INFO(1, "Created connection to remote gateway stream %d.%d.%d socket = %d!", i, gateway_rank, s, socket);
         }

      }

      for (i=cluster_rank+1;i<cluster_count;i++) {
         send_gateway_ready_opcode(gateway_connections[i].sockets[0].socketfd);
      }

   } else if (crank < cluster_rank) {
     // I must accept the connection!
     remoteIndex = crank*gateway_count + gateway_rank;

     gateway_connections[crank].stream_count = gateway_addresses[remoteIndex].streams;
     gateway_connections[crank].mpi_receive_buffer = create_message_buffer();

     for (s=0;s<gateway_addresses[remoteIndex].streams;s++) {

        INFO(2, "Accepting from gateway stream %d.%d.%d -> index = %d", crank, gateway_rank, s, remoteIndex);

        // Create a path to the target gateway.
        status = socket_accept(local_port + s, gateway_addresses[remoteIndex].ipv4, &socket);

        if (status != CONNECT_OK) {
           ERROR(1, "Failed to accept!");
           return status;
        }

        init_socket_info(&(gateway_connections[crank].sockets[s]), socket, TYPE_DATA);

        INFO(1, "Accepted connection from remote gateway %d.%d.%d socket = %d!", crank, gateway_rank, s, socket);
     }


     // Once the connection has been established, we must
     // wait until the gateway ready opcode is received.
     // This ensures that the gateway that initiated the
     // connection has finished completely (needed to prevent
     // race conditions during connectin setup).
     receive_gateway_ready_opcode(gateway_connections[crank].sockets[0].socketfd);
   }

   return CONNECT_OK;
}

static int set_gateway_to_nonblocking(int index)
{
   int i, status;

   DEBUG(1, "Adding gateway %d to epoll", index);

   // We do not need to add a connection to ourselves!
   if (index == cluster_rank) {
      return 0;
   }

   for (i=0;i<gateway_connections[index].stream_count;i++) {
      status = set_socket_non_blocking(gateway_connections[index].sockets[i].socketfd);

      if (status != EMPI_SUCCESS) {
         ERROR(1, "Failed to set socket to non-blocking mode!");
         return status;
      }

//      status = add_socket_to_epoll(gateway_connections[index].sockets[i].socketfd, &(gateway_connections[index].sockets[i]));

//      if (status != EMPI_SUCCESS) {
//         ERROR(1, "Failed to add socket to epoll set!");
//         return status;
//      }
   }

   return 0;
}

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

   for (i=0;i<cluster_count;i++) {
      status = connect_to_gateways(i, local_port);

      if (status != CONNECT_OK) {
         ERROR(1, "Failed to connect to gateway %d (error=%d)", i, status);
         return status;
      }
   }

   for (i=0;i<cluster_count;i++) {
      status = set_gateway_to_nonblocking(i);

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

   mpi_messages = NULL;

   INFO(1, "Initializing master gateway");

   // Create an fd for polling. Not needed on non-gateway nodes, so on these nodes we close it later.
//   epollfd = epoll_create1(0);

   // Create a queue for messages being send to or received from the server.
   server_mpi_receive_buffer = create_message_buffer();

   //server_queue_in = linked_queue_create();

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

   status = get_server_address();

   if (status != CONNECT_OK) {
      WARN(1, "Failed to retrieve server address!");
      return EMPI_ERR_INTERN;
   }

   INFO(1, "I am master gateway in cluster %s. Server at %s %d", cluster_name, server_name, server_port);

   // At least this last process should be a gateway, although there may be more.
   // Therefore, we let this process connect to the server to get information on the others.
   status = socket_connect(server_ipv4, server_port, &serverfd);

   if (status != CONNECT_OK) {
      WARN(1, "Failed to connect to server!");
      return EMPI_ERR_INTERN;
   }

   init_socket_info(&server_info, serverfd, TYPE_SERVER);

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
   int status, error, src, dst, ip4count;
   struct in_addr *ip4ads;

   // I am one of the gateways.
   INFO(1, "I am one of the gateways -- performing generic gateway init!");

   gateway_start_time = current_time_micros();

   mpi_messages = NULL;

#ifdef DETAILED_TIMING
   write_timing_count = 0;
   read_timing_count = 0;
#endif // DETAILED_TIMING

   // Create an fd for polling.
//   epollfd = epoll_create1(0);

   // Init the message queue.
   //incoming_queue = linked_queue_create();

   // Init the data_buffer queue
   // waiting_mpi_buffers = bounded_array_list_create(MAX_WAITING_BUFFERS);

   // Retrieve the local IPv4 addresses.
   status = get_local_ips(&ip4ads, &ip4count);

   if (status != CONNECT_OK) {
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

      // We are done talking to the server for now. Set the server socket to non-blocking mode for future messages.
      set_socket_non_blocking(server_info.socketfd);
  //    add_socket_to_epoll(server_info.socketfd, &server_info);
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

/*
static int nonblock_read_message(socket_info *info, int *more)
{
   ssize_t tmp, count;
   uint32_t len;

   DEBUG(1, "Reading message from socket");

   // Assume here that we return when we are out of data or encounter an error.
   *more = 0;

   if (info->in == NULL) {
      DEBUG(1, "No message available yet -- will allocate!");
      // Allocate an empty message and read that first.
      info->in = malloc(sizeof(generic_message));
      info->inpos = 0;
      info->incount = sizeof(generic_message);
#ifdef DETAILED_TIMING
      info->in_starttime = current_time_micros();
#endif // DETAILED_TIMING
   }

   count = info->inpos;

   while (count < info->incount) {

      DEBUG(1, "Reading message from socket %d %d %d", count, info->incount, info->incount - count);

      tmp = read(info->socketfd, ((unsigned char *) info->in) + count, info->incount - count);

      DEBUG(1, "Read message from socket %d %d %d", count, info->incount, tmp);

      if (tmp == -1) {
         if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Cannot read any more data. Socket is empty
            info->inpos = (uint32_t) count;
            return EMPI_SUCCESS;
         } else {
            ERROR(1, "Failed to read message from socket! (error=%d)", errno);
            return EMPI_ERR_INTERN;
         }
      } else if (tmp == 0) {
         ERROR(1, "Unexpected EOF on socket %d! count=%ld incount=%d", info->socketfd, count, info->incount);
         return EMPI_ERR_INTERN;
      }

      count += tmp;
   }

   // We've either read the entire message, or just the entire header, depending on the size!

   // Sanity check - should not happen!
   if (count < sizeof(generic_message)) {
      ERROR(1, "Read invalid message size %d!", count);
      return EMPI_ERR_INTERN;
   }

    // In all subsequent cases we return when there may still be data on the stream.
   *more = 1;

   if (count > sizeof(generic_message)) {

      DEBUG(1, "Read full message %d", count);

#ifdef DETAILED_TIMING
      store_read_timings(info->in_starttime, current_time_micros(), count);
#endif // DETAILED_TIMING

      // We've read the entire message, so queue it and return.
      message_enqueue(info->in_queue, info->in);

      info->in_bytes += count;
      info->in_messages++;
      info->in = NULL;
      info->inpos = 0;
      info->incount = 0;
      return EMPI_SUCCESS;
   }

   // We've only read a header!
   if (info->type == TYPE_SERVER) {
      len = ntohl(info->in->header.length);
   } else {
      len = info->in->header.length;
   }

   if (len == sizeof(generic_message)) {

      DEBUG(1, "Read SPECIAL message %d %d", len, count);

      // The message only contains a header.
      message_enqueue(info->in_queue, info->in);
//      process_special_message(info->in, order);

      info->in_bytes += count;
      info->in_messages++;
      info->in = NULL;
      info->inpos = 0;
      info->incount = sizeof(generic_message);
      return EMPI_SUCCESS;
   }

   DEBUG(1, "Read PARTIAL message %d %d", count, len);

   // Otherwise, we have to realloc the message, and read more data!
   info->in = realloc(info->in, len);
   info->inpos = count;
   info->incount = len;

   if (info->in == NULL) {
      ERROR(1, "Failed to realloc message!");
      *more = 0;
      return EMPI_ERR_INTERN;
   }

   // Attempt to read the rest of the message before returning...
   return nonblock_read_message(info, more);
}
*/

/*
static void ensure_state_ro(socket_info *info)
{
   if (info->state == STATE_RW) {
      DEBUG(1, "Switching socket %d to RO", info->socketfd);
      set_socket_in_epoll_to_ro(info->socketfd, info);
      info->state = STATE_RO;
   }
}

static void ensure_state_rw(socket_info *info)
{
   if (info->state == STATE_RO) {
      DEBUG(1, "Switching socket %d to RW", info->socketfd);
      set_socket_in_epoll_to_rw(info->socketfd, info);
      info->state = STATE_RW;
   }
}
*/
/*
static message_buffer *grab_partial_buffer(int cluster)
{
   message_buffer *tmp;

   if (cluster < 0) {

      tmp = server_mpi_receive_buffer;

      if (tmp == NULL || tmp->end == 0) {
         return NULL;
      }

      server_mpi_receive_buffer = NULL;

//fprintf(stderr, "Grabbing server message buffer of size %d)\n", tmp->end);

   } else {

      tmp = gateway_connections[cluster].mpi_receive_buffer;

      if (tmp == NULL || tmp->end == 0) {
         return NULL;
      }

      gateway_connections[cluster].mpi_receive_buffer = NULL;

//fprintf(stderr, "Grabbing data message buffer of size %d)\n", tmp->end);
   }

   return tmp;
}
*/
/*
static void release_message_buffer(message_buffer *buffer)
{
   if (buffer != NULL) {
      free(buffer);
   }
}
*/
/*
static bool prepare_message(socket_info *info)
{
   if (info->out != NULL) {
      // Message buffer in progress, keep writing it!
//fprintf(stderr, "prepare_message %d left\n", info->out->end-info->out->start);
      return true;
   }

   // No message in progress so dequeue next message buffer.
   info->out = (message_buffer *) linked_queue_dequeue(info->out_queue);

   if (info->out != NULL) {
//fprintf(stderr, "prepare_message %d dequeued\n", info->out->end-info->out->start);
      return true;
   }

   // Try to grab a partially filled buffer that wasn't queued yet.
   info->out = grab_partial_buffer(info->cluster);

   if (info->out != NULL) {
//fprintf(stderr, "prepare_message %d grabbed\n", info->out->end-info->out->start);
   }

   return (info->out != NULL);

//   info->outpos = 0;
//   if (info->type == TYPE_SERVER) {
//      info->outcount = ntohl(info->out->header.length);
//   } else {
//      info->outcount = info->out->header.length;
//   }
//
//   return false;
}

static void prepare_messages(int index)
{
   int i;
   socket_info *info;

   for (i=0;i<gateway_connections[index].stream_count;i++) {

      info = &(gateway_connections[index].sockets[i]);

      if (prepare_message(info)) {
         ensure_state_rw(info);
      } else {
         ensure_state_ro(info);
      }
   }
}
*/

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
static int nonblock_write_message(socket_info *info, int *more) {

   message_buffer *buffer;
   ssize_t tmp;

   // SANITY CHECK
   if (info->state != STATE_RW) {
      ERROR(1, "inconsistent state in nonblock_write_message!");
   }

   if (!prepare_message(info)) {
      // We have run out of messages to write!
      DEBUG(1, "No more messages to write to socket %d -- switching to RO mode!", info->socketfd);
      *more = 0;
      ensure_state_ro(info);
      return EMPI_SUCCESS;
   } else {
#ifdef DETAILED_TIMING
      info->out_starttime = current_time_micros();
#endif // DETAILED_TIMING
   }

   buffer = info->out;

   while (buffer->start < buffer->end) {

      DEBUG(1, "Writing message(s) to socket %d %d %d\n", info->socketfd, buffer->start, buffer->end);

      tmp = write(info->socketfd, &(buffer->data[buffer->start]), buffer->end-buffer->start);

      if (tmp == -1) {
         if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Cannot write any more data. Socket is full.
            *more = 0;
            return EMPI_SUCCESS;
         } else {
            *more = 0;
            ERROR(1, "Failed to write message to socket! (error=%d)", errno);
            return EMPI_ERR_INTERN;
         }
      }

      DEBUG(1, "Written %d bytes to socket %d\n", tmp, info->socketfd);

      buffer->start += tmp;
   }

   DEBUG(1, "Finished writing message to socket %d %d %d", info->socketfd, count, info->outcount);

   // We've finished writing the message!
   info->out_bytes += buffer->end;
   info->out_messages += buffer->messages;

   release_message_buffer(info->out);
   info->out = NULL;

#ifdef DETAILED_TIMING
   store_write_timings(info->out_starttime, current_time_micros(), count);
#endif // DETAILED_TIMING
//   info->outpos   = 0;
//   info->outcount = 0;
   *more = 1;

   // Update the pending data counts.
//   if (info->type == TYPE_DATA) {
//      pending_data_messages--;
//      pending_data_size -= count;
//   }

   return EMPI_SUCCESS;
}
*/

/*
static int nonblock_read_messages(socket_info *info)
{
   int more, error, count;

   count = 0;

   do {
      more = 0;
      error = nonblock_read_message(info, &more);
      count++;
   } while (more == 1 && error == MPI_SUCCESS && count < MAX_SOCKETS_READ_SEQUENCE);

   return error;
}

static int nonblock_write_messages(socket_info *info)
{
   int more, error;

   do {
      more = 0;
      error = nonblock_write_message(info, &more);
   } while (more == 1 && error == MPI_SUCCESS);

   return error;
}

static int handle_socket_event(uint32_t events, socket_info *info)
{
   int error;

   if ((events & EPOLLERR) || (events & EPOLLHUP)) {
      // We've received an error on the socket!
      ERROR(1, "Unexpected socket error!");
      return EMPI_ERR_INTERN;
   }

   if (events & EPOLLOUT) {
      error = nonblock_write_messages(info);

      if (error != 0) {
         ERROR(1, "Unexpected socket error (write)!");
         return EMPI_ERR_INTERN;
      }
   }

   if (events & EPOLLIN) {
      error = nonblock_read_messages(info);

      if (error != 0) {
         ERROR(1, "Unexpected socket error (read)!");
         return EMPI_ERR_INTERN;
      }
   }

   return EMPI_SUCCESS;
}

// Process at most one incoming message from each gateway.
static int poll_socket_event(int *done, int *progress)
{
   int n, i, status;
   struct epoll_event events[MAX_EVENTS];

   n = epoll_wait (epollfd, events, MAX_EVENTS, 0);

   if (n == 0) {
      *progress = 0;
      return MPI_SUCCESS;
   }

   *progress = 1;

   DEBUG(1, "Got %d socket events", n);

   for (i=0;i<n;i++) {
      status = handle_socket_event(events[i].events, events[i].data.ptr);

      if (status != EMPI_SUCCESS) {
         return status;
      }
   }

   return EMPI_SUCCESS;
}
*/

static int nonblock_write_message(message_buffer *buffer, int socketfd, bool *wouldblock, int *written_data) {

   ssize_t avail, tmp;

   avail = buffer->end - buffer->start;

   while (avail > 0) {

//fprintf(stderr, "Writing message(s) to socket %d %d %d %ld\n", socketfd, buffer->start, buffer->end, avail);

      tmp = write(socketfd, &(buffer->data[buffer->start]), avail);

      if (tmp == -1) {

//fprintf(stderr, "Written no bytes to socket %d (would block)\n", socketfd);

         if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Cannot write any more data. Socket is full.
            *wouldblock = true;
            return EMPI_SUCCESS;
         } else {
            ERROR(1, "Failed to write message to socket! %d %d %d %d (error=%d)", socketfd, buffer->start, buffer->end, avail, errno);
            return EMPI_ERR_INTERN;
         }
      }

//fprintf(stderr, "Written %ld bytes to socket %d\n", tmp, socketfd);

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

static inline void reset_message_buffer(message_buffer *buffer)
{
   buffer->start = buffer->end = 0;
}

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


static int nonblock_read_message(message_buffer *buffer, int socketfd, bool *wouldblock, int *read_data)
{
   size_t avail, tmp;

   avail = buffer->size - buffer->end;

//if (avail <= 0) {
//  fprintf(stderr, "NOT Reading message from socket %d avail=0\n", socketfd);
//}

   while (avail > 0) {

//fprintf(stderr, "Reading message from socket %d available space %ld %d %d %d\n", socketfd, avail, buffer->start, buffer->end, buffer->size);

      tmp = read(socketfd, &(buffer->data[buffer->end]), avail);

      if (tmp == -1) {
         if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Cannot read any more data. Socket is empty

//fprintf(stderr, "Read NO message from socket %d (would block)\n", socketfd);

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

//fprintf(stderr, "Read message from socket %d size %ld\n", socketfd, tmp);

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

static int process_gateway_message(generic_message *m, int *done)
{
   // NOTE: server messages are external and therefore in network byte order!
   int opcode = ntohl(m->header.opcode);

   DEBUG(1, "Received gateway message with opcode %d", opcode);

   if (opcode != OPCODE_FINALIZE_REPLY) {
      ERROR(1, "Failed to process gateway message with unknown opcode %d!", opcode);
      return EMPI_ERR_INTERN;
   }

   INFO(1, "Gateway receive FINALIZE message from server!");

   *done = 1;

   return EMPI_SUCCESS;
}

// NOTE: Assiumption here is that "generic_message *m" points to a shared buffer, and
// we therefore need to copy any data that is cannot be reused after this call returns!!
static int forward_mpi_message(generic_message *m, int pid, int len, int tag)
{
   int cluster, rank, error;
   mpi_message *mpi_msg;

   cluster = GET_CLUSTER_RANK(pid);
   rank = GET_PROCESS_RANK(pid);

   DEBUG(1, "Forwarding message to MPI %d:%d tag=%d len-%d", cluster, rank, tag, len);

   if (cluster != cluster_rank) {
      ERROR(1, "Received message for wrong cluster! dst=%d me=%d!", cluster, cluster_rank);
      return EMPI_ERR_INTERN;
   }

   mpi_msg = create_mpi_message(m, len);

   if (mpi_msg == NULL) {
      ERROR(1, "Failed to allocate MPI request!");
      return EMPI_ERR_INTERN;
   }

   error = PMPI_Isend(&(mpi_msg->data[0]), len, MPI_BYTE, rank, tag, mpi_comm_gateway_and_application, &(mpi_msg->r));

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to perform Isend! (error=%d)", error);
      free(mpi_msg);
      return TRANSLATE_ERROR(error);
   }

   mpi_msg->next = mpi_messages;
   mpi_messages = mpi_msg;

   return EMPI_SUCCESS;
}

static int poll_mpi_requests()
{
   MPI_Status status;
   int flag, error;

   mpi_message *curr;
   mpi_message *tmp;
   mpi_message *prev;

   prev = NULL;
   curr = mpi_messages;

   while (curr != NULL) {

      error = PMPI_Test(&(curr->r), &flag, &status);

      if (error != MPI_SUCCESS) {
         ERROR(1, "Failed to test status of MPI_Isend! (error=%d)", error);
         return TRANSLATE_ERROR(error);
      }

      if (flag) {
         // The operation has completed, so delete message and element!
         tmp = curr;

         if (prev == NULL) {
            // The first operation has finished!
            mpi_messages = mpi_messages->next;
            curr = mpi_messages;
         } else {
            prev->next = curr->next;
            curr = curr->next;
         }

         free(tmp);

      } else {
         // The operation has not completed, so move to next element!
         prev = curr;
         curr = curr->next;
      }
   }

   return EMPI_SUCCESS;
}

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

/*
static int process_socket_messages(int *done)
{
   int i, status, progress, count;

   count = 0;

   do {
      if (gateway_rank == 0) {
         // Prepare a server request for sending into the server connection (if possible).
         if (prepare_message(&server_info)) {
            ensure_state_rw(&server_info);
         } else {
            ensure_state_ro(&server_info);
         }
      }

      // For each gateway connection, prepare a pending data message for sending (if available).
      for (i=0;i<cluster_count;i++) {
         if (i != cluster_rank) {
            prepare_messages(i);
         }
      }

      // DEBUG(1, "Poll socket event");

      progress = 0;

      status = poll_socket_event(done, &progress);

      if (status != EMPI_SUCCESS) {
         ERROR(1, "Failed to poll for messages!");
      }

      count++;

   } while (progress == 1 && *done != 1 && count < MAX_SOCKETS_RECEIVE_SEQUENCE);

   return status;
}
*/
/*
static int enqueue_data_message(generic_message *message, int size)
{
   // NOTE: data messages are internal and therefore in host byte order!
   int success, target;

   // Up the global message and data count.
   pending_data_messages++;
   pending_data_size += size;

   target = GET_CLUSTER_RANK(message->header.dst_pid);

   if (array_queue_enqueue(gateway_connections[target].out_queue, message)) {
      return EMPI_SUCCESS;
   } else {
      ERROR(1, "Failed to enqueue message!");
      return EMPI_ERR_INTERN;
   }
}
*/

/*
static int enqueue_server_request(generic_message *message, int size)
{
   // NOTE: server messages are external and therefore in network byte order!
   int error;

   if (gateway_rank != 0) {
      ERROR(1, "Cannot forward message to server, as I am not gateway 0!");
      return EMPI_ERR_INTERN;
   }

   DEBUG(1, "Enqueue request for server %d %d %d", message->header.length, ntohl( message->header.length), size);

   error = array_queue_enqueue(server_queue_out, message);

   if (error == -1) {
      ERROR(1, "Failed to enqueue message!");
      return EMPI_ERR_INTERN;
   }

   return EMPI_SUCCESS;
}
*/

/*
static int receive_mpi_server_request(int *received)
{
   int error, flag, count;
   MPI_Status status;
   unsigned char *buffer;

   error = PMPI_Iprobe(MPI_ANY_SOURCE, TAG_SERVER_REQUEST, mpi_comm_gateway_and_application, &flag, &status);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to probe MPI! (error=%d)", error);
      return TRANSLATE_ERROR(error);
   }

   if (!flag) {
      *received = 0;
      return EMPI_SUCCESS;
   }

   *received = 1;

   DEBUG(1, "Incoming MPI SERVER REQUEST!");

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

   DEBUG(2, "Receiving MPI SERVER REQUEST from source %d, with tag %d and size %d", status.MPI_SOURCE, status.MPI_TAG, count);

   // NOTE: Blocking receive should NOT block, as the probe already told us a message is waiting.
   error = PMPI_Recv(buffer, count, MPI_BYTE, status.MPI_SOURCE, TAG_SERVER_REQUEST, mpi_comm_gateway_and_application, MPI_STATUS_IGNORE);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to receive after probe! source=%d tag=%d count=%d (error=%d)", status.MPI_SOURCE, status.MPI_TAG, count, error);
      return TRANSLATE_ERROR(error);
   }

   DEBUG(2, "Received MPI SERVER REQUEST! %d %d", ntohl(((generic_message *)buffer)->header.opcode), ntohl(((generic_message *)buffer)->header.length));

   // This is a request from an application process to forward a request to the server.
   return enqueue_server_request((generic_message *)buffer, count);
}
*/
/*
static int receive_mpi_server_reply(int *done, int *received)
{
   int error, flag, count;
   MPI_Status status;
   unsigned char *buffer;

   error = PMPI_Iprobe(MPI_ANY_SOURCE, TAG_SERVER_REPLY, mpi_comm_gateway_and_application, &flag, &status);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to probe MPI! (error=%d)", error);
      return TRANSLATE_ERROR(error);
   }

   if (!flag) {
      *received = 0;
      return EMPI_SUCCESS;
   }

   *received = 1;

   DEBUG(1, "Incoming MPI SERVER REPLY!");

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

   DEBUG(2, "Receiving MPI SERVER REPLY from source %d, with tag %d and size %d", status.MPI_SOURCE, status.MPI_TAG, count);

   // NOTE: Blocking receive should NOT block, as the probe already told us a message is waiting.
   error = PMPI_Recv(buffer, count, MPI_BYTE, status.MPI_SOURCE, TAG_SERVER_REPLY, mpi_comm_gateway_and_application, MPI_STATUS_IGNORE);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to receive after probe! source=%d tag=%d count=%d (error=%d)", status.MPI_SOURCE, status.MPI_TAG, count, error);
      return TRANSLATE_ERROR(error);
   }

   DEBUG(2, "Received MPI SERVER REPLY! %d %d", ntohl(((generic_message *)buffer)->header.opcode), ntohl(((generic_message *)buffer)->header.length));

   // This is a message from the server forwarded to me via MPI by the master gateway.
   DEBUG(2, "Received SERVER reply via MPI!");
   return process_gateway_message((generic_message *)buffer, done);
}
*/
/*
static int receive_mpi_data_message(int *received)
{
   int error, flag, count;
   MPI_Status status;
   unsigned char *buffer;

   error = PMPI_Iprobe(MPI_ANY_SOURCE, TAG_DATA_MSG, mpi_comm_gateway_and_application, &flag, &status);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to probe MPI! (error=%d)", error);
      return TRANSLATE_ERROR(error);
   }

   if (!flag) {
      *received = 0;
      return EMPI_SUCCESS;
   }

   *received = 1;

   DEBUG(1, "Incoming MPI DATA message!");

   error = PMPI_Get_count(&status, MPI_BYTE, &count);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to receive size of MPI data message! (error=%d)", error);
      return TRANSLATE_ERROR(error);
   }

   buffer = malloc(count);

   if (buffer == NULL) {
      ERROR(1, "Failed to allocate space for MPI data message! (error=%d)", error);
      return EMPI_ERR_INTERN;
   }

   DEBUG(2, "Receiving MPI data message from source %d, with tag %d and size %d", status.MPI_SOURCE, status.MPI_TAG, count);

   // NOTE: Blocking receive should NOT block, as the probe already told us a message is waiting.
   error = PMPI_Recv(buffer, count, MPI_BYTE, status.MPI_SOURCE, TAG_DATA_MSG, mpi_comm_gateway_and_application, MPI_STATUS_IGNORE);

   if (error != MPI_SUCCESS) {
      DEBUG(1, "Failed to receive after probe! source=%d tag=%d count=%d (error=%d)", status.MPI_SOURCE, status.MPI_TAG, count, error);
      return TRANSLATE_ERROR(error);
   }

   DEBUG(2, "Received MPI DATA message! %d %d", ((generic_message *)buffer)->header.opcode, ((generic_message *)buffer)->header.length);

   // This is a request from an application process to forward data to another cluster.
   return enqueue_data_message((generic_message *)buffer, count);
}
*/


// Forward all data messages received from other gateways to their destinations using MPI.
/*
static int forward_data_messages_to_mpi()
{
   int len, pid, error;
   generic_message *m;

   m = (generic_message *) linked_queue_dequeue(incoming_queue);

   while (m != NULL) {

      len = m->header.length;
      pid = m->header.dst_pid;

      error = forward_mpi_message(m, pid, len, TAG_FORWARDED_DATA_MSG);

      if (error != MPI_SUCCESS) {
         ERROR(1, "Failed to forward message to MPI! (error=%d)", error);
         return error;
      }

      m = (generic_message *) linked_queue_dequeue(incoming_queue);
   }

   return EMPI_SUCCESS;
}

// Forward all server messages received from the server to their destinations using MPI.
static int forward_server_messages_to_mpi(int *done)
{
   int len, pid, error;
   generic_message *m;

   m = (generic_message *) linked_queue_dequeue(server_queue_in);

   while (m != NULL) {

      len = ntohl(m->header.length);
      pid = ntohl(m->header.dst_pid);

      if (pid == my_pid) {
          // This message is indented for me!
         error = process_gateway_message(m, done);

         if (error != MPI_SUCCESS) {
            ERROR(1, "Failed to process gateway message! (error=%d)", error);
            return error;
         }
      } else {
         error = forward_mpi_message(m, pid, len, TAG_SERVER_REPLY);

         if (error != MPI_SUCCESS) {
            ERROR(1, "Failed to forward message to MPI! (error=%d)", error);
            return error;
         }
      }

      m = (generic_message *) linked_queue_dequeue(server_queue_in);
   }

   return EMPI_SUCCESS;
}
*/


/*
// Process the MPI messages.
static int process_mpi_messages(int *done)
{
   int error, receivedRequest, receivedReply, receivedData, count;

   // First process all available server requests and replies.
   do {
      receivedRequest = 0;

      error = receive_mpi_server_request(&receivedRequest);

      if (error != EMPI_SUCCESS) {
         ERROR(1, "Failed to receive from MPI! (error=%d)", error);
         return error;
      }

      receivedReply = 0;

      error = receive_mpi_server_reply(&receivedReply, done);

      if (error != EMPI_SUCCESS) {
         ERROR(1, "Failed to receive from MPI! (error=%d)", error);
         return error;
      }

      error = forward_server_messages_to_mpi(done);

      if (error != EMPI_SUCCESS) {
         ERROR(1, "Failed to forward MPI server messages! (error=%d)", error);
         return error;
      }

   } while ((receivedRequest + receivedReply) > 0 && *done != 1);

   // Next, attempt to push data messages into MPI.
   error = forward_data_messages_to_mpi();

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to forward MPI data messages! (error=%d)", error);
      return error;
   }

   // Next, see if any pending send messages have finished.
   error = poll_mpi_requests();

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to poll MPI requests! (error=%d)", error);
      return error;
   }

   if (*done == 1) {
      return MPI_SUCCESS;
   }

   count = 0;
   receivedData = 1;

   // Finally, receive new data from MPI to push into sockets,
   // provided that/ we are not running out of buffer space.
   while (pending_data_messages < MAX_PENDING_DATA_MSG &&
          pending_data_size < MAX_PENDING_DATA_SIZE &&
          receivedData == 1 &&
          count < MAX_MPI_RECEIVE_SEQUENCE) {

      receivedData = 0;

      error = receive_mpi_data_message(&receivedData);

      if (error != EMPI_SUCCESS) {
         ERROR(1, "Failed to receive from MPI! (error=%d)", error);
         return error;
      }

      count++;
   }

   return EMPI_SUCCESS;
}
*/

/*
static int poll_pending_receive(int *done, int *received)
{
   MPI_Status status;
   int flag, error;

   error = PMPI_Test(&pending_receive_request, &flag, &status);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to test status of MPI_Isend! (error=%d)", error);
      return TRANSLATE_ERROR(error);
   }

   if (flag) {
      DEBUG(2, "Received MPI message! %d %d", pending_receive_buffer->header.opcode, pending_receive_buffer->header.length);

      *received = 1;
      pending_receive = 0;

      switch (status.MPI_TAG) {
      case TAG_DATA_MSG:
         // This is a request from an application process to forward data to another cluster.
         return enqueue_data_message(pending_receive_buffer, pending_receive_count);
      case TAG_SERVER_REPLY:
         // This is a message from the server forwarded to me via MPI by the master gateway.
         return process_gateway_message(pending_receive_buffer, done);
      case TAG_SERVER_REQUEST:
         // This is a request from an application process to forward a request to the server.
         return enqueue_server_request(pending_receive_buffer, pending_receive_count);
      default:
         ERROR(1, "Received MPI message from source %d, with UNKNOWN tag %d and size %d", status.MPI_SOURCE, status.MPI_TAG, pending_receive_count);
         return EMPI_ERR_INTERN;
      }
    } else {
       *received = 0;
       return EMPI_SUCCESS;
    }
}
*/

//static message_buffer *dequeue_mpi_receive_buffer()
//{
//   return (message_buffer *) bounded_array_list_dequeue(waiting_mpi_buffers);
//}

/*
static void enqueue_mpi_receive_buffer(int cluster)
{
   array_queue *q;
   message_buffer *tmp;

   if (cluster == -1) {
      q = server_queue_out;
      tmp = server_mpi_receive_buffer;
      server_mpi_receive_buffer = NULL;
   } else {
      q = gateway_connections[cluster].out_queue;
      tmp = gateway_connections[cluster].mpi_receive_buffer;
      gateway_connections[cluster].mpi_receive_buffer = NULL;
   }

   if (tmp == NULL) {
      return;
   }

   if (!array_queue_enqueue(q, tmp)) {
      ERROR(1, "Failed to enqueue message buffer for cluster %d", cluster);
      return;
   }
}
*/
/*
static data_buffer *acquire_mpi_receive_buffer(int cluster, int required_bytes, bool *filled_buffer)
{
   message_buffer *tmp;

   if (cluster == -1) {
      tmp = server_mpi_receive_buffer;
   } else {
      tmp = gateway_connections[cluster].mpi_receive_buffer;
   }

   if (tmp != NULL) {

      if (required_bytes <= tmp->size - tmp->end) {
         // Current buffer has enough space.
         return tmp;
      }

      // Current buffer is too small, so replace it.
      if (tmp->end > 0) {
         // Current buffer contains data, so enqueue for sending.
         filled_buffer = true;
         enqueue_mpi_receive_buffer(cluster);
      } else {
         // Current buffer is empty, so discard it. FIXME: instead reuse later?
         free(tmp);
      }
   }

   // Increase buffer size if needed
   if (required_bytes < STANDARD_BUFFER_SIZE) {
      required_bytes = STANDARD_BUFFER_SIZE;
   }

   tmp = malloc(sizeof(data_buffer) + required_bytes);

   if (tmp == NULL) {
      ERROR(1, "Failed to create receive buffer of size %d", required_bytes);
      return NULL;
   }

   tmp->size = required_bytes;
   tmp->messages = 0;
   tmp->start = 0;
   tmp->end = 0;

   if (cluster == -1) {
      server_mpi_receive_buffer = tmp;
   } else {
      gateway_connections[cluster].mpi_receive_buffer = tmp;
   }

   return tmp;
}
*/

/*
static message_buffer *check_mpi_receive_buffer(message_buffer *current, int required_bytes, linked_queue *out_queue, bool *stored_buffer)
{
   message_buffer *tmp;

   if (current != NULL) {

      if (required_bytes <= (current->size - current->end)) {
         // Current buffer has enough space.
         return current;
      }

      // Current buffer is too small, so replace it. Enqueue current if it contains any data.
      if (current->end > 0) {
          linked_queue_enqueue(out_queue, current);
         *stored_buffer = true;
      } else {
         // Current buffer is empty, so discard it. FIXME: instead reuse later?
         release_message_buffer(current);
      }
   }

   // Increase buffer size if needed
   if (required_bytes < STANDARD_BUFFER_SIZE) {
      required_bytes = STANDARD_BUFFER_SIZE;
   }

//fprintf(stderr, "Alloc message_buffer of size %ld (%ld + %ld)\n", sizeof(message_buffer) + required_bytes, sizeof(message_buffer), required_bytes);

   tmp = malloc(sizeof(message_buffer) + required_bytes);

   if (tmp == NULL) {
      ERROR(1, "Failed to create receive buffer of size %d", required_bytes);
      return NULL;
   }

   tmp->size = required_bytes;
   tmp->messages = 0;
   tmp->start = 0;
   tmp->end = 0;

   return tmp;
}
*/
/*
static void release_mpi_receive_buffer(int cluster, bool *filled_buffer)
{
   message_buffer *tmp;

   if (cluster == -1) {
      tmp = server_mpi_receive_buffer;
   } else {
      tmp = gateway_connections[cluster].mpi_receive_buffer;
   }

   if (tmp == NULL) {
      return;
   }

   if (tmp->end == tmp->size) {
      enqueue_mpi_receive_buffer(cluster);
      filled_buffer = true;
   }

}
*/

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

/*
static int receive_data_message(int cluster, int count, MPI_Status *status, bool *stored_buffer)
{
   gateway_connections[cluster].mpi_receive_buffer = check_mpi_receive_buffer(gateway_connections[cluster].mpi_receive_buffer, count, gateway_connections[cluster].out_queue, stored_buffer);
   return receive_message(count, status, gateway_connections[cluster].mpi_receive_buffer);
}

//static int receive_server_request(int count, MPI_Status *status, bool *stored_buffer)
//{
//}

/*
static int receive_server_reply(int count, MPI_Status *status, int *done)
{
   int error;

   unsigned char *buffer = malloc(count);

   if (buffer == NULL) {
      ERROR(1, "Failed to allocate space for MPI server message!");
      return EMPI_ERR_INTERN;
   }

   DEBUG(2, "Receiving MPI server reply from source %d, with tag %d and size %d", status->MPI_SOURCE, status->MPI_TAG, count);

   // NOTE: Blocking receive should NOT block, as the probe already told us a message is waiting -- IS THIS TRUE???
   error = PMPI_Recv(buffer, count, MPI_BYTE, status->MPI_SOURCE, status->MPI_TAG, mpi_comm_gateway_and_application, MPI_STATUS_IGNORE);

   if (error != MPI_SUCCESS) {
      DEBUG(1, "Failed to receive MPI server message! source=%d tag=%d count=%d (error=%d)", status->MPI_SOURCE, status->MPI_TAG, count, error);
      return TRANSLATE_ERROR(error);
   }

   // This is a message from the server forwarded to me via MPI by the master gateway.
   return process_gateway_message((generic_message *)buffer, done);
}

/*
static int receive_server_message(int count, MPI_Status *status, int *done)
{
   int error;

   unsigned char *buffer = malloc(count);

   if (buffer == NULL) {
      ERROR(1, "Failed to allocate space for MPI server message!");
      return EMPI_ERR_INTERN;
   }

   DEBUG(2, "Receiving MPI server message from source %d, with tag %d and size %d", status->MPI_SOURCE, status->MPI_TAG, count);

   // NOTE: Blocking receive should NOT block, as the probe already told us a message is waiting -- IS THIS TRUE???
   error = PMPI_Recv(buffer, count, MPI_BYTE, status->MPI_SOURCE, status->MPI_TAG, mpi_comm_gateway_and_application, MPI_STATUS_IGNORE);

   if (error != MPI_SUCCESS) {
      DEBUG(1, "Failed to receive MPI server message! source=%d tag=%d count=%d (error=%d)", status->MPI_SOURCE, status->MPI_TAG, count, error);
      return TRANSLATE_ERROR(error);
   }

   if (status->MPI_TAG == TAG_SERVER_REQUEST) {
      // This is a request from an application process to forward a request to the server.
      return enqueue_server_request((generic_message *)buffer, count);
   } else { // status->MPI_TAG == TAG_SERVER_REPLY
      // This is a message from the server forwarded to me via MPI by the master gateway.
      return process_gateway_message((generic_message *)buffer, done);
   }
}
*/

/*
static int receive_server_request(bool *received, bool *stored_buffer)
{
   int error, flag, count;
   MPI_Status status;

   error = PMPI_Iprobe(MPI_ANY_SOURCE, TAG_SERVER_REQUEST, mpi_comm_gateway_and_application, &flag, &status);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to probe MPI for TAG_SERVER_REQUEST! (error=%d)", error);
      return TRANSLATE_ERROR(error);
   }

   if (!flag) {
      return EMPI_SUCCESS;
   }

   DEBUG(1, "Incoming MPI server_request!");

   *received = true;

   error = PMPI_Get_count(&status, MPI_BYTE, &count);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to receive size of MPI data message! (error=%d)", error);
      return TRANSLATE_ERROR(error);
   }

   server_mpi_receive_buffer = check_mpi_receive_buffer(server_mpi_receive_buffer, count, server_queue_out, stored_buffer);

   return receive_message(count, &status, server_mpi_receive_buffer);
}
*/

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

/*
static int receive_data_message(int cluster, bool *received, bool *stored_buffer)
{
   int error, flag, count;
   MPI_Status status;

   error = PMPI_Iprobe(MPI_ANY_SOURCE, TAG_DATA_MSG+cluster, mpi_comm_gateway_and_application, &flag, &status);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to probe MPI for TAG_SERVER_REQUEST! (error=%d)", error);
      return TRANSLATE_ERROR(error);
   }

   if (!flag) {
      return EMPI_SUCCESS;
   }

   DEBUG(1, "Incoming MPI server_request!");

   *received = true;

   error = PMPI_Get_count(&status, MPI_BYTE, &count);

//fprintf(stderr, "Received message from cluster %d count %d\n", cluster, count);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to receive size of MPI data message! (error=%d)", error);
      return TRANSLATE_ERROR(error);
   }

   gateway_connections[cluster].mpi_receive_buffer = check_mpi_receive_buffer(gateway_connections[cluster].mpi_receive_buffer, count, gateway_connections[cluster].out_queue, stored_buffer);

   return receive_message(count, &status, gateway_connections[cluster].mpi_receive_buffer);
}


// Receive ANY MPI messages.
/*
static int receive_any_from_mpi(int *done, bool *received, bool *stored_buffer)
{
   int error, flag, count;
   MPI_Status status;

   error = PMPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, mpi_comm_gateway_and_application, &flag, &status);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to probe MPI! (error=%d)", error);
      return TRANSLATE_ERROR(error);
   }

   if (!flag) {
      *received = false;
      return EMPI_SUCCESS;
   }

   DEBUG(1, "Incoming MPI message!");

   *received = true;

   error = PMPI_Get_count(&status, MPI_BYTE, &count);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to receive size of MPI data message! (error=%d)", error);
      return TRANSLATE_ERROR(error);
   }

   // Check the tag to see what type of message we have received. Three options here:
   //
   // TAG_DATA_MSG ... (TAG_DATA_MSG+cluster_count) - it's a data message
   // TAG_SERVER_REQUEST                            - it's a message for the server
   // TAG_SERVER_REPLY                              - it's a message from the server

   if (status.MPI_TAG >= TAG_DATA_MSG && status.MPI_TAG < (TAG_DATA_MSG+cluster_count)) {
      // This is a request from an application process to forward data to another cluster.
      return receive_data_message(status.MPI_TAG-TAG_DATA_MSG, count, &status, stored_buffer);
   }

   if (status.MPI_TAG == TAG_SERVER_REQUEST) {
      // This is a request for the server from an application process that I should forward.
      // NOTE: only valid on master gateway!!
      if (gateway_rank != 0) {
         ERROR(1, "Cannot forward message to server, as I am not gateway 0!");
         return EMPI_ERR_INTERN;
      }

      return receive_server_request(count, &status, stored_buffer);
   }

   if (status.MPI_TAG == TAG_SERVER_REPLY) {
      // This is a message from the server forwarded to me via MPI by the master gateway.
      return receive_server_reply(count, &status, done);
   }

   // We don't recognize the tag, so complain to the user...
   ERROR(1, "Received MPI message from source %d, with UNKNOWN tag %d and size %d", status.MPI_SOURCE, status.MPI_TAG, count);
   return EMPI_ERR_INTERN;
}
*/

/*
static int receive_from_mpi(int *done)
{
   bool received, stored_buffer;
   int error, count;

   count = 0;

   received = true;
   stored_buffer = false;

   // We keep receiving until run out bufferspace or MPI runs out of messages!
   while (received && !stored_buffer && (*done == 0)) {
      error = receive_any_from_mpi(done, &received, &stored_buffer);

      count++;

      if (error != EMPI_SUCCESS) {
         ERROR(1, "Failed to receive from MPI! (error=%d)", error);
         return error;
      }
   }

//fprintf(stderr, "Receive_from_mpi quit after %d loops %d %d %d\n", count, *done, received, stored_buffer);

   return MPI_SUCCESS;
}
*/

static int receive_mpi_message(message_buffer *buffer, int tag, int *wouldblock, int *nospace, int *received_data, int *messages)
{
   int error, flag, count, space;
   MPI_Status status;

   error = PMPI_Iprobe(MPI_ANY_SOURCE, tag, mpi_comm_gateway_and_application, &flag, &status);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to probe MPI for TAG_SERVER_REQUEST! (error=%d)", error);
      return TRANSLATE_ERROR(error);
   }

   if (!flag) {
      *wouldblock += 1;
      return EMPI_SUCCESS;
   }

   DEBUG(1, "Incoming MPI message for tag %d!", tag);

   error = PMPI_Get_count(&status, MPI_BYTE, &count);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to receive size of MPI message for tag %d! (error=%d)", tag, error);
      return TRANSLATE_ERROR(error);
   }

   space = buffer->size - buffer->end;

   if (count > space) {
      *nospace += 1;
      return EMPI_SUCCESS;
   }

   *received_data += count;
   *messages += 1;

   return receive_message(count, &status, buffer);
}

static int receive_from_mpi(int *done)
{
   int received_data, wouldblock, nospace, messages, error, i, miss, buffered_data;

   message_buffer *buffer;

   messages = 0;
   received_data = 0;
   wouldblock = 0;
   nospace = 0;
   miss = 0;
   buffered_data = 0;

   // We keep receiving until we have received enough data, or all receivers run out of MPI messages or buffer space.
//   while (received_data < MAX_SINGLE_MPI_RECEIVE && ((wouldblock + nospace) < cluster_count-1)) {
//   while (miss < MAX_POLL_MISS_COUNT && received_data < MAX_SINGLE_MPI_RECEIVE) {

   do {

      if ((wouldblock + nospace) == cluster_count-1) {
         miss++;
      } else {
         miss = 0;
      }

      wouldblock = 0;
      nospace = 0;

      for (i=0;i<cluster_count;i++) {
         if (i != cluster_rank) {

            // FIXME: only records the last buffer size!
            buffer = gateway_connections[i].mpi_receive_buffer;

            buffered_data += buffer->end-buffer->start;

            error = receive_mpi_message(buffer, TAG_DATA_MSG+i, &wouldblock, &nospace, &received_data, &messages);

            if (error != EMPI_SUCCESS) {
               ERROR(1, "Failed to receive from MPI! (error=%d)", error);
               return error;
            }
         }
      }

   } while (miss < MAX_POLL_MISS_COUNT && buffered_data < MIN_INTERESTING_BUFFER_SIZE);

//if (received_data > 0) {
//   fprintf(stderr, "Stopped receiving MPI data after %d bytes, wouldblock=%d, nospace=%d messages %d miss=%d buffered_data=%d\n",
//                            received_data, wouldblock, nospace, messages, miss, buffered_data);
//}

   error = receive_server_reply(done);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to receive from MPI! (error=%d)", error);
      return error;
   }

   if (gateway_rank == 0) {
      // The master gateway reads as many server requests as it can.
      nospace = 0;
      wouldblock = 0;
      messages = 0;

      while (nospace == 0 && wouldblock == 0) {
         error = receive_mpi_message(server_mpi_receive_buffer, TAG_SERVER_REQUEST, &nospace, &wouldblock, &received_data, &messages);

         if (error != EMPI_SUCCESS) {
            ERROR(1, "Failed to receive server request from MPI! (error=%d)", error);
            return error;
         }
      }
    }

//      error = receive_any_from_mpi(done, &received, &stored_buffer);


//fprintf(stderr, "Receive_from_mpi count %d done %d received %d stored %d\n", count, *done, received, stored_buffer);

   return MPI_SUCCESS;
}


// Process the MPI messages.
static int process_mpi_messages(int *done)
{
   int error;

   // First, attempt to push data messages into MPI.
//   error = forward_data_messages_to_mpi();

//   if (error != EMPI_SUCCESS) {
//      ERROR(1, "Failed to forward MPI data messages! (error=%d)", error);
//      return error;
//   }

//   if (gateway_rank == 0) {
      // Second, attempt to push server messages into MPI.
//      error = forward_server_messages_to_mpi(done);

//      if (error != EMPI_SUCCESS) {
//         ERROR(1, "Failed to forward MPI server messages! (error=%d)", error);
//         return error;
//      }
//   }

   // Third, receive messages from MPI
   error = receive_from_mpi(done);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to receive MPI messages! (error=%d)", error);
      return error;
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

static void print_gateway_statistics(uint64_t deltat)
{
   int i,j;

   uint64_t sec;
   uint64_t millis;

   sec = deltat / 1000000UL;
   millis = (deltat % 1000000UL) / 1000UL;

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

int messaging_run_gateway(int rank, int size, int empi_size)
{
   int error;
   int done = 0;

#ifdef SIMPLE_TIMING
   uint64_t start, last;
   uint64_t current;
#endif

   pending_data_messages = 0;
   pending_data_size = 0;

#ifdef SIMPLE_TIMING

   start = current_time_micros();
   last = start;

#endif // SIMPLE_TIMING

   fprintf(stderr, "GATEWAY %d.%d starting!\n", cluster_rank, gateway_rank);

   while (done == 0) {

//      fprintf(stderr, "process MPI messages\n");

      error = process_mpi_messages(&done);

      if (error != EMPI_SUCCESS) {
         ERROR(1, "Failed to receive MPI message!");
         return error;
      }

      if (done == 1) {
         break;
      }

//      fprintf(stderr, "process socket messages\n");

      error = process_socket_messages(&done);

      if (error != EMPI_SUCCESS) {
         ERROR(1, "Failed to handle gateway messages!");
         return error;
      }

      if (done == 1) {
         break;
      }

#ifdef SIMPLE_TIMING
      current = current_time_micros();

      // Check if a second has passed
      if ((current - last) > 1000000LU) {
         last = current;
         print_gateway_statistics(current-start);
      }
#endif // SIMPLE_TIMING
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

#ifdef END_TIMING
   current = current_time_micros();

   print_gateway_statistics(current-start);

   printf("GATEWAY %d.%d finished after %ld usec\n", cluster_rank, gateway_rank, (current-start));
#endif // END_TIMING

   error = PMPI_Finalize();

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to finalize MPI! (error=%d)", error);
      return error;
   }

   return EMPI_SUCCESS;
}

