#ifndef __GENERATED_HEADER_H_
#define __GENERATED_HEADER_H_

#include "flags.h"

#ifdef IBIS_INTERCEPT

#include "mpi.h"

#define __IMPI_Type_create_f90_complex_FORWARD 1
int IMPI_Type_create_f90_complex ( int p, int r, MPI_Datatype *newtype );

#define __IMPI_Type_create_f90_integer_FORWARD 1
int IMPI_Type_create_f90_integer ( int r, MPI_Datatype *newtype );

#define __IMPI_Type_create_f90_real_FORWARD 1
int IMPI_Type_create_f90_real ( int p, int r, MPI_Datatype *newtype );

#define __IMPI_Abort_FORWARD 0
int IMPI_Abort ( MPI_Comm comm, int errorcode );

#define __IMPI_Accumulate_FORWARD 0
int IMPI_Accumulate ( void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Op op, MPI_Win win );

#define __IMPI_Add_error_class_FORWARD 1
int IMPI_Add_error_class ( int *errorclass );

#define __IMPI_Add_error_code_FORWARD 1
int IMPI_Add_error_code ( int errorclass, int *errorcode );

#define __IMPI_Add_error_string_FORWARD 1
int IMPI_Add_error_string ( int errorcode, char *string );

#define __IMPI_Address_FORWARD 1
int IMPI_Address ( void *location, MPI_Aint *address );

#define __IMPI_Allgather_FORWARD 0
int IMPI_Allgather ( void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm );

#define __IMPI_Allgatherv_FORWARD 0
int IMPI_Allgatherv ( void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int *recvcounts, int *displs, MPI_Datatype recvtype, MPI_Comm comm );

#define __IMPI_Alloc_mem_FORWARD 1
int IMPI_Alloc_mem ( MPI_Aint size, MPI_Info info, void *baseptr );

#define __IMPI_Allreduce_FORWARD 0
int IMPI_Allreduce ( void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm );

#define __IMPI_Alltoall_FORWARD 0
int IMPI_Alltoall ( void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm );

#define __IMPI_Alltoallv_FORWARD 0
int IMPI_Alltoallv ( void *sendbuf, int *sendcnts, int *sdispls, MPI_Datatype sendtype, void *recvbuf, int *recvcnts, int *rdispls, MPI_Datatype recvtype, MPI_Comm comm );

#define __IMPI_Alltoallw_FORWARD 0
int IMPI_Alltoallw ( void *sendbuf, int *sendcnts, int *sdispls, MPI_Datatype *sendtypes, void *recvbuf, int *recvcnts, int *rdispls, MPI_Datatype *recvtypes, MPI_Comm comm );

#define __IMPI_Attr_delete_FORWARD 0
int IMPI_Attr_delete ( MPI_Comm comm, int keyval );

#define __IMPI_Attr_get_FORWARD 0
int IMPI_Attr_get ( MPI_Comm comm, int keyval, void *attr_value, int *flag );

#define __IMPI_Attr_put_FORWARD 0
int IMPI_Attr_put ( MPI_Comm comm, int keyval, void *attr_value );

#define __IMPI_Barrier_FORWARD 0
int IMPI_Barrier ( MPI_Comm comm );

#define __IMPI_Bcast_FORWARD 0
int IMPI_Bcast ( void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm );

#define __IMPI_Bsend_FORWARD 0
int IMPI_Bsend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm );

#define __IMPI_Bsend_init_FORWARD 0
int IMPI_Bsend_init ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );

#define __IMPI_Buffer_attach_FORWARD 1
int IMPI_Buffer_attach ( void *buffer, int size );

#define __IMPI_Buffer_detach_FORWARD 1
int IMPI_Buffer_detach ( void *buffer, int *size );

#define __IMPI_Cancel_FORWARD 0
int IMPI_Cancel ( MPI_Request *r );

#define __IMPI_Cart_coords_FORWARD 0
int IMPI_Cart_coords ( MPI_Comm comm, int rank, int maxdims, int *coords );

#define __IMPI_Cart_create_FORWARD 0
int IMPI_Cart_create ( MPI_Comm comm_old, int ndims, int *dims, int *periods, int reorder, MPI_Comm *comm_cart );

#define __IMPI_Cartdim_get_FORWARD 0
int IMPI_Cartdim_get ( MPI_Comm comm, int *ndims );

#define __IMPI_Cart_get_FORWARD 0
int IMPI_Cart_get ( MPI_Comm comm, int maxdims, int *dims, int *periods, int *coords );

#define __IMPI_Cart_map_FORWARD 0
int IMPI_Cart_map ( MPI_Comm comm_old, int ndims, int *dims, int *periods, int *newrank );

#define __IMPI_Cart_rank_FORWARD 0
int IMPI_Cart_rank ( MPI_Comm comm, int *coords, int *rank );

#define __IMPI_Cart_shift_FORWARD 0
int IMPI_Cart_shift ( MPI_Comm comm, int direction, int displ, int *source, int *dest );

#define __IMPI_Cart_sub_FORWARD 0
int IMPI_Cart_sub ( MPI_Comm comm, int *remain_dims, MPI_Comm *comm_new );

#define __IMPI_Close_port_FORWARD 1
int IMPI_Close_port ( char *port_name );

#define __IMPI_Comm_accept_FORWARD 0
int IMPI_Comm_accept ( char *port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *newcomm );

#define __IMPI_Comm_call_errhandler_FORWARD 0
int IMPI_Comm_call_errhandler ( MPI_Comm comm, int errorcode );

#define __IMPI_Comm_compare_FORWARD 0
int IMPI_Comm_compare ( MPI_Comm comm1, MPI_Comm comm2, int *result );

#define __IMPI_Comm_connect_FORWARD 0
int IMPI_Comm_connect ( char *port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *newcomm );

