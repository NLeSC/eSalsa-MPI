#include "mpi.h"

#define FORT_NAME(lower,upper) lower##_

/* Misc. utilities */

void FORT_NAME( mpi_init , MPI_INIT ) ( int *ierr )
{
	// TODO!!!
}

void FORT_NAME( mpi_initialized , MPI_INITIALIZED ) ( int *flag , int *ierr )
{
   *ierr = MPI_Initialized(flag);
}

void FORT_NAME( mpi_abort , MPI_ABORT ) ( int *comm, int *errorcode , int *ierr )
{
   *ierr = MPI_Abort(*comm, *errorcode);
}

void FORT_NAME( mpi_finalize , MPI_FINALIZE ) ( int *ierr )
{
   *ierr = MPI_Finalize();
}

void FORT_NAME( mpi_finalized , MPI_FINALIZED ) ( int *flag , int *ierr )
{
   *ierr = MPI_Finalized(flag);
}

double FORT_NAME( mpi_wtime , MPI_WTIME ) ( void )
{
   return MPI_Wtime();
}

void FORT_NAME( mpi_get_processor_name , MPI_GET_PROCESSOR_NAME ) ( char *name, int *resultlen , int *ierr )
{
   // FIXME: does this work ?
   *ierr = MPI_Get_processor_name(name, resultlen);
}

void FORT_NAME( mpi_error_string , MPI_ERROR_STRING ) ( int *errorcode, char *string, int *resultlen , int *ierr )
{
   // FIXME: does this work ?
   *ierr = MPI_Error_string(*errorcode, string, resultlen);
}



/* Communicators and groups */

void FORT_NAME( mpi_comm_create , MPI_COMM_CREATE ) ( int *comm, int *g, int *newcomm , int *ierr )
{
   *ierr = MPI_Comm_create(*comm, *g, newcomm);
}


void FORT_NAME( mpi_comm_dup , MPI_COMM_DUP ) ( int *comm, int *newcomm , int *ierr )
{
   *ierr = MPI_Comm_dup(*comm, newcomm);
}

void FORT_NAME( mpi_comm_free , MPI_COMM_FREE ) ( int *comm , int *ierr )
{
   *ierr = MPI_Comm_free(comm);
}

void FORT_NAME( mpi_comm_group , MPI_COMM_GROUP ) ( int *comm, int *g , int *ierr )
{
   *ierr = MPI_Comm_group(*comm, g);
}


void FORT_NAME( mpi_comm_rank , MPI_COMM_RANK ) ( int *comm, int *rank , int *ierr )
{
   *ierr = MPI_Comm_rank(*comm, rank);
}

void FORT_NAME( mpi_comm_size , MPI_COMM_SIZE ) ( int *comm, int *size , int *ierr )
{
   *ierr = MPI_Comm_size(*comm, size);
}

void FORT_NAME( mpi_comm_split , MPI_COMM_SPLIT ) ( int *comm, int *color, int *key, int *newcomm , int *ierr )
{
   *ierr = MPI_Comm_split(*comm, *color, *key, newcomm);
}

void FORT_NAME( mpi_group_range_incl , MPI_GROUP_RANGE_INCL ) ( int *g, int *n, int ranges[][3], int *newgroup , int *ierr )
{
   *ierr = MPI_Group_range_incl(*g, *n, ranges, newgroup);
}

void FORT_NAME( mpi_group_incl , MPI_GROUP_INCL ) ( int *g, int *n, int *ranks, int *newgroup , int *ierr )
{
   *ierr = MPI_Group_incl(*g, *n, ranks, newgroup);
}

void FORT_NAME( mpi_group_range_excl , MPI_GROUP_RANGE_EXCL ) ( int *g, int *n, int ranges[][3], int *newgroup , int *ierr )
{
   *ierr = MPI_Group_range_excl(*g, *n, ranges, newgroup);
}

void FORT_NAME( mpi_group_union , MPI_GROUP_UNION ) ( int *group1, int *group2, int *newgroup , int *ierr )
{
   *ierr = MPI_Group_union(*group1, *group2, newgroup);
}

void FORT_NAME( mpi_group_translate_ranks , MPI_GROUP_TRANSLATE_RANKS ) ( int *group1, int *n, int *ranks1, int *group2, int *ranks2 , int *ierr )
{
   *ierr = MPI_Group_translate_ranks(*group1, *n, ranks1, *group2, ranks2);
}


/* Collectives */

void FORT_NAME( mpi_allgather , MPI_ALLGATHER ) ( void *sendbuf, int *sendcount, int *sendtype, void *recvbuf, int *recvcount, int *recvtype, int *comm , int *ierr )
{
   *ierr = MPI_Allgather(sendbuf, *sendcount, *sendtype, recvbuf, *recvcount, *recvtype, *comm);
}

void FORT_NAME( mpi_allgatherv , MPI_ALLGATHERV ) ( void *sendbuf, int *sendcount, int *sendtype, void *recvbuf, int *recvcounts, int *displs, int *recvtype, int *comm , int *ierr )
{
   *ierr = MPI_Allgatherv(sendbuf, *sendcount, *sendtype, recvbuf, recvcounts, displs, *recvtype, *comm);
}

void FORT_NAME( mpi_allreduce , MPI_ALLREDUCE ) ( void *sendbuf, void *recvbuf, int *count, int *type, int *op, int *comm , int *ierr )
{
   *ierr = MPI_Allreduce(sendbuf, recvbuf, *count, *type, *op, *comm);
}

void FORT_NAME( mpi_alltoall , MPI_ALLTOALL ) ( void *sendbuf, int *sendcount, int *sendtype, void *recvbuf, int *recvcount, int *recvtype, int *comm , int *ierr )
{
   *ierr = MPI_Alltoall(sendbuf, *sendcount, *sendtype, recvbuf, *recvcount, *recvtype, *comm);
}

void FORT_NAME( mpi_alltoallv , MPI_ALLTOALLV ) ( void *sendbuf, int *sendcnts, int *sdispls, int *sendtype, void *recvbuf, int *recvcnts, int *rdispls, int *recvtype, int *comm , int *ierr )
{
   *ierr = MPI_Alltoallv(sendbuf, sendcnts, sdispls, *sendtype, recvbuf, recvcnts, rdispls, *recvtype, *comm);
}

void FORT_NAME( mpi_alltoallw , MPI_ALLTOALLW ) ( void *sendbuf, int *sendcnts, int *sdispls, int *sendtypes, void *recvbuf, int *recvcnts, int *rdispls, int *recvtypes, int *comm , int *ierr )
{
   *ierr = MPI_Alltoallw(sendbuf, sendcnts, sdispls, sendtypes, recvbuf, recvcnts, rdispls, recvtypes, *comm);
}

void FORT_NAME( mpi_scatter , MPI_SCATTER ) ( void *sendbuf, int *sendcnt, int *sendtype, void *recvbuf, int *recvcnt, int *recvtype, int *root, int *comm , int *ierr )
{
   *ierr = MPI_Scatter(sendbuf, *sendcnt, *sendtype, recvbuf, *recvcnt, *recvtype, *root, *comm);
}

