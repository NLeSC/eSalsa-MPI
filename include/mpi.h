#ifndef __MPI_H_
#define __MPI_H_

#include <unistd.h>

// FIXME: do we need this ?
#define FALSE 0
#define TRUE  1

typedef int MPI_Comm;

typedef int MPI_Group;
typedef int MPI_Info;
typedef int MPI_Request;
typedef int MPI_Op;
typedef int MPI_Aint;
typedef int MPI_Win;
typedef int MPI_File;

typedef int MPI_Comm_errhandler_fn;
typedef int MPI_Comm_copy_attr_function;
typedef int MPI_Comm_delete_attr_function;
typedef int MPI_Errhandler;

typedef int MPI_Handler_function;
typedef int MPI_File_errhandler_fn;
typedef int MPI_Request;
typedef int MPI_Grequest_query_function;
typedef int MPI_Grequest_free_function;
typedef int MPI_Grequest_cancel_function;
typedef int MPI_Copy_function;
typedef int MPI_Delete_function;
typedef int MPI_Datarep_conversion_function;
typedef int MPI_Datarep_extent_function;
typedef int MPI_Type_copy_attr_function;
typedef int MPI_Type_delete_attr_function;
typedef int MPI_Win_errhandler_fn;
typedef int MPI_Win_copy_attr_function;
typedef int MPI_Win_delete_attr_function;

typedef int MPI_Fint;

typedef int MPI_Datatype;

typedef struct {
  int MPI_SOURCE;
  int MPI_TAG;
  int MPI_ERROR;
  // TODO: hide these ?? */
  int a, b, c;
} MPI_Status;

typedef size_t MPI_Offset;

typedef void (MPI_User_function)( void *invec, void *inoutvec, int *len, MPI_Datatype *datatype);

/* BASIC C Datatypes -- as defined in MPI 2.1 standard. */

#define MPI_DATATYPE_NULL         (0)
#define MPI_CHAR                  (1)
#define MPI_SHORT                 (2)
#define MPI_INT                   (3)
#define MPI_LONG                  (4)
#define MPI_LONG_LONG_INT         (5)
#define MPI_LONG_LONG             (MPI_LONG_LONG_INT) // Official synonym since MPI 2.1

#define MPI_SIGNED_CHAR           (6)
#define MPI_UNSIGNED_CHAR         (7)
#define MPI_UNSIGNED_SHORT        (8)
#define MPI_UNSIGNED              (9)
#define MPI_UNSIGNED_LONG         (10)
#define MPI_UNSIGNED_LONG_LONG    (11)

#define MPI_FLOAT                 (12)
#define MPI_DOUBLE                (13)
#define MPI_LONG_DOUBLE           (14)

#define MPI_WCHAR                 (15)

#define MPI_BYTE                  (16)
#define MPI_PACKED                (17)

/* Special datatypes for C reduction operations -- as defined in MPI 2.1 standard. */

#define MPI_FLOAT_INT             (18)
#define MPI_DOUBLE_INT            (19)
#define MPI_LONG_INT              (20)
#define MPI_2INT                  (21)
#define MPI_SHORT_INT             (22)
#define MPI_LONG_DOUBLE_INT       (23)

/* BASIC Fortran Datatypes -- as defined in MPI 2.1 standard. */

#define MPI_INTEGER               (24)
#define MPI_REAL                  (25)
#define MPI_DOUBLE_PRECISION      (26)
#define MPI_COMPLEX               (27)
#define MPI_LOGICAL               (28)
#define MPI_CHARACTER             (29)

/* Optional Fortran Datatypes -- as defined in MPI 2.1 standard. */

#define MPI_DOUBLE_COMPLEX        (30)

#define MPI_INTEGER1              (31)
#define MPI_INTEGER2              (32)
#define MPI_INTEGER4              (33)
#define MPI_INTEGER8              (34)

#define MPI_REAL2                 (35)
#define MPI_REAL4                 (36)
#define MPI_REAL8                 (37)

/* Special datatypes for Fortran reduction operations -- as defined in MPI 2.1 standard. */

#define MPI_2REAL                 (38)
#define MPI_2DOUBLE_PRECISION     (39)
#define MPI_2INTEGER              (40)

#define MPI_DEFINED_DATATYPES     (41)

/*
#ifdef HAVE_MPI_2_2

#define MPI_C_BOOL                (16)
#define MPI_INT8_T                (17)
#define MPI_INT16_T               (18)
#define MPI_INT32_T               (19)
#define MPI_INT64_T               (20)
#define MPI_UINT8_T               (21)
#define MPI_UINT16_T              (22)
#define MPI_UINT32_T              (23)
#define MPI_UINT64_T              (24)
#define MPI_C_COMPLEX             (25)
#define MPI_C_FLOAT_COMPLEX       (MPI_C_COMPLEX)
#define MPI_C_DOUBLE_COMPLEX      (26)
#define MPI_C_LONG_DOUBLE_COMPLEX (27)

#define MPI_COMPLEX8              (MPI_C_FLOAT_COMPLEX)
#define MPI_COMPLEX16             (MPI_C_DOUBLE_COMPLEX)
#define MPI_COMPLEX32             (MPI_C_LONG_DOUBLE_COMPLEX)

#endif
*/


