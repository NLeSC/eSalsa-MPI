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
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <errno.h>
#include <limits.h>
#include <fcntl.h>

#include "../../include/settings.h"

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

#define MAX_LENGTH_CLUSTER_NAME 128
#define MAX_STREAMS 16
#define MAX_EVENTS 32

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

   generic_message *in;
   uint32_t inpos;
   uint32_t incount;

   generic_message *out;
   uint32_t outpos;
   uint32_t outcount;
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
   message_queue out_queue;
} gateway_connection;

typedef struct s_mpi_message {
   MPI_Request r;
   struct s_mpi_message *next;
   generic_message *message;
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
static int epollfd;

// The filedescriptor of the socket connected to the 'server'.
static int serverfd = 0;

// The hostname of the server
static char *server_name;
static long server_ipv4;
static unsigned short server_port;

// Socket info containing information on the address of the server.
static socket_info server_info;

static message_queue server_queue_out;

static message_queue server_queue_in;

// Message queue for incoming messages. They are parked here until they can be forwarded.
static message_queue incoming_queue;

// Queue of pending MPI_Isends.
static mpi_message *mpi_messages;

// The PID of this process. Not valid on a gateway process.
extern uint32_t my_pid;

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


static void init_socket_info(socket_info *info, int socketfd)
{
   info->socketfd = socketfd;
   info->in = NULL;
   info->out = NULL;
   info->inpos = 0;
   info->outpos = 0;
   info->incount = 0;
   info->outcount = 0;
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

/*
static int socket_wait_for_data(int socketfd, int blocking)
{
   int    max_sd, result;
   struct timeval timeout;
   fd_set select_set;

   DEBUG(1, "WA_WAIT_FOR_DATA: Waiting for data to appear on socket (blocking=%d)", blocking);

   FD_ZERO(&select_set);
   max_sd = socketfd;
   FD_SET(socketfd, &select_set);

   if (blocking == 0) {
      timeout.tv_sec  = 0;
      timeout.tv_usec = 0;
      result = select(max_sd + 1, &select_set, NULL, NULL, &timeout);
   } else {
      timeout.tv_sec  = WA_SOCKET_TIMEOUT;
      timeout.tv_usec = 0;
      result = select(max_sd + 1, &select_set, NULL, NULL, &timeout);
   }

   DEBUG(1, "WA_WAIT_FOR_DATA: Result is %d", result);

   // Result will be 1 (have data), 0 (no data), -1 (error)
   return result;
}
*/
/*
static int socket_wait_for_opcode(int socketfd, int blocking, int *opcode)
{
   int    max_sd, result;
   struct timeval timeout;
   fd_set select_set;
   size_t len = 4;
   size_t r = 0;
   ssize_t tmp = 0;

   DEBUG(1, "WA_WAIT_FOR_DATA: Waiting for data to appear on socket (blocking=%d)", blocking);

   if (blocking == 0) {

      FD_ZERO(&select_set);
      max_sd = socketfd;
      FD_SET(socketfd, &select_set);
      timeout.tv_sec  = 0;
      timeout.tv_usec = 0;
      result = select(max_sd + 1, &select_set, NULL, NULL, &timeout);

      if (result <= 0) {
         // If we don't have data, or gotten an error we return.
	 return result;
      }
   }

   // If we want a blocking read, or our non-blocking select said we
   // had data, we read a full opcode using a blocking read.
   while (r < len) {
      tmp = read(socketfd, ((unsigned char *) opcode)+r, len-r);

      if (tmp <= 0) {
         return -1;
      } else {
         r += tmp;
      }
   }

   return 1;
}
*/

static int socket_connect(unsigned long ipv4, unsigned short port, int *socketfd)
{
   struct sockaddr_in address;
   socklen_t addrlen;
   int error;
   int connected = 0;
   int attempts = 0;
   int flag = 1;
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

   error = setsockopt(*socketfd, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));

   if (error != 0) {
      close(*socketfd);
      ERROR(1, "Failed to set TCP_NODELAY!");
      return CONNECT_ERROR_OPTIONS;
   }

   return CONNECT_OK;
}

static int socket_accept(unsigned short local_port, uint32_t expected_host, int *socketfd)
{
   int sd, new_socket;
   int error;
   int flag = 1;
   uint32_t host;
   struct sockaddr_in address;
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

//   setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

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

   error = setsockopt(new_socket, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));

   if (error != 0) {
      close(new_socket);
      ERROR(1, "Failed to set TCP_NODELAY!");
      return CONNECT_ERROR_OPTIONS;
   }

   *socketfd = new_socket;

   return CONNECT_OK;
}

