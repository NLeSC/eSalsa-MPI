#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <errno.h>
#include <limits.h>
#include <fcntl.h>
#include <stdbool.h>

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

#include <netinet/in.h>
#include <netinet/tcp.h>

#include "settings.h"
#include "logging.h"
#include "socket_util.h"
#include "message_buffer.h"

/*****************************************************************************/
/*                          Socket operations                                */
/*****************************************************************************/

int socket_get_ipv4_address(char *name, long *ipv4)
{
   struct addrinfo *result;
   int error;

   /* resolve the server name into a list of addresses */
   error = getaddrinfo(name, NULL, NULL, &result);

   if (error != 0) {
      ERROR(1, "getaddrinfo failed: %s", gai_strerror(error));
      return SOCKET_ERROR_HOST_NOT_FOUND;
   }

   if (result->ai_family == AF_INET) {
      DEBUG(1, "Got inet4");
   } else {
      ERROR(1, 0, "Got unsupported address type!");
      return SOCKET_ERROR_ADDRESS_TYPE;
   }

   *ipv4 = ((struct sockaddr_in *)(result->ai_addr))->sin_addr.s_addr;

   freeaddrinfo(result);

   return SOCKET_OK;
}

int socket_set_non_blocking(int socketfd)
{
   int error;

   int flags = fcntl (socketfd, F_GETFL, 0);

   if (flags == -1) {
      ERROR(1, "Failed to get socket flags! (error=%d)", errno);
      return SOCKET_ERROR_CNTL_GET;
   }

   flags |= O_NONBLOCK;

   error = fcntl (socketfd, F_SETFL, flags);

   if (error == -1) {
      ERROR(1, "Failed to set socket to NON_BLOCKING mode! (error=%d)", errno);
      return SOCKET_ERROR_CNTL_SET;
   }

   return SOCKET_OK;
}

int socket_set_blocking(int socketfd)
{
   int error;

   int flags = fcntl (socketfd, F_GETFL, 0);

   if (flags == -1) {
      ERROR(1, "Failed to get socket flags! (error=%d)", errno);
      return SOCKET_ERROR_CNTL_GET;
   }

   flags ^= O_NONBLOCK;

   error = fcntl (socketfd, F_SETFL, flags);

   if (error == -1) {
      ERROR(1, "Failed to set socket to BLOCKING mode! (error=%d)", errno);
      return SOCKET_ERROR_CNTL_SET;
   }

   return SOCKET_OK;
}

/*****************************************************************************/
/*                      Send / Receive operations                            */
/*****************************************************************************/


int socket_sendfully(int socketfd, unsigned char *buffer, size_t len)
{
   size_t w = 0;
   ssize_t tmp = 0;

   while (w < len) {
      tmp = write(socketfd, buffer+w, len-w);

      if (tmp < 0) {
         ERROR(1, "socket_sendfully failed! (%s)", strerror(errno));
         return SOCKET_ERROR_SEND_FAILED;
      } else {
         w += tmp;
      }
   }

   return SOCKET_OK;
}

int socket_receivefully(int socketfd, unsigned char *buffer, size_t len)
{
   size_t r = 0;
   ssize_t tmp = 0;

   while (r < len) {
      tmp = read(socketfd, buffer+r, len-r);

      if (tmp < 0) {
         ERROR(1, "socket_receivefully failed! (%s)", strerror(errno));
         return SOCKET_ERROR_RECEIVE_FAILED;
      } else {
         r += tmp;
      }
   }

   return SOCKET_OK;
}

/*
ssize_t socket_receive(int socketfd, unsigned char *buffer, size_t len, size_t packet_size, bool blocking)
{
	ssize_t tmp;
	int flags;
	size_t todo, bytes, torecv;

	if (len <= 0) {
		return 0;
	}

	if (blocking) {
		flags = 0;
	} else {
		flags = MSG_DONTWAIT;
	}

	if (packet_size <= 0) {
		packet_size = 8*1024;
	}

	todo = len;
	bytes = 0;

	while (todo > 0) {

		if (todo <= packet_size) {
			torecv = todo;
		} else {
			torecv = packet_size;
		}

		tmp = recv(socketfd, buffer+bytes, torecv, flags);

		if (tmp < 0) {
			if (!blocking && (errno == EAGAIN || errno == EWOULDBLOCK)) {
				return bytes;
			} else {
				ERROR(1, "socket receive failed! (%s) errno=%d", strerror(errno), errno);
				return SOCKET_ERROR_RECEIVE_FAILED;
			}
		}

		if (tmp == 0) {
			// Other side has shut down the connection!
			return SOCKET_DISCONNECT;
		}

		bytes += tmp;
		todo -= tmp;
	}

	return len;
}
*/