#define __IMPI_Comm_create_errhandler_FORWARD 1
int IMPI_Comm_create_errhandler ( MPI_Comm_errhandler_fn *function, MPI_Errhandler *errhandler );

#define __IMPI_Comm_create_FORWARD 0
int IMPI_Comm_create ( MPI_Comm comm, MPI_Group g, MPI_Comm *newcomm );

#define __IMPI_Comm_create_keyval_FORWARD 1
int IMPI_Comm_create_keyval ( MPI_Comm_copy_attr_function *comm_copy_attr_fn, MPI_Comm_delete_attr_function *comm_delete_attr_fn, int *comm_keyval, void *extra_state );

#define __IMPI_Comm_delete_attr_FORWARD 0
int IMPI_Comm_delete_attr ( MPI_Comm comm, int comm_keyval );

#define __IMPI_Comm_disconnect_FORWARD 0
int IMPI_Comm_disconnect ( MPI_Comm *comm );

#define __IMPI_Comm_dup_FORWARD 0
int IMPI_Comm_dup ( MPI_Comm comm, MPI_Comm *newcomm );

#define __IMPI_Comm_free_FORWARD 0
int IMPI_Comm_free ( MPI_Comm *comm );

#define __IMPI_Comm_free_keyval_FORWARD 1
int IMPI_Comm_free_keyval ( int *comm_keyval );

#define __IMPI_Comm_get_attr_FORWARD 0
int IMPI_Comm_get_attr ( MPI_Comm comm, int comm_keyval, void *attribute_val, int *flag );

#define __IMPI_Comm_get_errhandler_FORWARD 0
int IMPI_Comm_get_errhandler ( MPI_Comm comm, MPI_Errhandler *errhandler );

#define __IMPI_Comm_get_name_FORWARD 0
int IMPI_Comm_get_name ( MPI_Comm comm, char *comm_name, int *resultlen );

#define __IMPI_Comm_get_parent_FORWARD 0
int IMPI_Comm_get_parent ( MPI_Comm *parent );

#define __IMPI_Comm_group_FORWARD 0
int IMPI_Comm_group ( MPI_Comm comm, MPI_Group *g );

#define __IMPI_Comm_join_FORWARD 0
int IMPI_Comm_join ( int fd, MPI_Comm *intercomm );

#define __IMPI_Comm_rank_FORWARD 0
int IMPI_Comm_rank ( MPI_Comm comm, int *rank );

#define __IMPI_Comm_remote_group_FORWARD 0
int IMPI_Comm_remote_group ( MPI_Comm comm, MPI_Group *g );

#define __IMPI_Comm_remote_size_FORWARD 0
int IMPI_Comm_remote_size ( MPI_Comm comm, int *size );

#define __IMPI_Comm_set_attr_FORWARD 0
int IMPI_Comm_set_attr ( MPI_Comm comm, int comm_keyval, void *attribute_val );

#define __IMPI_Comm_set_errhandler_FORWARD 0
int IMPI_Comm_set_errhandler ( MPI_Comm comm, MPI_Errhandler errhandler );

#define __IMPI_Comm_set_name_FORWARD 0
int IMPI_Comm_set_name ( MPI_Comm comm, char *comm_name );

#define __IMPI_Comm_size_FORWARD 0
int IMPI_Comm_size ( MPI_Comm comm, int *size );

#define __IMPI_Comm_spawn_FORWARD 0
int IMPI_Comm_spawn ( char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[] );

#define __IMPI_Comm_spawn_multiple_FORWARD 0
int IMPI_Comm_spawn_multiple ( int count, char *array_of_commands[], char* *array_of_argv[], int array_of_maxprocs[], MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[] );

#define __IMPI_Comm_split_FORWARD 0
int IMPI_Comm_split ( MPI_Comm comm, int color, int key, MPI_Comm *newcomm );

#define __IMPI_Comm_test_inter_FORWARD 0
int IMPI_Comm_test_inter ( MPI_Comm comm, int *flag );

#define __IMPI_Dims_create_FORWARD 1
int IMPI_Dims_create ( int nnodes, int ndims, int *dims );

#define __IMPI_Dist_graph_create_adjacent_FORWARD 0
int IMPI_Dist_graph_create_adjacent ( MPI_Comm comm_old, int indegree, int sources[], int sourceweights[], int outdegree, int destinations[], int destweights[], MPI_Info info, int reorder, MPI_Comm *comm_dist_graph );

#define __IMPI_Dist_graph_create_FORWARD 0
int IMPI_Dist_graph_create ( MPI_Comm comm_old, int n, int sources[], int degrees[], int destinations[], int weights[], MPI_Info info, int reorder, MPI_Comm *comm_dist_graph );

#define __IMPI_Dist_graph_neighbors_count_FORWARD 0
int IMPI_Dist_graph_neighbors_count ( MPI_Comm comm, int *indegree, int *outdegree, int *weighted );

#define __IMPI_Dist_graph_neighbors_FORWARD 0
int IMPI_Dist_graph_neighbors ( MPI_Comm comm, int maxindegree, int sources[], int sourceweights[], int maxoutdegree, int destinations[], int destweights[] );

#define __IMPI_Errhandler_create_FORWARD 1
int IMPI_Errhandler_create ( MPI_Handler_function *function, MPI_Errhandler *errhandler );

#define __IMPI_Errhandler_free_FORWARD 1
int IMPI_Errhandler_free ( MPI_Errhandler *errhandler );

#define __IMPI_Errhandler_get_FORWARD 0
int IMPI_Errhandler_get ( MPI_Comm comm, MPI_Errhandler *errhandler );

#define __IMPI_Errhandler_set_FORWARD 0
int IMPI_Errhandler_set ( MPI_Comm comm, MPI_Errhandler errhandler );