void FORT_NAME( mpi_scatterv , MPI_SCATTERV ) ( void *sendbuf, int *sendcnts, int *displs, int *sendtype, void *recvbuf, int *recvcnt, int *recvtype, int *root, int *comm , int *ierr )
{
   *ierr = MPI_Scatterv(sendbuf, sendcnts, displs, *sendtype, recvbuf, *recvcnt, *recvtype, *root, *comm);
}

void FORT_NAME( mpi_barrier , MPI_BARRIER ) ( int *comm , int *ierr )
{
   *ierr = MPI_Barrier(*comm);
}

void FORT_NAME( mpi_bcast , MPI_BCAST ) ( void *buffer, int *count, int *type, int *root, int *comm , int *ierr )
{
   *ierr = MPI_Bcast(buffer, *count, *type, *root, *comm);
}

void FORT_NAME( mpi_gather , MPI_GATHER ) ( void *sendbuf, int *sendcnt, int *sendtype, void *recvbuf, int *recvcnt, int *recvtype, int *root, int *comm , int *ierr )
{
   *ierr = MPI_Gather(sendbuf, *sendcnt, *sendtype, recvbuf, *recvcnt, *recvtype, *root, *comm);
}

void FORT_NAME( mpi_gatherv , MPI_GATHERV ) ( void *sendbuf, int *sendcnt, int *sendtype, void *recvbuf, int *recvcnts, int *displs, int *recvtype, int *root, int *comm , int *ierr )
{
   *ierr = MPI_Gatherv(sendbuf, *sendcnt, *sendtype, recvbuf, recvcnts, displs, *recvtype, *root, *comm);
}

void FORT_NAME( mpi_reduce , MPI_REDUCE ) ( void *sendbuf, void *recvbuf, int *count, int *type, int *op, int *root, int *comm , int *ierr )
{
   *ierr = MPI_Reduce(sendbuf, recvbuf, *count, *type, *op, *root, *comm);
}


/* Send / receive */

//void FORT_NAME( mpi_ibsend , MPI_IBSEND ) ( void *buf, int *count, int *type, int *dest, int *tag, int *comm, int *r , int *ierr )
//{
//   *ierr = MPI_Ibsend(buf, *count, *type, *dest, *tag, *comm, r);
//}

void FORT_NAME( mpi_irecv , MPI_IRECV ) ( void *buf, int *count, int *type, int *source, int *tag, int *comm, int *r , int *ierr )
{
   *ierr = MPI_Irecv(buf, *count, *type, *source, *tag, *comm, r);
}

void FORT_NAME( mpi_irsend , MPI_IRSEND ) ( void *buf, int *count, int *type, int *dest, int *tag, int *comm, int *r , int *ierr )
{
   *ierr = MPI_Irsend(buf, *count, *type, *dest, *tag, *comm, r);
}

void FORT_NAME( mpi_isend , MPI_ISEND ) ( void *buf, int *count, int *type, int *dest, int *tag, int *comm, int *r , int *ierr )
{
   *ierr = MPI_Isend(buf, *count, *type, *dest, *tag, *comm, r);
}

void FORT_NAME( mpi_rsend , MPI_RSEND ) ( void *buf, int *count, int *type, int *dest, int *tag, int *comm , int *ierr )
{
   *ierr = MPI_Rsend(buf, *count, *type, *dest, *tag, *comm);
}

void FORT_NAME( mpi_send , MPI_SEND ) ( void *buf, int *count, int *type, int *dest, int *tag, int *comm , int *ierr )
{
   *ierr = MPI_Send(buf, *count, *type, *dest, *tag, *comm);
}

void FORT_NAME( mpi_sendrecv , MPI_SENDRECV ) ( void *sendbuf, int *sendcount, int *sendtype, int *dest, int *sendtag, void *recvbuf, int *recvcount, int *recvtype, int *source, int *recvtag, int *comm, int *stat , int *ierr )
{
   *ierr = MPI_Sendrecv(sendbuf, *sendcount, *sendtype, *dest, *sendtag, recvbuf, *recvcount, *recvtype, *source, *recvtag, *comm, (MPI_Status *) stat);
}

void FORT_NAME( mpi_ssend , MPI_SSEND ) ( void *buf, int *count, int *type, int *dest, int *tag, int *comm , int *ierr )
{
   *ierr = MPI_Ssend(buf, *count, *type, *dest, *tag, *comm);
}

void FORT_NAME( mpi_recv , MPI_RECV ) ( void *buf, int *count, int *type, int *source, int *tag, int *comm, int *stat, int *ierr )
{
   *ierr = MPI_Recv(buf, *count, *type, *source, *tag, *comm, (MPI_Status *)stat);
}


/* Request / status */

void FORT_NAME( mpi_wait , MPI_WAIT ) ( int *r, int *stat , int *ierr )
{
   *ierr = MPI_Wait(r, (MPI_Status *)stat);
}

void FORT_NAME( mpi_waitall , MPI_WAITALL ) ( int *count, int *array_of_requests, int *array_of_statuses , int *ierr )
{
   *ierr = MPI_Waitall(*count, array_of_requests, (MPI_Status *)array_of_statuses);
}

void FORT_NAME( mpi_waitany , MPI_WAITANY ) ( int *count, int *array_of_requests, int *index, MPI_Status *stat , int *ierr )
{
   *ierr = MPI_Waitany(*count, array_of_requests, index, (MPI_Status *)stat);
}

void FORT_NAME( mpi_request_free , MPI_REQUEST_FREE ) ( int *r , int *ierr )
{
   *ierr = MPI_Request_free(r);
}



/* I/O */


void FORT_NAME( mpi_file_open , MPI_FILE_OPEN ) ( int *comm, char *filename, int *amode, int *info, int *fh , int *ierr )
{
   *ierr = MPI_File_open(*comm, filename, *amode, *info, fh);
}


void FORT_NAME( mpi_file_close , MPI_FILE_CLOSE ) ( int *fh , int *ierr )
{
   *ierr = MPI_File_close(fh);
}

void FORT_NAME( mpi_file_read_all , MPI_FILE_READ_ALL ) ( int *fh, void *buf, int *count, int *type, int *stat , int *ierr )
{
   *ierr = MPI_File_read_all(*fh, buf, *count, *type, (MPI_Status *)stat);
}

void FORT_NAME( mpi_file_read_at , MPI_FILE_READ_AT ) ( int *fh, int *offset, void *buf, int *count, int *type, int *stat , int *ierr )
{
   *ierr = MPI_File_read_at(*fh, *offset, buf, *count, *type, (MPI_Status *)stat);
}

void FORT_NAME( mpi_file_write_at , MPI_FILE_WRITE_AT ) ( int *fh, int *offset, void *buf, int *count, int *type, int *stat , int *ierr )
{
   *ierr = MPI_File_write_at(*fh, *offset, buf, *count, *type, (MPI_Status *) stat);
}

void FORT_NAME( mpi_file_set_view , MPI_FILE_SET_VIEW ) ( int *fh, int *disp, int *etype, int *filetype, char *datarep, int *info , int *ierr )
{
   *ierr = MPI_File_set_view(*fh, *disp, *etype, *filetype, datarep, *info);
}

void FORT_NAME( mpi_file_write_all , MPI_FILE_WRITE_ALL ) ( int *fh, void *buf, int *count, int *type, int *stat , int *ierr )
{
   *ierr = MPI_File_write_all(*fh, buf, *count, *type, (MPI_Status *) stat);
}

/* Info */

