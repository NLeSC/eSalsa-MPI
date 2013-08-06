#include "flags.h"

#ifdef IBIS_INTERCEPT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shared.h"

#include "mpi.h"

#include "types.h"
#include "communicator.h"
#include "messaging.h"
#include "logging.h"

extern uint32_t cluster_rank;

static communicator *comms[MAX_COMMUNICATORS];

static int add_communicator(MPI_Comm comm, int number, int initial,
                           int local_rank, int local_size, int global_rank, int global_size,
                           int cluster_count, int *coordinators, int *cluster_sizes,
                           int flags, uint32_t *members,
                           int *cluster_ranks, uint32_t *member_cluster_index, uint32_t *local_ranks,
                           communicator **out)
{
   int i; //, start, end, local, remote;

   if (number < 0 || number >= MAX_COMMUNICATORS) {
      ERROR(1, "Ran out of communicator storage (%d)!", number);
      return MPI_ERR_INTERN;
   }

   if (initial == 0 && number < 3) {
      ERROR(1, "Attempting to overwrite reserved communicator (%d)!", number);
      return MPI_ERR_INTERN;
   }

   if (comms[number] != NULL) {
      ERROR(1, "Attempting to overwrite existing communicator (%d)!", number);
      return MPI_ERR_INTERN;
   }

   INFO(0, "Creating communicator %d : local(%d %d) | global(%d %d)",
           number, local_rank, local_size, global_rank, global_size);

   communicator *c = malloc(sizeof(communicator));

   if (c == NULL) {
      ERROR(1, "Failed to allocate space for communicator (%d)!", number);
      return MPI_ERR_INTERN;
   }

   c->number = number;
   c->flags = flags;
   c->comm = comm;
   c->local_rank = local_rank;
   c->local_size = local_size;
   c->global_rank = global_rank;
   c->global_size = global_size;
   c->cluster_count = cluster_count;
   c->queue_head = NULL;
   c->queue_tail = NULL;
   c->coordinators = coordinators;
   c->cluster_sizes = cluster_sizes;
   c->members = members;
   c->cluster_ranks = cluster_ranks;
   c->member_cluster_index = member_cluster_index;
   c->local_ranks = local_ranks;

   if (c->cluster_count > 0) {
/*
      c->cluster_ranks = malloc(c->cluster_count * sizeof(unsigned char));

      if (c->cluster_ranks == NULL) {
         ERROR(1, "Failed to allocate space for communicator (%d)!", number);
         return MPI_ERR_INTERN;
      }
*/
      for (i=0;i<c->cluster_count;i++) {
//         c->cluster_ranks[i] = (unsigned char) GET_CLUSTER_RANK(c->members[c->coordinators[i]]);

         if (c->cluster_ranks[i] == cluster_rank) {
            c->my_coordinator = c->coordinators[i];
         }
      }

   } else {
//      c->cluster_ranks = NULL;
      c->my_coordinator = 0;
   }

/*
   if (c->global_size > 0) {

      c->member_cluster_index = malloc(c->global_size * sizeof(uint8_t));

      if (c->member_cluster_index == NULL) {
         ERROR(1, "Failed to allocate space for communicator (%d)!", number);
         return MPI_ERR_INTERN;
      }

      c->local_ranks = malloc(c->global_size * sizeof(uint32_t));

      if (c->local_ranks == NULL) {
         ERROR(1, "Failed to allocate space for communicator (%d)!", number);
         return MPI_ERR_INTERN;
      }

      tmp = malloc(c->cluster_count * sizeof(int));

      if (tmp == NULL) {
         ERROR(1, "Failed to allocate space for communicator (%d)!", number);
         return MPI_ERR_INTERN;
      }

      for (i=0;i<c->global_size;i++) {



         c->member_cluster_index[i] = (uint8_t) comm_cluster_rank_to_cluster_index(c, GET_CLUSTER_RANK(c->members[i]));
      }

      free(tmp);

   } else {
      c->member_cluster_index = NULL;
   }
*/

   comms[number] = c;

#if GATHER_STATISTICS
   for (i=0;i<STATS_TOTAL;i++) {
      comms[number]->counters[i] = 0L;
   }
#endif

   if (out != NULL) {
      *out = c;
   }

   return MPI_SUCCESS;
}

