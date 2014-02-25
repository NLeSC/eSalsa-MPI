#ifndef __xEMPI_H_
#define __xEMPI_H_

#include "mpi.h"

// Misc. utility functions

int xEMPI_Init ( int *argc, char ***argv );
int xEMPI_Initialized ( int *flag );

int xEMPI_Finalized ( int *flag );
int xEMPI_Finalize ( void );

int xEMPI_Abort ( MPI_Comm comm, int errorcode );

int xEMPI_Get_processor_name ( char *name, int *resultlen );
int xEMPI_Error_string ( int errorcode, char *string, int *resultlen );

double xEMPI_Wtime(void);


// Communicators and groups.

int xEMPI_Comm_create ( MPI_Comm comm, MPI_Group g, MPI_Comm *newcomm );
int xEMPI_Comm_dup ( MPI_Comm comm, MPI_Comm *newcomm );
int xEMPI_Comm_free ( MPI_Comm *comm );
int xEMPI_Comm_group ( MPI_Comm comm, MPI_Group *g );
int xEMPI_Comm_rank ( MPI_Comm comm, int *rank );
int xEMPI_Comm_size ( MPI_Comm comm, int *size );
int xEMPI_Comm_split ( MPI_Comm comm, int color, int key, MPI_Comm *newcomm );

int xEMPI_Group_range_incl ( MPI_Group g, int n, int ranges[][3], MPI_Group *newgroup );
int xEMPI_Group_range_excl ( MPI_Group g, int n, int ranges[][3], MPI_Group *newgroup );

int xEMPI_Group_translate_ranks ( MPI_Group group1, int n, int *ranks1, MPI_Group group2, int *ranks2 );
int xEMPI_Group_union ( MPI_Group group1, MPI_Group group2, MPI_Group *newgroup );
int xEMPI_Group_incl ( MPI_Group g, int n, int *ranks, MPI_Group *newgroup );

// Collectives

int xEMPI_Allgather ( void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm );
int xEMPI_Allgatherv ( void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int *recvcounts, int *displs, MPI_Datatype recvtype, MPI_Comm comm );
int xEMPI_Allreduce ( void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm );
int xEMPI_Alltoall ( void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm );
int xEMPI_Alltoallv ( void *sendbuf, int *sendcnts, int *sdispls, MPI_Datatype sendtype, void *recvbuf, int *recvcnts, int *rdispls, MPI_Datatype recvtype, MPI_Comm comm );
int xEMPI_Alltoallw ( void *sendbuf, int *sendcnts, int *sdispls, MPI_Datatype *sendtypes, void *recvbuf, int *recvcnts, int *rdispls, MPI_Datatype *recvtypes, MPI_Comm comm );
int xEMPI_Scatter ( void *sendbuf, int sendcnt, MPI_Datatype sendtype, void *recvbuf, int recvcnt, MPI_Datatype recvtype, int root, MPI_Comm comm );
int xEMPI_Scatterv ( void *sendbuf, int *sendcnts, int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcnt, MPI_Datatype recvtype, int root, MPI_Comm comm );
int xEMPI_Barrier ( MPI_Comm comm );
int xEMPI_Bcast ( void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm );
int xEMPI_Gather ( void *sendbuf, int sendcnt, MPI_Datatype sendtype, void *recvbuf, int recvcnt, MPI_Datatype recvtype, int root, MPI_Comm comm );
int xEMPI_Gatherv ( void *sendbuf, int sendcnt, MPI_Datatype sendtype, void *recvbuf, int *recvcnts, int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm );
int xEMPI_Reduce ( void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm );


// Send / receive.

int xEMPI_Ibsend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );
int xEMPI_Irecv ( void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *r );
int xEMPI_Irsend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );
int xEMPI_Isend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );
int xEMPI_Rsend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm );
int xEMPI_Send ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm );
int xEMPI_Sendrecv ( void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status );
int xEMPI_Ssend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm );
int xEMPI_Recv(void *buf, int count, MPI_Datatype type, int source, int tag, MPI_Comm comm, MPI_Status *s);

// Request and status handling.
int xEMPI_Wait ( MPI_Request *r, MPI_Status *status );
int xEMPI_Waitall ( int count, MPI_Request *array_of_requests, MPI_Status *array_of_statuses );
int xEMPI_Request_free ( MPI_Request *r );
int xEMPI_Waitany(int count, MPI_Request *array_of_requests, int *index, MPI_Status *s);


