#include "flags.h"
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>

#include "wa_sockets.h"
#include "util.h"
#include "logging.h"

// The filedescriptor of the socket connected to the 'server'
static int socketfd = 0;

// The location of the server (hostname/ip and port)
static char *server;
static unsigned short port;

// The name of this cluster (must be unique).
static char *cluster_name;

// The number of clusters and the rank of our cluster in this set.
uint32_t cluster_count;
uint32_t cluster_rank;
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
      WARN(0, "EMPI_CONFIG not set");
      // FIXME -- hardcoded path :-P
      file = "/home/jason/empi.config";
   }


   INFO(0, "looking for config file %s", file);

   config = fopen(file, "r");

   if (config == NULL) {
      ERROR(1, "Failed to open config file %s", file);
      return 0;
   }

   INFO(0, "config file %s opened.", file);

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

   error = fscanf(config, "%hu", &port);

   if (error == EOF || error == 0) {
      fclose(config);
      ERROR(1, "Failed to read server port from %s", file);
      return 0;
   }

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

   error = fscanf(config, "%d", &cluster_rank);

   if (error == EOF || error == 0) {
      fclose(config);
      ERROR(1, "Failed to read cluster rank from %s", file);
      return 0;
   }

   error = fscanf(config, "%d", &cluster_count);

   if (error == EOF || error == 0) {
      fclose(config);
      ERROR(1, "Failed to read cluster rank from %s", file);
      return 0;
   }

   fclose(config);
   return 1;
}

static int init_cluster_info(int *argc, char ***argv)
{
   int i, changed, cnt, len;

   server = NULL;
   port = -1;

   cluster_name = NULL;
   cluster_count = 0;
   cluster_rank = -1;

   // First try to read the configuration from an input file whose location is set in the environment.
   // This is needed since Fortran does not pass the command line arguments to the MPI library like C does.
   if (read_config_file() != 1) {
      return 0;
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

      } else if ( strcmp((*argv)[i],"--wa-cluster-rank") == 0 ) {
         if ( i+1 < *argc ){
            cluster_rank = atoi((*argv)[i+1]);
            DELETE_ARG;
         } else {
            ERROR(1, "Missing option for --wa-cluster-rank");
            return 0;
         }
         DELETE_ARG;

      } else if ( strcmp((*argv)[i],"--wa-cluster-count") == 0 ) {
         if ( i+1 < *argc ){
            cluster_count = atoi((*argv)[i+1]);
            DELETE_ARG;
         } else {
            ERROR(1, 0, "Missing option for --wa-cluster-rank");
            return 0;
         }
         DELETE_ARG;
      }

      if ( !changed ) i++;
   }

   if (server == NULL || port <= 0) {
      ERROR(1, "WA server not (correctly) set (%s %d)!", server, port);
      return 0;
   }

   INFO(1, "WA server at %s %d", server, port);

//   if (local_rank < 0 || local_count <= 0 || local_rank >= local_count) {
//      ERROR(1, "Local cluster info not set correctly (%d, %d)!", local_rank, local_count);
//      return 0;
//   }

   if (cluster_name == NULL || cluster_rank < 0 || cluster_count <= 0 || cluster_rank >= cluster_count) {
      ERROR(1, "Cluster info not set correctly (%s, %d, %d)!", cluster_name, cluster_rank, cluster_count);
      return 0;
   }

   cluster_sizes = malloc(cluster_count * sizeof(int));
   cluster_offsets = malloc((cluster_count+1) * sizeof(int));

   return 1;
}

static int wa_connect(char *server, unsigned short port)
{
   struct addrinfo *result;
   int error;
   int flag = 1;

   /* resolve the domain name into a list of addresses */
   error = getaddrinfo(server, NULL, NULL, &result);

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

   socketfd = socket(AF_INET, SOCK_STREAM, 0);

   if (socketfd < 0) {
      ERROR(1, "Failed to create socket!");
      return CONNECT_ERROR_CANNOT_CREATE_SOCKET;
   }

   ((struct sockaddr_in *) result->ai_addr)->sin_port =  htons(port);

   error = connect(socketfd, result->ai_addr, result->ai_addrlen);

   if (error != 0) {
      close(socketfd);
      ERROR(1, "Failed to connect to server!");
      return CONNECT_ERROR_CANNOT_CONNECT;
   }

   error = setsockopt(socketfd, IPPROTO_TCP, TCP_NODELAY, (char *) &flag, sizeof(int)); 

   if (error != 0) {
      close(socketfd);
      ERROR(1, "Failed to set TCP_NODELAY!");
      return CONNECT_ERROR_CANNOT_CONNECT;
   }

   freeaddrinfo(result);

   return CONNECT_OK;
}

