#ifndef _MESSAGING_H_
#define _MESSAGING_H_

#include <stdint.h>
#include <stdbool.h>

#include "empi.h"
#include "types.h"
#include "mpi.h"
#include "generic_message.h"

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

// This is data message, that carries application data in its payload.
typedef struct s_data_msg {
   // Generic message header
   // message_header header;

   // Info needed for fragmentation / assembly of messages
   //uint32_t sequence_nr;      // data message sequence nr
   //uint32_t fragment_count;   // data message fragment count
   //uint32_t fragment_nr;      // data message fragment nr

   // Info needed by MPI to deliver the message.
   size_t length;             // length of message (including this header).
   int comm;                  // communicator used (and for which source and dest are valid).
   int source;                // source rank
   int dest;                  // destination rank
   int count;                 // data size in elements
   int tag;                   // message tag

   // Message payload.
   unsigned char payload[];   // message data
} data_message;

#define DATA_MESSAGE_SIZE (sizeof(size_t) + sizeof(int)*5)

// This struct is used to return the servers reply to a comm-split request.
typedef struct {
//    int comm;           // communicator used
//    int src;            // source rank (unused)
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

// This struct is used to return the servers reply to a group-to-comm request.
typedef struct {
//    int comm;           // communicator used
//    int src;            // source rank (unused)
    int newComm;        // communicator created
    int rank;           // rank in new communicator
    int size;           // size of new communicator
    int type;           // type of group reply
    int cluster_count;  // number of clusters in communicator
    int flags;          // flags of new communicator
    int *coordinators;  // cluster coordinators
    int *cluster_sizes; // cluster sizes
    int *cluster_ranks; // cluster ranks
    uint32_t *members;  // communicator members
    uint32_t *member_cluster_index; // members cluster indeces
    uint32_t *local_ranks; // members local ranks
} group_reply;

// This struct is used to return the servers reply to a dup request.
typedef struct {
//    int comm;    // communicator used
//    int src;     // source rank (unused)
    int newComm; // communicator created
} dup_reply;

// Initialize and finalize the messaging layer
int messaging_init(int rank, int size, int *argc, char ***argv);
int messaging_finalize();

// Run the messaging gateway (if applicable).
//int messaging_run_gateway(int rank, int size, int empi_size);

// Broadcast messages (to all cluster coordinators).
int messaging_bcast(void* buf, int count, datatype *t, int root, communicator* c);
int messaging_bcast_receive(void *buf, int count, datatype *t, int root, communicator* c);

// Finalize a pending receive request.
int messaging_finalize_receive(request *r, status *s);

// Send and receive messages (to any -remote- participant).

// FIXME: split send into syns and async version -- just like receive vs probe!
int messaging_send(void* buf, int count, datatype *t, int dest, int tag, communicator* c, request *req, bool needs_ack);
int messaging_receive(void *buf, int count, datatype *t, int source, int tag, status *s, communicator* c);

// Probe if a message is available.
int messaging_probe_receive(request *r, int blocking);


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

// Returns is a data message matches a given communicator, source and tag.
int match_message(data_message *m, int comm, int source, int tag);

int messaging_poll_receive_queue(request *r);
void messaging_post_receive(request *r);
int messaging_poll_sends(bool blocking);
int messaging_poll_receives();
int messaging_poll();
int messaging_peek_receive_queue(request *r);

#endif // _MESSAGING_H_