/*
ssize_t socket_receive(int socketfd, unsigned char *buffer, size_t count, bool blocking)
{
	register ssize_t r;
	register size_t nleft = count;
	register int flags;

	if (blocking) {
		flags = 0;
	} else {
		flags = MSG_DONTWAIT;
	}

	while (nleft > 0) {

		r = recv(socketfd, buffer, nleft, flags);

		if (r < 0) {
			 // An error occurred. If we are in non-blocking mode this may be OK.
			if (!blocking && (errno == EAGAIN || errno == EWOULDBLOCK)) {
				return count - nleft;
			} else {
				ERROR(1, "socket receive failed! (%s) errno=%d", strerror(errno), errno);
				return SOCKET_ERROR_RECEIVE_FAILED;
			}
		} else if (r == 0) {
			// Other side has shut down the connection!
			if (!blocking && count != nleft) {
				// We have already received some bytes
				return count - nleft;
			} else {
				return SOCKET_DISCONNECT;
			}
		}

		nleft -= r;
		buffer += r;
	}

	return count;
}
*/

ssize_t socket_receive(int socketfd, unsigned char *buffer, size_t count)
{
	register ssize_t r;
	register size_t nleft = count;
	register int flags;

	flags = MSG_DONTWAIT;

	while (nleft > 0) {

		r = recv(socketfd, buffer, nleft, flags);

		if (r < 0) {
			 // An error occurred. If we are in non-blocking mode this may be OK.
			if (errno == EAGAIN || errno == EWOULDBLOCK) {
				return count - nleft;
			}

			ERROR(1, "socket receive failed! (%s) errno=%d", strerror(errno), errno);
			return SOCKET_ERROR_RECEIVE_FAILED;

		} else if (r == 0) {
			// Other side has shut down the connection!
			if (count != nleft) {
				// We have already received some bytes
				return count - nleft;
			} else {
				return SOCKET_DISCONNECT;
			}
		}

		nleft -= r;
		buffer += r;
	}

	return count;
}



/*
ssize_t socket_receive_mb(int socketfd, message_buffer *buffer, size_t to_read, size_t packet_size, bool blocking)
{
	ssize_t bytes_read;

	if (to_read <= 0) {
		to_read = buffer->size - buffer->end;
	}

	bytes_read = socket_receive(socketfd, buffer->data + buffer->end, to_read, packet_size, blocking);

	if (bytes_read <= 0) {
		return bytes_read;
	}

	buffer->end += bytes_read;
	return bytes_read;
}

//int socket_receive(int socketfd, unsigned char *buffer, size_t len, size_t *bytes_read)
//{
//	ssize_t tmp = read(socketfd, buffer, len);
//
//	if (tmp < 0) {
//        ERROR(1, "socket_receive failed! (%s)", strerror(errno));
//        return SOCKET_ERROR_RECEIVE_FAILED;
//	}
//
//	*bytes_read = tmp;
//	return SOCKET_OK;
//}

*/

/*
ssize_t socket_send(int socketfd, unsigned char *buffer, size_t count, bool blocking)
{
	 register ssize_t r;
	 register size_t nleft = count;
	 register int flags;

	 if (blocking) {
		flags = 0;
	 } else {
		flags = MSG_DONTWAIT;
	 }

	 while (nleft > 0) {

		 r = send(socketfd, buffer, nleft, flags);

		 if (r <= 0) {
			 // An error occurred. If we are in non-blocking mode this may be OK.
			 if (!blocking && (errno == EAGAIN || errno == EWOULDBLOCK)) {
				 return count - nleft;
			 }

			 ERROR(1, "socket_send failed! (%s)", strerror(errno));
			 return SOCKET_ERROR_SEND_FAILED;

		 }

		 nleft -= r;
		 buffer += r;
	 }

	 return count;
}
*/

