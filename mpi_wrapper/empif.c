#include "empif_names.h"
#include "empi.h"

/* Misc. utilities */

void FORT_NAME( empi_init , EMPI_INIT ) ( int *ierr )
{
	// TODO!!!
}

void FORT_NAME( empi_initialized , EMPI_INITIALIZED ) ( int *flag , int *ierr )
{
   *ierr = EMPI_Initialized(flag);
}

void FORT_NAME( empi_abort , EMPI_ABORT ) ( int *comm, int *errorcode , int *ierr )
{
   *ierr = EMPI_Abort(*comm, *errorcode);
}

void FORT_NAME( empi_finalize , EMPI_FINALIZE ) ( int *ierr )
{
   *ierr = EMPI_Finalize();
}

void FORT_NAME( empi_finalized , EMPI_FINALIZED ) ( int *flag , int *ierr )
{
   *ierr = EMPI_Finalized(flag);
}

double FORT_NAME( empi_wtime , EMPI_WTIME ) ( void )
{
   return EMPI_Wtime();
}

void FORT_NAME( empi_get_processor_name , EMPI_GET_PROCESSOR_NAME ) ( char *name, int *resultlen , int *ierr )
{
   // FIXME: does this work ?
   *ierr = EMPI_Get_processor_name(name, resultlen);
}

void FORT_NAME( empi_error_string , EMPI_ERROR_STRING ) ( int *errorcode, char *string, int *resultlen , int *ierr )
{
   // FIXME: does this work ?
   *ierr = EMPI_Error_string(*errorcode, string, resultlen);
}



/* Communicators and groups */

void FORT_NAME( empi_comm_create , EMPI_COMM_CREATE ) ( int *comm, int *g, int *newcomm , int *ierr )
{
   *ierr = EMPI_Comm_create(*comm, *g, newcomm);
}


void FORT_NAME( empi_comm_dup , EMPI_COMM_DUP ) ( int *comm, int *newcomm , int *ierr )
{
   *ierr = EMPI_Comm_dup(*comm, newcomm);
}

void FORT_NAME( empi_comm_free , EMPI_COMM_FREE ) ( int *comm , int *ierr )
{
   *ierr = EMPI_Comm_free(comm);
}

void FORT_NAME( empi_comm_group , EMPI_COMM_GROUP ) ( int *comm, int *g , int *ierr )
{
   *ierr = EMPI_Comm_group(*comm, g);
}


void FORT_NAME( empi_comm_rank , EMPI_COMM_RANK ) ( int *comm, int *rank , int *ierr )
{
   *ierr = EMPI_Comm_rank(*comm, rank);
}

void FORT_NAME( empi_comm_size , EMPI_COMM_SIZE ) ( int *comm, int *size , int *ierr )
{
   *ierr = EMPI_Comm_size(*comm, size);
}

void FORT_NAME( empi_comm_split , EMPI_COMM_SPLIT ) ( int *comm, int *color, int *key, int *newcomm , int *ierr )
{
   *ierr = EMPI_Comm_split(*comm, *color, *key, newcomm);
}

void FORT_NAME( empi_group_range_incl , EMPI_GROUP_RANGE_INCL ) ( int *g, int *n, int ranges[][3], int *newgroup , int *ierr )
{
   *ierr = EMPI_Group_range_incl(*g, *n, ranges, newgroup);
}

void FORT_NAME( empi_group_incl , EMPI_GROUP_INCL ) ( int *g, int *n, int *ranks, int *newgroup , int *ierr )
{
   *ierr = EMPI_Group_incl(*g, *n, ranks, newgroup);
}

void FORT_NAME( empi_group_range_excl , EMPI_GROUP_RANGE_EXCL ) ( int *g, int *n, int ranges[][3], int *newgroup , int *ierr )
{
   *ierr = EMPI_Group_range_excl(*g, *n, ranges, newgroup);
}

void FORT_NAME( empi_group_union , EMPI_GROUP_UNION ) ( int *group1, int *group2, int *newgroup , int *ierr )
{
   *ierr = EMPI_Group_union(*group1, *group2, newgroup);
}

void FORT_NAME( empi_group_translate_ranks , EMPI_GROUP_TRANSLATE_RANKS ) ( int *group1, int *n, int *ranks1, int *group2, int *ranks2 , int *ierr )
{
   *ierr = EMPI_Group_translate_ranks(*group1, *n, ranks1, *group2, ranks2);
}


/* Collectives */

void FORT_NAME( empi_allgather , EMPI_ALLGATHER ) ( void *sendbuf, int *sendcount, int *sendtype, void *recvbuf, int *recvcount, int *recvtype, int *comm , int *ierr )
{
   *ierr = EMPI_Allgather(sendbuf, *sendcount, *sendtype, recvbuf, *recvcount, *recvtype, *comm);
}

void FORT_NAME( empi_allgatherv , EMPI_ALLGATHERV ) ( void *sendbuf, int *sendcount, int *sendtype, void *recvbuf, int *recvcounts, int *displs, int *recvtype, int *comm , int *ierr )
{
   *ierr = EMPI_Allgatherv(sendbuf, *sendcount, *sendtype, recvbuf, recvcounts, displs, *recvtype, *comm);
}

void FORT_NAME( empi_allreduce , EMPI_ALLREDUCE ) ( void *sendbuf, void *recvbuf, int *count, int *type, int *op, int *comm , int *ierr )
{
   *ierr = EMPI_Allreduce(sendbuf, recvbuf, *count, *type, *op, *comm);
}

void FORT_NAME( empi_alltoall , EMPI_ALLTOALL ) ( void *sendbuf, int *sendcount, int *sendtype, void *recvbuf, int *recvcount, int *recvtype, int *comm , int *ierr )
{
   *ierr = EMPI_Alltoall(sendbuf, *sendcount, *sendtype, recvbuf, *recvcount, *recvtype, *comm);
}

void FORT_NAME( empi_alltoallv , EMPI_ALLTOALLV ) ( void *sendbuf, int *sendcnts, int *sdispls, int *sendtype, void *recvbuf, int *recvcnts, int *rdispls, int *recvtype, int *comm , int *ierr )
{
   *ierr = EMPI_Alltoallv(sendbuf, sendcnts, sdispls, *sendtype, recvbuf, recvcnts, rdispls, *recvtype, *comm);
}

void FORT_NAME( empi_alltoallw , EMPI_ALLTOALLW ) ( void *sendbuf, int *sendcnts, int *sdispls, int *sendtypes, void *recvbuf, int *recvcnts, int *rdispls, int *recvtypes, int *comm , int *ierr )
{
   *ierr = EMPI_Alltoallw(sendbuf, sendcnts, sdispls, sendtypes, recvbuf, recvcnts, rdispls, recvtypes, *comm);
}

void FORT_NAME( empi_scatter , EMPI_SCATTER ) ( void *sendbuf, int *sendcnt, int *sendtype, void *recvbuf, int *recvcnt, int *recvtype, int *root, int *comm , int *ierr )
{
   *ierr = EMPI_Scatter(sendbuf, *sendcnt, *sendtype, recvbuf, *recvcnt, *recvtype, *root, *comm);
}

void FORT_NAME( empi_scatterv , EMPI_SCATTERV ) ( void *sendbuf, int *sendcnts, int *displs, int *sendtype, void *recvbuf, int *recvcnt, int *recvtype, int *root, int *comm , int *ierr )
{
   *ierr = EMPI_Scatterv(sendbuf, sendcnts, displs, *sendtype, recvbuf, *recvcnt, *recvtype, *root, *comm);
}

void FORT_NAME( empi_barrier , EMPI_BARRIER ) ( int *comm , int *ierr )
{
   *ierr = EMPI_Barrier(*comm);
}

