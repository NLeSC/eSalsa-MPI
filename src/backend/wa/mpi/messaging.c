#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <errno.h>
#include <limits.h>

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
#include "wa_sockets.h"

#define BYTE_ORDER_UNDEFINED 0
#define BYTE_ORDER_HOST      1
#define BYTE_ORDER_NETWORK   2

// A type to store gateway information.
struct gateway_info {
   unsigned short port;
   char hostname[HOST_NAME_MAX+1];
};

// The filedescriptor of the socket connected to the 'server'.
static int serverfd = 0;

// The location of the server (hostname/ip and port)
static char *server;
static unsigned short port;

// The name of this cluster (must be unique).
static char *cluster_name;
static struct gateway_info *gateways;
static int *gateway_sockets;

// The number of clusters and the rank of our cluster in this set.
uint32_t cluster_count;
uint32_t cluster_rank;

// The number of application processes and gateways in this cluster.
uint32_t local_application_size;
uint32_t gateway_count;

// The gateway rank of this gateway process
static uint32_t gateway_rank;



// The ID of this process. Not valid on a gateway process.
uint32_t my_pid;

// The size of each cluster, and the offset of each cluster in the
// total set of machines.
int *cluster_sizes;
int *cluster_offsets;

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

   server = malloc(strlen(buffer+1));

   if (server == NULL) {
      fclose(config);
      ERROR(1, "Failed to allocate space for server adres %s", buffer);
      return 0;
   }

   strcpy(server, buffer);

   // Read the server port
   error = fscanf(config, "%hu", &port);

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
   server = NULL;
   port = -1;

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
         ERROR(1, "wa_sendfully failed! (%s)", strerror(errno));
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
         ERROR(1, "wa_receivefully failed! (%s)", strerror(errno));
         return CONNECT_ERROR_RECEIVE_FAILED;
      } else {
         r += tmp;
      }
   }

   return CONNECT_OK;
}


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


static message_buffer *receive_message(int blocking, int *error);

static int socket_connect(char *host, unsigned short port, int *socketfd)
{
   struct addrinfo *result;
   int error;
   int flag = 1;

   /* resolve the domain name into a list of addresses */
   error = getaddrinfo(host, NULL, NULL, &result);

   if (error != 0) {
       ERROR(1, "getaddrinfo failed: %s", gai_strerror(error));
       return CONNECT_ERROR_SERVER_NOT_FOUND;
   }

   if (result->ai_family == AF_INET) {
	DEBUG(1, "Got inet4");
   } else if (result->ai_family == AF_INET6) {
	DEBUG(1, "Got inet6");
   } else {
	ERROR(1, 0, "Got unknown address type!");
   }

   *socketfd = socket(AF_INET, SOCK_STREAM, 0);

   if (*socketfd < 0) {
      ERROR(1, "Failed to create socket!");
      return CONNECT_ERROR_CANNOT_CREATE_SOCKET;
   }

   ((struct sockaddr_in *) result->ai_addr)->sin_port = htons(port);

   error = connect(*socketfd, result->ai_addr, result->ai_addrlen);

   if (error != 0) {
      close(*socketfd);
      ERROR(1, "Failed to connect to server!");
      return CONNECT_ERROR_CANNOT_CONNECT;
   }

   error = setsockopt(*socketfd, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int));

   if (error != 0) {
      close(*socketfd);
      ERROR(1, "Failed to set TCP_NODELAY!");
      return CONNECT_ERROR_CANNOT_CONNECT;
   }

   freeaddrinfo(result);

   return CONNECT_OK;
}

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
   unsigned int reply[5];

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

   error = socket_receivefully(serverfd, (unsigned char *)reply, 5*4);

   if (error != CONNECT_OK) {
      ERROR(1, "Handshake with server failed! (%d)", error);
      return CONNECT_ERROR_HANDSHAKE_FAILED;
   }

   if (reply[0] != OPCODE_HANDSHAKE_ACCEPTED) {
      ERROR(1, "Server refused handshake! (%d)", reply[0]);
      return CONNECT_ERROR_HANDSHAKE_FAILED;
   }

   cluster_rank           = ntohl(reply[1]);
   cluster_count          = ntohl(reply[2]);
   local_application_size = ntohl(reply[3]);
   gateway_count          = ntohl(reply[4]);

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

   error = wa_receivefully((unsigned char*) cluster_sizes, cluster_count * 4);

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