#define __IMPI_Error_class_FORWARD 1
int IMPI_Error_class ( int errorcode, int *errorclass );

#define __IMPI_Error_string_FORWARD 1
int IMPI_Error_string ( int errorcode, char *string, int *resultlen );

#define __IMPI_Exscan_FORWARD 0
int IMPI_Exscan ( void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm );

#define __IMPI_File_call_errhandler_FORWARD 1
int IMPI_File_call_errhandler ( MPI_File fh, int errorcode );

#define __IMPI_File_close_FORWARD 1
int IMPI_File_close ( MPI_File *mpi_fh );

#define __IMPI_File_create_errhandler_FORWARD 1
int IMPI_File_create_errhandler ( MPI_File_errhandler_fn *function, MPI_Errhandler *errhandler );

#define __IMPI_File_delete_FORWARD 1
int IMPI_File_delete ( char *filename, MPI_Info info );

#define __IMPI_File_get_amode_FORWARD 1
int IMPI_File_get_amode ( MPI_File mpi_fh, int *amode );

#define __IMPI_File_get_atomicity_FORWARD 1
int IMPI_File_get_atomicity ( MPI_File mpi_fh, int *flag );

#define __IMPI_File_get_byte_offset_FORWARD 1
int IMPI_File_get_byte_offset ( MPI_File mpi_fh, MPI_Offset offset, MPI_Offset *disp );

#define __IMPI_File_get_errhandler_FORWARD 1
int IMPI_File_get_errhandler ( MPI_File file, MPI_Errhandler *errhandler );

#define __IMPI_File_get_group_FORWARD 0
int IMPI_File_get_group ( MPI_File mpi_fh, MPI_Group *g );

#define __IMPI_File_get_info_FORWARD 1
int IMPI_File_get_info ( MPI_File mpi_fh, MPI_Info *info_used );

#define __IMPI_File_get_position_FORWARD 1
int IMPI_File_get_position ( MPI_File mpi_fh, MPI_Offset *offset );

#define __IMPI_File_get_position_shared_FORWARD 1
int IMPI_File_get_position_shared ( MPI_File mpi_fh, MPI_Offset *offset );

#define __IMPI_File_get_size_FORWARD 1
int IMPI_File_get_size ( MPI_File mpi_fh, MPI_Offset *size );

#define __IMPI_File_get_type_extent_FORWARD 1
int IMPI_File_get_type_extent ( MPI_File mpi_fh, MPI_Datatype datatype, MPI_Aint *extent );

#define __IMPI_File_get_view_FORWARD 1
int IMPI_File_get_view ( MPI_File mpi_fh, MPI_Offset *disp, MPI_Datatype *etype, MPI_Datatype *filetype, char *datarep );

#define __IMPI_File_iread_at_FORWARD 1
int IMPI_File_iread_at ( MPI_File mpi_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPIO_Request *r );

#define __IMPI_File_iread_FORWARD 0
int IMPI_File_iread ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *r );

#define __IMPI_File_iread_shared_FORWARD 0
int IMPI_File_iread_shared ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *r );

#define __IMPI_File_iwrite_at_FORWARD 1
int IMPI_File_iwrite_at ( MPI_File mpi_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPIO_Request *r );

#define __IMPI_File_iwrite_FORWARD 0
int IMPI_File_iwrite ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *r );

#define __IMPI_File_iwrite_shared_FORWARD 1
int IMPI_File_iwrite_shared ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPIO_Request *r );

#define __IMPI_File_open_FORWARD 0
int IMPI_File_open ( MPI_Comm comm, char *filename, int amode, MPI_Info info, MPI_File *fh );

#define __IMPI_File_preallocate_FORWARD 1
int IMPI_File_preallocate ( MPI_File mpi_fh, MPI_Offset size );

#define __IMPI_File_read_all_begin_FORWARD 1
int IMPI_File_read_all_begin ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype );

#define __IMPI_File_read_all_end_FORWARD 1
int IMPI_File_read_all_end ( MPI_File mpi_fh, void *buf, MPI_Status *status );

#define __IMPI_File_read_all_FORWARD 1
int IMPI_File_read_all ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );

#define __IMPI_File_read_at_all_begin_FORWARD 1
int IMPI_File_read_at_all_begin ( MPI_File mpi_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype );

#define __IMPI_File_read_at_all_end_FORWARD 1
int IMPI_File_read_at_all_end ( MPI_File mpi_fh, void *buf, MPI_Status *status );

#define __IMPI_File_read_at_all_FORWARD 1
int IMPI_File_read_at_all ( MPI_File mpi_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );

#define __IMPI_File_read_at_FORWARD 1
int IMPI_File_read_at ( MPI_File mpi_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );

#define __IMPI_File_read_FORWARD 1
int IMPI_File_read ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );

#define __IMPI_File_read_ordered_begin_FORWARD 1
int IMPI_File_read_ordered_begin ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype );

#define __IMPI_File_read_ordered_end_FORWARD 1
int IMPI_File_read_ordered_end ( MPI_File mpi_fh, void *buf, MPI_Status *status );

#define __IMPI_File_read_ordered_FORWARD 1
int IMPI_File_read_ordered ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );

#define __IMPI_File_read_shared_FORWARD 1
int IMPI_File_read_shared ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );

#define __IMPI_File_seek_FORWARD 1
int IMPI_File_seek ( MPI_File mpi_fh, MPI_Offset offset, int whence );

#define __IMPI_File_seek_shared_FORWARD 1
int IMPI_File_seek_shared ( MPI_File mpi_fh, MPI_Offset offset, int whence );

#define __IMPI_File_set_atomicity_FORWARD 1
int IMPI_File_set_atomicity ( MPI_File mpi_fh, int flag );

