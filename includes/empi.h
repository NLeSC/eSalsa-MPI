#ifndef __EMPI_H_
#define __EMPI_H_

#include "mpi.h"

// Misc. utility functions

int EMPI_Init ( int *argc, char ***argv );
int EMPI_Initialized ( int *flag );

int EMPI_Finalized ( int *flag );
int EMPI_Finalize ( void );

int EMPI_Abort ( MPI_Comm comm, int errorcode );

int EMPI_Get_processor_name ( char *name, int *resultlen );
int EMPI_Error_string ( int errorcode, char *string, int *resultlen );

double EMPI_Wtime(void);


// Communicators and groups.

int EMPI_Comm_create ( MPI_Comm comm, MPI_Group g, MPI_Comm *newcomm );
int EMPI_Comm_dup ( MPI_Comm comm, MPI_Comm *newcomm );
int EMPI_Comm_free ( MPI_Comm *comm );
int EMPI_Comm_group ( MPI_Comm comm, MPI_Group *g );
int EMPI_Comm_rank ( MPI_Comm comm, int *rank );
int EMPI_Comm_size ( MPI_Comm comm, int *size );
int EMPI_Comm_split ( MPI_Comm comm, int color, int key, MPI_Comm *newcomm );

int EMPI_Group_range_incl ( MPI_Group g, int n, int ranges[][3], MPI_Group *newgroup );
int EMPI_Group_range_excl ( MPI_Group g, int n, int ranges[][3], MPI_Group *newgroup );

int EMPI_Group_translate_ranks ( MPI_Group group1, int n, int *ranks1, MPI_Group group2, int *ranks2 );
int EMPI_Group_union ( MPI_Group group1, MPI_Group group2, MPI_Group *newgroup );
int EMPI_Group_incl ( MPI_Group g, int n, int *ranks, MPI_Group *newgroup );

// Collectives

int EMPI_Allgather ( void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm );
int EMPI_Allgatherv ( void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int *recvcounts, int *displs, MPI_Datatype recvtype, MPI_Comm comm );
int EMPI_Allreduce ( void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm );
int EMPI_Alltoall ( void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm );
int EMPI_Alltoallv ( void *sendbuf, int *sendcnts, int *sdispls, MPI_Datatype sendtype, void *recvbuf, int *recvcnts, int *rdispls, MPI_Datatype recvtype, MPI_Comm comm );
int EMPI_Alltoallw ( void *sendbuf, int *sendcnts, int *sdispls, MPI_Datatype *sendtypes, void *recvbuf, int *recvcnts, int *rdispls, MPI_Datatype *recvtypes, MPI_Comm comm );
int EMPI_Scatter ( void *sendbuf, int sendcnt, MPI_Datatype sendtype, void *recvbuf, int recvcnt, MPI_Datatype recvtype, int root, MPI_Comm comm );
int EMPI_Scatterv ( void *sendbuf, int *sendcnts, int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcnt, MPI_Datatype recvtype, int root, MPI_Comm comm );
int EMPI_Barrier ( MPI_Comm comm );
int EMPI_Bcast ( void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm );
int EMPI_Gather ( void *sendbuf, int sendcnt, MPI_Datatype sendtype, void *recvbuf, int recvcnt, MPI_Datatype recvtype, int root, MPI_Comm comm );
int EMPI_Gatherv ( void *sendbuf, int sendcnt, MPI_Datatype sendtype, void *recvbuf, int *recvcnts, int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm );
int EMPI_Reduce ( void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm );


// Send / receive.

int EMPI_Ibsend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );
int EMPI_Irecv ( void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *r );
int EMPI_Irsend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );
int EMPI_Isend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );
int EMPI_Rsend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm );
int EMPI_Send ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm );
int EMPI_Sendrecv ( void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status );
int EMPI_Ssend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm );
int EMPI_Recv(void *buf, int count, MPI_Datatype type, int source, int tag, MPI_Comm comm, MPI_Status *s);

// Request and status handling.
int EMPI_Wait ( MPI_Request *r, MPI_Status *status );
int EMPI_Waitall ( int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses );
int EMPI_Request_free ( MPI_Request *r );
int EMPI_Waitany(int count, MPI_Request *array_of_requests, int *index, MPI_Status *s);


