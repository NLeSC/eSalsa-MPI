#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "shared.h"

#include "empi.h"
#include "types.h"
#include "group.h"
#include "communicator.h"
#include "logging.h"


static group *groups[MAX_GROUPS];
static int next_group = 2;

// A group is an ordered set of process identifiers (henceforth processes); processes are implementation-dependent
// objects. Each process in a group is associated with an integer rank. Ranks are contiguous and start from zero.
// Groups are represented by opaque group objects, and hence cannot be directly transferred from one process to
// another. A group is used within a communicator to describe the participants in a communication ``universe'' and
// to rank such participants (thus giving them unique names within that ``universe'' of communication).
//
// There is a special pre-defined group: EMPI_GROUP_EMPTY, which is a group with no members. The predefined constant
// EMPI_GROUP_NULL is the value used for invalid group handles.

int init_groups()
{
   int i;

   groups[EMPI_GROUP_NULL] = NULL;
   groups[EMPI_GROUP_EMPTY] = malloc(sizeof(group));

   if (groups[EMPI_GROUP_EMPTY] == NULL) {
      ERROR(1, "Failed to initialize groups!");
      return EMPI_ERR_INTERN;
   }

   groups[EMPI_GROUP_EMPTY]->size = 0;
   groups[EMPI_GROUP_EMPTY]->rank = -1;
   groups[EMPI_GROUP_EMPTY]->members = NULL;

   for (i=2;i<MAX_GROUPS;i++) {
      groups[i] = NULL;
   }

   return EMPI_SUCCESS;
}

//static int is_special(int index)
//{
//   return (index == EMPI_GROUP_NULL || index == EMPI_GROUP_EMPTY);
//}

static int add_group(group *g)
{
   if (next_group >= MAX_GROUPS) {
      ERROR(1, "MAX_GROUPS reached!");
      return -1;
   }

   groups[next_group++] = g;
   return (next_group-1);
}

group *handle_to_group(EMPI_Group handle)
{
   if (handle < 0 || handle >= MAX_GROUPS) {
      ERROR(1, "Group handle %d out of bounds!", handle);
      return NULL;
   }

   return groups[handle];
}

EMPI_Group group_to_handle(group *g)
{
   if (g == NULL) {
      return EMPI_GROUP_NULL;
   }

   return g->handle;
}


// Utility function to print a group value.
/*
void print_group(MPI_Group g)
{
    unsigned char *buf;
    int i;

    buf = (unsigned char *) &g;

    for (i=0;i<sizeof(MPI_Group);i++) {

       if (i == 0) {
          fprintf(stderr, "[%x", buf[i]);
       } else {
          fprintf(stderr, ":%x", buf[i]);
       }
    }
    fprintf(stderr, "]");
}
*/

// Utility function to get a group value.
/*
group *get_group(EMPI_Group src)
{
  group *res;

  if (src == EMPI_GROUP_EMPTY || src == EMPI_GROUP_NULL) {
     WARN(0, "get_group returns NULL!");
     return NULL;
  }

FIXME return special values ?

  memcpy(&res, &src, sizeof(group *));

  return res;
}


group* get_group_with_index(int f)
{
   if (f < 0 || f >= MAX_GROUPS) {
      ERROR(1, "get_group_with_index(index=%d) index out of bounds!", f);
      return NULL;
   }

   if (groups[f] == NULL) {
      ERROR(1, "get_group_with_index(index=%d) index not active!", f);
      return NULL;
   }

   return groups[f];
}
*/
// Utility function to set group ptr.
//void set_group_ptr(EMPI_Group *dst, group *src)
//{
//   memcpy(dst, &src, sizeof(group *));
//}


static group *create_group(int members)
{
   DEBUG(1, "Creating group of size %d", members);

   if (members < 0) {
      ERROR(1, "Invalid group size %d!", members);
      return NULL;
   }

   if (members == 0) {
      return groups[EMPI_GROUP_EMPTY];
   }

   group *res = malloc(sizeof(group));

   if (res == NULL) {
      ERROR(1, "Failed to allocate group!");
      return NULL;
   }

   DEBUG(1, "allocating members array for %d members", members);

   // NOTE: this may alloc an array of length 0!
   res->members = malloc(members * sizeof(uint32_t));

   if (res->members == NULL) {
      ERROR(1, "Failed to allocate group member array!");
      free(res);
      return NULL;
   }

   res->handle = add_group(res);

   DEBUG(1, "index of group is %d", res->handle);

   if (res->handle == -1) {
      ERROR(1, "Failed to store group!");
      if (res->members != NULL) {
         free(res->members);
      }
      free(res);
      return NULL;
   }

   DEBUG(1, "group creating done");

   return res;
}

static void delete_group(group *group)
{
   if (group == NULL || (group == groups[EMPI_GROUP_EMPTY])) {
      return;
   }

   DEBUG(1, "deleting group %d of size %d", group->handle, group->size);

   groups[group->handle] = NULL;

   if (group->members != NULL) {
      free(group->members);
   }

   free(group);
}