#define __IMPI_File_set_errhandler_FORWARD 1
int IMPI_File_set_errhandler ( MPI_File file, MPI_Errhandler errhandler );

#define __IMPI_File_set_info_FORWARD 1
int IMPI_File_set_info ( MPI_File mpi_fh, MPI_Info info );

#define __IMPI_File_set_size_FORWARD 1
int IMPI_File_set_size ( MPI_File mpi_fh, MPI_Offset size );

#define __IMPI_File_set_view_FORWARD 1
int IMPI_File_set_view ( MPI_File mpi_fh, MPI_Offset disp, MPI_Datatype etype, MPI_Datatype filetype, char *datarep, MPI_Info info );

#define __IMPI_File_sync_FORWARD 1
int IMPI_File_sync ( MPI_File mpi_fh );

#define __IMPI_File_write_all_begin_FORWARD 1
int IMPI_File_write_all_begin ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype );

#define __IMPI_File_write_all_end_FORWARD 1
int IMPI_File_write_all_end ( MPI_File mpi_fh, void *buf, MPI_Status *status );

#define __IMPI_File_write_all_FORWARD 1
int IMPI_File_write_all ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );

#define __IMPI_File_write_at_all_begin_FORWARD 1
int IMPI_File_write_at_all_begin ( MPI_File mpi_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype );

#define __IMPI_File_write_at_all_end_FORWARD 1
int IMPI_File_write_at_all_end ( MPI_File mpi_fh, void *buf, MPI_Status *status );

#define __IMPI_File_write_at_all_FORWARD 1
int IMPI_File_write_at_all ( MPI_File mpi_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );

#define __IMPI_File_write_at_FORWARD 1
int IMPI_File_write_at ( MPI_File mpi_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );

#define __IMPI_File_write_FORWARD 1
int IMPI_File_write ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );

#define __IMPI_File_write_ordered_begin_FORWARD 1
int IMPI_File_write_ordered_begin ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype );

#define __IMPI_File_write_ordered_end_FORWARD 1
int IMPI_File_write_ordered_end ( MPI_File mpi_fh, void *buf, MPI_Status *status );

#define __IMPI_File_write_ordered_FORWARD 1
int IMPI_File_write_ordered ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );

#define __IMPI_File_write_shared_FORWARD 1
int IMPI_File_write_shared ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status );

#define __IMPI_Finalized_FORWARD 1
int IMPI_Finalized ( int *flag );

#define __IMPI_Finalize_FORWARD 1
int IMPI_Finalize (  );

#define __IMPI_Free_mem_FORWARD 1
int IMPI_Free_mem ( void *base );

#define __IMPI_Gather_FORWARD 0
int IMPI_Gather ( void *sendbuf, int sendcnt, MPI_Datatype sendtype, void *recvbuf, int recvcnt, MPI_Datatype recvtype, int root, MPI_Comm comm );

#define __IMPI_Gatherv_FORWARD 0
int IMPI_Gatherv ( void *sendbuf, int sendcnt, MPI_Datatype sendtype, void *recvbuf, int *recvcnts, int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm );

#define __IMPI_Get_address_FORWARD 1
int IMPI_Get_address ( void *location, MPI_Aint *address );

#define __IMPI_Get_count_FORWARD 1
int IMPI_Get_count ( MPI_Status *status, MPI_Datatype datatype, int *count );

#define __IMPI_Get_elements_FORWARD 1
int IMPI_Get_elements ( MPI_Status *status, MPI_Datatype datatype, int *elements );

#define __IMPI_Get_FORWARD 1
int IMPI_Get ( void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win );

#define __IMPI_Get_processor_name_FORWARD 1
int IMPI_Get_processor_name ( char *name, int *resultlen );

#define __IMPI_Get_version_FORWARD 1
int IMPI_Get_version ( int *version, int *subversion );

#define __IMPI_Graph_create_FORWARD 0
int IMPI_Graph_create ( MPI_Comm comm_old, int nnodes, int *indx, int *edges, int reorder, MPI_Comm *comm_graph );

#define __IMPI_Graphdims_get_FORWARD 0
int IMPI_Graphdims_get ( MPI_Comm comm, int *nnodes, int *nedges );

#define __IMPI_Graph_get_FORWARD 0
int IMPI_Graph_get ( MPI_Comm comm, int maxindex, int maxedges, int *indx, int *edges );

#define __IMPI_Graph_map_FORWARD 0
int IMPI_Graph_map ( MPI_Comm comm_old, int nnodes, int *indx, int *edges, int *newrank );

#define __IMPI_Graph_neighbors_count_FORWARD 0
int IMPI_Graph_neighbors_count ( MPI_Comm comm, int rank, int *nneighbors );

#define __IMPI_Graph_neighbors_FORWARD 0
int IMPI_Graph_neighbors ( MPI_Comm comm, int rank, int maxneighbors, int *neighbors );

#define __IMPI_Grequest_complete_FORWARD 0
int IMPI_Grequest_complete ( MPI_Request r );

#define __IMPI_Grequest_start_FORWARD 0
int IMPI_Grequest_start ( MPI_Grequest_query_function *query_fn, MPI_Grequest_free_function *free_fn, MPI_Grequest_cancel_function *cancel_fn, void *extra_state, MPI_Request *r );

#define __IMPI_Group_compare_FORWARD 0
int IMPI_Group_compare ( MPI_Group group1, MPI_Group group2, int *result );

#define __IMPI_Group_difference_FORWARD 0
int IMPI_Group_difference ( MPI_Group group1, MPI_Group group2, MPI_Group *newgroup );

#define __IMPI_Group_excl_FORWARD 0
int IMPI_Group_excl ( MPI_Group g, int n, int *ranks, MPI_Group *newgroup );