// Datatypes
int xEMPI_Type_free ( MPI_Datatype *type );
int xEMPI_Type_get_envelope ( MPI_Datatype type, int *num_integers, int *num_addresses, int *num_datatypes, int *combiner );
int xEMPI_Type_create_indexed_block ( int count, int blocklength, int array_of_displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype );
int xEMPI_Type_contiguous ( int count, MPI_Datatype old_type, MPI_Datatype *new_type_p );
int xEMPI_Type_commit ( MPI_Datatype *type );
int xEMPI_Type_get_name ( MPI_Datatype type, char *type_name, int *resultlen );


// Files
int xEMPI_File_set_view ( MPI_File MPI_fh, MPI_Offset disp, MPI_Datatype etype, MPI_Datatype filetype, char *datarep, MPI_Info info );
int xEMPI_File_open ( MPI_Comm comm, char *filename, int amode, MPI_Info info, MPI_File *fh );
int xEMPI_File_close ( MPI_File *mpi_fh );
int xEMPI_File_write_at ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype type, MPI_Status *s );
int xEMPI_File_read_at ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype type, MPI_Status *s );
int xEMPI_File_read_all ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype type, MPI_Status *s );
int xEMPI_File_write_all ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype type, MPI_Status *s );


// Info
int xEMPI_Info_create ( MPI_Info *info );
int xEMPI_Info_delete ( MPI_Info info, char *key );
int xEMPI_Info_set ( MPI_Info info, char *key, char *value );
int xEMPI_Info_free ( MPI_Info *info );


// Intercomm
int xEMPI_Intercomm_create ( MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm );
int xEMPI_Intercomm_merge ( MPI_Comm intercomm, int high, MPI_Comm *newintracomm );