ssize_t socket_send(int socketfd, unsigned char *buffer, size_t count)
{
	 register ssize_t r;
	 register size_t nleft = count;
	 register int flags;

	 flags = MSG_DONTWAIT;

	 while (nleft > 0) {

		 r = send(socketfd, buffer, nleft, flags);

		 if (r <= 0) {
			 // An error occurred. If we are in non-blocking mode this may be OK.
			 if (errno == EAGAIN || errno == EWOULDBLOCK) {
				 return count - nleft;
			 }

			 ERROR(1, "socket_send failed! (%s)", strerror(errno));
			 return SOCKET_ERROR_SEND_FAILED;
		 }

		 nleft -= r;
		 buffer += r;
	 }

	 return count;
}


/*
ssize_t socket_send(int socketfd, unsigned char *buffer, size_t len, size_t packet_size, bool blocking)
{
	ssize_t tmp;
	int flags;
	size_t todo, bytes, tosend;

	if (len <= 0) {
		return 0;
	}

	if (blocking) {
		flags = 0;
	} else {
		flags = MSG_DONTWAIT;
	}

	if (packet_size <= 0) {
		packet_size = 8*1024;
	}

	todo = len;
	bytes = 0;

	while (todo > 0) {

		if (todo <= packet_size) {
			tosend = todo;
		} else {
			tosend = packet_size;
		}

		tmp = send(socketfd, buffer + bytes, tosend, flags);

		if (tmp < 0) {
			if (!blocking && (errno == EAGAIN || errno == EWOULDBLOCK)) {
				return bytes;
			} else {
				ERROR(1, "socket_send failed! (%s)", strerror(errno));
				return SOCKET_ERROR_SEND_FAILED;
			}
		}

		bytes += tmp;
		todo -= tmp;
	}

	return len;
}
*/
/*
ssize_t socket_send_mb(int socketfd, message_buffer *buffer, size_t packet_size, bool blocking)
{
	ssize_t bytes_sent;

	bytes_sent = socket_send(socketfd, buffer->data+buffer->start, buffer->end-buffer->start, packet_size, blocking);

	if (bytes_sent < 0) {
		return bytes_sent;
	}

	buffer->start += bytes_sent;

	if (buffer->start == buffer->end) {
		buffer->start = buffer->end = 0;
	}

	return bytes_sent;
}
*/

int socket_get_options(int socket, int *send_buffer, int *receive_buffer, bool *nodelay)
{
	int error;
	socklen_t size;

    size = sizeof(int);
	error = getsockopt(socket, SOL_SOCKET, SO_SNDBUF, (void *)send_buffer, &size);

	if (error != 0) {
		return error;
	}

    size = sizeof(int);
	error = getsockopt(socket, SOL_SOCKET, SO_RCVBUF, (void *)receive_buffer, &size);

	if (error != 0) {
		return error;
	}

    size = sizeof(int);

    // Verify results
    error = getsockopt(socket, IPPROTO_TCP, TCP_NODELAY, nodelay, &size);

    if (error != 0) {
    	return error;
    }

	return SOCKET_OK;
}

int socket_set_nodelay(int socket, bool nodelay)
{
	int flag, error, tmp;
	socklen_t size;

	// Set TCP nodelay
	flag = (nodelay ? 1 : 0);

	size = sizeof(int);
	error = setsockopt(socket, IPPROTO_TCP, TCP_NODELAY, &flag, size);

	if (error != 0) {
		WARN(1, "Failed to set TCP_NODELAY!");
	}

	// Verify results
	error = getsockopt(socket, IPPROTO_TCP, TCP_NODELAY, &tmp, &size);

	if (error != 0) {
		WARN(1, "Failed to get TCP_NODELAY!");
	}

	if (flag != tmp) {
		WARN(1, "TCP_NODELAY set to %d but asked for %d\n", tmp, flag);
	}

	return SOCKET_OK;
}