static int receive_gateway(int index)
{
   int len, error;

   error = wa_receivefully((unsigned char *) &len, 4);

   if (error != CONNECT_OK) {
      ERROR(1, "Receive of gateway info failed! (%d)", error);
      return CONNECT_ERROR_HANDSHAKE_FAILED;
   }

   error = wa_receivefully((unsigned char *) &(gateways[index].hostname), len);

   if (error != CONNECT_OK) {
      ERROR(1, "Receive of gateway info failed! (%d)", error);
      return CONNECT_ERROR_HANDSHAKE_FAILED;
   }

   error = wa_receivefully((unsigned char*) &(gateways[index].port), 2);

   if (error != CONNECT_OK) {
      ERROR(1, "Receive of gateway info failed! (%d)", error);
      return CONNECT_ERROR_HANDSHAKE_FAILED;
   }

   return CONNECT_OK;
}

static int receive_gateway_info()
{
   int i,j, error;

   for (i=0;i<cluster_count;i++) {
      for (j=0;j<gateway_count;j++) {
         error = receive_gateway(i*gateway_count+j);

         if (error != CONNECT_OK) {
            ERROR(1, "Failed to receive gateway info!");
            return CONNECT_ERROR_HANDSHAKE_FAILED;
         }
      }
   }

   return CONNECT_OK;
}

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


static void free_message(message_buffer *m)
{
   if (m == NULL) {
      return;
   }

   if (m->data_buffer != NULL) {
      free(m->data_buffer);
   }

   free(m);
}

static message_buffer *probe_wa(communicator *c, int source, int tag, int blocking, int *error)
{
   message_buffer *m;

   DEBUG(4, "Probing socket for incoming messages from source=%d tag=%d blocking=%d", source, tag, blocking);

   do {
      m = receive_message(blocking, error);

      if (m == NULL) {
         DEBUG(5, "No message received");
         return NULL;
      }

      if (*error != EMPI_SUCCESS) {
         ERROR(1, "Failed to receive message error=%d", error);
         return NULL;
      }

      DEBUG(5, "Message received from source=%d tag=%d count=%d bytes=%d", m->header.source, m->header.tag, m->header.count, m->header.bytes);

      if (match_message(m, c->handle, source, tag)) {
         // we have a match!
         DEBUG(5, "Match! Returning message");
         return m;
      } else {
         DEBUG(5, "No match. Storing message");
         store_message(m);
      }

   } while (blocking);

   DEBUG(4, "No message received");

   return NULL;
}

static int unpack_message(void *buf, int count, datatype *t, communicator *c,
                           message_buffer *m, EMPI_Status *s)
{
  int error = 0;
  int position = 0;

/*
  This check is wrong! The sendnig and receiving count may differ if the sending and receiveing types differ!
 
  if (m->header.count != count) {
      ERROR(1, "Message size mismatch! (expected %d got %d)",
                  count, m->header.count);

      if (m->header.count < count) {
         // we have received less data
         count = m->header.count;
      }
      // else we have received too much data!
   }
*/

   error = TRANSLATE_ERROR(PMPI_Unpack(m->data_buffer, m->header.bytes, &position, buf,
                        count, t->type, c->comm));

   if (error == EMPI_SUCCESS) {
      set_status(s, m->header.source, m->header.tag, error, t, count, FALSE);
   }

   free_message(m);
   return error;
}

static message_buffer *create_message_buffer()
{
   message_buffer *m = malloc(sizeof(message_buffer));

   if (m == NULL) {
       ERROR(1, "Failed to allocate message header\n");
       return NULL;
   }

   m->byte_order = BYTE_ORDER_UNDEFINED;
   m->data_buffer_size = 0;
   m->data_buffer = NULL;

   return m;
}

static int create_data_buffer(message_buffer *m, int max_size)
{
   m->data_buffer_size = max_size;
   m->data_buffer = malloc(max_size);

   if (m->data_buffer == NULL) {
       ERROR(1, "Failed to allocate message buffer of size %d", max_size);
       return CONNECT_ERROR_ALLOCATE;
   }

   return CONNECT_OK;
}

static message_buffer *create_message(int max_size)
{
   message_buffer *m = create_message_buffer();

   if (m == NULL) {
       return NULL;
   }

   if (create_data_buffer(m, max_size) != CONNECT_OK) {
       free_message(m);
       return NULL;
   }

   return m;
}

static void write_message_header(message_buffer *m, int opcode, int comm, int source, int dest, int tag, int count, int bytes)
{
   m->byte_order = BYTE_ORDER_HOST;

   m->header.opcode = opcode;
   m->header.comm = comm;
   m->header.source = source;
   m->header.dest = dest;
   m->header.tag = tag;
   m->header.count = count;
   m->header.bytes = bytes;
}