// Datatypes
/*
int xEMPI_Type_get_name ( MPI_Datatype datatype, char *type_name, int *resultlen );
int xEMPI_Type_create_f90_complex ( int p, int r, MPI_Datatype *newtype );
int xEMPI_Type_create_f90_integer ( int r, MPI_Datatype *newtype );
int xEMPI_Type_create_f90_real ( int p, int r, MPI_Datatype *newtype );
int xEMPI_Accumulate ( void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Op op, MPI_Win win );
int xEMPI_Add_error_class ( int *errorclass );
int xEMPI_Add_error_code ( int errorclass, int *errorcode );
int xEMPI_Add_error_string ( int errorcode, char *string );
int xEMPI_Address ( void *location, MPI_Aint *address );
int xEMPI_Alloc_mem ( MPI_Aint size, MPI_Info info, void *baseptr );
int xEMPI_Attr_delete ( MPI_Comm comm, int keyval );
int xEMPI_Attr_get ( MPI_Comm comm, int keyval, void *attr_value, int *flag );
int xEMPI_Attr_put ( MPI_Comm comm, int keyval, void *attr_value );
int xEMPI_Bsend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm );
int xEMPI_Bsend_init ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );
int xEMPI_Buffer_attach ( void *buffer, int size );
int xEMPI_Buffer_detach ( void *buffer, int *size );
int xEMPI_Cancel ( MPI_Request *r );
int xEMPI_Cart_coords ( MPI_Comm comm, int rank, int maxdims, int *coords );
int xEMPI_Cart_create ( MPI_Comm comm_old, int ndims, int *dims, int *periods, int reorder, MPI_Comm *comm_cart );
int xEMPI_Cartdim_get ( MPI_Comm comm, int *ndims );
int xEMPI_Cart_get ( MPI_Comm comm, int maxdims, int *dims, int *periods, int *coords );
int xEMPI_Cart_map ( MPI_Comm comm_old, int ndims, int *dims, int *periods, int *newrank );
int xEMPI_Cart_rank ( MPI_Comm comm, int *coords, int *rank );
int xEMPI_Cart_shift ( MPI_Comm comm, int direction, int displ, int *source, int *dest );
int xEMPI_Cart_sub ( MPI_Comm comm, int *remain_dims, MPI_Comm *comm_new );
int xEMPI_Close_port ( char *port_name );
int xEMPI_Comm_accept ( char *port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *newcomm );
int xEMPI_Comm_call_errhandler ( MPI_Comm comm, int errorcode );
int xEMPI_Comm_compare ( MPI_Comm comm1, MPI_Comm comm2, int *result );
int xEMPI_Comm_connect ( char *port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *newcomm );
int xEMPI_Comm_create_errhandler ( MPI_Comm_errhandler_fn *function, MPI_Errhandler *errhandler );
int xEMPI_Comm_create_keyval ( MPI_Comm_copy_attr_function *comm_copy_attr_fn, MPI_Comm_delete_attr_function *comm_delete_attr_fn, int *comm_keyval, void *extra_state );
int xEMPI_Comm_delete_attr ( MPI_Comm comm, int comm_keyval );
int xEMPI_Comm_disconnect ( MPI_Comm *comm );
int xEMPI_Comm_free_keyval ( int *comm_keyval );
int xEMPI_Comm_get_attr ( MPI_Comm comm, int comm_keyval, void *attribute_val, int *flag );
int xEMPI_Comm_get_errhandler ( MPI_Comm comm, MPI_Errhandler *errhandler );
int xEMPI_Comm_get_name ( MPI_Comm comm, char *comm_name, int *resultlen );
int xEMPI_Comm_get_parent ( MPI_Comm *parent );
int xEMPI_Comm_join ( int fd, MPI_Comm *intercomm );
int xEMPI_Comm_remote_group ( MPI_Comm comm, MPI_Group *g );
int xEMPI_Comm_remote_size ( MPI_Comm comm, int *size );
int xEMPI_Comm_set_attr ( MPI_Comm comm, int comm_keyval, void *attribute_val );
int xEMPI_Comm_set_errhandler ( MPI_Comm comm, MPI_Errhandler errhandler );
int xEMPI_Comm_set_name ( MPI_Comm comm, char *comm_name );
int xEMPI_Comm_spawn ( char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[] );
int xEMPI_Comm_spawn_multiple ( int count, char *array_of_commands[], char* *array_of_argv[], int array_of_maxprocs[], MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[] );
int xEMPI_Comm_test_inter ( MPI_Comm comm, int *flag );
int xEMPI_Dims_create ( int nnodes, int ndims, int *dims );
int xEMPI_Dist_graph_create_adjacent ( MPI_Comm comm_old, int indegree, int sources[], int sourceweights[], int outdegree, int destinations[], int destweights[], MPI_Info info, int reorder, MPI_Comm *comm_dist_graph );
int xEMPI_Dist_graph_create ( MPI_Comm comm_old, int n, int sources[], int degrees[], int destinations[], int weights[], MPI_Info info, int reorder, MPI_Comm *comm_dist_graph );
int xEMPI_Dist_graph_neighbors_count ( MPI_Comm comm, int *indegree, int *outdegree, int *weighted );
int xEMPI_Dist_graph_neighbors ( MPI_Comm comm, int maxindegree, int sources[], int sourceweights[], int maxoutdegree, int destinations[], int destweights[] );
int xEMPI_Errhandler_create ( MPI_Handler_function *function, MPI_Errhandler *errhandler );
int xEMPI_Errhandler_free ( MPI_Errhandler *errhandler );
int xEMPI_Errhandler_get ( MPI_Comm comm, MPI_Errhandler *errhandler );
int xEMPI_Errhandler_set ( MPI_Comm comm, MPI_Errhandler errhandler );
int xEMPI_Error_class ( int errorcode, int *errorclass );
int xEMPI_Exscan ( void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm );
int xEMPI_File_call_errhandler ( MPI_File fh, int errorcode );
int xEMPI_File_close ( MPI_File *mpi_fh );
int xEMPI_File_create_errhandler ( MPI_File_errhandler_fn *function, MPI_Errhandler *errhandler );
int xEMPI_File_delete ( char *filename, MPI_Info info );
int xEMPI_File_get_amode ( MPI_File MPI_fh, int *amode );
int xEMPI_File_get_atomicity ( MPI_File MPI_fh, int *flag );
int xEMPI_File_get_byte_offset ( MPI_File MPI_fh, MPI_Offset offset, MPI_Offset *disp );
int xEMPI_File_get_errhandler ( MPI_File file, MPI_Errhandler *errhandler );
int xEMPI_File_get_group ( MPI_File MPI_fh, MPI_Group *g );
int xEMPI_File_get_info ( MPI_File MPI_fh, MPI_Info *info_used );
int xEMPI_File_get_position ( MPI_File MPI_fh, MPI_Offset *offset );
int xEMPI_File_get_position_shared ( MPI_File MPI_fh, MPI_Offset *offset );
int xEMPI_File_get_size ( MPI_File MPI_fh, MPI_Offset *size );
int xEMPI_File_get_type_extent ( MPI_File MPI_fh, MPI_Datatype datatype, MPI_Aint *extent );
int xEMPI_File_get_view ( MPI_File MPI_fh, MPI_Offset *disp, MPI_Datatype *etype, MPI_Datatype *filetype, char *datarep );
int xEMPI_File_iread_at ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPIO_Request *r );
int xEMPI_File_iread ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *r );
int xEMPI_File_iread_shared ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *r );
int xEMPI_File_iwrite_at ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPIO_Request *r );
int xEMPI_File_iwrite ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *r );
int xEMPI_File_iwrite_shared ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPIO_Request *r );
int xEMPI_File_open ( MPI_Comm comm, char *filename, int amode, MPI_Info info, MPI_File *fh );
int xEMPI_File_preallocate ( MPI_File MPI_fh, MPI_Offset size );
int xEMPI_File_read_all_begin ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype );
int xEMPI_File_read_all_end ( MPI_File MPI_fh, void *buf, MPI_Status *status );
int xEMPI_File_read_all ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );
int xEMPI_File_read_at_all_begin ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype );
int xEMPI_File_read_at_all_end ( MPI_File MPI_fh, void *buf, MPI_Status *status );
int xEMPI_File_read_at_all ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );
int xEMPI_File_read_at ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );
int xEMPI_File_read ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );
int xEMPI_File_read_ordered_begin ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype );
int xEMPI_File_read_ordered_end ( MPI_File MPI_fh, void *buf, MPI_Status *status );
int xEMPI_File_read_ordered ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );
int xEMPI_File_read_shared ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );
int xEMPI_File_seek ( MPI_File MPI_fh, MPI_Offset offset, int whence );
int xEMPI_File_seek_shared ( MPI_File MPI_fh, MPI_Offset offset, int whence );
int xEMPI_File_set_atomicity ( MPI_File MPI_fh, int flag );
int xEMPI_File_set_errhandler ( MPI_File file, MPI_Errhandler errhandler );
int xEMPI_File_set_info ( MPI_File MPI_fh, MPI_Info info );
int xEMPI_File_set_size ( MPI_File MPI_fh, MPI_Offset size );
int xEMPI_File_set_view ( MPI_File MPI_fh, MPI_Offset disp, MPI_Datatype etype, MPI_Datatype filetype, char *datarep, MPI_Info info );
int xEMPI_File_sync ( MPI_File MPI_fh );
int xEMPI_File_write_all_begin ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype );
int xEMPI_File_write_all_end ( MPI_File MPI_fh, void *buf, MPI_Status *status );
int xEMPI_File_write_all ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );
int xEMPI_File_write_at_all_begin ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype );
int xEMPI_File_write_at_all_end ( MPI_File MPI_fh, void *buf, MPI_Status *status );
int xEMPI_File_write_at_all ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );
int xEMPI_File_write_at ( MPI_File MPI_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );
int xEMPI_File_write ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );
int xEMPI_File_write_ordered_begin ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype );
int xEMPI_File_write_ordered_end ( MPI_File MPI_fh, void *buf, MPI_Status *status );
int xEMPI_File_write_ordered ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );
int xEMPI_File_write_shared ( MPI_File MPI_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );
int xEMPI_Free_mem ( void *base );
int xEMPI_Get_address ( void *location, MPI_Aint *address );
int xEMPI_Get_count ( MPI_Status *status, MPI_Datatype datatype, int *count );
int xEMPI_Get_elements ( MPI_Status *status, MPI_Datatype datatype, int *elements );
int xEMPI_Get ( void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win );
int xEMPI_Get_version ( int *version, int *subversion );
int xEMPI_Graph_create ( MPI_Comm comm_old, int nnodes, int *indx, int *edges, int reorder, MPI_Comm *comm_graph );
int xEMPI_Graphdims_get ( MPI_Comm comm, int *nnodes, int *nedges );
int xEMPI_Graph_get ( MPI_Comm comm, int maxindex, int maxedges, int *indx, int *edges );
int xEMPI_Graph_map ( MPI_Comm comm_old, int nnodes, int *indx, int *edges, int *newrank );
int xEMPI_Graph_neighbors_count ( MPI_Comm comm, int rank, int *nneighbors );
int xEMPI_Graph_neighbors ( MPI_Comm comm, int rank, int maxneighbors, int *neighbors );
int xEMPI_Grequest_complete ( MPI_Request r );
int xEMPI_Grequest_start ( MPI_Grequest_query_function *query_fn, MPI_Grequest_free_function *free_fn, MPI_Grequest_cancel_function *cancel_fn, void *extra_state, MPI_Request *r );
int xEMPI_Group_compare ( MPI_Group group1, MPI_Group group2, int *result );
int xEMPI_Group_difference ( MPI_Group group1, MPI_Group group2, MPI_Group *newgroup );
int xEMPI_Group_excl ( MPI_Group g, int n, int *ranks, MPI_Group *newgroup );
int xEMPI_Group_free ( MPI_Group *g );
int xEMPI_Group_incl ( MPI_Group g, int n, int *ranks, MPI_Group *newgroup );
int xEMPI_Group_intersection ( MPI_Group group1, MPI_Group group2, MPI_Group *newgroup );
int xEMPI_Group_range_excl ( MPI_Group g, int n, int ranges[][3], MPI_Group *newgroup );
int xEMPI_Group_rank ( MPI_Group g, int *rank );
int xEMPI_Group_size ( MPI_Group g, int *size );
int xEMPI_Group_translate_ranks ( MPI_Group group1, int n, int *ranks1, MPI_Group group2, int *ranks2 );
int xEMPI_Group_union ( MPI_Group group1, MPI_Group group2, MPI_Group *newgroup );
int xEMPI_Info_create ( MPI_Info *info );
int xEMPI_Info_delete ( MPI_Info info, char *key );
int xEMPI_Info_dup ( MPI_Info info, MPI_Info *newinfo );
int xEMPI_Info_free ( MPI_Info *info );
int xEMPI_Info_get ( MPI_Info info, char *key, int valuelen, char *value, int *flag );
int xEMPI_Info_get_nkeys ( MPI_Info info, int *nkeys );
int xEMPI_Info_get_nthkey ( MPI_Info info, int n, char *key );
int xEMPI_Info_get_valuelen ( MPI_Info info, char *key, int *valuelen, int *flag );
int xEMPI_Info_set ( MPI_Info info, char *key, char *value );
int xEMPI_Init_thread ( int *argc, char ***argv, int required, int *provided );
int xEMPI_Intercomm_create ( MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm );
int xEMPI_Intercomm_merge ( MPI_Comm intercomm, int high, MPI_Comm *newintracomm );
int xEMPI_Iprobe ( int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status );
int xEMPI_Issend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );
int xEMPI_Is_thread_main ( int *flag );
int xEMPI_Keyval_create ( MPI_Copy_function *copy_fn, MPI_Delete_function *delete_fn, int *keyval, void *extra_state );
int xEMPI_Keyval_free ( int *keyval );
int xEMPI_Lookup_name ( char *service_name, MPI_Info info, char *port_name );
int xEMPI_Op_commutative ( MPI_Op op, int *commute );
int xEMPI_Op_create ( MPI_User_function *function, int commute, MPI_Op *op );
int xEMPI_Open_port ( MPI_Info info, char *port_name );
int xEMPI_Op_free ( MPI_Op *op );
int xEMPI_Pack_external ( char *datarep, void *inbuf, int incount, MPI_Datatype datatype, void *outbuf, MPI_Aint outcount, MPI_Aint *position );
int xEMPI_Pack_external_size ( char *datarep, int incount, MPI_Datatype datatype, MPI_Aint *size );
int xEMPI_Pack ( void *inbuf, int incount, MPI_Datatype datatype, void *outbuf, int outcount, int *position, MPI_Comm comm );
int xEMPI_Pack_size ( int incount, MPI_Datatype datatype, MPI_Comm comm, int *size );
int xEMPI_Probe ( int source, int tag, MPI_Comm comm, MPI_Status *status );
int xEMPI_Publish_name ( char *service_name, MPI_Info info, char *port_name );
int xEMPI_Put ( void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win );
int xEMPI_Query_thread ( int *provided );
int xEMPI_Recv ( void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status );
int xEMPI_Recv_init ( void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *r );
int xEMPI_Reduce_local ( void *inbuf, void *inoutbuf, int count, MPI_Datatype datatype, MPI_Op op );
int xEMPI_Reduce_scatter_block ( void *sendbuf, void *recvbuf, int recvcount, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm );
int xEMPI_Reduce_scatter ( void *sendbuf, void *recvbuf, int *recvcnts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm );
int xEMPI_Register_datarep ( char *name, MPI_Datarep_conversion_function *read_conv_fn, MPI_Datarep_conversion_function *write_conv_fn, MPI_Datarep_extent_function *extent_fn, void *state );
int xEMPI_Request_get_status ( MPI_Request r, int *flag, MPI_Status *status );
int xEMPI_Rsend_init ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );
int xEMPI_Scan ( void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm );
int xEMPI_Send_init ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );
int xEMPI_Sendrecv_replace ( void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status );
int xEMPI_Ssend_init ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );
int xEMPI_Startall ( int count, MPI_Request array_of_requests[] );
int xEMPI_Start ( MPI_Request *r );
int xEMPI_Status_set_cancelled ( MPI_Status *status, int flag );
int xEMPI_Status_set_elements ( MPI_Status *status, MPI_Datatype datatype, int count );
int xEMPI_Testall ( int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[] );
int xEMPI_Testany ( int count, MPI_Request array_of_requests[], int *index, int *flag, MPI_Status *status );
int xEMPI_Test_cancelled ( MPI_Status *status, int *flag );
int xEMPI_Test ( MPI_Request *r, int *flag, MPI_Status *status );
int xEMPI_Testsome ( int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[] );
int xEMPI_Topo_test ( MPI_Comm comm, int *topo_type );
int xEMPI_Type_commit ( MPI_Datatype *datatype );
int xEMPI_Type_contiguous ( int count, MPI_Datatype old_type, MPI_Datatype *new_type_p );
int xEMPI_Type_create_darray ( int size, int rank, int ndims, int array_of_gsizes[], int array_of_distribs[], int array_of_dargs[], int array_of_psizes[], int order, MPI_Datatype oldtype, MPI_Datatype *newtype );
int xEMPI_Type_create_hindexed ( int count, int blocklengths[], MPI_Aint displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype );
int xEMPI_Type_create_hvector ( int count, int blocklength, MPI_Aint stride, MPI_Datatype oldtype, MPI_Datatype *newtype );
int xEMPI_Type_create_indexed_block ( int count, int blocklength, int array_of_displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype );
int xEMPI_Type_create_keyval ( MPI_Type_copy_attr_function *type_copy_attr_fn, MPI_Type_delete_attr_function *type_delete_attr_fn, int *type_keyval, void *extra_state );
int xEMPI_Type_create_resized ( MPI_Datatype oldtype, MPI_Aint lb, MPI_Aint extent, MPI_Datatype *newtype );
int xEMPI_Type_create_struct ( int count, int array_of_blocklengths[], MPI_Aint array_of_displacements[], MPI_Datatype array_of_types[], MPI_Datatype *newtype );
int xEMPI_Type_create_subarray ( int ndims, int array_of_sizes[], int array_of_subsizes[], int array_of_starts[], int order, MPI_Datatype oldtype, MPI_Datatype *newtype );
int xEMPI_Type_delete_attr ( MPI_Datatype type, int type_keyval );
int xEMPI_Type_dup ( MPI_Datatype datatype, MPI_Datatype *newtype );
int xEMPI_Type_extent ( MPI_Datatype datatype, MPI_Aint *extent );
int xEMPI_Type_free ( MPI_Datatype *datatype );
int xEMPI_Type_free_keyval ( int *type_keyval );
int xEMPI_Type_get_attr ( MPI_Datatype type, int type_keyval, void *attribute_val, int *flag );
int xEMPI_Type_get_contents ( MPI_Datatype datatype, int max_integers, int max_addresses, int max_datatypes, int array_of_integers[], MPI_Aint array_of_addresses[], MPI_Datatype array_of_datatypes[] );
int xEMPI_Type_get_envelope ( MPI_Datatype datatype, int *num_integers, int *num_addresses, int *num_datatypes, int *combiner );
int xEMPI_Type_get_extent ( MPI_Datatype datatype, MPI_Aint *lb, MPI_Aint *extent );
int xEMPI_Type_get_true_extent ( MPI_Datatype datatype, MPI_Aint *true_lb, MPI_Aint *true_extent );
int xEMPI_Type_hindexed ( int count, int blocklens[], MPI_Aint indices[], MPI_Datatype old_type, MPI_Datatype *newtype );
int xEMPI_Type_hvector ( int count, int blocklen, MPI_Aint stride, MPI_Datatype old_type, MPI_Datatype *newtype_p );
int xEMPI_Type_indexed ( int count, int blocklens[], int indices[], MPI_Datatype old_type, MPI_Datatype *newtype );
int xEMPI_Type_lb ( MPI_Datatype datatype, MPI_Aint *displacement );
int xEMPI_Type_match_size ( int typeclass, int size, MPI_Datatype *datatype );
int xEMPI_Type_set_attr ( MPI_Datatype type, int type_keyval, void *attribute_val );
int xEMPI_Type_set_name ( MPI_Datatype type, char *type_name );
int xEMPI_Type_size ( MPI_Datatype datatype, int *size );
int xEMPI_Type_struct ( int count, int blocklens[], MPI_Aint indices[], MPI_Datatype old_types[], MPI_Datatype *newtype );
int xEMPI_Type_ub ( MPI_Datatype datatype, MPI_Aint *displacement );
int xEMPI_Type_vector ( int count, int blocklength, int stride, MPI_Datatype old_type, MPI_Datatype *newtype_p );
int xEMPI_Unpack_external ( char *datarep, void *inbuf, MPI_Aint insize, MPI_Aint *position, void *outbuf, int outcount, MPI_Datatype datatype );
int xEMPI_Unpack ( void *inbuf, int insize, int *position, void *outbuf, int outcount, MPI_Datatype datatype, MPI_Comm comm );
int xEMPI_Unpublish_name ( char *service_name, MPI_Info info, char *port_name );
int xEMPI_Waitany ( int count, MPI_Request array_of_requests[], int *index, MPI_Status *status );
int xEMPI_Waitsome ( int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[] );
int xEMPI_Win_call_errhandler ( MPI_Win win, int errorcode );
int xEMPI_Win_complete ( MPI_Win win );
int xEMPI_Win_create_errhandler ( MPI_Win_errhandler_fn *function, MPI_Errhandler *errhandler );
int xEMPI_Win_create ( void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win );
int xEMPI_Win_create_keyval ( MPI_Win_copy_attr_function *win_copy_attr_fn, MPI_Win_delete_attr_function *win_delete_attr_fn, int *win_keyval, void *extra_state );
int xEMPI_Win_delete_attr ( MPI_Win win, int win_keyval );
int xEMPI_Win_fence ( int assert, MPI_Win win );
int xEMPI_Win_free ( MPI_Win *win );
int xEMPI_Win_free_keyval ( int *win_keyval );
int xEMPI_Win_get_attr ( MPI_Win win, int win_keyval, void *attribute_val, int *flag );
int xEMPI_Win_get_errhandler ( MPI_Win win, MPI_Errhandler *errhandler );
int xEMPI_Win_get_group ( MPI_Win win, MPI_Group *g );
int xEMPI_Win_get_name ( MPI_Win win, char *win_name, int *resultlen );
int xEMPI_Win_lock ( int lock_type, int rank, int assert, MPI_Win win );
int xEMPI_Win_post ( MPI_Group g, int assert, MPI_Win win );
int xEMPI_Win_set_attr ( MPI_Win win, int win_keyval, void *attribute_val );
int xEMPI_Win_set_errhandler ( MPI_Win win, MPI_Errhandler errhandler );
int xEMPI_Win_set_name ( MPI_Win win, char *win_name );
int xEMPI_Win_start ( MPI_Group g, int assert, MPI_Win win );
int xEMPI_Win_test ( MPI_Win win, int *flag );
int xEMPI_Win_unlock ( int rank, MPI_Win win );
int xEMPI_Win_wait ( MPI_Win win );
int xEMPI_Group_comm_create ( MPI_Comm old_comm, MPI_Group g, int tag, MPI_Comm *new_comm );
MPI_Fint xEMPI_Comm_c2f ( MPI_Comm comm );
MPI_Fint xEMPI_Group_c2f ( MPI_Group g );
MPI_Fint xEMPI_Request_c2f ( MPI_Request r );
MPI_Fint xEMPI_Info_c2f ( MPI_Info info );
MPI_Fint xEMPI_File_c2f ( MPI_File file );
MPI_Fint xEMPI_Op_c2f ( MPI_Op op );
MPI_Fint xEMPI_Win_c2f ( MPI_Win Win );
MPI_Fint xEMPI_Errhandler_c2f ( MPI_Errhandler Errhandler );
MPI_Fint xEMPI_Type_c2f ( MPI_Datatype Type );
*/

#endif // __xEMPI_H_
