#ifndef _OPCODES_H_
#define _OPCODES_H_

// Protocol used to talk to the server

// These are only used by the gateway to setup a connection to the server.
#define OPCODE_HANDSHAKE                  42
#define OPCODE_HANDSHAKE_ACCEPTED         43
#define OPCODE_HANDSHAKE_REJECTED         44
#define OPCODE_GATEWAY_INFO               45
#define OPCODE_GATEWAY_READY              46


// These are used by all processes to communicate with the server.
#define OPCODE_SPLIT                      51
#define OPCODE_SPLIT_REPLY                52
#define OPCODE_GROUP                      53
#define OPCODE_GROUP_REPLY                54
#define OPCODE_DUP                        55
#define OPCODE_DUP_REPLY                  56
#define OPCODE_FREE                       57
#define OPCODE_FINALIZE                   58
#define OPCODE_FINALIZE_REPLY             59

#define OPCODE_CLOSE_LINK                127







// #define OPCODE_COLLECTIVE_BCAST           60

// Tags use for MPI communication between the gateway and application nodes.
//
// Note that the rank of the target cluster is added to TAG_DATA_MSG. As a
// result all tags between TAG_DATA_MSG ... (TAG_DATA_MSG+NUM_CLUSTERS) may
// be in use in the gateway.
#define TAG_SERVER_REQUEST      0
#define TAG_SERVER_REPLY        1
#define TAG_FORWARDED_DATA_MSG  2
#define TAG_DATA_MSG            3


#endif // _OPCODES_H_


