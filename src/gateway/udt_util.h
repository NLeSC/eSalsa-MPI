#ifndef _UDT_UTIL_H_
#define _UDT_UTIL_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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

#include "udt4.h"

#define SEND_BUFFER_SIZE    (32*1024*1024)
#define RECEIVE_BUFFER_SIZE (32*1024*1024)


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

int udt_sendfully(int socketfd, unsigned char *buffer, size_t len);
int udt_receivefully(int socketfd, unsigned char *buffer, size_t len);

// Create a new socket and connect it to the specified IP:port destination. The new socket is returned in socketfd.
//
// Use send_buffer and receive_buffer to specify the desired TCP window sizes to use for the socket. These must be set before the
// socket is connected, or they may not work. Use 0 to for autotuning and < 0 to use the default.
//
int udt_connect(unsigned long ipv4, unsigned short port, int send_buffer, int receive_buffer, int *socketfd);

// Create a new socket at local port, and wait for a connection from the expected host. The socket is returned in socketfd.
//
// If expected_host is 0, any connection is accepted. If expected_host contains a IPv4 value, only connections from that IP
// address will be accepted.
//
// Use send_buffer and receive_buffer to specify the desired TCP window sizes to use for the socket. These must be set before the
// socket is connected, or they may not work. Use 0 to for autotuning and < 0 to use the default.
//
int udt_accept(unsigned short local_port, uint32_t expected_host, int send_buffer, int receive_buffer, int *socketfd);

#endif