int socket_set_buffers(int socket, int send_buffer, int receive_buffer)
{
   int rcvbuf, sndbuf, error, tmp;
   socklen_t size;

   if (send_buffer != 0) {

	   if (send_buffer < 0) {
		   sndbuf = SEND_BUFFER_SIZE;
	   } else {
		   sndbuf = send_buffer;
	   }

	   size = sizeof(int);
	   error = setsockopt(socket, SOL_SOCKET, SO_SNDBUF, &sndbuf, size);

	   if (error != 0) {
		   WARN(1, "Failed to set SO_SNDBUF!");
	   }

	   // Verify results
	   error = getsockopt(socket, SOL_SOCKET, SO_SNDBUF, &tmp, &size);

	   if (error != 0) {
		   WARN(1, "Failed to get SO_SNDBUF!");
	   }

	   if (sndbuf != tmp) {
		   WARN(1, "Socket %d send buffer set to %d but asked for %d", socket, tmp, sndbuf);
	   }
   }

   if (receive_buffer != 0) {

	   if (receive_buffer < 0) {
		   rcvbuf = RECEIVE_BUFFER_SIZE;
	   } else {
		   rcvbuf = receive_buffer;
	   }

	   size = sizeof(int);
	   error = setsockopt(socket, SOL_SOCKET, SO_RCVBUF, &rcvbuf, size);

	   if (error != 0) {
		   WARN(1, "Failed to set SO_RCVBUF!");
	   }

	   // Verify results
	   error = getsockopt(socket, SOL_SOCKET, SO_RCVBUF, &tmp, &size);

	   if (error != 0) {
		   WARN(1, "Failed to get SO_RCVBUF!");
	   }

	   if (sndbuf != tmp) {
		   WARN(1, "Socket %d receive buffer set to %d but asked for %d", socket, tmp, rcvbuf);
	   }
   }

   return SOCKET_OK;
}

int socket_connect(unsigned long ipv4, unsigned short port, int send_buffer, int receive_buffer, int *socketfd)
{
   struct sockaddr_in address;
   socklen_t addrlen;
   int error;
   int connected = 0;
   int attempts = 0;
   useconds_t timeout;
   char ipstring[INET_ADDRSTRLEN+1];

   address.sin_family = AF_INET;
   address.sin_addr.s_addr = ipv4;
   address.sin_port = htons(port);

   inet_ntop(AF_INET, &(address.sin_addr), ipstring, INET_ADDRSTRLEN);

   INFO(2, "Connecting to %s:%d", ipstring, port);

   *socketfd = socket(AF_INET, SOCK_STREAM, 0);

   if (*socketfd < 0) {
      ERROR(1, "Failed to create socket!");
      return SOCKET_ERROR_CREATE_SOCKET;
   }

   error = socket_set_buffers(*socketfd, send_buffer, receive_buffer);

   if (error != 0) {
	   WARN(1, "Failed to set buffers for socket %d (error = %d)!", *socketfd, error);
   }

   addrlen = sizeof(struct sockaddr_in);

   // Start with 100 usec timeout, and keep doubling until 1 sec is reached.
   timeout = 100;

   while (attempts < 1000) {

      error = connect(*socketfd, (struct sockaddr *)&address, addrlen);

      if (error != 0) {
         WARN(1, "Failed to connect to %s:%d (error = %d) -- will retry after %ld usec!", ipstring, port, error, timeout);
         usleep(timeout);
         timeout *= 2;

         if (timeout > 1000000) {
        	 timeout = 1000000;
         }
      } else {
         connected = 1;
         break;
      }
   }

   if (connected == 0) {
      close(*socketfd);
      ERROR(1, "Failed to connect to %s:%d", ipstring, port);
      return SOCKET_ERROR_CONNECT;
   }

   INFO(1, "Created connection to %s:%d", ipstring, port);

   return SOCKET_OK;
}

int socket_listen(unsigned short local_port, int send_buffer, int receive_buffer, int backlog, int *listenfd)
{
	int sd, error, flag;
	struct sockaddr_in address;

	sd = socket(AF_INET, SOCK_STREAM, 0);

	if (sd < 0) {
		ERROR(1, "Failed to create socket!");
		return SOCKET_ERROR_CREATE_SOCKET;
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(local_port);

	flag = 1;

	error = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *) &flag, sizeof(int));

	if (error != 0) {
		close(sd);
		ERROR(1, "Failed to set SO_REUSEADDR on server socket!");
		return SOCKET_ERROR_BIND;
	}

	error = socket_set_buffers(sd, send_buffer, receive_buffer);

	if (error != 0) {
		WARN(1, "Failed to set buffers for socket %d (error = %d)!", sd, error);
	}

	error = bind(sd,(struct sockaddr *)&address, sizeof(address));

	if (error != 0) {
		close(sd);
		ERROR(1, "Failed to bind socket to port %d!", local_port);
		return SOCKET_ERROR_BIND;
	}

	error = listen(sd, backlog);

	if (error != 0) {
		close(sd);
		ERROR(1, "Failed to listen to socket on port %d!", local_port);
		return SOCKET_ERROR_LISTEN;
	}