/*
static int host_connect(char *host, unsigned short port, int *socketfd)
{
   struct addrinfo *result;
   long ipv4;
   int error;

   // resolve the domain name into a list of addresses
   error = getaddrinfo(host, NULL, NULL, &result);

   if (error != 0) {
      ERROR(1, "getaddrinfo failed: %s", gai_strerror(error));
      return CONNECT_ERROR_SERVER_NOT_FOUND;
   }

   if (result->ai_family == AF_INET) {
      DEBUG(1, "Got inet4");
   } else if (result->ai_family == AF_INET6) {
      ERROR(1, 0, "Got IPv6 address type!");
      return CONNECT_ERROR_SERVER_NOT_FOUND;
   } else {
      ERROR(1, 0, "Got unknown address type!");
      return CONNECT_ERROR_SERVER_NOT_FOUND;
   }

   ipv4 = ((struct sockaddr_in *) result)->sin_addr.s_addr;

   freeaddrinfo(result);

   return socket_connect(ipv4, port, socketfd);
}
*/

static int handshake()
{
//int local_rank, int local_count, int cluster_rank, int cluster_count,
//       char* cluster_name, int *cluster_sizes, int *cluster_offsets)
//{

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


/*
int wa_finalize() {
   int error;

   // Send a close link opcode before shutting down the socket.
   int tmp = htonl(OPCODE_CLOSE_LINK);

   error = wa_sendfully((unsigned char *) &tmp, 4);

   if (error != 0) {
      ERROR(1, "Failed to close link! %d", error);
      return CONNECT_ERROR_CLOSE_FAILED;
   }

   error = shutdown(serverfd, SHUT_RDWR);

   if (error != 0) {
      ERROR(1, "Failed to shutdown socket! %d", error);
      return CONNECT_ERROR_CLOSE_FAILED;
   }

   error = close(serverfd);

   if (error != 0) {
      ERROR(1, "Failed to close socket! %d", error);
      return CONNECT_ERROR_CLOSE_FAILED;
   }

   // TODO: cleanup communicators!

   return CONNECT_OK;
}
*/

/*
static int ensure_byte_order(message_buffer *m, int order)
{
   if (m->byte_order == order) {
      return CONNECT_OK;
   }

   if (order == BYTE_ORDER_HOST && m->byte_order == BYTE_ORDER_NETWORK) {
      m->byte_order = BYTE_ORDER_HOST;

      m->msg.opcode = ntohl(m->msg.opcode);
      m->msg.comm = ntohl(m->msg.comm);
      m->msg.source = ntohl(m->msg.source);
      m->msg.dest = ntohl(m->msg.dest);
      m->msg.tag = ntohl(m->msg.tag);
      m->msg.count = ntohl(m->msg.count);
      m->msg.bytes = ntohl(m->msg.bytes);

      return CONNECT_OK;
   }

   if (order == BYTE_ORDER_NETWORK && m->byte_order == BYTE_ORDER_HOST) {
      m->byte_order = BYTE_ORDER_NETWORK;

      m->msg.opcode = htonl(m->msg.opcode);
      m->msg.comm = htonl(m->msg.comm);
      m->msg.source = htonl(m->msg.source);
      m->msg.dest = htonl(m->msg.dest);
      m->msg.tag = htonl(m->msg.tag);
      m->msg.count = htonl(m->msg.count);
      m->msg.bytes = htonl(m->msg.bytes);

      return CONNECT_OK;
   }

   ERROR(1, "Failed to convert byte order %d to %d", order, m->byte_order);
   return CONNECT_ERROR_BYTE_ORDER;
}
*/

/*

static int wa_wait_for_opcode(int socketfd, int blocking, int *opcode)
{
   int    max_sd, result;
   struct timeval timeout;
   fd_set select_set;
   size_t len = 4;
   size_t r = 0;
   ssize_t tmp = 0;

   DEBUG(1, "WA_WAIT_FOR_DATA: Waiting for data to appear on socket (blocking=%d)", blocking);

   if (blocking == 0) {

      FD_ZERO(&select_set);
      max_sd = socketfd;
      FD_SET(socketfd, &select_set);
      timeout.tv_sec  = 0;
      timeout.tv_usec = 0;
      result = select(max_sd + 1, &select_set, NULL, NULL, &timeout);

      if (result <= 0) {
         // If we don't have data, or gotten an error we return.
	 return result;
      }
   }

   // If we want a blocking read, or our non-blocking select said we
   // had data, we read a full opcode using a blocking read.
   while (r < len) {
      tmp = read(socketfd, ((unsigned char *) opcode)+r, len-r);

      if (tmp <= 0) {
         return -1;
      } else {
         r += tmp;
      }
   }

   return 1;
}

static int receive_opcode(int socketfd, int* opcode, int *error, int blocking)
{
   DEBUG(1, "Receiving from socket (blocking=%d)", blocking);

   int result = wa_wait_for_opcode(socketfd, blocking, opcode);

   DEBUG(1, "Result of receive: result=%d error=%d", result, *error);

   if (result == -1) {
      *error = EMPI_ERR_INTERN;
      return 0;
   }

   if (result == 0) {
      if (blocking == 1) {
         *error = EMPI_ERR_INTERN;
      } else {
         *error = EMPI_SUCCESS;
      }
      return 0;
   }

   *opcode = ntohl(*opcode);
   return 1;
}
*/


/*


   struct ifaddrs * ifAddrStruct=NULL;
    struct ifaddrs * ifa=NULL;
    void * tmpAddrPtr=NULL;

    getifaddrs(&ifAddrStruct);

    for (ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa ->ifa_addr->sa_family==AF_INET) { // check it is IP4
            // is a valid IP4 Address
            tmpAddrPtr=&((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;
            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer); 
        } else if (ifa->ifa_addr->sa_family==AF_INET6) { // check it is IP6
            // is a valid IP6 Address
            tmpAddrPtr=&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;
            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            printf("%s IP Address %s\n", ifa->ifa_name, addressBuffer); 
        } 
    }
    if (ifAddrStruct!=NULL) freeifaddrs(ifAddrStruct);
    return 0;
*/


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
   int remoteIndex, i;
   int status, socket;

   if (crank == cluster_rank) {
      // I must initiate the connection!
      for (i=cluster_rank+1;i<cluster_count;i++) {
         remoteIndex = i*gateway_count + gateway_rank;

         INFO(2, "Connecting to gateway %d/%d -> index = %d", i, gateway_rank, remoteIndex);

         // Create a path to the target gateway.
         status = socket_connect(gateway_addresses[remoteIndex].ipv4, gateway_addresses[remoteIndex].port, &socket);

         if (status != CONNECT_OK) {
            ERROR(1, "Failed to connect to server!");
            return status;
         }

         // FIXME: add parallel streams!
         init_socket_info(&(gateway_connections[i].sockets[0]), socket);
         gateway_connections[i].stream_count = 1;
         message_queue_init(&(gateway_connections[i].out_queue));

         INFO(1, "Created connection to remote gateway %d:%d!", i, gateway_rank);
      }

      for (i=cluster_rank+1;i<cluster_count;i++) {
         send_gateway_ready_opcode(gateway_connections[i].sockets[0].socketfd);
      }

   } else if (crank < cluster_rank) {
     // I must accept the connection!
     remoteIndex = crank*gateway_count + gateway_rank;

     // Create a path to the target gateway.
     status = socket_accept(local_port, gateway_addresses[remoteIndex].ipv4, &socket);

     if (status != CONNECT_OK) {
        ERROR(1, "Failed to connect to server!");
        return status;
     }

     // FIXME: add parallel streams!
     init_socket_info(&(gateway_connections[crank].sockets[0]), socket);
     gateway_connections[crank].stream_count = 1;
     message_queue_init(&(gateway_connections[crank].out_queue));

     INFO(1, "Accepted connection from remote gateway %d!", remoteIndex);

     // Once the connection has been established, we must
     // wait until the gateway ready opcode is received.
     // This ensures that the gateway that initiated the
     // connection has finished completely (needed to prevent
     // race conditions during connectin setup).
     receive_gateway_ready_opcode(gateway_connections[crank].sockets[0].socketfd);
   }

   return CONNECT_OK;
}

