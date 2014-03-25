#ifndef _WA_SOCKETS_H_
#define _WA_SOCKETS_H_

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>

// Timeout for a WA receive (in seconds).
#define WA_SOCKET_TIMEOUT                  3600

#define CONNECT_OK                         0
#define CONNECT_ERROR_SERVER_NOT_FOUND     1
#define CONNECT_ERROR_CANNOT_CREATE_SOCKET 2
#define CONNECT_ERROR_CANNOT_CONNECT       3
#define CONNECT_ERROR_CLOSE_FAILED         4
#define CONNECT_ERROR_CLUSTER_NOT_SET      5
#define CONNECT_ERROR_HANDSHAKE_FAILED     6
#define CONNECT_ERROR_SEND_FAILED          7
#define CONNECT_ERROR_RECEIVE_FAILED       8
#define CONNECT_ERROR_OUT_OF_COMMUNICATORS 9
#define CONNECT_ERROR_ALLOCATE             10
#define CONNECT_ERROR_BYTE_ORDER           11
#define CONNECT_ERROR_OPAQUE               12

// TODO: move these ?
#define OPCODE_HANDSHAKE                  42
#define OPCODE_HANDSHAKE_ACCEPTED         43
#define OPCODE_HANDSHAKE_REJECTED         44

#define OPCODE_DATA                       50
#define OPCODE_COMM                       51
#define OPCODE_COMM_REPLY                 52

#define OPCODE_GROUP                      53
#define OPCODE_GROUP_REPLY                54

#define OPCODE_DUP                        55
#define OPCODE_DUP_REPLY                  56

#define OPCODE_TERMINATE                  57

#define OPCODE_COLLECTIVE_BCAST           60

#define OPCODE_CLOSE_LINK                127

#define MAX_LENGTH_CLUSTER_NAME          128
#define MAX_COMMUNICATOR_BYTES             1
#define MAX_REQUEST_BYTES   (sizeof (int *))

/*
int wa_init(char *server_name, unsigned short port,
            int local_rank, int local_count,
            char *cluster_name, int cluster_rank, int cluster_count,
            int *cluster_sizes, int *cluster_offsets);
*/
int wa_init(int local_rank, int local_count, int *argc, char ***argv);

int wa_finalize();

int wa_sendfully(unsigned char *buffer, size_t len);
int wa_receivefully(unsigned char *buffer, size_t len);

//int wa_wait_for_data(int blocking);
int wa_wait_for_opcode(int blocking, int *opcode);

#endif // _WA_SOCKETS_H_