//	if (!blocking) {
//		error = socket_set_non_blocking(sd);
//
//		if (error != 0) {
//			close(sd);
//			ERROR(1, "Failed to set listen socket to non-blocking!");
//			return SOCKET_ERROR_BLOCKING;
//		}
//	}

	*listenfd = sd;
	return SOCKET_OK;
}

int socket_accept_one(unsigned short local_port, uint32_t expected_host, int send_buffer, int receive_buffer, int *socketfd)
{
   int sd, new_socket, error;
   uint32_t host;
   struct sockaddr_in address;
   socklen_t addrlen;
   char buffer[INET_ADDRSTRLEN+1];

//   sd = socket(AF_INET, SOCK_STREAM, 0);
//
//   if (sd < 0) {
//      ERROR(1, "Failed to create socket!");
//      return SOCKET_ERROR_CREATE_SOCKET;
//   }
//
//   address.sin_family = AF_INET;
//   address.sin_addr.s_addr = INADDR_ANY;
//   address.sin_port = htons(local_port);

   INFO(2, "Accepting connection from host %s on port %d", inet_ntop(AF_INET, &expected_host, buffer, INET_ADDRSTRLEN+1), local_port);

//   error = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *) &flag, sizeof(int));
//
//   if (error != 0) {
//      close(sd);
//      ERROR(1, "Failed to set SO_REUSEADDR on server socket!");
//      return SOCKET_ERROR_BIND;
//   }
//
//   error = socket_set_buffers(sd, send_buffer, receive_buffer);
//
//   if (error != 0) {
//	   WARN(1, "Failed to set buffers for socket %d (error = %d)!", sd, error);
//   }
//
//   error = bind(sd,(struct sockaddr *)&address, sizeof(address));
//
//   if (error != 0) {
//      close(sd);
//      ERROR(1, "Failed to bind socket to port %d!", local_port);
//      return SOCKET_ERROR_BIND;
//   }
//
//   error = listen(sd, 1);
//
//   if (error != 0) {
//      close(sd);
//      ERROR(1, "Failed to listen to socket on port %d!", local_port);
//      return SOCKET_ERROR_LISTEN;
//   }

   error = socket_listen(local_port, send_buffer, receive_buffer, 1, &sd);

   if (error != 0) {
	   return error;
   }

   addrlen = sizeof(struct sockaddr_in);

   new_socket = accept(sd, (struct sockaddr *)&address, &addrlen);

   close(sd);

   if (new_socket < 0) {
      ERROR(1, "Failed to accept socket connection on port %d!", local_port);
      return SOCKET_ERROR_ACCEPT;
   }

   host = address.sin_addr.s_addr;

   if (expected_host == 0 || expected_host == host) {
     INFO(1, "Received connection from expected host %s:%d!", inet_ntop(AF_INET, &(address.sin_addr), buffer, INET_ADDRSTRLEN+1), ntohs(address.sin_port));
   } else {
      close(new_socket);
      ERROR(1, "Received connection from unexpected host %s (%d != %d)!", inet_ntop(AF_INET, &host, buffer, INET_ADDRSTRLEN+1), host, expected_host);
      return SOCKET_ERROR_HOST;
   }

   *socketfd = new_socket;

   return SOCKET_OK;
}

int socket_get_local_ips(struct in_addr **ip4ads, int *ip4count)
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
      WARN(1, "Failed to find valid IPv4 address!");
      return SOCKET_ERROR_CANNOT_FIND_IP;
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
      WARN(1, "Failed to find valid IPv4 address!");
      freeifaddrs(addresses);
      return SOCKET_ERROR_CANNOT_FIND_IP;
   }

   output = malloc(count * sizeof(struct in_addr));

   if (output == NULL) {
      WARN(1, "Failed to allocate space for IPv4 addresses on this gateway!");
      freeifaddrs(addresses);
      return SOCKET_ERROR_ALLOCATE;
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

   return SOCKET_OK;
}