// Convert a communicator to a group.
int group_comm_group(communicator *in, group **out)
{
   // TODO: makes sense to cache some of these groups!
   int i;
   group *res;

   if (in == NULL || out == NULL) {
      ERROR(1, "Group_comm_group(in=%p, out=%p) get NULL as input!", in, out);
      return EMPI_ERR_COMM;
   }

   res = create_group(in->global_size);

   if (res == NULL) {
      ERROR(1, "Failed to create new group!");
      return EMPI_ERR_INTERN;
   }

   res->size = in->global_size;
   res->rank = in->global_rank;

   for (i=0;i<in->global_size;i++) {
      res->members[i] = in->members[i];
   }

   *out = res;

   return EMPI_SUCCESS;
}

// Returns the rank of this process in the given group.
int group_rank(group *g, int *rank)
{
   if (g == NULL) {
      return EMPI_ERR_GROUP;
   }

   if (g->rank == -1) {
      *rank = EMPI_UNDEFINED;
   } else {
      *rank = g->rank;
   }

   return EMPI_SUCCESS;
}

// Returns the size of a group.
int group_size(group *g, int *size)
{
   if (g == NULL) {
      return EMPI_ERR_GROUP;
   }

   *size = g->size;

   return EMPI_SUCCESS;
}

// returns all elements of the first group ( group1), followed by all elements of second group ( group2) not in first.
int group_union(group *in1, group *in2, group **out)
{
   group *res;
   int *members;
   int i, j, found, index, size, current, rank = -1;

//fprintf(stderr, "   JASON: Group union %p %p %p", in1, in2, out);

   size = in1->size + in2->size;

//fprintf(stderr, "   JASON: Group union size %d %d %d", in1->size, in2->size, size);

   index = 0;

   if (size > 0) {

      members = malloc(size * sizeof(uint32_t));

      if (members == NULL) {
         ERROR(1, "Failed to allocate temporary group!");
         return EMPI_ERR_INTERN;
      }

      for (i=0;i<in1->size;i++) {
         members[index++] = in1->members[i];
      }

//fprintf(stderr, "   JASON: Group union group 1 copied");

      for (i=0;i<in2->size;i++) {

         found = 0;

         current = in2->members[i];

         for (j=0;j<in1->size;j++) {
            if (current == in1->members[j]) {
               found = 1;
               break;
           }
         }

         if (found == 0) {
            if (current == my_pid) {
               rank = index;
            }

            members[index++] = current;
         }
      }

   } else {
      members = NULL;
   }

//fprintf(stderr, "   JASON: Group union create_group APP %d", index);

   res = create_group(index);

   if (res == NULL) {
      ERROR(1, "Failed to create group!");

      if (members != NULL) {
         free(members);
      }

      return EMPI_ERR_INTERN;
   }

//fprintf(stderr, "   JASON: Group union create_group done %p", res);

   res->size = index;

   if (in1->rank != -1) {
      res->rank = in1->rank;
   } else if (in2->rank != -1) {
      res->rank = rank;
   } else {
      res->rank = -1;
   }

//fprintf(stderr, "   JASON: Group union copy to group");

   if (members != NULL) {
      for (i=0;i<index;i++) {
         res->members[i] = members[i];
      }

      free(members);
   }

//fprintf(stderr, "   JASON: Group union done %p %p!", out, res);

   *out = res;
   return EMPI_SUCCESS;
}

// returns all elements of the first group that are also in the second group, ordered as in first group.
int group_intersection(group *in1, group *in2, group **out)
{
   // TODO: not implemented
   ERROR(1, "group_intersection NOT IMPLEMENTED!");
   return EMPI_ERR_GROUP;
}

// returns all elements of the first group that are not in the second group, ordered as in the first group.
int group_difference(group *in1, group *in2, group **out)
{
   // TODO: not implemented
   ERROR(1, "group_difference NOT IMPLEMENTED!");
   return EMPI_ERR_GROUP;
}

// The function EMPI_GROUP_INCL creates a group newgroup that consists of the n processes in group 'in' with 
// ranks rank[0], , rank[n-1]; the process with rank i in newgroup is the process with rank ranks[i] in group. 
// Each of the n elements of ranks must be a valid rank in group and all elements must be distinct, or else the
// program is erroneous. If n~=~0, then newgroup is EMPI_GROUP_EMPTY. This function can, for instance, be used
// to reorder the elements of a group. See also EMPI_GROUP_COMPARE.

int group_incl(group *in, int n, int ranks[], group **out)
{
   group *res;
   int i, next;

   if (in == NULL) {
      ERROR(1, "Group include got NULL as input!");
      return EMPI_ERR_GROUP;
   }

   res = create_group(n);

   if (res == NULL) {
      ERROR(1, "Failed to allocate space for new group!");
      return EMPI_ERR_INTERN;
   }

   res->size = n;
   res->rank = -1;

   for (i=0;i<n;i++) {

      next = ranks[i];

      if (next < 0 || next >= in->size) {
         ERROR(1, "Rank out of bounds (%d)!", next);
         delete_group(res);
         return EMPI_ERR_RANK;
      }

      res->members[i] = in->members[next];

      if (res->members[i] == my_pid) {
         res->rank = i;
      }
   }

   *out = res;
   return EMPI_SUCCESS;
}