static int handshake(int local_rank, int local_count, int cluster_rank, int cluster_count,
       char* cluster_name, int *cluster_sizes, int *cluster_offsets)
{
   // A handshake consist of a single RPC that sends all information:
   // about this process to the server and gets a OK/ERROR as a reply.
   int error,i;

   // The maximum size of the handshake message is 
   //  (6*4 + MAX_LENGTH_CLUSTER_NAME) bytes
   unsigned char message[6*4+MAX_LENGTH_CLUSTER_NAME];
   unsigned int *message_i; 

   message_i = (unsigned int *) message;

   memset(message, 0, 6*4+MAX_LENGTH_CLUSTER_NAME);

   message_i[0] = htonl(OPCODE_HANDSHAKE);
   message_i[1] = htonl(local_rank);
   message_i[2] = htonl(local_count);
   message_i[3] = htonl(cluster_rank);
   message_i[4] = htonl(cluster_count);
   message_i[5] = htonl(strlen(cluster_name));

   strcpy((char*) (message+(6*4)), cluster_name);

   error = wa_sendfully(message, 6*4+strlen(cluster_name));

   if (error != CONNECT_OK) {
      ERROR(1, "Handshake with server failed! (%d)", error);
      return CONNECT_ERROR_HANDSHAKE_FAILED;
   }

   error = wa_receivefully(message, 1);

   if (error != CONNECT_OK) {
      ERROR(1, "Handshake with server failed! (%d)", error);
      return CONNECT_ERROR_HANDSHAKE_FAILED;
   }

   if (message[0] != OPCODE_HANDSHAKE_ACCEPTED) { 
      ERROR(1, "Server refused handshake! (%d)", message[0]);
      return CONNECT_ERROR_HANDSHAKE_FAILED;
   }

//   cluster_sizes = malloc(cluster_count * sizeof(int));
//   cluster_offsets = malloc((cluster_count+1) * sizeof(int));

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

   if (local_rank == 0) {
      for (i=0;i<cluster_count;i++) {
        INFO(0, "Cluster %d %d %d", i, cluster_sizes[i], cluster_offsets[i]);
      }

      INFO(0, "Total size %d", cluster_offsets[cluster_count]);
   }

   return CONNECT_OK;
}

// Init the wide area communication.
int wa_init(int local_rank, int local_count, int *argc, char ***argv)
{
   int status, error;

   status = init_cluster_info(argc, argv);

   if (status == 0) {
      ERROR(1, "Failed to initialize WA sockets implementation!");
      return EMPI_ERR_INTERN;
   }

   INFO(1, "I am %d of %d in cluster %s", local_rank, local_count, cluster_name);
   INFO(1, "Cluster %s is %d of %d clusters", cluster_name, cluster_rank, cluster_count);

   my_pid = SET_PID(cluster_rank, local_rank);

   status = wa_connect(server, port);

   if (status != CONNECT_OK) {
      ERROR(1, "Failed to connect to hub!");
      return 0;
   }

   error = handshake(local_rank, local_count, cluster_rank, cluster_count,
                      cluster_name, cluster_sizes, cluster_offsets);

   if (error != CONNECT_OK) {
      ERROR(1, "Failed to perform handshake with hub!");
      close(socketfd);
      return 0;
   }

   return 1;
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

   error = shutdown(socketfd, SHUT_RDWR);

   if (error != 0) {
      ERROR(1, "Failed to shutdown socket! %d", error);
      return CONNECT_ERROR_CLOSE_FAILED;
   }

   error = close(socketfd);

   if (error != 0) {
      ERROR(1, "Failed to close socket! %d", error);
      return CONNECT_ERROR_CLOSE_FAILED;
   }

   // TODO: cleanup communicators!

   return CONNECT_OK;
}

/*****************************************************************************/
/*                      Send / Receive operations                            */
/*****************************************************************************/

int wa_sendfully(unsigned char *buffer, size_t len)
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

int wa_receivefully(unsigned char *buffer, size_t len)
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


int wa_wait_for_data(int blocking)
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

int wa_wait_for_opcode(int blocking, int *opcode)
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