int socket_add_to_epoll(int epollfd, int socketfd, void *data)
{
   int error;
   struct epoll_event event;

   DEBUG(1, "Adding socket %d to epoll %d", socketfd, epollfd);

   event.data.ptr = data;
   event.events = EPOLLIN;

   error = epoll_ctl (epollfd, EPOLL_CTL_ADD, socketfd, &event);

   if (error == -1) {
      ERROR(1, "Failed to add socket to epoll (error=%d %s)", errno, strerror(errno));
      return SOCKET_ERROR_ADD_EPOLL;
   }

   return SOCKET_OK;
}

int socket_remove_from_epoll(int epollfd, int socketfd)
{
   int error;

   DEBUG(1, "Removinf socket %d from epoll %d", socketfd, epollfd);

   error = epoll_ctl (epollfd, EPOLL_CTL_DEL, socketfd, NULL);

   if (error == -1) {
      ERROR(1, "Failed to remove socket from epoll (error=%d %s)", errno, strerror(errno));
      return SOCKET_ERROR_DEL_EPOLL;
   }

   return SOCKET_OK;
}

int socket_set_rw(int epollfd, int socketfd, void *data)
{
   int error;
   struct epoll_event event;

   DEBUG(1, "Setting socket %d to RW in epoll %d", socketfd, epollfd);

   event.data.ptr = data;
   event.events = EPOLLIN | EPOLLOUT;

   error = epoll_ctl (epollfd, EPOLL_CTL_MOD, socketfd, &event);

   if (error != 0) {
      ERROR(1, "Failed to set socket to RW (%d %d error=%d %s )", epollfd, socketfd, errno, strerror(errno));
      return SOCKET_ERROR_SET_EPOLL;
   }

   return SOCKET_OK;
}

int socket_set_ro(int epollfd, int socketfd, void *data)
{
   int error;
   struct epoll_event event;

   INFO(1, "Setting socket %d to RO in epoll %d", socketfd, epollfd);

   event.data.ptr = data;
   event.events = EPOLLIN;

   error = epoll_ctl (epollfd, EPOLL_CTL_MOD, socketfd, &event);

   if (error != 0) {
      ERROR(1, "Failed to set socket to RO (socket=%d error=%d %s)", socketfd, error, strerror(errno));
      return SOCKET_ERROR_SET_EPOLL;
   }

   return SOCKET_OK;
}

int socket_set_wo(int epollfd, int socketfd, void *data)
{
   int error;
   struct epoll_event event;

   DEBUG(1, "Setting socket %d to WO in epoll %d", socketfd, epollfd);

   event.data.ptr = data;
   event.events = EPOLLOUT;

   error = epoll_ctl (epollfd, EPOLL_CTL_MOD, socketfd, &event);

   if (error != 0) {
      ERROR(1, "Failed to set socket to WO (socket=%d error=%d %s )", socketfd, errno, strerror(errno));
      return SOCKET_ERROR_SET_EPOLL;
   }

   return SOCKET_OK;
}

int socket_set_idle(int epollfd, int socketfd, void *data)
{
   int error;
   struct epoll_event event;

   DEBUG(1, "Setting socket %d to IDLE in epoll %d", socketfd, epollfd);

   event.data.ptr = data;
   event.events = 0;

   error = epoll_ctl (epollfd, EPOLL_CTL_MOD, socketfd, &event);

   if (error != 0) {
      ERROR(1, "Failed to set socket to IDLE (%d %d error=%d %s )", epollfd, socketfd, errno, strerror(errno));
      return SOCKET_ERROR_SET_EPOLL;
   }

   return SOCKET_OK;
}

/*
int socket_epoll_event(int epollfd, int timeout, struct epoll_event *events, int max_events, int *count)
{
   *count = epoll_wait (epollfd, events, max_events, timeout);

   DEBUG(1, "Got %d socket events", *count);

   return SOCKET_OK;
}
*/