void FORT_NAME( empi_bcast , EMPI_BCAST ) ( void *buffer, int *count, int *type, int *root, int *comm , int *ierr )
{
   *ierr = EMPI_Bcast(buffer, *count, *type, *root, *comm);
}

void FORT_NAME( empi_gather , EMPI_GATHER ) ( void *sendbuf, int *sendcnt, int *sendtype, void *recvbuf, int *recvcnt, int *recvtype, int *root, int *comm , int *ierr )
{
   *ierr = EMPI_Gather(sendbuf, *sendcnt, *sendtype, recvbuf, *recvcnt, *recvtype, *root, *comm);
}

void FORT_NAME( empi_gatherv , EMPI_GATHERV ) ( void *sendbuf, int *sendcnt, int *sendtype, void *recvbuf, int *recvcnts, int *displs, int *recvtype, int *root, int *comm , int *ierr )
{
   *ierr = EMPI_Gatherv(sendbuf, *sendcnt, *sendtype, recvbuf, recvcnts, displs, *recvtype, *root, *comm);
}

void FORT_NAME( empi_reduce , EMPI_REDUCE ) ( void *sendbuf, void *recvbuf, int *count, int *type, int *op, int *root, int *comm , int *ierr )
{
   *ierr = EMPI_Reduce(sendbuf, recvbuf, *count, *type, *op, *root, *comm);
}


/* Send / receive */

//void FORT_NAME( empi_ibsend , EMPI_IBSEND ) ( void *buf, int *count, int *type, int *dest, int *tag, int *comm, int *r , int *ierr )
//{
//   *ierr = EMPI_Ibsend(buf, *count, *type, *dest, *tag, *comm, r);
//}

void FORT_NAME( empi_irecv , EMPI_IRECV ) ( void *buf, int *count, int *type, int *source, int *tag, int *comm, int *r , int *ierr )
{
   *ierr = EMPI_Irecv(buf, *count, *type, *source, *tag, *comm, r);
}

void FORT_NAME( empi_irsend , EMPI_IRSEND ) ( void *buf, int *count, int *type, int *dest, int *tag, int *comm, int *r , int *ierr )
{
   *ierr = EMPI_Irsend(buf, *count, *type, *dest, *tag, *comm, r);
}

void FORT_NAME( empi_isend , EMPI_ISEND ) ( void *buf, int *count, int *type, int *dest, int *tag, int *comm, int *r , int *ierr )
{
   *ierr = EMPI_Isend(buf, *count, *type, *dest, *tag, *comm, r);
}

void FORT_NAME( empi_rsend , EMPI_RSEND ) ( void *buf, int *count, int *type, int *dest, int *tag, int *comm , int *ierr )
{
   *ierr = EMPI_Rsend(buf, *count, *type, *dest, *tag, *comm);
}

void FORT_NAME( empi_send , EMPI_SEND ) ( void *buf, int *count, int *type, int *dest, int *tag, int *comm , int *ierr )
{
   *ierr = EMPI_Send(buf, *count, *type, *dest, *tag, *comm);
}

void FORT_NAME( empi_sendrecv , EMPI_SENDRECV ) ( void *sendbuf, int *sendcount, int *sendtype, int *dest, int *sendtag, void *recvbuf, int *recvcount, int *recvtype, int *source, int *recvtag, int *comm, int *stat , int *ierr )
{
   *ierr = EMPI_Sendrecv(sendbuf, *sendcount, *sendtype, *dest, *sendtag, recvbuf, *recvcount, *recvtype, *source, *recvtag, *comm, (EMPI_Status *) stat);
}

void FORT_NAME( empi_ssend , EMPI_SSEND ) ( void *buf, int *count, int *type, int *dest, int *tag, int *comm , int *ierr )
{
   *ierr = EMPI_Ssend(buf, *count, *type, *dest, *tag, *comm);
}

void FORT_NAME( empi_recv , EMPI_RECV ) ( void *buf, int *count, int *type, int *source, int *tag, int *comm, int *stat, int *ierr )
{
   *ierr = EMPI_Recv(buf, *count, *type, *source, *tag, *comm, (EMPI_Status *)stat);
}


/* Request / status */

void FORT_NAME( empi_wait , EMPI_WAIT ) ( int *r, int *stat , int *ierr )
{
   *ierr = EMPI_Wait(r, (EMPI_Status *)stat);
}

void FORT_NAME( empi_waitall , EMPI_WAITALL ) ( int *count, int *array_of_requests, int *array_of_statuses , int *ierr )
{
   *ierr = EMPI_Waitall(*count, array_of_requests, (EMPI_Status *)array_of_statuses);
}

void FORT_NAME( empi_waitany , EMPI_WAITANY ) ( int *count, int *array_of_requests, int *index, EMPI_Status *stat , int *ierr )
{
   *ierr = EMPI_Waitany(*count, array_of_requests, index, (EMPI_Status *)stat);
}

void FORT_NAME( empi_request_free , EMPI_REQUEST_FREE ) ( int *r , int *ierr )
{
   *ierr = EMPI_Request_free(r);
}



/* I/O */


void FORT_NAME( empi_file_open , EMPI_FILE_OPEN ) ( int *comm, char *filename, int *amode, int *info, int *fh , int *ierr )
{
   *ierr = EMPI_File_open(*comm, filename, *amode, *info, fh);
}


void FORT_NAME( empi_file_close , EMPI_FILE_CLOSE ) ( int *fh , int *ierr )
{
   *ierr = EMPI_File_close(fh);
}

void FORT_NAME( empi_file_read_all , EMPI_FILE_READ_ALL ) ( int *fh, void *buf, int *count, int *type, int *stat , int *ierr )
{
   *ierr = EMPI_File_read_all(*fh, buf, *count, *type, (EMPI_Status *)stat);
}

void FORT_NAME( empi_file_read_at , EMPI_FILE_READ_AT ) ( int *fh, int *offset, void *buf, int *count, int *type, int *stat , int *ierr )
{
   *ierr = EMPI_File_read_at(*fh, *offset, buf, *count, *type, (EMPI_Status *)stat);
}

void FORT_NAME( empi_file_write_at , EMPI_FILE_WRITE_AT ) ( int *fh, int *offset, void *buf, int *count, int *type, int *stat , int *ierr )
{
   *ierr = EMPI_File_write_at(*fh, *offset, buf, *count, *type, (EMPI_Status *) stat);
}

void FORT_NAME( empi_file_set_view , EMPI_FILE_SET_VIEW ) ( int *fh, int *disp, int *etype, int *filetype, char *datarep, int *info , int *ierr )
{
   *ierr = EMPI_File_set_view(*fh, *disp, *etype, *filetype, datarep, *info);
}

void FORT_NAME( empi_file_write_all , EMPI_FILE_WRITE_ALL ) ( int *fh, void *buf, int *count, int *type, int *stat , int *ierr )
{
   *ierr = EMPI_File_write_all(*fh, buf, *count, *type, (EMPI_Status *) stat);
}

/* Info */

void FORT_NAME( empi_info_set , EMPI_INFO_SET ) ( int *info, char *key, char *value , int *ierr )
{
   *ierr = EMPI_Info_set(*info, key, value);
}

void FORT_NAME( empi_info_create , EMPI_INFO_CREATE ) ( int *info , int *ierr )
{
   *ierr = EMPI_Info_create(info);
}


void FORT_NAME( empi_info_delete , EMPI_INFO_DELETE ) ( int *info, char *key , int *ierr )
{
   *ierr = EMPI_Info_delete(*info, key);
}

void FORT_NAME( empi_info_free , EMPI_INFO_FREE ) ( int *info , int *ierr )
{
   *ierr = EMPI_Info_free(info);
}