#define __IMPI_Group_free_FORWARD 0
int IMPI_Group_free ( MPI_Group *g );

#define __IMPI_Group_incl_FORWARD 0
int IMPI_Group_incl ( MPI_Group g, int n, int *ranks, MPI_Group *newgroup );

#define __IMPI_Group_intersection_FORWARD 0
int IMPI_Group_intersection ( MPI_Group group1, MPI_Group group2, MPI_Group *newgroup );

#define __IMPI_Group_range_excl_FORWARD 0
int IMPI_Group_range_excl ( MPI_Group g, int n, int ranges[][3], MPI_Group *newgroup );

#define __IMPI_Group_range_incl_FORWARD 0
int IMPI_Group_range_incl ( MPI_Group g, int n, int ranges[][3], MPI_Group *newgroup );

#define __IMPI_Group_rank_FORWARD 0
int IMPI_Group_rank ( MPI_Group g, int *rank );

#define __IMPI_Group_size_FORWARD 0
int IMPI_Group_size ( MPI_Group g, int *size );

#define __IMPI_Group_translate_ranks_FORWARD 0
int IMPI_Group_translate_ranks ( MPI_Group group1, int n, int *ranks1, MPI_Group group2, int *ranks2 );

#define __IMPI_Group_union_FORWARD 0
int IMPI_Group_union ( MPI_Group group1, MPI_Group group2, MPI_Group *newgroup );

#define __IMPI_Ibsend_FORWARD 0
int IMPI_Ibsend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );

#define __IMPI_Info_create_FORWARD 1
int IMPI_Info_create ( MPI_Info *info );

#define __IMPI_Info_delete_FORWARD 1
int IMPI_Info_delete ( MPI_Info info, char *key );

#define __IMPI_Info_dup_FORWARD 1
int IMPI_Info_dup ( MPI_Info info, MPI_Info *newinfo );

#define __IMPI_Info_free_FORWARD 1
int IMPI_Info_free ( MPI_Info *info );

#define __IMPI_Info_get_FORWARD 1
int IMPI_Info_get ( MPI_Info info, char *key, int valuelen, char *value, int *flag );

#define __IMPI_Info_get_nkeys_FORWARD 1
int IMPI_Info_get_nkeys ( MPI_Info info, int *nkeys );

#define __IMPI_Info_get_nthkey_FORWARD 1
int IMPI_Info_get_nthkey ( MPI_Info info, int n, char *key );

#define __IMPI_Info_get_valuelen_FORWARD 1
int IMPI_Info_get_valuelen ( MPI_Info info, char *key, int *valuelen, int *flag );

#define __IMPI_Info_set_FORWARD 1
int IMPI_Info_set ( MPI_Info info, char *key, char *value );

#define __IMPI_Init_FORWARD 1
int IMPI_Init ( int *argc, char ***argv );

#define __IMPI_Initialized_FORWARD 1
int IMPI_Initialized ( int *flag );

#define __IMPI_Init_thread_FORWARD 1
int IMPI_Init_thread ( int *argc, char ***argv, int required, int *provided );

#define __IMPI_Intercomm_create_FORWARD 0
int IMPI_Intercomm_create ( MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm );

#define __IMPI_Intercomm_merge_FORWARD 0
int IMPI_Intercomm_merge ( MPI_Comm intercomm, int high, MPI_Comm *newintracomm );

#define __IMPI_Iprobe_FORWARD 0
int IMPI_Iprobe ( int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status );

#define __IMPI_Irecv_FORWARD 0
int IMPI_Irecv ( void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *r );

#define __IMPI_Irsend_FORWARD 0
int IMPI_Irsend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );

#define __IMPI_Isend_FORWARD 0
int IMPI_Isend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );

#define __IMPI_Issend_FORWARD 0
int IMPI_Issend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );

#define __IMPI_Is_thread_main_FORWARD 1
int IMPI_Is_thread_main ( int *flag );

#define __IMPI_Keyval_create_FORWARD 1
int IMPI_Keyval_create ( MPI_Copy_function *copy_fn, MPI_Delete_function *delete_fn, int *keyval, void *extra_state );

#define __IMPI_Keyval_free_FORWARD 1
int IMPI_Keyval_free ( int *keyval );

#define __IMPI_Lookup_name_FORWARD 1
int IMPI_Lookup_name ( char *service_name, MPI_Info info, char *port_name );

#define __IMPI_Op_commutative_FORWARD 0
int IMPI_Op_commutative ( MPI_Op op, int *commute );

#define __IMPI_Op_create_FORWARD 0
int IMPI_Op_create ( MPI_User_function *function, int commute, MPI_Op *op );

#define __IMPI_Open_port_FORWARD 1
int IMPI_Open_port ( MPI_Info info, char *port_name );

#define __IMPI_Op_free_FORWARD 0
int IMPI_Op_free ( MPI_Op *op );

#define __IMPI_Pack_external_FORWARD 1
int IMPI_Pack_external ( char *datarep, void *inbuf, int incount, MPI_Datatype datatype, void *outbuf, MPI_Aint outcount, MPI_Aint *position );

#define __IMPI_Pack_external_size_FORWARD 1
int IMPI_Pack_external_size ( char *datarep, int incount, MPI_Datatype datatype, MPI_Aint *size );

#define __IMPI_Pack_FORWARD 0
int IMPI_Pack ( void *inbuf, int incount, MPI_Datatype datatype, void *outbuf, int outcount, int *position, MPI_Comm comm );

#define __IMPI_Pack_size_FORWARD 0
int IMPI_Pack_size ( int incount, MPI_Datatype datatype, MPI_Comm comm, int *size );

#define __IMPI_Probe_FORWARD 0
int IMPI_Probe ( int source, int tag, MPI_Comm comm, MPI_Status *status );