static int add_gateway_to_epoll(int index)
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

      status = add_socket_to_epoll(gateway_connections[index].sockets[i].socketfd, &(gateway_connections[index].sockets[i]));

      if (status != EMPI_SUCCESS) {
         ERROR(1, "Failed to add socket to epoll set!");
         return status;
      }
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
      status = add_gateway_to_epoll(i);

      if (status != CONNECT_OK) {
         ERROR(1, "Failed to add gateway %d to epoll (error=%d)", i, status);
         return status;
      }
   }

   return CONNECT_OK;
}

static int disconnect_gateway(int index)
{
   // TODO: add parallel streams!

   if (index != cluster_rank) {
      close(gateway_connections[index].sockets[0].socketfd);
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
   message_queue_init(&server_queue_out);
   message_queue_init(&server_queue_in);

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

   init_socket_info(&server_info, serverfd);

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

   // Create an fd for polling.
   epollfd = epoll_create1(0);

   // Init the message queue.
   message_queue_init(&incoming_queue);

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
      add_socket_to_epoll(server_info.socketfd, &server_info);
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






/*
static int forward_data_message(unsigned char *buffer, int size)
{
   int error, cluster, socketfd;
   message *m;

   m = (message *) buffer;

   cluster = m->dest_cluster;


HIERO



   socketfd = gateway_addresses[cluster*gateway_count + gateway_rank].socket;

   // EEP: blocking send!
   error = socket_sendfully(socketfd, buffer, len);

   if (error != CONNECT_OK) {
      ERROR(1, "Failed to forward data to remote gateway! (error=%d)", error);
      return EMPI_ERR_INTERN;
   }

   return EMPI_SUCCESS;
}

static int handle_gateway_msg(generic_message *message, int size, int *done)
{
   // TODO: implement!!1
   return EMPI_SUCCESS;
}

// Process at most one incoming message from MPI.
static int probe_mpi_message(generic_message **message, int *tag)
{
   int error, flag;
   MPI_Status status;
   unsigned char *buffer;

   error = PMPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, mpi_comm_gateway_and_application, &flag, &status);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to probe MPI! (error=%d)", error);
      return TRANSLATE_ERROR(error);
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
   error = PMPI_Recv(buffer, count, MPI_BYTE, status.SOURCE, status.TAG, mpi_comm_gateway_and_application, MPI_STATUS_IGNORE);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to receive after probe! source=%d tag=%d count=%d (error=%d)", status.source, status.tag, count, error);
      return TRANSLATE_ERROR(error);
   }

   *message = buffer;
   *tag = status.TAG;
   return EMPIP_SUCCESS;
}

static int receive_mpi_message(int *done)
{
   generic_message *m;
   int tag,error;

   m = NULL;

   error = probe_mpi_message(&m, &tag);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to probe MPI! (error=%d)", error);
      return error;
   }

   if (m != NULL) {
      // The tag now detemines what we should to with the message. Forward it to the server, or to one or more clusters...
      switch (tag) {
      case TAG_DATA_MSG:
         return forward_data_msg_mpi_to_sockets(buffer, count);
      case TAG_SERVER_REQUEST:
         return forward_server_msg_mpi_to_sockets(buffer, count);
      case TAG_GATEWAY_REQUEST:
         return handle_gateway_msg(buffer, count, done);
      default:
         ERROR(1, "Received message with unsupported tag %d from %d (size=%d)", status.tag, status.source, count);
         return EMPI_ERR_INTERN;
      }
   }

   return MPI_SUCCESS;
}
*/

static int nonblock_read_message(socket_info *info, message_queue *queue, int order)
{
   ssize_t tmp, count;
   uint32_t len;

   DEBUG(1, "Reading message from socket");

   if (info->in == NULL) {

      DEBUG(1, "No message available yet -- will allocate!");

      // Allocate an empty message and read that first.
      info->in = malloc(sizeof(generic_message));
      info->inpos = 0;
      info->incount = sizeof(generic_message);
   }

   count = info->inpos;

   while (count < info->incount) {

      DEBUG(1, "Reading message from socket %d %d %d", count, info->incount, info->incount - count);

      tmp = read(info->socketfd, ((unsigned char *) info->in) + count, info->incount - count);

      DEBUG(1, "Read message from socket %d %d %d", count, info->incount, tmp);

      if (tmp == -1) {
         if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Cannot write any more data.
            info->inpos = (uint32_t) count;
            return EMPI_SUCCESS;
         } else {
            ERROR(1, "Failed to read message from socket! (error=%d)", errno);
            return EMPI_ERR_INTERN;
         }
      } else if (tmp == 0) {
         ERROR(1, "Unexpected EOF on socket!");
         return EMPI_ERR_INTERN;
      }

      count += tmp;
   }

   // We've either read the entire message, or just the entire header, depending on the size!

   if (count < sizeof(generic_message)) {
      // Sanity check - should not happen!
      ERROR(1, "Read invalid message size %d!", count);
      return EMPI_ERR_INTERN;
   }

   if (count > sizeof(generic_message)) {

      DEBUG(1, "Read full message %d", count);

      // We've read the entire message, so queue it and return.
      message_enqueue(queue, info->in);

      info->in = NULL;
      info->inpos = 0;
      info->incount = 0;

      return EMPI_SUCCESS;
   }

   // We've only read a header!
   if (order == 0) {
      len = ntohl(info->in->header.length);
   } else {
      len = info->in->header.length;
   }

   if (len == sizeof(generic_message)) {

      DEBUG(1, "Read SPECIAL message %d %d", len, count);

      // The message only contains a header.
      message_enqueue(queue, info->in);
//      process_special_message(info->in, order);

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
      return EMPI_ERR_INTERN;
   }

   return nonblock_read_message(info, queue, order);
}

static int nonblock_write_message(socket_info *info)
{
   ssize_t tmp, count;

   if (info->out == NULL) {
      // Sanity check -- nothing to read
      set_socket_in_epoll_to_ro(info->socketfd, info);
      return EMPI_SUCCESS;
   }

   count = info->outpos;

   while (count < info->outcount) {

      DEBUG(1, "Writing message to socket %d %d", count, info->outcount);

      tmp = write(info->socketfd, ((unsigned char *) info->out) + count, info->outcount - count);

      DEBUG(1, "Written %d bytes", tmp);

      if (tmp == -1) {
         if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // Cannot write any more data.
            info->outpos = (uint32_t) count;
            return EMPI_SUCCESS;
         } else {
            ERROR(1, "Failed to write message to socket! (error=%d)", errno);
            return EMPI_ERR_INTERN;
         }
      }

      count += tmp;
   }

   DEBUG(1, "Finished writing message to socket %d  %d", count, info->outcount);

   // We've finished writing the message!
   info->out      = NULL;
   info->outpos   = 0;
   info->outcount = 0;

   set_socket_in_epoll_to_ro(info->socketfd, info);
   return EMPI_SUCCESS;
}

