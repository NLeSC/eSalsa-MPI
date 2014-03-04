#ifndef _GROUP_H_
#define _GPOUP_H_

#include "types.h"
#include "empi.h"

#define GROUP_TYPE_ACTIVE     0
#define GROUP_TYPE_SEPERATIST 1
#define GROUP_TYPE_IDLE       2

struct s_group {

   /* The handle of this group [2-MAX_GROUPS] */
   int handle;

   /* The rank of this process in this group, or -1 if this process is not part of this group */
   int rank;

   /* The size of this group, 0 if group equals EMPI_GROUP_EMPTY */
   int size;

   /* The PIDs of the members of this group */
   uint32_t *members;
};

// A group is an ordered set of process identifiers (henceforth processes); processes are implementation-dependent
// objects. Each process in a group is associated with an integer rank. Ranks are contiguous and start from zero.
// Groups are represented by opaque group objects, and hence cannot be directly transferred from one process to
// another. A group is used within a communicator to describe the participants in a communication ``universe'' and
// to rank such participants (thus giving them unique names within that ``universe'' of communication).
//
// There is a special pre-defined group: MPI_GROUP_EMPTY, which is a group with no members. The predefined constant
// MPI_GROUP_NULL is the value used for invalid group handles.
//
// Available group operations:

int init_groups();

int group_rank(group *g, int *rank);
int group_size(group *g, int *size);


// Returns a group representing the members of comm.
int group_comm_group(communicator *in, group **out);

// Returns all elements of the first group ( group1), followed by all elements of second group ( group2) not in first.
int group_union(group *in1, group *in2, group **out);

// Returns all elements of the first group that are also in the second group, ordered as in first group.
int group_intersection(group *in1, group *in2, group **out);

// Returns all elements of the first group that are not in the second group, ordered as in the first group.
int group_difference(group *in1, group *in2, group **out);

// The function MPI_GROUP_INCL creates a group newgroup that consists of the n processes in group with ranks
// rank[0], , rank[n-1]; the process with rank i in newgroup is the process with rank ranks[i] in group. Each
// of the n elements of ranks must be a valid rank in group and all elements must be distinct, or else the
// program is erroneous. If n~=~0, then newgroup is MPI_GROUP_EMPTY. This function can, for instance, be used
// to reorder the elements of a group. See also MPI_GROUP_COMPARE.
int group_incl(group *in, int n, int ranks[], group **out);

// The function MPI_GROUP_EXCL creates a group of processes newgroup that is obtained by deleting from group
// those processes with ranks ranks[0] , ranks[n-1]. The ordering of processes in newgroup is identical to
// the ordering in group. Each of the n elements of ranks must be a valid rank in group and all elements must
// be distinct; otherwise, the program is erroneous. If n~=~0, then newgroup is identical to group.
int group_excl(group *in, int n, int ranks[], group **out);

// The ranges consist of the triplets (first, strid, last). The functionality of this routine is specified to
// be equivalent to expanding the array of ranges to an array of the included ranks and passing the resulting
// array of ranks and other arguments to MPI_GROUP_INCL. A call to MPI_GROUP_INCL is equivalent to a call to
// MPI_GROUP_RANGE_INCL with each rank i in ranks replaced by the triplet (i,i,1) in the argument ranges.
int group_range_incl(group *in, int n, int ranges[][3], group **out);

// The functionality of this routine is specified to be equivalent to expanding the array of ranges to an array
// of the excluded ranks and passing the resulting array of ranks and other arguments to MPI_GROUP_EXCL. A call
// to MPI_GROUP_EXCL is equivalent to a call to MPI_GROUP_RANGE_EXCL with each rank i in ranks replaced by the
// triplet (i,i,1) in the argument ranges.
int group_range_excl(group *in, int n, int ranges[][3], group **out);

// Utility functions to convert handles to group ptr and vice-versa..
group *handle_to_group(EMPI_Group src);
EMPI_Group group_to_handle(group *src);

// Utility function to print group.
// void print_group(MPI_Group g);

#endif // _GROUP_H_