#define __IMPI_Publish_name_FORWARD 1
int IMPI_Publish_name ( char *service_name, MPI_Info info, char *port_name );

#define __IMPI_Put_FORWARD 1
int IMPI_Put ( void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win );

#define __IMPI_Query_thread_FORWARD 1
int IMPI_Query_thread ( int *provided );

#define __IMPI_Recv_FORWARD 0
int IMPI_Recv ( void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status );

#define __IMPI_Recv_init_FORWARD 0
int IMPI_Recv_init ( void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *r );

#define __IMPI_Reduce_FORWARD 0
int IMPI_Reduce ( void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm );

#define __IMPI_Reduce_local_FORWARD 0
int IMPI_Reduce_local ( void *inbuf, void *inoutbuf, int count, MPI_Datatype datatype, MPI_Op op );

#define __IMPI_Reduce_scatter_block_FORWARD 0
int IMPI_Reduce_scatter_block ( void *sendbuf, void *recvbuf, int recvcount, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm );

#define __IMPI_Reduce_scatter_FORWARD 0
int IMPI_Reduce_scatter ( void *sendbuf, void *recvbuf, int *recvcnts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm );

#define __IMPI_Register_datarep_FORWARD 1
int IMPI_Register_datarep ( char *name, MPI_Datarep_conversion_function *read_conv_fn, MPI_Datarep_conversion_function *write_conv_fn, MPI_Datarep_extent_function *extent_fn, void *state );

#define __IMPI_Request_free_FORWARD 0
int IMPI_Request_free ( MPI_Request *r );

#define __IMPI_Request_get_status_FORWARD 0
int IMPI_Request_get_status ( MPI_Request r, int *flag, MPI_Status *status );

#define __IMPI_Rsend_FORWARD 0
int IMPI_Rsend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm );

#define __IMPI_Rsend_init_FORWARD 0
int IMPI_Rsend_init ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );

#define __IMPI_Scan_FORWARD 0
int IMPI_Scan ( void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm );

#define __IMPI_Scatter_FORWARD 0
int IMPI_Scatter ( void *sendbuf, int sendcnt, MPI_Datatype sendtype, void *recvbuf, int recvcnt, MPI_Datatype recvtype, int root, MPI_Comm comm );

#define __IMPI_Scatterv_FORWARD 0
int IMPI_Scatterv ( void *sendbuf, int *sendcnts, int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcnt, MPI_Datatype recvtype, int root, MPI_Comm comm );

#define __IMPI_Send_FORWARD 0
int IMPI_Send ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm );

#define __IMPI_Send_init_FORWARD 0
int IMPI_Send_init ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );

#define __IMPI_Sendrecv_FORWARD 0
int IMPI_Sendrecv ( void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status );

#define __IMPI_Sendrecv_replace_FORWARD 0
int IMPI_Sendrecv_replace ( void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status );

#define __IMPI_Ssend_FORWARD 0
int IMPI_Ssend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm );

#define __IMPI_Ssend_init_FORWARD 0
int IMPI_Ssend_init ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r );

#define __IMPI_Startall_FORWARD 0
int IMPI_Startall ( int count, MPI_Request array_of_requests[] );

#define __IMPI_Start_FORWARD 0
int IMPI_Start ( MPI_Request *r );

#define __IMPI_Status_set_cancelled_FORWARD 1
int IMPI_Status_set_cancelled ( MPI_Status *status, int flag );

#define __IMPI_Status_set_elements_FORWARD 1
int IMPI_Status_set_elements ( MPI_Status *status, MPI_Datatype datatype, int count );

#define __IMPI_Testall_FORWARD 0
int IMPI_Testall ( int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[] );

#define __IMPI_Testany_FORWARD 0
int IMPI_Testany ( int count, MPI_Request array_of_requests[], int *index, int *flag, MPI_Status *status );

#define __IMPI_Test_cancelled_FORWARD 1
int IMPI_Test_cancelled ( MPI_Status *status, int *flag );

#define __IMPI_Test_FORWARD 0
int IMPI_Test ( MPI_Request *r, int *flag, MPI_Status *status );

#define __IMPI_Testsome_FORWARD 0
int IMPI_Testsome ( int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[] );

#define __IMPI_Topo_test_FORWARD 0
int IMPI_Topo_test ( MPI_Comm comm, int *topo_type );

#define __IMPI_Type_commit_FORWARD 1
int IMPI_Type_commit ( MPI_Datatype *datatype );

#define __IMPI_Type_contiguous_FORWARD 1
int IMPI_Type_contiguous ( int count, MPI_Datatype old_type, MPI_Datatype *new_type_p );

#define __IMPI_Type_create_darray_FORWARD 1
int IMPI_Type_create_darray ( int size, int rank, int ndims, int array_of_gsizes[], int array_of_distribs[], int array_of_dargs[], int array_of_psizes[], int order, MPI_Datatype oldtype, MPI_Datatype *newtype );

#define __IMPI_Type_create_hindexed_FORWARD 1
int IMPI_Type_create_hindexed ( int count, int blocklengths[], MPI_Aint displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype );

#define __IMPI_Type_create_hvector_FORWARD 1
int IMPI_Type_create_hvector ( int count, int blocklength, MPI_Aint stride, MPI_Datatype oldtype, MPI_Datatype *newtype );

#define __IMPI_Type_create_indexed_block_FORWARD 1
int IMPI_Type_create_indexed_block ( int count, int blocklength, int array_of_displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype );

#define __IMPI_Type_create_keyval_FORWARD 1
int IMPI_Type_create_keyval ( MPI_Type_copy_attr_function *type_copy_attr_fn, MPI_Type_delete_attr_function *type_delete_attr_fn, int *type_keyval, void *extra_state );

