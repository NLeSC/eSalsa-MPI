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

#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <netinet/in.h>
#include <netinet/tcp.h>

#include "settings.h"
#include "logging.h"

#include "udt_util.h"
#include "udt4.h"

/*****************************************************************************/
/*                      Send / Receive operations                            */
/*****************************************************************************/

int udt_sendfully(int socketfd, unsigned char *buffer, size_t len)
{
	size_t w = 0;
	ssize_t tmp = 0;

	while (w < len) {

		tmp = udt4_send(socketfd, (char *)buffer+w, len-w, 0);

		if (tmp < 0) {
			ERROR(1, "udt_sendfully failed! (%s)", strerror(errno));
			return SOCKET_ERROR_SEND_FAILED;
		} else {
			w += tmp;
		}
	}

	return SOCKET_OK;
}

int udt_receivefully(int socketfd, unsigned char *buffer, size_t len)
{
	size_t r = 0;
	ssize_t tmp = 0;

	while (r < len) {
		tmp = udt4_recv(socketfd, (char *)buffer+r, len-r, 0);

		if (tmp < 0) {
			ERROR(1, "udt_receivefully failed! (%s)", strerror(errno));
			return SOCKET_ERROR_RECEIVE_FAILED;
		} else {
			r += tmp;
		}
	}

	return SOCKET_OK;
}

int udt_connect(unsigned long ipv4, unsigned short port, int send_buffer, int receive_buffer, int *socketfd)
{
	struct sockaddr_in address;
	socklen_t addrlen;
	int error;
	int connected = 0;
	int attempts = 0;
	char ipstring[INET_ADDRSTRLEN+1];
	struct addrinfo hints, *local;

	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	// FIXME: hardcoded port!
	if (getaddrinfo(NULL, "9000", &hints, &local) != 0) {
		ERROR(1, "Incorrect network address.");
		return SOCKET_ERROR_CREATE_SOCKET;
	}

	*socketfd = udt4_create_socket(local->ai_family, local->ai_socktype, local->ai_protocol);

	if (*socketfd < 0) {
		ERROR(1, "Failed to create socket!");
		return SOCKET_ERROR_CREATE_SOCKET;
	}

	// FIXME: set socket options here!!
	// TODO
	//error = socket_set_buffers(*socketfd, send_buffer, receive_buffer);

	///if (error != 0) {
	//WARN(1, "Failed to set buffers for socket %d (error = %d)!", *socketfd, error);
	//}

	freeaddrinfo(local);

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = ipv4;
	address.sin_port = htons(port);

	inet_ntop(AF_INET, &(address.sin_addr), ipstring, INET_ADDRSTRLEN);

	INFO(2, "Connecting to %s:%d", ipstring, port);

	addrlen = sizeof(struct sockaddr_in);

	while (attempts < 1000) {

		error = udt4_connect(*socketfd, (struct sockaddr *)&address, addrlen);

		if (error != 0) {
			WARN(1, "Failed to connect to %s:%d (error = %d) -- will retry!", ipstring, port, error);
			sleep(1);
		} else {
			connected = 1;
			break;
		}
	}

	if (connected == 0) {
		udt4_close(*socketfd);
		ERROR(1, "Failed to connect to %s:%d", ipstring, port);
		return SOCKET_ERROR_CONNECT;
	}

	INFO(1, "Created connection to %s:%d", ipstring, port);

	return SOCKET_OK;
}

int udt_accept(unsigned short local_port, uint32_t expected_host,int send_buffer, int receive_buffer, int *socketfd)
{
	int sd, new_socket;
	int error;
	uint32_t host;
	struct sockaddr_in address;
	int addrlen;
	char buffer[INET_ADDRSTRLEN+1];

	struct addrinfo hints, *res;

	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	//hints.ai_socktype = SOCK_DGRAM;

	if (getaddrinfo(NULL, "9000", &hints, &res) != 0) {
		ERROR(1, "Illegal UDT port number %d or port is busy.", 9000);
		return SOCKET_ERROR_CREATE_SOCKET;
	}

	sd = udt4_create_socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	if (sd < 0) {
		ERROR(1, "Failed to create socket!");
		return SOCKET_ERROR_CREATE_SOCKET;
	}

	// FIXME: set options here!

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(local_port);

	INFO(2, "Accepting connection from host %s on port %d", inet_ntop(AF_INET, &expected_host, buffer, INET_ADDRSTRLEN+1), local_port);

	error = udt4_bind(sd, (struct sockaddr *)&address, sizeof(address));

	if (error != 0) {
		ERROR(1, "Failed to bind UDT socket to port number %d.", 9000);
		return SOCKET_ERROR_CREATE_SOCKET;
	}

	freeaddrinfo(res);

	error = udt4_listen(sd, 1);

	if (error != 0) {
		ERROR(1, "Failed to listen to UDT socket");
		return SOCKET_ERROR_CREATE_SOCKET;
	}

	new_socket = udt4_accept(sd, (struct sockaddr *)&address, &addrlen);

	udt4_close(sd);

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