/* Communicators */

#define MPI_COMM_WORLD 0
#define MPI_COMM_SELF  1
#define MPI_COMM_NULL  2

/* Groups */

// FIXME: check!
#define MPI_GROUP_NULL 0
#define MPI_GROUP_EMPTY 1

/* Requests */

// FIXME: check!
#define MPI_REQUEST_NULL -1

#define MPI_STATUS_IGNORE ((MPI_Status *) NULL)
#define MPI_STATUSES_IGNORE ((MPI_Status *) NULL)

/* Source / tags */
#define MPI_ANY_SOURCE (-1)
#define MPI_ANY_TAG    (-1)
#define MPI_TAG_LB     (0)      // not officially defined in MPI standard (but value is set in standard)
#define MPI_TAG_UB     (32767)  // minimum as defined by MPI standard
#define MPI_UNDEFINED  (-32766) // borrowed from openmpi

/* Files */
#define MPI_FILE_NULL  (-1)

#define MPI_MODE_CREATE              1
#define MPI_MODE_RDONLY              2
#define MPI_MODE_WRONLY              4
#define MPI_MODE_RDWR                8
#define MPI_MODE_DELETE_ON_CLOSE    16
#define MPI_MODE_UNIQUE_OPEN        32
#define MPI_MODE_EXCL               64
#define MPI_MODE_APPEND            128
#define MPI_MODE_SEQUENTIAL        256

/* Info */
#define MPI_INFO_NULL  (-1)

/*
 * Error codes borrowed from OpenMPI.
 */
#define MPI_SUCCESS                   0
#define MPI_ERR_BUFFER                1
#define MPI_ERR_COUNT                 2
#define MPI_ERR_TYPE                  3
#define MPI_ERR_TAG                   4
#define MPI_ERR_COMM                  5
#define MPI_ERR_RANK                  6
#define MPI_ERR_REQUEST               7
#define MPI_ERR_ROOT                  8
#define MPI_ERR_GROUP                 9
#define MPI_ERR_OP                    10
#define MPI_ERR_TOPOLOGY              11
#define MPI_ERR_DIMS                  12
#define MPI_ERR_ARG                   13
#define MPI_ERR_UNKNOWN               14
#define MPI_ERR_TRUNCATE              15
#define MPI_ERR_OTHER                 16
#define MPI_ERR_INTERN                17
#define MPI_ERR_IN_STATUS             18
#define MPI_ERR_PENDING               19
#define MPI_ERR_ACCESS                20
#define MPI_ERR_AMODE                 21
#define MPI_ERR_ASSERT                22
#define MPI_ERR_BAD_FILE              23
#define MPI_ERR_BASE                  24
#define MPI_ERR_CONVERSION            25
#define MPI_ERR_DISP                  26
#define MPI_ERR_DUP_DATAREP           27
#define MPI_ERR_FILE_EXISTS           28
#define MPI_ERR_FILE_IN_USE           29
#define MPI_ERR_FILE                  30
#define MPI_ERR_INFO_KEY              31
#define MPI_ERR_INFO_NOKEY            32
#define MPI_ERR_INFO_VALUE            33
#define MPI_ERR_INFO                  34
#define MPI_ERR_IO                    35
#define MPI_ERR_KEYVAL                36
#define MPI_ERR_LOCKTYPE              37
#define MPI_ERR_NAME                  38
#define MPI_ERR_NO_MEM                39
#define MPI_ERR_NOT_SAME              40
#define MPI_ERR_NO_SPACE              41
#define MPI_ERR_NO_SUCH_FILE          42
#define MPI_ERR_PORT                  43
#define MPI_ERR_QUOTA                 44
#define MPI_ERR_READ_ONLY             45
#define MPI_ERR_RMA_CONFLICT          46
#define MPI_ERR_RMA_SYNC              47
#define MPI_ERR_SERVICE               48
#define MPI_ERR_SIZE                  49
#define MPI_ERR_SPAWN                 50
#define MPI_ERR_UNSUPPORTED_DATAREP   51
#define MPI_ERR_UNSUPPORTED_OPERATION 52
#define MPI_ERR_WIN                   53
#define MPI_ERR_LASTCODE              54

#define MPI_ERR_SYSRESOURCE          -2

/* Predefined operations */

#define MPI_OP_NULL  0
#define MPI_MAX      1
#define MPI_MIN      2
#define MPI_SUM      3
#define MPI_PROD     4
#define MPI_MAXLOC   5
#define MPI_MINLOC   6
#define MPI_BOR      7
#define MPI_BAND     8
#define MPI_BXOR     9
#define MPI_LOR     10
#define MPI_LAND    11
#define MPI_LXOR    12


/* Various constants */
#define MPI_MAX_PROCESSOR_NAME (255)
#define MPI_MAX_ERROR_STRING (255)

// Misc. utility functions
int MPI_Init ( int *argc, char ***argv );
int MPI_Initialized ( int *flag );
int MPI_Abort ( MPI_Comm comm, int errorcode );
int MPI_Finalized ( int *flag );
int MPI_Finalize ( void );
double MPI_Wtime(void);