#define __IMPI_Type_create_resized_FORWARD 1
int IMPI_Type_create_resized ( MPI_Datatype oldtype, MPI_Aint lb, MPI_Aint extent, MPI_Datatype *newtype );

#define __IMPI_Type_create_struct_FORWARD 1
int IMPI_Type_create_struct ( int count, int array_of_blocklengths[], MPI_Aint array_of_displacements[], MPI_Datatype array_of_types[], MPI_Datatype *newtype );

#define __IMPI_Type_create_subarray_FORWARD 1
int IMPI_Type_create_subarray ( int ndims, int array_of_sizes[], int array_of_subsizes[], int array_of_starts[], int order, MPI_Datatype oldtype, MPI_Datatype *newtype );

#define __IMPI_Type_delete_attr_FORWARD 1
int IMPI_Type_delete_attr ( MPI_Datatype type, int type_keyval );

#define __IMPI_Type_dup_FORWARD 1
int IMPI_Type_dup ( MPI_Datatype datatype, MPI_Datatype *newtype );

#define __IMPI_Type_extent_FORWARD 1
int IMPI_Type_extent ( MPI_Datatype datatype, MPI_Aint *extent );

#define __IMPI_Type_free_FORWARD 1
int IMPI_Type_free ( MPI_Datatype *datatype );

#define __IMPI_Type_free_keyval_FORWARD 1
int IMPI_Type_free_keyval ( int *type_keyval );

#define __IMPI_Type_get_attr_FORWARD 1
int IMPI_Type_get_attr ( MPI_Datatype type, int type_keyval, void *attribute_val, int *flag );

#define __IMPI_Type_get_contents_FORWARD 1
int IMPI_Type_get_contents ( MPI_Datatype datatype, int max_integers, int max_addresses, int max_datatypes, int array_of_integers[], MPI_Aint array_of_addresses[], MPI_Datatype array_of_datatypes[] );

#define __IMPI_Type_get_envelope_FORWARD 1
int IMPI_Type_get_envelope ( MPI_Datatype datatype, int *num_integers, int *num_addresses, int *num_datatypes, int *combiner );

#define __IMPI_Type_get_extent_FORWARD 1
int IMPI_Type_get_extent ( MPI_Datatype datatype, MPI_Aint *lb, MPI_Aint *extent );

#define __IMPI_Type_get_name_FORWARD 1
int IMPI_Type_get_name ( MPI_Datatype datatype, char *type_name, int *resultlen );

#define __IMPI_Type_get_true_extent_FORWARD 1
int IMPI_Type_get_true_extent ( MPI_Datatype datatype, MPI_Aint *true_lb, MPI_Aint *true_extent );

#define __IMPI_Type_hindexed_FORWARD 1
int IMPI_Type_hindexed ( int count, int blocklens[], MPI_Aint indices[], MPI_Datatype old_type, MPI_Datatype *newtype );

#define __IMPI_Type_hvector_FORWARD 1
int IMPI_Type_hvector ( int count, int blocklen, MPI_Aint stride, MPI_Datatype old_type, MPI_Datatype *newtype_p );

#define __IMPI_Type_indexed_FORWARD 1
int IMPI_Type_indexed ( int count, int blocklens[], int indices[], MPI_Datatype old_type, MPI_Datatype *newtype );

#define __IMPI_Type_lb_FORWARD 1
int IMPI_Type_lb ( MPI_Datatype datatype, MPI_Aint *displacement );

#define __IMPI_Type_match_size_FORWARD 1
int IMPI_Type_match_size ( int typeclass, int size, MPI_Datatype *datatype );

#define __IMPI_Type_set_attr_FORWARD 1
int IMPI_Type_set_attr ( MPI_Datatype type, int type_keyval, void *attribute_val );

#define __IMPI_Type_set_name_FORWARD 1
int IMPI_Type_set_name ( MPI_Datatype type, char *type_name );

#define __IMPI_Type_size_FORWARD 1
int IMPI_Type_size ( MPI_Datatype datatype, int *size );

#define __IMPI_Type_struct_FORWARD 1
int IMPI_Type_struct ( int count, int blocklens[], MPI_Aint indices[], MPI_Datatype old_types[], MPI_Datatype *newtype );

#define __IMPI_Type_ub_FORWARD 1
int IMPI_Type_ub ( MPI_Datatype datatype, MPI_Aint *displacement );

#define __IMPI_Type_vector_FORWARD 1
int IMPI_Type_vector ( int count, int blocklength, int stride, MPI_Datatype old_type, MPI_Datatype *newtype_p );

#define __IMPI_Unpack_external_FORWARD 1
int IMPI_Unpack_external ( char *datarep, void *inbuf, MPI_Aint insize, MPI_Aint *position, void *outbuf, int outcount, MPI_Datatype datatype );

#define __IMPI_Unpack_FORWARD 0
int IMPI_Unpack ( void *inbuf, int insize, int *position, void *outbuf, int outcount, MPI_Datatype datatype, MPI_Comm comm );

#define __IMPI_Unpublish_name_FORWARD 1
int IMPI_Unpublish_name ( char *service_name, MPI_Info info, char *port_name );

#define __IMPI_Waitall_FORWARD 0
int IMPI_Waitall ( int count, MPI_Request array_of_requests[], MPI_Status array_of_statuses[] );

#define __IMPI_Waitany_FORWARD 0
int IMPI_Waitany ( int count, MPI_Request array_of_requests[], int *index, MPI_Status *status );

#define __IMPI_Wait_FORWARD 0
int IMPI_Wait ( MPI_Request *r, MPI_Status *status );

#define __IMPI_Waitsome_FORWARD 0
int IMPI_Waitsome ( int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[] );

#define __IMPI_Win_call_errhandler_FORWARD 1
int IMPI_Win_call_errhandler ( MPI_Win win, int errorcode );