static int handle_socket_event(uint32_t events, socket_info *info)
{
   int error;

   // TODO: end of application will cause storm of errors ?
   if ((events & EPOLLERR) || (events & EPOLLHUP)) {
      // We've received an error on the socket!
      ERROR(1, "Unexpected socket error!");
      return EMPI_ERR_INTERN;
   }

   if (events & EPOLLOUT) {
      error = nonblock_write_message(info);

      if (error != 0) {
         ERROR(1, "Unexpected socket error (write)!");
         return EMPI_ERR_INTERN;
      }
   }

   if (events & EPOLLIN) {

      if (info == &server_info) {
         error = nonblock_read_message(info, &server_queue_in, 0);
      } else {
         error = nonblock_read_message(info, &incoming_queue, 1);
      }

      if (error != 0) {
         ERROR(1, "Unexpected socket error (read)!");
         return EMPI_ERR_INTERN;
      }
   }

   return EMPI_SUCCESS;
}

// Process at most one incoming message from each gateway.
static int poll_socket_event(int *done)
{
   int n, i, status;
   struct epoll_event events[MAX_EVENTS];

   n = epoll_wait (epollfd, events, MAX_EVENTS, 0);

   if (n > 0) {
      DEBUG(1, "Got %d socket events", n);
   }

   for (i=0;i<n;i++) {
      status = handle_socket_event(events[i].events, events[i].data.ptr);

      if (status != EMPI_SUCCESS) {
         return status;
      }
   }

   return EMPI_SUCCESS;
}