static int ensure_byte_order(message_buffer *m, int order)
{
   if (m->byte_order == order) {
      return CONNECT_OK;
   }

   if (order == BYTE_ORDER_HOST && m->byte_order == BYTE_ORDER_NETWORK) {
      m->byte_order = BYTE_ORDER_HOST;

      m->header.opcode = ntohl(m->header.opcode);
      m->header.comm = ntohl(m->header.comm);
      m->header.source = ntohl(m->header.source);
      m->header.dest = ntohl(m->header.dest);
      m->header.tag = ntohl(m->header.tag);
      m->header.count = ntohl(m->header.count);
      m->header.bytes = ntohl(m->header.bytes);

      return CONNECT_OK;
   }

   if (order == BYTE_ORDER_NETWORK && m->byte_order == BYTE_ORDER_HOST) {
      m->byte_order = BYTE_ORDER_NETWORK;

      m->header.opcode = htonl(m->header.opcode);
      m->header.comm = htonl(m->header.comm);
      m->header.source = htonl(m->header.source);
      m->header.dest = htonl(m->header.dest);
      m->header.tag = htonl(m->header.tag);
      m->header.count = htonl(m->header.count);
      m->header.bytes = htonl(m->header.bytes);

      return CONNECT_OK;
   }

   ERROR(1, "Failed to convert byte order %d to %d", order, m->byte_order);
   return CONNECT_ERROR_BYTE_ORDER;
}

static int send_message(message_buffer *m)
{
   int error;
   int size = m->header.bytes;

   if (m->byte_order == BYTE_ORDER_NETWORK) {
      size = ntohl(size);
   } else {
      ensure_byte_order(m, BYTE_ORDER_NETWORK);
   }

   error = wa_sendfully((unsigned char *) &(m->header), MSG_HEADER_SIZE);

   if (error != CONNECT_OK) {
      ERROR(1, "Failed to send message header!");
      return EMPI_ERR_INTERN;
   }

   error = wa_sendfully(m->data_buffer, size);

   if (error != CONNECT_OK) {
      ERROR(1, "Failed to send message body!");
      return EMPI_ERR_INTERN;
   }

   return EMPI_SUCCESS;
}

