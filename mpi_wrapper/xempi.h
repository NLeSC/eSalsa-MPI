#ifndef __XEMPI_H_
#define __XEMPI_H_

#include "empi.h"

// Misc. utility functions

int xEMPI_Init ( int *argc, char ***argv );
int xEMPI_Initialized ( int *flag );

int xEMPI_Finalized ( int *flag );
int xEMPI_Finalize ( void );

int xEMPI_Abort ( EMPI_Comm comm, int errorcode );

int xEMPI_Get_processor_name ( char *name, int *resultlen );
int xEMPI_Error_string ( int errorcode, char *string, int *resultlen );

double xEMPI_Wtime(void);


// Communicators and groups.

int xEMPI_Comm_create ( EMPI_Comm comm, EMPI_Group g, EMPI_Comm *newcomm );
int xEMPI_Comm_dup ( EMPI_Comm comm, EMPI_Comm *newcomm );
int xEMPI_Comm_free ( EMPI_Comm *comm );
int xEMPI_Comm_group ( EMPI_Comm comm, EMPI_Group *g );
int xEMPI_Comm_rank ( EMPI_Comm comm, int *rank );
int xEMPI_Comm_size ( EMPI_Comm comm, int *size );
int xEMPI_Comm_split ( EMPI_Comm comm, int color, int key, EMPI_Comm *newcomm );

int xEMPI_Group_range_incl ( EMPI_Group g, int n, int ranges[][3], EMPI_Group *newgroup );
int xEMPI_Group_range_excl ( EMPI_Group g, int n, int ranges[][3], EMPI_Group *newgroup );

int xEMPI_Group_translate_ranks ( EMPI_Group group1, int n, int *ranks1, EMPI_Group group2, int *ranks2 );
int xEMPI_Group_union ( EMPI_Group group1, EMPI_Group group2, EMPI_Group *newgroup );
int xEMPI_Group_incl ( EMPI_Group g, int n, int *ranks, EMPI_Group *newgroup );

// Collectives

int xEMPI_Allgather ( void *sendbuf, int sendcount, EMPI_Datatype sendtype, void *recvbuf, int recvcount, EMPI_Datatype recvtype, EMPI_Comm comm );
int xEMPI_Allgatherv ( void *sendbuf, int sendcount, EMPI_Datatype sendtype, void *recvbuf, int *recvcounts, int *displs, EMPI_Datatype recvtype, EMPI_Comm comm );
int xEMPI_Allreduce ( void *sendbuf, void *recvbuf, int count, EMPI_Datatype datatype, EMPI_Op op, EMPI_Comm comm );
int xEMPI_Alltoall ( void *sendbuf, int sendcount, EMPI_Datatype sendtype, void *recvbuf, int recvcount, EMPI_Datatype recvtype, EMPI_Comm comm );
int xEMPI_Alltoallv ( void *sendbuf, int *sendcnts, int *sdispls, EMPI_Datatype sendtype, void *recvbuf, int *recvcnts, int *rdispls, EMPI_Datatype recvtype, EMPI_Comm comm );
int xEMPI_Alltoallw ( void *sendbuf, int *sendcnts, int *sdispls, EMPI_Datatype *sendtypes, void *recvbuf, int *recvcnts, int *rdispls, EMPI_Datatype *recvtypes, EMPI_Comm comm );
int xEMPI_Scatter ( void *sendbuf, int sendcnt, EMPI_Datatype sendtype, void *recvbuf, int recvcnt, EMPI_Datatype recvtype, int root, EMPI_Comm comm );
int xEMPI_Scatterv ( void *sendbuf, int *sendcnts, int *displs, EMPI_Datatype sendtype, void *recvbuf, int recvcnt, EMPI_Datatype recvtype, int root, EMPI_Comm comm );
int xEMPI_Barrier ( EMPI_Comm comm );
int xEMPI_Bcast ( void *buffer, int count, EMPI_Datatype datatype, int root, EMPI_Comm comm );
int xEMPI_Gather ( void *sendbuf, int sendcnt, EMPI_Datatype sendtype, void *recvbuf, int recvcnt, EMPI_Datatype recvtype, int root, EMPI_Comm comm );
int xEMPI_Gatherv ( void *sendbuf, int sendcnt, EMPI_Datatype sendtype, void *recvbuf, int *recvcnts, int *displs, EMPI_Datatype recvtype, int root, EMPI_Comm comm );
int xEMPI_Reduce ( void *sendbuf, void *recvbuf, int count, EMPI_Datatype datatype, EMPI_Op op, int root, EMPI_Comm comm );