static void push_message(socket_info *target, message_queue *queue, int order)
{
   if (target->out != NULL || message_queue_empty(queue)) {
      return;
   }

   target->out = message_dequeue(queue);
   target->outpos = 0;

   if (order == 0) {
      target->outcount = ntohl(target->out->header.length);
   } else {
      target->outcount = target->out->header.length;
   }

   DEBUG(1, "Pushing queue message into socket %d %d!", ntohl(target->out->header.length), target->out->header.length);

   set_socket_in_epoll_to_rw(target->socketfd, target);
}

// Move messages queued for the gateway at 'index' to the queue of a specific socket (if possible).
static void push_messages(int index)
{
   int i;

   for (i=0;i<gateway_connections[index].stream_count;i++) {

      if (message_queue_empty(&(gateway_connections[index].out_queue))) {
         return;
      }

      push_message(&(gateway_connections[index].sockets[i]), &(gateway_connections[index].out_queue), 1);
/*
      info = &(gateway_connections[index].sockets[i]);

      if (info->out == NULL) {
         info->out = message_dequeue(&(gateway_connections[index].out_queue));
         info->outpos = 0;
         info->outcount = info->out->header.length;
         set_socket_in_epoll_to_rw(info->socketfd, info);
      }
*/
   }
}

// Handle the gateway sockets.
static int process_socket_messages(int *done)
{
   int i, status;

   if (gateway_rank == 0) {
      // Attempt to push a pending server request from the queue into the server connection (if possible).
      push_message(&server_info, &server_queue_out, 0);
   }

   // For each gateway connection, push a pending data message into a socket connection (if available).
   for (i=0;i<cluster_count;i++) {
      if (i != cluster_rank) {
         push_messages(i);
      }
   }

//   DEBUG(1, "Poll socket event");

   status = poll_socket_event(done);

   if (status != EMPI_SUCCESS) {
      ERROR(1, "Failed to poll for messages!");
   }

   return status;
}

