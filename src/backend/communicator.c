#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shared.h"
#include "empi.h"
#include "types.h"
#include "communicator.h"
#include "messaging.h"
#include "logging.h"

extern uint32_t cluster_rank;

static communicator *comms[MAX_COMMUNICATORS];

static int add_communicator(int handle, MPI_Comm comm, int initial,
                           int local_rank, int local_size, int global_rank, int global_size,
                           int cluster_count, int *coordinators, int *cluster_sizes,
                           int flags, uint32_t *members,
                           int *cluster_ranks, uint32_t *member_cluster_index, uint32_t *local_ranks,
                           communicator **out)
{
   int i; //, start, end, local, remote;

   if (handle < 0 || handle >= MAX_COMMUNICATORS) {
      ERROR(1, "Ran out of communicator storage (%d)!", handle);
      return EMPI_ERR_INTERN;
   }

   if (initial == 0 && handle < 3) {
      ERROR(1, "Attempting to overwrite reserved communicator (%d)!", handle);
      return EMPI_ERR_INTERN;
   }

   if (comms[handle] != NULL) {
      ERROR(1, "Attempting to overwrite existing communicator (%d)!", handle);
      return EMPI_ERR_INTERN;
   }

   INFO(0, "Creating communicator %d : local(%d %d) | global(%d %d)",
           handle, local_rank, local_size, global_rank, global_size);

   communicator *c = malloc(sizeof(communicator));

   if (c == NULL) {
      ERROR(1, "Failed to allocate space for communicator (%d)!", handle);
      return EMPI_ERR_INTERN;
   }

   c->handle = handle;
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
      for (i=0;i<c->cluster_count;i++) {
         if (c->cluster_ranks[i] == cluster_rank) {
            c->my_coordinator = c->coordinators[i];
         }
      }

   } else {
      c->my_coordinator = 0;
   }

   comms[handle] = c;

#if GATHER_STATISTICS
   for (i=0;i<STATS_TOTAL;i++) {
      comms[handle]->counters[i] = 0L;
   }
#endif

   if (out != NULL) {
      *out = c;
   }

   return EMPI_SUCCESS;
}