int init_communicators(int cluster_rank, int cluster_count,
                       int *cluster_sizes, int *cluster_offsets)
{
   // We create three special communicators here for
   // MPI_COMM_WORLD, MPI_COMM_SELF and MPI_COMM_NULL.
   int global_rank, global_count, tmp_process_rank, tmp_cluster_rank;
   int i, error, flags;

   uint32_t *members;
   int *coordinators;

   int *cluster_ranks;
   uint32_t *member_cluster_index;
   uint32_t *local_ranks;

   // Create MPI_COMM_WORLD
   global_rank = cluster_offsets[cluster_rank]+local_rank;
   global_count = cluster_offsets[cluster_count];

   coordinators = malloc(cluster_count * sizeof(int));

   if (coordinators == NULL) {
      ERROR(1, "Failed to allocate space for communicator (coordinators)!");
      return MPI_ERR_INTERN;
   }

   for (i=0;i<cluster_count;i++) {
      coordinators[i] = cluster_offsets[i];
   }

   for (i=0;i<MAX_COMMUNICATORS;i++) {
      comms[i] = NULL;
   }

   cluster_ranks = malloc(cluster_count * sizeof(int));

   if (cluster_ranks == NULL) {
      ERROR(1, "Failed to allocate space for communicator (cluster_ranks)!");
      free(coordinators);
      return MPI_ERR_INTERN;
   }

   for (i=0;i<cluster_count;i++) {
      cluster_ranks[i] = i;
   }

   members = malloc(global_count * sizeof(uint32_t));

   if (members == NULL) {
      ERROR(1, "Failed to allocate space for communicator (members)!");
      free(coordinators);
      free(cluster_ranks);
      return MPI_ERR_INTERN;
   }

   member_cluster_index = malloc(global_count * sizeof(uint32_t));

   if (member_cluster_index == NULL) {
      ERROR(1, "Failed to allocate space for communicator (member_cluster_index)!");
      free(coordinators);
      free(cluster_ranks);
      free(members);
      return MPI_ERR_INTERN;
   }

   local_ranks = malloc(global_count * sizeof(uint32_t));

   if (local_ranks == NULL) {
      ERROR(1, "Failed to allocate space for communicator (local_ranks)!");
      free(coordinators);
      free(cluster_ranks);
      free(members);
      free(member_cluster_index);
      return MPI_ERR_INTERN;
   }

   tmp_process_rank = 0;
   tmp_cluster_rank = 0;

   for (i=0;i<global_count;i++) {

      if (i >= cluster_offsets[tmp_cluster_rank+1]) {
         tmp_process_rank = 0;
         tmp_cluster_rank++;
      }

      members[i] = SET_PID(tmp_cluster_rank, tmp_process_rank);
      member_cluster_index[i] = tmp_cluster_rank;
      local_ranks[i] = tmp_process_rank;

      tmp_process_rank++;
   }

   flags = COMM_FLAG_WORLD | COMM_FLAG_LOCAL;

   if (cluster_count > 1) {
      flags |= COMM_FLAG_REMOTE;
   }

   // FIXME: this will fail hopelessly if FORTRAN_MPI_COMM_WORLD has a weird value!
   error = add_communicator(MPI_COMM_WORLD, FORTRAN_MPI_COMM_WORLD, 1,
                            local_rank, local_count, global_rank, global_count,
                            cluster_count, coordinators, cluster_sizes,
                            flags, members,
                            cluster_ranks, member_cluster_index, local_ranks,
                            NULL);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to create MPI_COMM_WORLD!");
      return error;
   }

   // Create MPI_COMM_SELF
   members = malloc(sizeof(uint32_t));

   if (members == NULL) {
      ERROR(1, "Failed to allocate space for communicator (members -- self)!");
      return MPI_ERR_INTERN;
   }

   coordinators = malloc(sizeof(int));

   if (coordinators == NULL) {
      ERROR(1, "Failed to allocate space for communicator (coordinators -- self)!");
      return MPI_ERR_INTERN;
   }

   cluster_sizes = malloc(sizeof(int));

   if (cluster_sizes == NULL) {
      ERROR(1, "Failed to allocate space for communicator (cluster_sizes -- self)!");
      return MPI_ERR_INTERN;
   }

   member_cluster_index = malloc(sizeof(uint32_t));

   if (member_cluster_index == NULL) {
      ERROR(1, "Failed to allocate space for communicator (member_cluster_index -- self)!");
      return MPI_ERR_INTERN;
   }

   local_ranks = malloc(sizeof(uint32_t));

   if (local_ranks == NULL) {
      ERROR(1, "Failed to allocate space for communicator (local_ranks -- self)!");
      return MPI_ERR_INTERN;
   }

   members[0] = my_pid;
   coordinators[0] = 0;
   cluster_sizes[0] = 1;
   cluster_ranks[0] = 0;
   member_cluster_index[0] = 0;
   local_ranks[0] = 0;

   flags = COMM_FLAG_SELF | COMM_FLAG_LOCAL;

   // FIXME: this will fail hopelessly if FORTRAN_MPI_COMM_SELF has a weird value!
   error = add_communicator(MPI_COMM_SELF, FORTRAN_MPI_COMM_SELF, 1,
                            0, 1, 0, 1, 1, coordinators, cluster_sizes, flags, members,
                            cluster_ranks, member_cluster_index, local_ranks, NULL);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to create MPI_COMM_SELF!");
      return error;
   }

   // FIXME: this will fail hopelessly if FORTRAN_MPI_COMM_NULL has a weird value!
   error = add_communicator(MPI_COMM_NULL, FORTRAN_MPI_COMM_NULL, 1,
                            0, 0, 0, 0, 0, NULL, NULL, 0, NULL,
                            NULL, NULL, NULL, NULL);

   if (error != MPI_SUCCESS) {
      ERROR(1, "Failed to create MPI_COMM_NULL!");
   }

   return error;
}