/* Datatypes */

void FORT_NAME( empi_type_free , EMPI_TYPE_FREE ) ( int *type , int *ierr )
{
   *ierr = EMPI_Type_free(type);
}

void FORT_NAME( empi_type_commit , EMPI_TYPE_COMMIT ) ( int *type , int *ierr )
{
   *ierr = EMPI_Type_commit(type);
}

void FORT_NAME( empi_type_contiguous , EMPI_TYPE_CONTIGUOUS ) ( int *count, int *old_type, int *new_type , int *ierr )
{
   *ierr = EMPI_Type_contiguous(*count, *old_type, new_type);
}

void FORT_NAME( empi_type_create_indexed_block , EMPI_TYPE_CREATE_INDEXED_BLOCK ) ( int *count, int *blocklength, int *array_of_displacements, int *oldtype, int *newtype , int *ierr )
{
   *ierr = EMPI_Type_create_indexed_block(*count, *blocklength, array_of_displacements, *oldtype, newtype);
}

void FORT_NAME( empi_type_get_envelope , EMPI_TYPE_GET_ENVELOPE ) ( int *type, int *num_integers, int *num_addresses, int *num_datatypes, int *combiner , int *ierr )
{
   *ierr = EMPI_Type_get_envelope(*type, num_integers, num_addresses, num_datatypes, combiner);
}


/* Intercomm */

void FORT_NAME( empi_intercomm_create , EMPI_INTERCOMM_CREATE ) ( int *local_comm, int *local_leader, int *peer_comm, int *remote_leader, int *tag, int *newintercomm , int *ierr )
{
   *ierr = EMPI_Intercomm_create(*local_comm, *local_leader, *peer_comm, *remote_leader, *tag, newintercomm);
}

void FORT_NAME( empi_intercomm_merge , EMPI_INTERCOMM_MERGE ) ( int *intercomm, int *high, int *newintracomm , int *ierr )
{
   *ierr = EMPI_Intercomm_merge(*intercomm, *high, newintracomm);
}





// HIERO




