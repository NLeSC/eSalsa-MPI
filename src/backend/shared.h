#ifndef __SHARED_H_
#define __SHARED_H_

#include <stdint.h>

/* These are shared fields, initialized in mpibis.c */

// The number of clusters and the rank of our cluster in this set.
extern uint32_t cluster_count;
extern uint32_t cluster_rank;

// The number of processes in our cluster and our process' rank in this set.
//extern int local_count;
//extern int local_rank;

// The pid of my machine.
extern uint32_t my_pid;

// Value of various Fortan MPI constants.
extern int FORTRAN_MPI_COMM_NULL;
extern int FORTRAN_MPI_GROUP_NULL;
extern int FORTRAN_MPI_REQUEST_NULL;
extern int FORTRAN_MPI_GROUP_EMPTY;
extern int FORTRAN_MPI_COMM_WORLD;
extern int FORTRAN_MPI_COMM_SELF;

#endif