int create_communicator(MPI_Comm comm, int number,
         int local_rank, int local_size, int global_rank, int global_size,
         int cluster_count, int *coordinators, int *cluster_sizes,
         int flags, uint32_t *members,
         int *cluster_ranks, uint32_t *member_cluster_index, uint32_t *local_ranks,
         communicator **out)
{
   return add_communicator(comm, number, 0,
                    local_rank, local_size, global_rank, global_size,
                    cluster_count, coordinators, cluster_sizes,
                    flags, members,
                    cluster_ranks, member_cluster_index, local_ranks,
                    out);
}

int free_communicator(communicator * c)
{
   int error;

   MPI_Comm comm = c->comm;

   error = PMPI_Comm_free(&comm);

   comms[c->number] = NULL;

   free(c->coordinators);
   free(c->cluster_sizes);
   free(c->members);
   free(c);

   return error;
}

communicator* get_communicator(MPI_Comm comm)
{
   if (comm == MPI_COMM_WORLD) {
      return comms[FORTRAN_MPI_COMM_WORLD];
   } else if (comm == MPI_COMM_SELF) {
      return comms[FORTRAN_MPI_COMM_SELF];
   } else if (comm == MPI_COMM_NULL) {
      return comms[FORTRAN_MPI_COMM_NULL];
   }

   return (communicator *) comm;
}

communicator *get_communicator_with_index(int index)
{
   if (index < 0 || index >= MAX_COMMUNICATORS) {
      ERROR(1, "get_communicator_with_index(index=%d) index out of bounds!", index);
      return NULL;
   }

   if (comms[index] == NULL) {
      ERROR(1, "get_communicator_with_index(index=%d) communicator not found!", index);
      return NULL;
   }

   return comms[index];
}

int comm_cluster_rank_to_cluster_index(communicator *c, int cluster_rank)
{
   int i;

   // FIXME: ouch!
   for (i=0;i<c->cluster_count;i++) {
      if (c->cluster_ranks[i] == cluster_rank) {
         return i;
      }
   }

   return -1;
}

void set_communicator_ptr(MPI_Comm *dst, communicator *src)
{
   memcpy(dst, &src, sizeof(communicator *));
}

int comm_is_world(communicator* c)
{
   return (c->flags & COMM_FLAG_WORLD) != 0;
}

int comm_is_self(communicator* c)
{
   return (c->flags & COMM_FLAG_SELF) != 0;
}

int comm_is_local(communicator* c)
{
   return ((c->flags & COMM_FLAG_LOCAL) != 0) && ((c->flags & COMM_FLAG_REMOTE) == 0);
}

int comm_is_wa(communicator* c)
{
   return ((c->flags & COMM_FLAG_LOCAL) == 0) && ((c->flags & COMM_FLAG_REMOTE) != 0);
}

int comm_is_mixed(communicator* c)
{
   return ((c->flags & COMM_FLAG_LOCAL) != 0) && ((c->flags & COMM_FLAG_REMOTE) != 0);
}

int rank_is_local(communicator *c, int rank)
{
   return (GET_CLUSTER_RANK(c->members[rank]) == cluster_rank);
}

int rank_is_remote(communicator *c, int rank)
{
   return (GET_CLUSTER_RANK(c->members[rank]) != cluster_rank);
}