static int enqueue_data_message(generic_message *message, int size)
{
   // NOTE: data messages are internal and therefore in host byte order!
   int error, target;

   target = GET_CLUSTER_RANK(message->header.dst_pid);

   error = message_enqueue(&(gateway_connections[target].out_queue), message);

   if (error == -1) {
      ERROR(1, "Failed to enqueue message!");
      return EMPI_ERR_INTERN;
   }

   return EMPI_SUCCESS;
}

static int enqueue_server_request(generic_message *message, int size)
{
   // NOTE: server messages are external and therefore in network byte order!
   int error;

   if (gateway_rank != 0) {
      ERROR(1, "Cannot forward message to server, as I am not gateway 0!");
      return EMPI_ERR_INTERN;
   }

   DEBUG(1, "Enqueue request for server %d %d %d", message->header.length, ntohl( message->header.length), size);

   error = message_enqueue(&server_queue_out, message);

   if (error == -1) {
      ERROR(1, "Failed to enqueue message!");
      return EMPI_ERR_INTERN;
   }

   return EMPI_SUCCESS;
}

static int process_gateway_message(generic_message *m, int *done)
{
   // NOTE: server messages are external and therefore in network byte order!
   int opcode;

   opcode = ntohl(m->header.opcode);

   DEBUG(1, "Received gateway message with opcode %d", opcode);

   if (opcode != OPCODE_FINALIZE_REPLY) {
      ERROR(1, "Failed to process gateway message with unknown opcode %d!", opcode);
      return EMPI_ERR_INTERN;
   }

   INFO(1, "Gateway receive FINALIZE message from server!");

   *done = 1;

   return EMPI_SUCCESS;
}

