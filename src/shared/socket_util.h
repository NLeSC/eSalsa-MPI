#ifndef _SOCKET_UTIL_H_
#define _SOCKET_UTIL_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <ifaddrs.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

// #include "message_buffer.h"

#define SEND_BUFFER_SIZE (32*1024*1024)
#define RECEIVE_BUFFER_SIZE (32*1024*1024)

#define DEFAULT_PACKET_SIZE (64*1024)

#define SOCKET_OK                     0
#define SOCKET_DISCONNECT            -1
#define SOCKET_ERROR_CREATE_SOCKET  -10
#define SOCKET_ERROR_CONNECT        -11
#define SOCKET_ERROR_OPTIONS        -12
#define SOCKET_ERROR_BIND           -13
#define SOCKET_ERROR_LISTEN         -14
#define SOCKET_ERROR_ACCEPT         -15
#define SOCKET_ERROR_HOST           -16
#define SOCKET_ERROR_SEND_FAILED    -17
#define SOCKET_ERROR_RECEIVE_FAILED -18
#define SOCKET_ERROR_CNTL_GET       -19
#define SOCKET_ERROR_CNTL_SET       -20
#define SOCKET_ERROR_HOST_NOT_FOUND -21
#define SOCKET_ERROR_ADDRESS_TYPE   -22
#define SOCKET_ERROR_CANNOT_FIND_IP -23
#define SOCKET_ERROR_ALLOCATE       -24
#define SOCKET_ERROR_BLOCKING       -25
#define SOCKET_ERROR_ADD_EPOLL      -26
#define SOCKET_ERROR_DEL_EPOLL      -27
#define SOCKET_ERROR_SET_EPOLL      -28

typedef struct {
	uint32_t events;
	void *data;
}  poll_result;

// Translate a hostname into an IPv4 address
int socket_get_ipv4_address(char *name, long *ipv4);

// Get all locally available IPv4 addresses.
int socket_get_local_ips(struct in_addr **ip4ads, int *ip4count);

int socket_set_non_blocking(int socketfd);
int socket_set_blocking(int socketfd);

int socket_sendfully(int socketfd, unsigned char *buffer, size_t len);
int socket_receivefully(int socketfd, unsigned char *buffer, size_t len);

ssize_t socket_receive(int socketfd, unsigned char *buffer, size_t count, bool blocking);
ssize_t socket_send(int socketfd, unsigned char *buffer, size_t count, bool blocking);

//ssize_t socket_send_mb(int socketfd, message_buffer *buffer, size_t packet_size, bool blocking);
//ssize_t socket_receive_mb(int socketfd, message_buffer *buffer, size_t to_read, size_t packet_size, bool blocking);

// Set buffer sizes for socket.
//
// WARNING: changing these values after a socket is connected may not work!
//
// send_buffer or receive_buffer == 0 means don't set, < 0 means use default, > 0 means set value.
int socket_set_buffers(int socket, int send_buffer, int receive_buffer);

// Set the TCP_NODELAY option of socket.
//
// If TRUE, any data written to the socket will be send as soon as possible.
// If FALSE, data may be buffered for while to1 allow larger packets to be send.
int socket_set_nodelay(int socket, bool nodelay);

// Create a new socket and connect it to the specified IP:port destination. The new socket is returned in socketfd.
//
// Use send_buffer and receive_buffer to specify the desired TCP window sizes to use for the socket. These must be set before the
// socket is connected, or they may not work. Use 0 to for autotuning and < 0 to use the default.
//
int socket_connect(unsigned long ipv4, unsigned short port, int send_buffer, int receive_buffer, int *socketfd);

// Create a new listen socket at local port, and waits for a connection from the expected host. The listen socket is then closed
// and the new socket is returned in socketfd.
//
// If expected_host is 0, any connection is accepted. If expected_host contains a IPv4 value, only connections from that IP
// address will be accepted.
//
// Use send_buffer and receive_buffer to specify the desired TCP window sizes to use for the socket. These must be set before the
// socket is connected, or they may not work. Use 0 to for autotuning and < 0 to use the default.
//
int socket_accept_one(unsigned short local_port, uint32_t expected_host, int send_buffer, int receive_buffer, int *socketfd);

// Create a new listen socket at local port. The listen socket is returned in listenfd.
//
// Use send_buffer and receive_buffer to specify the desired TCP window sizes to use for the socket. These must be set before the
// socket is connected, or they may not work. Use 0 to for autotuning and < 0 to use the default.
//
int socket_listen(unsigned short local_port, int send_buffer, int receive_buffer, int backlog, int *listenfd);

int socket_add_to_epoll(int epollfd, int socketfd, void *data);
int socket_remove_from_epoll(int epollfd, int socketfd);

int socket_set_rw(int epollfd, int socketfd, void *data);
int socket_set_ro(int epollfd, int socketfd, void *data);
int socket_set_wo(int epollfd, int socketfd, void *data);
int socket_set_idle(int epollfd, int socketfd, void *data);

//int socket_epoll_event(int epollfd, int timeout, poll_result *events, int max_events, int *event_count);

#endif