int get_local_rank(communicator *c, int rank)
{
   if (rank < 0 || rank >= c->global_size) {
      FATAL("Illegal global rank %d!", rank);
   }

   return c->local_ranks[rank];
}

int get_global_rank(communicator *c, int cluster, int rank)
{
   int i, cluster_index;

   cluster_index = c->member_cluster_index[c->global_rank];

   for (i=0;i<c->global_size;i++) {
      if (c->member_cluster_index[i] == cluster_index && c->local_ranks[i] == rank) {
         return i;
      }
   }

   // We failed to find the global rank. Print some info for debugging
   INFO(1, "FAILED TO CONVERT LOCAL TO GLOBAL RANK!");

   for (i=0;i<c->global_size;i++) {
      INFO(2, "index / cluster %d != %d / rank %d != %d", i, c->member_cluster_index[i], cluster_index, c->local_ranks[i], rank);

      if (c->member_cluster_index[i] == cluster_index && c->local_ranks[i] == rank) {
         return i;
      }
   }

   FATAL("Failed to translate local rank (%d:%d) to global rank!", cluster, rank);
   return -1;
}

int get_cluster_rank(communicator *c, int rank)
{
   return GET_CLUSTER_RANK(c->members[rank]);
}

void store_message(message_buffer *m)
{
   communicator* c = comms[m->header.comm];

   if (c == NULL) {
      ERROR(1, "Failed to find communicator %d in store_message!",
		m->header.comm);
      ERROR(1, "Dropping message!");
      return;
   }

   m->next = NULL;

   if (c->queue_head == NULL) {
      c->queue_head = c->queue_tail = m;
   } else { 
      c->queue_tail->next = m;
      c->queue_tail = m;
   }
}

int match_message(message_buffer *m, int comm, int source, int tag)
{
   int result = ((comm == m->header.comm) &&
                 (source == MPI_ANY_SOURCE || source == m->header.source) &&
                 (tag == MPI_ANY_TAG || tag == m->header.tag));

   DEBUG(5, "MATCH_MESSAGE: (comm=%d source=%d [any=%d] tag=%d [any=%d]) == (m.comm=%d m.source=%d m.tag=%d) => %d",
	comm, source, MPI_ANY_SOURCE, tag, MPI_ANY_TAG,
        m->header.comm, m->header.source, m->header.tag, result);

   return result;
}

message_buffer *find_pending_message(communicator *c, int source, int tag)
{
   message_buffer *curr, *prev;

   DEBUG(4, "FIND_PENDING_MESSAGE: Checking for pending messages in comm=%d from source=%d tag=%d", c->number, source, tag);

   if (c->queue_head == NULL) {
      DEBUG(4, "FIND_PENDING_MESSAGE: No pending messages");
      return NULL;
   }

   curr = c->queue_head;
   prev = NULL;

   while (curr != NULL) {

      if (match_message(curr, c->number, source, tag)) {
          if (curr == c->queue_head) {
              // delete head. check if list empty
              if (c->queue_head == c->queue_tail) {
                 c->queue_head = c->queue_tail = NULL;
              } else {
                 c->queue_head = c->queue_head->next;
              }

          } else if (curr == c->queue_tail) {
              // delete tail. set tail to prev
              c->queue_tail = prev;
              c->queue_tail->next = NULL;
          } else {
              // delete middle.
              prev->next = curr->next;
          }

          curr->next = NULL;
          DEBUG(4, "FIND_PENDING_MESSAGE: Found pending message from %d", curr->header.source);
          return curr;
      }

      prev = curr;
      curr = curr->next;
   }

   DEBUG(4, "FIND_PENDING_MESSAGE: No matching messages");

   return NULL;
}

/*
MPI_Comm comm_f2c(int f)
{
   int i;
   MPI_Comm res;

fprintf(stderr, "   JASON: comm_f2c(%d)\n", f);

   if (f == -1) {
fprintf(stderr, "   JASON: comm_f2c(%d) return null\n", f);
      return MPI_COMM_NULL;
   }

   for (i=0;i<MAX_COMMUNICATORS;i++) {

      if (comms[i] != NULL) {

fprintf(stderr, "   JASON: comm_f2c(%d) %p %p\n", f, comms[i], comms[i]->comm);

         if (PMPI_Comm_c2f(comms[i]->comm) == f) {

fprintf(stderr, "   FOUND MATCHING COMM!\n");
            set_communicator_ptr(&res, comms[i]);
            return res;
         }
      }
   }

   return MPI_COMM_NULL;
}
*/

#endif // IBIS_INTERCEPT