int MPI_Get_processor_name ( char *name, int *resultlen );
int MPI_Error_string ( int errorcode, char *string, int *resultlen );

// Communicators and groups.

int MPI_Comm_create ( MPI_Comm comm, MPI_Group g, MPI_Comm *newcomm );
int MPI_Comm_dup ( MPI_Comm comm, MPI_Comm *newcomm );
int MPI_Comm_free ( MPI_Comm *comm );
int MPI_Comm_group ( MPI_Comm comm, MPI_Group *g );
int MPI_Comm_rank ( MPI_Comm comm, int *rank );
int MPI_Comm_size ( MPI_Comm comm, int *size );
int MPI_Comm_split ( MPI_Comm comm, int color, int key, MPI_Comm *newcomm );
int MPI_Group_range_incl ( MPI_Group g, int n, int ranges[][3], MPI_Group *newgroup );
int MPI_Group_incl ( MPI_Group g, int n, int *ranks, MPI_Group *newgroup );
int MPI_Group_range_excl ( MPI_Group g, int n, int ranges[][3], MPI_Group *newgroup );
int MPI_Group_translate_ranks ( MPI_Group group1, int n, int *ranks1, MPI_Group group2, int *ranks2 );
int MPI_Group_union ( MPI_Group group1, MPI_Group group2, MPI_Group *newgroup );


// Collectives

int MPI_Allgather ( void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm );
int MPI_Allgatherv ( void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int *recvcounts, int *displs, MPI_Datatype recvtype, MPI_Comm comm );
int MPI_Allreduce ( void *sendbuf, void *recvbuf, int count, MPI_Datatype type, MPI_Op op, MPI_Comm comm );
int MPI_Alltoall ( void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm );
int MPI_Alltoallv ( void *sendbuf, int *sendcnts, int *sdispls, MPI_Datatype sendtype, void *recvbuf, int *recvcnts, int *rdispls, MPI_Datatype recvtype, MPI_Comm comm );
int MPI_Alltoallw ( void *sendbuf, int *sendcnts, int *sdispls, MPI_Datatype *sendtypes, void *recvbuf, int *recvcnts, int *rdispls, MPI_Datatype *recvtypes, MPI_Comm comm );
int MPI_Scatter ( void *sendbuf, int sendcnt, MPI_Datatype sendtype, void *recvbuf, int recvcnt, MPI_Datatype recvtype, int root, MPI_Comm comm );
int MPI_Scatterv ( void *sendbuf, int *sendcnts, int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcnt, MPI_Datatype recvtype, int root, MPI_Comm comm );
int MPI_Barrier ( MPI_Comm comm );
int MPI_Bcast ( void *buffer, int count, MPI_Datatype type, int root, MPI_Comm comm );
int MPI_Gather ( void *sendbuf, int sendcnt, MPI_Datatype sendtype, void *recvbuf, int recvcnt, MPI_Datatype recvtype, int root, MPI_Comm comm );
int MPI_Gatherv ( void *sendbuf, int sendcnt, MPI_Datatype sendtype, void *recvbuf, int *recvcnts, int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm );
int MPI_Reduce ( void *sendbuf, void *recvbuf, int count, MPI_Datatype type, MPI_Op op, int root, MPI_Comm comm );

// Send / receive.

int MPI_Ibsend ( void *buf, int count, MPI_Datatype type, int dest, int tag, MPI_Comm comm, MPI_Request *r );
int MPI_Irecv ( void *buf, int count, MPI_Datatype type, int source, int tag, MPI_Comm comm, MPI_Request *r );
int MPI_Irsend ( void *buf, int count, MPI_Datatype type, int dest, int tag, MPI_Comm comm, MPI_Request *r );
int MPI_Isend ( void *buf, int count, MPI_Datatype type, int dest, int tag, MPI_Comm comm, MPI_Request *r );
int MPI_Rsend ( void *buf, int count, MPI_Datatype type, int dest, int tag, MPI_Comm comm );
int MPI_Send ( void *buf, int count, MPI_Datatype type, int dest, int tag, MPI_Comm comm );
int MPI_Sendrecv ( void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *stat );
int MPI_Ssend ( void *buf, int count, MPI_Datatype type, int dest, int tag, MPI_Comm comm );
int MPI_Recv ( void *buf, int count, MPI_Datatype type, int source, int tag, MPI_Comm comm, MPI_Status *stat );

// Request and status handling.

int MPI_Wait ( MPI_Request *r, MPI_Status *stat );
int MPI_Waitall ( int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses );
int MPI_Waitany ( int count, MPI_Request array_of_requests[], int *index, MPI_Status *stat );
int MPI_Request_free ( MPI_Request *r );

// Datatypes

int MPI_Type_get_name ( MPI_Datatype type, char *type_name, int *resultlen );
int MPI_Type_free ( MPI_Datatype *type );
int MPI_Type_commit ( MPI_Datatype *type );
int MPI_Type_contiguous ( int count, MPI_Datatype old_type, MPI_Datatype *new_type_p );
int MPI_Type_create_indexed_block ( int count, int blocklength, int array_of_displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype );
int MPI_Type_get_envelope ( MPI_Datatype type, int *num_integers, int *num_addresses, int *num_datatypes, int *combiner );

