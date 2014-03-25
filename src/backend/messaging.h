#ifndef _MESSAGING_H_
#define _MESSAGING_H_

#include <stdint.h>
#include "empi.h"
#include "types.h"

// special message tags

#define MAX_TAG 32768

#define BCAST_TAG      (MAX_TAG-1)
#define BARRIER_TAG    (MAX_TAG-2)
#define REDUCE_TAG     (MAX_TAG-3)
#define SCAN_TAG       (MAX_TAG-4)
#define GATHERV_TAG    (MAX_TAG-5)
#define SCATTERV_TAG   (MAX_TAG-6)
#define ALLGATHERV_TAG (MAX_TAG-7)
#define ALLTOALLV_TAG  (MAX_TAG-8)

// requests

#define COMM_REQUEST_SIZE (5*sizeof(int))

typedef struct s_comm_request {
    int opcode;  // type of message
    int comm;    // communicator used
    int src;     // rank in current communicator
    int color;   // target new communicator
    int key;     // prefered rank in target communicator
} comm_request;


#define GROUP_REQUEST_SIZE (4*sizeof(int))

typedef struct s_group_request {
    int opcode;  // type of message
    int comm;    // communicator used
    int src;     // rank in current communicator
    int size;    // number of ranks in group
    // int *ranks // ranks for group
} group_request;


#define DUP_REQUEST_SIZE (3*sizeof(int))

typedef struct s_dup_request {
    int opcode;  // type of message
    int comm;    // communicator used
    int src;     // rank in current communicator
} dup_request;

#define TERMINATE_REQUEST_SIZE (3*sizeof(int))

typedef struct s_terminate_request {
    int opcode;  // type of message
    int comm;    // communicator used
    int src;     // rank in current communicator
} terminate_request;

// replies


#define COMM_REPLY_SIZE (9*sizeof(int))

typedef struct s_comm_reply {
    int comm;           // communicator used
    int src;            // source rank (unused)
    int newComm;        // communicator created
    int rank;           // rank in new communicator
    int size;           // size of new communicator
    int color;          // color for local split
    int key;            // key for local split
    int cluster_count;  // number of cluster in communicator
    int flags;          // flags of new communicator
    int *coordinators;  // cluster coordinators
    int *cluster_sizes; // cluster sizes
    int *cluster_ranks; // cluster ranks
    uint32_t *members;  // communicator members
    uint32_t *member_cluster_index; // members cluster indeces
    uint32_t *local_ranks; // members local ranks
} comm_reply;


#define GROUP_REPLY_SIZE (8*sizeof(int))

// TODO: merge with s_comm_reply ?
typedef struct s_group_reply {
    int comm;           // communicator used
    int src;            // source rank (unused)
    int newComm;        // communicator created
    int rank;           // rank in new communicator
    int size;           // size of new communicator
    int type;           // type of groupreply
    int cluster_count;  // number of clusters in communicator
    int flags;          // flags of new communicator
    int *coordinators;  // cluster coordinators
    int *cluster_sizes; // cluster sizes
    int *cluster_ranks; // cluster ranks
    uint32_t *members;  // communicator members
    uint32_t *member_cluster_index; // members cluster indeces
    uint32_t *local_ranks; // members local ranks
} group_reply;


#define DUP_REPLY_SIZE (3*sizeof(int))

// TODO: merge with s_comm_reply ?
typedef struct s_dup_reply {
    int comm;    // communicator used
    int src;     // source rank (unused)
    int newComm; // communicator created
} dup_reply;

// mesg

#define MSG_HEADER_SIZE (7*sizeof(int))

typedef struct s_msg_header {
    int opcode;  // type of message
    int comm;    // communicator used
    int source;  // source rank
    int dest;    // destination rank
    int tag;     // message tag
    int count;   // data size in elements
    int bytes;   // data size in bytes
} message_header;

struct s_msg_buffer {
    message_header header;      // Message header
    int byte_order;             // Current byte-order of header
    int data_buffer_size;       // Message buffer size
    unsigned char *data_buffer; // Message buffer

    message_buffer *next;       // Next message in queue
};

// Initialize and finalize the messaging layer
int messaging_init(int rank, int size, int *adjusted_rank, int *adjusted_size, int *argc, char ***argv);
int messaging_finalize();

// Run the messaging gateway (if applicable).
int messaging_run_gateway(int rank, int size, int empi_size);

// Send and receive messages (to any remote participant).
int messaging_send(void* buf, int count, datatype *t, int dest, int tag, communicator* c);
int messaging_receive(void *buf, int count, datatype *t, int source, int tag, status *s, communicator* c);

// Broadcast messages (to all cluster coordinators).
int messaging_bcast(void* buf, int count, datatype *t, int root, communicator* c);
int messaging_bcast_receive(void *buf, int count, datatype *t, int root, communicator* c);

// Probe if a message is available.
int messaging_probe_receive(request *r, int blocking);

// Finalize a pending receive request.
int messaging_finalize_receive(request *r, status *s);

// Send and receive functions used to implement an MPI_COMM_SPLIT
int messaging_comm_split_send(communicator* c, int color, int key);
int messaging_comm_split_receive(comm_reply *reply);

// Send and receive functions used to implement an MPI_COMM_CREATE
int messaging_comm_create_send(communicator* c, group *g);
int messaging_comm_create_receive(group_reply *reply);

// Send and receive functions used to implement an MPI_COMM_DUP
int messaging_comm_dup_send(communicator* c);
int messaging_comm_dup_receive(dup_reply *reply);

// Send and receive functions used to implement an MPI_COMM_FREE and MPI_FINALIZE
int messaging_comm_free_send(communicator* c);

// Profiling support
int messaging_print_profile();

#endif // _MESSAGING_H_