// Datatypes
int EMPI_Type_free ( MPI_Datatype *type );
int EMPI_Type_get_envelope ( MPI_Datatype type, int *num_integers, int *num_addresses, int *num_datatypes, int *combiner );
int EMPI_Type_create_indexed_block ( int count, int blocklength, int array_of_displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype );
int EMPI_Type_contiguous ( int count, MPI_Datatype old_type, MPI_Datatype *new_type_p );
int EMPI_Type_commit ( MPI_Datatype *type );
int EMPI_Type_get_name ( MPI_Datatype type, char *type_name, int *resultlen );


// Files
int EMPI_File_set_view ( MPI_File MPI_fh, MPI_Offset disp, MPI_Datatype etype, MPI_Datatype filetype, char *datarep, MPI_Info info );
int EMPI_File_open ( MPI_Comm comm, char *filename, int amode, MPI_Info info, MPI_File *fh );
int EMPI_File_close ( MPI_File *mpi_fh );
int EMPI_File_write_at ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype type, MPI_Status *s );
int EMPI_File_read_at ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype type, MPI_Status *s );
int EMPI_File_read_all ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype type, MPI_Status *s );
int EMPI_File_write_all ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype type, MPI_Status *s );


// Info
int EMPI_Info_create ( MPI_Info *info );
int EMPI_Info_delete ( MPI_Info info, char *key );
int EMPI_Info_set ( MPI_Info info, char *key, char *value );
int EMPI_Info_free ( MPI_Info *info );


// Intercomm
int EMPI_Intercomm_create ( MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm );
int EMPI_Intercomm_merge ( MPI_Comm intercomm, int high, MPI_Comm *newintracomm );