/*


void FORT_NAME( empi_add_error_string , EMPI_ADD_ERROR_STRING ) ( int errorcode, char *string , int *resultlen, int *ierr )
{
}


FORT_NAME( empi_type_create_f90_complex , EMPI_TYPE_CREATE_F90_COMPLEX ) ( int p, int r, EMPI_Datatype *newtype , int *ierr )
{
}

FORT_NAME( empi_type_create_f90_integer , EMPI_TYPE_CREATE_F90_INTEGER ) ( int r, EMPI_Datatype *newtype , int *ierr )
{
}

FORT_NAME( empi_type_create_f90_real , EMPI_TYPE_CREATE_F90_REAL ) ( int p, int r, EMPI_Datatype *newtype , int *ierr )
{
}

FORT_NAME( empi_accumulate , EMPI_ACCUMULATE ) ( void *origin_addr, int origin_count, EMPI_Datatype origin_datatype, int target_rank, EMPI_Aint target_disp, int target_count, EMPI_Datatype target_datatype, EMPI_Op op, EMPI_Win win , int *ierr )
{
}

FORT_NAME( empi_add_error_class , EMPI_ADD_ERROR_CLASS ) ( int *errorclass , int *ierr )
{
}

FORT_NAME( empi_add_error_code , EMPI_ADD_ERROR_CODE ) ( int errorclass, int *errorcode , int *ierr )
{
}


FORT_NAME( empi_address , EMPI_ADDRESS ) ( void *location, EMPI_Aint *address , int *ierr )
{
}


FORT_NAME( empi_alloc_mem , EMPI_ALLOC_MEM ) ( EMPI_Aint size, EMPI_Info info, void *baseptr , int *ierr )
{
}


FORT_NAME( empi_attr_delete , EMPI_ATTR_DELETE ) ( EMPI_Comm comm, int keyval , int *ierr )
{
}

FORT_NAME( empi_attr_get , EMPI_ATTR_GET ) ( EMPI_Comm comm, int keyval, void *attr_value, int *flag , int *ierr )
{
}

FORT_NAME( empi_attr_put , EMPI_ATTR_PUT ) ( EMPI_Comm comm, int keyval, void *attr_value , int *ierr )
{
}


FORT_NAME( empi_bsend , EMPI_BSEND ) ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm , int *ierr )
{
}

FORT_NAME( empi_bsend_init , EMPI_BSEND_INIT ) ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm, EMPI_Request *r , int *ierr )
{
}

FORT_NAME( empi_buffer_attach , EMPI_BUFFER_ATTACH ) ( void *buffer, int size , int *ierr )
{
}

FORT_NAME( empi_buffer_detach , EMPI_BUFFER_DETACH ) ( void *buffer, int *size , int *ierr )
{
}

FORT_NAME( empi_cancel , EMPI_CANCEL ) ( EMPI_Request *r , int *ierr )
{
}

FORT_NAME( empi_cart_coords , EMPI_CART_COORDS ) ( EMPI_Comm comm, int rank, int maxdims, int *coords , int *ierr )
{
}

FORT_NAME( empi_cart_create , EMPI_CART_CREATE ) ( EMPI_Comm comm_old, int ndims, int *dims, int *periods, int reorder, EMPI_Comm *comm_cart , int *ierr )
{
}

FORT_NAME( empi_cartdim_get , EMPI_CARTDIM_GET ) ( EMPI_Comm comm, int *ndims , int *ierr )
{
}

FORT_NAME( empi_cart_get , EMPI_CART_GET ) ( EMPI_Comm comm, int maxdims, int *dims, int *periods, int *coords , int *ierr )
{
}

FORT_NAME( empi_cart_map , EMPI_CART_MAP ) ( EMPI_Comm comm_old, int ndims, int *dims, int *periods, int *newrank , int *ierr )
{
}

FORT_NAME( empi_cart_rank , EMPI_CART_RANK ) ( EMPI_Comm comm, int *coords, int *rank , int *ierr )
{
}

FORT_NAME( empi_cart_shift , EMPI_CART_SHIFT ) ( EMPI_Comm comm, int direction, int displ, int *source, int *dest , int *ierr )
{
}

FORT_NAME( empi_cart_sub , EMPI_CART_SUB ) ( EMPI_Comm comm, int *remain_dims, EMPI_Comm *comm_new , int *ierr )
{
}

FORT_NAME( empi_close_port , EMPI_CLOSE_PORT ) ( char *port_name , int *ierr )
{
}

FORT_NAME( empi_comm_accept , EMPI_COMM_ACCEPT ) ( char *port_name, EMPI_Info info, int root, EMPI_Comm comm, EMPI_Comm *newcomm , int *ierr )
{
}

FORT_NAME( empi_comm_call_errhandler , EMPI_COMM_CALL_ERRHANDLER ) ( EMPI_Comm comm, int errorcode , int *ierr )
{
}

FORT_NAME( empi_comm_compare , EMPI_COMM_COMPARE ) ( EMPI_Comm comm1, EMPI_Comm comm2, int *result , int *ierr )
{
}

FORT_NAME( empi_comm_connect , EMPI_COMM_CONNECT ) ( char *port_name, EMPI_Info info, int root, EMPI_Comm comm, EMPI_Comm *newcomm , int *ierr )
{
}

FORT_NAME( empi_comm_create_errhandler , EMPI_COMM_CREATE_ERRHANDLER ) ( EMPI_Comm_errhandler_fn *function, EMPI_Errhandler *errhandler , int *ierr )
{
}

FORT_NAME( empi_comm_create_keyval , EMPI_COMM_CREATE_KEYVAL ) ( EMPI_Comm_copy_attr_function *comm_copy_attr_fn, EMPI_Comm_delete_attr_function *comm_delete_attr_fn, int *comm_keyval, void *extra_state , int *ierr )
{
}

FORT_NAME( empi_comm_delete_attr , EMPI_COMM_DELETE_ATTR ) ( EMPI_Comm comm, int comm_keyval , int *ierr )
{
}

FORT_NAME( empi_comm_disconnect , EMPI_COMM_DISCONNECT ) ( EMPI_Comm *comm , int *ierr )
{
}

FORT_NAME( empi_comm_free_keyval , EMPI_COMM_FREE_KEYVAL ) ( int *comm_keyval , int *ierr )
{
}

FORT_NAME( empi_comm_get_attr , EMPI_COMM_GET_ATTR ) ( EMPI_Comm comm, int comm_keyval, void *attribute_val, int *flag , int *ierr )
{
}

FORT_NAME( empi_comm_get_errhandler , EMPI_COMM_GET_ERRHANDLER ) ( EMPI_Comm comm, EMPI_Errhandler *errhandler , int *ierr )
{
}

FORT_NAME( empi_comm_get_name , EMPI_COMM_GET_NAME ) ( EMPI_Comm comm, char *comm_name, int *resultlen , int *ierr )
{
}

FORT_NAME( empi_comm_get_parent , EMPI_COMM_GET_PARENT ) ( EMPI_Comm *parent , int *ierr )
{
}

FORT_NAME( empi_comm_join , EMPI_COMM_JOIN ) ( int fd, EMPI_Comm *intercomm , int *ierr )
{
}

FORT_NAME( empi_comm_remote_group , EMPI_COMM_REMOTE_GROUP ) ( EMPI_Comm comm, EMPI_Group *g , int *ierr )
{
}

FORT_NAME( empi_comm_remote_size , EMPI_COMM_REMOTE_SIZE ) ( EMPI_Comm comm, int *size , int *ierr )
{
}

FORT_NAME( empi_comm_set_attr , EMPI_COMM_SET_ATTR ) ( EMPI_Comm comm, int comm_keyval, void *attribute_val , int *ierr )
{
}

FORT_NAME( empi_comm_set_errhandler , EMPI_COMM_SET_ERRHANDLER ) ( EMPI_Comm comm, EMPI_Errhandler errhandler , int *ierr )
{
}

FORT_NAME( empi_comm_set_name , EMPI_COMM_SET_NAME ) ( EMPI_Comm comm, char *comm_name , int *ierr )
{
}

FORT_NAME( empi_comm_spawn , EMPI_COMM_SPAWN ) ( char *command, char *argv[], int maxprocs, EMPI_Info info, int root, EMPI_Comm comm, EMPI_Comm *intercomm, int array_of_errcodes[] , int *ierr )
{
}

FORT_NAME( empi_comm_spawn_multiple , EMPI_COMM_SPAWN_MULTIPLE ) ( int count, char *array_of_commands[], char* *array_of_argv[], int array_of_maxprocs[], EMPI_Info array_of_info[], int root, EMPI_Comm comm, EMPI_Comm *intercomm, int array_of_errcodes[] , int *ierr )
{
}

FORT_NAME( empi_comm_test_inter , EMPI_COMM_TEST_INTER ) ( EMPI_Comm comm, int *flag , int *ierr )
{
}

FORT_NAME( empi_dims_create , EMPI_DIMS_CREATE ) ( int nnodes, int ndims, int *dims , int *ierr )
{
}

FORT_NAME( empi_dist_graph_create_adjacent , EMPI_DIST_GRAPH_CREATE_ADJACENT ) ( EMPI_Comm comm_old, int indegree, int sources[], int sourceweights[], int outdegree, int destinations[], int destweights[], EMPI_Info info, int reorder, EMPI_Comm *comm_dist_graph , int *ierr )
{
}

FORT_NAME( empi_dist_graph_create , EMPI_DIST_GRAPH_CREATE ) ( EMPI_Comm comm_old, int n, int sources[], int degrees[], int destinations[], int weights[], EMPI_Info info, int reorder, EMPI_Comm *comm_dist_graph , int *ierr )
{
}

FORT_NAME( empi_dist_graph_neighbors_count , EMPI_DIST_GRAPH_NEIGHBORS_COUNT ) ( EMPI_Comm comm, int *indegree, int *outdegree, int *weighted , int *ierr )
{
}

FORT_NAME( empi_dist_graph_neighbors , EMPI_DIST_GRAPH_NEIGHBORS ) ( EMPI_Comm comm, int maxindegree, int sources[], int sourceweights[], int maxoutdegree, int destinations[], int destweights[] , int *ierr )
{
}

FORT_NAME( empi_errhandler_create , EMPI_ERRHANDLER_CREATE ) ( EMPI_Handler_function *function, EMPI_Errhandler *errhandler , int *ierr )
{
}

FORT_NAME( empi_errhandler_free , EMPI_ERRHANDLER_FREE ) ( EMPI_Errhandler *errhandler , int *ierr )
{
}

FORT_NAME( empi_errhandler_get , EMPI_ERRHANDLER_GET ) ( EMPI_Comm comm, EMPI_Errhandler *errhandler , int *ierr )
{
}

FORT_NAME( empi_errhandler_set , EMPI_ERRHANDLER_SET ) ( EMPI_Comm comm, EMPI_Errhandler errhandler , int *ierr )
{
}

FORT_NAME( empi_error_class , EMPI_ERROR_CLASS ) ( int errorcode, int *errorclass , int *ierr )
{
}


FORT_NAME( empi_exscan , EMPI_EXSCAN ) ( void *sendbuf, void *recvbuf, int count, EMPI_Datatype datatype, EMPI_Op op, EMPI_Comm comm , int *ierr )
{
}

FORT_NAME( empi_file_call_errhandler , EMPI_FILE_CALL_ERRHANDLER ) ( EMPI_File fh, int errorcode , int *ierr )
{
}

FORT_NAME( empi_file_create_errhandler , EMPI_FILE_CREATE_ERRHANDLER ) ( EMPI_File_errhandler_fn *function, EMPI_Errhandler *errhandler , int *ierr )
{
}


FORT_NAME( empi_file_delete , EMPI_FILE_DELETE ) ( char *filename, EMPI_Info info , int *ierr )
{
}

FORT_NAME( empi_file_get_amode , EMPI_FILE_GET_AMODE ) ( EMPI_File EMPI_fh, int *amode , int *ierr )
{
}

FORT_NAME( empi_file_get_atomicity , EMPI_FILE_GET_ATOMICITY ) ( EMPI_File EMPI_fh, int *flag , int *ierr )
{
}

FORT_NAME( empi_file_get_byte_offset , EMPI_FILE_GET_BYTE_OFFSET ) ( EMPI_File EMPI_fh, EMPI_Offset offset, EMPI_Offset *disp , int *ierr )
{
}

FORT_NAME( empi_file_get_errhandler , EMPI_FILE_GET_ERRHANDLER ) ( EMPI_File file, EMPI_Errhandler *errhandler , int *ierr )
{
}

FORT_NAME( empi_file_get_group , EMPI_FILE_GET_GROUP ) ( EMPI_File EMPI_fh, EMPI_Group *g , int *ierr )
{
}

FORT_NAME( empi_file_get_info , EMPI_FILE_GET_INFO ) ( EMPI_File EMPI_fh, EMPI_Info *info_used , int *ierr )
{
}

FORT_NAME( empi_file_get_position , EMPI_FILE_GET_POSITION ) ( EMPI_File EMPI_fh, EMPI_Offset *offset , int *ierr )
{
}

FORT_NAME( empi_file_get_position_shared , EMPI_FILE_GET_POSITION_SHARED ) ( EMPI_File EMPI_fh, EMPI_Offset *offset , int *ierr )
{
}

FORT_NAME( empi_file_get_size , EMPI_FILE_GET_SIZE ) ( EMPI_File EMPI_fh, EMPI_Offset *size , int *ierr )
{
}

FORT_NAME( empi_file_get_type_extent , EMPI_FILE_GET_TYPE_EXTENT ) ( EMPI_File EMPI_fh, EMPI_Datatype datatype, EMPI_Aint *extent , int *ierr )
{
}

FORT_NAME( empi_file_get_view , EMPI_FILE_GET_VIEW ) ( EMPI_File EMPI_fh, EMPI_Offset *disp, EMPI_Datatype *etype, EMPI_Datatype *filetype, char *datarep , int *ierr )
{
}

FORT_NAME( empi_file_iread_at , EMPI_FILE_IREAD_AT ) ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype datatype, MPIO_Request *r , int *ierr )
{
}

FORT_NAME( empi_file_iread , EMPI_FILE_IREAD ) ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Request *r , int *ierr )
{
}

FORT_NAME( empi_file_iread_shared , EMPI_FILE_IREAD_SHARED ) ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Request *r , int *ierr )
{
}

FORT_NAME( empi_file_iwrite_at , EMPI_FILE_IWRITE_AT ) ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype datatype, MPIO_Request *r , int *ierr )
{
}

FORT_NAME( empi_file_iwrite , EMPI_FILE_IWRITE ) ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Request *r , int *ierr )
{
}

FORT_NAME( empi_file_iwrite_shared , EMPI_FILE_IWRITE_SHARED ) ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, MPIO_Request *r , int *ierr )
{
}


FORT_NAME( empi_file_preallocate , EMPI_FILE_PREALLOCATE ) ( EMPI_File EMPI_fh, EMPI_Offset size , int *ierr )
{
}

FORT_NAME( empi_file_read_all_begin , EMPI_FILE_READ_ALL_BEGIN ) ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype , int *ierr )
{
}

FORT_NAME( empi_file_read_all_end , EMPI_FILE_READ_ALL_END ) ( EMPI_File EMPI_fh, void *buf, EMPI_Status *status , int *ierr )
{
}


FORT_NAME( empi_file_read_at_all_begin , EMPI_FILE_READ_AT_ALL_BEGIN ) ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype datatype , int *ierr )
{
}

FORT_NAME( empi_file_read_at_all_end , EMPI_FILE_READ_AT_ALL_END ) ( EMPI_File EMPI_fh, void *buf, EMPI_Status *status , int *ierr )
{
}

FORT_NAME( empi_file_read_at_all , EMPI_FILE_READ_AT_ALL ) ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status , int *ierr )
{
}

FORT_NAME( empi_file_read , EMPI_FILE_READ ) ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status , int *ierr )
{
}

FORT_NAME( empi_file_read_ordered_begin , EMPI_FILE_READ_ORDERED_BEGIN ) ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype , int *ierr )
{
}

FORT_NAME( empi_file_read_ordered_end , EMPI_FILE_READ_ORDERED_END ) ( EMPI_File EMPI_fh, void *buf, EMPI_Status *status , int *ierr )
{
}

FORT_NAME( empi_file_read_ordered , EMPI_FILE_READ_ORDERED ) ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status , int *ierr )
{
}

FORT_NAME( empi_file_read_shared , EMPI_FILE_READ_SHARED ) ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status , int *ierr )
{
}

FORT_NAME( empi_file_seek , EMPI_FILE_SEEK ) ( EMPI_File EMPI_fh, EMPI_Offset offset, int whence , int *ierr )
{
}

FORT_NAME( empi_file_seek_shared , EMPI_FILE_SEEK_SHARED ) ( EMPI_File EMPI_fh, EMPI_Offset offset, int whence , int *ierr )
{
}

FORT_NAME( empi_file_set_atomicity , EMPI_FILE_SET_ATOMICITY ) ( EMPI_File EMPI_fh, int flag , int *ierr )
{
}

FORT_NAME( empi_file_set_errhandler , EMPI_FILE_SET_ERRHANDLER ) ( EMPI_File file, EMPI_Errhandler errhandler , int *ierr )
{
}

FORT_NAME( empi_file_set_info , EMPI_FILE_SET_INFO ) ( EMPI_File EMPI_fh, EMPI_Info info , int *ierr )
{
}

FORT_NAME( empi_file_set_size , EMPI_FILE_SET_SIZE ) ( EMPI_File EMPI_fh, EMPI_Offset size , int *ierr )
{
}


FORT_NAME( empi_file_sync , EMPI_FILE_SYNC ) ( EMPI_File EMPI_fh , int *ierr )
{
}

FORT_NAME( empi_file_write_all_begin , EMPI_FILE_WRITE_ALL_BEGIN ) ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype , int *ierr )
{
}

FORT_NAME( empi_file_write_all_end , EMPI_FILE_WRITE_ALL_END ) ( EMPI_File EMPI_fh, void *buf, EMPI_Status *status , int *ierr )
{
}


FORT_NAME( empi_file_write_at_all_begin , EMPI_FILE_WRITE_AT_ALL_BEGIN ) ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype datatype , int *ierr )
{
}

FORT_NAME( empi_file_write_at_all_end , EMPI_FILE_WRITE_AT_ALL_END ) ( EMPI_File EMPI_fh, void *buf, EMPI_Status *status , int *ierr )
{
}

FORT_NAME( empi_file_write_at_all , EMPI_FILE_WRITE_AT_ALL ) ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status , int *ierr )
{
}

FORT_NAME( empi_file_write , EMPI_FILE_WRITE ) ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status , int *ierr )
{
}

FORT_NAME( empi_file_write_ordered_begin , EMPI_FILE_WRITE_ORDERED_BEGIN ) ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype , int *ierr )
{
}

FORT_NAME( empi_file_write_ordered_end , EMPI_FILE_WRITE_ORDERED_END ) ( EMPI_File EMPI_fh, void *buf, EMPI_Status *status , int *ierr )
{
}

FORT_NAME( empi_file_write_ordered , EMPI_FILE_WRITE_ORDERED ) ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status , int *ierr )
{
}

FORT_NAME( empi_file_write_shared , EMPI_FILE_WRITE_SHARED ) ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status , int *ierr )
{
}


FORT_NAME( empi_free_mem , EMPI_FREE_MEM ) ( void *base , int *ierr )
{
}


FORT_NAME( empi_get_address , EMPI_GET_ADDRESS ) ( void *location, EMPI_Aint *address , int *ierr )
{
}

FORT_NAME( empi_get_count , EMPI_GET_COUNT ) ( EMPI_Status *status, EMPI_Datatype datatype, int *count , int *ierr )
{
}

FORT_NAME( empi_get_elements , EMPI_GET_ELEMENTS ) ( EMPI_Status *status, EMPI_Datatype datatype, int *elements , int *ierr )
{
}

FORT_NAME( empi_get , EMPI_GET ) ( void *origin_addr, int origin_count, EMPI_Datatype origin_datatype, int target_rank, EMPI_Aint target_disp, int target_count, EMPI_Datatype target_datatype, EMPI_Win win , int *ierr )
{
}


FORT_NAME( empi_get_version , EMPI_GET_VERSION ) ( int *version, int *subversion , int *ierr )
{
}

FORT_NAME( empi_graph_create , EMPI_GRAPH_CREATE ) ( EMPI_Comm comm_old, int nnodes, int *indx, int *edges, int reorder, EMPI_Comm *comm_graph , int *ierr )
{
}

FORT_NAME( empi_graphdims_get , EMPI_GRAPHDIMS_GET ) ( EMPI_Comm comm, int *nnodes, int *nedges , int *ierr )
{
}

FORT_NAME( empi_graph_get , EMPI_GRAPH_GET ) ( EMPI_Comm comm, int maxindex, int maxedges, int *indx, int *edges , int *ierr )
{
}

FORT_NAME( empi_graph_map , EMPI_GRAPH_MAP ) ( EMPI_Comm comm_old, int nnodes, int *indx, int *edges, int *newrank , int *ierr )
{
}

FORT_NAME( empi_graph_neighbors_count , EMPI_GRAPH_NEIGHBORS_COUNT ) ( EMPI_Comm comm, int rank, int *nneighbors , int *ierr )
{
}

FORT_NAME( empi_graph_neighbors , EMPI_GRAPH_NEIGHBORS ) ( EMPI_Comm comm, int rank, int maxneighbors, int *neighbors , int *ierr )
{
}

FORT_NAME( empi_grequest_complete , EMPI_GREQUEST_COMPLETE ) ( EMPI_Request r , int *ierr )
{
}

FORT_NAME( empi_grequest_start , EMPI_GREQUEST_START ) ( EMPI_Grequest_query_function *query_fn, EMPI_Grequest_free_function *free_fn, EMPI_Grequest_cancel_function *cancel_fn, void *extra_state, EMPI_Request *r , int *ierr )
{
}

FORT_NAME( empi_group_compare , EMPI_GROUP_COMPARE ) ( EMPI_Group group1, EMPI_Group group2, int *result , int *ierr )
{
}

FORT_NAME( empi_group_difference , EMPI_GROUP_DIFFERENCE ) ( EMPI_Group group1, EMPI_Group group2, EMPI_Group *newgroup , int *ierr )
{
}

FORT_NAME( empi_group_free , EMPI_GROUP_FREE ) ( EMPI_Group *g , int *ierr )
{
}



FORT_NAME( empi_group_intersection , EMPI_GROUP_INTERSECTION ) ( EMPI_Group group1, EMPI_Group group2, EMPI_Group *newgroup , int *ierr )
{
}


FORT_NAME( empi_group_excl , EMPI_GROUP_EXCL ) ( EMPI_Group g, int n, int *ranks, EMPI_Group *newgroup , int *ierr )
{
}



FORT_NAME( empi_group_rank , EMPI_GROUP_RANK ) ( EMPI_Group g, int *rank , int *ierr )
{
}

FORT_NAME( empi_group_size , EMPI_GROUP_SIZE ) ( EMPI_Group g, int *size , int *ierr )
{
}



FORT_NAME( empi_info_dup , EMPI_INFO_DUP ) ( EMPI_Info info, EMPI_Info *newinfo , int *ierr )
{
}


FORT_NAME( empi_info_get , EMPI_INFO_GET ) ( EMPI_Info info, char *key, int valuelen, char *value, int *flag , int *ierr )
{
}

FORT_NAME( empi_info_get_nkeys , EMPI_INFO_GET_NKEYS ) ( EMPI_Info info, int *nkeys , int *ierr )
{
}

FORT_NAME( empi_info_get_nthkey , EMPI_INFO_GET_NTHKEY ) ( EMPI_Info info, int n, char *key , int *ierr )
{
}

FORT_NAME( empi_info_get_valuelen , EMPI_INFO_GET_VALUELEN ) ( EMPI_Info info, char *key, int *valuelen, int *flag , int *ierr )
{
}


FORT_NAME( empi_init_thread , EMPI_INIT_THREAD ) ( int *argc, char ***argv, int required, int *provided , int *ierr )
{
}

FORT_NAME( empi_iprobe , EMPI_IPROBE ) ( int source, int tag, EMPI_Comm comm, int *flag, EMPI_Status *status , int *ierr )
{
}


FORT_NAME( empi_issend , EMPI_ISSEND ) ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm, EMPI_Request *r , int *ierr )
{
}

FORT_NAME( empi_is_thread_main , EMPI_IS_THREAD_MAIN ) ( int *flag , int *ierr )
{
}

FORT_NAME( empi_keyval_create , EMPI_KEYVAL_CREATE ) ( EMPI_Copy_function *copy_fn, EMPI_Delete_function *delete_fn, int *keyval, void *extra_state , int *ierr )
{
}

FORT_NAME( empi_keyval_free , EMPI_KEYVAL_FREE ) ( int *keyval , int *ierr )
{
}

FORT_NAME( empi_lookup_name , EMPI_LOOKUP_NAME ) ( char *service_name, EMPI_Info info, char *port_name , int *ierr )
{
}

FORT_NAME( empi_op_commutative , EMPI_OP_COMMUTATIVE ) ( EMPI_Op op, int *commute , int *ierr )
{
}

FORT_NAME( empi_op_create , EMPI_OP_CREATE ) ( EMPI_User_function *function, int commute, EMPI_Op *op , int *ierr )
{
}

FORT_NAME( empi_open_port , EMPI_OPEN_PORT ) ( EMPI_Info info, char *port_name , int *ierr )
{
}

FORT_NAME( empi_op_free , EMPI_OP_FREE ) ( EMPI_Op *op , int *ierr )
{
}

FORT_NAME( empi_pack_external , EMPI_PACK_EXTERNAL ) ( char *datarep, void *inbuf, int incount, EMPI_Datatype datatype, void *outbuf, EMPI_Aint outcount, EMPI_Aint *position , int *ierr )
{
}

FORT_NAME( empi_pack_external_size , EMPI_PACK_EXTERNAL_SIZE ) ( char *datarep, int incount, EMPI_Datatype datatype, EMPI_Aint *size , int *ierr )
{
}

FORT_NAME( empi_pack , EMPI_PACK ) ( void *inbuf, int incount, EMPI_Datatype datatype, void *outbuf, int outcount, int *position, EMPI_Comm comm , int *ierr )
{
}

FORT_NAME( empi_pack_size , EMPI_PACK_SIZE ) ( int incount, EMPI_Datatype datatype, EMPI_Comm comm, int *size , int *ierr )
{
}

FORT_NAME( empi_probe , EMPI_PROBE ) ( int source, int tag, EMPI_Comm comm, EMPI_Status *status , int *ierr )
{
}

FORT_NAME( empi_publish_name , EMPI_PUBLISH_NAME ) ( char *service_name, EMPI_Info info, char *port_name , int *ierr )
{
}

FORT_NAME( empi_put , EMPI_PUT ) ( void *origin_addr, int origin_count, EMPI_Datatype origin_datatype, int target_rank, EMPI_Aint target_disp, int target_count, EMPI_Datatype target_datatype, EMPI_Win win , int *ierr )
{
}

FORT_NAME( empi_query_thread , EMPI_QUERY_THREAD ) ( int *provided , int *ierr )
{
}

FORT_NAME( empi_recv_init , EMPI_RECV_INIT ) ( void *buf, int count, EMPI_Datatype datatype, int source, int tag, EMPI_Comm comm, EMPI_Request *r , int *ierr )
{
}


FORT_NAME( empi_reduce_local , EMPI_REDUCE_LOCAL ) ( void *inbuf, void *inoutbuf, int count, EMPI_Datatype datatype, EMPI_Op op , int *ierr )
{
}

FORT_NAME( empi_reduce_scatter_block , EMPI_REDUCE_SCATTER_BLOCK ) ( void *sendbuf, void *recvbuf, int recvcount, EMPI_Datatype datatype, EMPI_Op op, EMPI_Comm comm , int *ierr )
{
}

FORT_NAME( empi_reduce_scatter , EMPI_REDUCE_SCATTER ) ( void *sendbuf, void *recvbuf, int *recvcnts, EMPI_Datatype datatype, EMPI_Op op, EMPI_Comm comm , int *ierr )
{
}

FORT_NAME( empi_register_datarep , EMPI_REGISTER_DATAREP ) ( char *name, EMPI_Datarep_conversion_function *read_conv_fn, EMPI_Datarep_conversion_function *write_conv_fn, EMPI_Datarep_extent_function *extent_fn, void *state , int *ierr )
{
}


FORT_NAME( empi_request_get_status , EMPI_REQUEST_GET_STATUS ) ( EMPI_Request r, int *flag, EMPI_Status *status , int *ierr )
{
}

FORT_NAME( empi_rsend_init , EMPI_RSEND_INIT ) ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm, EMPI_Request *r , int *ierr )
{
}

FORT_NAME( empi_scan , EMPI_SCAN ) ( void *sendbuf, void *recvbuf, int count, EMPI_Datatype datatype, EMPI_Op op, EMPI_Comm comm , int *ierr )
{
}

FORT_NAME( empi_send_init , EMPI_SEND_INIT ) ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm, EMPI_Request *r , int *ierr )
{
}



FORT_NAME( empi_sendrecv_replace , EMPI_SENDRECV_REPLACE ) ( void *buf, int count, EMPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, EMPI_Comm comm, EMPI_Status *status , int *ierr )
{
}

FORT_NAME( empi_ssend_init , EMPI_SSEND_INIT ) ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm, EMPI_Request *r , int *ierr )
{
}

FORT_NAME( empi_startall , EMPI_STARTALL ) ( int count, EMPI_Request array_of_requests[] , int *ierr )
{
}

FORT_NAME( empi_start , EMPI_START ) ( EMPI_Request *r , int *ierr )
{
}

FORT_NAME( empi_status_set_cancelled , EMPI_STATUS_SET_CANCELLED ) ( EMPI_Status *status, int flag , int *ierr )
{
}

FORT_NAME( empi_status_set_elements , EMPI_STATUS_SET_ELEMENTS ) ( EMPI_Status *status, EMPI_Datatype datatype, int count , int *ierr )
{
}

FORT_NAME( empi_testall , EMPI_TESTALL ) ( int count, EMPI_Request array_of_requests[], int *flag, EMPI_Status array_of_statuses[] , int *ierr )
{
}

FORT_NAME( empi_testany , EMPI_TESTANY ) ( int count, EMPI_Request array_of_requests[], int *index, int *flag, EMPI_Status *status , int *ierr )
{
}

FORT_NAME( empi_test_cancelled , EMPI_TEST_CANCELLED ) ( EMPI_Status *status, int *flag , int *ierr )
{
}

FORT_NAME( empi_test , EMPI_TEST ) ( EMPI_Request *r, int *flag, EMPI_Status *status , int *ierr )
{
}

FORT_NAME( empi_testsome , EMPI_TESTSOME ) ( int incount, EMPI_Request array_of_requests[], int *outcount, int array_of_indices[], EMPI_Status array_of_statuses[] , int *ierr )
{
}

FORT_NAME( empi_topo_test , EMPI_TOPO_TEST ) ( EMPI_Comm comm, int *topo_type , int *ierr )
{
}

FORT_NAME( empi_type_create_darray , EMPI_TYPE_CREATE_DARRAY ) ( int size, int rank, int ndims, int array_of_gsizes[], int array_of_distribs[], int array_of_dargs[], int array_of_psizes[], int order, EMPI_Datatype oldtype, EMPI_Datatype *newtype , int *ierr )
{
}

FORT_NAME( empi_type_create_hindexed , EMPI_TYPE_CREATE_HINDEXED ) ( int count, int blocklengths[], EMPI_Aint displacements[], EMPI_Datatype oldtype, EMPI_Datatype *newtype , int *ierr )
{
}

FORT_NAME( empi_type_create_hvector , EMPI_TYPE_CREATE_HVECTOR ) ( int count, int blocklength, EMPI_Aint stride, EMPI_Datatype oldtype, EMPI_Datatype *newtype , int *ierr )
{
}

FORT_NAME( empi_type_create_keyval , EMPI_TYPE_CREATE_KEYVAL ) ( EMPI_Type_copy_attr_function *type_copy_attr_fn, EMPI_Type_delete_attr_function *type_delete_attr_fn, int *type_keyval, void *extra_state , int *ierr )
{
}

FORT_NAME( empi_type_create_resized , EMPI_TYPE_CREATE_RESIZED ) ( EMPI_Datatype oldtype, EMPI_Aint lb, EMPI_Aint extent, EMPI_Datatype *newtype , int *ierr )
{
}

FORT_NAME( empi_type_create_struct , EMPI_TYPE_CREATE_STRUCT ) ( int count, int array_of_blocklengths[], EMPI_Aint array_of_displacements[], EMPI_Datatype array_of_types[], EMPI_Datatype *newtype , int *ierr )
{
}

FORT_NAME( empi_type_create_subarray , EMPI_TYPE_CREATE_SUBARRAY ) ( int ndims, int array_of_sizes[], int array_of_subsizes[], int array_of_starts[], int order, EMPI_Datatype oldtype, EMPI_Datatype *newtype , int *ierr )
{
}

FORT_NAME( empi_type_delete_attr , EMPI_TYPE_DELETE_ATTR ) ( EMPI_Datatype type, int type_keyval , int *ierr )
{
}

FORT_NAME( empi_type_dup , EMPI_TYPE_DUP ) ( EMPI_Datatype datatype, EMPI_Datatype *newtype , int *ierr )
{
}

FORT_NAME( empi_type_extent , EMPI_TYPE_EXTENT ) ( EMPI_Datatype datatype, EMPI_Aint *extent , int *ierr )
{
}

FORT_NAME( empi_type_free_keyval , EMPI_TYPE_FREE_KEYVAL ) ( int *type_keyval , int *ierr )
{
}

FORT_NAME( empi_type_get_attr , EMPI_TYPE_GET_ATTR ) ( EMPI_Datatype type, int type_keyval, void *attribute_val, int *flag , int *ierr )
{
}

FORT_NAME( empi_type_get_contents , EMPI_TYPE_GET_CONTENTS ) ( EMPI_Datatype datatype, int max_integers, int max_addresses, int max_datatypes, int array_of_integers[], EMPI_Aint array_of_addresses[], EMPI_Datatype array_of_datatypes[] , int *ierr )
{
}


FORT_NAME( empi_type_get_extent , EMPI_TYPE_GET_EXTENT ) ( EMPI_Datatype datatype, EMPI_Aint *lb, EMPI_Aint *extent , int *ierr )
{
}

FORT_NAME( empi_type_get_name , EMPI_TYPE_GET_NAME ) ( EMPI_Datatype datatype, char *type_name, int *resultlen , int *ierr )
{
}

FORT_NAME( empi_type_get_true_extent , EMPI_TYPE_GET_TRUE_EXTENT ) ( EMPI_Datatype datatype, EMPI_Aint *true_lb, EMPI_Aint *true_extent , int *ierr )
{
}

FORT_NAME( empi_type_hindexed , EMPI_TYPE_HINDEXED ) ( int count, int blocklens[], EMPI_Aint indices[], EMPI_Datatype old_type, EMPI_Datatype *newtype , int *ierr )
{
}

FORT_NAME( empi_type_hvector , EMPI_TYPE_HVECTOR ) ( int count, int blocklen, EMPI_Aint stride, EMPI_Datatype old_type, EMPI_Datatype *newtype_p , int *ierr )
{
}

FORT_NAME( empi_type_indexed , EMPI_TYPE_INDEXED ) ( int count, int blocklens[], int indices[], EMPI_Datatype old_type, EMPI_Datatype *newtype , int *ierr )
{
}

FORT_NAME( empi_type_lb , EMPI_TYPE_LB ) ( EMPI_Datatype datatype, EMPI_Aint *displacement , int *ierr )
{
}

FORT_NAME( empi_type_match_size , EMPI_TYPE_MATCH_SIZE ) ( int typeclass, int size, EMPI_Datatype *datatype , int *ierr )
{
}

FORT_NAME( empi_type_set_attr , EMPI_TYPE_SET_ATTR ) ( EMPI_Datatype type, int type_keyval, void *attribute_val , int *ierr )
{
}

FORT_NAME( empi_type_set_name , EMPI_TYPE_SET_NAME ) ( EMPI_Datatype type, char *type_name , int *ierr )
{
}

FORT_NAME( empi_type_size , EMPI_TYPE_SIZE ) ( EMPI_Datatype datatype, int *size , int *ierr )
{
}

FORT_NAME( empi_type_struct , EMPI_TYPE_STRUCT ) ( int count, int blocklens[], EMPI_Aint indices[], EMPI_Datatype old_types[], EMPI_Datatype *newtype , int *ierr )
{
}

FORT_NAME( empi_type_ub , EMPI_TYPE_UB ) ( EMPI_Datatype datatype, EMPI_Aint *displacement , int *ierr )
{
}

FORT_NAME( empi_type_vector , EMPI_TYPE_VECTOR ) ( int count, int blocklength, int stride, EMPI_Datatype old_type, EMPI_Datatype *newtype_p , int *ierr )
{
}

FORT_NAME( empi_unpack_external , EMPI_UNPACK_EXTERNAL ) ( char *datarep, void *inbuf, EMPI_Aint insize, EMPI_Aint *position, void *outbuf, int outcount, EMPI_Datatype datatype , int *ierr )
{
}

FORT_NAME( empi_unpack , EMPI_UNPACK ) ( void *inbuf, int insize, int *position, void *outbuf, int outcount, EMPI_Datatype datatype, EMPI_Comm comm , int *ierr )
{
}

FORT_NAME( empi_unpublish_name , EMPI_UNPUBLISH_NAME ) ( char *service_name, EMPI_Info info, char *port_name , int *ierr )
{
}



FORT_NAME( empi_waitsome , EMPI_WAITSOME ) ( int incount, EMPI_Request array_of_requests[], int *outcount, int array_of_indices[], EMPI_Status array_of_statuses[] , int *ierr )
{
}

FORT_NAME( empi_win_call_errhandler , EMPI_WIN_CALL_ERRHANDLER ) ( EMPI_Win win, int errorcode , int *ierr )
{
}

FORT_NAME( empi_win_complete , EMPI_WIN_COMPLETE ) ( EMPI_Win win , int *ierr )
{
}

FORT_NAME( empi_win_create_errhandler , EMPI_WIN_CREATE_ERRHANDLER ) ( EMPI_Win_errhandler_fn *function, EMPI_Errhandler *errhandler , int *ierr )
{
}

FORT_NAME( empi_win_create , EMPI_WIN_CREATE ) ( void *base, EMPI_Aint size, int disp_unit, EMPI_Info info, EMPI_Comm comm, EMPI_Win *win , int *ierr )
{
}

FORT_NAME( empi_win_create_keyval , EMPI_WIN_CREATE_KEYVAL ) ( EMPI_Win_copy_attr_function *win_copy_attr_fn, EMPI_Win_delete_attr_function *win_delete_attr_fn, int *win_keyval, void *extra_state , int *ierr )
{
}

FORT_NAME( empi_win_delete_attr , EMPI_WIN_DELETE_ATTR ) ( EMPI_Win win, int win_keyval , int *ierr )
{
}

FORT_NAME( empi_win_fence , EMPI_WIN_FENCE ) ( int assert, EMPI_Win win , int *ierr )
{
}

FORT_NAME( empi_win_free , EMPI_WIN_FREE ) ( EMPI_Win *win , int *ierr )
{
}

FORT_NAME( empi_win_free_keyval , EMPI_WIN_FREE_KEYVAL ) ( int *win_keyval , int *ierr )
{
}

FORT_NAME( empi_win_get_attr , EMPI_WIN_GET_ATTR ) ( EMPI_Win win, int win_keyval, void *attribute_val, int *flag , int *ierr )
{
}

FORT_NAME( empi_win_get_errhandler , EMPI_WIN_GET_ERRHANDLER ) ( EMPI_Win win, EMPI_Errhandler *errhandler , int *ierr )
{
}

FORT_NAME( empi_win_get_group , EMPI_WIN_GET_GROUP ) ( EMPI_Win win, EMPI_Group *g , int *ierr )
{
}

FORT_NAME( empi_win_get_name , EMPI_WIN_GET_NAME ) ( EMPI_Win win, char *win_name, int *resultlen , int *ierr )
{
}

FORT_NAME( empi_win_lock , EMPI_WIN_LOCK ) ( int lock_type, int rank, int assert, EMPI_Win win , int *ierr )
{
}

FORT_NAME( empi_win_post , EMPI_WIN_POST ) ( EMPI_Group g, int assert, EMPI_Win win , int *ierr )
{
}

FORT_NAME( empi_win_set_attr , EMPI_WIN_SET_ATTR ) ( EMPI_Win win, int win_keyval, void *attribute_val , int *ierr )
{
}

FORT_NAME( empi_win_set_errhandler , EMPI_WIN_SET_ERRHANDLER ) ( EMPI_Win win, EMPI_Errhandler errhandler , int *ierr )
{
}

FORT_NAME( empi_win_set_name , EMPI_WIN_SET_NAME ) ( EMPI_Win win, char *win_name , int *ierr )
{
}

FORT_NAME( empi_win_start , EMPI_WIN_START ) ( EMPI_Group g, int assert, EMPI_Win win , int *ierr )
{
}

FORT_NAME( empi_win_test , EMPI_WIN_TEST ) ( EMPI_Win win, int *flag , int *ierr )
{
}

FORT_NAME( empi_win_unlock , EMPI_WIN_UNLOCK ) ( int rank, EMPI_Win win , int *ierr )
{
}

FORT_NAME( empi_win_wait , EMPI_WIN_WAIT ) ( EMPI_Win win , int *ierr )
{
}

FORT_NAME( empi_group_comm_create , EMPI_GROUP_COMM_CREATE ) ( EMPI_Comm old_comm, EMPI_Group g, int tag, EMPI_Comm *new_comm , int *ierr )
{
}

FORT_NAME( empi_comm_c2f , EMPI_COMM_C2F ) ( EMPI_Comm comm , int *ierr )
{
}

FORT_NAME( empi_group_c2f , EMPI_GROUP_C2F ) ( EMPI_Group g , int *ierr )
{
}

FORT_NAME( empi_request_c2f , EMPI_REQUEST_C2F ) ( EMPI_Request r , int *ierr )
{
}

FORT_NAME( empi_info_c2f , EMPI_INFO_C2F ) ( EMPI_Info info , int *ierr )
{
}

FORT_NAME( empi_file_c2f , EMPI_FILE_C2F ) ( EMPI_File file , int *ierr )
{
}

FORT_NAME( empi_op_c2f , EMPI_OP_C2F ) ( EMPI_Op op , int *ierr )
{
}

FORT_NAME( empi_win_c2f , EMPI_WIN_C2F ) ( EMPI_Win Win , int *ierr )
{
}

FORT_NAME( empi_errhandler_c2f , EMPI_ERRHANDLER_C2F ) ( EMPI_Errhandler Errhandler , int *ierr )
{
}

FORT_NAME( empi_type_c2f , EMPI_TYPE_C2F ) ( EMPI_Datatype Type , int *ierr )
{
}

*/