static int receive_mpi_message(int *done)
{
   int error, flag, count;
   MPI_Status status;
   unsigned char *buffer;

   error = PMPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, mpi_comm_gateway_and_application, &flag, &status);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to probe MPI! (error=%d)", error);
      return TRANSLATE_ERROR(error);
   }

   if (!flag) {
      return EMPI_SUCCESS;
   }

   DEBUG(1, "Incoming MPI message!");

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

   DEBUG(2, "Receiving MPI message from source %d, with tag %d and size %d", status.MPI_SOURCE, status.MPI_TAG, count);

   // NOTE: Blocking receive should NOT block, as the probe already told us a message is waiting.
   error = PMPI_Recv(buffer, count, MPI_BYTE, status.MPI_SOURCE, status.MPI_TAG, mpi_comm_gateway_and_application, MPI_STATUS_IGNORE);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to receive after probe! source=%d tag=%d count=%d (error=%d)", status.MPI_SOURCE, status.MPI_TAG, count, error);
      return TRANSLATE_ERROR(error);
   }

   DEBUG(2, "Received MPI message! %d %d", ntohl(((generic_message *)buffer)->header.opcode), ntohl(((generic_message *)buffer)->header.length));

   // The tag now detemines what we should to with the message. Forward it to the server, or to one or more clusters...
   switch (status.MPI_TAG) {
   case TAG_DATA_MSG:
      // This is a request from an application process to forward data to another cluster.
      DEBUG(2, "Forwarding MPI DATA message to remote gateway via socket!");
      return enqueue_data_message((generic_message *)buffer, count);
   case TAG_SERVER_REQUEST:
      // This is a request from an application process to forward a request to the server.
      DEBUG(2, "Forwarding MPI SERVER request to server via socket!");
      return enqueue_server_request((generic_message *)buffer, count);
   case TAG_SERVER_REPLY:
      // This is a message from the server forwarded to me via MPI by the master gateway.
      DEBUG(2, "Received SERVER reply via MPI!");
      return process_gateway_message((generic_message *)buffer, done);
   default:
      ERROR(1, "Received message with unsupported tag %d from %d (size=%d)", status.MPI_TAG, status.MPI_SOURCE, count);
      return EMPI_ERR_INTERN;
   }
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

         free(tmp->message);
         free(tmp);

      } else {
         // The operation has not completed, so move to next element!
         prev = curr;
         curr = curr->next;
      }
   }

   return EMPI_SUCCESS;
}


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

   mpi_msg = malloc(sizeof(mpi_message));

   if (mpi_msg == NULL) {
      ERROR(1, "Failed to allocate MPI request!");
      return EMPI_ERR_INTERN;
   }

   error = PMPI_Isend(m, len, MPI_BYTE, rank, tag, mpi_comm_gateway_and_application, &(mpi_msg->r));

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to perform Isend! (error=%d)", error);
      return TRANSLATE_ERROR(error);
   }

   mpi_msg->message = m;
   mpi_msg->next = mpi_messages;
   mpi_messages = mpi_msg;

   return EMPI_SUCCESS;
}


// Forward all data messages received from other gateways to their destinations using MPI.
static int forward_data_messages_to_mpi()
{
   int len, pid, error;
   generic_message *m;

   m = message_dequeue(&incoming_queue);

   while (m != NULL) {

      len = m->header.length;
      pid = m->header.dst_pid;

      error = forward_mpi_message(m, pid, len, TAG_FORWARDED_DATA_MSG);

      if (error != MPI_SUCCESS) {
         ERROR(1, "Failed to forward message to MPI! (error=%d)", error);
         return error;
      }

      m = message_dequeue(&incoming_queue);
   }

   return EMPI_SUCCESS;
}

// Forward all server messages received from the server to their destinations using MPI.
static int forward_server_messages_to_mpi(int *done)
{
   int len, pid, error;
   generic_message *m;

   m = message_dequeue(&server_queue_in);

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

      m = message_dequeue(&server_queue_in);
   }

   return EMPI_SUCCESS;
}

// Process the MPI messages.
static int process_mpi_messages(int *done)
{
   int error;

   error = poll_mpi_requests();

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to poll MPI requests! (error=%d)", error);
      return error;
   }

   error = forward_server_messages_to_mpi(done);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to forward MPI server messages! (error=%d)", error);
      return error;
   }

   error = forward_data_messages_to_mpi();

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to forward MPI data messages! (error=%d)", error);
      return error;
   }

   error = receive_mpi_message(done);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to receive from MPI! (error=%d)", error);
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
   close(epollfd);
}

int messaging_run_gateway(int rank, int size, int empi_size)
{
   int error;
   int done = 0;

   while (done == 0) {

      error = process_mpi_messages(&done);

      if (error != EMPI_SUCCESS) {
         ERROR(1, "Failed to receive MPI message!");
         return error;
      }

      if (done == 1) {
         break;
      }

      error = process_socket_messages(&done);

      if (error != EMPI_SUCCESS) {
         ERROR(1, "Failed to handle gateway messages!");
         return error;
      }

      if (done == 1) {
         break;
      }
   }

   // We need a barrier to ensure everybody (application and gateway nodes) is ready to finalize.
   // TODO: -- DO WE NEED THIS ??
   error = PMPI_Barrier(MPI_COMM_WORLD);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to wait for temination! (error=%d)", error);
      return error;
   }

   cleanup();

   error = PMPI_Finalize();

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to finalize MPI! (error=%d)", error);
      return error;
   }

   return EMPI_SUCCESS;
}