int init_communicators(int rank, int size, int empi_rank, int empi_size, int cluster_rank, int cluster_count,  int *cluster_sizes, int *cluster_offsets)
{
   // We create three special communicators here for
   // EMPI_COMM_WORLD, EMPI_COMM_SELF and EMPI_COMM_NULL.
   int global_rank, global_count, tmp_process_rank, tmp_cluster_rank;
   int i, error, flags;

   uint32_t *members;
   int *coordinators;

   int *cluster_ranks;
   uint32_t *member_cluster_index;
   uint32_t *local_ranks;

   MPI_Comm world;
   MPI_Group group_world, group_empi_world;

   int *excluded;
   int gateways;
   int new_rank, new_size;

   // We first need to check if we must create an alternative MPI Communicator to replace
   // MPI_COMM_WORLD. Some of the MPI processes may not participate in the application, but
   // serve as gateways instead. These processes will NOT be visible to the application. It
   // depends on the implementation of wide area messaging ig gateways are used.

   if (size == empi_size) {
      // We do not need to replace MPI_COMM_WORLD.
      world = MPI_COMM_WORLD;
   } else {

      gateways = size - empi_size;

      if (gateways <= 0) {
         ERROR(1, "Failed to create EMPI_COMM_WORLD, negative number of gateways processes! (%d)", gateways);
         return EMPI_ERR_INTERN;
      }

      error = MPI_Comm_group(MPI_COMM_WORLD, &group_world);

      if (error != MPI_SUCCESS) {
         ERROR(1, "Failed to create EMPI_COMM_WORLD, could not retrieve group! (error=%d)", error);
         return EMPI_ERR_INTERN;
      }

      excluded = malloc(sizeof(int) * gateways);

      if (excluded == NULL) {
         ERROR(1, "Failed to create EMPI_COMM_WORLD, no space left for allocation!");
         return EMPI_ERR_INTERN;
      }

      for (i=0;i<gateways;i++) {
         excluded[i] = empi_size + i;
      }

      error = MPI_Group_excl(group_world, gateways, excluded, &group_empi_world);

      if (error != MPI_SUCCESS) {
         ERROR(1, "Failed to create EMPI_COMM_WORLD, could not exclude gateway nodes! (error=%d)", error);
         return EMPI_ERR_INTERN;
      }

      error = MPI_Comm_create(MPI_COMM_WORLD, group_empi_world, &world);

      if (error != MPI_SUCCESS) {
         ERROR(1, "Failed to create EMPI_COMM_WORLD, could not create communicator without gateway nodes! (error=%d)", error);
         return EMPI_ERR_INTERN;
      }

      // Intercept all gateway processes here, as they do not need to continue initalization.
      if (empi_rank == -1) {

         if (world == MPI_COMM_NULL) {
            return EMPI_SUCCESS;
         }

         // EMPI and MPI disagree on the status of this process!
         ERROR(1, "Inconsistent EMPI_COMM_WORLD: mismatch between EMPI and MPI!");
         return EMPI_ERR_INTERN;
      }

      // Sanity check to ensure MPI and MPI agree on the new ranks and size.
      error = MPI_Comm_rank(world, &new_rank);

      if (error != MPI_SUCCESS) {
         ERROR(1, "Failed to retrieve rank from WORLD! (error=%d)", error);
         return EMPI_ERR_INTERN;
      }

      if (empi_rank != new_rank) {
         ERROR(1, "Rank mismatch between EMPI_COMM_WORLD (%d) and WORLD (%d)!", empi_rank, new_rank);
         return EMPI_ERR_INTERN;
      }

      error = MPI_Comm_size(world, &new_size);

      if (error != MPI_SUCCESS) {
         ERROR(1, "Failed to retrieve size from WORLD! (error=%d)", error);
         return EMPI_ERR_INTERN;
      }

      if (empi_size != new_size) {
         ERROR(1, "Size mismatch between EMPI_COMM_WORLD (%d) and WORLD (%d)!", empi_size, new_size);
         return EMPI_ERR_INTERN;
      }

      // Succesfully created a subset of MPI_COMM_WORLD!
   }

   // Create EMPI_COMM_WORLD
   global_rank = cluster_offsets[cluster_rank]+empi_rank;
   global_count = cluster_offsets[cluster_count];

   coordinators = malloc(cluster_count * sizeof(int));

   if (coordinators == NULL) {
      ERROR(1, "Failed to allocate space for communicator (coordinators)!");
      return EMPI_ERR_INTERN;
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
      return EMPI_ERR_INTERN;
   }

   for (i=0;i<cluster_count;i++) {
      cluster_ranks[i] = i;
   }

   members = malloc(global_count * sizeof(uint32_t));

   if (members == NULL) {
      ERROR(1, "Failed to allocate space for communicator (members)!");
      free(coordinators);
      free(cluster_ranks);
      return EMPI_ERR_INTERN;
   }

   member_cluster_index = malloc(global_count * sizeof(uint32_t));

   if (member_cluster_index == NULL) {
      ERROR(1, "Failed to allocate space for communicator (member_cluster_index)!");
      free(coordinators);
      free(cluster_ranks);
      free(members);
      return EMPI_ERR_INTERN;
   }

   local_ranks = malloc(global_count * sizeof(uint32_t));

   if (local_ranks == NULL) {
      ERROR(1, "Failed to allocate space for communicator (local_ranks)!");
      free(coordinators);
      free(cluster_ranks);
      free(members);
      free(member_cluster_index);
      return EMPI_ERR_INTERN;
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
   error = add_communicator(EMPI_COMM_WORLD, world, 1,
                            empi_rank, empi_size, global_rank, global_count,
                            cluster_count, coordinators, cluster_sizes,
                            flags, members,
                            cluster_ranks, member_cluster_index, local_ranks,
                            NULL);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to create MPI_COMM_WORLD!");
      return error;
   }

   // Create MPI_COMM_SELF
   members = malloc(sizeof(uint32_t));

   if (members == NULL) {
      ERROR(1, "Failed to allocate space for communicator (members -- self)!");
      return EMPI_ERR_INTERN;
   }

   coordinators = malloc(sizeof(int));

   if (coordinators == NULL) {
      ERROR(1, "Failed to allocate space for communicator (coordinators -- self)!");
      return EMPI_ERR_INTERN;
   }

   cluster_sizes = malloc(sizeof(int));

   if (cluster_sizes == NULL) {
      ERROR(1, "Failed to allocate space for communicator (cluster_sizes -- self)!");
      return EMPI_ERR_INTERN;
   }

   member_cluster_index = malloc(sizeof(uint32_t));

   if (member_cluster_index == NULL) {
      ERROR(1, "Failed to allocate space for communicator (member_cluster_index -- self)!");
      return EMPI_ERR_INTERN;
   }

   local_ranks = malloc(sizeof(uint32_t));

   if (local_ranks == NULL) {
      ERROR(1, "Failed to allocate space for communicator (local_ranks -- self)!");
      return EMPI_ERR_INTERN;
   }

   members[0] = my_pid;
   coordinators[0] = 0;
   cluster_sizes[0] = 1;
   cluster_ranks[0] = 0;
   member_cluster_index[0] = 0;
   local_ranks[0] = 0;

   flags = COMM_FLAG_SELF | COMM_FLAG_LOCAL;

   // FIXME: this will fail hopelessly if FORTRAN_MPI_COMM_SELF has a weird value!
   error = add_communicator(EMPI_COMM_SELF, MPI_COMM_SELF, 1,
                            0, 1, 0, 1, 1, coordinators, cluster_sizes, flags, members,
                            cluster_ranks, member_cluster_index, local_ranks, NULL);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to create MPI_COMM_SELF!");
      return error;
   }

   // FIXME: this will fail hopelessly if FORTRAN_MPI_COMM_NULL has a weird value!
   error = add_communicator(EMPI_COMM_NULL, MPI_COMM_NULL, 1,
                            0, 0, 0, 0, 0, NULL, NULL, 0, NULL,
                            NULL, NULL, NULL, NULL);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "Failed to create MPI_COMM_NULL!");
   }

   return error;
}

