#ifndef __EMPI_H_
#define __EMPI_H_

#include "types.h"

// FIXME: do we need this ?
#define FALSE 0
#define TRUE  1

typedef int EMPI_Comm;

typedef status EMPI_Status;

typedef int EMPI_Group;
typedef int EMPI_Info;
typedef int EMPI_Request;
typedef int EMPI_Op;
typedef int EMPI_Aint;
typedef int EMPI_Win;
typedef int EMPI_File;
typedef int EMPI_Offset;

typedef int EMPI_Comm_errhandler_fn;
typedef int EMPI_Comm_copy_attr_function;
typedef int EMPI_Comm_delete_attr_function;
typedef int EMPI_Errhandler;


typedef int EMPI_Handler_function;
typedef int EMPI_File_errhandler_fn;
typedef int EMPI_Request;
typedef int EMPI_Grequest_query_function;
typedef int EMPI_Grequest_free_function;
typedef int EMPI_Grequest_cancel_function;
typedef int EMPI_Copy_function;
typedef int EMPI_Delete_function;
typedef int EMPI_Datarep_conversion_function;
typedef int EMPI_Datarep_extent_function;
typedef int EMPI_Type_copy_attr_function;
typedef int EMPI_Type_delete_attr_function;
typedef int EMPI_Win_errhandler_fn;
typedef int EMPI_Win_copy_attr_function;
typedef int EMPI_Win_delete_attr_function;

typedef int EMPI_Fint;

typedef int EMPI_Datatype;

typedef void (EMPI_User_function)( void *invec, void *inoutvec, int *len, EMPI_Datatype *datatype);

/* BASIC C Datatypes -- as defined in MPI 2.2 standard. */

#define EMPI_DATATYPE_NULL         (0)
#define EMPI_CHAR                  (1)
#define EMPI_SHORT                 (2)
#define EMPI_INT                   (3)
#define EMPI_LONG                  (4)
#define EMPI_LONG_LONG_INT         (5)
#define EMPI_LONG_LONG             (EMPI_LONG_LONG_INT)

#define EMPI_SIGNED_CHAR           (6)
#define EMPI_UNSIGNED_CHAR         (7)
#define EMPI_UNSIGNED_SHORT        (8)
#define EMPI_UNSIGNED              (9)
#define EMPI_UNSIGNED_LONG         (10)
#define EMPI_UNSIGNED_LONG_LONG    (11)

#define EMPI_FLOAT                 (12)
#define EMPI_DOUBLE                (13)
#define EMPI_LONG_DOUBLE           (14)

#define EMPI_WCHAR                 (15)

#ifdef HAVE_MPI_2_2

#define EMPI_C_BOOL                (16)
#define EMPI_INT8_T                (17)
#define EMPI_INT16_T               (18)
#define EMPI_INT32_T               (19)
#define EMPI_INT64_T               (20)
#define EMPI_UINT8_T               (21)
#define EMPI_UINT16_T              (22)
#define EMPI_UINT32_T              (23)
#define EMPI_UINT64_T              (24)
#define EMPI_C_COMPLEX             (25)
#define EMPI_C_FLOAT_COMPLEX       (EMPI_C_COMPLEX)
#define EMPI_C_DOUBLE_COMPLEX      (26)
#define EMPI_C_LONG_DOUBLE_COMPLEX (27)

#define EMPI_COMPLEX8              (EMPI_C_FLOAT_COMPLEX)
#define EMPI_COMPLEX16             (EMPI_C_DOUBLE_COMPLEX)
#define EMPI_COMPLEX32             (EMPI_C_LONG_DOUBLE_COMPLEX)

#endif

#define EMPI_BYTE                  (28)

#define EMPI_PACKED                (29)

/* Special datatypes for reduction operations -- as defined in MPI 2.2 standard. */

#define EMPI_FLOAT_INT             (30)
#define EMPI_DOUBLE_INT            (31)
#define EMPI_LONG_INT              (32)
#define EMPI_2INT                  (33)
#define EMPI_SHORT_INT             (34)
#define EMPI_LONG_DOUBLE_INT       (35)

/* BASIC Fortran Datatypes -- as defined in MPI 2.1 standard. */

#define EMPI_INTEGER               (EMPI_INT)
#define EMPI_REAL                  (EMPI_FLOAT)
#define EMPI_DOUBLE_PRECISION      (EMPI_DOUBLE)
#define EMPI_LOGICAL               (EMPI_INT) // FIXME -- check!
#define EMPI_CHARACTER             (EMPI_CHAR)
#define EMPI_COMPLEX               (36)
#define EMPI_DOUBLE_COMPLEX        (37)

#define EMPI_INTEGER1              (EMPI_SIGNED_CHAR)
#define EMPI_INTEGER2              (EMPI_SHORT)
#define EMPI_INTEGER4              (EMPI_INT)
#define EMPI_INTEGER8              (EMPI_LONG_LONG_INT)

#define EMPI_UNSIGNED_INTEGER1     (EMPI_UNSIGNED_CHAR)
#define EMPI_UNSIGNED_INTEGER2     (EMPI_UNSIGNED_SHORT)
#define EMPI_UNSIGNED_INTEGER4     (EMPI_UNSIGNED)
#define EMPI_UNSIGNED_INTEGER8     (EMPI_UNSIGNED_LONG_LONG)