// Datatypes
/*
int EMPI_Type_get_name ( MPI_Datatype datatype, char *type_name, int *resultlen );
int EMPI_Type_create_f90_complex ( int p, int r, MPI_Datatype *newtype );
int EMPI_Type_create_f90_integer ( int r, MPI_Datatype *newtype );
int EMPI_Type_create_f90_real ( int p, int r, MPI_Datatype *newtype );
int EMPI_Accumulate ( void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Op op, MPI_Win win );
int EMPI_Add_error_class ( int *errorclass );
int EMPI_Add_error_code ( int errorclass, int *errorcode );
int EMPI_Add_error_string ( int errorcode, char *string );
int EMPI_Address ( void *location, MPI_Aint *address );
int EMPI_Alloc_mem ( MPI_Aint size, MPI_Info info, void *baseptr );
int EMPI_Attr_delete ( MPI_Comm comm, int keyval );
int EMPI_Attr_get ( MPI_Comm comm, int keyval, void *attr_value, int *flag );
int EMPI_Attr_put ( MPI_Comm comm, int keyval, void *attr_value );
int EMPI_Bsend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm );
int EMPI_Bsend_init ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );
int EMPI_Buffer_attach ( void *buffer, int size );
int EMPI_Buffer_detach ( void *buffer, int *size );
int EMPI_Cancel ( MPI_Request *r );
int EMPI_Cart_coords ( MPI_Comm comm, int rank, int maxdims, int *coords );
int EMPI_Cart_create ( MPI_Comm comm_old, int ndims, int *dims, int *periods, int reorder, MPI_Comm *comm_cart );
int EMPI_Cartdim_get ( MPI_Comm comm, int *ndims );
int EMPI_Cart_get ( MPI_Comm comm, int maxdims, int *dims, int *periods, int *coords );
int EMPI_Cart_map ( MPI_Comm comm_old, int ndims, int *dims, int *periods, int *newrank );
int EMPI_Cart_rank ( MPI_Comm comm, int *coords, int *rank );
int EMPI_Cart_shift ( MPI_Comm comm, int direction, int displ, int *source, int *dest );
int EMPI_Cart_sub ( MPI_Comm comm, int *remain_dims, MPI_Comm *comm_new );
int EMPI_Close_port ( char *port_name );
int EMPI_Comm_accept ( char *port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *newcomm );
int EMPI_Comm_call_errhandler ( MPI_Comm comm, int errorcode );
int EMPI_Comm_compare ( MPI_Comm comm1, MPI_Comm comm2, int *result );
int EMPI_Comm_connect ( char *port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *newcomm );
int EMPI_Comm_create_errhandler ( MPI_Comm_errhandler_fn *function, MPI_Errhandler *errhandler );
int EMPI_Comm_create_keyval ( MPI_Comm_copy_attr_function *comm_copy_attr_fn, MPI_Comm_delete_attr_function *comm_delete_attr_fn, int *comm_keyval, void *extra_state );
int EMPI_Comm_delete_attr ( MPI_Comm comm, int comm_keyval );
int EMPI_Comm_disconnect ( MPI_Comm *comm );
int EMPI_Comm_free_keyval ( int *comm_keyval );
int EMPI_Comm_get_attr ( MPI_Comm comm, int comm_keyval, void *attribute_val, int *flag );
int EMPI_Comm_get_errhandler ( MPI_Comm comm, MPI_Errhandler *errhandler );
int EMPI_Comm_get_name ( MPI_Comm comm, char *comm_name, int *resultlen );
int EMPI_Comm_get_parent ( MPI_Comm *parent );
int EMPI_Comm_join ( int fd, MPI_Comm *intercomm );
int EMPI_Comm_remote_group ( MPI_Comm comm, MPI_Group *g );
int EMPI_Comm_remote_size ( MPI_Comm comm, int *size );
int EMPI_Comm_set_attr ( MPI_Comm comm, int comm_keyval, void *attribute_val );
int EMPI_Comm_set_errhandler ( MPI_Comm comm, MPI_Errhandler errhandler );
int EMPI_Comm_set_name ( MPI_Comm comm, char *comm_name );
int EMPI_Comm_spawn ( char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[] );
int EMPI_Comm_spawn_multiple ( int count, char *array_of_commands[], char* *array_of_argv[], int array_of_maxprocs[], MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[] );
int EMPI_Comm_test_inter ( MPI_Comm comm, int *flag );
int EMPI_Dims_create ( int nnodes, int ndims, int *dims );
int EMPI_Dist_graph_create_adjacent ( MPI_Comm comm_old, int indegree, int sources[], int sourceweights[], int outdegree, int destinations[], int destweights[], MPI_Info info, int reorder, MPI_Comm *comm_dist_graph );
int EMPI_Dist_graph_create ( MPI_Comm comm_old, int n, int sources[], int degrees[], int destinations[], int weights[], MPI_Info info, int reorder, MPI_Comm *comm_dist_graph );
int EMPI_Dist_graph_neighbors_count ( MPI_Comm comm, int *indegree, int *outdegree, int *weighted );
int EMPI_Dist_graph_neighbors ( MPI_Comm comm, int maxindegree, int sources[], int sourceweights[], int maxoutdegree, int destinations[], int destweights[] );
int EMPI_Errhandler_create ( MPI_Handler_function *function, MPI_Errhandler *errhandler );
int EMPI_Errhandler_free ( MPI_Errhandler *errhandler );
int EMPI_Errhandler_get ( MPI_Comm comm, MPI_Errhandler *errhandler );
int EMPI_Errhandler_set ( MPI_Comm comm, MPI_Errhandler errhandler );
int EMPI_Error_class ( int errorcode, int *errorclass );
int EMPI_Exscan ( void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm );
int EMPI_File_call_errhandler ( MPI_File fh, int errorcode );
int EMPI_File_close ( MPI_File *mpi_fh );
int EMPI_File_create_errhandler ( MPI_File_errhandler_fn *function, MPI_Errhandler *errhandler );
int EMPI_File_delete ( char *filename, MPI_Info info );
int EMPI_File_get_amode ( MPI_File MPI_fh, int *amode );
int EMPI_File_get_atomicity ( MPI_File MPI_fh, int *flag );
int EMPI_File_get_byte_offset ( MPI_File MPI_fh, MPI_Offset offset, MPI_Offset *disp );
int EMPI_File_get_errhandler ( MPI_File file, MPI_Errhandler *errhandler );
int EMPI_File_get_group ( MPI_File MPI_fh, MPI_Group *g );
int EMPI_File_get_info ( MPI_File MPI_fh, MPI_Info *info_used );
int EMPI_File_get_position ( MPI_File MPI_fh, MPI_Offset *offset );
int EMPI_File_get_position_shared ( MPI_File MPI_fh, MPI_Offset *offset );
int EMPI_File_get_size ( MPI_File MPI_fh, MPI_Offset *size );
int EMPI_File_get_type_extent ( MPI_File MPI_fh, MPI_Datatype datatype, MPI_Aint *extent );
int EMPI_File_get_view ( MPI_File MPI_fh, MPI_Offset *disp, MPI_Datatype *etype, MPI_Datatype *filetype, char *datarep );
int EMPI_File_iread_at ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPIO_Request *r );
int EMPI_File_iread ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *r );
int EMPI_File_iread_shared ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *r );
int EMPI_File_iwrite_at ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPIO_Request *r );
int EMPI_File_iwrite ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *r );
int EMPI_File_iwrite_shared ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPIO_Request *r );
int EMPI_File_open ( MPI_Comm comm, char *filename, int amode, MPI_Info info, MPI_File *fh );
int EMPI_File_preallocate ( MPI_File MPI_fh, MPI_Offset size );
int EMPI_File_read_all_begin ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype );
int EMPI_File_read_all_end ( MPI_File MPI_fh, void *buf, MPI_Status *status );
int EMPI_File_read_all ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );
int EMPI_File_read_at_all_begin ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype );
int EMPI_File_read_at_all_end ( MPI_File MPI_fh, void *buf, MPI_Status *status );
int EMPI_File_read_at_all ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );
int EMPI_File_read_at ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );
int EMPI_File_read ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );
int EMPI_File_read_ordered_begin ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype );
int EMPI_File_read_ordered_end ( MPI_File MPI_fh, void *buf, MPI_Status *status );
int EMPI_File_read_ordered ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );
int EMPI_File_read_shared ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );
int EMPI_File_seek ( MPI_File MPI_fh, MPI_Offset offset, int whence );
int EMPI_File_seek_shared ( MPI_File MPI_fh, MPI_Offset offset, int whence );
int EMPI_File_set_atomicity ( MPI_File MPI_fh, int flag );
int EMPI_File_set_errhandler ( MPI_File file, MPI_Errhandler errhandler );
int EMPI_File_set_info ( MPI_File MPI_fh, MPI_Info info );
int EMPI_File_set_size ( MPI_File MPI_fh, MPI_Offset size );
int EMPI_File_set_view ( MPI_File MPI_fh, MPI_Offset disp, MPI_Datatype etype, MPI_Datatype filetype, char *datarep, MPI_Info info );
int EMPI_File_sync ( MPI_File MPI_fh );
int EMPI_File_write_all_begin ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype );
int EMPI_File_write_all_end ( MPI_File MPI_fh, void *buf, MPI_Status *status );
int EMPI_File_write_all ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );
int EMPI_File_write_at_all_begin ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype );
int EMPI_File_write_at_all_end ( MPI_File MPI_fh, void *buf, MPI_Status *status );
int EMPI_File_write_at_all ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );
int EMPI_File_write_at ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );
int EMPI_File_write ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );
int EMPI_File_write_ordered_begin ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype );
int EMPI_File_write_ordered_end ( MPI_File MPI_fh, void *buf, MPI_Status *status );
int EMPI_File_write_ordered ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );
int EMPI_File_write_shared ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );
int EMPI_Free_mem ( void *base );
int EMPI_Get_address ( void *location, MPI_Aint *address );
int EMPI_Get_count ( MPI_Status *status, MPI_Datatype datatype, int *count );
int EMPI_Get_elements ( MPI_Status *status, MPI_Datatype datatype, int *elements );
int EMPI_Get ( void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win );
int EMPI_Get_version ( int *version, int *subversion );
int EMPI_Graph_create ( MPI_Comm comm_old, int nnodes, int *indx, int *edges, int reorder, MPI_Comm *comm_graph );
int EMPI_Graphdims_get ( MPI_Comm comm, int *nnodes, int *nedges );
int EMPI_Graph_get ( MPI_Comm comm, int maxindex, int maxedges, int *indx, int *edges );
int EMPI_Graph_map ( MPI_Comm comm_old, int nnodes, int *indx, int *edges, int *newrank );
int EMPI_Graph_neighbors_count ( MPI_Comm comm, int rank, int *nneighbors );
int EMPI_Graph_neighbors ( MPI_Comm comm, int rank, int maxneighbors, int *neighbors );
int EMPI_Grequest_complete ( MPI_Request r );
int EMPI_Grequest_start ( MPI_Grequest_query_function *query_fn, MPI_Grequest_free_function *free_fn, MPI_Grequest_cancel_function *cancel_fn, void *extra_state, MPI_Request *r );
int EMPI_Group_compare ( MPI_Group group1, MPI_Group group2, int *result );
int EMPI_Group_difference ( MPI_Group group1, MPI_Group group2, MPI_Group *newgroup );
int EMPI_Group_excl ( MPI_Group g, int n, int *ranks, MPI_Group *newgroup );
int EMPI_Group_free ( MPI_Group *g );
int EMPI_Group_incl ( MPI_Group g, int n, int *ranks, MPI_Group *newgroup );
int EMPI_Group_intersection ( MPI_Group group1, MPI_Group group2, MPI_Group *newgroup );
int EMPI_Group_range_excl ( MPI_Group g, int n, int ranges[][3], MPI_Group *newgroup );
int EMPI_Group_rank ( MPI_Group g, int *rank );
int EMPI_Group_size ( MPI_Group g, int *size );
int EMPI_Group_translate_ranks ( MPI_Group group1, int n, int *ranks1, MPI_Group group2, int *ranks2 );
int EMPI_Group_union ( MPI_Group group1, MPI_Group group2, MPI_Group *newgroup );
int EMPI_Info_create ( MPI_Info *info );
int EMPI_Info_delete ( MPI_Info info, char *key );
int EMPI_Info_dup ( MPI_Info info, MPI_Info *newinfo );
int EMPI_Info_free ( MPI_Info *info );
int EMPI_Info_get ( MPI_Info info, char *key, int valuelen, char *value, int *flag );
int EMPI_Info_get_nkeys ( MPI_Info info, int *nkeys );
int EMPI_Info_get_nthkey ( MPI_Info info, int n, char *key );
int EMPI_Info_get_valuelen ( MPI_Info info, char *key, int *valuelen, int *flag );
int EMPI_Info_set ( MPI_Info info, char *key, char *value );
int EMPI_Init_thread ( int *argc, char ***argv, int required, int *provided );
int EMPI_Intercomm_create ( MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm );
int EMPI_Intercomm_merge ( MPI_Comm intercomm, int high, MPI_Comm *newintracomm );
int EMPI_Iprobe ( int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status );
int EMPI_Issend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );
int EMPI_Is_thread_main ( int *flag );
int EMPI_Keyval_create ( MPI_Copy_function *copy_fn, MPI_Delete_function *delete_fn, int *keyval, void *extra_state );
int EMPI_Keyval_free ( int *keyval );
int EMPI_Lookup_name ( char *service_name, MPI_Info info, char *port_name );
int EMPI_Op_commutative ( MPI_Op op, int *commute );
int EMPI_Op_create ( MPI_User_function *function, int commute, MPI_Op *op );
int EMPI_Open_port ( MPI_Info info, char *port_name );
int EMPI_Op_free ( MPI_Op *op );
int EMPI_Pack_external ( char *datarep, void *inbuf, int incount, MPI_Datatype datatype, void *outbuf, MPI_Aint outcount, MPI_Aint *position );
int EMPI_Pack_external_size ( char *datarep, int incount, MPI_Datatype datatype, MPI_Aint *size );
int EMPI_Pack ( void *inbuf, int incount, MPI_Datatype datatype, void *outbuf, int outcount, int *position, MPI_Comm comm );
int EMPI_Pack_size ( int incount, MPI_Datatype datatype, MPI_Comm comm, int *size );
int EMPI_Probe ( int source, int tag, MPI_Comm comm, MPI_Status *status );
int EMPI_Publish_name ( char *service_name, MPI_Info info, char *port_name );
int EMPI_Put ( void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win );
int EMPI_Query_thread ( int *provided );
int EMPI_Recv ( void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status );
int EMPI_Recv_init ( void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *r );
int EMPI_Reduce_local ( void *inbuf, void *inoutbuf, int count, MPI_Datatype datatype, MPI_Op op );
int EMPI_Reduce_scatter_block ( void *sendbuf, void *recvbuf, int recvcount, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm );
int EMPI_Reduce_scatter ( void *sendbuf, void *recvbuf, int *recvcnts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm );
int EMPI_Register_datarep ( char *name, MPI_Datarep_conversion_function *read_conv_fn, MPI_Datarep_conversion_function *write_conv_fn, MPI_Datarep_extent_function *extent_fn, void *state );
int EMPI_Request_get_status ( MPI_Request r, int *flag, MPI_Status *status );
int EMPI_Rsend_init ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );
int EMPI_Scan ( void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm );
int EMPI_Send_init ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );
int EMPI_Sendrecv_replace ( void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status );
int EMPI_Ssend_init ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );
int EMPI_Startall ( int count, MPI_Request array_of_requests[] );
int EMPI_Start ( MPI_Request *r );
int EMPI_Status_set_cancelled ( MPI_Status *status, int flag );
int EMPI_Status_set_elements ( MPI_Status *status, MPI_Datatype datatype, int count );
int EMPI_Testall ( int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[] );
int EMPI_Testany ( int count, MPI_Request array_of_requests[], int *index, int *flag, MPI_Status *status );
int EMPI_Test_cancelled ( MPI_Status *status, int *flag );
int EMPI_Test ( MPI_Request *r, int *flag, MPI_Status *status );
int EMPI_Testsome ( int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[] );
int EMPI_Topo_test ( MPI_Comm comm, int *topo_type );
int EMPI_Type_commit ( MPI_Datatype *datatype );
int EMPI_Type_contiguous ( int count, MPI_Datatype old_type, MPI_Datatype *new_type_p );
int EMPI_Type_create_darray ( int size, int rank, int ndims, int array_of_gsizes[], int array_of_distribs[], int array_of_dargs[], int array_of_psizes[], int order, MPI_Datatype oldtype, MPI_Datatype *newtype );
int EMPI_Type_create_hindexed ( int count, int blocklengths[], MPI_Aint displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype );
int EMPI_Type_create_hvector ( int count, int blocklength, MPI_Aint stride, MPI_Datatype oldtype, MPI_Datatype *newtype );
int EMPI_Type_create_indexed_block ( int count, int blocklength, int array_of_displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype );
int EMPI_Type_create_keyval ( MPI_Type_copy_attr_function *type_copy_attr_fn, MPI_Type_delete_attr_function *type_delete_attr_fn, int *type_keyval, void *extra_state );
int EMPI_Type_create_resized ( MPI_Datatype oldtype, MPI_Aint lb, MPI_Aint extent, MPI_Datatype *newtype );
int EMPI_Type_create_struct ( int count, int array_of_blocklengths[], MPI_Aint array_of_displacements[], MPI_Datatype array_of_types[], MPI_Datatype *newtype );
int EMPI_Type_create_subarray ( int ndims, int array_of_sizes[], int array_of_subsizes[], int array_of_starts[], int order, MPI_Datatype oldtype, MPI_Datatype *newtype );
int EMPI_Type_delete_attr ( MPI_Datatype type, int type_keyval );
int EMPI_Type_dup ( MPI_Datatype datatype, MPI_Datatype *newtype );
int EMPI_Type_extent ( MPI_Datatype datatype, MPI_Aint *extent );
int EMPI_Type_free ( MPI_Datatype *datatype );
int EMPI_Type_free_keyval ( int *type_keyval );
int EMPI_Type_get_attr ( MPI_Datatype type, int type_keyval, void *attribute_val, int *flag );
int EMPI_Type_get_contents ( MPI_Datatype datatype, int max_integers, int max_addresses, int max_datatypes, int array_of_integers[], MPI_Aint array_of_addresses[], MPI_Datatype array_of_datatypes[] );
int EMPI_Type_get_envelope ( MPI_Datatype datatype, int *num_integers, int *num_addresses, int *num_datatypes, int *combiner );
int EMPI_Type_get_extent ( MPI_Datatype datatype, MPI_Aint *lb, MPI_Aint *extent );
int EMPI_Type_get_true_extent ( MPI_Datatype datatype, MPI_Aint *true_lb, MPI_Aint *true_extent );
int EMPI_Type_hindexed ( int count, int blocklens[], MPI_Aint indices[], MPI_Datatype old_type, MPI_Datatype *newtype );
int EMPI_Type_hvector ( int count, int blocklen, MPI_Aint stride, MPI_Datatype old_type, MPI_Datatype *newtype_p );
int EMPI_Type_indexed ( int count, int blocklens[], int indices[], MPI_Datatype old_type, MPI_Datatype *newtype );
int EMPI_Type_lb ( MPI_Datatype datatype, MPI_Aint *displacement );
int EMPI_Type_match_size ( int typeclass, int size, MPI_Datatype *datatype );
int EMPI_Type_set_attr ( MPI_Datatype type, int type_keyval, void *attribute_val );
int EMPI_Type_set_name ( MPI_Datatype type, char *type_name );
int EMPI_Type_size ( MPI_Datatype datatype, int *size );
int EMPI_Type_struct ( int count, int blocklens[], MPI_Aint indices[], MPI_Datatype old_types[], MPI_Datatype *newtype );
int EMPI_Type_ub ( MPI_Datatype datatype, MPI_Aint *displacement );
int EMPI_Type_vector ( int count, int blocklength, int stride, MPI_Datatype old_type, MPI_Datatype *newtype_p );
int EMPI_Unpack_external ( char *datarep, void *inbuf, MPI_Aint insize, MPI_Aint *position, void *outbuf, int outcount, MPI_Datatype datatype );
int EMPI_Unpack ( void *inbuf, int insize, int *position, void *outbuf, int outcount, MPI_Datatype datatype, MPI_Comm comm );
int EMPI_Unpublish_name ( char *service_name, MPI_Info info, char *port_name );
int EMPI_Waitany ( int count, MPI_Request array_of_requests[], int *index, MPI_Status *status );
int EMPI_Waitsome ( int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[] );
int EMPI_Win_call_errhandler ( MPI_Win win, int errorcode );
int EMPI_Win_complete ( MPI_Win win );
int EMPI_Win_create_errhandler ( MPI_Win_errhandler_fn *function, MPI_Errhandler *errhandler );
int EMPI_Win_create ( void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win );
int EMPI_Win_create_keyval ( MPI_Win_copy_attr_function *win_copy_attr_fn, MPI_Win_delete_attr_function *win_delete_attr_fn, int *win_keyval, void *extra_state );
int EMPI_Win_delete_attr ( MPI_Win win, int win_keyval );
int EMPI_Win_fence ( int assert, MPI_Win win );
int EMPI_Win_free ( MPI_Win *win );
int EMPI_Win_free_keyval ( int *win_keyval );
int EMPI_Win_get_attr ( MPI_Win win, int win_keyval, void *attribute_val, int *flag );
int EMPI_Win_get_errhandler ( MPI_Win win, MPI_Errhandler *errhandler );
int EMPI_Win_get_group ( MPI_Win win, MPI_Group *g );
int EMPI_Win_get_name ( MPI_Win win, char *win_name, int *resultlen );
int EMPI_Win_lock ( int lock_type, int rank, int assert, MPI_Win win );
int EMPI_Win_post ( MPI_Group g, int assert, MPI_Win win );
int EMPI_Win_set_attr ( MPI_Win win, int win_keyval, void *attribute_val );
int EMPI_Win_set_errhandler ( MPI_Win win, MPI_Errhandler errhandler );
int EMPI_Win_set_name ( MPI_Win win, char *win_name );
int EMPI_Win_start ( MPI_Group g, int assert, MPI_Win win );
int EMPI_Win_test ( MPI_Win win, int *flag );
int EMPI_Win_unlock ( int rank, MPI_Win win );
int EMPI_Win_wait ( MPI_Win win );
int EMPI_Group_comm_create ( MPI_Comm old_comm, MPI_Group g, int tag, MPI_Comm *new_comm );
MPI_Fint EMPI_Comm_c2f ( MPI_Comm comm );
MPI_Fint EMPI_Group_c2f ( MPI_Group g );
MPI_Fint EMPI_Request_c2f ( MPI_Request r );
MPI_Fint EMPI_Info_c2f ( MPI_Info info );
MPI_Fint EMPI_File_c2f ( MPI_File file );
MPI_Fint EMPI_Op_c2f ( MPI_Op op );
MPI_Fint EMPI_Win_c2f ( MPI_Win Win );
MPI_Fint EMPI_Errhandler_c2f ( MPI_Errhandler Errhandler );
MPI_Fint EMPI_Type_c2f ( MPI_Datatype Type );
*/

#endif // __EMPI_H_