// Info

int MPI_Info_create ( MPI_Info *info );
int MPI_Info_delete ( MPI_Info info, char *key );
int MPI_Info_set ( MPI_Info info, char *key, char *value );
int MPI_Info_free ( MPI_Info *info );

// I/O

int MPI_File_open ( MPI_Comm comm, char *filename, int amode, MPI_Info info, MPI_File *fh );
int MPI_File_close ( MPI_File *fh );
int MPI_File_read_all ( MPI_File fh, void *buf, int count, MPI_Datatype type, MPI_Status *stat );
int MPI_File_read_at ( MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype type, MPI_Status *stat );
int MPI_File_write_at ( MPI_File fh, MPI_Offset offset, void *buf, int count, MPI_Datatype type, MPI_Status *stat );
int MPI_File_set_view ( MPI_File fh, MPI_Offset disp, MPI_Datatype etype, MPI_Datatype filetype, char *datarep, MPI_Info info );
int MPI_File_write_all ( MPI_File fh, void *buf, int count, MPI_Datatype type, MPI_Status *stat );

// Intercomm

int MPI_Intercomm_create ( MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm );
int MPI_Intercomm_merge ( MPI_Comm intercomm, int high, MPI_Comm *newintracomm );



/*
int MPI_Type_create_f90_complex ( int p, int r, MPI_Datatype *newtype );
int MPI_Type_create_f90_integer ( int r, MPI_Datatype *newtype );
int MPI_Type_create_f90_real ( int p, int r, MPI_Datatype *newtype );
int MPI_Accumulate ( void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Op op, MPI_Win win );
int MPI_Add_error_class ( int *errorclass );
int MPI_Add_error_code ( int errorclass, int *errorcode );
int MPI_Add_error_string ( int errorcode, char *string );
int MPI_Address ( void *location, MPI_Aint *address );
int MPI_Allgather ( void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm );
int MPI_Allgatherv ( void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int *recvcounts, int *displs, MPI_Datatype recvtype, MPI_Comm comm );
int MPI_Alloc_mem ( MPI_Aint size, MPI_Info info, void *baseptr );
int MPI_Allreduce ( void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm );
int MPI_Alltoall ( void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm );
int MPI_Alltoallv ( void *sendbuf, int *sendcnts, int *sdispls, MPI_Datatype sendtype, void *recvbuf, int *recvcnts, int *rdispls, MPI_Datatype recvtype, MPI_Comm comm );
int MPI_Alltoallw ( void *sendbuf, int *sendcnts, int *sdispls, MPI_Datatype *sendtypes, void *recvbuf, int *recvcnts, int *rdispls, MPI_Datatype *recvtypes, MPI_Comm comm );
int MPI_Attr_delete ( MPI_Comm comm, int keyval );
int MPI_Attr_get ( MPI_Comm comm, int keyval, void *attr_value, int *flag );
int MPI_Attr_put ( MPI_Comm comm, int keyval, void *attr_value );
int MPI_Barrier ( MPI_Comm comm );
int MPI_Bcast ( void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm );
int MPI_Bsend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm );
int MPI_Bsend_init ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );
int MPI_Buffer_attach ( void *buffer, int size );
int MPI_Buffer_detach ( void *buffer, int *size );
int MPI_Cancel ( MPI_Request *r );
int MPI_Cart_coords ( MPI_Comm comm, int rank, int maxdims, int *coords );
int MPI_Cart_create ( MPI_Comm comm_old, int ndims, int *dims, int *periods, int reorder, MPI_Comm *comm_cart );
int MPI_Cartdim_get ( MPI_Comm comm, int *ndims );
int MPI_Cart_get ( MPI_Comm comm, int maxdims, int *dims, int *periods, int *coords );
int MPI_Cart_map ( MPI_Comm comm_old, int ndims, int *dims, int *periods, int *newrank );
int MPI_Cart_rank ( MPI_Comm comm, int *coords, int *rank );
int MPI_Cart_shift ( MPI_Comm comm, int direction, int displ, int *source, int *dest );
int MPI_Cart_sub ( MPI_Comm comm, int *remain_dims, MPI_Comm *comm_new );
int MPI_Close_port ( char *port_name );
int MPI_Comm_accept ( char *port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *newcomm );
int MPI_Comm_call_errhandler ( MPI_Comm comm, int errorcode );
int MPI_Comm_compare ( MPI_Comm comm1, MPI_Comm comm2, int *result );
int MPI_Comm_connect ( char *port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *newcomm );
int MPI_Comm_create_errhandler ( MPI_Comm_errhandler_fn *function, MPI_Errhandler *errhandler );
int MPI_Comm_create ( MPI_Comm comm, MPI_Group g, MPI_Comm *newcomm );
int MPI_Comm_create_keyval ( MPI_Comm_copy_attr_function *comm_copy_attr_fn, MPI_Comm_delete_attr_function *comm_delete_attr_fn, int *comm_keyval, void *extra_state );
int MPI_Comm_delete_attr ( MPI_Comm comm, int comm_keyval );
int MPI_Comm_disconnect ( MPI_Comm *comm );
int MPI_Comm_dup ( MPI_Comm comm, MPI_Comm *newcomm );
int MPI_Comm_free ( MPI_Comm *comm );
int MPI_Comm_free_keyval ( int *comm_keyval );
int MPI_Comm_get_attr ( MPI_Comm comm, int comm_keyval, void *attribute_val, int *flag );
int MPI_Comm_get_errhandler ( MPI_Comm comm, MPI_Errhandler *errhandler );
int MPI_Comm_get_name ( MPI_Comm comm, char *comm_name, int *resultlen );
int MPI_Comm_get_parent ( MPI_Comm *parent );
int MPI_Comm_group ( MPI_Comm comm, MPI_Group *g );
int MPI_Comm_join ( int fd, MPI_Comm *intercomm );
int MPI_Comm_rank ( MPI_Comm comm, int *rank );
int MPI_Comm_remote_group ( MPI_Comm comm, MPI_Group *g );
int MPI_Comm_remote_size ( MPI_Comm comm, int *size );
int MPI_Comm_set_attr ( MPI_Comm comm, int comm_keyval, void *attribute_val );
int MPI_Comm_set_errhandler ( MPI_Comm comm, MPI_Errhandler errhandler );
int MPI_Comm_set_name ( MPI_Comm comm, char *comm_name );
int MPI_Comm_size ( MPI_Comm comm, int *size );
int MPI_Comm_spawn ( char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[] );
int MPI_Comm_spawn_multiple ( int count, char *array_of_commands[], char* *array_of_argv[], int array_of_maxprocs[], MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[] );
int MPI_Comm_split ( MPI_Comm comm, int color, int key, MPI_Comm *newcomm );
int MPI_Comm_test_inter ( MPI_Comm comm, int *flag );
int MPI_Dims_create ( int nnodes, int ndims, int *dims );
int MPI_Dist_graph_create_adjacent ( MPI_Comm comm_old, int indegree, int sources[], int sourceweights[], int outdegree, int destinations[], int destweights[], MPI_Info info, int reorder, MPI_Comm *comm_dist_graph );
int MPI_Dist_graph_create ( MPI_Comm comm_old, int n, int sources[], int degrees[], int destinations[], int weights[], MPI_Info info, int reorder, MPI_Comm *comm_dist_graph );
int MPI_Dist_graph_neighbors_count ( MPI_Comm comm, int *indegree, int *outdegree, int *weighted );
int MPI_Dist_graph_neighbors ( MPI_Comm comm, int maxindegree, int sources[], int sourceweights[], int maxoutdegree, int destinations[], int destweights[] );
int MPI_Errhandler_create ( MPI_Handler_function *function, MPI_Errhandler *errhandler );
int MPI_Errhandler_free ( MPI_Errhandler *errhandler );
int MPI_Errhandler_get ( MPI_Comm comm, MPI_Errhandler *errhandler );
int MPI_Errhandler_set ( MPI_Comm comm, MPI_Errhandler errhandler );
int MPI_Error_class ( int errorcode, int *errorclass );
int MPI_Exscan ( void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm );
int MPI_File_call_errhandler ( MPI_File fh, int errorcode );
int MPI_File_create_errhandler ( MPI_File_errhandler_fn *function, MPI_Errhandler *errhandler );
int MPI_File_delete ( char *filename, MPI_Info info );
int MPI_File_get_amode ( MPI_File MPI_fh, int *amode );
int MPI_File_get_atomicity ( MPI_File MPI_fh, int *flag );
int MPI_File_get_byte_offset ( MPI_File MPI_fh, MPI_Offset offset, MPI_Offset *disp );
int MPI_File_get_errhandler ( MPI_File file, MPI_Errhandler *errhandler );
int MPI_File_get_group ( MPI_File MPI_fh, MPI_Group *g );
int MPI_File_get_info ( MPI_File MPI_fh, MPI_Info *info_used );
int MPI_File_get_position ( MPI_File MPI_fh, MPI_Offset *offset );
int MPI_File_get_position_shared ( MPI_File MPI_fh, MPI_Offset *offset );
int MPI_File_get_size ( MPI_File MPI_fh, MPI_Offset *size );
int MPI_File_get_type_extent ( MPI_File MPI_fh, MPI_Datatype datatype, MPI_Aint *extent );
int MPI_File_get_view ( MPI_File MPI_fh, MPI_Offset *disp, MPI_Datatype *etype, MPI_Datatype *filetype, char *datarep );
int MPI_File_iread_at ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPIO_Request *r );
int MPI_File_iread ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *r );
int MPI_File_iread_shared ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *r );
int MPI_File_iwrite_at ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPIO_Request *r );
int MPI_File_iwrite ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *r );
int MPI_File_iwrite_shared ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPIO_Request *r );
int MPI_File_preallocate ( MPI_File MPI_fh, MPI_Offset size );
int MPI_File_read_all_begin ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype );
int MPI_File_read_all_end ( MPI_File MPI_fh, void *buf, MPI_Status *status );
int MPI_File_read_at_all_begin ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype );
int MPI_File_read_at_all_end ( MPI_File MPI_fh, void *buf, MPI_Status *status );
int MPI_File_read_at_all ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );
int MPI_File_read ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );
int MPI_File_read_ordered_begin ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype );
int MPI_File_read_ordered_end ( MPI_File MPI_fh, void *buf, MPI_Status *status );
int MPI_File_read_ordered ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );
int MPI_File_read_shared ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );
int MPI_File_seek ( MPI_File MPI_fh, MPI_Offset offset, int whence );
int MPI_File_seek_shared ( MPI_File MPI_fh, MPI_Offset offset, int whence );
int MPI_File_set_atomicity ( MPI_File MPI_fh, int flag );
int MPI_File_set_errhandler ( MPI_File file, MPI_Errhandler errhandler );
int MPI_File_set_info ( MPI_File MPI_fh, MPI_Info info );
int MPI_File_set_size ( MPI_File MPI_fh, MPI_Offset size );



int MPI_File_sync ( MPI_File MPI_fh );
int MPI_File_write_all_begin ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype );
int MPI_File_write_all_end ( MPI_File MPI_fh, void *buf, MPI_Status *status );
int MPI_File_write_at_all_begin ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype );
int MPI_File_write_at_all_end ( MPI_File MPI_fh, void *buf, MPI_Status *status );
int MPI_File_write_at_all ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );




int MPI_File_write ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );
int MPI_File_write_ordered_begin ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype );
int MPI_File_write_ordered_end ( MPI_File MPI_fh, void *buf, MPI_Status *status );
int MPI_File_write_ordered ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );
int MPI_File_write_shared ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );
int MPI_Free_mem ( void *base );
int MPI_Gather ( void *sendbuf, int sendcnt, MPI_Datatype sendtype, void *recvbuf, int recvcnt, MPI_Datatype recvtype, int root, MPI_Comm comm );
int MPI_Gatherv ( void *sendbuf, int sendcnt, MPI_Datatype sendtype, void *recvbuf, int *recvcnts, int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm );
int MPI_Get_address ( void *location, MPI_Aint *address );
int MPI_Get_count ( MPI_Status *status, MPI_Datatype datatype, int *count );
int MPI_Get_elements ( MPI_Status *status, MPI_Datatype datatype, int *elements );
int MPI_Get ( void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win );
int MPI_Get_version ( int *version, int *subversion );
int MPI_Graph_create ( MPI_Comm comm_old, int nnodes, int *indx, int *edges, int reorder, MPI_Comm *comm_graph );
int MPI_Graphdims_get ( MPI_Comm comm, int *nnodes, int *nedges );
int MPI_Graph_get ( MPI_Comm comm, int maxindex, int maxedges, int *indx, int *edges );
int MPI_Graph_map ( MPI_Comm comm_old, int nnodes, int *indx, int *edges, int *newrank );
int MPI_Graph_neighbors_count ( MPI_Comm comm, int rank, int *nneighbors );
int MPI_Graph_neighbors ( MPI_Comm comm, int rank, int maxneighbors, int *neighbors );
int MPI_Grequest_complete ( MPI_Request r );
int MPI_Grequest_start ( MPI_Grequest_query_function *query_fn, MPI_Grequest_free_function *free_fn, MPI_Grequest_cancel_function *cancel_fn, void *extra_state, MPI_Request *r );
int MPI_Group_compare ( MPI_Group group1, MPI_Group group2, int *result );
int MPI_Group_difference ( MPI_Group group1, MPI_Group group2, MPI_Group *newgroup );
int MPI_Group_excl ( MPI_Group g, int n, int *ranks, MPI_Group *newgroup );
int MPI_Group_free ( MPI_Group *g );
int MPI_Group_intersection ( MPI_Group group1, MPI_Group group2, MPI_Group *newgroup );
int MPI_Group_range_incl ( MPI_Group g, int n, int ranges[][3], MPI_Group *newgroup );
int MPI_Group_rank ( MPI_Group g, int *rank );
int MPI_Group_size ( MPI_Group g, int *size );
int MPI_Ibsend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );
int MPI_Info_dup ( MPI_Info info, MPI_Info *newinfo );
int MPI_Info_get ( MPI_Info info, char *key, int valuelen, char *value, int *flag );
int MPI_Info_get_nkeys ( MPI_Info info, int *nkeys );
int MPI_Info_get_nthkey ( MPI_Info info, int n, char *key );
int MPI_Info_get_valuelen ( MPI_Info info, char *key, int *valuelen, int *flag );
int MPI_Init_thread ( int *argc, char ***argv, int required, int *provided );
int MPI_Iprobe ( int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status );
int MPI_Irecv ( void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *r );
int MPI_Irsend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );
int MPI_Isend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );
int MPI_Issend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );
int MPI_Is_thread_main ( int *flag );
int MPI_Keyval_create ( MPI_Copy_function *copy_fn, MPI_Delete_function *delete_fn, int *keyval, void *extra_state );
int MPI_Keyval_free ( int *keyval );
int MPI_Lookup_name ( char *service_name, MPI_Info info, char *port_name );
int MPI_Op_commutative ( MPI_Op op, int *commute );
int MPI_Op_create ( MPI_User_function *function, int commute, MPI_Op *op );
int MPI_Open_port ( MPI_Info info, char *port_name );
int MPI_Op_free ( MPI_Op *op );
int MPI_Pack_external ( char *datarep, void *inbuf, int incount, MPI_Datatype datatype, void *outbuf, MPI_Aint outcount, MPI_Aint *position );
int MPI_Pack_external_size ( char *datarep, int incount, MPI_Datatype datatype, MPI_Aint *size );
int MPI_Pack ( void *inbuf, int incount, MPI_Datatype datatype, void *outbuf, int outcount, int *position, MPI_Comm comm );
int MPI_Pack_size ( int incount, MPI_Datatype datatype, MPI_Comm comm, int *size );
int MPI_Probe ( int source, int tag, MPI_Comm comm, MPI_Status *status );
int MPI_Publish_name ( char *service_name, MPI_Info info, char *port_name );
int MPI_Put ( void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win );
int MPI_Query_thread ( int *provided );
int MPI_Recv_init ( void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *r );
int MPI_Reduce ( void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm );
int MPI_Reduce_local ( void *inbuf, void *inoutbuf, int count, MPI_Datatype datatype, MPI_Op op );
int MPI_Reduce_scatter_block ( void *sendbuf, void *recvbuf, int recvcount, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm );
int MPI_Reduce_scatter ( void *sendbuf, void *recvbuf, int *recvcnts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm );
int MPI_Register_datarep ( char *name, MPI_Datarep_conversion_function *read_conv_fn, MPI_Datarep_conversion_function *write_conv_fn, MPI_Datarep_extent_function *extent_fn, void *state );
int MPI_Request_free ( MPI_Request *r );
int MPI_Request_get_status ( MPI_Request r, int *flag, MPI_Status *status );
int MPI_Rsend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm );
int MPI_Rsend_init ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );
int MPI_Scan ( void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm );
int MPI_Scatter ( void *sendbuf, int sendcnt, MPI_Datatype sendtype, void *recvbuf, int recvcnt, MPI_Datatype recvtype, int root, MPI_Comm comm );
int MPI_Scatterv ( void *sendbuf, int *sendcnts, int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcnt, MPI_Datatype recvtype, int root, MPI_Comm comm );
int MPI_Send ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm );
int MPI_Send_init ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );
int MPI_Sendrecv ( void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status );
int MPI_Sendrecv_replace ( void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status );
int MPI_Ssend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm );
int MPI_Ssend_init ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );
int MPI_Startall ( int count, MPI_Request array_of_requests[] );
int MPI_Start ( MPI_Request *r );
int MPI_Status_set_cancelled ( MPI_Status *status, int flag );
int MPI_Status_set_elements ( MPI_Status *status, MPI_Datatype datatype, int count );
int MPI_Testall ( int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[] );
int MPI_Testany ( int count, MPI_Request array_of_requests[], int *index, int *flag, MPI_Status *status );
int MPI_Test_cancelled ( MPI_Status *status, int *flag );
int MPI_Test ( MPI_Request *r, int *flag, MPI_Status *status );
int MPI_Testsome ( int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[] );
int MPI_Topo_test ( MPI_Comm comm, int *topo_type );
int MPI_Type_create_darray ( int size, int rank, int ndims, int array_of_gsizes[], int array_of_distribs[], int array_of_dargs[], int array_of_psizes[], int order, MPI_Datatype oldtype, MPI_Datatype *newtype );
int MPI_Type_create_hindexed ( int count, int blocklengths[], MPI_Aint displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype );
int MPI_Type_create_hvector ( int count, int blocklength, MPI_Aint stride, MPI_Datatype oldtype, MPI_Datatype *newtype );
int MPI_Type_create_keyval ( MPI_Type_copy_attr_function *type_copy_attr_fn, MPI_Type_delete_attr_function *type_delete_attr_fn, int *type_keyval, void *extra_state );
int MPI_Type_create_resized ( MPI_Datatype oldtype, MPI_Aint lb, MPI_Aint extent, MPI_Datatype *newtype );
int MPI_Type_create_struct ( int count, int array_of_blocklengths[], MPI_Aint array_of_displacements[], MPI_Datatype array_of_types[], MPI_Datatype *newtype );
int MPI_Type_create_subarray ( int ndims, int array_of_sizes[], int array_of_subsizes[], int array_of_starts[], int order, MPI_Datatype oldtype, MPI_Datatype *newtype );
int MPI_Type_delete_attr ( MPI_Datatype type, int type_keyval );
int MPI_Type_dup ( MPI_Datatype datatype, MPI_Datatype *newtype );
int MPI_Type_extent ( MPI_Datatype datatype, MPI_Aint *extent );
int MPI_Type_free_keyval ( int *type_keyval );
int MPI_Type_get_attr ( MPI_Datatype type, int type_keyval, void *attribute_val, int *flag );
int MPI_Type_get_contents ( MPI_Datatype datatype, int max_integers, int max_addresses, int max_datatypes, int array_of_integers[], MPI_Aint array_of_addresses[], MPI_Datatype array_of_datatypes[] );
int MPI_Type_get_extent ( MPI_Datatype datatype, MPI_Aint *lb, MPI_Aint *extent );
int MPI_Type_get_true_extent ( MPI_Datatype datatype, MPI_Aint *true_lb, MPI_Aint *true_extent );
int MPI_Type_hindexed ( int count, int blocklens[], MPI_Aint indices[], MPI_Datatype old_type, MPI_Datatype *newtype );
int MPI_Type_hvector ( int count, int blocklen, MPI_Aint stride, MPI_Datatype old_type, MPI_Datatype *newtype_p );
int MPI_Type_indexed ( int count, int blocklens[], int indices[], MPI_Datatype old_type, MPI_Datatype *newtype );
int MPI_Type_lb ( MPI_Datatype datatype, MPI_Aint *displacement );
int MPI_Type_match_size ( int typeclass, int size, MPI_Datatype *datatype );
int MPI_Type_set_attr ( MPI_Datatype type, int type_keyval, void *attribute_val );
int MPI_Type_set_name ( MPI_Datatype type, char *type_name );
int MPI_Type_size ( MPI_Datatype datatype, int *size );
int MPI_Type_struct ( int count, int blocklens[], MPI_Aint indices[], MPI_Datatype old_types[], MPI_Datatype *newtype );
int MPI_Type_ub ( MPI_Datatype datatype, MPI_Aint *displacement );
int MPI_Type_vector ( int count, int blocklength, int stride, MPI_Datatype old_type, MPI_Datatype *newtype_p );
int MPI_Unpack_external ( char *datarep, void *inbuf, MPI_Aint insize, MPI_Aint *position, void *outbuf, int outcount, MPI_Datatype datatype );
int MPI_Unpack ( void *inbuf, int insize, int *position, void *outbuf, int outcount, MPI_Datatype datatype, MPI_Comm comm );
int MPI_Unpublish_name ( char *service_name, MPI_Info info, char *port_name );
int MPI_Waitall ( int count, MPI_Request array_of_requests[], MPI_Status array_of_statuses[] );
int MPI_Waitany ( int count, MPI_Request array_of_requests[], int *index, MPI_Status *status );
int MPI_Wait ( MPI_Request *r, MPI_Status *status );
int MPI_Waitsome ( int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[] );
int MPI_Win_call_errhandler ( MPI_Win win, int errorcode );
int MPI_Win_complete ( MPI_Win win );
int MPI_Win_create_errhandler ( MPI_Win_errhandler_fn *function, MPI_Errhandler *errhandler );
int MPI_Win_create ( void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win );
int MPI_Win_create_keyval ( MPI_Win_copy_attr_function *win_copy_attr_fn, MPI_Win_delete_attr_function *win_delete_attr_fn, int *win_keyval, void *extra_state );
int MPI_Win_delete_attr ( MPI_Win win, int win_keyval );
int MPI_Win_fence ( int assert, MPI_Win win );
int MPI_Win_free ( MPI_Win *win );
int MPI_Win_free_keyval ( int *win_keyval );
int MPI_Win_get_attr ( MPI_Win win, int win_keyval, void *attribute_val, int *flag );
int MPI_Win_get_errhandler ( MPI_Win win, MPI_Errhandler *errhandler );
int MPI_Win_get_group ( MPI_Win win, MPI_Group *g );
int MPI_Win_get_name ( MPI_Win win, char *win_name, int *resultlen );
int MPI_Win_lock ( int lock_type, int rank, int assert, MPI_Win win );
int MPI_Win_post ( MPI_Group g, int assert, MPI_Win win );
int MPI_Win_set_attr ( MPI_Win win, int win_keyval, void *attribute_val );
int MPI_Win_set_errhandler ( MPI_Win win, MPI_Errhandler errhandler );
int MPI_Win_set_name ( MPI_Win win, char *win_name );
int MPI_Win_start ( MPI_Group g, int assert, MPI_Win win );
int MPI_Win_test ( MPI_Win win, int *flag );
int MPI_Win_unlock ( int rank, MPI_Win win );
int MPI_Win_wait ( MPI_Win win );
int MPI_Group_comm_create ( MPI_Comm old_comm, MPI_Group g, int tag, MPI_Comm *new_comm );
MPI_Fint MPI_Comm_c2f ( MPI_Comm comm );
MPI_Fint MPI_Group_c2f ( MPI_Group g );
MPI_Fint MPI_Request_c2f ( MPI_Request r );
MPI_Fint MPI_Info_c2f ( MPI_Info info );
MPI_Fint MPI_File_c2f ( MPI_File file );
MPI_Fint MPI_Op_c2f ( MPI_Op op );
MPI_Fint MPI_Win_c2f ( MPI_Win Win );
MPI_Fint MPI_Errhandler_c2f ( MPI_Errhandler Errhandler );
MPI_Fint MPI_Type_c2f ( MPI_Datatype Type );
*/

#endif // __MPI_H_