// The function EMPI_GROUP_EXCL creates a group of processes newgroup that is obtained by deleting from group
// those processes with ranks ranks[0] , ranks[n-1]. The ordering of processes in newgroup is identical to
// the ordering in group. Each of the n elements of ranks must be a valid rank in group and all elements must
// be distinct; otherwise, the program is erroneous. If n~=~0, then newgroup is identical to group.
int group_excl(group *in, int n, int ranks[], group **out)
{
   // TODO: not implemented
   ERROR(1, "group_excl NOT IMPLEMENTED!");
   return EMPI_ERR_GROUP;
}

static int get_range(group *in, int *dst, int *next, int first, int last, int stride)
{
   // Extract a certain range of processes from the group in.
   // Note that stride may be positive or negative, but may not be 0!
   int i, index;

   index = *next;

   if (stride == 0 || first < 0 || first >= in->size || last < 0 || last >= in->size) {

      ERROR(2, "GET_RANGE: Illegal rank first=%d last=%d stride=%d in->size=%d)!",
                                first, last, stride, in->size);

      return EMPI_ERR_RANK;
   }

// FIXME: Is this <= OK ??? 2 times!

   if (first <= last && stride > 0) {
      // forward range
      for (i=first;(i<in->size && i<=last);i+=stride) {

         if (index >= in->size) {

            ERROR(2, "GET_RANGE: Illegal arg (1) index=%d first=%d last=%d stride=%d in->size=%d)!",
                                 index, first, last, stride, in->size);

            return EMPI_ERR_ARG;
         }

         dst[index++] = in->members[i];
      }

   } else if (last <= first && stride < 0) {
      // backward range
      for (i=first;(i>=0 && i>=last);i+=stride) {

         if (index >= in->size) {

            ERROR(2, "GET_RANGE: Illegal arg (2) index=%d first=%d last=%d stride=%d in->size=%d)!",
                                 index, first, last, stride, in->size);

            return EMPI_ERR_ARG;
         }

         dst[index++] = in->members[i];
      }

   } else {
      ERROR(2, "GET_RANGE: Illegal request first=%d last=%d stride=%d in->size=%d)!",
                                 first, last, stride, in->size);

      return EMPI_ERR_RANK;
   }

   *next = index;

   return EMPI_SUCCESS;
}

// The ranges consist of the triplets (first, last, stride). The functionality of this routine is specified to
// be equivalent to expanding the array of ranges to an array of the included ranks and passing the resulting
// array of ranks and other arguments to EMPI_GROUP_INCL. A call to EMPI_GROUP_INCL is equivalent to a call to
// EMPI_GROUP_RANGE_INCL with each rank i in ranks replaced by the triplet (i,i,1) in the argument ranges.
int group_range_incl(group *in, int n, int ranges[][3], group **out)
{
   group *res;
   int *tmp;
   int i, next, error;

   if (in == NULL) {
      ERROR(1, "Group_range_incl got NULL as input!");
      return EMPI_ERR_GROUP;
   }

   // If the input group is empty, the result will be EMPI_GROUP_EMPTY
   if (in->size == 0) {
      *out = groups[EMPI_GROUP_EMPTY];
      return EMPI_SUCCESS;
   }

   next = 0;

   tmp = malloc(in->size * sizeof(int));

   if (tmp == NULL) {
      ERROR(1, "Failed to allocate space for temporary group!");
      return EMPI_ERR_INTERN;
   }

   for (i=0;i<n;i++) {
      error = get_range(in, tmp, &next, ranges[i][0], ranges[i][1], ranges[i][2]);

      if (error != EMPI_SUCCESS) {
         ERROR(1, "Failed to retrieve range %d (%d,%d,%d)!",
  			i, ranges[i][0], ranges[i][1], ranges[i][2]);
         free(tmp);
         return error;
      }
   }

   res = create_group(next);

   if (res == NULL) {
      ERROR(1, "Failed to allocate space for new group!");
      free(tmp);
      return EMPI_ERR_INTERN;
   }

   res->size = next;
   res->rank = -1;

   for (i=0;i<next;i++) {
      res->members[i] = tmp[i];

      if (tmp[i] == my_pid) {
         res->rank = i;
      }
   }

   free(tmp);
   *out = res;

   return EMPI_SUCCESS;
}

// The functionality of this routine is specified to be equivalent to expanding the array of ranges to an array
// of the excluded ranks and passing the resulting array of ranks and other arguments to EMPI_GROUP_EXCL. A call
// to EMPI_GROUP_EXCL is equivalent to a call to EMPI_GROUP_RANGE_EXCL with each rank i in ranks replaced by the
// triplet (i,i,1) in the argument ranges.
int group_range_excl(group *in, int n, int ranges[][3], group **out)
{
   // TODO: not implemented
   ERROR(1, "group_range_excl NOT IMPLEMENTED!");
   return EMPI_ERR_GROUP;
}

