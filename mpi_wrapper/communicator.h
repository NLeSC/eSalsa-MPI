#ifndef _COMMUNICATOR_H_
#define _COMMUNICATOR_H_

#include "flags.h"
#include "mpi.h"
#include "types.h"
#include "stdint.h"

#define COMM_FLAG_LOCAL  (1 << 0)
#define COMM_FLAG_REMOTE (1 << 1)
#define COMM_FLAG_WORLD  (1 << 2)
#define COMM_FLAG_SELF   (1 << 3)

#define COMM_OK                  0
#define COMM_ERROR_ALLOCATE      1
#define COMM_ERROR_COMMUNICATORS 2

struct s_communicator {

   int handle;
   int flags;

   MPI_Comm comm;

   int local_rank;
   int local_size;

   int global_rank;
   int global_size;

   int my_coordinator;
   int *coordinators;

   int cluster_count;

   // FIXME: Merge cluster ranks + sizes into single cluster info field ?
   int *cluster_ranks;
   int *cluster_sizes;

   uint32_t *members;

   // FIXME: Merge member_cluster_index + local_ranks into single local pid array ?
   uint32_t *member_cluster_index;
   uint32_t *local_ranks;

//   unsigned char *bitmap;

   message_buffer *queue_head;
   message_buffer *queue_tail;
};

// Communicator mapping
int init_communicators(int cluster_rank, int cluster_count,
                       int* cluster_sizes, int *cluster_offsets);

int create_communicator(int handle, MPI_Comm comm,
         int local_rank, int local_size, int global_rank, int global_size,
         int cluster_count, int *coordinators, int *cluster_sizes,
         int flags, uint32_t *members,
         int *cluster_ranks, uint32_t *member_cluster_index, uint32_t *local_ranks,
         communicator **out);

int free_communicator(communicator * c);

communicator *handle_to_communicator(int handle);
int communicator_to_handle(communicator *c);

//communicator *get_communicator(MPI_Comm comm);
//communicator *get_communicator_with_index(int index);

int comm_dup(communicator *in, communicator **out);

int comm_cluster_rank_to_cluster_index(communicator *c, int cluster_rank);

//int rank_is_local(communicator *c, int rank, int *result);

void store_message(message_buffer *m);
message_buffer *find_pending_message(communicator *c, int source, int tag);
int match_message(message_buffer *m, int comm, int source, int tag);

// FIXME: These are all one-liners that should be inlined ?
void set_communicator_ptr(MPI_Comm *dst, communicator *src);
int comm_is_world(communicator* c);
int comm_is_self(communicator* c);
int comm_is_local(communicator* c);
int comm_is_wa(communicator* c);
int comm_is_mixed(communicator* c);
int rank_is_local(communicator *c, int rank);
int rank_is_remote(communicator *c, int rank);

int get_local_rank(communicator *c, int rank);
int get_cluster_rank(communicator *c, int rank);
int get_global_rank(communicator *c, int cluster, int rank);


#endif // _COMMUNICATOR_H_