// Send / receive.

int xEMPI_Ibsend ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm, EMPI_Request *r );
int xEMPI_Irecv ( void *buf, int count, EMPI_Datatype datatype, int source, int tag, EMPI_Comm comm, EMPI_Request *r );
int xEMPI_Irsend ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm, EMPI_Request *r );
int xEMPI_Isend ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm, EMPI_Request *r );
int xEMPI_Rsend ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm );
int xEMPI_Send ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm );
int xEMPI_Sendrecv ( void *sendbuf, int sendcount, EMPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, EMPI_Datatype recvtype, int source, int recvtag, EMPI_Comm comm, EMPI_Status *status );
int xEMPI_Ssend ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm );
int xEMPI_Recv(void *buf, int count, EMPI_Datatype type, int source, int tag, EMPI_Comm comm, EMPI_Status *s);

// Request and status handling.
int xEMPI_Wait ( EMPI_Request *r, EMPI_Status *status );
int xEMPI_Waitall ( int count, EMPI_Request *array_of_requests, EMPI_Status *array_of_statuses );
int xEMPI_Request_free ( EMPI_Request *r );
int xEMPI_Waitany(int count, EMPI_Request *array_of_requests, int *index, EMPI_Status *s);


// Datatypes
int xEMPI_Type_free ( EMPI_Datatype *type );
int xEMPI_Type_get_envelope ( EMPI_Datatype type, int *num_integers, int *num_addresses, int *num_datatypes, int *combiner );
int xEMPI_Type_create_indexed_block ( int count, int blocklength, int array_of_displacements[], EMPI_Datatype oldtype, EMPI_Datatype *newtype );
int xEMPI_Type_contiguous ( int count, EMPI_Datatype old_type, EMPI_Datatype *new_type_p );
int xEMPI_Type_commit ( EMPI_Datatype *type );
int xEMPI_Type_get_name ( EMPI_Datatype type, char *type_name, int *resultlen );


// Files
int xEMPI_File_set_view ( EMPI_File EMPI_fh, EMPI_Offset disp, EMPI_Datatype etype, EMPI_Datatype filetype, char *datarep, EMPI_Info info );
int xEMPI_File_open ( EMPI_Comm comm, char *filename, int amode, EMPI_Info info, EMPI_File *fh );
int xEMPI_File_close ( EMPI_File *mpi_fh );
int xEMPI_File_write_at ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype type, EMPI_Status *s );
int xEMPI_File_read_at ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype type, EMPI_Status *s );
int xEMPI_File_read_all ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype type, EMPI_Status *s );
int xEMPI_File_write_all ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype type, EMPI_Status *s );


// Info
int xEMPI_Info_create ( EMPI_Info *info );
int xEMPI_Info_delete ( EMPI_Info info, char *key );
int xEMPI_Info_set ( EMPI_Info info, char *key, char *value );
int xEMPI_Info_free ( EMPI_Info *info );


// Intercomm
int xEMPI_Intercomm_create ( EMPI_Comm local_comm, int local_leader, EMPI_Comm peer_comm, int remote_leader, int tag, EMPI_Comm *newintercomm );
int xEMPI_Intercomm_merge ( EMPI_Comm intercomm, int high, EMPI_Comm *newintracomm );