static int receive_opcode(int* opcode, int *error, int blocking)
{
   DEBUG(1, "Receiving from socket (blocking=%d)", blocking);

   int result = wa_wait_for_opcode(blocking, opcode);

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

static int receive_message_header(message_buffer *m)
{
   // Shift the buffer 4 bytes to skip the opcode.
   unsigned char *tmp = (unsigned char *) &(m->header);
   tmp += 4;

   int error = wa_receivefully(tmp, MSG_HEADER_SIZE-4);

   if (error != CONNECT_OK) {
      ERROR(1, "Failed to receive message header!");
      return error;
   }

   m->header.opcode = htonl(OPCODE_DATA);
   m->byte_order = BYTE_ORDER_NETWORK;
   return CONNECT_OK;
}

static int receive_message_data(message_buffer *m)
{
   int error = wa_receivefully(m->data_buffer, m->header.bytes);

   if (error != CONNECT_OK) {
      ERROR(1, "Failed to receive message data!");
      return error;
   }

   return CONNECT_OK;
}

static message_buffer *receive_data_message(int *error)
{
   message_buffer *m = create_message_buffer();

   if (m == NULL) {
      *error = EMPI_ERR_INTERN;
      return NULL;
   }

   if (receive_message_header(m) != CONNECT_OK) {
      free_message(m);
      *error = EMPI_ERR_INTERN;
      return NULL;
   }

   ensure_byte_order(m, BYTE_ORDER_HOST);

   if (create_data_buffer(m, m->header.bytes) != CONNECT_OK) {
      free_message(m);
      *error = EMPI_ERR_INTERN;
      return NULL;
   }

   if (receive_message_data(m) != CONNECT_OK) {
      free_message(m);
      *error = EMPI_ERR_INTERN;
      return NULL;
   }

   *error = EMPI_SUCCESS;
   return m;
}

static message_buffer *receive_message(int blocking, int *error)
{
   int opcode;

   DEBUG(4, "Receiving message from socket (blocking=%d)", blocking);

   int result = receive_opcode(&opcode, error, blocking);

   DEBUG(4, "Result of receive: result=%d error=%d", result, *error);

   if (result == 0) {
      // Note: error will be set correctly if blocking was true
      return NULL;
   }

   if (opcode == OPCODE_DATA || opcode == OPCODE_COLLECTIVE_BCAST) {
      return receive_data_message(error);
   }

   ERROR(1, "Unexpected message opcode (RM) %d", opcode);
   *error = EMPI_ERR_INTERN;
   return NULL;
}

/*
static int pack_size(int count, datatype *d, int *bytes)
{
   if (count < 0) {
      ERROR(1, "Unexpected message opcode (RM) %d", opcode);
   }

FIXME!
}
*/

static int connect_to_gateway(int index, int *socketfd)
{
   // TODO!!!HIERO

HIERO!!

   return CONNECT_OK;
}

static int connect_gateways()
{
   int i, index, error;
   int socketfd;

   // NOTE: Each gateway connects to -one- other gateway in every other cluster.
   gateway_sockets = malloc(sizeof(int) * cluster_count);
   memset(gateway_sockets, 0, sizeof(int) * cluster_count);

   if (gateway_sockets == NULL) {
      ERROR(1, "Failed to allocate space for gateway sockets!");
      return CONNECT_ERROR_ALLOCATE;
   }

   for (i=0;i<cluster_count;i++) {

      if (i != cluster_rank) {

         index = i*gateway_count + gateway_rank;

         error = connect_to_gateway(index, &socketfd);

         if (error != CONNECT_OK) {
            // TODO: cleanup mess!
            ERROR(1, "Failed to connect to gateway %s:%d!", &(gateways[index].hostname), gateways[index].port);
            return EMPI_ERR_INTERN;
         }

         gateway_sockets[i] = socketfd;
      }
   }

   return CONNECT_OK;
}


int messaging_init(int rank, int size, int *adjusted_rank, int *adjusted_size, int *argc, char ***argv)
{
   int status, error, dest;
   int message[5];

   cluster_count = 0;
   cluster_rank = -1;

   // Clear the message
   message[0] = 0;
   message[1] = 0;
   message[2] = 0;
   message[3] = 0;
   message[4] = 0;

   // Initially, only the last MPI process needs to connect to the server.
   if (rank == size-1) {

      // Read the cluster name and server location from a file.
      status = init_cluster_info(argc, argv);

      if (status == 0) {
         // TODO: tell others ?
         ERROR(1, "Failed to initialize WA sockets implementation!");
         return EMPI_ERR_INTERN;
      }

      if (cluster_name == NULL || server == NULL || port <= 0 || port > 65535) {
         // TODO: tell others ?
         ERROR(1, "Cluster info not correctly set (name=%s server=%d port=%d)!", cluster_name, server, port);
         return EMPI_ERR_INTERN;
      }

      INFO(1, "I am %d of %d in cluster %s", rank, size, cluster_name);
      INFO(1, "Server at %s %d", server, port);

      // At least this last process should be a gateway, although there may be more.
      // Therefore, we let this process connect to the server to get information on the others.

      status = socket_connect(server, port, &serverfd);

      if (status != CONNECT_OK) {
         // TODO: tell others!
         ERROR(1, "Failed to connect to server!");
         return EMPI_ERR_INTERN;
      }

      status = handshake();

      if (status != CONNECT_OK) {
         // TODO: tell the others!
         ERROR(1, "Failed to perform handshake with server!");
         close(serverfd);
         return EMPI_ERR_INTERN;
      }

      if ((local_application_size + gateway_count) != size) {
         // TODO: tell others ?
         ERROR(1, "Cluster size mismatch. Expected %d + %d, but got %d!", local_application_size, gateway_count, size);
         return EMPI_ERR_INTERN;
      }

      message[0] = 1;
      message[1] = cluster_count;
      message[2] = cluster_rank;
      message[3] = local_application_size;
      message[4] = gateway_count;

      error = PMPI_Bcast(message, 5, MPI_INT, size-1, MPI_COMM_WORLD);

      if (error != MPI_SUCCESS) {
         ERROR(1, "Failed to send broadcast of cluster info! (error=%d)", error);
         close(serverfd);
         return EMPI_ERR_INTERN;
      }

      cluster_sizes = malloc(cluster_count * sizeof(int));
      cluster_offsets = malloc((cluster_count+1) * sizeof(int));

      if (cluster_sizes == NULL || cluster_offsets == NULL) {
         ERROR(1, "Failed to allocate space for cluster info!");
         close(serverfd);
         return EMPI_ERR_INTERN;
      }

      status = receive_cluster_sizes();

      if (status != CONNECT_OK) {
         // TODO: tell the others!
         ERROR(1, "Failed to receive cluster sizes!");
         close(serverfd);
         return EMPI_ERR_INTERN;
      }

      error = PMPI_Bcast(cluster_sizes, cluster_count, MPI_INT, size-1, MPI_COMM_WORLD);

      if (error != MPI_SUCCESS) {
         ERROR(1, "Failed to send broadcast of cluster sizes! (error=%d)", error);
         close(serverfd);
         return EMPI_ERR_INTERN;
      }

      error = PMPI_Bcast(cluster_offsets, cluster_count+1, MPI_INT, size-1, MPI_COMM_WORLD);

      if (error != MPI_SUCCESS) {
         ERROR(1, "Failed to send broadcast of cluster offsets! (error=%d)", error);
         close(serverfd);
         return EMPI_ERR_INTERN;
      }

      gateways = malloc(sizeof(struct gateway_info) * cluster_count * gateway_count);

      if (gateways == NULL) {
         ERROR(1, "Failed to allocate space for gateway info!");
         return CONNECT_ERROR_HANDSHAKE_FAILED;
      }

      memset(gateways, 0, sizeof(struct gateway_info) * cluster_count * gateway_count);

      status = receive_gateway_info();

      if (status != CONNECT_OK) {
         // TODO: tell the others!
         ERROR(1, "Failed to receive cluster sizes!");
         close(serverfd);
         return EMPI_ERR_INTERN;
      }

      gateway_rank = rank-local_application_size;

      if (gateway_count > 1) {
         // Send the gateway info to the other gateways in this cluster.
         for (dest=local_application_size;dest<size-1;dest++) {
            error = PMPI_Send(gateways, sizeof(struct gateway_info) * cluster_count * gateway_count, MPI_BYTE, dest, 0, MPI_COMM_WORLD);

            if (error != MPI_SUCCESS) {
               ERROR(1, "Failed to send gateway info to other gateways! (error=%d)", error);
               close(serverfd);
               return EMPI_ERR_INTERN;
            }
         }
      }

   } else {

      // Wait for the braodcast that informs us of the cluster info.
      error = PMPI_Bcast(message, 5, MPI_INT, size-1, MPI_COMM_WORLD);

      if (error != MPI_SUCCESS) {
         ERROR(1, "Failed to receive broadcast of cluster info! (error=%d)", error);
         return EMPI_ERR_INTERN;
      }

      if (message[0] != 1) {
         ERROR(1, "Gateway failed to connect to server!");
         return EMPI_ERR_INTERN;
      }

      cluster_count = message[1];
      cluster_rank = message[2];
      local_application_size = message[3];
      gateway_count = message[4];

      cluster_sizes = malloc(cluster_count * sizeof(int));
      cluster_offsets = malloc((cluster_count+1) * sizeof(int));

      if (cluster_sizes == NULL || cluster_offsets == NULL) {
         // TODO: tell others
         ERROR(1, "Failed to allocate space for cluster info!");
         return EMPI_ERR_INTERN;
      }

      error = PMPI_Bcast(cluster_sizes, cluster_count, MPI_INT, size-1, MPI_COMM_WORLD);

      if (error != MPI_SUCCESS) {
         ERROR(1, "Failed to receive broadcast of cluster sizes! (error=%d)", error);
         return EMPI_ERR_INTERN;
      }

      error = PMPI_Bcast(cluster_offsets, cluster_count+1, MPI_INT, size-1, MPI_COMM_WORLD);

      if (error != MPI_SUCCESS) {
         ERROR(1, "Failed to receive broadcast of cluster offsets! (error=%d)", error);
         return EMPI_ERR_INTERN;
      }

      if (rank >= local_application_size) {
         // We are an additional gateway!
         gateway_rank = rank-local_application_size;

         gateways = malloc(sizeof(struct gateway_info) * cluster_count * gateway_count);

         if (gateways == NULL) {
            ERROR(1, "Failed to allocate space for gateway info!");
            return CONNECT_ERROR_HANDSHAKE_FAILED;
         }

         // Receive the gateway info from the initial gateway in this cluster.
         error = PMPI_Recv(gateways, sizeof(struct gateway_info) * cluster_count * gateway_count, MPI_BYTE, size-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

         if (error != MPI_SUCCESS) {
            ERROR(1, "Failed to send gateway info to other gateways! (error=%d)", error);
            return EMPI_ERR_INTERN;
         }
      }
   }

   *adjusted_size = local_application_size;

   if (rank >= local_application_size) {
      *adjusted_rank = -1;

      INFO(1, "I am gateway process %d of %d in cluster %d of %d", *adjusted_rank, *adjusted_size, cluster_rank, cluster_count);

      // Now connect gateway processes to each other. This assumes a direct connection is possible!
      error = connect_gateways();

      if (error != CONNECT_OK) {
         ERROR(1, "Failed to connect gateways! (error=%d)", error);
         return EMPI_ERR_INTERN;
      }

   } else {
      *adjusted_rank = rank;
      my_pid = SET_PID(cluster_rank, rank);
      INFO(1, "I am application process %d of %d in cluster %d of %d", *adjusted_rank, *adjusted_size, cluster_rank, cluster_count);
   }

   return EMPI_SUCCESS;
}

int messaging_finalize()
{
   return wa_finalize();
}

int messaging_run_gateway(int rank, int size, int empi_size)
{
   // Should not be used!
   return EMPI_ERR_INTERN;
}

static int do_send(int opcode, void* buf, int count, datatype *t, int dest, int tag, communicator* c)
{
   // We have already checked the various parameters, so all we have to so is send the lot!
   int bytes, error;

   // FIXME ??
   error = TRANSLATE_ERROR(PMPI_Pack_size(count, t->type, c->comm, &bytes));

   if (error != EMPI_SUCCESS) {
      return error;
   }

   message_buffer *m = create_message(bytes);

   if (m == NULL) {
      return EMPI_ERR_INTERN;
   }

   bytes = 0;

   // FIXME ??
   error = TRANSLATE_ERROR(PMPI_Pack(buf, count, t->type, m->data_buffer, m->data_buffer_size, &bytes, c->comm));

   write_message_header(m, opcode, c->handle, c->global_rank, dest, tag, count, bytes);

   if (error == EMPI_SUCCESS) {
      error = send_message(m);
   }

   free_message(m);
   return error;
}

int messaging_send(void* buf, int count, datatype *t, int dest, int tag, communicator* c)
{
   return do_send(OPCODE_DATA, buf, count, t, dest, tag, c);
}

int messaging_bcast(void* buf, int count, datatype *t, int root, communicator* c)
{
   return do_send(OPCODE_COLLECTIVE_BCAST, buf, count, t, root, BCAST_TAG, c);
}

int messaging_bcast_receive(void *buf, int count, datatype *t, int root, communicator* c)
{
   return messaging_receive(buf, count, t, root, BCAST_TAG, EMPI_STATUS_IGNORE, c);
}

int messaging_receive(void *buf, int count, datatype *t,
              int source, int tag, EMPI_Status *status, communicator* c)
{
   int error = EMPI_SUCCESS;

   message_buffer *m = find_pending_message(c, source, tag);

   while (m == NULL && error == EMPI_SUCCESS) {
      m = probe_wa(c, source, tag, 1, &error);
   }

   if (error == EMPI_SUCCESS) {
      error = unpack_message(buf, count, t, c, m, status);
   }

   return error;
}

int messaging_probe_receive(request *r, int blocking)
{
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
      r->message = probe_wa(r->c, r->source_or_dest, r->tag, blocking, &(r->error));
   } while (blocking && r->message == NULL && r->error == EMPI_SUCCESS);

   if (r->message != NULL || r->error != EMPI_SUCCESS) {
      r->flags |= REQUEST_FLAG_COMPLETED;
   }

   return EMPI_SUCCESS;
}

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

static int queue_pending_messages(int *next_opcode)
{
   int opcode;
   int error = 0;
   int result = 0;

   message_buffer *m;

   while (result == 0) {

      DEBUG(1, "Start blocking receive for opcode");

      result = receive_opcode(&opcode, &error, 1);

      if (result == 0) {
         ERROR(1, "Failed to receive opcode! (%d)", error);
         return error;
      }

      DEBUG(1, "Result of receive result=%d error=%d", result, error);

      if (opcode == OPCODE_DATA || opcode == OPCODE_COLLECTIVE_BCAST) {
         // There is a message blocking the stream!
         m = receive_data_message(&error);

         if (m == NULL) {
            ERROR(1, "Failed to receive message!");
            return error;
         }

         DEBUG(1, "Message received from src: %d opcode: %d tag: %d", m->header.source, opcode, m->header.tag);

         store_message(m);
         result = 0;
      } else {
         DEBUG(1, "Received non-message opcode %d", opcode);
         *next_opcode = opcode;
         result = 1;
      }
   }

   return EMPI_SUCCESS;
}

static int *alloc_and_receive_int_array(int len)
{
   int i, error;
   int *tmp = malloc(len * sizeof(int));

   if (tmp == NULL) {
      ERROR(1, "Failed to allocate int buffer!");
      return NULL;
   }

   error = wa_receivefully((unsigned char *)tmp, len * sizeof(int));

   if (error != CONNECT_OK) {
      ERROR(1, "Failed to receive int buffer!");
      free(tmp);
      return NULL;
   }

   for (i=0;i<len;i++) {
      tmp[i] = ntohl(tmp[i]);
   }

   return tmp;
}

int messaging_comm_split_receive(comm_reply *reply)
{
   // Since operations on communicators are collective operations, we can
   // assume here that the reply has not be received yet. There is a chance,
   // however, that other messages are on the stream before our reply message.
   int opcode;

   int error = queue_pending_messages(&opcode);

   if (error != EMPI_SUCCESS) {
      return error;
   }

   if (opcode != OPCODE_COMM_REPLY) {
      ERROR(1, "unexpected message opcode (RC) %d", opcode);
      return EMPI_ERR_INTERN;
   }

   error = wa_receivefully((unsigned char *) reply, COMM_REPLY_SIZE);

   if (error != CONNECT_OK) {
      ERROR(1, "Failed to receive comm reply!");
      return EMPI_ERR_INTERN;
   }

   reply->comm = ntohl(reply->comm);
   reply->src = ntohl(reply->src);
   reply->newComm = ntohl(reply->newComm);
   reply->rank = ntohl(reply->rank);
   reply->size = ntohl(reply->size);
   reply->color = ntohl(reply->color);
   reply->key = ntohl(reply->key);
   reply->cluster_count = ntohl(reply->cluster_count);
   reply->flags = ntohl(reply->flags);

   DEBUG(1, "*Received comm reply (comm=%d src=%d newComm=%d rank=%d size=%d color=%d key=%d cluster_count=%d flag=%d)", 
           reply->comm, reply->src, reply->newComm, reply->rank, reply->size, reply->color, reply->key, reply->cluster_count, reply->flags);

   reply->coordinators = alloc_and_receive_int_array(reply->cluster_count);

   if (reply->coordinators == NULL) {
      ERROR(1, "Failed to allocate or receive coordinators");
      return EMPI_ERR_INTERN;
   }

   reply->cluster_sizes = alloc_and_receive_int_array(reply->cluster_count);

   if (reply->cluster_sizes == NULL) {
      ERROR(1, "Failed to allocate or receive cluster sizes");
      return EMPI_ERR_INTERN;
   }

   reply->cluster_ranks = alloc_and_receive_int_array(reply->cluster_count);

   if (reply->cluster_ranks == NULL) {
      ERROR(1, "Failed to allocate or receive cluster ranks");
      return EMPI_ERR_INTERN;
   }

   if (reply->size > 0) {
      reply->members = (uint32_t *) alloc_and_receive_int_array(reply->size);

      if (reply->members == NULL) {
         ERROR(1, "Failed to allocate or receive communicator members");
         return EMPI_ERR_INTERN;
      }

      reply->member_cluster_index = (uint32_t *) alloc_and_receive_int_array(reply->size);

      if (reply->member_cluster_index == NULL) {
         ERROR(1, "Failed to allocate or receive communicator member cluster index");
         return EMPI_ERR_INTERN;
      }

      reply->local_ranks = (uint32_t *) alloc_and_receive_int_array(reply->size);

      if (reply->local_ranks == NULL) {
         ERROR(1, "Failed to allocate or receive communicator member local ranks");
         return EMPI_ERR_INTERN;
      }
   }

   return EMPI_SUCCESS;
}

int messaging_comm_split_send(communicator* c, int color, int key)
{
   comm_request req;

   req.opcode = htonl(OPCODE_COMM);
   req.comm = htonl(c->handle);
   req.src = htonl(c->global_rank);
   req.color = htonl(color);
   req.key = htonl(key);

   int error = wa_sendfully((unsigned char *) &req, COMM_REQUEST_SIZE);

   if (error != CONNECT_OK) {
      ERROR(1, "Failed to send comm request!");
      return EMPI_ERR_INTERN;
   }

   return EMPI_SUCCESS;
}

int messaging_comm_create_send(communicator* c, group *g)
{
   int i;
   group_request req;

   req.opcode = htonl(OPCODE_GROUP);
   req.comm = htonl(c->handle);
   req.src = htonl(c->global_rank);
   req.size = htonl(g->size);

   int error = wa_sendfully((unsigned char *) &req, GROUP_REQUEST_SIZE);

   if (error != CONNECT_OK) {
      ERROR(1, "Failed to send comm request!");
      return EMPI_ERR_INTERN;
   }

   for (i=0;i<g->size;i++) {
      g->members[i] = htonl(g->members[i]);
   }

   error = wa_sendfully((unsigned char *) g->members, g->size * sizeof(uint32_t));

   for (i=0;i<g->size;i++) {
      g->members[i] = ntohl(g->members[i]);
   }

   if (error != CONNECT_OK) {
      ERROR(1, "Failed to send comm request!");
      return EMPI_ERR_INTERN;
   }

   return EMPI_SUCCESS;
}

int messaging_comm_create_receive(group_reply *reply)
{
   // Since operations on communicators are collective operations, we can
   // assume here that the reply has not be received yet. There is a chance,
   // however, that other messages are on the stream before our reply message.
   int opcode;

   int error = queue_pending_messages(&opcode);

   if (error != EMPI_SUCCESS) {
      return error;
   }

   if (opcode != OPCODE_GROUP_REPLY) {
      ERROR(1, "unexpected message opcode (RG) %d", opcode);
      return EMPI_ERR_INTERN;
   }

DEBUG(1, "*Receiving group reply %lu %lu %lu %lu", sizeof(group_reply), sizeof(unsigned char *), sizeof(group_reply)-sizeof(unsigned char *), GROUP_REPLY_SIZE);

   error = wa_receivefully((unsigned char *) reply, GROUP_REPLY_SIZE);

   if (error != CONNECT_OK) {
      ERROR(1, "INTERNAL ERROR: Failed to receive group reply!");
      return EMPI_ERR_INTERN;
   }

   reply->comm = ntohl(reply->comm);
   reply->src = ntohl(reply->src);
   reply->newComm = ntohl(reply->newComm);
   reply->rank = ntohl(reply->rank);
   reply->size = ntohl(reply->size);
   reply->type = ntohl(reply->type);
   reply->cluster_count = ntohl(reply->cluster_count);
   reply->flags = ntohl(reply->flags);

DEBUG(1, "*Received group reply (comm=%d src=%d newComm=%d rank=%d size=%d type=%d cluster_count=%d flags=%d)", reply->comm, reply->src, reply->newComm, reply->rank, reply->size, reply->type, reply->cluster_count, reply->flags);

   if (reply->type == GROUP_TYPE_ACTIVE) {

      reply->coordinators = alloc_and_receive_int_array(reply->cluster_count);

      if (reply->coordinators == NULL) {
         ERROR(1, "Failed to allocate or receive coordinators");
         return EMPI_ERR_INTERN;
      }

      reply->cluster_sizes = alloc_and_receive_int_array(reply->cluster_count);

      if (reply->cluster_sizes == NULL) {
         ERROR(1, "Failed to allocate or receive cluster sizes");
         return EMPI_ERR_INTERN;
      }

      reply->cluster_ranks = alloc_and_receive_int_array(reply->cluster_count);

      if (reply->cluster_ranks == NULL) {
         ERROR(1, "Failed to allocate or receive cluster ranks");
         return EMPI_ERR_INTERN;
      }

      if (reply->size > 0) {
         reply->members = (uint32_t *) alloc_and_receive_int_array(reply->size);

         if (reply->members == NULL) {
            ERROR(1, "Failed to allocate or receive communicator members");
            return EMPI_ERR_INTERN;
         }

         reply->member_cluster_index = (uint32_t *) alloc_and_receive_int_array(reply->size);

         if (reply->member_cluster_index == NULL) {
            ERROR(1, "Failed to allocate or receive communicator member cluster index");
            return EMPI_ERR_INTERN;
         }

         reply->local_ranks = (uint32_t *) alloc_and_receive_int_array(reply->size);

         if (reply->local_ranks == NULL) {
            ERROR(1, "Failed to allocate or receive communicator member local ranks");
            return EMPI_ERR_INTERN;
         }
      }
   }

   return EMPI_SUCCESS;
}

int messaging_comm_dup_send(communicator* c)
{
   dup_request req;

   req.opcode = htonl(OPCODE_DUP);
   req.comm = htonl(c->handle);
   req.src = htonl(c->global_rank);

   int error = wa_sendfully((unsigned char *) &req, DUP_REQUEST_SIZE);

   if (error != CONNECT_OK) {
      ERROR(1, "Failed to send dup request!");
      return EMPI_ERR_INTERN;
   }

   return EMPI_SUCCESS;
}

int messaging_comm_free_send(communicator* c)
{
   terminate_request req;

   req.opcode = htonl(OPCODE_TERMINATE);
   req.comm = htonl(c->handle);
   req.src = htonl(c->global_rank);

   int error = wa_sendfully((unsigned char *) &req, TERMINATE_REQUEST_SIZE);

   if (error != CONNECT_OK) {
      ERROR(1, "Failed to send terminate request!");
      return EMPI_ERR_INTERN;
   }

   return EMPI_SUCCESS;
}


int messaging_comm_dup_receive(dup_reply *reply)
{
   // Since operations on communicators are collective operations, we can
   // assume here that the reply has not be received yet. There is a chance,
   // however, that other messages are on the stream before our reply message.
   int opcode;

   int error = queue_pending_messages(&opcode);

   if (error != EMPI_SUCCESS) {
      return error;
   }

   if (opcode != OPCODE_DUP_REPLY) {
      ERROR(1, "INTERNAL ERROR: unexpected message opcode (RD) %d", opcode);
      return EMPI_ERR_INTERN;
   }

   error = wa_receivefully((unsigned char *) reply, DUP_REPLY_SIZE);

   if (error != CONNECT_OK) {
      ERROR(1, "INTERNAL ERROR: Failed to receive group reply!");
      return EMPI_ERR_INTERN;
   }

   reply->comm = ntohl(reply->comm);
   reply->src = ntohl(reply->src);
   reply->newComm = ntohl(reply->newComm);

DEBUG(1, "*Received dup reply (comm=%d src=%d newComm=%d)", reply->comm, reply->src, reply->newComm);

   return EMPI_SUCCESS;
}