#define EMPI_REAL4                 (EMPI_FLOAT)
#define EMPI_REAL8                 (EMPI_DOUBLE)
#define EMPI_REAL16                (EMPI_LONG_DOUBLE)

#define EMPI_2REAL                 (38)
#define EMPI_2DOUBLE_PRECISION     (39)
#define EMPI_2INTEGER              (EMPI_2INT)

#define EMPI_DEFINED_DATATYPES     (40)

/* Communicators */

#define EMPI_COMM_WORLD 0
#define EMPI_COMM_SELF  1
#define EMPI_COMM_NULL  3

/* Groups */

// FIXME: check!
#define EMPI_GROUP_NULL 0
#define EMPI_GROUP_EMPTY 1

/* Requests */

// FIXME: check!
#define EMPI_REQUEST_NULL -1

#define EMPI_STATUS_IGNORE ((EMPI_Status *) NULL)
#define EMPI_STATUSES_IGNORE ((EMPI_Status *) NULL)

/* Source / tags */
#define EMPI_ANY_SOURCE (-1)
#define EMPI_ANY_TAG    (-1)
#define EMPI_TAG_LB     (0)      // not officially defined in MPI standard (but value is set in standard)
#define EMPI_TAG_UB     (32767)  // minimum as defined by MPI standard
#define EMPI_UNDEFINED  (-32766) // borrowed from openmpi

/*
 * Error codes borrowed from OpenMPI.
 */
#define EMPI_SUCCESS                   0
#define EMPI_ERR_BUFFER                1
#define EMPI_ERR_COUNT                 2
#define EMPI_ERR_TYPE                  3
#define EMPI_ERR_TAG                   4
#define EMPI_ERR_COMM                  5
#define EMPI_ERR_RANK                  6
#define EMPI_ERR_REQUEST               7
#define EMPI_ERR_ROOT                  8
#define EMPI_ERR_GROUP                 9
#define EMPI_ERR_OP                    10
#define EMPI_ERR_TOPOLOGY              11
#define EMPI_ERR_DIMS                  12
#define EMPI_ERR_ARG                   13
#define EMPI_ERR_UNKNOWN               14
#define EMPI_ERR_TRUNCATE              15
#define EMPI_ERR_OTHER                 16
#define EMPI_ERR_INTERN                17
#define EMPI_ERR_IN_STATUS             18
#define EMPI_ERR_PENDING               19
#define EMPI_ERR_ACCESS                20
#define EMPI_ERR_AMODE                 21
#define EMPI_ERR_ASSERT                22
#define EMPI_ERR_BAD_FILE              23
#define EMPI_ERR_BASE                  24
#define EMPI_ERR_CONVERSION            25
#define EMPI_ERR_DISP                  26
#define EMPI_ERR_DUP_DATAREP           27
#define EMPI_ERR_FILE_EXISTS           28
#define EMPI_ERR_FILE_IN_USE           29
#define EMPI_ERR_FILE                  30
#define EMPI_ERR_INFO_KEY              31
#define EMPI_ERR_INFO_NOKEY            32
#define EMPI_ERR_INFO_VALUE            33
#define EMPI_ERR_INFO                  34
#define EMPI_ERR_IO                    35
#define EMPI_ERR_KEYVAL                36
#define EMPI_ERR_LOCKTYPE              37
#define EMPI_ERR_NAME                  38
#define EMPI_ERR_NO_MEM                39
#define EMPI_ERR_NOT_SAME              40
#define EMPI_ERR_NO_SPACE              41
#define EMPI_ERR_NO_SUCH_FILE          42
#define EMPI_ERR_PORT                  43
#define EMPI_ERR_QUOTA                 44
#define EMPI_ERR_READ_ONLY             45
#define EMPI_ERR_RMA_CONFLICT          46
#define EMPI_ERR_RMA_SYNC              47
#define EMPI_ERR_SERVICE               48
#define EMPI_ERR_SIZE                  49
#define EMPI_ERR_SPAWN                 50
#define EMPI_ERR_UNSUPPORTED_DATAREP   51
#define EMPI_ERR_UNSUPPORTED_OPERATION 52
#define EMPI_ERR_WIN                   53
#define EMPI_ERR_LASTCODE              54

#define EMPI_ERR_SYSRESOURCE          -2

/* Predefined operations */

#define EMPI_OP_NULL  0
#define EMPI_MAX      1
#define EMPI_MIN      2
#define EMPI_SUM      3
#define EMPI_PROD     4
#define EMPI_MAXLOC   5
#define EMPI_MINLOC   6
#define EMPI_BOR      7
#define EMPI_BAND     8
#define EMPI_BXOR     9
#define EMPI_LOR     10
#define EMPI_LAND    11
#define EMPI_LXOR    12

// Misc. utility functions
int EMPI_Init ( int *argc, char ***argv );
int EMPI_Abort ( EMPI_Comm comm, int errorcode );
int EMPI_Finalized ( int *flag );
int EMPI_Finalize ( void );
double EMPI_Wtime(void);


// Datatypes
int EMPI_Type_get_name ( EMPI_Datatype datatype, char *type_name, int *resultlen );