// Datatypes
/*
int xEMPI_Type_get_name ( EMPI_Datatype datatype, char *type_name, int *resultlen );
int xEMPI_Type_create_f90_complex ( int p, int r, EMPI_Datatype *newtype );
int xEMPI_Type_create_f90_integer ( int r, EMPI_Datatype *newtype );
int xEMPI_Type_create_f90_real ( int p, int r, EMPI_Datatype *newtype );
int xEMPI_Accumulate ( void *origin_addr, int origin_count, EMPI_Datatype origin_datatype, int target_rank, EMPI_Aint target_disp, int target_count, EMPI_Datatype target_datatype, EMPI_Op op, EMPI_Win win );
int xEMPI_Add_error_class ( int *errorclass );
int xEMPI_Add_error_code ( int errorclass, int *errorcode );
int xEMPI_Add_error_string ( int errorcode, char *string );
int xEMPI_Address ( void *location, EMPI_Aint *address );
int xEMPI_Alloc_mem ( EMPI_Aint size, EMPI_Info info, void *baseptr );
int xEMPI_Attr_delete ( EMPI_Comm comm, int keyval );
int xEMPI_Attr_get ( EMPI_Comm comm, int keyval, void *attr_value, int *flag );
int xEMPI_Attr_put ( EMPI_Comm comm, int keyval, void *attr_value );
int xEMPI_Bsend ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm );
int xEMPI_Bsend_init ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm, EMPI_Request *r );
int xEMPI_Buffer_attach ( void *buffer, int size );
int xEMPI_Buffer_detach ( void *buffer, int *size );
int xEMPI_Cancel ( EMPI_Request *r );
int xEMPI_Cart_coords ( EMPI_Comm comm, int rank, int maxdims, int *coords );
int xEMPI_Cart_create ( EMPI_Comm comm_old, int ndims, int *dims, int *periods, int reorder, EMPI_Comm *comm_cart );
int xEMPI_Cartdim_get ( EMPI_Comm comm, int *ndims );
int xEMPI_Cart_get ( EMPI_Comm comm, int maxdims, int *dims, int *periods, int *coords );
int xEMPI_Cart_map ( EMPI_Comm comm_old, int ndims, int *dims, int *periods, int *newrank );
int xEMPI_Cart_rank ( EMPI_Comm comm, int *coords, int *rank );
int xEMPI_Cart_shift ( EMPI_Comm comm, int direction, int displ, int *source, int *dest );
int xEMPI_Cart_sub ( EMPI_Comm comm, int *remain_dims, EMPI_Comm *comm_new );
int xEMPI_Close_port ( char *port_name );
int xEMPI_Comm_accept ( char *port_name, EMPI_Info info, int root, EMPI_Comm comm, EMPI_Comm *newcomm );
int xEMPI_Comm_call_errhandler ( EMPI_Comm comm, int errorcode );
int xEMPI_Comm_compare ( EMPI_Comm comm1, EMPI_Comm comm2, int *result );
int xEMPI_Comm_connect ( char *port_name, EMPI_Info info, int root, EMPI_Comm comm, EMPI_Comm *newcomm );
int xEMPI_Comm_create_errhandler ( EMPI_Comm_errhandler_fn *function, EMPI_Errhandler *errhandler );
int xEMPI_Comm_create_keyval ( EMPI_Comm_copy_attr_function *comm_copy_attr_fn, EMPI_Comm_delete_attr_function *comm_delete_attr_fn, int *comm_keyval, void *extra_state );
int xEMPI_Comm_delete_attr ( EMPI_Comm comm, int comm_keyval );
int xEMPI_Comm_disconnect ( EMPI_Comm *comm );
int xEMPI_Comm_free_keyval ( int *comm_keyval );
int xEMPI_Comm_get_attr ( EMPI_Comm comm, int comm_keyval, void *attribute_val, int *flag );
int xEMPI_Comm_get_errhandler ( EMPI_Comm comm, EMPI_Errhandler *errhandler );
int xEMPI_Comm_get_name ( EMPI_Comm comm, char *comm_name, int *resultlen );
int xEMPI_Comm_get_parent ( EMPI_Comm *parent );
int xEMPI_Comm_join ( int fd, EMPI_Comm *intercomm );
int xEMPI_Comm_remote_group ( EMPI_Comm comm, EMPI_Group *g );
int xEMPI_Comm_remote_size ( EMPI_Comm comm, int *size );
int xEMPI_Comm_set_attr ( EMPI_Comm comm, int comm_keyval, void *attribute_val );
int xEMPI_Comm_set_errhandler ( EMPI_Comm comm, EMPI_Errhandler errhandler );
int xEMPI_Comm_set_name ( EMPI_Comm comm, char *comm_name );
int xEMPI_Comm_spawn ( char *command, char *argv[], int maxprocs, EMPI_Info info, int root, EMPI_Comm comm, EMPI_Comm *intercomm, int array_of_errcodes[] );
int xEMPI_Comm_spawn_multiple ( int count, char *array_of_commands[], char* *array_of_argv[], int array_of_maxprocs[], EMPI_Info array_of_info[], int root, EMPI_Comm comm, EMPI_Comm *intercomm, int array_of_errcodes[] );
int xEMPI_Comm_test_inter ( EMPI_Comm comm, int *flag );
int xEMPI_Dims_create ( int nnodes, int ndims, int *dims );
int xEMPI_Dist_graph_create_adjacent ( EMPI_Comm comm_old, int indegree, int sources[], int sourceweights[], int outdegree, int destinations[], int destweights[], EMPI_Info info, int reorder, EMPI_Comm *comm_dist_graph );
int xEMPI_Dist_graph_create ( EMPI_Comm comm_old, int n, int sources[], int degrees[], int destinations[], int weights[], EMPI_Info info, int reorder, EMPI_Comm *comm_dist_graph );
int xEMPI_Dist_graph_neighbors_count ( EMPI_Comm comm, int *indegree, int *outdegree, int *weighted );
int xEMPI_Dist_graph_neighbors ( EMPI_Comm comm, int maxindegree, int sources[], int sourceweights[], int maxoutdegree, int destinations[], int destweights[] );
int xEMPI_Errhandler_create ( EMPI_Handler_function *function, EMPI_Errhandler *errhandler );
int xEMPI_Errhandler_free ( EMPI_Errhandler *errhandler );
int xEMPI_Errhandler_get ( EMPI_Comm comm, EMPI_Errhandler *errhandler );
int xEMPI_Errhandler_set ( EMPI_Comm comm, EMPI_Errhandler errhandler );
int xEMPI_Error_class ( int errorcode, int *errorclass );
int xEMPI_Exscan ( void *sendbuf, void *recvbuf, int count, EMPI_Datatype datatype, EMPI_Op op, EMPI_Comm comm );
int xEMPI_File_call_errhandler ( EMPI_File fh, int errorcode );
int xEMPI_File_close ( EMPI_File *mpi_fh );
int xEMPI_File_create_errhandler ( EMPI_File_errhandler_fn *function, EMPI_Errhandler *errhandler );
int xEMPI_File_delete ( char *filename, EMPI_Info info );
int xEMPI_File_get_amode ( EMPI_File EMPI_fh, int *amode );
int xEMPI_File_get_atomicity ( EMPI_File EMPI_fh, int *flag );
int xEMPI_File_get_byte_offset ( EMPI_File EMPI_fh, EMPI_Offset offset, EMPI_Offset *disp );
int xEMPI_File_get_errhandler ( EMPI_File file, EMPI_Errhandler *errhandler );
int xEMPI_File_get_group ( EMPI_File EMPI_fh, EMPI_Group *g );
int xEMPI_File_get_info ( EMPI_File EMPI_fh, EMPI_Info *info_used );
int xEMPI_File_get_position ( EMPI_File EMPI_fh, EMPI_Offset *offset );
int xEMPI_File_get_position_shared ( EMPI_File EMPI_fh, EMPI_Offset *offset );
int xEMPI_File_get_size ( EMPI_File EMPI_fh, EMPI_Offset *size );
int xEMPI_File_get_type_extent ( EMPI_File EMPI_fh, EMPI_Datatype datatype, EMPI_Aint *extent );
int xEMPI_File_get_view ( EMPI_File EMPI_fh, EMPI_Offset *disp, EMPI_Datatype *etype, EMPI_Datatype *filetype, char *datarep );
int xEMPI_File_iread_at ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype datatype, MPIO_Request *r );
int xEMPI_File_iread ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Request *r );
int xEMPI_File_iread_shared ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Request *r );
int xEMPI_File_iwrite_at ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype datatype, MPIO_Request *r );
int xEMPI_File_iwrite ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Request *r );
int xEMPI_File_iwrite_shared ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, MPIO_Request *r );
int xEMPI_File_open ( EMPI_Comm comm, char *filename, int amode, EMPI_Info info, EMPI_File *fh );
int xEMPI_File_preallocate ( EMPI_File EMPI_fh, EMPI_Offset size );
int xEMPI_File_read_all_begin ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype );
int xEMPI_File_read_all_end ( EMPI_File EMPI_fh, void *buf, EMPI_Status *status );
int xEMPI_File_read_all ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status );
int xEMPI_File_read_at_all_begin ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype datatype );
int xEMPI_File_read_at_all_end ( EMPI_File EMPI_fh, void *buf, EMPI_Status *status );
int xEMPI_File_read_at_all ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status );
int xEMPI_File_read_at ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status );
int xEMPI_File_read ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status );
int xEMPI_File_read_ordered_begin ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype );
int xEMPI_File_read_ordered_end ( EMPI_File EMPI_fh, void *buf, EMPI_Status *status );
int xEMPI_File_read_ordered ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status );
int xEMPI_File_read_shared ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status );
int xEMPI_File_seek ( EMPI_File EMPI_fh, EMPI_Offset offset, int whence );
int xEMPI_File_seek_shared ( EMPI_File EMPI_fh, EMPI_Offset offset, int whence );
int xEMPI_File_set_atomicity ( EMPI_File EMPI_fh, int flag );
int xEMPI_File_set_errhandler ( EMPI_File file, EMPI_Errhandler errhandler );
int xEMPI_File_set_info ( EMPI_File EMPI_fh, EMPI_Info info );
int xEMPI_File_set_size ( EMPI_File EMPI_fh, EMPI_Offset size );
int xEMPI_File_set_view ( EMPI_File EMPI_fh, EMPI_Offset disp, EMPI_Datatype etype, EMPI_Datatype filetype, char *datarep, EMPI_Info info );
int xEMPI_File_sync ( EMPI_File EMPI_fh );
int xEMPI_File_write_all_begin ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype );
int xEMPI_File_write_all_end ( EMPI_File EMPI_fh, void *buf, EMPI_Status *status );
int xEMPI_File_write_all ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status );
int xEMPI_File_write_at_all_begin ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype datatype );
int xEMPI_File_write_at_all_end ( EMPI_File EMPI_fh, void *buf, EMPI_Status *status );
int xEMPI_File_write_at_all ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status );
int xEMPI_File_write_at ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status );
int xEMPI_File_write ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status );
int xEMPI_File_write_ordered_begin ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype );
int xEMPI_File_write_ordered_end ( EMPI_File EMPI_fh, void *buf, EMPI_Status *status );
int xEMPI_File_write_ordered ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status );
int xEMPI_File_write_shared ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status );
int xEMPI_Free_mem ( void *base );
int xEMPI_Get_address ( void *location, EMPI_Aint *address );
int xEMPI_Get_count ( EMPI_Status *status, EMPI_Datatype datatype, int *count );
int xEMPI_Get_elements ( EMPI_Status *status, EMPI_Datatype datatype, int *elements );
int xEMPI_Get ( void *origin_addr, int origin_count, EMPI_Datatype origin_datatype, int target_rank, EMPI_Aint target_disp, int target_count, EMPI_Datatype target_datatype, EMPI_Win win );
int xEMPI_Get_version ( int *version, int *subversion );
int xEMPI_Graph_create ( EMPI_Comm comm_old, int nnodes, int *indx, int *edges, int reorder, EMPI_Comm *comm_graph );
int xEMPI_Graphdims_get ( EMPI_Comm comm, int *nnodes, int *nedges );
int xEMPI_Graph_get ( EMPI_Comm comm, int maxindex, int maxedges, int *indx, int *edges );
int xEMPI_Graph_map ( EMPI_Comm comm_old, int nnodes, int *indx, int *edges, int *newrank );
int xEMPI_Graph_neighbors_count ( EMPI_Comm comm, int rank, int *nneighbors );
int xEMPI_Graph_neighbors ( EMPI_Comm comm, int rank, int maxneighbors, int *neighbors );
int xEMPI_Grequest_complete ( EMPI_Request r );
int xEMPI_Grequest_start ( EMPI_Grequest_query_function *query_fn, EMPI_Grequest_free_function *free_fn, EMPI_Grequest_cancel_function *cancel_fn, void *extra_state, EMPI_Request *r );
int xEMPI_Group_compare ( EMPI_Group group1, EMPI_Group group2, int *result );
int xEMPI_Group_difference ( EMPI_Group group1, EMPI_Group group2, EMPI_Group *newgroup );
int xEMPI_Group_excl ( EMPI_Group g, int n, int *ranks, EMPI_Group *newgroup );
int xEMPI_Group_free ( EMPI_Group *g );
int xEMPI_Group_incl ( EMPI_Group g, int n, int *ranks, EMPI_Group *newgroup );
int xEMPI_Group_intersection ( EMPI_Group group1, EMPI_Group group2, EMPI_Group *newgroup );
int xEMPI_Group_range_excl ( EMPI_Group g, int n, int ranges[][3], EMPI_Group *newgroup );
int xEMPI_Group_rank ( EMPI_Group g, int *rank );
int xEMPI_Group_size ( EMPI_Group g, int *size );
int xEMPI_Group_translate_ranks ( EMPI_Group group1, int n, int *ranks1, EMPI_Group group2, int *ranks2 );
int xEMPI_Group_union ( EMPI_Group group1, EMPI_Group group2, EMPI_Group *newgroup );
int xEMPI_Info_create ( EMPI_Info *info );
int xEMPI_Info_delete ( EMPI_Info info, char *key );
int xEMPI_Info_dup ( EMPI_Info info, EMPI_Info *newinfo );
int xEMPI_Info_free ( EMPI_Info *info );
int xEMPI_Info_get ( EMPI_Info info, char *key, int valuelen, char *value, int *flag );
int xEMPI_Info_get_nkeys ( EMPI_Info info, int *nkeys );
int xEMPI_Info_get_nthkey ( EMPI_Info info, int n, char *key );
int xEMPI_Info_get_valuelen ( EMPI_Info info, char *key, int *valuelen, int *flag );
int xEMPI_Info_set ( EMPI_Info info, char *key, char *value );
int xEMPI_Init_thread ( int *argc, char ***argv, int required, int *provided );
int xEMPI_Intercomm_create ( EMPI_Comm local_comm, int local_leader, EMPI_Comm peer_comm, int remote_leader, int tag, EMPI_Comm *newintercomm );
int xEMPI_Intercomm_merge ( EMPI_Comm intercomm, int high, EMPI_Comm *newintracomm );
int xEMPI_Iprobe ( int source, int tag, EMPI_Comm comm, int *flag, EMPI_Status *status );
int xEMPI_Issend ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm, EMPI_Request *r );
int xEMPI_Is_thread_main ( int *flag );
int xEMPI_Keyval_create ( EMPI_Copy_function *copy_fn, EMPI_Delete_function *delete_fn, int *keyval, void *extra_state );
int xEMPI_Keyval_free ( int *keyval );
int xEMPI_Lookup_name ( char *service_name, EMPI_Info info, char *port_name );
int xEMPI_Op_commutative ( EMPI_Op op, int *commute );
int xEMPI_Op_create ( EMPI_User_function *function, int commute, EMPI_Op *op );
int xEMPI_Open_port ( EMPI_Info info, char *port_name );
int xEMPI_Op_free ( EMPI_Op *op );
int xEMPI_Pack_external ( char *datarep, void *inbuf, int incount, EMPI_Datatype datatype, void *outbuf, EMPI_Aint outcount, EMPI_Aint *position );
int xEMPI_Pack_external_size ( char *datarep, int incount, EMPI_Datatype datatype, EMPI_Aint *size );
int xEMPI_Pack ( void *inbuf, int incount, EMPI_Datatype datatype, void *outbuf, int outcount, int *position, EMPI_Comm comm );
int xEMPI_Pack_size ( int incount, EMPI_Datatype datatype, EMPI_Comm comm, int *size );
int xEMPI_Probe ( int source, int tag, EMPI_Comm comm, EMPI_Status *status );
int xEMPI_Publish_name ( char *service_name, EMPI_Info info, char *port_name );
int xEMPI_Put ( void *origin_addr, int origin_count, EMPI_Datatype origin_datatype, int target_rank, EMPI_Aint target_disp, int target_count, EMPI_Datatype target_datatype, EMPI_Win win );
int xEMPI_Query_thread ( int *provided );
int xEMPI_Recv ( void *buf, int count, EMPI_Datatype datatype, int source, int tag, EMPI_Comm comm, EMPI_Status *status );
int xEMPI_Recv_init ( void *buf, int count, EMPI_Datatype datatype, int source, int tag, EMPI_Comm comm, EMPI_Request *r );
int xEMPI_Reduce_local ( void *inbuf, void *inoutbuf, int count, EMPI_Datatype datatype, EMPI_Op op );
int xEMPI_Reduce_scatter_block ( void *sendbuf, void *recvbuf, int recvcount, EMPI_Datatype datatype, EMPI_Op op, EMPI_Comm comm );
int xEMPI_Reduce_scatter ( void *sendbuf, void *recvbuf, int *recvcnts, EMPI_Datatype datatype, EMPI_Op op, EMPI_Comm comm );
int xEMPI_Register_datarep ( char *name, EMPI_Datarep_conversion_function *read_conv_fn, EMPI_Datarep_conversion_function *write_conv_fn, EMPI_Datarep_extent_function *extent_fn, void *state );
int xEMPI_Request_get_status ( EMPI_Request r, int *flag, EMPI_Status *status );
int xEMPI_Rsend_init ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm, EMPI_Request *r );
int xEMPI_Scan ( void *sendbuf, void *recvbuf, int count, EMPI_Datatype datatype, EMPI_Op op, EMPI_Comm comm );
int xEMPI_Send_init ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm, EMPI_Request *r );
int xEMPI_Sendrecv_replace ( void *buf, int count, EMPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, EMPI_Comm comm, EMPI_Status *status );
int xEMPI_Ssend_init ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm, EMPI_Request *r );
int xEMPI_Startall ( int count, EMPI_Request array_of_requests[] );
int xEMPI_Start ( EMPI_Request *r );
int xEMPI_Status_set_cancelled ( EMPI_Status *status, int flag );
int xEMPI_Status_set_elements ( EMPI_Status *status, EMPI_Datatype datatype, int count );
int xEMPI_Testall ( int count, EMPI_Request array_of_requests[], int *flag, EMPI_Status array_of_statuses[] );
int xEMPI_Testany ( int count, EMPI_Request array_of_requests[], int *index, int *flag, EMPI_Status *status );
int xEMPI_Test_cancelled ( EMPI_Status *status, int *flag );
int xEMPI_Test ( EMPI_Request *r, int *flag, EMPI_Status *status );
int xEMPI_Testsome ( int incount, EMPI_Request array_of_requests[], int *outcount, int array_of_indices[], EMPI_Status array_of_statuses[] );
int xEMPI_Topo_test ( EMPI_Comm comm, int *topo_type );
int xEMPI_Type_commit ( EMPI_Datatype *datatype );
int xEMPI_Type_contiguous ( int count, EMPI_Datatype old_type, EMPI_Datatype *new_type_p );
int xEMPI_Type_create_darray ( int size, int rank, int ndims, int array_of_gsizes[], int array_of_distribs[], int array_of_dargs[], int array_of_psizes[], int order, EMPI_Datatype oldtype, EMPI_Datatype *newtype );
int xEMPI_Type_create_hindexed ( int count, int blocklengths[], EMPI_Aint displacements[], EMPI_Datatype oldtype, EMPI_Datatype *newtype );
int xEMPI_Type_create_hvector ( int count, int blocklength, EMPI_Aint stride, EMPI_Datatype oldtype, EMPI_Datatype *newtype );
int xEMPI_Type_create_indexed_block ( int count, int blocklength, int array_of_displacements[], EMPI_Datatype oldtype, EMPI_Datatype *newtype );
int xEMPI_Type_create_keyval ( EMPI_Type_copy_attr_function *type_copy_attr_fn, EMPI_Type_delete_attr_function *type_delete_attr_fn, int *type_keyval, void *extra_state );
int xEMPI_Type_create_resized ( EMPI_Datatype oldtype, EMPI_Aint lb, EMPI_Aint extent, EMPI_Datatype *newtype );
int xEMPI_Type_create_struct ( int count, int array_of_blocklengths[], EMPI_Aint array_of_displacements[], EMPI_Datatype array_of_types[], EMPI_Datatype *newtype );
int xEMPI_Type_create_subarray ( int ndims, int array_of_sizes[], int array_of_subsizes[], int array_of_starts[], int order, EMPI_Datatype oldtype, EMPI_Datatype *newtype );
int xEMPI_Type_delete_attr ( EMPI_Datatype type, int type_keyval );
int xEMPI_Type_dup ( EMPI_Datatype datatype, EMPI_Datatype *newtype );
int xEMPI_Type_extent ( EMPI_Datatype datatype, EMPI_Aint *extent );
int xEMPI_Type_free ( EMPI_Datatype *datatype );
int xEMPI_Type_free_keyval ( int *type_keyval );
int xEMPI_Type_get_attr ( EMPI_Datatype type, int type_keyval, void *attribute_val, int *flag );
int xEMPI_Type_get_contents ( EMPI_Datatype datatype, int max_integers, int max_addresses, int max_datatypes, int array_of_integers[], EMPI_Aint array_of_addresses[], EMPI_Datatype array_of_datatypes[] );
int xEMPI_Type_get_envelope ( EMPI_Datatype datatype, int *num_integers, int *num_addresses, int *num_datatypes, int *combiner );
int xEMPI_Type_get_extent ( EMPI_Datatype datatype, EMPI_Aint *lb, EMPI_Aint *extent );
int xEMPI_Type_get_true_extent ( EMPI_Datatype datatype, EMPI_Aint *true_lb, EMPI_Aint *true_extent );
int xEMPI_Type_hindexed ( int count, int blocklens[], EMPI_Aint indices[], EMPI_Datatype old_type, EMPI_Datatype *newtype );
int xEMPI_Type_hvector ( int count, int blocklen, EMPI_Aint stride, EMPI_Datatype old_type, EMPI_Datatype *newtype_p );
int xEMPI_Type_indexed ( int count, int blocklens[], int indices[], EMPI_Datatype old_type, EMPI_Datatype *newtype );
int xEMPI_Type_lb ( EMPI_Datatype datatype, EMPI_Aint *displacement );
int xEMPI_Type_match_size ( int typeclass, int size, EMPI_Datatype *datatype );
int xEMPI_Type_set_attr ( EMPI_Datatype type, int type_keyval, void *attribute_val );
int xEMPI_Type_set_name ( EMPI_Datatype type, char *type_name );
int xEMPI_Type_size ( EMPI_Datatype datatype, int *size );
int xEMPI_Type_struct ( int count, int blocklens[], EMPI_Aint indices[], EMPI_Datatype old_types[], EMPI_Datatype *newtype );
int xEMPI_Type_ub ( EMPI_Datatype datatype, EMPI_Aint *displacement );
int xEMPI_Type_vector ( int count, int blocklength, int stride, EMPI_Datatype old_type, EMPI_Datatype *newtype_p );
int xEMPI_Unpack_external ( char *datarep, void *inbuf, EMPI_Aint insize, EMPI_Aint *position, void *outbuf, int outcount, EMPI_Datatype datatype );
int xEMPI_Unpack ( void *inbuf, int insize, int *position, void *outbuf, int outcount, EMPI_Datatype datatype, EMPI_Comm comm );
int xEMPI_Unpublish_name ( char *service_name, EMPI_Info info, char *port_name );
int xEMPI_Waitany ( int count, EMPI_Request array_of_requests[], int *index, EMPI_Status *status );
int xEMPI_Waitsome ( int incount, EMPI_Request array_of_requests[], int *outcount, int array_of_indices[], EMPI_Status array_of_statuses[] );
int xEMPI_Win_call_errhandler ( EMPI_Win win, int errorcode );
int xEMPI_Win_complete ( EMPI_Win win );
int xEMPI_Win_create_errhandler ( EMPI_Win_errhandler_fn *function, EMPI_Errhandler *errhandler );
int xEMPI_Win_create ( void *base, EMPI_Aint size, int disp_unit, EMPI_Info info, EMPI_Comm comm, EMPI_Win *win );
int xEMPI_Win_create_keyval ( EMPI_Win_copy_attr_function *win_copy_attr_fn, EMPI_Win_delete_attr_function *win_delete_attr_fn, int *win_keyval, void *extra_state );
int xEMPI_Win_delete_attr ( EMPI_Win win, int win_keyval );
int xEMPI_Win_fence ( int assert, EMPI_Win win );
int xEMPI_Win_free ( EMPI_Win *win );
int xEMPI_Win_free_keyval ( int *win_keyval );
int xEMPI_Win_get_attr ( EMPI_Win win, int win_keyval, void *attribute_val, int *flag );
int xEMPI_Win_get_errhandler ( EMPI_Win win, EMPI_Errhandler *errhandler );
int xEMPI_Win_get_group ( EMPI_Win win, EMPI_Group *g );
int xEMPI_Win_get_name ( EMPI_Win win, char *win_name, int *resultlen );
int xEMPI_Win_lock ( int lock_type, int rank, int assert, EMPI_Win win );
int xEMPI_Win_post ( EMPI_Group g, int assert, EMPI_Win win );
int xEMPI_Win_set_attr ( EMPI_Win win, int win_keyval, void *attribute_val );
int xEMPI_Win_set_errhandler ( EMPI_Win win, EMPI_Errhandler errhandler );
int xEMPI_Win_set_name ( EMPI_Win win, char *win_name );
int xEMPI_Win_start ( EMPI_Group g, int assert, EMPI_Win win );
int xEMPI_Win_test ( EMPI_Win win, int *flag );
int xEMPI_Win_unlock ( int rank, EMPI_Win win );
int xEMPI_Win_wait ( EMPI_Win win );
int xEMPI_Group_comm_create ( EMPI_Comm old_comm, EMPI_Group g, int tag, EMPI_Comm *new_comm );
EMPI_Fint xEMPI_Comm_c2f ( EMPI_Comm comm );
EMPI_Fint xEMPI_Group_c2f ( EMPI_Group g );
EMPI_Fint xEMPI_Request_c2f ( EMPI_Request r );
EMPI_Fint xEMPI_Info_c2f ( EMPI_Info info );
EMPI_Fint xEMPI_File_c2f ( EMPI_File file );
EMPI_Fint xEMPI_Op_c2f ( EMPI_Op op );
EMPI_Fint xEMPI_Win_c2f ( EMPI_Win Win );
EMPI_Fint xEMPI_Errhandler_c2f ( EMPI_Errhandler Errhandler );
EMPI_Fint xEMPI_Type_c2f ( EMPI_Datatype Type );
*/

#endif // __XEMPI_H_
