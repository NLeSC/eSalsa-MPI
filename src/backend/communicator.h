#ifndef _COMMUNICATOR_H_
#define _COMMUNICATOR_H_

#include "../../include/settings.h"
#include "empi.h"
#include "types.h"
#include "stdint.h"
#include "mpi.h"
#include "message_queue.h"

#define COMM_FLAG_LOCAL  (1 << 0)
#define COMM_FLAG_REMOTE (1 << 1)
#define COMM_FLAG_WORLD  (1 << 2)
#define COMM_FLAG_SELF   (1 << 3)

#define COMM_OK                  0
#define COMM_ERROR_ALLOCATE      1
#define COMM_ERROR_COMMUNICATORS 2

struct s_communicator {

   /* The handle of this communicator. */
   int handle;

   /* DO WE NEED THESE ?? */
   int flags;

   /* The MPI handle for the local part of the communicator. */
   MPI_Comm comm;

   /* Our rank in the local communicator */
   int local_rank;

   /* The size of the local communicator */
   int local_size;

   /* Our rank in the global communicator */
   int global_rank;

   /* The size of the global communicator */
   int global_size;

   /* The global rank of the coordinator of my cluster. */
   int my_coordinator;

   /* The global ranks of all cluster coordinators (one per cluster, with local rank 0). */
   int *coordinators;

   /* The number of clusters participating in this communicator. */
   int cluster_count;

   // FIXME: Merge cluster ranks + sizes into single cluster info field ?

   /* The ranks of each of the participating clusters. */
   int *cluster_ranks;

   /* The size of each local communicator on the participating clusters. */
   int *cluster_sizes;

   /* The PID of all (read: global) members of this communicator. */
   uint32_t *members;

   // FIXME: Merge member_cluster_index + local_ranks into single local pid array ?

   /* The cluster index for each of the global members. REPLACE WITH GET_CLUSTER_RANK(members[i]). */
   uint32_t *member_cluster_index;

   /* The local rank for each of the global members. REPLACE WITH GET_PROCESS_RANK(members[i]). */
   uint32_t *local_ranks;

   data_message_queue queue;

   /* Head of the message queue. */
//   message_buffer *queue_head;

   /* Tail of the message queue. */
//   message_buffer *queue_tail;
};

// Communicator mapping
int init_communicators(int rank, int size, MPI_Comm world,
                       int cluster_rank, int cluster_count,
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

void store_message(data_message *m);

data_message *find_pending_message(communicator *c, int source, int tag);

// int match_message(data_message *m, int comm, int source, int tag);

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
int get_pid(communicator *c, int rank);

#endif // _COMMUNICATOR_H_