int create_communicator(int handle, MPI_Comm comm,
         int local_rank, int local_size, int global_rank, int global_size,
         int cluster_count, int *coordinators, int *cluster_sizes,
         int flags, uint32_t *members,
         int *cluster_ranks, uint32_t *member_cluster_index, uint32_t *local_ranks,
         communicator **out)
{
   return add_communicator(handle, comm, 0,
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

   comms[c->handle] = NULL;

   free(c->coordinators);
   free(c->cluster_sizes);
   free(c->members);
   free(c);

   return error;
}

communicator *handle_to_communicator(int handle)
{
   if (handle < 0 || handle >= MAX_COMMUNICATORS) {
      ERROR(1, "handle_to_communicator(handle=%d) handle out of bounds!", handle);
      return NULL;
   }

   if (comms[handle] == NULL) {
      ERROR(1, "handle_to_communicator(handle=%d) communicator not found!", handle);
      return NULL;
   }

   return comms[handle];
}

int communicator_to_handle(communicator *c)
{
   if (c == NULL) {
      ERROR(1, "communicator_to_handle(c=NULL) communicator is NULL!");
      return -1;
   }

   return c->handle;
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
      ERROR(1, "Failed to find communicator %d in store_message!", m->header.comm);
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
                 (source == EMPI_ANY_SOURCE || source == m->header.source) &&
                 (tag == EMPI_ANY_TAG || tag == m->header.tag));

   DEBUG(5, "MATCH_MESSAGE: (comm=%d source=%d [any=%d] tag=%d [any=%d]) == (m.comm=%d m.source=%d m.tag=%d) => %d",
	comm, source, EMPI_ANY_SOURCE, tag, EMPI_ANY_TAG,
        m->header.comm, m->header.source, m->header.tag, result);

   return result;
}

message_buffer *find_pending_message(communicator *c, int source, int tag)
{
   message_buffer *curr, *prev;

   DEBUG(4, "FIND_PENDING_MESSAGE: Checking for pending messages in comm=%d from source=%d tag=%d", c->handle, source, tag);

   if (c->queue_head == NULL) {
      DEBUG(4, "FIND_PENDING_MESSAGE: No pending messages");
      return NULL;
   }

   curr = c->queue_head;
   prev = NULL;

   while (curr != NULL) {

      if (match_message(curr, c->handle, source, tag)) {
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