void FORT_NAME( mpi_info_set , MPI_INFO_SET ) ( int *info, char *key, char *value , int *ierr )
{
   *ierr = MPI_Info_set(*info, key, value);
}

void FORT_NAME( mpi_info_create , MPI_INFO_CREATE ) ( int *info , int *ierr )
{
   *ierr = MPI_Info_create(info);
}


void FORT_NAME( mpi_info_delete , MPI_INFO_DELETE ) ( int *info, char *key , int *ierr )
{
   *ierr = MPI_Info_delete(*info, key);
}

void FORT_NAME( mpi_info_free , MPI_INFO_FREE ) ( int *info , int *ierr )
{
   *ierr = MPI_Info_free(info);
}


/* Datatypes */

void FORT_NAME( mpi_type_free , MPI_TYPE_FREE ) ( int *type , int *ierr )
{
   *ierr = MPI_Type_free(type);
}

void FORT_NAME( mpi_type_commit , MPI_TYPE_COMMIT ) ( int *type , int *ierr )
{
   *ierr = MPI_Type_commit(type);
}

void FORT_NAME( mpi_type_contiguous , MPI_TYPE_CONTIGUOUS ) ( int *count, int *old_type, int *new_type , int *ierr )
{
   *ierr = MPI_Type_contiguous(*count, *old_type, new_type);
}

void FORT_NAME( mpi_type_create_indexed_block , MPI_TYPE_CREATE_INDEXED_BLOCK ) ( int *count, int *blocklength, int *array_of_displacements, int *oldtype, int *newtype , int *ierr )
{
   *ierr = MPI_Type_create_indexed_block(*count, *blocklength, array_of_displacements, *oldtype, newtype);
}

void FORT_NAME( mpi_type_get_envelope , MPI_TYPE_GET_ENVELOPE ) ( int *type, int *num_integers, int *num_addresses, int *num_datatypes, int *combiner , int *ierr )
{
   *ierr = MPI_Type_get_envelope(*type, num_integers, num_addresses, num_datatypes, combiner);
}


/* Intercomm */

void FORT_NAME( mpi_intercomm_create , MPI_INTERCOMM_CREATE ) ( int *local_comm, int *local_leader, int *peer_comm, int *remote_leader, int *tag, int *newintercomm , int *ierr )
{
   *ierr = MPI_Intercomm_create(*local_comm, *local_leader, *peer_comm, *remote_leader, *tag, newintercomm);
}

void FORT_NAME( mpi_intercomm_merge , MPI_INTERCOMM_MERGE ) ( int *intercomm, int *high, int *newintracomm , int *ierr )
{
   *ierr = MPI_Intercomm_merge(*intercomm, *high, newintracomm);
}





// HIERO