#define __IMPI_Win_complete_FORWARD 1
int IMPI_Win_complete ( MPI_Win win );

#define __IMPI_Win_create_errhandler_FORWARD 1
int IMPI_Win_create_errhandler ( MPI_Win_errhandler_fn *function, MPI_Errhandler *errhandler );

#define __IMPI_Win_create_FORWARD 0
int IMPI_Win_create ( void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win );

#define __IMPI_Win_create_keyval_FORWARD 1
int IMPI_Win_create_keyval ( MPI_Win_copy_attr_function *win_copy_attr_fn, MPI_Win_delete_attr_function *win_delete_attr_fn, int *win_keyval, void *extra_state );

#define __IMPI_Win_delete_attr_FORWARD 1
int IMPI_Win_delete_attr ( MPI_Win win, int win_keyval );

#define __IMPI_Win_fence_FORWARD 1
int IMPI_Win_fence ( int assert, MPI_Win win );

#define __IMPI_Win_free_FORWARD 1
int IMPI_Win_free ( MPI_Win *win );

#define __IMPI_Win_free_keyval_FORWARD 1
int IMPI_Win_free_keyval ( int *win_keyval );

#define __IMPI_Win_get_attr_FORWARD 1
int IMPI_Win_get_attr ( MPI_Win win, int win_keyval, void *attribute_val, int *flag );

#define __IMPI_Win_get_errhandler_FORWARD 1
int IMPI_Win_get_errhandler ( MPI_Win win, MPI_Errhandler *errhandler );

#define __IMPI_Win_get_group_FORWARD 0
int IMPI_Win_get_group ( MPI_Win win, MPI_Group *g );

#define __IMPI_Win_get_name_FORWARD 1
int IMPI_Win_get_name ( MPI_Win win, char *win_name, int *resultlen );

#define __IMPI_Win_lock_FORWARD 1
int IMPI_Win_lock ( int lock_type, int rank, int assert, MPI_Win win );

#define __IMPI_Win_post_FORWARD 0
int IMPI_Win_post ( MPI_Group g, int assert, MPI_Win win );

#define __IMPI_Win_set_attr_FORWARD 1
int IMPI_Win_set_attr ( MPI_Win win, int win_keyval, void *attribute_val );

#define __IMPI_Win_set_errhandler_FORWARD 1
int IMPI_Win_set_errhandler ( MPI_Win win, MPI_Errhandler errhandler );

#define __IMPI_Win_set_name_FORWARD 1
int IMPI_Win_set_name ( MPI_Win win, char *win_name );

#define __IMPI_Win_start_FORWARD 0
int IMPI_Win_start ( MPI_Group g, int assert, MPI_Win win );

#define __IMPI_Win_test_FORWARD 1
int IMPI_Win_test ( MPI_Win win, int *flag );

#define __IMPI_Win_unlock_FORWARD 1
int IMPI_Win_unlock ( int rank, MPI_Win win );

#define __IMPI_Win_wait_FORWARD 1
int IMPI_Win_wait ( MPI_Win win );

#define __IMPI_Group_comm_create_FORWARD 0
int IMPI_Group_comm_create ( MPI_Comm old_comm, MPI_Group g, int tag, MPI_Comm *new_comm );

#define __IMPI_Comm_f2c_FORWARD 1
MPI_Comm IMPI_Comm_f2c ( MPI_Fint comm );

#define __IMPI_Group_f2c_FORWARD 1
MPI_Group IMPI_Group_f2c ( MPI_Fint g );

#define __IMPI_Request_f2c_FORWARD 1
MPI_Request IMPI_Request_f2c ( MPI_Fint r );

#define __IMPI_Info_f2c_FORWARD 1
MPI_Info IMPI_Info_f2c ( MPI_Fint info );

#define __IMPI_File_f2c_FORWARD 1
MPI_File IMPI_File_f2c ( MPI_Fint file );

#define __IMPI_Op_f2c_FORWARD 1
MPI_Op IMPI_Op_f2c ( MPI_Fint op );

#define __IMPI_Win_f2c_FORWARD 1
MPI_Win IMPI_Win_f2c ( MPI_Fint Win );

#define __IMPI_Errhandler_f2c_FORWARD 1
MPI_Errhandler IMPI_Errhandler_f2c ( MPI_Fint Errhandler );

#define __IMPI_Type_f2c_FORWARD 1
MPI_Datatype IMPI_Type_f2c ( MPI_Fint Type );

#define __IMPI_Comm_c2f_FORWARD 0
MPI_Fint IMPI_Comm_c2f ( MPI_Comm comm );

#define __IMPI_Group_c2f_FORWARD 0
MPI_Fint IMPI_Group_c2f ( MPI_Group g );

#define __IMPI_Request_c2f_FORWARD 0
MPI_Fint IMPI_Request_c2f ( MPI_Request r );

#define __IMPI_Info_c2f_FORWARD 1
MPI_Fint IMPI_Info_c2f ( MPI_Info info );

#define __IMPI_File_c2f_FORWARD 1
MPI_Fint IMPI_File_c2f ( MPI_File file );

#define __IMPI_Op_c2f_FORWARD 0
MPI_Fint IMPI_Op_c2f ( MPI_Op op );

#define __IMPI_Win_c2f_FORWARD 1
MPI_Fint IMPI_Win_c2f ( MPI_Win Win );

#define __IMPI_Errhandler_c2f_FORWARD 1
MPI_Fint IMPI_Errhandler_c2f ( MPI_Errhandler Errhandler );

#define __IMPI_Type_c2f_FORWARD 1
MPI_Fint IMPI_Type_c2f ( MPI_Datatype Type );

#endif // IBIS_INTERCEPT

#endif // _GENERATED_HEADER_H_