/*
int EMPI_Type_create_f90_complex ( int p, int r, EMPI_Datatype *newtype );
int EMPI_Type_create_f90_integer ( int r, EMPI_Datatype *newtype );
int EMPI_Type_create_f90_real ( int p, int r, EMPI_Datatype *newtype );
int EMPI_Accumulate ( void *origin_addr, int origin_count, EMPI_Datatype origin_datatype, int target_rank, EMPI_Aint target_disp, int target_count, EMPI_Datatype target_datatype, EMPI_Op op, EMPI_Win win );
int EMPI_Add_error_class ( int *errorclass );
int EMPI_Add_error_code ( int errorclass, int *errorcode );
int EMPI_Add_error_string ( int errorcode, char *string );
int EMPI_Address ( void *location, EMPI_Aint *address );
int EMPI_Allgather ( void *sendbuf, int sendcount, EMPI_Datatype sendtype, void *recvbuf, int recvcount, EMPI_Datatype recvtype, EMPI_Comm comm );
int EMPI_Allgatherv ( void *sendbuf, int sendcount, EMPI_Datatype sendtype, void *recvbuf, int *recvcounts, int *displs, EMPI_Datatype recvtype, EMPI_Comm comm );
int EMPI_Alloc_mem ( EMPI_Aint size, EMPI_Info info, void *baseptr );
int EMPI_Allreduce ( void *sendbuf, void *recvbuf, int count, EMPI_Datatype datatype, EMPI_Op op, EMPI_Comm comm );
int EMPI_Alltoall ( void *sendbuf, int sendcount, EMPI_Datatype sendtype, void *recvbuf, int recvcount, EMPI_Datatype recvtype, EMPI_Comm comm );
int EMPI_Alltoallv ( void *sendbuf, int *sendcnts, int *sdispls, EMPI_Datatype sendtype, void *recvbuf, int *recvcnts, int *rdispls, EMPI_Datatype recvtype, EMPI_Comm comm );
int EMPI_Alltoallw ( void *sendbuf, int *sendcnts, int *sdispls, EMPI_Datatype *sendtypes, void *recvbuf, int *recvcnts, int *rdispls, EMPI_Datatype *recvtypes, EMPI_Comm comm );
int EMPI_Attr_delete ( EMPI_Comm comm, int keyval );
int EMPI_Attr_get ( EMPI_Comm comm, int keyval, void *attr_value, int *flag );
int EMPI_Attr_put ( EMPI_Comm comm, int keyval, void *attr_value );
int EMPI_Barrier ( EMPI_Comm comm );
int EMPI_Bcast ( void *buffer, int count, EMPI_Datatype datatype, int root, EMPI_Comm comm );
int EMPI_Bsend ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm );
int EMPI_Bsend_init ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm, EMPI_Request *r );
int EMPI_Buffer_attach ( void *buffer, int size );
int EMPI_Buffer_detach ( void *buffer, int *size );
int EMPI_Cancel ( EMPI_Request *r );
int EMPI_Cart_coords ( EMPI_Comm comm, int rank, int maxdims, int *coords );
int EMPI_Cart_create ( EMPI_Comm comm_old, int ndims, int *dims, int *periods, int reorder, EMPI_Comm *comm_cart );
int EMPI_Cartdim_get ( EMPI_Comm comm, int *ndims );
int EMPI_Cart_get ( EMPI_Comm comm, int maxdims, int *dims, int *periods, int *coords );
int EMPI_Cart_map ( EMPI_Comm comm_old, int ndims, int *dims, int *periods, int *newrank );
int EMPI_Cart_rank ( EMPI_Comm comm, int *coords, int *rank );
int EMPI_Cart_shift ( EMPI_Comm comm, int direction, int displ, int *source, int *dest );
int EMPI_Cart_sub ( EMPI_Comm comm, int *remain_dims, EMPI_Comm *comm_new );
int EMPI_Close_port ( char *port_name );
int EMPI_Comm_accept ( char *port_name, EMPI_Info info, int root, EMPI_Comm comm, EMPI_Comm *newcomm );
int EMPI_Comm_call_errhandler ( EMPI_Comm comm, int errorcode );
int EMPI_Comm_compare ( EMPI_Comm comm1, EMPI_Comm comm2, int *result );
int EMPI_Comm_connect ( char *port_name, EMPI_Info info, int root, EMPI_Comm comm, EMPI_Comm *newcomm );
int EMPI_Comm_create_errhandler ( EMPI_Comm_errhandler_fn *function, EMPI_Errhandler *errhandler );
int EMPI_Comm_create ( EMPI_Comm comm, EMPI_Group g, EMPI_Comm *newcomm );
int EMPI_Comm_create_keyval ( EMPI_Comm_copy_attr_function *comm_copy_attr_fn, EMPI_Comm_delete_attr_function *comm_delete_attr_fn, int *comm_keyval, void *extra_state );
int EMPI_Comm_delete_attr ( EMPI_Comm comm, int comm_keyval );
int EMPI_Comm_disconnect ( EMPI_Comm *comm );
int EMPI_Comm_dup ( EMPI_Comm comm, EMPI_Comm *newcomm );
int EMPI_Comm_free ( EMPI_Comm *comm );
int EMPI_Comm_free_keyval ( int *comm_keyval );
int EMPI_Comm_get_attr ( EMPI_Comm comm, int comm_keyval, void *attribute_val, int *flag );
int EMPI_Comm_get_errhandler ( EMPI_Comm comm, EMPI_Errhandler *errhandler );
int EMPI_Comm_get_name ( EMPI_Comm comm, char *comm_name, int *resultlen );
int EMPI_Comm_get_parent ( EMPI_Comm *parent );
int EMPI_Comm_group ( EMPI_Comm comm, EMPI_Group *g );
int EMPI_Comm_join ( int fd, EMPI_Comm *intercomm );
int EMPI_Comm_rank ( EMPI_Comm comm, int *rank );
int EMPI_Comm_remote_group ( EMPI_Comm comm, EMPI_Group *g );
int EMPI_Comm_remote_size ( EMPI_Comm comm, int *size );
int EMPI_Comm_set_attr ( EMPI_Comm comm, int comm_keyval, void *attribute_val );
int EMPI_Comm_set_errhandler ( EMPI_Comm comm, EMPI_Errhandler errhandler );
int EMPI_Comm_set_name ( EMPI_Comm comm, char *comm_name );
int EMPI_Comm_size ( EMPI_Comm comm, int *size );
int EMPI_Comm_spawn ( char *command, char *argv[], int maxprocs, EMPI_Info info, int root, EMPI_Comm comm, EMPI_Comm *intercomm, int array_of_errcodes[] );
int EMPI_Comm_spawn_multiple ( int count, char *array_of_commands[], char* *array_of_argv[], int array_of_maxprocs[], EMPI_Info array_of_info[], int root, EMPI_Comm comm, EMPI_Comm *intercomm, int array_of_errcodes[] );
int EMPI_Comm_split ( EMPI_Comm comm, int color, int key, EMPI_Comm *newcomm );
int EMPI_Comm_test_inter ( EMPI_Comm comm, int *flag );
int EMPI_Dims_create ( int nnodes, int ndims, int *dims );
int EMPI_Dist_graph_create_adjacent ( EMPI_Comm comm_old, int indegree, int sources[], int sourceweights[], int outdegree, int destinations[], int destweights[], EMPI_Info info, int reorder, EMPI_Comm *comm_dist_graph );
int EMPI_Dist_graph_create ( EMPI_Comm comm_old, int n, int sources[], int degrees[], int destinations[], int weights[], EMPI_Info info, int reorder, EMPI_Comm *comm_dist_graph );
int EMPI_Dist_graph_neighbors_count ( EMPI_Comm comm, int *indegree, int *outdegree, int *weighted );
int EMPI_Dist_graph_neighbors ( EMPI_Comm comm, int maxindegree, int sources[], int sourceweights[], int maxoutdegree, int destinations[], int destweights[] );
int EMPI_Errhandler_create ( EMPI_Handler_function *function, EMPI_Errhandler *errhandler );
int EMPI_Errhandler_free ( EMPI_Errhandler *errhandler );
int EMPI_Errhandler_get ( EMPI_Comm comm, EMPI_Errhandler *errhandler );
int EMPI_Errhandler_set ( EMPI_Comm comm, EMPI_Errhandler errhandler );
int EMPI_Error_class ( int errorcode, int *errorclass );
int EMPI_Error_string ( int errorcode, char *string, int *resultlen );
int EMPI_Exscan ( void *sendbuf, void *recvbuf, int count, EMPI_Datatype datatype, EMPI_Op op, EMPI_Comm comm );
int EMPI_File_call_errhandler ( EMPI_File fh, int errorcode );
int EMPI_File_close ( EMPI_File *mpi_fh );
int EMPI_File_create_errhandler ( EMPI_File_errhandler_fn *function, EMPI_Errhandler *errhandler );
int EMPI_File_delete ( char *filename, EMPI_Info info );
int EMPI_File_get_amode ( EMPI_File EMPI_fh, int *amode );
int EMPI_File_get_atomicity ( EMPI_File EMPI_fh, int *flag );
int EMPI_File_get_byte_offset ( EMPI_File EMPI_fh, EMPI_Offset offset, EMPI_Offset *disp );
int EMPI_File_get_errhandler ( EMPI_File file, EMPI_Errhandler *errhandler );
int EMPI_File_get_group ( EMPI_File EMPI_fh, EMPI_Group *g );
int EMPI_File_get_info ( EMPI_File EMPI_fh, EMPI_Info *info_used );
int EMPI_File_get_position ( EMPI_File EMPI_fh, EMPI_Offset *offset );
int EMPI_File_get_position_shared ( EMPI_File EMPI_fh, EMPI_Offset *offset );
int EMPI_File_get_size ( EMPI_File EMPI_fh, EMPI_Offset *size );
int EMPI_File_get_type_extent ( EMPI_File EMPI_fh, EMPI_Datatype datatype, EMPI_Aint *extent );
int EMPI_File_get_view ( EMPI_File EMPI_fh, EMPI_Offset *disp, EMPI_Datatype *etype, EMPI_Datatype *filetype, char *datarep );
int EMPI_File_iread_at ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype datatype, MPIO_Request *r );
int EMPI_File_iread ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Request *r );
int EMPI_File_iread_shared ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Request *r );
int EMPI_File_iwrite_at ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype datatype, MPIO_Request *r );
int EMPI_File_iwrite ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Request *r );
int EMPI_File_iwrite_shared ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, MPIO_Request *r );
int EMPI_File_open ( EMPI_Comm comm, char *filename, int amode, EMPI_Info info, EMPI_File *fh );
int EMPI_File_preallocate ( EMPI_File EMPI_fh, EMPI_Offset size );
int EMPI_File_read_all_begin ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype );
int EMPI_File_read_all_end ( EMPI_File EMPI_fh, void *buf, EMPI_Status *status );
int EMPI_File_read_all ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status );
int EMPI_File_read_at_all_begin ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype datatype );
int EMPI_File_read_at_all_end ( EMPI_File EMPI_fh, void *buf, EMPI_Status *status );
int EMPI_File_read_at_all ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status );
int EMPI_File_read_at ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status );
int EMPI_File_read ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status );
int EMPI_File_read_ordered_begin ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype );
int EMPI_File_read_ordered_end ( EMPI_File EMPI_fh, void *buf, EMPI_Status *status );
int EMPI_File_read_ordered ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status );
int EMPI_File_read_shared ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status );
int EMPI_File_seek ( EMPI_File EMPI_fh, EMPI_Offset offset, int whence );
int EMPI_File_seek_shared ( EMPI_File EMPI_fh, EMPI_Offset offset, int whence );
int EMPI_File_set_atomicity ( EMPI_File EMPI_fh, int flag );
int EMPI_File_set_errhandler ( EMPI_File file, EMPI_Errhandler errhandler );
int EMPI_File_set_info ( EMPI_File EMPI_fh, EMPI_Info info );
int EMPI_File_set_size ( EMPI_File EMPI_fh, EMPI_Offset size );
int EMPI_File_set_view ( EMPI_File EMPI_fh, EMPI_Offset disp, EMPI_Datatype etype, EMPI_Datatype filetype, char *datarep, EMPI_Info info );
int EMPI_File_sync ( EMPI_File EMPI_fh );
int EMPI_File_write_all_begin ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype );
int EMPI_File_write_all_end ( EMPI_File EMPI_fh, void *buf, EMPI_Status *status );
int EMPI_File_write_all ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status );
int EMPI_File_write_at_all_begin ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype datatype );
int EMPI_File_write_at_all_end ( EMPI_File EMPI_fh, void *buf, EMPI_Status *status );
int EMPI_File_write_at_all ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status );
int EMPI_File_write_at ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status );
int EMPI_File_write ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status );
int EMPI_File_write_ordered_begin ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype );
int EMPI_File_write_ordered_end ( EMPI_File EMPI_fh, void *buf, EMPI_Status *status );
int EMPI_File_write_ordered ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status );
int EMPI_File_write_shared ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status );
int EMPI_Free_mem ( void *base );
int EMPI_Gather ( void *sendbuf, int sendcnt, EMPI_Datatype sendtype, void *recvbuf, int recvcnt, EMPI_Datatype recvtype, int root, EMPI_Comm comm );
int EMPI_Gatherv ( void *sendbuf, int sendcnt, EMPI_Datatype sendtype, void *recvbuf, int *recvcnts, int *displs, EMPI_Datatype recvtype, int root, EMPI_Comm comm );
int EMPI_Get_address ( void *location, EMPI_Aint *address );
int EMPI_Get_count ( EMPI_Status *status, EMPI_Datatype datatype, int *count );
int EMPI_Get_elements ( EMPI_Status *status, EMPI_Datatype datatype, int *elements );
int EMPI_Get ( void *origin_addr, int origin_count, EMPI_Datatype origin_datatype, int target_rank, EMPI_Aint target_disp, int target_count, EMPI_Datatype target_datatype, EMPI_Win win );
int EMPI_Get_processor_name ( char *name, int *resultlen );
int EMPI_Get_version ( int *version, int *subversion );
int EMPI_Graph_create ( EMPI_Comm comm_old, int nnodes, int *indx, int *edges, int reorder, EMPI_Comm *comm_graph );
int EMPI_Graphdims_get ( EMPI_Comm comm, int *nnodes, int *nedges );
int EMPI_Graph_get ( EMPI_Comm comm, int maxindex, int maxedges, int *indx, int *edges );
int EMPI_Graph_map ( EMPI_Comm comm_old, int nnodes, int *indx, int *edges, int *newrank );
int EMPI_Graph_neighbors_count ( EMPI_Comm comm, int rank, int *nneighbors );
int EMPI_Graph_neighbors ( EMPI_Comm comm, int rank, int maxneighbors, int *neighbors );
int EMPI_Grequest_complete ( EMPI_Request r );
int EMPI_Grequest_start ( EMPI_Grequest_query_function *query_fn, EMPI_Grequest_free_function *free_fn, EMPI_Grequest_cancel_function *cancel_fn, void *extra_state, EMPI_Request *r );
int EMPI_Group_compare ( EMPI_Group group1, EMPI_Group group2, int *result );
int EMPI_Group_difference ( EMPI_Group group1, EMPI_Group group2, EMPI_Group *newgroup );
int EMPI_Group_excl ( EMPI_Group g, int n, int *ranks, EMPI_Group *newgroup );
int EMPI_Group_free ( EMPI_Group *g );
int EMPI_Group_incl ( EMPI_Group g, int n, int *ranks, EMPI_Group *newgroup );
int EMPI_Group_intersection ( EMPI_Group group1, EMPI_Group group2, EMPI_Group *newgroup );
int EMPI_Group_range_excl ( EMPI_Group g, int n, int ranges[][3], EMPI_Group *newgroup );
int EMPI_Group_range_incl ( EMPI_Group g, int n, int ranges[][3], EMPI_Group *newgroup );
int EMPI_Group_rank ( EMPI_Group g, int *rank );
int EMPI_Group_size ( EMPI_Group g, int *size );
int EMPI_Group_translate_ranks ( EMPI_Group group1, int n, int *ranks1, EMPI_Group group2, int *ranks2 );
int EMPI_Group_union ( EMPI_Group group1, EMPI_Group group2, EMPI_Group *newgroup );
int EMPI_Ibsend ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm, EMPI_Request *r );
int EMPI_Info_create ( EMPI_Info *info );
int EMPI_Info_delete ( EMPI_Info info, char *key );
int EMPI_Info_dup ( EMPI_Info info, EMPI_Info *newinfo );
int EMPI_Info_free ( EMPI_Info *info );
int EMPI_Info_get ( EMPI_Info info, char *key, int valuelen, char *value, int *flag );
int EMPI_Info_get_nkeys ( EMPI_Info info, int *nkeys );
int EMPI_Info_get_nthkey ( EMPI_Info info, int n, char *key );
int EMPI_Info_get_valuelen ( EMPI_Info info, char *key, int *valuelen, int *flag );
int EMPI_Info_set ( EMPI_Info info, char *key, char *value );
int EMPI_Initialized ( int *flag );
int EMPI_Init_thread ( int *argc, char ***argv, int required, int *provided );
int EMPI_Intercomm_create ( EMPI_Comm local_comm, int local_leader, EMPI_Comm peer_comm, int remote_leader, int tag, EMPI_Comm *newintercomm );
int EMPI_Intercomm_merge ( EMPI_Comm intercomm, int high, EMPI_Comm *newintracomm );
int EMPI_Iprobe ( int source, int tag, EMPI_Comm comm, int *flag, EMPI_Status *status );
int EMPI_Irecv ( void *buf, int count, EMPI_Datatype datatype, int source, int tag, EMPI_Comm comm, EMPI_Request *r );
int EMPI_Irsend ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm, EMPI_Request *r );
int EMPI_Isend ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm, EMPI_Request *r );
int EMPI_Issend ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm, EMPI_Request *r );
int EMPI_Is_thread_main ( int *flag );
int EMPI_Keyval_create ( EMPI_Copy_function *copy_fn, EMPI_Delete_function *delete_fn, int *keyval, void *extra_state );
int EMPI_Keyval_free ( int *keyval );
int EMPI_Lookup_name ( char *service_name, EMPI_Info info, char *port_name );
int EMPI_Op_commutative ( EMPI_Op op, int *commute );
int EMPI_Op_create ( EMPI_User_function *function, int commute, EMPI_Op *op );
int EMPI_Open_port ( EMPI_Info info, char *port_name );
int EMPI_Op_free ( EMPI_Op *op );
int EMPI_Pack_external ( char *datarep, void *inbuf, int incount, EMPI_Datatype datatype, void *outbuf, EMPI_Aint outcount, EMPI_Aint *position );
int EMPI_Pack_external_size ( char *datarep, int incount, EMPI_Datatype datatype, EMPI_Aint *size );
int EMPI_Pack ( void *inbuf, int incount, EMPI_Datatype datatype, void *outbuf, int outcount, int *position, EMPI_Comm comm );
int EMPI_Pack_size ( int incount, EMPI_Datatype datatype, EMPI_Comm comm, int *size );
int EMPI_Probe ( int source, int tag, EMPI_Comm comm, EMPI_Status *status );
int EMPI_Publish_name ( char *service_name, EMPI_Info info, char *port_name );
int EMPI_Put ( void *origin_addr, int origin_count, EMPI_Datatype origin_datatype, int target_rank, EMPI_Aint target_disp, int target_count, EMPI_Datatype target_datatype, EMPI_Win win );
int EMPI_Query_thread ( int *provided );
int EMPI_Recv ( void *buf, int count, EMPI_Datatype datatype, int source, int tag, EMPI_Comm comm, EMPI_Status *status );
int EMPI_Recv_init ( void *buf, int count, EMPI_Datatype datatype, int source, int tag, EMPI_Comm comm, EMPI_Request *r );
int EMPI_Reduce ( void *sendbuf, void *recvbuf, int count, EMPI_Datatype datatype, EMPI_Op op, int root, EMPI_Comm comm );
int EMPI_Reduce_local ( void *inbuf, void *inoutbuf, int count, EMPI_Datatype datatype, EMPI_Op op );
int EMPI_Reduce_scatter_block ( void *sendbuf, void *recvbuf, int recvcount, EMPI_Datatype datatype, EMPI_Op op, EMPI_Comm comm );
int EMPI_Reduce_scatter ( void *sendbuf, void *recvbuf, int *recvcnts, EMPI_Datatype datatype, EMPI_Op op, EMPI_Comm comm );
int EMPI_Register_datarep ( char *name, EMPI_Datarep_conversion_function *read_conv_fn, EMPI_Datarep_conversion_function *write_conv_fn, EMPI_Datarep_extent_function *extent_fn, void *state );
int EMPI_Request_free ( EMPI_Request *r );
int EMPI_Request_get_status ( EMPI_Request r, int *flag, EMPI_Status *status );
int EMPI_Rsend ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm );
int EMPI_Rsend_init ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm, EMPI_Request *r );
int EMPI_Scan ( void *sendbuf, void *recvbuf, int count, EMPI_Datatype datatype, EMPI_Op op, EMPI_Comm comm );
int EMPI_Scatter ( void *sendbuf, int sendcnt, EMPI_Datatype sendtype, void *recvbuf, int recvcnt, EMPI_Datatype recvtype, int root, EMPI_Comm comm );
int EMPI_Scatterv ( void *sendbuf, int *sendcnts, int *displs, EMPI_Datatype sendtype, void *recvbuf, int recvcnt, EMPI_Datatype recvtype, int root, EMPI_Comm comm );
int EMPI_Send ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm );
int EMPI_Send_init ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm, EMPI_Request *r );
int EMPI_Sendrecv ( void *sendbuf, int sendcount, EMPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, EMPI_Datatype recvtype, int source, int recvtag, EMPI_Comm comm, EMPI_Status *status );
int EMPI_Sendrecv_replace ( void *buf, int count, EMPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, EMPI_Comm comm, EMPI_Status *status );
int EMPI_Ssend ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm );
int EMPI_Ssend_init ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm, EMPI_Request *r );
int EMPI_Startall ( int count, EMPI_Request array_of_requests[] );
int EMPI_Start ( EMPI_Request *r );
int EMPI_Status_set_cancelled ( EMPI_Status *status, int flag );
int EMPI_Status_set_elements ( EMPI_Status *status, EMPI_Datatype datatype, int count );
int EMPI_Testall ( int count, EMPI_Request array_of_requests[], int *flag, EMPI_Status array_of_statuses[] );
int EMPI_Testany ( int count, EMPI_Request array_of_requests[], int *index, int *flag, EMPI_Status *status );
int EMPI_Test_cancelled ( EMPI_Status *status, int *flag );
int EMPI_Test ( EMPI_Request *r, int *flag, EMPI_Status *status );
int EMPI_Testsome ( int incount, EMPI_Request array_of_requests[], int *outcount, int array_of_indices[], EMPI_Status array_of_statuses[] );
int EMPI_Topo_test ( EMPI_Comm comm, int *topo_type );
int EMPI_Type_commit ( EMPI_Datatype *datatype );
int EMPI_Type_contiguous ( int count, EMPI_Datatype old_type, EMPI_Datatype *new_type_p );
int EMPI_Type_create_darray ( int size, int rank, int ndims, int array_of_gsizes[], int array_of_distribs[], int array_of_dargs[], int array_of_psizes[], int order, EMPI_Datatype oldtype, EMPI_Datatype *newtype );
int EMPI_Type_create_hindexed ( int count, int blocklengths[], EMPI_Aint displacements[], EMPI_Datatype oldtype, EMPI_Datatype *newtype );
int EMPI_Type_create_hvector ( int count, int blocklength, EMPI_Aint stride, EMPI_Datatype oldtype, EMPI_Datatype *newtype );
int EMPI_Type_create_indexed_block ( int count, int blocklength, int array_of_displacements[], EMPI_Datatype oldtype, EMPI_Datatype *newtype );
int EMPI_Type_create_keyval ( EMPI_Type_copy_attr_function *type_copy_attr_fn, EMPI_Type_delete_attr_function *type_delete_attr_fn, int *type_keyval, void *extra_state );
int EMPI_Type_create_resized ( EMPI_Datatype oldtype, EMPI_Aint lb, EMPI_Aint extent, EMPI_Datatype *newtype );
int EMPI_Type_create_struct ( int count, int array_of_blocklengths[], EMPI_Aint array_of_displacements[], EMPI_Datatype array_of_types[], EMPI_Datatype *newtype );
int EMPI_Type_create_subarray ( int ndims, int array_of_sizes[], int array_of_subsizes[], int array_of_starts[], int order, EMPI_Datatype oldtype, EMPI_Datatype *newtype );
int EMPI_Type_delete_attr ( EMPI_Datatype type, int type_keyval );
int EMPI_Type_dup ( EMPI_Datatype datatype, EMPI_Datatype *newtype );
int EMPI_Type_extent ( EMPI_Datatype datatype, EMPI_Aint *extent );
int EMPI_Type_free ( EMPI_Datatype *datatype );
int EMPI_Type_free_keyval ( int *type_keyval );
int EMPI_Type_get_attr ( EMPI_Datatype type, int type_keyval, void *attribute_val, int *flag );
int EMPI_Type_get_contents ( EMPI_Datatype datatype, int max_integers, int max_addresses, int max_datatypes, int array_of_integers[], EMPI_Aint array_of_addresses[], EMPI_Datatype array_of_datatypes[] );
int EMPI_Type_get_envelope ( EMPI_Datatype datatype, int *num_integers, int *num_addresses, int *num_datatypes, int *combiner );
int EMPI_Type_get_extent ( EMPI_Datatype datatype, EMPI_Aint *lb, EMPI_Aint *extent );
int EMPI_Type_get_true_extent ( EMPI_Datatype datatype, EMPI_Aint *true_lb, EMPI_Aint *true_extent );
int EMPI_Type_hindexed ( int count, int blocklens[], EMPI_Aint indices[], EMPI_Datatype old_type, EMPI_Datatype *newtype );
int EMPI_Type_hvector ( int count, int blocklen, EMPI_Aint stride, EMPI_Datatype old_type, EMPI_Datatype *newtype_p );
int EMPI_Type_indexed ( int count, int blocklens[], int indices[], EMPI_Datatype old_type, EMPI_Datatype *newtype );
int EMPI_Type_lb ( EMPI_Datatype datatype, EMPI_Aint *displacement );
int EMPI_Type_match_size ( int typeclass, int size, EMPI_Datatype *datatype );
int EMPI_Type_set_attr ( EMPI_Datatype type, int type_keyval, void *attribute_val );
int EMPI_Type_set_name ( EMPI_Datatype type, char *type_name );
int EMPI_Type_size ( EMPI_Datatype datatype, int *size );
int EMPI_Type_struct ( int count, int blocklens[], EMPI_Aint indices[], EMPI_Datatype old_types[], EMPI_Datatype *newtype );
int EMPI_Type_ub ( EMPI_Datatype datatype, EMPI_Aint *displacement );
int EMPI_Type_vector ( int count, int blocklength, int stride, EMPI_Datatype old_type, EMPI_Datatype *newtype_p );
int EMPI_Unpack_external ( char *datarep, void *inbuf, EMPI_Aint insize, EMPI_Aint *position, void *outbuf, int outcount, EMPI_Datatype datatype );
int EMPI_Unpack ( void *inbuf, int insize, int *position, void *outbuf, int outcount, EMPI_Datatype datatype, EMPI_Comm comm );
int EMPI_Unpublish_name ( char *service_name, EMPI_Info info, char *port_name );
int EMPI_Waitall ( int count, EMPI_Request array_of_requests[], EMPI_Status array_of_statuses[] );
int EMPI_Waitany ( int count, EMPI_Request array_of_requests[], int *index, EMPI_Status *status );
int EMPI_Wait ( EMPI_Request *r, EMPI_Status *status );
int EMPI_Waitsome ( int incount, EMPI_Request array_of_requests[], int *outcount, int array_of_indices[], EMPI_Status array_of_statuses[] );
int EMPI_Win_call_errhandler ( EMPI_Win win, int errorcode );
int EMPI_Win_complete ( EMPI_Win win );
int EMPI_Win_create_errhandler ( EMPI_Win_errhandler_fn *function, EMPI_Errhandler *errhandler );
int EMPI_Win_create ( void *base, EMPI_Aint size, int disp_unit, EMPI_Info info, EMPI_Comm comm, EMPI_Win *win );
int EMPI_Win_create_keyval ( EMPI_Win_copy_attr_function *win_copy_attr_fn, EMPI_Win_delete_attr_function *win_delete_attr_fn, int *win_keyval, void *extra_state );
int EMPI_Win_delete_attr ( EMPI_Win win, int win_keyval );
int EMPI_Win_fence ( int assert, EMPI_Win win );
int EMPI_Win_free ( EMPI_Win *win );
int EMPI_Win_free_keyval ( int *win_keyval );
int EMPI_Win_get_attr ( EMPI_Win win, int win_keyval, void *attribute_val, int *flag );
int EMPI_Win_get_errhandler ( EMPI_Win win, EMPI_Errhandler *errhandler );
int EMPI_Win_get_group ( EMPI_Win win, EMPI_Group *g );
int EMPI_Win_get_name ( EMPI_Win win, char *win_name, int *resultlen );
int EMPI_Win_lock ( int lock_type, int rank, int assert, EMPI_Win win );
int EMPI_Win_post ( EMPI_Group g, int assert, EMPI_Win win );
int EMPI_Win_set_attr ( EMPI_Win win, int win_keyval, void *attribute_val );
int EMPI_Win_set_errhandler ( EMPI_Win win, EMPI_Errhandler errhandler );
int EMPI_Win_set_name ( EMPI_Win win, char *win_name );
int EMPI_Win_start ( EMPI_Group g, int assert, EMPI_Win win );
int EMPI_Win_test ( EMPI_Win win, int *flag );
int EMPI_Win_unlock ( int rank, EMPI_Win win );
int EMPI_Win_wait ( EMPI_Win win );
int EMPI_Group_comm_create ( EMPI_Comm old_comm, EMPI_Group g, int tag, EMPI_Comm *new_comm );
EMPI_Fint EMPI_Comm_c2f ( EMPI_Comm comm );
EMPI_Fint EMPI_Group_c2f ( EMPI_Group g );
EMPI_Fint EMPI_Request_c2f ( EMPI_Request r );
EMPI_Fint EMPI_Info_c2f ( EMPI_Info info );
EMPI_Fint EMPI_File_c2f ( EMPI_File file );
EMPI_Fint EMPI_Op_c2f ( EMPI_Op op );
EMPI_Fint EMPI_Win_c2f ( EMPI_Win Win );
EMPI_Fint EMPI_Errhandler_c2f ( EMPI_Errhandler Errhandler );
EMPI_Fint EMPI_Type_c2f ( EMPI_Datatype Type );
*/

#endif // __EMPI_H_