/*


void FORT_NAME( mpi_add_error_string , MPI_ADD_ERROR_STRING ) ( int errorcode, char *string , int *resultlen, int *ierr )
{
}


FORT_NAME( mpi_type_create_f90_complex , MPI_TYPE_CREATE_F90_COMPLEX ) ( int p, int r, MPI_Datatype *newtype , int *ierr )
{
}

FORT_NAME( mpi_type_create_f90_integer , MPI_TYPE_CREATE_F90_INTEGER ) ( int r, MPI_Datatype *newtype , int *ierr )
{
}

FORT_NAME( mpi_type_create_f90_real , MPI_TYPE_CREATE_F90_REAL ) ( int p, int r, MPI_Datatype *newtype , int *ierr )
{
}

FORT_NAME( mpi_accumulate , MPI_ACCUMULATE ) ( void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Op op, MPI_Win win , int *ierr )
{
}

FORT_NAME( mpi_add_error_class , MPI_ADD_ERROR_CLASS ) ( int *errorclass , int *ierr )
{
}

FORT_NAME( mpi_add_error_code , MPI_ADD_ERROR_CODE ) ( int errorclass, int *errorcode , int *ierr )
{
}


FORT_NAME( mpi_address , MPI_ADDRESS ) ( void *location, MPI_Aint *address , int *ierr )
{
}


FORT_NAME( mpi_alloc_mem , MPI_ALLOC_MEM ) ( MPI_Aint size, MPI_Info info, void *baseptr , int *ierr )
{
}


FORT_NAME( mpi_attr_delete , MPI_ATTR_DELETE ) ( MPI_Comm comm, int keyval , int *ierr )
{
}

FORT_NAME( mpi_attr_get , MPI_ATTR_GET ) ( MPI_Comm comm, int keyval, void *attr_value, int *flag , int *ierr )
{
}

FORT_NAME( mpi_attr_put , MPI_ATTR_PUT ) ( MPI_Comm comm, int keyval, void *attr_value , int *ierr )
{
}


FORT_NAME( mpi_bsend , MPI_BSEND ) ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm , int *ierr )
{
}

FORT_NAME( mpi_bsend_init , MPI_BSEND_INIT ) ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r , int *ierr )
{
}

FORT_NAME( mpi_buffer_attach , MPI_BUFFER_ATTACH ) ( void *buffer, int size , int *ierr )
{
}

FORT_NAME( mpi_buffer_detach , MPI_BUFFER_DETACH ) ( void *buffer, int *size , int *ierr )
{
}

FORT_NAME( mpi_cancel , MPI_CANCEL ) ( MPI_Request *r , int *ierr )
{
}

FORT_NAME( mpi_cart_coords , MPI_CART_COORDS ) ( MPI_Comm comm, int rank, int maxdims, int *coords , int *ierr )
{
}

FORT_NAME( mpi_cart_create , MPI_CART_CREATE ) ( MPI_Comm comm_old, int ndims, int *dims, int *periods, int reorder, MPI_Comm *comm_cart , int *ierr )
{
}

FORT_NAME( mpi_cartdim_get , MPI_CARTDIM_GET ) ( MPI_Comm comm, int *ndims , int *ierr )
{
}

FORT_NAME( mpi_cart_get , MPI_CART_GET ) ( MPI_Comm comm, int maxdims, int *dims, int *periods, int *coords , int *ierr )
{
}

FORT_NAME( mpi_cart_map , MPI_CART_MAP ) ( MPI_Comm comm_old, int ndims, int *dims, int *periods, int *newrank , int *ierr )
{
}

FORT_NAME( mpi_cart_rank , MPI_CART_RANK ) ( MPI_Comm comm, int *coords, int *rank , int *ierr )
{
}

FORT_NAME( mpi_cart_shift , MPI_CART_SHIFT ) ( MPI_Comm comm, int direction, int displ, int *source, int *dest , int *ierr )
{
}

FORT_NAME( mpi_cart_sub , MPI_CART_SUB ) ( MPI_Comm comm, int *remain_dims, MPI_Comm *comm_new , int *ierr )
{
}

FORT_NAME( mpi_close_port , MPI_CLOSE_PORT ) ( char *port_name , int *ierr )
{
}

FORT_NAME( mpi_comm_accept , MPI_COMM_ACCEPT ) ( char *port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *newcomm , int *ierr )
{
}

FORT_NAME( mpi_comm_call_errhandler , MPI_COMM_CALL_ERRHANDLER ) ( MPI_Comm comm, int errorcode , int *ierr )
{
}

FORT_NAME( mpi_comm_compare , MPI_COMM_COMPARE ) ( MPI_Comm comm1, MPI_Comm comm2, int *result , int *ierr )
{
}

FORT_NAME( mpi_comm_connect , MPI_COMM_CONNECT ) ( char *port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *newcomm , int *ierr )
{
}

FORT_NAME( mpi_comm_create_errhandler , MPI_COMM_CREATE_ERRHANDLER ) ( MPI_Comm_errhandler_fn *function, MPI_Errhandler *errhandler , int *ierr )
{
}

FORT_NAME( mpi_comm_create_keyval , MPI_COMM_CREATE_KEYVAL ) ( MPI_Comm_copy_attr_function *comm_copy_attr_fn, MPI_Comm_delete_attr_function *comm_delete_attr_fn, int *comm_keyval, void *extra_state , int *ierr )
{
}

FORT_NAME( mpi_comm_delete_attr , MPI_COMM_DELETE_ATTR ) ( MPI_Comm comm, int comm_keyval , int *ierr )
{
}

FORT_NAME( mpi_comm_disconnect , MPI_COMM_DISCONNECT ) ( MPI_Comm *comm , int *ierr )
{
}

FORT_NAME( mpi_comm_free_keyval , MPI_COMM_FREE_KEYVAL ) ( int *comm_keyval , int *ierr )
{
}

FORT_NAME( mpi_comm_get_attr , MPI_COMM_GET_ATTR ) ( MPI_Comm comm, int comm_keyval, void *attribute_val, int *flag , int *ierr )
{
}

FORT_NAME( mpi_comm_get_errhandler , MPI_COMM_GET_ERRHANDLER ) ( MPI_Comm comm, MPI_Errhandler *errhandler , int *ierr )
{
}

FORT_NAME( mpi_comm_get_name , MPI_COMM_GET_NAME ) ( MPI_Comm comm, char *comm_name, int *resultlen , int *ierr )
{
}

FORT_NAME( mpi_comm_get_parent , MPI_COMM_GET_PARENT ) ( MPI_Comm *parent , int *ierr )
{
}

FORT_NAME( mpi_comm_join , MPI_COMM_JOIN ) ( int fd, MPI_Comm *intercomm , int *ierr )
{
}

FORT_NAME( mpi_comm_remote_group , MPI_COMM_REMOTE_GROUP ) ( MPI_Comm comm, MPI_Group *g , int *ierr )
{
}

FORT_NAME( mpi_comm_remote_size , MPI_COMM_REMOTE_SIZE ) ( MPI_Comm comm, int *size , int *ierr )
{
}

FORT_NAME( mpi_comm_set_attr , MPI_COMM_SET_ATTR ) ( MPI_Comm comm, int comm_keyval, void *attribute_val , int *ierr )
{
}

FORT_NAME( mpi_comm_set_errhandler , MPI_COMM_SET_ERRHANDLER ) ( MPI_Comm comm, MPI_Errhandler errhandler , int *ierr )
{
}

FORT_NAME( mpi_comm_set_name , MPI_COMM_SET_NAME ) ( MPI_Comm comm, char *comm_name , int *ierr )
{
}

FORT_NAME( mpi_comm_spawn , MPI_COMM_SPAWN ) ( char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[] , int *ierr )
{
}

FORT_NAME( mpi_comm_spawn_multiple , MPI_COMM_SPAWN_MULTIPLE ) ( int count, char *array_of_commands[], char* *array_of_argv[], int array_of_maxprocs[], MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[] , int *ierr )
{
}

FORT_NAME( mpi_comm_test_inter , MPI_COMM_TEST_INTER ) ( MPI_Comm comm, int *flag , int *ierr )
{
}

FORT_NAME( mpi_dims_create , MPI_DIMS_CREATE ) ( int nnodes, int ndims, int *dims , int *ierr )
{
}

FORT_NAME( mpi_dist_graph_create_adjacent , MPI_DIST_GRAPH_CREATE_ADJACENT ) ( MPI_Comm comm_old, int indegree, int sources[], int sourceweights[], int outdegree, int destinations[], int destweights[], MPI_Info info, int reorder, MPI_Comm *comm_dist_graph , int *ierr )
{
}

FORT_NAME( mpi_dist_graph_create , MPI_DIST_GRAPH_CREATE ) ( MPI_Comm comm_old, int n, int sources[], int degrees[], int destinations[], int weights[], MPI_Info info, int reorder, MPI_Comm *comm_dist_graph , int *ierr )
{
}

FORT_NAME( mpi_dist_graph_neighbors_count , MPI_DIST_GRAPH_NEIGHBORS_COUNT ) ( MPI_Comm comm, int *indegree, int *outdegree, int *weighted , int *ierr )
{
}

FORT_NAME( mpi_dist_graph_neighbors , MPI_DIST_GRAPH_NEIGHBORS ) ( MPI_Comm comm, int maxindegree, int sources[], int sourceweights[], int maxoutdegree, int destinations[], int destweights[] , int *ierr )
{
}

FORT_NAME( mpi_errhandler_create , MPI_ERRHANDLER_CREATE ) ( MPI_Handler_function *function, MPI_Errhandler *errhandler , int *ierr )
{
}

FORT_NAME( mpi_errhandler_free , MPI_ERRHANDLER_FREE ) ( MPI_Errhandler *errhandler , int *ierr )
{
}

FORT_NAME( mpi_errhandler_get , MPI_ERRHANDLER_GET ) ( MPI_Comm comm, MPI_Errhandler *errhandler , int *ierr )
{
}

FORT_NAME( mpi_errhandler_set , MPI_ERRHANDLER_SET ) ( MPI_Comm comm, MPI_Errhandler errhandler , int *ierr )
{
}

FORT_NAME( mpi_error_class , MPI_ERROR_CLASS ) ( int errorcode, int *errorclass , int *ierr )
{
}


FORT_NAME( mpi_exscan , MPI_EXSCAN ) ( void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm , int *ierr )
{
}

FORT_NAME( mpi_file_call_errhandler , MPI_FILE_CALL_ERRHANDLER ) ( MPI_File fh, int errorcode , int *ierr )
{
}

FORT_NAME( mpi_file_create_errhandler , MPI_FILE_CREATE_ERRHANDLER ) ( MPI_File_errhandler_fn *function, MPI_Errhandler *errhandler , int *ierr )
{
}


FORT_NAME( mpi_file_delete , MPI_FILE_DELETE ) ( char *filename, MPI_Info info , int *ierr )
{
}

FORT_NAME( mpi_file_get_amode , MPI_FILE_GET_AMODE ) ( MPI_File MPI_fh, int *amode , int *ierr )
{
}

FORT_NAME( mpi_file_get_atomicity , MPI_FILE_GET_ATOMICITY ) ( MPI_File MPI_fh, int *flag , int *ierr )
{
}

FORT_NAME( mpi_file_get_byte_offset , MPI_FILE_GET_BYTE_OFFSET ) ( MPI_File MPI_fh, MPI_Offset offset, MPI_Offset *disp , int *ierr )
{
}

FORT_NAME( mpi_file_get_errhandler , MPI_FILE_GET_ERRHANDLER ) ( MPI_File file, MPI_Errhandler *errhandler , int *ierr )
{
}

FORT_NAME( mpi_file_get_group , MPI_FILE_GET_GROUP ) ( MPI_File MPI_fh, MPI_Group *g , int *ierr )
{
}

FORT_NAME( mpi_file_get_info , MPI_FILE_GET_INFO ) ( MPI_File MPI_fh, MPI_Info *info_used , int *ierr )
{
}

FORT_NAME( mpi_file_get_position , MPI_FILE_GET_POSITION ) ( MPI_File MPI_fh, MPI_Offset *offset , int *ierr )
{
}

FORT_NAME( mpi_file_get_position_shared , MPI_FILE_GET_POSITION_SHARED ) ( MPI_File MPI_fh, MPI_Offset *offset , int *ierr )
{
}

FORT_NAME( mpi_file_get_size , MPI_FILE_GET_SIZE ) ( MPI_File MPI_fh, MPI_Offset *size , int *ierr )
{
}

FORT_NAME( mpi_file_get_type_extent , MPI_FILE_GET_TYPE_EXTENT ) ( MPI_File MPI_fh, MPI_Datatype datatype, MPI_Aint *extent , int *ierr )
{
}

FORT_NAME( mpi_file_get_view , MPI_FILE_GET_VIEW ) ( MPI_File MPI_fh, MPI_Offset *disp, MPI_Datatype *etype, MPI_Datatype *filetype, char *datarep , int *ierr )
{
}

FORT_NAME( mpi_file_iread_at , MPI_FILE_IREAD_AT ) ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPIO_Request *r , int *ierr )
{
}

FORT_NAME( mpi_file_iread , MPI_FILE_IREAD ) ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *r , int *ierr )
{
}

FORT_NAME( mpi_file_iread_shared , MPI_FILE_IREAD_SHARED ) ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *r , int *ierr )
{
}

FORT_NAME( mpi_file_iwrite_at , MPI_FILE_IWRITE_AT ) ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPIO_Request *r , int *ierr )
{
}

FORT_NAME( mpi_file_iwrite , MPI_FILE_IWRITE ) ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *r , int *ierr )
{
}

FORT_NAME( mpi_file_iwrite_shared , MPI_FILE_IWRITE_SHARED ) ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPIO_Request *r , int *ierr )
{
}


FORT_NAME( mpi_file_preallocate , MPI_FILE_PREALLOCATE ) ( MPI_File MPI_fh, MPI_Offset size , int *ierr )
{
}

FORT_NAME( mpi_file_read_all_begin , MPI_FILE_READ_ALL_BEGIN ) ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype , int *ierr )
{
}

FORT_NAME( mpi_file_read_all_end , MPI_FILE_READ_ALL_END ) ( MPI_File MPI_fh, void *buf, MPI_Status *status , int *ierr )
{
}


FORT_NAME( mpi_file_read_at_all_begin , MPI_FILE_READ_AT_ALL_BEGIN ) ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype , int *ierr )
{
}

FORT_NAME( mpi_file_read_at_all_end , MPI_FILE_READ_AT_ALL_END ) ( MPI_File MPI_fh, void *buf, MPI_Status *status , int *ierr )
{
}

FORT_NAME( mpi_file_read_at_all , MPI_FILE_READ_AT_ALL ) ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status , int *ierr )
{
}

FORT_NAME( mpi_file_read , MPI_FILE_READ ) ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status , int *ierr )
{
}

FORT_NAME( mpi_file_read_ordered_begin , MPI_FILE_READ_ORDERED_BEGIN ) ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype , int *ierr )
{
}

FORT_NAME( mpi_file_read_ordered_end , MPI_FILE_READ_ORDERED_END ) ( MPI_File MPI_fh, void *buf, MPI_Status *status , int *ierr )
{
}

FORT_NAME( mpi_file_read_ordered , MPI_FILE_READ_ORDERED ) ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status , int *ierr )
{
}

FORT_NAME( mpi_file_read_shared , MPI_FILE_READ_SHARED ) ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status , int *ierr )
{
}

FORT_NAME( mpi_file_seek , MPI_FILE_SEEK ) ( MPI_File MPI_fh, MPI_Offset offset, int whence , int *ierr )
{
}

FORT_NAME( mpi_file_seek_shared , MPI_FILE_SEEK_SHARED ) ( MPI_File MPI_fh, MPI_Offset offset, int whence , int *ierr )
{
}

FORT_NAME( mpi_file_set_atomicity , MPI_FILE_SET_ATOMICITY ) ( MPI_File MPI_fh, int flag , int *ierr )
{
}

FORT_NAME( mpi_file_set_errhandler , MPI_FILE_SET_ERRHANDLER ) ( MPI_File file, MPI_Errhandler errhandler , int *ierr )
{
}

FORT_NAME( mpi_file_set_info , MPI_FILE_SET_INFO ) ( MPI_File MPI_fh, MPI_Info info , int *ierr )
{
}

FORT_NAME( mpi_file_set_size , MPI_FILE_SET_SIZE ) ( MPI_File MPI_fh, MPI_Offset size , int *ierr )
{
}


FORT_NAME( mpi_file_sync , MPI_FILE_SYNC ) ( MPI_File MPI_fh , int *ierr )
{
}

FORT_NAME( mpi_file_write_all_begin , MPI_FILE_WRITE_ALL_BEGIN ) ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype , int *ierr )
{
}

FORT_NAME( mpi_file_write_all_end , MPI_FILE_WRITE_ALL_END ) ( MPI_File MPI_fh, void *buf, MPI_Status *status , int *ierr )
{
}


FORT_NAME( mpi_file_write_at_all_begin , MPI_FILE_WRITE_AT_ALL_BEGIN ) ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype , int *ierr )
{
}

FORT_NAME( mpi_file_write_at_all_end , MPI_FILE_WRITE_AT_ALL_END ) ( MPI_File MPI_fh, void *buf, MPI_Status *status , int *ierr )
{
}

FORT_NAME( mpi_file_write_at_all , MPI_FILE_WRITE_AT_ALL ) ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status , int *ierr )
{
}

FORT_NAME( mpi_file_write , MPI_FILE_WRITE ) ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status , int *ierr )
{
}

FORT_NAME( mpi_file_write_ordered_begin , MPI_FILE_WRITE_ORDERED_BEGIN ) ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype , int *ierr )
{
}

FORT_NAME( mpi_file_write_ordered_end , MPI_FILE_WRITE_ORDERED_END ) ( MPI_File MPI_fh, void *buf, MPI_Status *status , int *ierr )
{
}

FORT_NAME( mpi_file_write_ordered , MPI_FILE_WRITE_ORDERED ) ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status , int *ierr )
{
}

FORT_NAME( mpi_file_write_shared , MPI_FILE_WRITE_SHARED ) ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status , int *ierr )
{
}


FORT_NAME( mpi_free_mem , MPI_FREE_MEM ) ( void *base , int *ierr )
{
}


FORT_NAME( mpi_get_address , MPI_GET_ADDRESS ) ( void *location, MPI_Aint *address , int *ierr )
{
}

FORT_NAME( mpi_get_count , MPI_GET_COUNT ) ( MPI_Status *status, MPI_Datatype datatype, int *count , int *ierr )
{
}

FORT_NAME( mpi_get_elements , MPI_GET_ELEMENTS ) ( MPI_Status *status, MPI_Datatype datatype, int *elements , int *ierr )
{
}

FORT_NAME( mpi_get , MPI_GET ) ( void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win , int *ierr )
{
}


FORT_NAME( mpi_get_version , MPI_GET_VERSION ) ( int *version, int *subversion , int *ierr )
{
}

FORT_NAME( mpi_graph_create , MPI_GRAPH_CREATE ) ( MPI_Comm comm_old, int nnodes, int *indx, int *edges, int reorder, MPI_Comm *comm_graph , int *ierr )
{
}

FORT_NAME( mpi_graphdims_get , MPI_GRAPHDIMS_GET ) ( MPI_Comm comm, int *nnodes, int *nedges , int *ierr )
{
}

FORT_NAME( mpi_graph_get , MPI_GRAPH_GET ) ( MPI_Comm comm, int maxindex, int maxedges, int *indx, int *edges , int *ierr )
{
}

FORT_NAME( mpi_graph_map , MPI_GRAPH_MAP ) ( MPI_Comm comm_old, int nnodes, int *indx, int *edges, int *newrank , int *ierr )
{
}

FORT_NAME( mpi_graph_neighbors_count , MPI_GRAPH_NEIGHBORS_COUNT ) ( MPI_Comm comm, int rank, int *nneighbors , int *ierr )
{
}

FORT_NAME( mpi_graph_neighbors , MPI_GRAPH_NEIGHBORS ) ( MPI_Comm comm, int rank, int maxneighbors, int *neighbors , int *ierr )
{
}

FORT_NAME( mpi_grequest_complete , MPI_GREQUEST_COMPLETE ) ( MPI_Request r , int *ierr )
{
}

FORT_NAME( mpi_grequest_start , MPI_GREQUEST_START ) ( MPI_Grequest_query_function *query_fn, MPI_Grequest_free_function *free_fn, MPI_Grequest_cancel_function *cancel_fn, void *extra_state, MPI_Request *r , int *ierr )
{
}

FORT_NAME( mpi_group_compare , MPI_GROUP_COMPARE ) ( MPI_Group group1, MPI_Group group2, int *result , int *ierr )
{
}

FORT_NAME( mpi_group_difference , MPI_GROUP_DIFFERENCE ) ( MPI_Group group1, MPI_Group group2, MPI_Group *newgroup , int *ierr )
{
}

FORT_NAME( mpi_group_free , MPI_GROUP_FREE ) ( MPI_Group *g , int *ierr )
{
}



FORT_NAME( mpi_group_intersection , MPI_GROUP_INTERSECTION ) ( MPI_Group group1, MPI_Group group2, MPI_Group *newgroup , int *ierr )
{
}


FORT_NAME( mpi_group_excl , MPI_GROUP_EXCL ) ( MPI_Group g, int n, int *ranks, MPI_Group *newgroup , int *ierr )
{
}



FORT_NAME( mpi_group_rank , MPI_GROUP_RANK ) ( MPI_Group g, int *rank , int *ierr )
{
}

FORT_NAME( mpi_group_size , MPI_GROUP_SIZE ) ( MPI_Group g, int *size , int *ierr )
{
}



FORT_NAME( mpi_info_dup , MPI_INFO_DUP ) ( MPI_Info info, MPI_Info *newinfo , int *ierr )
{
}


FORT_NAME( mpi_info_get , MPI_INFO_GET ) ( MPI_Info info, char *key, int valuelen, char *value, int *flag , int *ierr )
{
}

FORT_NAME( mpi_info_get_nkeys , MPI_INFO_GET_NKEYS ) ( MPI_Info info, int *nkeys , int *ierr )
{
}

FORT_NAME( mpi_info_get_nthkey , MPI_INFO_GET_NTHKEY ) ( MPI_Info info, int n, char *key , int *ierr )
{
}

FORT_NAME( mpi_info_get_valuelen , MPI_INFO_GET_VALUELEN ) ( MPI_Info info, char *key, int *valuelen, int *flag , int *ierr )
{
}


FORT_NAME( mpi_init_thread , MPI_INIT_THREAD ) ( int *argc, char ***argv, int required, int *provided , int *ierr )
{
}

FORT_NAME( mpi_iprobe , MPI_IPROBE ) ( int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status , int *ierr )
{
}


FORT_NAME( mpi_issend , MPI_ISSEND ) ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r , int *ierr )
{
}

FORT_NAME( mpi_is_thread_main , MPI_IS_THREAD_MAIN ) ( int *flag , int *ierr )
{
}

FORT_NAME( mpi_keyval_create , MPI_KEYVAL_CREATE ) ( MPI_Copy_function *copy_fn, MPI_Delete_function *delete_fn, int *keyval, void *extra_state , int *ierr )
{
}

FORT_NAME( mpi_keyval_free , MPI_KEYVAL_FREE ) ( int *keyval , int *ierr )
{
}

FORT_NAME( mpi_lookup_name , MPI_LOOKUP_NAME ) ( char *service_name, MPI_Info info, char *port_name , int *ierr )
{
}

FORT_NAME( mpi_op_commutative , MPI_OP_COMMUTATIVE ) ( MPI_Op op, int *commute , int *ierr )
{
}

FORT_NAME( mpi_op_create , MPI_OP_CREATE ) ( MPI_User_function *function, int commute, MPI_Op *op , int *ierr )
{
}

FORT_NAME( mpi_open_port , MPI_OPEN_PORT ) ( MPI_Info info, char *port_name , int *ierr )
{
}

FORT_NAME( mpi_op_free , MPI_OP_FREE ) ( MPI_Op *op , int *ierr )
{
}

FORT_NAME( mpi_pack_external , MPI_PACK_EXTERNAL ) ( char *datarep, void *inbuf, int incount, MPI_Datatype datatype, void *outbuf, MPI_Aint outcount, MPI_Aint *position , int *ierr )
{
}

FORT_NAME( mpi_pack_external_size , MPI_PACK_EXTERNAL_SIZE ) ( char *datarep, int incount, MPI_Datatype datatype, MPI_Aint *size , int *ierr )
{
}

FORT_NAME( mpi_pack , MPI_PACK ) ( void *inbuf, int incount, MPI_Datatype datatype, void *outbuf, int outcount, int *position, MPI_Comm comm , int *ierr )
{
}

FORT_NAME( mpi_pack_size , MPI_PACK_SIZE ) ( int incount, MPI_Datatype datatype, MPI_Comm comm, int *size , int *ierr )
{
}

FORT_NAME( mpi_probe , MPI_PROBE ) ( int source, int tag, MPI_Comm comm, MPI_Status *status , int *ierr )
{
}

FORT_NAME( mpi_publish_name , MPI_PUBLISH_NAME ) ( char *service_name, MPI_Info info, char *port_name , int *ierr )
{
}

FORT_NAME( mpi_put , MPI_PUT ) ( void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win , int *ierr )
{
}

FORT_NAME( mpi_query_thread , MPI_QUERY_THREAD ) ( int *provided , int *ierr )
{
}

FORT_NAME( mpi_recv_init , MPI_RECV_INIT ) ( void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *r , int *ierr )
{
}


FORT_NAME( mpi_reduce_local , MPI_REDUCE_LOCAL ) ( void *inbuf, void *inoutbuf, int count, MPI_Datatype datatype, MPI_Op op , int *ierr )
{
}

FORT_NAME( mpi_reduce_scatter_block , MPI_REDUCE_SCATTER_BLOCK ) ( void *sendbuf, void *recvbuf, int recvcount, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm , int *ierr )
{
}

FORT_NAME( mpi_reduce_scatter , MPI_REDUCE_SCATTER ) ( void *sendbuf, void *recvbuf, int *recvcnts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm , int *ierr )
{
}

FORT_NAME( mpi_register_datarep , MPI_REGISTER_DATAREP ) ( char *name, MPI_Datarep_conversion_function *read_conv_fn, MPI_Datarep_conversion_function *write_conv_fn, MPI_Datarep_extent_function *extent_fn, void *state , int *ierr )
{
}


FORT_NAME( mpi_request_get_status , MPI_REQUEST_GET_STATUS ) ( MPI_Request r, int *flag, MPI_Status *status , int *ierr )
{
}

FORT_NAME( mpi_rsend_init , MPI_RSEND_INIT ) ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r , int *ierr )
{
}

FORT_NAME( mpi_scan , MPI_SCAN ) ( void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm , int *ierr )
{
}

FORT_NAME( mpi_send_init , MPI_SEND_INIT ) ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r , int *ierr )
{
}



FORT_NAME( mpi_sendrecv_replace , MPI_SENDRECV_REPLACE ) ( void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status , int *ierr )
{
}

FORT_NAME( mpi_ssend_init , MPI_SSEND_INIT ) ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r , int *ierr )
{
}

FORT_NAME( mpi_startall , MPI_STARTALL ) ( int count, MPI_Request array_of_requests[] , int *ierr )
{
}

FORT_NAME( mpi_start , MPI_START ) ( MPI_Request *r , int *ierr )
{
}

FORT_NAME( mpi_status_set_cancelled , MPI_STATUS_SET_CANCELLED ) ( MPI_Status *status, int flag , int *ierr )
{
}

FORT_NAME( mpi_status_set_elements , MPI_STATUS_SET_ELEMENTS ) ( MPI_Status *status, MPI_Datatype datatype, int count , int *ierr )
{
}

FORT_NAME( mpi_testall , MPI_TESTALL ) ( int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[] , int *ierr )
{
}

FORT_NAME( mpi_testany , MPI_TESTANY ) ( int count, MPI_Request array_of_requests[], int *index, int *flag, MPI_Status *status , int *ierr )
{
}

FORT_NAME( mpi_test_cancelled , MPI_TEST_CANCELLED ) ( MPI_Status *status, int *flag , int *ierr )
{
}

FORT_NAME( mpi_test , MPI_TEST ) ( MPI_Request *r, int *flag, MPI_Status *status , int *ierr )
{
}

FORT_NAME( mpi_testsome , MPI_TESTSOME ) ( int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[] , int *ierr )
{
}

FORT_NAME( mpi_topo_test , MPI_TOPO_TEST ) ( MPI_Comm comm, int *topo_type , int *ierr )
{
}

FORT_NAME( mpi_type_create_darray , MPI_TYPE_CREATE_DARRAY ) ( int size, int rank, int ndims, int array_of_gsizes[], int array_of_distribs[], int array_of_dargs[], int array_of_psizes[], int order, MPI_Datatype oldtype, MPI_Datatype *newtype , int *ierr )
{
}

FORT_NAME( mpi_type_create_hindexed , MPI_TYPE_CREATE_HINDEXED ) ( int count, int blocklengths[], MPI_Aint displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype , int *ierr )
{
}

FORT_NAME( mpi_type_create_hvector , MPI_TYPE_CREATE_HVECTOR ) ( int count, int blocklength, MPI_Aint stride, MPI_Datatype oldtype, MPI_Datatype *newtype , int *ierr )
{
}

FORT_NAME( mpi_type_create_keyval , MPI_TYPE_CREATE_KEYVAL ) ( MPI_Type_copy_attr_function *type_copy_attr_fn, MPI_Type_delete_attr_function *type_delete_attr_fn, int *type_keyval, void *extra_state , int *ierr )
{
}

FORT_NAME( mpi_type_create_resized , MPI_TYPE_CREATE_RESIZED ) ( MPI_Datatype oldtype, MPI_Aint lb, MPI_Aint extent, MPI_Datatype *newtype , int *ierr )
{
}

FORT_NAME( mpi_type_create_struct , MPI_TYPE_CREATE_STRUCT ) ( int count, int array_of_blocklengths[], MPI_Aint array_of_displacements[], MPI_Datatype array_of_types[], MPI_Datatype *newtype , int *ierr )
{
}

FORT_NAME( mpi_type_create_subarray , MPI_TYPE_CREATE_SUBARRAY ) ( int ndims, int array_of_sizes[], int array_of_subsizes[], int array_of_starts[], int order, MPI_Datatype oldtype, MPI_Datatype *newtype , int *ierr )
{
}

FORT_NAME( mpi_type_delete_attr , MPI_TYPE_DELETE_ATTR ) ( MPI_Datatype type, int type_keyval , int *ierr )
{
}

FORT_NAME( mpi_type_dup , MPI_TYPE_DUP ) ( MPI_Datatype datatype, MPI_Datatype *newtype , int *ierr )
{
}

FORT_NAME( mpi_type_extent , MPI_TYPE_EXTENT ) ( MPI_Datatype datatype, MPI_Aint *extent , int *ierr )
{
}

FORT_NAME( mpi_type_free_keyval , MPI_TYPE_FREE_KEYVAL ) ( int *type_keyval , int *ierr )
{
}

FORT_NAME( mpi_type_get_attr , MPI_TYPE_GET_ATTR ) ( MPI_Datatype type, int type_keyval, void *attribute_val, int *flag , int *ierr )
{
}

FORT_NAME( mpi_type_get_contents , MPI_TYPE_GET_CONTENTS ) ( MPI_Datatype datatype, int max_integers, int max_addresses, int max_datatypes, int array_of_integers[], MPI_Aint array_of_addresses[], MPI_Datatype array_of_datatypes[] , int *ierr )
{
}


FORT_NAME( mpi_type_get_extent , MPI_TYPE_GET_EXTENT ) ( MPI_Datatype datatype, MPI_Aint *lb, MPI_Aint *extent , int *ierr )
{
}

FORT_NAME( mpi_type_get_name , MPI_TYPE_GET_NAME ) ( MPI_Datatype datatype, char *type_name, int *resultlen , int *ierr )
{
}

FORT_NAME( mpi_type_get_true_extent , MPI_TYPE_GET_TRUE_EXTENT ) ( MPI_Datatype datatype, MPI_Aint *true_lb, MPI_Aint *true_extent , int *ierr )
{
}

FORT_NAME( mpi_type_hindexed , MPI_TYPE_HINDEXED ) ( int count, int blocklens[], MPI_Aint indices[], MPI_Datatype old_type, MPI_Datatype *newtype , int *ierr )
{
}

FORT_NAME( mpi_type_hvector , MPI_TYPE_HVECTOR ) ( int count, int blocklen, MPI_Aint stride, MPI_Datatype old_type, MPI_Datatype *newtype_p , int *ierr )
{
}

FORT_NAME( mpi_type_indexed , MPI_TYPE_INDEXED ) ( int count, int blocklens[], int indices[], MPI_Datatype old_type, MPI_Datatype *newtype , int *ierr )
{
}

FORT_NAME( mpi_type_lb , MPI_TYPE_LB ) ( MPI_Datatype datatype, MPI_Aint *displacement , int *ierr )
{
}

FORT_NAME( mpi_type_match_size , MPI_TYPE_MATCH_SIZE ) ( int typeclass, int size, MPI_Datatype *datatype , int *ierr )
{
}

FORT_NAME( mpi_type_set_attr , MPI_TYPE_SET_ATTR ) ( MPI_Datatype type, int type_keyval, void *attribute_val , int *ierr )
{
}

FORT_NAME( mpi_type_set_name , MPI_TYPE_SET_NAME ) ( MPI_Datatype type, char *type_name , int *ierr )
{
}

FORT_NAME( mpi_type_size , MPI_TYPE_SIZE ) ( MPI_Datatype datatype, int *size , int *ierr )
{
}

FORT_NAME( mpi_type_struct , MPI_TYPE_STRUCT ) ( int count, int blocklens[], MPI_Aint indices[], MPI_Datatype old_types[], MPI_Datatype *newtype , int *ierr )
{
}

FORT_NAME( mpi_type_ub , MPI_TYPE_UB ) ( MPI_Datatype datatype, MPI_Aint *displacement , int *ierr )
{
}

FORT_NAME( mpi_type_vector , MPI_TYPE_VECTOR ) ( int count, int blocklength, int stride, MPI_Datatype old_type, MPI_Datatype *newtype_p , int *ierr )
{
}

FORT_NAME( mpi_unpack_external , MPI_UNPACK_EXTERNAL ) ( char *datarep, void *inbuf, MPI_Aint insize, MPI_Aint *position, void *outbuf, int outcount, MPI_Datatype datatype , int *ierr )
{
}

FORT_NAME( mpi_unpack , MPI_UNPACK ) ( void *inbuf, int insize, int *position, void *outbuf, int outcount, MPI_Datatype datatype, MPI_Comm comm , int *ierr )
{
}

FORT_NAME( mpi_unpublish_name , MPI_UNPUBLISH_NAME ) ( char *service_name, MPI_Info info, char *port_name , int *ierr )
{
}



FORT_NAME( mpi_waitsome , MPI_WAITSOME ) ( int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[] , int *ierr )
{
}

FORT_NAME( mpi_win_call_errhandler , MPI_WIN_CALL_ERRHANDLER ) ( MPI_Win win, int errorcode , int *ierr )
{
}

FORT_NAME( mpi_win_complete , MPI_WIN_COMPLETE ) ( MPI_Win win , int *ierr )
{
}

FORT_NAME( mpi_win_create_errhandler , MPI_WIN_CREATE_ERRHANDLER ) ( MPI_Win_errhandler_fn *function, MPI_Errhandler *errhandler , int *ierr )
{
}

FORT_NAME( mpi_win_create , MPI_WIN_CREATE ) ( void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win , int *ierr )
{
}

FORT_NAME( mpi_win_create_keyval , MPI_WIN_CREATE_KEYVAL ) ( MPI_Win_copy_attr_function *win_copy_attr_fn, MPI_Win_delete_attr_function *win_delete_attr_fn, int *win_keyval, void *extra_state , int *ierr )
{
}

FORT_NAME( mpi_win_delete_attr , MPI_WIN_DELETE_ATTR ) ( MPI_Win win, int win_keyval , int *ierr )
{
}

FORT_NAME( mpi_win_fence , MPI_WIN_FENCE ) ( int assert, MPI_Win win , int *ierr )
{
}

FORT_NAME( mpi_win_free , MPI_WIN_FREE ) ( MPI_Win *win , int *ierr )
{
}

FORT_NAME( mpi_win_free_keyval , MPI_WIN_FREE_KEYVAL ) ( int *win_keyval , int *ierr )
{
}

FORT_NAME( mpi_win_get_attr , MPI_WIN_GET_ATTR ) ( MPI_Win win, int win_keyval, void *attribute_val, int *flag , int *ierr )
{
}

FORT_NAME( mpi_win_get_errhandler , MPI_WIN_GET_ERRHANDLER ) ( MPI_Win win, MPI_Errhandler *errhandler , int *ierr )
{
}

FORT_NAME( mpi_win_get_group , MPI_WIN_GET_GROUP ) ( MPI_Win win, MPI_Group *g , int *ierr )
{
}

FORT_NAME( mpi_win_get_name , MPI_WIN_GET_NAME ) ( MPI_Win win, char *win_name, int *resultlen , int *ierr )
{
}

FORT_NAME( mpi_win_lock , MPI_WIN_LOCK ) ( int lock_type, int rank, int assert, MPI_Win win , int *ierr )
{
}

FORT_NAME( mpi_win_post , MPI_WIN_POST ) ( MPI_Group g, int assert, MPI_Win win , int *ierr )
{
}

FORT_NAME( mpi_win_set_attr , MPI_WIN_SET_ATTR ) ( MPI_Win win, int win_keyval, void *attribute_val , int *ierr )
{
}

FORT_NAME( mpi_win_set_errhandler , MPI_WIN_SET_ERRHANDLER ) ( MPI_Win win, MPI_Errhandler errhandler , int *ierr )
{
}

FORT_NAME( mpi_win_set_name , MPI_WIN_SET_NAME ) ( MPI_Win win, char *win_name , int *ierr )
{
}

FORT_NAME( mpi_win_start , MPI_WIN_START ) ( MPI_Group g, int assert, MPI_Win win , int *ierr )
{
}

FORT_NAME( mpi_win_test , MPI_WIN_TEST ) ( MPI_Win win, int *flag , int *ierr )
{
}

FORT_NAME( mpi_win_unlock , MPI_WIN_UNLOCK ) ( int rank, MPI_Win win , int *ierr )
{
}

FORT_NAME( mpi_win_wait , MPI_WIN_WAIT ) ( MPI_Win win , int *ierr )
{
}

FORT_NAME( mpi_group_comm_create , MPI_GROUP_COMM_CREATE ) ( MPI_Comm old_comm, MPI_Group g, int tag, MPI_Comm *new_comm , int *ierr )
{
}

FORT_NAME( mpi_comm_c2f , MPI_COMM_C2F ) ( MPI_Comm comm , int *ierr )
{
}

FORT_NAME( mpi_group_c2f , MPI_GROUP_C2F ) ( MPI_Group g , int *ierr )
{
}

FORT_NAME( mpi_request_c2f , MPI_REQUEST_C2F ) ( MPI_Request r , int *ierr )
{
}

FORT_NAME( mpi_info_c2f , MPI_INFO_C2F ) ( MPI_Info info , int *ierr )
{
}

FORT_NAME( mpi_file_c2f , MPI_FILE_C2F ) ( MPI_File file , int *ierr )
{
}

FORT_NAME( mpi_op_c2f , MPI_OP_C2F ) ( MPI_Op op , int *ierr )
{
}

FORT_NAME( mpi_win_c2f , MPI_WIN_C2F ) ( MPI_Win Win , int *ierr )
{
}

FORT_NAME( mpi_errhandler_c2f , MPI_ERRHANDLER_C2F ) ( MPI_Errhandler Errhandler , int *ierr )
{
}

FORT_NAME( mpi_type_c2f , MPI_TYPE_C2F ) ( MPI_Datatype Type , int *ierr )
{
}

*/
