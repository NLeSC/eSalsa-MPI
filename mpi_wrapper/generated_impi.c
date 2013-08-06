#ifndef __IMPI_Type_create_f90_complex
int IMPI_Type_create_f90_complex ( int p, int r, MPI_Datatype *newtype )
{
#if __IMPI_Type_create_f90_complex_FORWARD >= 1
   return PMPI_Type_create_f90_complex(p, r, newtype);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_create_f90_complex (int p=%d, int r=%d, MPI_Datatype *newtype=%p)", p, r, newtype);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_create_f90_complex_FORWARD
}
#endif // __IMPI_Type_create_f90_complex


#ifndef __IMPI_Type_create_f90_integer
int IMPI_Type_create_f90_integer ( int r, MPI_Datatype *newtype )
{
#if __IMPI_Type_create_f90_integer_FORWARD >= 1
   return PMPI_Type_create_f90_integer(r, newtype);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_create_f90_integer (int r=%d, MPI_Datatype *newtype=%p)", r, newtype);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_create_f90_integer_FORWARD
}
#endif // __IMPI_Type_create_f90_integer


#ifndef __IMPI_Type_create_f90_real
int IMPI_Type_create_f90_real ( int p, int r, MPI_Datatype *newtype )
{
#if __IMPI_Type_create_f90_real_FORWARD >= 1
   return PMPI_Type_create_f90_real(p, r, newtype);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_create_f90_real (int p=%d, int r=%d, MPI_Datatype *newtype=%p)", p, r, newtype);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_create_f90_real_FORWARD
}
#endif // __IMPI_Type_create_f90_real


#ifndef __IMPI_Abort
int IMPI_Abort ( MPI_Comm comm, int errorcode )
{
#if __IMPI_Abort_FORWARD >= 1
   return PMPI_Abort(comm, errorcode);
#else
   FATAL("NOT IMPLEMENTED: MPI_Abort (MPI_Comm comm=%s, int errorcode=%d)", comm_to_string(comm), errorcode);
   return MPI_ERR_INTERN;
#endif // __IMPI_Abort_FORWARD
}
#endif // __IMPI_Abort


#ifndef __IMPI_Accumulate
int IMPI_Accumulate ( void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Op op, MPI_Win win )
{
#if __IMPI_Accumulate_FORWARD >= 1
   return PMPI_Accumulate(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, op, win);
#else
   FATAL("NOT IMPLEMENTED: MPI_Accumulate (void *origin_addr=%p, int origin_count=%d, MPI_Datatype origin_datatype=%s, int target_rank=%d, MPI_Aint target_disp=%p, int target_count=%d, MPI_Datatype target_datatype=%s, MPI_Op op=%s, MPI_Win win=%s)", origin_addr, origin_count, type_to_string(origin_datatype), target_rank, (void *) target_disp, target_count, type_to_string(target_datatype), op_to_string(op), win_to_string(win));
   return MPI_ERR_INTERN;
#endif // __IMPI_Accumulate_FORWARD
}
#endif // __IMPI_Accumulate


#ifndef __IMPI_Add_error_class
int IMPI_Add_error_class ( int *errorclass )
{
#if __IMPI_Add_error_class_FORWARD >= 1
   return PMPI_Add_error_class(errorclass);
#else
   FATAL("NOT IMPLEMENTED: MPI_Add_error_class (int *errorclass=%p)", errorclass);
   return MPI_ERR_INTERN;
#endif // __IMPI_Add_error_class_FORWARD
}
#endif // __IMPI_Add_error_class


#ifndef __IMPI_Add_error_code
int IMPI_Add_error_code ( int errorclass, int *errorcode )
{
#if __IMPI_Add_error_code_FORWARD >= 1
   return PMPI_Add_error_code(errorclass, errorcode);
#else
   FATAL("NOT IMPLEMENTED: MPI_Add_error_code (int errorclass=%d, int *errorcode=%p)", errorclass, errorcode);
   return MPI_ERR_INTERN;
#endif // __IMPI_Add_error_code_FORWARD
}
#endif // __IMPI_Add_error_code


#ifndef __IMPI_Add_error_string
int IMPI_Add_error_string ( int errorcode, char *string )
{
#if __IMPI_Add_error_string_FORWARD >= 1
   return PMPI_Add_error_string(errorcode, string);
#else
   FATAL("NOT IMPLEMENTED: MPI_Add_error_string (int errorcode=%d, char *string=%p)", errorcode, string);
   return MPI_ERR_INTERN;
#endif // __IMPI_Add_error_string_FORWARD
}
#endif // __IMPI_Add_error_string


#ifndef __IMPI_Address
int IMPI_Address ( void *location, MPI_Aint *address )
{
#if __IMPI_Address_FORWARD >= 1
   return PMPI_Address(location, address);
#else
   FATAL("NOT IMPLEMENTED: MPI_Address (void *location=%p, MPI_Aint *address=%p)", location, address);
   return MPI_ERR_INTERN;
#endif // __IMPI_Address_FORWARD
}
#endif // __IMPI_Address


#ifndef __IMPI_Allgather
int IMPI_Allgather ( void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm )
{
#if __IMPI_Allgather_FORWARD >= 1
   return PMPI_Allgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Allgather (void *sendbuf=%p, int sendcount=%d, MPI_Datatype sendtype=%s, void *recvbuf=%p, int recvcount=%d, MPI_Datatype recvtype=%s, MPI_Comm comm=%s)", sendbuf, sendcount, type_to_string(sendtype), recvbuf, recvcount, type_to_string(recvtype), comm_to_string(comm));
   return MPI_ERR_INTERN;
#endif // __IMPI_Allgather_FORWARD
}
#endif // __IMPI_Allgather


#ifndef __IMPI_Allgatherv
int IMPI_Allgatherv ( void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int *recvcounts, int *displs, MPI_Datatype recvtype, MPI_Comm comm )
{
#if __IMPI_Allgatherv_FORWARD >= 1
   return PMPI_Allgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Allgatherv (void *sendbuf=%p, int sendcount=%d, MPI_Datatype sendtype=%s, void *recvbuf=%p, int *recvcounts=%p, int *displs=%p, MPI_Datatype recvtype=%s, MPI_Comm comm=%s)", sendbuf, sendcount, type_to_string(sendtype), recvbuf, recvcounts, displs, type_to_string(recvtype), comm_to_string(comm));
   return MPI_ERR_INTERN;
#endif // __IMPI_Allgatherv_FORWARD
}
#endif // __IMPI_Allgatherv


#ifndef __IMPI_Alloc_mem
int IMPI_Alloc_mem ( MPI_Aint size, MPI_Info info, void *baseptr )
{
#if __IMPI_Alloc_mem_FORWARD >= 1
   return PMPI_Alloc_mem(size, info, baseptr);
#else
   FATAL("NOT IMPLEMENTED: MPI_Alloc_mem (MPI_Aint size=%p, MPI_Info info=%s, void *baseptr=%p)", (void *) size, info_to_string(info), baseptr);
   return MPI_ERR_INTERN;
#endif // __IMPI_Alloc_mem_FORWARD
}
#endif // __IMPI_Alloc_mem


#ifndef __IMPI_Allreduce
int IMPI_Allreduce ( void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm )
{
#if __IMPI_Allreduce_FORWARD >= 1
   return PMPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Allreduce (void *sendbuf=%p, void *recvbuf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Op op=%s, MPI_Comm comm=%s)", sendbuf, recvbuf, count, type_to_string(datatype), op_to_string(op), comm_to_string(comm));
   return MPI_ERR_INTERN;
#endif // __IMPI_Allreduce_FORWARD
}
#endif // __IMPI_Allreduce


#ifndef __IMPI_Alltoall
int IMPI_Alltoall ( void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm )
{
#if __IMPI_Alltoall_FORWARD >= 1
   return PMPI_Alltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Alltoall (void *sendbuf=%p, int sendcount=%d, MPI_Datatype sendtype=%s, void *recvbuf=%p, int recvcount=%d, MPI_Datatype recvtype=%s, MPI_Comm comm=%s)", sendbuf, sendcount, type_to_string(sendtype), recvbuf, recvcount, type_to_string(recvtype), comm_to_string(comm));
   return MPI_ERR_INTERN;
#endif // __IMPI_Alltoall_FORWARD
}
#endif // __IMPI_Alltoall


#ifndef __IMPI_Alltoallv
int IMPI_Alltoallv ( void *sendbuf, int *sendcnts, int *sdispls, MPI_Datatype sendtype, void *recvbuf, int *recvcnts, int *rdispls, MPI_Datatype recvtype, MPI_Comm comm )
{
#if __IMPI_Alltoallv_FORWARD >= 1
   return PMPI_Alltoallv(sendbuf, sendcnts, sdispls, sendtype, recvbuf, recvcnts, rdispls, recvtype, comm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Alltoallv (void *sendbuf=%p, int *sendcnts=%p, int *sdispls=%p, MPI_Datatype sendtype=%s, void *recvbuf=%p, int *recvcnts=%p, int *rdispls=%p, MPI_Datatype recvtype=%s, MPI_Comm comm=%s)", sendbuf, sendcnts, sdispls, type_to_string(sendtype), recvbuf, recvcnts, rdispls, type_to_string(recvtype), comm_to_string(comm));
   return MPI_ERR_INTERN;
#endif // __IMPI_Alltoallv_FORWARD
}
#endif // __IMPI_Alltoallv


#ifndef __IMPI_Alltoallw
int IMPI_Alltoallw ( void *sendbuf, int *sendcnts, int *sdispls, MPI_Datatype *sendtypes, void *recvbuf, int *recvcnts, int *rdispls, MPI_Datatype *recvtypes, MPI_Comm comm )
{
#if __IMPI_Alltoallw_FORWARD >= 1
   return PMPI_Alltoallw(sendbuf, sendcnts, sdispls, sendtypes, recvbuf, recvcnts, rdispls, recvtypes, comm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Alltoallw (void *sendbuf=%p, int *sendcnts=%p, int *sdispls=%p, MPI_Datatype *sendtypes=%p, void *recvbuf=%p, int *recvcnts=%p, int *rdispls=%p, MPI_Datatype *recvtypes=%p, MPI_Comm comm=%s)", sendbuf, sendcnts, sdispls, sendtypes, recvbuf, recvcnts, rdispls, recvtypes, comm_to_string(comm));
   return MPI_ERR_INTERN;
#endif // __IMPI_Alltoallw_FORWARD
}
#endif // __IMPI_Alltoallw


#ifndef __IMPI_Attr_delete
int IMPI_Attr_delete ( MPI_Comm comm, int keyval )
{
#if __IMPI_Attr_delete_FORWARD >= 1
   return PMPI_Attr_delete(comm, keyval);
#else
   FATAL("NOT IMPLEMENTED: MPI_Attr_delete (MPI_Comm comm=%s, int keyval=%d)", comm_to_string(comm), keyval);
   return MPI_ERR_INTERN;
#endif // __IMPI_Attr_delete_FORWARD
}
#endif // __IMPI_Attr_delete


#ifndef __IMPI_Attr_get
int IMPI_Attr_get ( MPI_Comm comm, int keyval, void *attr_value, int *flag )
{
#if __IMPI_Attr_get_FORWARD >= 1
   return PMPI_Attr_get(comm, keyval, attr_value, flag);
#else
   FATAL("NOT IMPLEMENTED: MPI_Attr_get (MPI_Comm comm=%s, int keyval=%d, void *attr_value=%p, int *flag=%p)", comm_to_string(comm), keyval, attr_value, flag);
   return MPI_ERR_INTERN;
#endif // __IMPI_Attr_get_FORWARD
}
#endif // __IMPI_Attr_get


#ifndef __IMPI_Attr_put
int IMPI_Attr_put ( MPI_Comm comm, int keyval, void *attr_value )
{
#if __IMPI_Attr_put_FORWARD >= 1
   return PMPI_Attr_put(comm, keyval, attr_value);
#else
   FATAL("NOT IMPLEMENTED: MPI_Attr_put (MPI_Comm comm=%s, int keyval=%d, void *attr_value=%p)", comm_to_string(comm), keyval, attr_value);
   return MPI_ERR_INTERN;
#endif // __IMPI_Attr_put_FORWARD
}
#endif // __IMPI_Attr_put


#ifndef __IMPI_Barrier
int IMPI_Barrier ( MPI_Comm comm )
{
#if __IMPI_Barrier_FORWARD >= 1
   return PMPI_Barrier(comm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Barrier (MPI_Comm comm=%s)", comm_to_string(comm));
   return MPI_ERR_INTERN;
#endif // __IMPI_Barrier_FORWARD
}
#endif // __IMPI_Barrier


#ifndef __IMPI_Bcast
int IMPI_Bcast ( void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm )
{
#if __IMPI_Bcast_FORWARD >= 1
   return PMPI_Bcast(buffer, count, datatype, root, comm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Bcast (void *buffer=%p, int count=%d, MPI_Datatype datatype=%s, int root=%d, MPI_Comm comm=%s)", buffer, count, type_to_string(datatype), root, comm_to_string(comm));
   return MPI_ERR_INTERN;
#endif // __IMPI_Bcast_FORWARD
}
#endif // __IMPI_Bcast


#ifndef __IMPI_Bsend
int IMPI_Bsend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm )
{
#if __IMPI_Bsend_FORWARD >= 1
   return PMPI_Bsend(buf, count, datatype, dest, tag, comm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Bsend (void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm));
   return MPI_ERR_INTERN;
#endif // __IMPI_Bsend_FORWARD
}
#endif // __IMPI_Bsend


#ifndef __IMPI_Bsend_init
int IMPI_Bsend_init ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request )
{
#if __IMPI_Bsend_init_FORWARD >= 1
   return PMPI_Bsend_init(buf, count, datatype, dest, tag, comm, request);
#else
   FATAL("NOT IMPLEMENTED: MPI_Bsend_init (void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s, MPI_Request *request=%p)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm), request);
   return MPI_ERR_INTERN;
#endif // __IMPI_Bsend_init_FORWARD
}
#endif // __IMPI_Bsend_init


#ifndef __IMPI_Buffer_attach
int IMPI_Buffer_attach ( void *buffer, int size )
{
#if __IMPI_Buffer_attach_FORWARD >= 1
   return PMPI_Buffer_attach(buffer, size);
#else
   FATAL("NOT IMPLEMENTED: MPI_Buffer_attach (void *buffer=%p, int size=%d)", buffer, size);
   return MPI_ERR_INTERN;
#endif // __IMPI_Buffer_attach_FORWARD
}
#endif // __IMPI_Buffer_attach


#ifndef __IMPI_Buffer_detach
int IMPI_Buffer_detach ( void *buffer, int *size )
{
#if __IMPI_Buffer_detach_FORWARD >= 1
   return PMPI_Buffer_detach(buffer, size);
#else
   FATAL("NOT IMPLEMENTED: MPI_Buffer_detach (void *buffer=%p, int *size=%p)", buffer, size);
   return MPI_ERR_INTERN;
#endif // __IMPI_Buffer_detach_FORWARD
}
#endif // __IMPI_Buffer_detach


#ifndef __IMPI_Cancel
int IMPI_Cancel ( MPI_Request *request )
{
#if __IMPI_Cancel_FORWARD >= 1
   return PMPI_Cancel(request);
#else
   FATAL("NOT IMPLEMENTED: MPI_Cancel (MPI_Request *request=%p)", request);
   return MPI_ERR_INTERN;
#endif // __IMPI_Cancel_FORWARD
}
#endif // __IMPI_Cancel


#ifndef __IMPI_Cart_coords
int IMPI_Cart_coords ( MPI_Comm comm, int rank, int maxdims, int *coords )
{
#if __IMPI_Cart_coords_FORWARD >= 1
   return PMPI_Cart_coords(comm, rank, maxdims, coords);
#else
   FATAL("NOT IMPLEMENTED: MPI_Cart_coords (MPI_Comm comm=%s, int rank=%d, int maxdims=%d, int *coords=%p)", comm_to_string(comm), rank, maxdims, coords);
   return MPI_ERR_INTERN;
#endif // __IMPI_Cart_coords_FORWARD
}
#endif // __IMPI_Cart_coords


#ifndef __IMPI_Cart_create
int IMPI_Cart_create ( MPI_Comm comm_old, int ndims, int *dims, int *periods, int reorder, MPI_Comm *comm_cart )
{
#if __IMPI_Cart_create_FORWARD >= 1
   return PMPI_Cart_create(comm_old, ndims, dims, periods, reorder, comm_cart);
#else
   FATAL("NOT IMPLEMENTED: MPI_Cart_create (MPI_Comm comm_old=%s, int ndims=%d, int *dims=%p, int *periods=%p, int reorder=%d, MPI_Comm *comm_cart=%p)", comm_to_string(comm_old), ndims, dims, periods, reorder, comm_cart);
   return MPI_ERR_INTERN;
#endif // __IMPI_Cart_create_FORWARD
}
#endif // __IMPI_Cart_create


#ifndef __IMPI_Cartdim_get
int IMPI_Cartdim_get ( MPI_Comm comm, int *ndims )
{
#if __IMPI_Cartdim_get_FORWARD >= 1
   return PMPI_Cartdim_get(comm, ndims);
#else
   FATAL("NOT IMPLEMENTED: MPI_Cartdim_get (MPI_Comm comm=%s, int *ndims=%p)", comm_to_string(comm), ndims);
   return MPI_ERR_INTERN;
#endif // __IMPI_Cartdim_get_FORWARD
}
#endif // __IMPI_Cartdim_get


#ifndef __IMPI_Cart_get
int IMPI_Cart_get ( MPI_Comm comm, int maxdims, int *dims, int *periods, int *coords )
{
#if __IMPI_Cart_get_FORWARD >= 1
   return PMPI_Cart_get(comm, maxdims, dims, periods, coords);
#else
   FATAL("NOT IMPLEMENTED: MPI_Cart_get (MPI_Comm comm=%s, int maxdims=%d, int *dims=%p, int *periods=%p, int *coords=%p)", comm_to_string(comm), maxdims, dims, periods, coords);
   return MPI_ERR_INTERN;
#endif // __IMPI_Cart_get_FORWARD
}
#endif // __IMPI_Cart_get


#ifndef __IMPI_Cart_map
int IMPI_Cart_map ( MPI_Comm comm_old, int ndims, int *dims, int *periods, int *newrank )
{
#if __IMPI_Cart_map_FORWARD >= 1
   return PMPI_Cart_map(comm_old, ndims, dims, periods, newrank);
#else
   FATAL("NOT IMPLEMENTED: MPI_Cart_map (MPI_Comm comm_old=%s, int ndims=%d, int *dims=%p, int *periods=%p, int *newrank=%p)", comm_to_string(comm_old), ndims, dims, periods, newrank);
   return MPI_ERR_INTERN;
#endif // __IMPI_Cart_map_FORWARD
}
#endif // __IMPI_Cart_map


#ifndef __IMPI_Cart_rank
int IMPI_Cart_rank ( MPI_Comm comm, int *coords, int *rank )
{
#if __IMPI_Cart_rank_FORWARD >= 1
   return PMPI_Cart_rank(comm, coords, rank);
#else
   FATAL("NOT IMPLEMENTED: MPI_Cart_rank (MPI_Comm comm=%s, int *coords=%p, int *rank=%p)", comm_to_string(comm), coords, rank);
   return MPI_ERR_INTERN;
#endif // __IMPI_Cart_rank_FORWARD
}
#endif // __IMPI_Cart_rank


#ifndef __IMPI_Cart_shift
int IMPI_Cart_shift ( MPI_Comm comm, int direction, int displ, int *source, int *dest )
{
#if __IMPI_Cart_shift_FORWARD >= 1
   return PMPI_Cart_shift(comm, direction, displ, source, dest);
#else
   FATAL("NOT IMPLEMENTED: MPI_Cart_shift (MPI_Comm comm=%s, int direction=%d, int displ=%d, int *source=%p, int *dest=%p)", comm_to_string(comm), direction, displ, source, dest);
   return MPI_ERR_INTERN;
#endif // __IMPI_Cart_shift_FORWARD
}
#endif // __IMPI_Cart_shift


#ifndef __IMPI_Cart_sub
int IMPI_Cart_sub ( MPI_Comm comm, int *remain_dims, MPI_Comm *comm_new )
{
#if __IMPI_Cart_sub_FORWARD >= 1
   return PMPI_Cart_sub(comm, remain_dims, comm_new);
#else
   FATAL("NOT IMPLEMENTED: MPI_Cart_sub (MPI_Comm comm=%s, int *remain_dims=%p, MPI_Comm *comm_new=%p)", comm_to_string(comm), remain_dims, comm_new);
   return MPI_ERR_INTERN;
#endif // __IMPI_Cart_sub_FORWARD
}
#endif // __IMPI_Cart_sub


#ifndef __IMPI_Close_port
int IMPI_Close_port ( char *port_name )
{
#if __IMPI_Close_port_FORWARD >= 1
   return PMPI_Close_port(port_name);
#else
   FATAL("NOT IMPLEMENTED: MPI_Close_port (char *port_name=%p)", port_name);
   return MPI_ERR_INTERN;
#endif // __IMPI_Close_port_FORWARD
}
#endif // __IMPI_Close_port


#ifndef __IMPI_Comm_accept
int IMPI_Comm_accept ( char *port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *newcomm )
{
#if __IMPI_Comm_accept_FORWARD >= 1
   return PMPI_Comm_accept(port_name, info, root, comm, newcomm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Comm_accept (char *port_name=%p, MPI_Info info=%s, int root=%d, MPI_Comm comm=%s, MPI_Comm *newcomm=%p)", port_name, info_to_string(info), root, comm_to_string(comm), newcomm);
   return MPI_ERR_INTERN;
#endif // __IMPI_Comm_accept_FORWARD
}
#endif // __IMPI_Comm_accept


#ifndef __IMPI_Comm_call_errhandler
int IMPI_Comm_call_errhandler ( MPI_Comm comm, int errorcode )
{
#if __IMPI_Comm_call_errhandler_FORWARD >= 1
   return PMPI_Comm_call_errhandler(comm, errorcode);
#else
   FATAL("NOT IMPLEMENTED: MPI_Comm_call_errhandler (MPI_Comm comm=%s, int errorcode=%d)", comm_to_string(comm), errorcode);
   return MPI_ERR_INTERN;
#endif // __IMPI_Comm_call_errhandler_FORWARD
}
#endif // __IMPI_Comm_call_errhandler


#ifndef __IMPI_Comm_compare
int IMPI_Comm_compare ( MPI_Comm comm1, MPI_Comm comm2, int *result )
{
#if __IMPI_Comm_compare_FORWARD >= 1
   return PMPI_Comm_compare(comm1, comm2, result);
#else
   FATAL("NOT IMPLEMENTED: MPI_Comm_compare (MPI_Comm comm1=%s, MPI_Comm comm2=%s, int *result=%p)", comm_to_string(comm1), comm_to_string(comm2), result);
   return MPI_ERR_INTERN;
#endif // __IMPI_Comm_compare_FORWARD
}
#endif // __IMPI_Comm_compare


#ifndef __IMPI_Comm_connect
int IMPI_Comm_connect ( char *port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *newcomm )
{
#if __IMPI_Comm_connect_FORWARD >= 1
   return PMPI_Comm_connect(port_name, info, root, comm, newcomm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Comm_connect (char *port_name=%p, MPI_Info info=%s, int root=%d, MPI_Comm comm=%s, MPI_Comm *newcomm=%p)", port_name, info_to_string(info), root, comm_to_string(comm), newcomm);
   return MPI_ERR_INTERN;
#endif // __IMPI_Comm_connect_FORWARD
}
#endif // __IMPI_Comm_connect


#ifndef __IMPI_Comm_create_errhandler
int IMPI_Comm_create_errhandler ( MPI_Comm_errhandler_fn *function, MPI_Errhandler *errhandler )
{
#if __IMPI_Comm_create_errhandler_FORWARD >= 1
   return PMPI_Comm_create_errhandler(function, errhandler);
#else
   FATAL("NOT IMPLEMENTED: MPI_Comm_create_errhandler (MPI_Comm_errhandler_fn *function=%p, MPI_Errhandler *errhandler=%p)", function, errhandler);
   return MPI_ERR_INTERN;
#endif // __IMPI_Comm_create_errhandler_FORWARD
}
#endif // __IMPI_Comm_create_errhandler


#ifndef __IMPI_Comm_create
int IMPI_Comm_create ( MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm )
{
#if __IMPI_Comm_create_FORWARD >= 1
   return PMPI_Comm_create(comm, group, newcomm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Comm_create (MPI_Comm comm=%s, MPI_Group group=%s, MPI_Comm *newcomm=%p)", comm_to_string(comm), group_to_string(group), newcomm);
   return MPI_ERR_INTERN;
#endif // __IMPI_Comm_create_FORWARD
}
#endif // __IMPI_Comm_create


#ifndef __IMPI_Comm_create_keyval
int IMPI_Comm_create_keyval ( MPI_Comm_copy_attr_function *comm_copy_attr_fn, MPI_Comm_delete_attr_function *comm_delete_attr_fn, int *comm_keyval, void *extra_state )
{
#if __IMPI_Comm_create_keyval_FORWARD >= 1
   return PMPI_Comm_create_keyval(comm_copy_attr_fn, comm_delete_attr_fn, comm_keyval, extra_state);
#else
   FATAL("NOT IMPLEMENTED: MPI_Comm_create_keyval (MPI_Comm_copy_attr_function *comm_copy_attr_fn=%p, MPI_Comm_delete_attr_function *comm_delete_attr_fn=%p, int *comm_keyval=%p, void *extra_state=%p)", comm_copy_attr_fn, comm_delete_attr_fn, comm_keyval, extra_state);
   return MPI_ERR_INTERN;
#endif // __IMPI_Comm_create_keyval_FORWARD
}
#endif // __IMPI_Comm_create_keyval


#ifndef __IMPI_Comm_delete_attr
int IMPI_Comm_delete_attr ( MPI_Comm comm, int comm_keyval )
{
#if __IMPI_Comm_delete_attr_FORWARD >= 1
   return PMPI_Comm_delete_attr(comm, comm_keyval);
#else
   FATAL("NOT IMPLEMENTED: MPI_Comm_delete_attr (MPI_Comm comm=%s, int comm_keyval=%d)", comm_to_string(comm), comm_keyval);
   return MPI_ERR_INTERN;
#endif // __IMPI_Comm_delete_attr_FORWARD
}
#endif // __IMPI_Comm_delete_attr


#ifndef __IMPI_Comm_disconnect
int IMPI_Comm_disconnect ( MPI_Comm *comm )
{
#if __IMPI_Comm_disconnect_FORWARD >= 1
   return PMPI_Comm_disconnect(comm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Comm_disconnect (MPI_Comm *comm=%p)", comm);
   return MPI_ERR_INTERN;
#endif // __IMPI_Comm_disconnect_FORWARD
}
#endif // __IMPI_Comm_disconnect


#ifndef __IMPI_Comm_dup
int IMPI_Comm_dup ( MPI_Comm comm, MPI_Comm *newcomm )
{
#if __IMPI_Comm_dup_FORWARD >= 1
   return PMPI_Comm_dup(comm, newcomm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Comm_dup (MPI_Comm comm=%s, MPI_Comm *newcomm=%p)", comm_to_string(comm), newcomm);
   return MPI_ERR_INTERN;
#endif // __IMPI_Comm_dup_FORWARD
}
#endif // __IMPI_Comm_dup


#ifndef __IMPI_Comm_free
int IMPI_Comm_free ( MPI_Comm *comm )
{
#if __IMPI_Comm_free_FORWARD >= 1
   return PMPI_Comm_free(comm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Comm_free (MPI_Comm *comm=%p)", comm);
   return MPI_ERR_INTERN;
#endif // __IMPI_Comm_free_FORWARD
}
#endif // __IMPI_Comm_free


#ifndef __IMPI_Comm_free_keyval
int IMPI_Comm_free_keyval ( int *comm_keyval )
{
#if __IMPI_Comm_free_keyval_FORWARD >= 1
   return PMPI_Comm_free_keyval(comm_keyval);
#else
   FATAL("NOT IMPLEMENTED: MPI_Comm_free_keyval (int *comm_keyval=%p)", comm_keyval);
   return MPI_ERR_INTERN;
#endif // __IMPI_Comm_free_keyval_FORWARD
}
#endif // __IMPI_Comm_free_keyval


#ifndef __IMPI_Comm_get_attr
int IMPI_Comm_get_attr ( MPI_Comm comm, int comm_keyval, void *attribute_val, int *flag )
{
#if __IMPI_Comm_get_attr_FORWARD >= 1
   return PMPI_Comm_get_attr(comm, comm_keyval, attribute_val, flag);
#else
   FATAL("NOT IMPLEMENTED: MPI_Comm_get_attr (MPI_Comm comm=%s, int comm_keyval=%d, void *attribute_val=%p, int *flag=%p)", comm_to_string(comm), comm_keyval, attribute_val, flag);
   return MPI_ERR_INTERN;
#endif // __IMPI_Comm_get_attr_FORWARD
}
#endif // __IMPI_Comm_get_attr


#ifndef __IMPI_Comm_get_errhandler
int IMPI_Comm_get_errhandler ( MPI_Comm comm, MPI_Errhandler *errhandler )
{
#if __IMPI_Comm_get_errhandler_FORWARD >= 1
   return PMPI_Comm_get_errhandler(comm, errhandler);
#else
   FATAL("NOT IMPLEMENTED: MPI_Comm_get_errhandler (MPI_Comm comm=%s, MPI_Errhandler *errhandler=%p)", comm_to_string(comm), errhandler);
   return MPI_ERR_INTERN;
#endif // __IMPI_Comm_get_errhandler_FORWARD
}
#endif // __IMPI_Comm_get_errhandler


#ifndef __IMPI_Comm_get_name
int IMPI_Comm_get_name ( MPI_Comm comm, char *comm_name, int *resultlen )
{
#if __IMPI_Comm_get_name_FORWARD >= 1
   return PMPI_Comm_get_name(comm, comm_name, resultlen);
#else
   FATAL("NOT IMPLEMENTED: MPI_Comm_get_name (MPI_Comm comm=%s, char *comm_name=%p, int *resultlen=%p)", comm_to_string(comm), comm_name, resultlen);
   return MPI_ERR_INTERN;
#endif // __IMPI_Comm_get_name_FORWARD
}
#endif // __IMPI_Comm_get_name


#ifndef __IMPI_Comm_get_parent
int IMPI_Comm_get_parent ( MPI_Comm *parent )
{
#if __IMPI_Comm_get_parent_FORWARD >= 1
   return PMPI_Comm_get_parent(parent);
#else
   FATAL("NOT IMPLEMENTED: MPI_Comm_get_parent (MPI_Comm *parent=%p)", parent);
   return MPI_ERR_INTERN;
#endif // __IMPI_Comm_get_parent_FORWARD
}
#endif // __IMPI_Comm_get_parent


#ifndef __IMPI_Comm_group
int IMPI_Comm_group ( MPI_Comm comm, MPI_Group *group )
{
#if __IMPI_Comm_group_FORWARD >= 1
   return PMPI_Comm_group(comm, group);
#else
   FATAL("NOT IMPLEMENTED: MPI_Comm_group (MPI_Comm comm=%s, MPI_Group *group=%p)", comm_to_string(comm), group);
   return MPI_ERR_INTERN;
#endif // __IMPI_Comm_group_FORWARD
}
#endif // __IMPI_Comm_group


#ifndef __IMPI_Comm_join
int IMPI_Comm_join ( int fd, MPI_Comm *intercomm )
{
#if __IMPI_Comm_join_FORWARD >= 1
   return PMPI_Comm_join(fd, intercomm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Comm_join (int fd=%d, MPI_Comm *intercomm=%p)", fd, intercomm);
   return MPI_ERR_INTERN;
#endif // __IMPI_Comm_join_FORWARD
}
#endif // __IMPI_Comm_join


#ifndef __IMPI_Comm_rank
int IMPI_Comm_rank ( MPI_Comm comm, int *rank )
{
#if __IMPI_Comm_rank_FORWARD >= 1
   return PMPI_Comm_rank(comm, rank);
#else
   FATAL("NOT IMPLEMENTED: MPI_Comm_rank (MPI_Comm comm=%s, int *rank=%p)", comm_to_string(comm), rank);
   return MPI_ERR_INTERN;
#endif // __IMPI_Comm_rank_FORWARD
}
#endif // __IMPI_Comm_rank


#ifndef __IMPI_Comm_remote_group
int IMPI_Comm_remote_group ( MPI_Comm comm, MPI_Group *group )
{
#if __IMPI_Comm_remote_group_FORWARD >= 1
   return PMPI_Comm_remote_group(comm, group);
#else
   FATAL("NOT IMPLEMENTED: MPI_Comm_remote_group (MPI_Comm comm=%s, MPI_Group *group=%p)", comm_to_string(comm), group);
   return MPI_ERR_INTERN;
#endif // __IMPI_Comm_remote_group_FORWARD
}
#endif // __IMPI_Comm_remote_group


#ifndef __IMPI_Comm_remote_size
int IMPI_Comm_remote_size ( MPI_Comm comm, int *size )
{
#if __IMPI_Comm_remote_size_FORWARD >= 1
   return PMPI_Comm_remote_size(comm, size);
#else
   FATAL("NOT IMPLEMENTED: MPI_Comm_remote_size (MPI_Comm comm=%s, int *size=%p)", comm_to_string(comm), size);
   return MPI_ERR_INTERN;
#endif // __IMPI_Comm_remote_size_FORWARD
}
#endif // __IMPI_Comm_remote_size


#ifndef __IMPI_Comm_set_attr
int IMPI_Comm_set_attr ( MPI_Comm comm, int comm_keyval, void *attribute_val )
{
#if __IMPI_Comm_set_attr_FORWARD >= 1
   return PMPI_Comm_set_attr(comm, comm_keyval, attribute_val);
#else
   FATAL("NOT IMPLEMENTED: MPI_Comm_set_attr (MPI_Comm comm=%s, int comm_keyval=%d, void *attribute_val=%p)", comm_to_string(comm), comm_keyval, attribute_val);
   return MPI_ERR_INTERN;
#endif // __IMPI_Comm_set_attr_FORWARD
}
#endif // __IMPI_Comm_set_attr


#ifndef __IMPI_Comm_set_errhandler
int IMPI_Comm_set_errhandler ( MPI_Comm comm, MPI_Errhandler errhandler )
{
#if __IMPI_Comm_set_errhandler_FORWARD >= 1
   return PMPI_Comm_set_errhandler(comm, errhandler);
#else
   FATAL("NOT IMPLEMENTED: MPI_Comm_set_errhandler (MPI_Comm comm=%s, MPI_Errhandler errhandler=%p)", comm_to_string(comm), (void *) errhandler);
   return MPI_ERR_INTERN;
#endif // __IMPI_Comm_set_errhandler_FORWARD
}
#endif // __IMPI_Comm_set_errhandler


#ifndef __IMPI_Comm_set_name
int IMPI_Comm_set_name ( MPI_Comm comm, char *comm_name )
{
#if __IMPI_Comm_set_name_FORWARD >= 1
   return PMPI_Comm_set_name(comm, comm_name);
#else
   FATAL("NOT IMPLEMENTED: MPI_Comm_set_name (MPI_Comm comm=%s, char *comm_name=%p)", comm_to_string(comm), comm_name);
   return MPI_ERR_INTERN;
#endif // __IMPI_Comm_set_name_FORWARD
}
#endif // __IMPI_Comm_set_name


#ifndef __IMPI_Comm_size
int IMPI_Comm_size ( MPI_Comm comm, int *size )
{
#if __IMPI_Comm_size_FORWARD >= 1
   return PMPI_Comm_size(comm, size);
#else
   FATAL("NOT IMPLEMENTED: MPI_Comm_size (MPI_Comm comm=%s, int *size=%p)", comm_to_string(comm), size);
   return MPI_ERR_INTERN;
#endif // __IMPI_Comm_size_FORWARD
}
#endif // __IMPI_Comm_size


#ifndef __IMPI_Comm_spawn
int IMPI_Comm_spawn ( char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[] )
{
#if __IMPI_Comm_spawn_FORWARD >= 1
   return PMPI_Comm_spawn(command, argv, maxprocs, info, root, comm, intercomm, array_of_errcodes);
#else
   FATAL("NOT IMPLEMENTED: MPI_Comm_spawn (char *command=%p, char *argv[]=%p, int maxprocs=%d, MPI_Info info=%s, int root=%d, MPI_Comm comm=%s, MPI_Comm *intercomm=%p, int array_of_errcodes[]=%p)", command, argv, maxprocs, info_to_string(info), root, comm_to_string(comm), intercomm, array_of_errcodes);
   return MPI_ERR_INTERN;
#endif // __IMPI_Comm_spawn_FORWARD
}
#endif // __IMPI_Comm_spawn


#ifndef __IMPI_Comm_spawn_multiple
int IMPI_Comm_spawn_multiple ( int count, char *array_of_commands[], char* *array_of_argv[], int array_of_maxprocs[], MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[] )
{
#if __IMPI_Comm_spawn_multiple_FORWARD >= 1
   return PMPI_Comm_spawn_multiple(count, array_of_commands, array_of_argv, array_of_maxprocs, array_of_info, root, comm, intercomm, array_of_errcodes);
#else
   FATAL("NOT IMPLEMENTED: MPI_Comm_spawn_multiple (int count=%d, char *array_of_commands[]=%p, char* *array_of_argv[]=%p, int array_of_maxprocs[]=%p, MPI_Info array_of_info[]=%p, int root=%d, MPI_Comm comm=%s, MPI_Comm *intercomm=%p, int array_of_errcodes[]=%p)", count, array_of_commands, array_of_argv, array_of_maxprocs, array_of_info, root, comm_to_string(comm), intercomm, array_of_errcodes);
   return MPI_ERR_INTERN;
#endif // __IMPI_Comm_spawn_multiple_FORWARD
}
#endif // __IMPI_Comm_spawn_multiple


#ifndef __IMPI_Comm_split
int IMPI_Comm_split ( MPI_Comm comm, int color, int key, MPI_Comm *newcomm )
{
#if __IMPI_Comm_split_FORWARD >= 1
   return PMPI_Comm_split(comm, color, key, newcomm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Comm_split (MPI_Comm comm=%s, int color=%d, int key=%d, MPI_Comm *newcomm=%p)", comm_to_string(comm), color, key, newcomm);
   return MPI_ERR_INTERN;
#endif // __IMPI_Comm_split_FORWARD
}
#endif // __IMPI_Comm_split


#ifndef __IMPI_Comm_test_inter
int IMPI_Comm_test_inter ( MPI_Comm comm, int *flag )
{
#if __IMPI_Comm_test_inter_FORWARD >= 1
   return PMPI_Comm_test_inter(comm, flag);
#else
   FATAL("NOT IMPLEMENTED: MPI_Comm_test_inter (MPI_Comm comm=%s, int *flag=%p)", comm_to_string(comm), flag);
   return MPI_ERR_INTERN;
#endif // __IMPI_Comm_test_inter_FORWARD
}
#endif // __IMPI_Comm_test_inter


#ifndef __IMPI_Dims_create
int IMPI_Dims_create ( int nnodes, int ndims, int *dims )
{
#if __IMPI_Dims_create_FORWARD >= 1
   return PMPI_Dims_create(nnodes, ndims, dims);
#else
   FATAL("NOT IMPLEMENTED: MPI_Dims_create (int nnodes=%d, int ndims=%d, int *dims=%p)", nnodes, ndims, dims);
   return MPI_ERR_INTERN;
#endif // __IMPI_Dims_create_FORWARD
}
#endif // __IMPI_Dims_create


#ifndef __IMPI_Dist_graph_create_adjacent
int IMPI_Dist_graph_create_adjacent ( MPI_Comm comm_old, int indegree, int sources[], int sourceweights[], int outdegree, int destinations[], int destweights[], MPI_Info info, int reorder, MPI_Comm *comm_dist_graph )
{
#if __IMPI_Dist_graph_create_adjacent_FORWARD >= 1
   return PMPI_Dist_graph_create_adjacent(comm_old, indegree, sources, sourceweights, outdegree, destinations, destweights, info, reorder, comm_dist_graph);
#else
   FATAL("NOT IMPLEMENTED: MPI_Dist_graph_create_adjacent (MPI_Comm comm_old=%s, int indegree=%d, int sources[]=%p, int sourceweights[]=%p, int outdegree=%d, int destinations[]=%p, int destweights[]=%p, MPI_Info info=%s, int reorder=%d, MPI_Comm *comm_dist_graph=%p)", comm_to_string(comm_old), indegree, sources, sourceweights, outdegree, destinations, destweights, info_to_string(info), reorder, comm_dist_graph);
   return MPI_ERR_INTERN;
#endif // __IMPI_Dist_graph_create_adjacent_FORWARD
}
#endif // __IMPI_Dist_graph_create_adjacent


#ifndef __IMPI_Dist_graph_create
int IMPI_Dist_graph_create ( MPI_Comm comm_old, int n, int sources[], int degrees[], int destinations[], int weights[], MPI_Info info, int reorder, MPI_Comm *comm_dist_graph )
{
#if __IMPI_Dist_graph_create_FORWARD >= 1
   return PMPI_Dist_graph_create(comm_old, n, sources, degrees, destinations, weights, info, reorder, comm_dist_graph);
#else
   FATAL("NOT IMPLEMENTED: MPI_Dist_graph_create (MPI_Comm comm_old=%s, int n=%d, int sources[]=%p, int degrees[]=%p, int destinations[]=%p, int weights[]=%p, MPI_Info info=%s, int reorder=%d, MPI_Comm *comm_dist_graph=%p)", comm_to_string(comm_old), n, sources, degrees, destinations, weights, info_to_string(info), reorder, comm_dist_graph);
   return MPI_ERR_INTERN;
#endif // __IMPI_Dist_graph_create_FORWARD
}
#endif // __IMPI_Dist_graph_create


#ifndef __IMPI_Dist_graph_neighbors_count
int IMPI_Dist_graph_neighbors_count ( MPI_Comm comm, int *indegree, int *outdegree, int *weighted )
{
#if __IMPI_Dist_graph_neighbors_count_FORWARD >= 1
   return PMPI_Dist_graph_neighbors_count(comm, indegree, outdegree, weighted);
#else
   FATAL("NOT IMPLEMENTED: MPI_Dist_graph_neighbors_count (MPI_Comm comm=%s, int *indegree=%p, int *outdegree=%p, int *weighted=%p)", comm_to_string(comm), indegree, outdegree, weighted);
   return MPI_ERR_INTERN;
#endif // __IMPI_Dist_graph_neighbors_count_FORWARD
}
#endif // __IMPI_Dist_graph_neighbors_count


#ifndef __IMPI_Dist_graph_neighbors
int IMPI_Dist_graph_neighbors ( MPI_Comm comm, int maxindegree, int sources[], int sourceweights[], int maxoutdegree, int destinations[], int destweights[] )
{
#if __IMPI_Dist_graph_neighbors_FORWARD >= 1
   return PMPI_Dist_graph_neighbors(comm, maxindegree, sources, sourceweights, maxoutdegree, destinations, destweights);
#else
   FATAL("NOT IMPLEMENTED: MPI_Dist_graph_neighbors (MPI_Comm comm=%s, int maxindegree=%d, int sources[]=%p, int sourceweights[]=%p, int maxoutdegree=%d, int destinations[]=%p, int destweights[]=%p)", comm_to_string(comm), maxindegree, sources, sourceweights, maxoutdegree, destinations, destweights);
   return MPI_ERR_INTERN;
#endif // __IMPI_Dist_graph_neighbors_FORWARD
}
#endif // __IMPI_Dist_graph_neighbors


#ifndef __IMPI_Errhandler_create
int IMPI_Errhandler_create ( MPI_Handler_function *function, MPI_Errhandler *errhandler )
{
#if __IMPI_Errhandler_create_FORWARD >= 1
   return PMPI_Errhandler_create(function, errhandler);
#else
   FATAL("NOT IMPLEMENTED: MPI_Errhandler_create (MPI_Handler_function *function=%p, MPI_Errhandler *errhandler=%p)", function, errhandler);
   return MPI_ERR_INTERN;
#endif // __IMPI_Errhandler_create_FORWARD
}
#endif // __IMPI_Errhandler_create


#ifndef __IMPI_Errhandler_free
int IMPI_Errhandler_free ( MPI_Errhandler *errhandler )
{
#if __IMPI_Errhandler_free_FORWARD >= 1
   return PMPI_Errhandler_free(errhandler);
#else
   FATAL("NOT IMPLEMENTED: MPI_Errhandler_free (MPI_Errhandler *errhandler=%p)", errhandler);
   return MPI_ERR_INTERN;
#endif // __IMPI_Errhandler_free_FORWARD
}
#endif // __IMPI_Errhandler_free


#ifndef __IMPI_Errhandler_get
int IMPI_Errhandler_get ( MPI_Comm comm, MPI_Errhandler *errhandler )
{
#if __IMPI_Errhandler_get_FORWARD >= 1
   return PMPI_Errhandler_get(comm, errhandler);
#else
   FATAL("NOT IMPLEMENTED: MPI_Errhandler_get (MPI_Comm comm=%s, MPI_Errhandler *errhandler=%p)", comm_to_string(comm), errhandler);
   return MPI_ERR_INTERN;
#endif // __IMPI_Errhandler_get_FORWARD
}
#endif // __IMPI_Errhandler_get


#ifndef __IMPI_Errhandler_set
int IMPI_Errhandler_set ( MPI_Comm comm, MPI_Errhandler errhandler )
{
#if __IMPI_Errhandler_set_FORWARD >= 1
   return PMPI_Errhandler_set(comm, errhandler);
#else
   FATAL("NOT IMPLEMENTED: MPI_Errhandler_set (MPI_Comm comm=%s, MPI_Errhandler errhandler=%p)", comm_to_string(comm), (void *) errhandler);
   return MPI_ERR_INTERN;
#endif // __IMPI_Errhandler_set_FORWARD
}
#endif // __IMPI_Errhandler_set


#ifndef __IMPI_Error_class
int IMPI_Error_class ( int errorcode, int *errorclass )
{
#if __IMPI_Error_class_FORWARD >= 1
   return PMPI_Error_class(errorcode, errorclass);
#else
   FATAL("NOT IMPLEMENTED: MPI_Error_class (int errorcode=%d, int *errorclass=%p)", errorcode, errorclass);
   return MPI_ERR_INTERN;
#endif // __IMPI_Error_class_FORWARD
}
#endif // __IMPI_Error_class


#ifndef __IMPI_Error_string
int IMPI_Error_string ( int errorcode, char *string, int *resultlen )
{
#if __IMPI_Error_string_FORWARD >= 1
   return PMPI_Error_string(errorcode, string, resultlen);
#else
   FATAL("NOT IMPLEMENTED: MPI_Error_string (int errorcode=%d, char *string=%p, int *resultlen=%p)", errorcode, string, resultlen);
   return MPI_ERR_INTERN;
#endif // __IMPI_Error_string_FORWARD
}
#endif // __IMPI_Error_string


#ifndef __IMPI_Exscan
int IMPI_Exscan ( void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm )
{
#if __IMPI_Exscan_FORWARD >= 1
   return PMPI_Exscan(sendbuf, recvbuf, count, datatype, op, comm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Exscan (void *sendbuf=%p, void *recvbuf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Op op=%s, MPI_Comm comm=%s)", sendbuf, recvbuf, count, type_to_string(datatype), op_to_string(op), comm_to_string(comm));
   return MPI_ERR_INTERN;
#endif // __IMPI_Exscan_FORWARD
}
#endif // __IMPI_Exscan


#ifndef __IMPI_File_call_errhandler
int IMPI_File_call_errhandler ( MPI_File fh, int errorcode )
{
#if __IMPI_File_call_errhandler_FORWARD >= 1
   return PMPI_File_call_errhandler(fh, errorcode);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_call_errhandler (MPI_File fh=%s, int errorcode=%d)", file_to_string(fh), errorcode);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_call_errhandler_FORWARD
}
#endif // __IMPI_File_call_errhandler


#ifndef __IMPI_File_close
int IMPI_File_close ( MPI_File *mpi_fh )
{
#if __IMPI_File_close_FORWARD >= 1
   return PMPI_File_close(mpi_fh);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_close (MPI_File *mpi_fh=%p)", mpi_fh);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_close_FORWARD
}
#endif // __IMPI_File_close


#ifndef __IMPI_File_create_errhandler
int IMPI_File_create_errhandler ( MPI_File_errhandler_fn *function, MPI_Errhandler *errhandler )
{
#if __IMPI_File_create_errhandler_FORWARD >= 1
   return PMPI_File_create_errhandler(function, errhandler);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_create_errhandler (MPI_File_errhandler_fn *function=%p, MPI_Errhandler *errhandler=%p)", function, errhandler);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_create_errhandler_FORWARD
}
#endif // __IMPI_File_create_errhandler


#ifndef __IMPI_File_delete
int IMPI_File_delete ( char *filename, MPI_Info info )
{
#if __IMPI_File_delete_FORWARD >= 1
   return PMPI_File_delete(filename, info);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_delete (char *filename=%p, MPI_Info info=%s)", filename, info_to_string(info));
   return MPI_ERR_INTERN;
#endif // __IMPI_File_delete_FORWARD
}
#endif // __IMPI_File_delete


#ifndef __IMPI_File_get_amode
int IMPI_File_get_amode ( MPI_File mpi_fh, int *amode )
{
#if __IMPI_File_get_amode_FORWARD >= 1
   return PMPI_File_get_amode(mpi_fh, amode);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_get_amode (MPI_File mpi_fh=%s, int *amode=%p)", file_to_string(mpi_fh), amode);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_get_amode_FORWARD
}
#endif // __IMPI_File_get_amode


#ifndef __IMPI_File_get_atomicity
int IMPI_File_get_atomicity ( MPI_File mpi_fh, int *flag )
{
#if __IMPI_File_get_atomicity_FORWARD >= 1
   return PMPI_File_get_atomicity(mpi_fh, flag);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_get_atomicity (MPI_File mpi_fh=%s, int *flag=%p)", file_to_string(mpi_fh), flag);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_get_atomicity_FORWARD
}
#endif // __IMPI_File_get_atomicity


#ifndef __IMPI_File_get_byte_offset
int IMPI_File_get_byte_offset ( MPI_File mpi_fh, MPI_Offset offset, MPI_Offset *disp )
{
#if __IMPI_File_get_byte_offset_FORWARD >= 1
   return PMPI_File_get_byte_offset(mpi_fh, offset, disp);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_get_byte_offset (MPI_File mpi_fh=%s, MPI_Offset offset=%p, MPI_Offset *disp=%p)", file_to_string(mpi_fh), (void *) offset, disp);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_get_byte_offset_FORWARD
}
#endif // __IMPI_File_get_byte_offset


#ifndef __IMPI_File_get_errhandler
int IMPI_File_get_errhandler ( MPI_File file, MPI_Errhandler *errhandler )
{
#if __IMPI_File_get_errhandler_FORWARD >= 1
   return PMPI_File_get_errhandler(file, errhandler);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_get_errhandler (MPI_File file=%s, MPI_Errhandler *errhandler=%p)", file_to_string(file), errhandler);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_get_errhandler_FORWARD
}
#endif // __IMPI_File_get_errhandler


#ifndef __IMPI_File_get_group
int IMPI_File_get_group ( MPI_File mpi_fh, MPI_Group *group )
{
#if __IMPI_File_get_group_FORWARD >= 1
   return PMPI_File_get_group(mpi_fh, group);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_get_group (MPI_File mpi_fh=%s, MPI_Group *group=%p)", file_to_string(mpi_fh), group);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_get_group_FORWARD
}
#endif // __IMPI_File_get_group


#ifndef __IMPI_File_get_info
int IMPI_File_get_info ( MPI_File mpi_fh, MPI_Info *info_used )
{
#if __IMPI_File_get_info_FORWARD >= 1
   return PMPI_File_get_info(mpi_fh, info_used);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_get_info (MPI_File mpi_fh=%s, MPI_Info *info_used=%p)", file_to_string(mpi_fh), info_used);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_get_info_FORWARD
}
#endif // __IMPI_File_get_info


#ifndef __IMPI_File_get_position
int IMPI_File_get_position ( MPI_File mpi_fh, MPI_Offset *offset )
{
#if __IMPI_File_get_position_FORWARD >= 1
   return PMPI_File_get_position(mpi_fh, offset);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_get_position (MPI_File mpi_fh=%s, MPI_Offset *offset=%p)", file_to_string(mpi_fh), offset);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_get_position_FORWARD
}
#endif // __IMPI_File_get_position


#ifndef __IMPI_File_get_position_shared
int IMPI_File_get_position_shared ( MPI_File mpi_fh, MPI_Offset *offset )
{
#if __IMPI_File_get_position_shared_FORWARD >= 1
   return PMPI_File_get_position_shared(mpi_fh, offset);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_get_position_shared (MPI_File mpi_fh=%s, MPI_Offset *offset=%p)", file_to_string(mpi_fh), offset);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_get_position_shared_FORWARD
}
#endif // __IMPI_File_get_position_shared


#ifndef __IMPI_File_get_size
int IMPI_File_get_size ( MPI_File mpi_fh, MPI_Offset *size )
{
#if __IMPI_File_get_size_FORWARD >= 1
   return PMPI_File_get_size(mpi_fh, size);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_get_size (MPI_File mpi_fh=%s, MPI_Offset *size=%p)", file_to_string(mpi_fh), size);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_get_size_FORWARD
}
#endif // __IMPI_File_get_size


#ifndef __IMPI_File_get_type_extent
int IMPI_File_get_type_extent ( MPI_File mpi_fh, MPI_Datatype datatype, MPI_Aint *extent )
{
#if __IMPI_File_get_type_extent_FORWARD >= 1
   return PMPI_File_get_type_extent(mpi_fh, datatype, extent);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_get_type_extent (MPI_File mpi_fh=%s, MPI_Datatype datatype=%s, MPI_Aint *extent=%p)", file_to_string(mpi_fh), type_to_string(datatype), extent);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_get_type_extent_FORWARD
}
#endif // __IMPI_File_get_type_extent


#ifndef __IMPI_File_get_view
int IMPI_File_get_view ( MPI_File mpi_fh, MPI_Offset *disp, MPI_Datatype *etype, MPI_Datatype *filetype, char *datarep )
{
#if __IMPI_File_get_view_FORWARD >= 1
   return PMPI_File_get_view(mpi_fh, disp, etype, filetype, datarep);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_get_view (MPI_File mpi_fh=%s, MPI_Offset *disp=%p, MPI_Datatype *etype=%p, MPI_Datatype *filetype=%p, char *datarep=%p)", file_to_string(mpi_fh), disp, etype, filetype, datarep);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_get_view_FORWARD
}
#endif // __IMPI_File_get_view


#ifndef __IMPI_File_iread_at
int IMPI_File_iread_at ( MPI_File mpi_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPIO_Request *request )
{
#if __IMPI_File_iread_at_FORWARD >= 1
   return PMPI_File_iread_at(mpi_fh, offset, buf, count, datatype, request);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_iread_at (MPI_File mpi_fh=%s, MPI_Offset offset=%p, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPIO_Request *request=%p)", file_to_string(mpi_fh), (void *) offset, buf, count, type_to_string(datatype), request);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_iread_at_FORWARD
}
#endif // __IMPI_File_iread_at


#ifndef __IMPI_File_iread
int IMPI_File_iread ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *request )
{
#if __IMPI_File_iread_FORWARD >= 1
   return PMPI_File_iread(mpi_fh, buf, count, datatype, request);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_iread (MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Request *request=%p)", file_to_string(mpi_fh), buf, count, type_to_string(datatype), request);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_iread_FORWARD
}
#endif // __IMPI_File_iread


#ifndef __IMPI_File_iread_shared
int IMPI_File_iread_shared ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *request )
{
#if __IMPI_File_iread_shared_FORWARD >= 1
   return PMPI_File_iread_shared(mpi_fh, buf, count, datatype, request);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_iread_shared (MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Request *request=%p)", file_to_string(mpi_fh), buf, count, type_to_string(datatype), request);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_iread_shared_FORWARD
}
#endif // __IMPI_File_iread_shared


#ifndef __IMPI_File_iwrite_at
int IMPI_File_iwrite_at ( MPI_File mpi_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPIO_Request *request )
{
#if __IMPI_File_iwrite_at_FORWARD >= 1
   return PMPI_File_iwrite_at(mpi_fh, offset, buf, count, datatype, request);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_iwrite_at (MPI_File mpi_fh=%s, MPI_Offset offset=%p, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPIO_Request *request=%p)", file_to_string(mpi_fh), (void *) offset, buf, count, type_to_string(datatype), request);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_iwrite_at_FORWARD
}
#endif // __IMPI_File_iwrite_at


#ifndef __IMPI_File_iwrite
int IMPI_File_iwrite ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *request )
{
#if __IMPI_File_iwrite_FORWARD >= 1
   return PMPI_File_iwrite(mpi_fh, buf, count, datatype, request);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_iwrite (MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Request *request=%p)", file_to_string(mpi_fh), buf, count, type_to_string(datatype), request);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_iwrite_FORWARD
}
#endif // __IMPI_File_iwrite


#ifndef __IMPI_File_iwrite_shared
int IMPI_File_iwrite_shared ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPIO_Request *request )
{
#if __IMPI_File_iwrite_shared_FORWARD >= 1
   return PMPI_File_iwrite_shared(mpi_fh, buf, count, datatype, request);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_iwrite_shared (MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPIO_Request *request=%p)", file_to_string(mpi_fh), buf, count, type_to_string(datatype), request);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_iwrite_shared_FORWARD
}
#endif // __IMPI_File_iwrite_shared


#ifndef __IMPI_File_open
int IMPI_File_open ( MPI_Comm comm, char *filename, int amode, MPI_Info info, MPI_File *fh )
{
#if __IMPI_File_open_FORWARD >= 1
   return PMPI_File_open(comm, filename, amode, info, fh);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_open (MPI_Comm comm=%s, char *filename=%p, int amode=%d, MPI_Info info=%s, MPI_File *fh=%p)", comm_to_string(comm), filename, amode, info_to_string(info), fh);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_open_FORWARD
}
#endif // __IMPI_File_open


#ifndef __IMPI_File_preallocate
int IMPI_File_preallocate ( MPI_File mpi_fh, MPI_Offset size )
{
#if __IMPI_File_preallocate_FORWARD >= 1
   return PMPI_File_preallocate(mpi_fh, size);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_preallocate (MPI_File mpi_fh=%s, MPI_Offset size=%p)", file_to_string(mpi_fh), (void *) size);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_preallocate_FORWARD
}
#endif // __IMPI_File_preallocate


#ifndef __IMPI_File_read_all_begin
int IMPI_File_read_all_begin ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype )
{
#if __IMPI_File_read_all_begin_FORWARD >= 1
   return PMPI_File_read_all_begin(mpi_fh, buf, count, datatype);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_read_all_begin (MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s)", file_to_string(mpi_fh), buf, count, type_to_string(datatype));
   return MPI_ERR_INTERN;
#endif // __IMPI_File_read_all_begin_FORWARD
}
#endif // __IMPI_File_read_all_begin


#ifndef __IMPI_File_read_all_end
int IMPI_File_read_all_end ( MPI_File mpi_fh, void *buf, MPI_Status *status )
{
#if __IMPI_File_read_all_end_FORWARD >= 1
   return PMPI_File_read_all_end(mpi_fh, buf, status);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_read_all_end (MPI_File mpi_fh=%s, void *buf=%p, MPI_Status *status=%p)", file_to_string(mpi_fh), buf, status);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_read_all_end_FORWARD
}
#endif // __IMPI_File_read_all_end


#ifndef __IMPI_File_read_all
int IMPI_File_read_all ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status )
{
#if __IMPI_File_read_all_FORWARD >= 1
   return PMPI_File_read_all(mpi_fh, buf, count, datatype, status);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_read_all (MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Status *status=%p)", file_to_string(mpi_fh), buf, count, type_to_string(datatype), status);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_read_all_FORWARD
}
#endif // __IMPI_File_read_all


#ifndef __IMPI_File_read_at_all_begin
int IMPI_File_read_at_all_begin ( MPI_File mpi_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype )
{
#if __IMPI_File_read_at_all_begin_FORWARD >= 1
   return PMPI_File_read_at_all_begin(mpi_fh, offset, buf, count, datatype);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_read_at_all_begin (MPI_File mpi_fh=%s, MPI_Offset offset=%p, void *buf=%p, int count=%d, MPI_Datatype datatype=%s)", file_to_string(mpi_fh), (void *) offset, buf, count, type_to_string(datatype));
   return MPI_ERR_INTERN;
#endif // __IMPI_File_read_at_all_begin_FORWARD
}
#endif // __IMPI_File_read_at_all_begin


#ifndef __IMPI_File_read_at_all_end
int IMPI_File_read_at_all_end ( MPI_File mpi_fh, void *buf, MPI_Status *status )
{
#if __IMPI_File_read_at_all_end_FORWARD >= 1
   return PMPI_File_read_at_all_end(mpi_fh, buf, status);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_read_at_all_end (MPI_File mpi_fh=%s, void *buf=%p, MPI_Status *status=%p)", file_to_string(mpi_fh), buf, status);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_read_at_all_end_FORWARD
}
#endif // __IMPI_File_read_at_all_end


#ifndef __IMPI_File_read_at_all
int IMPI_File_read_at_all ( MPI_File mpi_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status )
{
#if __IMPI_File_read_at_all_FORWARD >= 1
   return PMPI_File_read_at_all(mpi_fh, offset, buf, count, datatype, status);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_read_at_all (MPI_File mpi_fh=%s, MPI_Offset offset=%p, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Status *status=%p)", file_to_string(mpi_fh), (void *) offset, buf, count, type_to_string(datatype), status);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_read_at_all_FORWARD
}
#endif // __IMPI_File_read_at_all


#ifndef __IMPI_File_read_at
int IMPI_File_read_at ( MPI_File mpi_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status )
{
#if __IMPI_File_read_at_FORWARD >= 1
   return PMPI_File_read_at(mpi_fh, offset, buf, count, datatype, status);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_read_at (MPI_File mpi_fh=%s, MPI_Offset offset=%p, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Status *status=%p)", file_to_string(mpi_fh), (void *) offset, buf, count, type_to_string(datatype), status);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_read_at_FORWARD
}
#endif // __IMPI_File_read_at


#ifndef __IMPI_File_read
int IMPI_File_read ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status )
{
#if __IMPI_File_read_FORWARD >= 1
   return PMPI_File_read(mpi_fh, buf, count, datatype, status);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_read (MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Status *status=%p)", file_to_string(mpi_fh), buf, count, type_to_string(datatype), status);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_read_FORWARD
}
#endif // __IMPI_File_read


#ifndef __IMPI_File_read_ordered_begin
int IMPI_File_read_ordered_begin ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype )
{
#if __IMPI_File_read_ordered_begin_FORWARD >= 1
   return PMPI_File_read_ordered_begin(mpi_fh, buf, count, datatype);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_read_ordered_begin (MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s)", file_to_string(mpi_fh), buf, count, type_to_string(datatype));
   return MPI_ERR_INTERN;
#endif // __IMPI_File_read_ordered_begin_FORWARD
}
#endif // __IMPI_File_read_ordered_begin


#ifndef __IMPI_File_read_ordered_end
int IMPI_File_read_ordered_end ( MPI_File mpi_fh, void *buf, MPI_Status *status )
{
#if __IMPI_File_read_ordered_end_FORWARD >= 1
   return PMPI_File_read_ordered_end(mpi_fh, buf, status);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_read_ordered_end (MPI_File mpi_fh=%s, void *buf=%p, MPI_Status *status=%p)", file_to_string(mpi_fh), buf, status);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_read_ordered_end_FORWARD
}
#endif // __IMPI_File_read_ordered_end


#ifndef __IMPI_File_read_ordered
int IMPI_File_read_ordered ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status )
{
#if __IMPI_File_read_ordered_FORWARD >= 1
   return PMPI_File_read_ordered(mpi_fh, buf, count, datatype, status);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_read_ordered (MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Status *status=%p)", file_to_string(mpi_fh), buf, count, type_to_string(datatype), status);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_read_ordered_FORWARD
}
#endif // __IMPI_File_read_ordered


#ifndef __IMPI_File_read_shared
int IMPI_File_read_shared ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status )
{
#if __IMPI_File_read_shared_FORWARD >= 1
   return PMPI_File_read_shared(mpi_fh, buf, count, datatype, status);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_read_shared (MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Status *status=%p)", file_to_string(mpi_fh), buf, count, type_to_string(datatype), status);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_read_shared_FORWARD
}
#endif // __IMPI_File_read_shared


#ifndef __IMPI_File_seek
int IMPI_File_seek ( MPI_File mpi_fh, MPI_Offset offset, int whence )
{
#if __IMPI_File_seek_FORWARD >= 1
   return PMPI_File_seek(mpi_fh, offset, whence);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_seek (MPI_File mpi_fh=%s, MPI_Offset offset=%p, int whence=%d)", file_to_string(mpi_fh), (void *) offset, whence);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_seek_FORWARD
}
#endif // __IMPI_File_seek


#ifndef __IMPI_File_seek_shared
int IMPI_File_seek_shared ( MPI_File mpi_fh, MPI_Offset offset, int whence )
{
#if __IMPI_File_seek_shared_FORWARD >= 1
   return PMPI_File_seek_shared(mpi_fh, offset, whence);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_seek_shared (MPI_File mpi_fh=%s, MPI_Offset offset=%p, int whence=%d)", file_to_string(mpi_fh), (void *) offset, whence);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_seek_shared_FORWARD
}
#endif // __IMPI_File_seek_shared


#ifndef __IMPI_File_set_atomicity
int IMPI_File_set_atomicity ( MPI_File mpi_fh, int flag )
{
#if __IMPI_File_set_atomicity_FORWARD >= 1
   return PMPI_File_set_atomicity(mpi_fh, flag);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_set_atomicity (MPI_File mpi_fh=%s, int flag=%d)", file_to_string(mpi_fh), flag);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_set_atomicity_FORWARD
}
#endif // __IMPI_File_set_atomicity


#ifndef __IMPI_File_set_errhandler
int IMPI_File_set_errhandler ( MPI_File file, MPI_Errhandler errhandler )
{
#if __IMPI_File_set_errhandler_FORWARD >= 1
   return PMPI_File_set_errhandler(file, errhandler);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_set_errhandler (MPI_File file=%s, MPI_Errhandler errhandler=%p)", file_to_string(file), (void *) errhandler);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_set_errhandler_FORWARD
}
#endif // __IMPI_File_set_errhandler


#ifndef __IMPI_File_set_info
int IMPI_File_set_info ( MPI_File mpi_fh, MPI_Info info )
{
#if __IMPI_File_set_info_FORWARD >= 1
   return PMPI_File_set_info(mpi_fh, info);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_set_info (MPI_File mpi_fh=%s, MPI_Info info=%s)", file_to_string(mpi_fh), info_to_string(info));
   return MPI_ERR_INTERN;
#endif // __IMPI_File_set_info_FORWARD
}
#endif // __IMPI_File_set_info


#ifndef __IMPI_File_set_size
int IMPI_File_set_size ( MPI_File mpi_fh, MPI_Offset size )
{
#if __IMPI_File_set_size_FORWARD >= 1
   return PMPI_File_set_size(mpi_fh, size);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_set_size (MPI_File mpi_fh=%s, MPI_Offset size=%p)", file_to_string(mpi_fh), (void *) size);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_set_size_FORWARD
}
#endif // __IMPI_File_set_size


#ifndef __IMPI_File_set_view
int IMPI_File_set_view ( MPI_File mpi_fh, MPI_Offset disp, MPI_Datatype etype, MPI_Datatype filetype, char *datarep, MPI_Info info )
{
#if __IMPI_File_set_view_FORWARD >= 1
   return PMPI_File_set_view(mpi_fh, disp, etype, filetype, datarep, info);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_set_view (MPI_File mpi_fh=%s, MPI_Offset disp=%p, MPI_Datatype etype=%s, MPI_Datatype filetype=%s, char *datarep=%p, MPI_Info info=%s)", file_to_string(mpi_fh), (void *) disp, type_to_string(etype), type_to_string(filetype), datarep, info_to_string(info));
   return MPI_ERR_INTERN;
#endif // __IMPI_File_set_view_FORWARD
}
#endif // __IMPI_File_set_view


#ifndef __IMPI_File_sync
int IMPI_File_sync ( MPI_File mpi_fh )
{
#if __IMPI_File_sync_FORWARD >= 1
   return PMPI_File_sync(mpi_fh);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_sync (MPI_File mpi_fh=%s)", file_to_string(mpi_fh));
   return MPI_ERR_INTERN;
#endif // __IMPI_File_sync_FORWARD
}
#endif // __IMPI_File_sync


#ifndef __IMPI_File_write_all_begin
int IMPI_File_write_all_begin ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype )
{
#if __IMPI_File_write_all_begin_FORWARD >= 1
   return PMPI_File_write_all_begin(mpi_fh, buf, count, datatype);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_write_all_begin (MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s)", file_to_string(mpi_fh), buf, count, type_to_string(datatype));
   return MPI_ERR_INTERN;
#endif // __IMPI_File_write_all_begin_FORWARD
}
#endif // __IMPI_File_write_all_begin


#ifndef __IMPI_File_write_all_end
int IMPI_File_write_all_end ( MPI_File mpi_fh, void *buf, MPI_Status *status )
{
#if __IMPI_File_write_all_end_FORWARD >= 1
   return PMPI_File_write_all_end(mpi_fh, buf, status);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_write_all_end (MPI_File mpi_fh=%s, void *buf=%p, MPI_Status *status=%p)", file_to_string(mpi_fh), buf, status);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_write_all_end_FORWARD
}
#endif // __IMPI_File_write_all_end


#ifndef __IMPI_File_write_all
int IMPI_File_write_all ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status )
{
#if __IMPI_File_write_all_FORWARD >= 1
   return PMPI_File_write_all(mpi_fh, buf, count, datatype, status);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_write_all (MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Status *status=%p)", file_to_string(mpi_fh), buf, count, type_to_string(datatype), status);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_write_all_FORWARD
}
#endif // __IMPI_File_write_all


#ifndef __IMPI_File_write_at_all_begin
int IMPI_File_write_at_all_begin ( MPI_File mpi_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype )
{
#if __IMPI_File_write_at_all_begin_FORWARD >= 1
   return PMPI_File_write_at_all_begin(mpi_fh, offset, buf, count, datatype);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_write_at_all_begin (MPI_File mpi_fh=%s, MPI_Offset offset=%p, void *buf=%p, int count=%d, MPI_Datatype datatype=%s)", file_to_string(mpi_fh), (void *) offset, buf, count, type_to_string(datatype));
   return MPI_ERR_INTERN;
#endif // __IMPI_File_write_at_all_begin_FORWARD
}
#endif // __IMPI_File_write_at_all_begin


#ifndef __IMPI_File_write_at_all_end
int IMPI_File_write_at_all_end ( MPI_File mpi_fh, void *buf, MPI_Status *status )
{
#if __IMPI_File_write_at_all_end_FORWARD >= 1
   return PMPI_File_write_at_all_end(mpi_fh, buf, status);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_write_at_all_end (MPI_File mpi_fh=%s, void *buf=%p, MPI_Status *status=%p)", file_to_string(mpi_fh), buf, status);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_write_at_all_end_FORWARD
}
#endif // __IMPI_File_write_at_all_end


#ifndef __IMPI_File_write_at_all
int IMPI_File_write_at_all ( MPI_File mpi_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status )
{
#if __IMPI_File_write_at_all_FORWARD >= 1
   return PMPI_File_write_at_all(mpi_fh, offset, buf, count, datatype, status);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_write_at_all (MPI_File mpi_fh=%s, MPI_Offset offset=%p, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Status *status=%p)", file_to_string(mpi_fh), (void *) offset, buf, count, type_to_string(datatype), status);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_write_at_all_FORWARD
}
#endif // __IMPI_File_write_at_all


#ifndef __IMPI_File_write_at
int IMPI_File_write_at ( MPI_File mpi_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status )
{
#if __IMPI_File_write_at_FORWARD >= 1
   return PMPI_File_write_at(mpi_fh, offset, buf, count, datatype, status);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_write_at (MPI_File mpi_fh=%s, MPI_Offset offset=%p, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Status *status=%p)", file_to_string(mpi_fh), (void *) offset, buf, count, type_to_string(datatype), status);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_write_at_FORWARD
}
#endif // __IMPI_File_write_at


#ifndef __IMPI_File_write
int IMPI_File_write ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status )
{
#if __IMPI_File_write_FORWARD >= 1
   return PMPI_File_write(mpi_fh, buf, count, datatype, status);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_write (MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Status *status=%p)", file_to_string(mpi_fh), buf, count, type_to_string(datatype), status);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_write_FORWARD
}
#endif // __IMPI_File_write


#ifndef __IMPI_File_write_ordered_begin
int IMPI_File_write_ordered_begin ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype )
{
#if __IMPI_File_write_ordered_begin_FORWARD >= 1
   return PMPI_File_write_ordered_begin(mpi_fh, buf, count, datatype);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_write_ordered_begin (MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s)", file_to_string(mpi_fh), buf, count, type_to_string(datatype));
   return MPI_ERR_INTERN;
#endif // __IMPI_File_write_ordered_begin_FORWARD
}
#endif // __IMPI_File_write_ordered_begin


#ifndef __IMPI_File_write_ordered_end
int IMPI_File_write_ordered_end ( MPI_File mpi_fh, void *buf, MPI_Status *status )
{
#if __IMPI_File_write_ordered_end_FORWARD >= 1
   return PMPI_File_write_ordered_end(mpi_fh, buf, status);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_write_ordered_end (MPI_File mpi_fh=%s, void *buf=%p, MPI_Status *status=%p)", file_to_string(mpi_fh), buf, status);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_write_ordered_end_FORWARD
}
#endif // __IMPI_File_write_ordered_end


#ifndef __IMPI_File_write_ordered
int IMPI_File_write_ordered ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status )
{
#if __IMPI_File_write_ordered_FORWARD >= 1
   return PMPI_File_write_ordered(mpi_fh, buf, count, datatype, status);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_write_ordered (MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Status *status=%p)", file_to_string(mpi_fh), buf, count, type_to_string(datatype), status);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_write_ordered_FORWARD
}
#endif // __IMPI_File_write_ordered


#ifndef __IMPI_File_write_shared
int IMPI_File_write_shared ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status )
{
#if __IMPI_File_write_shared_FORWARD >= 1
   return PMPI_File_write_shared(mpi_fh, buf, count, datatype, status);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_write_shared (MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Status *status=%p)", file_to_string(mpi_fh), buf, count, type_to_string(datatype), status);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_write_shared_FORWARD
}
#endif // __IMPI_File_write_shared


#ifndef __IMPI_Finalized
int IMPI_Finalized ( int *flag )
{
#if __IMPI_Finalized_FORWARD >= 1
   return PMPI_Finalized(flag);
#else
   FATAL("NOT IMPLEMENTED: MPI_Finalized (int *flag=%p)", flag);
   return MPI_ERR_INTERN;
#endif // __IMPI_Finalized_FORWARD
}
#endif // __IMPI_Finalized


#ifndef __IMPI_Finalize
int IMPI_Finalize (  )
{
#if __IMPI_Finalize_FORWARD >= 1
   return PMPI_Finalize();
#else
   FATAL("NOT IMPLEMENTED: MPI_Finalize ()");
   return MPI_ERR_INTERN;
#endif // __IMPI_Finalize_FORWARD
}
#endif // __IMPI_Finalize


#ifndef __IMPI_Free_mem
int IMPI_Free_mem ( void *base )
{
#if __IMPI_Free_mem_FORWARD >= 1
   return PMPI_Free_mem(base);
#else
   FATAL("NOT IMPLEMENTED: MPI_Free_mem (void *base=%p)", base);
   return MPI_ERR_INTERN;
#endif // __IMPI_Free_mem_FORWARD
}
#endif // __IMPI_Free_mem


#ifndef __IMPI_Gather
int IMPI_Gather ( void *sendbuf, int sendcnt, MPI_Datatype sendtype, void *recvbuf, int recvcnt, MPI_Datatype recvtype, int root, MPI_Comm comm )
{
#if __IMPI_Gather_FORWARD >= 1
   return PMPI_Gather(sendbuf, sendcnt, sendtype, recvbuf, recvcnt, recvtype, root, comm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Gather (void *sendbuf=%p, int sendcnt=%d, MPI_Datatype sendtype=%s, void *recvbuf=%p, int recvcnt=%d, MPI_Datatype recvtype=%s, int root=%d, MPI_Comm comm=%s)", sendbuf, sendcnt, type_to_string(sendtype), recvbuf, recvcnt, type_to_string(recvtype), root, comm_to_string(comm));
   return MPI_ERR_INTERN;
#endif // __IMPI_Gather_FORWARD
}
#endif // __IMPI_Gather


#ifndef __IMPI_Gatherv
int IMPI_Gatherv ( void *sendbuf, int sendcnt, MPI_Datatype sendtype, void *recvbuf, int *recvcnts, int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm )
{
#if __IMPI_Gatherv_FORWARD >= 1
   return PMPI_Gatherv(sendbuf, sendcnt, sendtype, recvbuf, recvcnts, displs, recvtype, root, comm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Gatherv (void *sendbuf=%p, int sendcnt=%d, MPI_Datatype sendtype=%s, void *recvbuf=%p, int *recvcnts=%p, int *displs=%p, MPI_Datatype recvtype=%s, int root=%d, MPI_Comm comm=%s)", sendbuf, sendcnt, type_to_string(sendtype), recvbuf, recvcnts, displs, type_to_string(recvtype), root, comm_to_string(comm));
   return MPI_ERR_INTERN;
#endif // __IMPI_Gatherv_FORWARD
}
#endif // __IMPI_Gatherv


#ifndef __IMPI_Get_address
int IMPI_Get_address ( void *location, MPI_Aint *address )
{
#if __IMPI_Get_address_FORWARD >= 1
   return PMPI_Get_address(location, address);
#else
   FATAL("NOT IMPLEMENTED: MPI_Get_address (void *location=%p, MPI_Aint *address=%p)", location, address);
   return MPI_ERR_INTERN;
#endif // __IMPI_Get_address_FORWARD
}
#endif // __IMPI_Get_address


#ifndef __IMPI_Get_count
int IMPI_Get_count ( MPI_Status *status, MPI_Datatype datatype, int *count )
{
#if __IMPI_Get_count_FORWARD >= 1
   return PMPI_Get_count(status, datatype, count);
#else
   FATAL("NOT IMPLEMENTED: MPI_Get_count (MPI_Status *status=%p, MPI_Datatype datatype=%s, int *count=%p)", status, type_to_string(datatype), count);
   return MPI_ERR_INTERN;
#endif // __IMPI_Get_count_FORWARD
}
#endif // __IMPI_Get_count


#ifndef __IMPI_Get_elements
int IMPI_Get_elements ( MPI_Status *status, MPI_Datatype datatype, int *elements )
{
#if __IMPI_Get_elements_FORWARD >= 1
   return PMPI_Get_elements(status, datatype, elements);
#else
   FATAL("NOT IMPLEMENTED: MPI_Get_elements (MPI_Status *status=%p, MPI_Datatype datatype=%s, int *elements=%p)", status, type_to_string(datatype), elements);
   return MPI_ERR_INTERN;
#endif // __IMPI_Get_elements_FORWARD
}
#endif // __IMPI_Get_elements


#ifndef __IMPI_Get
int IMPI_Get ( void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win )
{
#if __IMPI_Get_FORWARD >= 1
   return PMPI_Get(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
#else
   FATAL("NOT IMPLEMENTED: MPI_Get (void *origin_addr=%p, int origin_count=%d, MPI_Datatype origin_datatype=%s, int target_rank=%d, MPI_Aint target_disp=%p, int target_count=%d, MPI_Datatype target_datatype=%s, MPI_Win win=%s)", origin_addr, origin_count, type_to_string(origin_datatype), target_rank, (void *) target_disp, target_count, type_to_string(target_datatype), win_to_string(win));
   return MPI_ERR_INTERN;
#endif // __IMPI_Get_FORWARD
}
#endif // __IMPI_Get


#ifndef __IMPI_Get_processor_name
int IMPI_Get_processor_name ( char *name, int *resultlen )
{
#if __IMPI_Get_processor_name_FORWARD >= 1
   return PMPI_Get_processor_name(name, resultlen);
#else
   FATAL("NOT IMPLEMENTED: MPI_Get_processor_name (char *name=%p, int *resultlen=%p)", name, resultlen);
   return MPI_ERR_INTERN;
#endif // __IMPI_Get_processor_name_FORWARD
}
#endif // __IMPI_Get_processor_name


#ifndef __IMPI_Get_version
int IMPI_Get_version ( int *version, int *subversion )
{
#if __IMPI_Get_version_FORWARD >= 1
   return PMPI_Get_version(version, subversion);
#else
   FATAL("NOT IMPLEMENTED: MPI_Get_version (int *version=%p, int *subversion=%p)", version, subversion);
   return MPI_ERR_INTERN;
#endif // __IMPI_Get_version_FORWARD
}
#endif // __IMPI_Get_version


#ifndef __IMPI_Graph_create
int IMPI_Graph_create ( MPI_Comm comm_old, int nnodes, int *indx, int *edges, int reorder, MPI_Comm *comm_graph )
{
#if __IMPI_Graph_create_FORWARD >= 1
   return PMPI_Graph_create(comm_old, nnodes, indx, edges, reorder, comm_graph);
#else
   FATAL("NOT IMPLEMENTED: MPI_Graph_create (MPI_Comm comm_old=%s, int nnodes=%d, int *indx=%p, int *edges=%p, int reorder=%d, MPI_Comm *comm_graph=%p)", comm_to_string(comm_old), nnodes, indx, edges, reorder, comm_graph);
   return MPI_ERR_INTERN;
#endif // __IMPI_Graph_create_FORWARD
}
#endif // __IMPI_Graph_create


#ifndef __IMPI_Graphdims_get
int IMPI_Graphdims_get ( MPI_Comm comm, int *nnodes, int *nedges )
{
#if __IMPI_Graphdims_get_FORWARD >= 1
   return PMPI_Graphdims_get(comm, nnodes, nedges);
#else
   FATAL("NOT IMPLEMENTED: MPI_Graphdims_get (MPI_Comm comm=%s, int *nnodes=%p, int *nedges=%p)", comm_to_string(comm), nnodes, nedges);
   return MPI_ERR_INTERN;
#endif // __IMPI_Graphdims_get_FORWARD
}
#endif // __IMPI_Graphdims_get


#ifndef __IMPI_Graph_get
int IMPI_Graph_get ( MPI_Comm comm, int maxindex, int maxedges, int *indx, int *edges )
{
#if __IMPI_Graph_get_FORWARD >= 1
   return PMPI_Graph_get(comm, maxindex, maxedges, indx, edges);
#else
   FATAL("NOT IMPLEMENTED: MPI_Graph_get (MPI_Comm comm=%s, int maxindex=%d, int maxedges=%d, int *indx=%p, int *edges=%p)", comm_to_string(comm), maxindex, maxedges, indx, edges);
   return MPI_ERR_INTERN;
#endif // __IMPI_Graph_get_FORWARD
}
#endif // __IMPI_Graph_get


#ifndef __IMPI_Graph_map
int IMPI_Graph_map ( MPI_Comm comm_old, int nnodes, int *indx, int *edges, int *newrank )
{
#if __IMPI_Graph_map_FORWARD >= 1
   return PMPI_Graph_map(comm_old, nnodes, indx, edges, newrank);
#else
   FATAL("NOT IMPLEMENTED: MPI_Graph_map (MPI_Comm comm_old=%s, int nnodes=%d, int *indx=%p, int *edges=%p, int *newrank=%p)", comm_to_string(comm_old), nnodes, indx, edges, newrank);
   return MPI_ERR_INTERN;
#endif // __IMPI_Graph_map_FORWARD
}
#endif // __IMPI_Graph_map


#ifndef __IMPI_Graph_neighbors_count
int IMPI_Graph_neighbors_count ( MPI_Comm comm, int rank, int *nneighbors )
{
#if __IMPI_Graph_neighbors_count_FORWARD >= 1
   return PMPI_Graph_neighbors_count(comm, rank, nneighbors);
#else
   FATAL("NOT IMPLEMENTED: MPI_Graph_neighbors_count (MPI_Comm comm=%s, int rank=%d, int *nneighbors=%p)", comm_to_string(comm), rank, nneighbors);
   return MPI_ERR_INTERN;
#endif // __IMPI_Graph_neighbors_count_FORWARD
}
#endif // __IMPI_Graph_neighbors_count


#ifndef __IMPI_Graph_neighbors
int IMPI_Graph_neighbors ( MPI_Comm comm, int rank, int maxneighbors, int *neighbors )
{
#if __IMPI_Graph_neighbors_FORWARD >= 1
   return PMPI_Graph_neighbors(comm, rank, maxneighbors, neighbors);
#else
   FATAL("NOT IMPLEMENTED: MPI_Graph_neighbors (MPI_Comm comm=%s, int rank=%d, int maxneighbors=%d, int *neighbors=%p)", comm_to_string(comm), rank, maxneighbors, neighbors);
   return MPI_ERR_INTERN;
#endif // __IMPI_Graph_neighbors_FORWARD
}
#endif // __IMPI_Graph_neighbors


#ifndef __IMPI_Grequest_complete
int IMPI_Grequest_complete ( MPI_Request request )
{
#if __IMPI_Grequest_complete_FORWARD >= 1
   return PMPI_Grequest_complete(request);
#else
   FATAL("NOT IMPLEMENTED: MPI_Grequest_complete (MPI_Request request=%s)", request_to_string(request));
   return MPI_ERR_INTERN;
#endif // __IMPI_Grequest_complete_FORWARD
}
#endif // __IMPI_Grequest_complete


#ifndef __IMPI_Grequest_start
int IMPI_Grequest_start ( MPI_Grequest_query_function *query_fn, MPI_Grequest_free_function *free_fn, MPI_Grequest_cancel_function *cancel_fn, void *extra_state, MPI_Request *request )
{
#if __IMPI_Grequest_start_FORWARD >= 1
   return PMPI_Grequest_start(query_fn, free_fn, cancel_fn, extra_state, request);
#else
   FATAL("NOT IMPLEMENTED: MPI_Grequest_start (MPI_Grequest_query_function *query_fn=%p, MPI_Grequest_free_function *free_fn=%p, MPI_Grequest_cancel_function *cancel_fn=%p, void *extra_state=%p, MPI_Request *request=%p)", query_fn, free_fn, cancel_fn, extra_state, request);
   return MPI_ERR_INTERN;
#endif // __IMPI_Grequest_start_FORWARD
}
#endif // __IMPI_Grequest_start


#ifndef __IMPI_Group_compare
int IMPI_Group_compare ( MPI_Group group1, MPI_Group group2, int *result )
{
#if __IMPI_Group_compare_FORWARD >= 1
   return PMPI_Group_compare(group1, group2, result);
#else
   FATAL("NOT IMPLEMENTED: MPI_Group_compare (MPI_Group group1=%s, MPI_Group group2=%s, int *result=%p)", group_to_string(group1), group_to_string(group2), result);
   return MPI_ERR_INTERN;
#endif // __IMPI_Group_compare_FORWARD
}
#endif // __IMPI_Group_compare


#ifndef __IMPI_Group_difference
int IMPI_Group_difference ( MPI_Group group1, MPI_Group group2, MPI_Group *newgroup )
{
#if __IMPI_Group_difference_FORWARD >= 1
   return PMPI_Group_difference(group1, group2, newgroup);
#else
   FATAL("NOT IMPLEMENTED: MPI_Group_difference (MPI_Group group1=%s, MPI_Group group2=%s, MPI_Group *newgroup=%p)", group_to_string(group1), group_to_string(group2), newgroup);
   return MPI_ERR_INTERN;
#endif // __IMPI_Group_difference_FORWARD
}
#endif // __IMPI_Group_difference


#ifndef __IMPI_Group_excl
int IMPI_Group_excl ( MPI_Group group, int n, int *ranks, MPI_Group *newgroup )
{
#if __IMPI_Group_excl_FORWARD >= 1
   return PMPI_Group_excl(group, n, ranks, newgroup);
#else
   FATAL("NOT IMPLEMENTED: MPI_Group_excl (MPI_Group group=%s, int n=%d, int *ranks=%p, MPI_Group *newgroup=%p)", group_to_string(group), n, ranks, newgroup);
   return MPI_ERR_INTERN;
#endif // __IMPI_Group_excl_FORWARD
}
#endif // __IMPI_Group_excl


#ifndef __IMPI_Group_free
int IMPI_Group_free ( MPI_Group *group )
{
#if __IMPI_Group_free_FORWARD >= 1
   return PMPI_Group_free(group);
#else
   FATAL("NOT IMPLEMENTED: MPI_Group_free (MPI_Group *group=%p)", group);
   return MPI_ERR_INTERN;
#endif // __IMPI_Group_free_FORWARD
}
#endif // __IMPI_Group_free


#ifndef __IMPI_Group_incl
int IMPI_Group_incl ( MPI_Group group, int n, int *ranks, MPI_Group *newgroup )
{
#if __IMPI_Group_incl_FORWARD >= 1
   return PMPI_Group_incl(group, n, ranks, newgroup);
#else
   FATAL("NOT IMPLEMENTED: MPI_Group_incl (MPI_Group group=%s, int n=%d, int *ranks=%p, MPI_Group *newgroup=%p)", group_to_string(group), n, ranks, newgroup);
   return MPI_ERR_INTERN;
#endif // __IMPI_Group_incl_FORWARD
}
#endif // __IMPI_Group_incl


#ifndef __IMPI_Group_intersection
int IMPI_Group_intersection ( MPI_Group group1, MPI_Group group2, MPI_Group *newgroup )
{
#if __IMPI_Group_intersection_FORWARD >= 1
   return PMPI_Group_intersection(group1, group2, newgroup);
#else
   FATAL("NOT IMPLEMENTED: MPI_Group_intersection (MPI_Group group1=%s, MPI_Group group2=%s, MPI_Group *newgroup=%p)", group_to_string(group1), group_to_string(group2), newgroup);
   return MPI_ERR_INTERN;
#endif // __IMPI_Group_intersection_FORWARD
}
#endif // __IMPI_Group_intersection


#ifndef __IMPI_Group_range_excl
int IMPI_Group_range_excl ( MPI_Group group, int n, int ranges[][3], MPI_Group *newgroup )
{
#if __IMPI_Group_range_excl_FORWARD >= 1
   return PMPI_Group_range_excl(group, n, ranges, newgroup);
#else
   FATAL("NOT IMPLEMENTED: MPI_Group_range_excl (MPI_Group group=%s, int n=%d, int ranges[][3]=%p, MPI_Group *newgroup=%p)", group_to_string(group), n, ranges, newgroup);
   return MPI_ERR_INTERN;
#endif // __IMPI_Group_range_excl_FORWARD
}
#endif // __IMPI_Group_range_excl


#ifndef __IMPI_Group_range_incl
int IMPI_Group_range_incl ( MPI_Group group, int n, int ranges[][3], MPI_Group *newgroup )
{
#if __IMPI_Group_range_incl_FORWARD >= 1
   return PMPI_Group_range_incl(group, n, ranges, newgroup);
#else
   FATAL("NOT IMPLEMENTED: MPI_Group_range_incl (MPI_Group group=%s, int n=%d, int ranges[][3]=%p, MPI_Group *newgroup=%p)", group_to_string(group), n, ranges, newgroup);
   return MPI_ERR_INTERN;
#endif // __IMPI_Group_range_incl_FORWARD
}
#endif // __IMPI_Group_range_incl


#ifndef __IMPI_Group_rank
int IMPI_Group_rank ( MPI_Group group, int *rank )
{
#if __IMPI_Group_rank_FORWARD >= 1
   return PMPI_Group_rank(group, rank);
#else
   FATAL("NOT IMPLEMENTED: MPI_Group_rank (MPI_Group group=%s, int *rank=%p)", group_to_string(group), rank);
   return MPI_ERR_INTERN;
#endif // __IMPI_Group_rank_FORWARD
}
#endif // __IMPI_Group_rank


#ifndef __IMPI_Group_size
int IMPI_Group_size ( MPI_Group group, int *size )
{
#if __IMPI_Group_size_FORWARD >= 1
   return PMPI_Group_size(group, size);
#else
   FATAL("NOT IMPLEMENTED: MPI_Group_size (MPI_Group group=%s, int *size=%p)", group_to_string(group), size);
   return MPI_ERR_INTERN;
#endif // __IMPI_Group_size_FORWARD
}
#endif // __IMPI_Group_size


#ifndef __IMPI_Group_translate_ranks
int IMPI_Group_translate_ranks ( MPI_Group group1, int n, int *ranks1, MPI_Group group2, int *ranks2 )
{
#if __IMPI_Group_translate_ranks_FORWARD >= 1
   return PMPI_Group_translate_ranks(group1, n, ranks1, group2, ranks2);
#else
   FATAL("NOT IMPLEMENTED: MPI_Group_translate_ranks (MPI_Group group1=%s, int n=%d, int *ranks1=%p, MPI_Group group2=%s, int *ranks2=%p)", group_to_string(group1), n, ranks1, group_to_string(group2), ranks2);
   return MPI_ERR_INTERN;
#endif // __IMPI_Group_translate_ranks_FORWARD
}
#endif // __IMPI_Group_translate_ranks


#ifndef __IMPI_Group_union
int IMPI_Group_union ( MPI_Group group1, MPI_Group group2, MPI_Group *newgroup )
{
#if __IMPI_Group_union_FORWARD >= 1
   return PMPI_Group_union(group1, group2, newgroup);
#else
   FATAL("NOT IMPLEMENTED: MPI_Group_union (MPI_Group group1=%s, MPI_Group group2=%s, MPI_Group *newgroup=%p)", group_to_string(group1), group_to_string(group2), newgroup);
   return MPI_ERR_INTERN;
#endif // __IMPI_Group_union_FORWARD
}
#endif // __IMPI_Group_union


#ifndef __IMPI_Ibsend
int IMPI_Ibsend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request )
{
#if __IMPI_Ibsend_FORWARD >= 1
   return PMPI_Ibsend(buf, count, datatype, dest, tag, comm, request);
#else
   FATAL("NOT IMPLEMENTED: MPI_Ibsend (void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s, MPI_Request *request=%p)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm), request);
   return MPI_ERR_INTERN;
#endif // __IMPI_Ibsend_FORWARD
}
#endif // __IMPI_Ibsend


#ifndef __IMPI_Info_create
int IMPI_Info_create ( MPI_Info *info )
{
#if __IMPI_Info_create_FORWARD >= 1
   return PMPI_Info_create(info);
#else
   FATAL("NOT IMPLEMENTED: MPI_Info_create (MPI_Info *info=%p)", info);
   return MPI_ERR_INTERN;
#endif // __IMPI_Info_create_FORWARD
}
#endif // __IMPI_Info_create


#ifndef __IMPI_Info_delete
int IMPI_Info_delete ( MPI_Info info, char *key )
{
#if __IMPI_Info_delete_FORWARD >= 1
   return PMPI_Info_delete(info, key);
#else
   FATAL("NOT IMPLEMENTED: MPI_Info_delete (MPI_Info info=%s, char *key=%p)", info_to_string(info), key);
   return MPI_ERR_INTERN;
#endif // __IMPI_Info_delete_FORWARD
}
#endif // __IMPI_Info_delete


#ifndef __IMPI_Info_dup
int IMPI_Info_dup ( MPI_Info info, MPI_Info *newinfo )
{
#if __IMPI_Info_dup_FORWARD >= 1
   return PMPI_Info_dup(info, newinfo);
#else
   FATAL("NOT IMPLEMENTED: MPI_Info_dup (MPI_Info info=%s, MPI_Info *newinfo=%p)", info_to_string(info), newinfo);
   return MPI_ERR_INTERN;
#endif // __IMPI_Info_dup_FORWARD
}
#endif // __IMPI_Info_dup


#ifndef __IMPI_Info_free
int IMPI_Info_free ( MPI_Info *info )
{
#if __IMPI_Info_free_FORWARD >= 1
   return PMPI_Info_free(info);
#else
   FATAL("NOT IMPLEMENTED: MPI_Info_free (MPI_Info *info=%p)", info);
   return MPI_ERR_INTERN;
#endif // __IMPI_Info_free_FORWARD
}
#endif // __IMPI_Info_free


#ifndef __IMPI_Info_get
int IMPI_Info_get ( MPI_Info info, char *key, int valuelen, char *value, int *flag )
{
#if __IMPI_Info_get_FORWARD >= 1
   return PMPI_Info_get(info, key, valuelen, value, flag);
#else
   FATAL("NOT IMPLEMENTED: MPI_Info_get (MPI_Info info=%s, char *key=%p, int valuelen=%d, char *value=%p, int *flag=%p)", info_to_string(info), key, valuelen, value, flag);
   return MPI_ERR_INTERN;
#endif // __IMPI_Info_get_FORWARD
}
#endif // __IMPI_Info_get


#ifndef __IMPI_Info_get_nkeys
int IMPI_Info_get_nkeys ( MPI_Info info, int *nkeys )
{
#if __IMPI_Info_get_nkeys_FORWARD >= 1
   return PMPI_Info_get_nkeys(info, nkeys);
#else
   FATAL("NOT IMPLEMENTED: MPI_Info_get_nkeys (MPI_Info info=%s, int *nkeys=%p)", info_to_string(info), nkeys);
   return MPI_ERR_INTERN;
#endif // __IMPI_Info_get_nkeys_FORWARD
}
#endif // __IMPI_Info_get_nkeys


#ifndef __IMPI_Info_get_nthkey
int IMPI_Info_get_nthkey ( MPI_Info info, int n, char *key )
{
#if __IMPI_Info_get_nthkey_FORWARD >= 1
   return PMPI_Info_get_nthkey(info, n, key);
#else
   FATAL("NOT IMPLEMENTED: MPI_Info_get_nthkey (MPI_Info info=%s, int n=%d, char *key=%p)", info_to_string(info), n, key);
   return MPI_ERR_INTERN;
#endif // __IMPI_Info_get_nthkey_FORWARD
}
#endif // __IMPI_Info_get_nthkey


#ifndef __IMPI_Info_get_valuelen
int IMPI_Info_get_valuelen ( MPI_Info info, char *key, int *valuelen, int *flag )
{
#if __IMPI_Info_get_valuelen_FORWARD >= 1
   return PMPI_Info_get_valuelen(info, key, valuelen, flag);
#else
   FATAL("NOT IMPLEMENTED: MPI_Info_get_valuelen (MPI_Info info=%s, char *key=%p, int *valuelen=%p, int *flag=%p)", info_to_string(info), key, valuelen, flag);
   return MPI_ERR_INTERN;
#endif // __IMPI_Info_get_valuelen_FORWARD
}
#endif // __IMPI_Info_get_valuelen


#ifndef __IMPI_Info_set
int IMPI_Info_set ( MPI_Info info, char *key, char *value )
{
#if __IMPI_Info_set_FORWARD >= 1
   return PMPI_Info_set(info, key, value);
#else
   FATAL("NOT IMPLEMENTED: MPI_Info_set (MPI_Info info=%s, char *key=%p, char *value=%p)", info_to_string(info), key, value);
   return MPI_ERR_INTERN;
#endif // __IMPI_Info_set_FORWARD
}
#endif // __IMPI_Info_set


#ifndef __IMPI_Initialized
int IMPI_Initialized ( int *flag )
{
#if __IMPI_Initialized_FORWARD >= 1
   return PMPI_Initialized(flag);
#else
   FATAL("NOT IMPLEMENTED: MPI_Initialized (int *flag=%p)", flag);
   return MPI_ERR_INTERN;
#endif // __IMPI_Initialized_FORWARD
}
#endif // __IMPI_Initialized


#ifndef __IMPI_Init_thread
int IMPI_Init_thread ( int *argc, char ***argv, int required, int *provided )
{
#if __IMPI_Init_thread_FORWARD >= 1
   return PMPI_Init_thread(argc, argv, required, provided);
#else
   FATAL("NOT IMPLEMENTED: MPI_Init_thread (int *argc=%p, char ***argv=%p, int required=%d, int *provided=%p)", argc, argv, required, provided);
   return MPI_ERR_INTERN;
#endif // __IMPI_Init_thread_FORWARD
}
#endif // __IMPI_Init_thread


#ifndef __IMPI_Intercomm_create
int IMPI_Intercomm_create ( MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm )
{
#if __IMPI_Intercomm_create_FORWARD >= 1
   return PMPI_Intercomm_create(local_comm, local_leader, peer_comm, remote_leader, tag, newintercomm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Intercomm_create (MPI_Comm local_comm=%s, int local_leader=%d, MPI_Comm peer_comm=%s, int remote_leader=%d, int tag=%d, MPI_Comm *newintercomm=%p)", comm_to_string(local_comm), local_leader, comm_to_string(peer_comm), remote_leader, tag, newintercomm);
   return MPI_ERR_INTERN;
#endif // __IMPI_Intercomm_create_FORWARD
}
#endif // __IMPI_Intercomm_create


#ifndef __IMPI_Intercomm_merge
int IMPI_Intercomm_merge ( MPI_Comm intercomm, int high, MPI_Comm *newintracomm )
{
#if __IMPI_Intercomm_merge_FORWARD >= 1
   return PMPI_Intercomm_merge(intercomm, high, newintracomm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Intercomm_merge (MPI_Comm intercomm=%s, int high=%d, MPI_Comm *newintracomm=%p)", comm_to_string(intercomm), high, newintracomm);
   return MPI_ERR_INTERN;
#endif // __IMPI_Intercomm_merge_FORWARD
}
#endif // __IMPI_Intercomm_merge


#ifndef __IMPI_Iprobe
int IMPI_Iprobe ( int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status )
{
#if __IMPI_Iprobe_FORWARD >= 1
   return PMPI_Iprobe(source, tag, comm, flag, status);
#else
   FATAL("NOT IMPLEMENTED: MPI_Iprobe (int source=%d, int tag=%d, MPI_Comm comm=%s, int *flag=%p, MPI_Status *status=%p)", source, tag, comm_to_string(comm), flag, status);
   return MPI_ERR_INTERN;
#endif // __IMPI_Iprobe_FORWARD
}
#endif // __IMPI_Iprobe


#ifndef __IMPI_Irecv
int IMPI_Irecv ( void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request )
{
#if __IMPI_Irecv_FORWARD >= 1
   return PMPI_Irecv(buf, count, datatype, source, tag, comm, request);
#else
   FATAL("NOT IMPLEMENTED: MPI_Irecv (void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int source=%d, int tag=%d, MPI_Comm comm=%s, MPI_Request *request=%p)", buf, count, type_to_string(datatype), source, tag, comm_to_string(comm), request);
   return MPI_ERR_INTERN;
#endif // __IMPI_Irecv_FORWARD
}
#endif // __IMPI_Irecv


#ifndef __IMPI_Irsend
int IMPI_Irsend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request )
{
#if __IMPI_Irsend_FORWARD >= 1
   return PMPI_Irsend(buf, count, datatype, dest, tag, comm, request);
#else
   FATAL("NOT IMPLEMENTED: MPI_Irsend (void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s, MPI_Request *request=%p)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm), request);
   return MPI_ERR_INTERN;
#endif // __IMPI_Irsend_FORWARD
}
#endif // __IMPI_Irsend


#ifndef __IMPI_Isend
int IMPI_Isend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request )
{
#if __IMPI_Isend_FORWARD >= 1
   return PMPI_Isend(buf, count, datatype, dest, tag, comm, request);
#else
   FATAL("NOT IMPLEMENTED: MPI_Isend (void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s, MPI_Request *request=%p)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm), request);
   return MPI_ERR_INTERN;
#endif // __IMPI_Isend_FORWARD
}
#endif // __IMPI_Isend


#ifndef __IMPI_Issend
int IMPI_Issend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request )
{
#if __IMPI_Issend_FORWARD >= 1
   return PMPI_Issend(buf, count, datatype, dest, tag, comm, request);
#else
   FATAL("NOT IMPLEMENTED: MPI_Issend (void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s, MPI_Request *request=%p)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm), request);
   return MPI_ERR_INTERN;
#endif // __IMPI_Issend_FORWARD
}
#endif // __IMPI_Issend


#ifndef __IMPI_Is_thread_main
int IMPI_Is_thread_main ( int *flag )
{
#if __IMPI_Is_thread_main_FORWARD >= 1
   return PMPI_Is_thread_main(flag);
#else
   FATAL("NOT IMPLEMENTED: MPI_Is_thread_main (int *flag=%p)", flag);
   return MPI_ERR_INTERN;
#endif // __IMPI_Is_thread_main_FORWARD
}
#endif // __IMPI_Is_thread_main


#ifndef __IMPI_Keyval_create
int IMPI_Keyval_create ( MPI_Copy_function *copy_fn, MPI_Delete_function *delete_fn, int *keyval, void *extra_state )
{
#if __IMPI_Keyval_create_FORWARD >= 1
   return PMPI_Keyval_create(copy_fn, delete_fn, keyval, extra_state);
#else
   FATAL("NOT IMPLEMENTED: MPI_Keyval_create (MPI_Copy_function *copy_fn=%p, MPI_Delete_function *delete_fn=%p, int *keyval=%p, void *extra_state=%p)", copy_fn, delete_fn, keyval, extra_state);
   return MPI_ERR_INTERN;
#endif // __IMPI_Keyval_create_FORWARD
}
#endif // __IMPI_Keyval_create


#ifndef __IMPI_Keyval_free
int IMPI_Keyval_free ( int *keyval )
{
#if __IMPI_Keyval_free_FORWARD >= 1
   return PMPI_Keyval_free(keyval);
#else
   FATAL("NOT IMPLEMENTED: MPI_Keyval_free (int *keyval=%p)", keyval);
   return MPI_ERR_INTERN;
#endif // __IMPI_Keyval_free_FORWARD
}
#endif // __IMPI_Keyval_free


#ifndef __IMPI_Lookup_name
int IMPI_Lookup_name ( char *service_name, MPI_Info info, char *port_name )
{
#if __IMPI_Lookup_name_FORWARD >= 1
   return PMPI_Lookup_name(service_name, info, port_name);
#else
   FATAL("NOT IMPLEMENTED: MPI_Lookup_name (char *service_name=%p, MPI_Info info=%s, char *port_name=%p)", service_name, info_to_string(info), port_name);
   return MPI_ERR_INTERN;
#endif // __IMPI_Lookup_name_FORWARD
}
#endif // __IMPI_Lookup_name


#ifndef __IMPI_Op_commutative
int IMPI_Op_commutative ( MPI_Op op, int *commute )
{
#if __IMPI_Op_commutative_FORWARD >= 1
   return PMPI_Op_commutative(op, commute);
#else
   FATAL("NOT IMPLEMENTED: MPI_Op_commutative (MPI_Op op=%s, int *commute=%p)", op_to_string(op), commute);
   return MPI_ERR_INTERN;
#endif // __IMPI_Op_commutative_FORWARD
}
#endif // __IMPI_Op_commutative


#ifndef __IMPI_Op_create
int IMPI_Op_create ( MPI_User_function *function, int commute, MPI_Op *op )
{
#if __IMPI_Op_create_FORWARD >= 1
   return PMPI_Op_create(function, commute, op);
#else
   FATAL("NOT IMPLEMENTED: MPI_Op_create (MPI_User_function *function=%p, int commute=%d, MPI_Op *op=%p)", function, commute, op);
   return MPI_ERR_INTERN;
#endif // __IMPI_Op_create_FORWARD
}
#endif // __IMPI_Op_create


#ifndef __IMPI_Open_port
int IMPI_Open_port ( MPI_Info info, char *port_name )
{
#if __IMPI_Open_port_FORWARD >= 1
   return PMPI_Open_port(info, port_name);
#else
   FATAL("NOT IMPLEMENTED: MPI_Open_port (MPI_Info info=%s, char *port_name=%p)", info_to_string(info), port_name);
   return MPI_ERR_INTERN;
#endif // __IMPI_Open_port_FORWARD
}
#endif // __IMPI_Open_port


#ifndef __IMPI_Op_free
int IMPI_Op_free ( MPI_Op *op )
{
#if __IMPI_Op_free_FORWARD >= 1
   return PMPI_Op_free(op);
#else
   FATAL("NOT IMPLEMENTED: MPI_Op_free (MPI_Op *op=%p)", op);
   return MPI_ERR_INTERN;
#endif // __IMPI_Op_free_FORWARD
}
#endif // __IMPI_Op_free


#ifndef __IMPI_Pack_external
int IMPI_Pack_external ( char *datarep, void *inbuf, int incount, MPI_Datatype datatype, void *outbuf, MPI_Aint outcount, MPI_Aint *position )
{
#if __IMPI_Pack_external_FORWARD >= 1
   return PMPI_Pack_external(datarep, inbuf, incount, datatype, outbuf, outcount, position);
#else
   FATAL("NOT IMPLEMENTED: MPI_Pack_external (char *datarep=%p, void *inbuf=%p, int incount=%d, MPI_Datatype datatype=%s, void *outbuf=%p, MPI_Aint outcount=%p, MPI_Aint *position=%p)", datarep, inbuf, incount, type_to_string(datatype), outbuf, (void *) outcount, position);
   return MPI_ERR_INTERN;
#endif // __IMPI_Pack_external_FORWARD
}
#endif // __IMPI_Pack_external


#ifndef __IMPI_Pack_external_size
int IMPI_Pack_external_size ( char *datarep, int incount, MPI_Datatype datatype, MPI_Aint *size )
{
#if __IMPI_Pack_external_size_FORWARD >= 1
   return PMPI_Pack_external_size(datarep, incount, datatype, size);
#else
   FATAL("NOT IMPLEMENTED: MPI_Pack_external_size (char *datarep=%p, int incount=%d, MPI_Datatype datatype=%s, MPI_Aint *size=%p)", datarep, incount, type_to_string(datatype), size);
   return MPI_ERR_INTERN;
#endif // __IMPI_Pack_external_size_FORWARD
}
#endif // __IMPI_Pack_external_size


#ifndef __IMPI_Pack
int IMPI_Pack ( void *inbuf, int incount, MPI_Datatype datatype, void *outbuf, int outcount, int *position, MPI_Comm comm )
{
#if __IMPI_Pack_FORWARD >= 1
   return PMPI_Pack(inbuf, incount, datatype, outbuf, outcount, position, comm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Pack (void *inbuf=%p, int incount=%d, MPI_Datatype datatype=%s, void *outbuf=%p, int outcount=%d, int *position=%p, MPI_Comm comm=%s)", inbuf, incount, type_to_string(datatype), outbuf, outcount, position, comm_to_string(comm));
   return MPI_ERR_INTERN;
#endif // __IMPI_Pack_FORWARD
}
#endif // __IMPI_Pack


#ifndef __IMPI_Pack_size
int IMPI_Pack_size ( int incount, MPI_Datatype datatype, MPI_Comm comm, int *size )
{
#if __IMPI_Pack_size_FORWARD >= 1
   return PMPI_Pack_size(incount, datatype, comm, size);
#else
   FATAL("NOT IMPLEMENTED: MPI_Pack_size (int incount=%d, MPI_Datatype datatype=%s, MPI_Comm comm=%s, int *size=%p)", incount, type_to_string(datatype), comm_to_string(comm), size);
   return MPI_ERR_INTERN;
#endif // __IMPI_Pack_size_FORWARD
}
#endif // __IMPI_Pack_size


#ifndef __IMPI_Probe
int IMPI_Probe ( int source, int tag, MPI_Comm comm, MPI_Status *status )
{
#if __IMPI_Probe_FORWARD >= 1
   return PMPI_Probe(source, tag, comm, status);
#else
   FATAL("NOT IMPLEMENTED: MPI_Probe (int source=%d, int tag=%d, MPI_Comm comm=%s, MPI_Status *status=%p)", source, tag, comm_to_string(comm), status);
   return MPI_ERR_INTERN;
#endif // __IMPI_Probe_FORWARD
}
#endif // __IMPI_Probe


#ifndef __IMPI_Publish_name
int IMPI_Publish_name ( char *service_name, MPI_Info info, char *port_name )
{
#if __IMPI_Publish_name_FORWARD >= 1
   return PMPI_Publish_name(service_name, info, port_name);
#else
   FATAL("NOT IMPLEMENTED: MPI_Publish_name (char *service_name=%p, MPI_Info info=%s, char *port_name=%p)", service_name, info_to_string(info), port_name);
   return MPI_ERR_INTERN;
#endif // __IMPI_Publish_name_FORWARD
}
#endif // __IMPI_Publish_name


#ifndef __IMPI_Put
int IMPI_Put ( void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win )
{
#if __IMPI_Put_FORWARD >= 1
   return PMPI_Put(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
#else
   FATAL("NOT IMPLEMENTED: MPI_Put (void *origin_addr=%p, int origin_count=%d, MPI_Datatype origin_datatype=%s, int target_rank=%d, MPI_Aint target_disp=%p, int target_count=%d, MPI_Datatype target_datatype=%s, MPI_Win win=%s)", origin_addr, origin_count, type_to_string(origin_datatype), target_rank, (void *) target_disp, target_count, type_to_string(target_datatype), win_to_string(win));
   return MPI_ERR_INTERN;
#endif // __IMPI_Put_FORWARD
}
#endif // __IMPI_Put


#ifndef __IMPI_Query_thread
int IMPI_Query_thread ( int *provided )
{
#if __IMPI_Query_thread_FORWARD >= 1
   return PMPI_Query_thread(provided);
#else
   FATAL("NOT IMPLEMENTED: MPI_Query_thread (int *provided=%p)", provided);
   return MPI_ERR_INTERN;
#endif // __IMPI_Query_thread_FORWARD
}
#endif // __IMPI_Query_thread


#ifndef __IMPI_Recv
int IMPI_Recv ( void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status )
{
#if __IMPI_Recv_FORWARD >= 1
   return PMPI_Recv(buf, count, datatype, source, tag, comm, status);
#else
   FATAL("NOT IMPLEMENTED: MPI_Recv (void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int source=%d, int tag=%d, MPI_Comm comm=%s, MPI_Status *status=%p)", buf, count, type_to_string(datatype), source, tag, comm_to_string(comm), status);
   return MPI_ERR_INTERN;
#endif // __IMPI_Recv_FORWARD
}
#endif // __IMPI_Recv


#ifndef __IMPI_Recv_init
int IMPI_Recv_init ( void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *request )
{
#if __IMPI_Recv_init_FORWARD >= 1
   return PMPI_Recv_init(buf, count, datatype, source, tag, comm, request);
#else
   FATAL("NOT IMPLEMENTED: MPI_Recv_init (void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int source=%d, int tag=%d, MPI_Comm comm=%s, MPI_Request *request=%p)", buf, count, type_to_string(datatype), source, tag, comm_to_string(comm), request);
   return MPI_ERR_INTERN;
#endif // __IMPI_Recv_init_FORWARD
}
#endif // __IMPI_Recv_init


#ifndef __IMPI_Reduce
int IMPI_Reduce ( void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm )
{
#if __IMPI_Reduce_FORWARD >= 1
   return PMPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Reduce (void *sendbuf=%p, void *recvbuf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Op op=%s, int root=%d, MPI_Comm comm=%s)", sendbuf, recvbuf, count, type_to_string(datatype), op_to_string(op), root, comm_to_string(comm));
   return MPI_ERR_INTERN;
#endif // __IMPI_Reduce_FORWARD
}
#endif // __IMPI_Reduce


#ifndef __IMPI_Reduce_local
int IMPI_Reduce_local ( void *inbuf, void *inoutbuf, int count, MPI_Datatype datatype, MPI_Op op )
{
#if __IMPI_Reduce_local_FORWARD >= 1
   return PMPI_Reduce_local(inbuf, inoutbuf, count, datatype, op);
#else
   FATAL("NOT IMPLEMENTED: MPI_Reduce_local (void *inbuf=%p, void *inoutbuf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Op op=%s)", inbuf, inoutbuf, count, type_to_string(datatype), op_to_string(op));
   return MPI_ERR_INTERN;
#endif // __IMPI_Reduce_local_FORWARD
}
#endif // __IMPI_Reduce_local


#ifndef __IMPI_Reduce_scatter_block
int IMPI_Reduce_scatter_block ( void *sendbuf, void *recvbuf, int recvcount, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm )
{
#if __IMPI_Reduce_scatter_block_FORWARD >= 1
   return PMPI_Reduce_scatter_block(sendbuf, recvbuf, recvcount, datatype, op, comm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Reduce_scatter_block (void *sendbuf=%p, void *recvbuf=%p, int recvcount=%d, MPI_Datatype datatype=%s, MPI_Op op=%s, MPI_Comm comm=%s)", sendbuf, recvbuf, recvcount, type_to_string(datatype), op_to_string(op), comm_to_string(comm));
   return MPI_ERR_INTERN;
#endif // __IMPI_Reduce_scatter_block_FORWARD
}
#endif // __IMPI_Reduce_scatter_block


#ifndef __IMPI_Reduce_scatter
int IMPI_Reduce_scatter ( void *sendbuf, void *recvbuf, int *recvcnts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm )
{
#if __IMPI_Reduce_scatter_FORWARD >= 1
   return PMPI_Reduce_scatter(sendbuf, recvbuf, recvcnts, datatype, op, comm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Reduce_scatter (void *sendbuf=%p, void *recvbuf=%p, int *recvcnts=%p, MPI_Datatype datatype=%s, MPI_Op op=%s, MPI_Comm comm=%s)", sendbuf, recvbuf, recvcnts, type_to_string(datatype), op_to_string(op), comm_to_string(comm));
   return MPI_ERR_INTERN;
#endif // __IMPI_Reduce_scatter_FORWARD
}
#endif // __IMPI_Reduce_scatter


#ifndef __IMPI_Register_datarep
int IMPI_Register_datarep ( char *name, MPI_Datarep_conversion_function *read_conv_fn, MPI_Datarep_conversion_function *write_conv_fn, MPI_Datarep_extent_function *extent_fn, void *state )
{
#if __IMPI_Register_datarep_FORWARD >= 1
   return PMPI_Register_datarep(name, read_conv_fn, write_conv_fn, extent_fn, state);
#else
   FATAL("NOT IMPLEMENTED: MPI_Register_datarep (char *name=%p, MPI_Datarep_conversion_function *read_conv_fn=%p, MPI_Datarep_conversion_function *write_conv_fn=%p, MPI_Datarep_extent_function *extent_fn=%p, void *state=%p)", name, read_conv_fn, write_conv_fn, extent_fn, state);
   return MPI_ERR_INTERN;
#endif // __IMPI_Register_datarep_FORWARD
}
#endif // __IMPI_Register_datarep


#ifndef __IMPI_Request_free
int IMPI_Request_free ( MPI_Request *request )
{
#if __IMPI_Request_free_FORWARD >= 1
   return PMPI_Request_free(request);
#else
   FATAL("NOT IMPLEMENTED: MPI_Request_free (MPI_Request *request=%p)", request);
   return MPI_ERR_INTERN;
#endif // __IMPI_Request_free_FORWARD
}
#endif // __IMPI_Request_free


#ifndef __IMPI_Request_get_status
int IMPI_Request_get_status ( MPI_Request request, int *flag, MPI_Status *status )
{
#if __IMPI_Request_get_status_FORWARD >= 1
   return PMPI_Request_get_status(request, flag, status);
#else
   FATAL("NOT IMPLEMENTED: MPI_Request_get_status (MPI_Request request=%s, int *flag=%p, MPI_Status *status=%p)", request_to_string(request), flag, status);
   return MPI_ERR_INTERN;
#endif // __IMPI_Request_get_status_FORWARD
}
#endif // __IMPI_Request_get_status


#ifndef __IMPI_Rsend
int IMPI_Rsend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm )
{
#if __IMPI_Rsend_FORWARD >= 1
   return PMPI_Rsend(buf, count, datatype, dest, tag, comm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Rsend (void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm));
   return MPI_ERR_INTERN;
#endif // __IMPI_Rsend_FORWARD
}
#endif // __IMPI_Rsend


#ifndef __IMPI_Rsend_init
int IMPI_Rsend_init ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request )
{
#if __IMPI_Rsend_init_FORWARD >= 1
   return PMPI_Rsend_init(buf, count, datatype, dest, tag, comm, request);
#else
   FATAL("NOT IMPLEMENTED: MPI_Rsend_init (void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s, MPI_Request *request=%p)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm), request);
   return MPI_ERR_INTERN;
#endif // __IMPI_Rsend_init_FORWARD
}
#endif // __IMPI_Rsend_init


#ifndef __IMPI_Scan
int IMPI_Scan ( void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm )
{
#if __IMPI_Scan_FORWARD >= 1
   return PMPI_Scan(sendbuf, recvbuf, count, datatype, op, comm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Scan (void *sendbuf=%p, void *recvbuf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Op op=%s, MPI_Comm comm=%s)", sendbuf, recvbuf, count, type_to_string(datatype), op_to_string(op), comm_to_string(comm));
   return MPI_ERR_INTERN;
#endif // __IMPI_Scan_FORWARD
}
#endif // __IMPI_Scan


#ifndef __IMPI_Scatter
int IMPI_Scatter ( void *sendbuf, int sendcnt, MPI_Datatype sendtype, void *recvbuf, int recvcnt, MPI_Datatype recvtype, int root, MPI_Comm comm )
{
#if __IMPI_Scatter_FORWARD >= 1
   return PMPI_Scatter(sendbuf, sendcnt, sendtype, recvbuf, recvcnt, recvtype, root, comm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Scatter (void *sendbuf=%p, int sendcnt=%d, MPI_Datatype sendtype=%s, void *recvbuf=%p, int recvcnt=%d, MPI_Datatype recvtype=%s, int root=%d, MPI_Comm comm=%s)", sendbuf, sendcnt, type_to_string(sendtype), recvbuf, recvcnt, type_to_string(recvtype), root, comm_to_string(comm));
   return MPI_ERR_INTERN;
#endif // __IMPI_Scatter_FORWARD
}
#endif // __IMPI_Scatter


#ifndef __IMPI_Scatterv
int IMPI_Scatterv ( void *sendbuf, int *sendcnts, int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcnt, MPI_Datatype recvtype, int root, MPI_Comm comm )
{
#if __IMPI_Scatterv_FORWARD >= 1
   return PMPI_Scatterv(sendbuf, sendcnts, displs, sendtype, recvbuf, recvcnt, recvtype, root, comm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Scatterv (void *sendbuf=%p, int *sendcnts=%p, int *displs=%p, MPI_Datatype sendtype=%s, void *recvbuf=%p, int recvcnt=%d, MPI_Datatype recvtype=%s, int root=%d, MPI_Comm comm=%s)", sendbuf, sendcnts, displs, type_to_string(sendtype), recvbuf, recvcnt, type_to_string(recvtype), root, comm_to_string(comm));
   return MPI_ERR_INTERN;
#endif // __IMPI_Scatterv_FORWARD
}
#endif // __IMPI_Scatterv


#ifndef __IMPI_Send
int IMPI_Send ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm )
{
#if __IMPI_Send_FORWARD >= 1
   return PMPI_Send(buf, count, datatype, dest, tag, comm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Send (void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm));
   return MPI_ERR_INTERN;
#endif // __IMPI_Send_FORWARD
}
#endif // __IMPI_Send


#ifndef __IMPI_Send_init
int IMPI_Send_init ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request )
{
#if __IMPI_Send_init_FORWARD >= 1
   return PMPI_Send_init(buf, count, datatype, dest, tag, comm, request);
#else
   FATAL("NOT IMPLEMENTED: MPI_Send_init (void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s, MPI_Request *request=%p)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm), request);
   return MPI_ERR_INTERN;
#endif // __IMPI_Send_init_FORWARD
}
#endif // __IMPI_Send_init


#ifndef __IMPI_Sendrecv
int IMPI_Sendrecv ( void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status )
{
#if __IMPI_Sendrecv_FORWARD >= 1
   return PMPI_Sendrecv(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status);
#else
   FATAL("NOT IMPLEMENTED: MPI_Sendrecv (void *sendbuf=%p, int sendcount=%d, MPI_Datatype sendtype=%s, int dest=%d, int sendtag=%d, void *recvbuf=%p, int recvcount=%d, MPI_Datatype recvtype=%s, int source=%d, int recvtag=%d, MPI_Comm comm=%s, MPI_Status *status=%p)", sendbuf, sendcount, type_to_string(sendtype), dest, sendtag, recvbuf, recvcount, type_to_string(recvtype), source, recvtag, comm_to_string(comm), status);
   return MPI_ERR_INTERN;
#endif // __IMPI_Sendrecv_FORWARD
}
#endif // __IMPI_Sendrecv


#ifndef __IMPI_Sendrecv_replace
int IMPI_Sendrecv_replace ( void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status )
{
#if __IMPI_Sendrecv_replace_FORWARD >= 1
   return PMPI_Sendrecv_replace(buf, count, datatype, dest, sendtag, source, recvtag, comm, status);
#else
   FATAL("NOT IMPLEMENTED: MPI_Sendrecv_replace (void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int dest=%d, int sendtag=%d, int source=%d, int recvtag=%d, MPI_Comm comm=%s, MPI_Status *status=%p)", buf, count, type_to_string(datatype), dest, sendtag, source, recvtag, comm_to_string(comm), status);
   return MPI_ERR_INTERN;
#endif // __IMPI_Sendrecv_replace_FORWARD
}
#endif // __IMPI_Sendrecv_replace


#ifndef __IMPI_Ssend
int IMPI_Ssend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm )
{
#if __IMPI_Ssend_FORWARD >= 1
   return PMPI_Ssend(buf, count, datatype, dest, tag, comm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Ssend (void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm));
   return MPI_ERR_INTERN;
#endif // __IMPI_Ssend_FORWARD
}
#endif // __IMPI_Ssend


#ifndef __IMPI_Ssend_init
int IMPI_Ssend_init ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *request )
{
#if __IMPI_Ssend_init_FORWARD >= 1
   return PMPI_Ssend_init(buf, count, datatype, dest, tag, comm, request);
#else
   FATAL("NOT IMPLEMENTED: MPI_Ssend_init (void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s, MPI_Request *request=%p)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm), request);
   return MPI_ERR_INTERN;
#endif // __IMPI_Ssend_init_FORWARD
}
#endif // __IMPI_Ssend_init


#ifndef __IMPI_Startall
int IMPI_Startall ( int count, MPI_Request array_of_requests[] )
{
#if __IMPI_Startall_FORWARD >= 1
   return PMPI_Startall(count, array_of_requests);
#else
   FATAL("NOT IMPLEMENTED: MPI_Startall (int count=%d, MPI_Request array_of_requests[]=%p)", count, array_of_requests);
   return MPI_ERR_INTERN;
#endif // __IMPI_Startall_FORWARD
}
#endif // __IMPI_Startall


#ifndef __IMPI_Start
int IMPI_Start ( MPI_Request *request )
{
#if __IMPI_Start_FORWARD >= 1
   return PMPI_Start(request);
#else
   FATAL("NOT IMPLEMENTED: MPI_Start (MPI_Request *request=%p)", request);
   return MPI_ERR_INTERN;
#endif // __IMPI_Start_FORWARD
}
#endif // __IMPI_Start


#ifndef __IMPI_Status_set_cancelled
int IMPI_Status_set_cancelled ( MPI_Status *status, int flag )
{
#if __IMPI_Status_set_cancelled_FORWARD >= 1
   return PMPI_Status_set_cancelled(status, flag);
#else
   FATAL("NOT IMPLEMENTED: MPI_Status_set_cancelled (MPI_Status *status=%p, int flag=%d)", status, flag);
   return MPI_ERR_INTERN;
#endif // __IMPI_Status_set_cancelled_FORWARD
}
#endif // __IMPI_Status_set_cancelled


#ifndef __IMPI_Status_set_elements
int IMPI_Status_set_elements ( MPI_Status *status, MPI_Datatype datatype, int count )
{
#if __IMPI_Status_set_elements_FORWARD >= 1
   return PMPI_Status_set_elements(status, datatype, count);
#else
   FATAL("NOT IMPLEMENTED: MPI_Status_set_elements (MPI_Status *status=%p, MPI_Datatype datatype=%s, int count=%d)", status, type_to_string(datatype), count);
   return MPI_ERR_INTERN;
#endif // __IMPI_Status_set_elements_FORWARD
}
#endif // __IMPI_Status_set_elements


#ifndef __IMPI_Testall
int IMPI_Testall ( int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[] )
{
#if __IMPI_Testall_FORWARD >= 1
   return PMPI_Testall(count, array_of_requests, flag, array_of_statuses);
#else
   FATAL("NOT IMPLEMENTED: MPI_Testall (int count=%d, MPI_Request array_of_requests[]=%p, int *flag=%p, MPI_Status array_of_statuses[]=%p)", count, array_of_requests, flag, array_of_statuses);
   return MPI_ERR_INTERN;
#endif // __IMPI_Testall_FORWARD
}
#endif // __IMPI_Testall


#ifndef __IMPI_Testany
int IMPI_Testany ( int count, MPI_Request array_of_requests[], int *index, int *flag, MPI_Status *status )
{
#if __IMPI_Testany_FORWARD >= 1
   return PMPI_Testany(count, array_of_requests, index, flag, status);
#else
   FATAL("NOT IMPLEMENTED: MPI_Testany (int count=%d, MPI_Request array_of_requests[]=%p, int *index=%p, int *flag=%p, MPI_Status *status=%p)", count, array_of_requests, index, flag, status);
   return MPI_ERR_INTERN;
#endif // __IMPI_Testany_FORWARD
}
#endif // __IMPI_Testany


#ifndef __IMPI_Test_cancelled
int IMPI_Test_cancelled ( MPI_Status *status, int *flag )
{
#if __IMPI_Test_cancelled_FORWARD >= 1
   return PMPI_Test_cancelled(status, flag);
#else
   FATAL("NOT IMPLEMENTED: MPI_Test_cancelled (MPI_Status *status=%p, int *flag=%p)", status, flag);
   return MPI_ERR_INTERN;
#endif // __IMPI_Test_cancelled_FORWARD
}
#endif // __IMPI_Test_cancelled


#ifndef __IMPI_Test
int IMPI_Test ( MPI_Request *request, int *flag, MPI_Status *status )
{
#if __IMPI_Test_FORWARD >= 1
   return PMPI_Test(request, flag, status);
#else
   FATAL("NOT IMPLEMENTED: MPI_Test (MPI_Request *request=%p, int *flag=%p, MPI_Status *status=%p)", request, flag, status);
   return MPI_ERR_INTERN;
#endif // __IMPI_Test_FORWARD
}
#endif // __IMPI_Test


#ifndef __IMPI_Testsome
int IMPI_Testsome ( int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[] )
{
#if __IMPI_Testsome_FORWARD >= 1
   return PMPI_Testsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
#else
   FATAL("NOT IMPLEMENTED: MPI_Testsome (int incount=%d, MPI_Request array_of_requests[]=%p, int *outcount=%p, int array_of_indices[]=%p, MPI_Status array_of_statuses[]=%p)", incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
   return MPI_ERR_INTERN;
#endif // __IMPI_Testsome_FORWARD
}
#endif // __IMPI_Testsome


#ifndef __IMPI_Topo_test
int IMPI_Topo_test ( MPI_Comm comm, int *topo_type )
{
#if __IMPI_Topo_test_FORWARD >= 1
   return PMPI_Topo_test(comm, topo_type);
#else
   FATAL("NOT IMPLEMENTED: MPI_Topo_test (MPI_Comm comm=%s, int *topo_type=%p)", comm_to_string(comm), topo_type);
   return MPI_ERR_INTERN;
#endif // __IMPI_Topo_test_FORWARD
}
#endif // __IMPI_Topo_test


#ifndef __IMPI_Type_commit
int IMPI_Type_commit ( MPI_Datatype *datatype )
{
#if __IMPI_Type_commit_FORWARD >= 1
   return PMPI_Type_commit(datatype);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_commit (MPI_Datatype *datatype=%p)", datatype);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_commit_FORWARD
}
#endif // __IMPI_Type_commit


#ifndef __IMPI_Type_contiguous
int IMPI_Type_contiguous ( int count, MPI_Datatype old_type, MPI_Datatype *new_type_p )
{
#if __IMPI_Type_contiguous_FORWARD >= 1
   return PMPI_Type_contiguous(count, old_type, new_type_p);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_contiguous (int count=%d, MPI_Datatype old_type=%s, MPI_Datatype *new_type_p=%p)", count, type_to_string(old_type), new_type_p);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_contiguous_FORWARD
}
#endif // __IMPI_Type_contiguous


#ifndef __IMPI_Type_create_darray
int IMPI_Type_create_darray ( int size, int rank, int ndims, int array_of_gsizes[], int array_of_distribs[], int array_of_dargs[], int array_of_psizes[], int order, MPI_Datatype oldtype, MPI_Datatype *newtype )
{
#if __IMPI_Type_create_darray_FORWARD >= 1
   return PMPI_Type_create_darray(size, rank, ndims, array_of_gsizes, array_of_distribs, array_of_dargs, array_of_psizes, order, oldtype, newtype);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_create_darray (int size=%d, int rank=%d, int ndims=%d, int array_of_gsizes[]=%p, int array_of_distribs[]=%p, int array_of_dargs[]=%p, int array_of_psizes[]=%p, int order=%d, MPI_Datatype oldtype=%s, MPI_Datatype *newtype=%p)", size, rank, ndims, array_of_gsizes, array_of_distribs, array_of_dargs, array_of_psizes, order, type_to_string(oldtype), newtype);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_create_darray_FORWARD
}
#endif // __IMPI_Type_create_darray


#ifndef __IMPI_Type_create_hindexed
int IMPI_Type_create_hindexed ( int count, int blocklengths[], MPI_Aint displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype )
{
#if __IMPI_Type_create_hindexed_FORWARD >= 1
   return PMPI_Type_create_hindexed(count, blocklengths, displacements, oldtype, newtype);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_create_hindexed (int count=%d, int blocklengths[]=%p, MPI_Aint displacements[]=%p, MPI_Datatype oldtype=%s, MPI_Datatype *newtype=%p)", count, blocklengths, displacements, type_to_string(oldtype), newtype);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_create_hindexed_FORWARD
}
#endif // __IMPI_Type_create_hindexed


#ifndef __IMPI_Type_create_hvector
int IMPI_Type_create_hvector ( int count, int blocklength, MPI_Aint stride, MPI_Datatype oldtype, MPI_Datatype *newtype )
{
#if __IMPI_Type_create_hvector_FORWARD >= 1
   return PMPI_Type_create_hvector(count, blocklength, stride, oldtype, newtype);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_create_hvector (int count=%d, int blocklength=%d, MPI_Aint stride=%p, MPI_Datatype oldtype=%s, MPI_Datatype *newtype=%p)", count, blocklength, (void *) stride, type_to_string(oldtype), newtype);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_create_hvector_FORWARD
}
#endif // __IMPI_Type_create_hvector


#ifndef __IMPI_Type_create_indexed_block
int IMPI_Type_create_indexed_block ( int count, int blocklength, int array_of_displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype )
{
#if __IMPI_Type_create_indexed_block_FORWARD >= 1
   return PMPI_Type_create_indexed_block(count, blocklength, array_of_displacements, oldtype, newtype);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_create_indexed_block (int count=%d, int blocklength=%d, int array_of_displacements[]=%p, MPI_Datatype oldtype=%s, MPI_Datatype *newtype=%p)", count, blocklength, array_of_displacements, type_to_string(oldtype), newtype);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_create_indexed_block_FORWARD
}
#endif // __IMPI_Type_create_indexed_block


#ifndef __IMPI_Type_create_keyval
int IMPI_Type_create_keyval ( MPI_Type_copy_attr_function *type_copy_attr_fn, MPI_Type_delete_attr_function *type_delete_attr_fn, int *type_keyval, void *extra_state )
{
#if __IMPI_Type_create_keyval_FORWARD >= 1
   return PMPI_Type_create_keyval(type_copy_attr_fn, type_delete_attr_fn, type_keyval, extra_state);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_create_keyval (MPI_Type_copy_attr_function *type_copy_attr_fn=%p, MPI_Type_delete_attr_function *type_delete_attr_fn=%p, int *type_keyval=%p, void *extra_state=%p)", type_copy_attr_fn, type_delete_attr_fn, type_keyval, extra_state);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_create_keyval_FORWARD
}
#endif // __IMPI_Type_create_keyval


#ifndef __IMPI_Type_create_resized
int IMPI_Type_create_resized ( MPI_Datatype oldtype, MPI_Aint lb, MPI_Aint extent, MPI_Datatype *newtype )
{
#if __IMPI_Type_create_resized_FORWARD >= 1
   return PMPI_Type_create_resized(oldtype, lb, extent, newtype);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_create_resized (MPI_Datatype oldtype=%s, MPI_Aint lb=%p, MPI_Aint extent=%p, MPI_Datatype *newtype=%p)", type_to_string(oldtype), (void *) lb, (void *) extent, newtype);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_create_resized_FORWARD
}
#endif // __IMPI_Type_create_resized


#ifndef __IMPI_Type_create_struct
int IMPI_Type_create_struct ( int count, int array_of_blocklengths[], MPI_Aint array_of_displacements[], MPI_Datatype array_of_types[], MPI_Datatype *newtype )
{
#if __IMPI_Type_create_struct_FORWARD >= 1
   return PMPI_Type_create_struct(count, array_of_blocklengths, array_of_displacements, array_of_types, newtype);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_create_struct (int count=%d, int array_of_blocklengths[]=%p, MPI_Aint array_of_displacements[]=%p, MPI_Datatype array_of_types[]=%p, MPI_Datatype *newtype=%p)", count, array_of_blocklengths, array_of_displacements, array_of_types, newtype);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_create_struct_FORWARD
}
#endif // __IMPI_Type_create_struct


#ifndef __IMPI_Type_create_subarray
int IMPI_Type_create_subarray ( int ndims, int array_of_sizes[], int array_of_subsizes[], int array_of_starts[], int order, MPI_Datatype oldtype, MPI_Datatype *newtype )
{
#if __IMPI_Type_create_subarray_FORWARD >= 1
   return PMPI_Type_create_subarray(ndims, array_of_sizes, array_of_subsizes, array_of_starts, order, oldtype, newtype);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_create_subarray (int ndims=%d, int array_of_sizes[]=%p, int array_of_subsizes[]=%p, int array_of_starts[]=%p, int order=%d, MPI_Datatype oldtype=%s, MPI_Datatype *newtype=%p)", ndims, array_of_sizes, array_of_subsizes, array_of_starts, order, type_to_string(oldtype), newtype);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_create_subarray_FORWARD
}
#endif // __IMPI_Type_create_subarray


#ifndef __IMPI_Type_delete_attr
int IMPI_Type_delete_attr ( MPI_Datatype type, int type_keyval )
{
#if __IMPI_Type_delete_attr_FORWARD >= 1
   return PMPI_Type_delete_attr(type, type_keyval);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_delete_attr (MPI_Datatype type=%s, int type_keyval=%d)", type_to_string(type), type_keyval);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_delete_attr_FORWARD
}
#endif // __IMPI_Type_delete_attr


#ifndef __IMPI_Type_dup
int IMPI_Type_dup ( MPI_Datatype datatype, MPI_Datatype *newtype )
{
#if __IMPI_Type_dup_FORWARD >= 1
   return PMPI_Type_dup(datatype, newtype);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_dup (MPI_Datatype datatype=%s, MPI_Datatype *newtype=%p)", type_to_string(datatype), newtype);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_dup_FORWARD
}
#endif // __IMPI_Type_dup


#ifndef __IMPI_Type_extent
int IMPI_Type_extent ( MPI_Datatype datatype, MPI_Aint *extent )
{
#if __IMPI_Type_extent_FORWARD >= 1
   return PMPI_Type_extent(datatype, extent);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_extent (MPI_Datatype datatype=%s, MPI_Aint *extent=%p)", type_to_string(datatype), extent);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_extent_FORWARD
}
#endif // __IMPI_Type_extent


#ifndef __IMPI_Type_free
int IMPI_Type_free ( MPI_Datatype *datatype )
{
#if __IMPI_Type_free_FORWARD >= 1
   return PMPI_Type_free(datatype);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_free (MPI_Datatype *datatype=%p)", datatype);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_free_FORWARD
}
#endif // __IMPI_Type_free


#ifndef __IMPI_Type_free_keyval
int IMPI_Type_free_keyval ( int *type_keyval )
{
#if __IMPI_Type_free_keyval_FORWARD >= 1
   return PMPI_Type_free_keyval(type_keyval);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_free_keyval (int *type_keyval=%p)", type_keyval);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_free_keyval_FORWARD
}
#endif // __IMPI_Type_free_keyval


#ifndef __IMPI_Type_get_attr
int IMPI_Type_get_attr ( MPI_Datatype type, int type_keyval, void *attribute_val, int *flag )
{
#if __IMPI_Type_get_attr_FORWARD >= 1
   return PMPI_Type_get_attr(type, type_keyval, attribute_val, flag);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_get_attr (MPI_Datatype type=%s, int type_keyval=%d, void *attribute_val=%p, int *flag=%p)", type_to_string(type), type_keyval, attribute_val, flag);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_get_attr_FORWARD
}
#endif // __IMPI_Type_get_attr


#ifndef __IMPI_Type_get_contents
int IMPI_Type_get_contents ( MPI_Datatype datatype, int max_integers, int max_addresses, int max_datatypes, int array_of_integers[], MPI_Aint array_of_addresses[], MPI_Datatype array_of_datatypes[] )
{
#if __IMPI_Type_get_contents_FORWARD >= 1
   return PMPI_Type_get_contents(datatype, max_integers, max_addresses, max_datatypes, array_of_integers, array_of_addresses, array_of_datatypes);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_get_contents (MPI_Datatype datatype=%s, int max_integers=%d, int max_addresses=%d, int max_datatypes=%d, int array_of_integers[]=%p, MPI_Aint array_of_addresses[]=%p, MPI_Datatype array_of_datatypes[]=%p)", type_to_string(datatype), max_integers, max_addresses, max_datatypes, array_of_integers, array_of_addresses, array_of_datatypes);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_get_contents_FORWARD
}
#endif // __IMPI_Type_get_contents


#ifndef __IMPI_Type_get_envelope
int IMPI_Type_get_envelope ( MPI_Datatype datatype, int *num_integers, int *num_addresses, int *num_datatypes, int *combiner )
{
#if __IMPI_Type_get_envelope_FORWARD >= 1
   return PMPI_Type_get_envelope(datatype, num_integers, num_addresses, num_datatypes, combiner);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_get_envelope (MPI_Datatype datatype=%s, int *num_integers=%p, int *num_addresses=%p, int *num_datatypes=%p, int *combiner=%p)", type_to_string(datatype), num_integers, num_addresses, num_datatypes, combiner);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_get_envelope_FORWARD
}
#endif // __IMPI_Type_get_envelope


#ifndef __IMPI_Type_get_extent
int IMPI_Type_get_extent ( MPI_Datatype datatype, MPI_Aint *lb, MPI_Aint *extent )
{
#if __IMPI_Type_get_extent_FORWARD >= 1
   return PMPI_Type_get_extent(datatype, lb, extent);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_get_extent (MPI_Datatype datatype=%s, MPI_Aint *lb=%p, MPI_Aint *extent=%p)", type_to_string(datatype), lb, extent);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_get_extent_FORWARD
}
#endif // __IMPI_Type_get_extent


#ifndef __IMPI_Type_get_name
int IMPI_Type_get_name ( MPI_Datatype datatype, char *type_name, int *resultlen )
{
#if __IMPI_Type_get_name_FORWARD >= 1
   return PMPI_Type_get_name(datatype, type_name, resultlen);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_get_name (MPI_Datatype datatype=%s, char *type_name=%p, int *resultlen=%p)", type_to_string(datatype), type_name, resultlen);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_get_name_FORWARD
}
#endif // __IMPI_Type_get_name


#ifndef __IMPI_Type_get_true_extent
int IMPI_Type_get_true_extent ( MPI_Datatype datatype, MPI_Aint *true_lb, MPI_Aint *true_extent )
{
#if __IMPI_Type_get_true_extent_FORWARD >= 1
   return PMPI_Type_get_true_extent(datatype, true_lb, true_extent);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_get_true_extent (MPI_Datatype datatype=%s, MPI_Aint *true_lb=%p, MPI_Aint *true_extent=%p)", type_to_string(datatype), true_lb, true_extent);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_get_true_extent_FORWARD
}
#endif // __IMPI_Type_get_true_extent


#ifndef __IMPI_Type_hindexed
int IMPI_Type_hindexed ( int count, int blocklens[], MPI_Aint indices[], MPI_Datatype old_type, MPI_Datatype *newtype )
{
#if __IMPI_Type_hindexed_FORWARD >= 1
   return PMPI_Type_hindexed(count, blocklens, indices, old_type, newtype);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_hindexed (int count=%d, int blocklens[]=%p, MPI_Aint indices[]=%p, MPI_Datatype old_type=%s, MPI_Datatype *newtype=%p)", count, blocklens, indices, type_to_string(old_type), newtype);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_hindexed_FORWARD
}
#endif // __IMPI_Type_hindexed


#ifndef __IMPI_Type_hvector
int IMPI_Type_hvector ( int count, int blocklen, MPI_Aint stride, MPI_Datatype old_type, MPI_Datatype *newtype_p )
{
#if __IMPI_Type_hvector_FORWARD >= 1
   return PMPI_Type_hvector(count, blocklen, stride, old_type, newtype_p);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_hvector (int count=%d, int blocklen=%d, MPI_Aint stride=%p, MPI_Datatype old_type=%s, MPI_Datatype *newtype_p=%p)", count, blocklen, (void *) stride, type_to_string(old_type), newtype_p);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_hvector_FORWARD
}
#endif // __IMPI_Type_hvector


#ifndef __IMPI_Type_indexed
int IMPI_Type_indexed ( int count, int blocklens[], int indices[], MPI_Datatype old_type, MPI_Datatype *newtype )
{
#if __IMPI_Type_indexed_FORWARD >= 1
   return PMPI_Type_indexed(count, blocklens, indices, old_type, newtype);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_indexed (int count=%d, int blocklens[]=%p, int indices[]=%p, MPI_Datatype old_type=%s, MPI_Datatype *newtype=%p)", count, blocklens, indices, type_to_string(old_type), newtype);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_indexed_FORWARD
}
#endif // __IMPI_Type_indexed


#ifndef __IMPI_Type_lb
int IMPI_Type_lb ( MPI_Datatype datatype, MPI_Aint *displacement )
{
#if __IMPI_Type_lb_FORWARD >= 1
   return PMPI_Type_lb(datatype, displacement);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_lb (MPI_Datatype datatype=%s, MPI_Aint *displacement=%p)", type_to_string(datatype), displacement);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_lb_FORWARD
}
#endif // __IMPI_Type_lb


#ifndef __IMPI_Type_match_size
int IMPI_Type_match_size ( int typeclass, int size, MPI_Datatype *datatype )
{
#if __IMPI_Type_match_size_FORWARD >= 1
   return PMPI_Type_match_size(typeclass, size, datatype);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_match_size (int typeclass=%d, int size=%d, MPI_Datatype *datatype=%p)", typeclass, size, datatype);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_match_size_FORWARD
}
#endif // __IMPI_Type_match_size


#ifndef __IMPI_Type_set_attr
int IMPI_Type_set_attr ( MPI_Datatype type, int type_keyval, void *attribute_val )
{
#if __IMPI_Type_set_attr_FORWARD >= 1
   return PMPI_Type_set_attr(type, type_keyval, attribute_val);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_set_attr (MPI_Datatype type=%s, int type_keyval=%d, void *attribute_val=%p)", type_to_string(type), type_keyval, attribute_val);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_set_attr_FORWARD
}
#endif // __IMPI_Type_set_attr


#ifndef __IMPI_Type_set_name
int IMPI_Type_set_name ( MPI_Datatype type, char *type_name )
{
#if __IMPI_Type_set_name_FORWARD >= 1
   return PMPI_Type_set_name(type, type_name);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_set_name (MPI_Datatype type=%s, char *type_name=%p)", type_to_string(type), type_name);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_set_name_FORWARD
}
#endif // __IMPI_Type_set_name


#ifndef __IMPI_Type_size
int IMPI_Type_size ( MPI_Datatype datatype, int *size )
{
#if __IMPI_Type_size_FORWARD >= 1
   return PMPI_Type_size(datatype, size);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_size (MPI_Datatype datatype=%s, int *size=%p)", type_to_string(datatype), size);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_size_FORWARD
}
#endif // __IMPI_Type_size


#ifndef __IMPI_Type_struct
int IMPI_Type_struct ( int count, int blocklens[], MPI_Aint indices[], MPI_Datatype old_types[], MPI_Datatype *newtype )
{
#if __IMPI_Type_struct_FORWARD >= 1
   return PMPI_Type_struct(count, blocklens, indices, old_types, newtype);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_struct (int count=%d, int blocklens[]=%p, MPI_Aint indices[]=%p, MPI_Datatype old_types[]=%p, MPI_Datatype *newtype=%p)", count, blocklens, indices, old_types, newtype);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_struct_FORWARD
}
#endif // __IMPI_Type_struct


#ifndef __IMPI_Type_ub
int IMPI_Type_ub ( MPI_Datatype datatype, MPI_Aint *displacement )
{
#if __IMPI_Type_ub_FORWARD >= 1
   return PMPI_Type_ub(datatype, displacement);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_ub (MPI_Datatype datatype=%s, MPI_Aint *displacement=%p)", type_to_string(datatype), displacement);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_ub_FORWARD
}
#endif // __IMPI_Type_ub


#ifndef __IMPI_Type_vector
int IMPI_Type_vector ( int count, int blocklength, int stride, MPI_Datatype old_type, MPI_Datatype *newtype_p )
{
#if __IMPI_Type_vector_FORWARD >= 1
   return PMPI_Type_vector(count, blocklength, stride, old_type, newtype_p);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_vector (int count=%d, int blocklength=%d, int stride=%d, MPI_Datatype old_type=%s, MPI_Datatype *newtype_p=%p)", count, blocklength, stride, type_to_string(old_type), newtype_p);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_vector_FORWARD
}
#endif // __IMPI_Type_vector


#ifndef __IMPI_Unpack_external
int IMPI_Unpack_external ( char *datarep, void *inbuf, MPI_Aint insize, MPI_Aint *position, void *outbuf, int outcount, MPI_Datatype datatype )
{
#if __IMPI_Unpack_external_FORWARD >= 1
   return PMPI_Unpack_external(datarep, inbuf, insize, position, outbuf, outcount, datatype);
#else
   FATAL("NOT IMPLEMENTED: MPI_Unpack_external (char *datarep=%p, void *inbuf=%p, MPI_Aint insize=%p, MPI_Aint *position=%p, void *outbuf=%p, int outcount=%d, MPI_Datatype datatype=%s)", datarep, inbuf, (void *) insize, position, outbuf, outcount, type_to_string(datatype));
   return MPI_ERR_INTERN;
#endif // __IMPI_Unpack_external_FORWARD
}
#endif // __IMPI_Unpack_external


#ifndef __IMPI_Unpack
int IMPI_Unpack ( void *inbuf, int insize, int *position, void *outbuf, int outcount, MPI_Datatype datatype, MPI_Comm comm )
{
#if __IMPI_Unpack_FORWARD >= 1
   return PMPI_Unpack(inbuf, insize, position, outbuf, outcount, datatype, comm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Unpack (void *inbuf=%p, int insize=%d, int *position=%p, void *outbuf=%p, int outcount=%d, MPI_Datatype datatype=%s, MPI_Comm comm=%s)", inbuf, insize, position, outbuf, outcount, type_to_string(datatype), comm_to_string(comm));
   return MPI_ERR_INTERN;
#endif // __IMPI_Unpack_FORWARD
}
#endif // __IMPI_Unpack


#ifndef __IMPI_Unpublish_name
int IMPI_Unpublish_name ( char *service_name, MPI_Info info, char *port_name )
{
#if __IMPI_Unpublish_name_FORWARD >= 1
   return PMPI_Unpublish_name(service_name, info, port_name);
#else
   FATAL("NOT IMPLEMENTED: MPI_Unpublish_name (char *service_name=%p, MPI_Info info=%s, char *port_name=%p)", service_name, info_to_string(info), port_name);
   return MPI_ERR_INTERN;
#endif // __IMPI_Unpublish_name_FORWARD
}
#endif // __IMPI_Unpublish_name


#ifndef __IMPI_Waitall
int IMPI_Waitall ( int count, MPI_Request array_of_requests[], MPI_Status array_of_statuses[] )
{
#if __IMPI_Waitall_FORWARD >= 1
   return PMPI_Waitall(count, array_of_requests, array_of_statuses);
#else
   FATAL("NOT IMPLEMENTED: MPI_Waitall (int count=%d, MPI_Request array_of_requests[]=%p, MPI_Status array_of_statuses[]=%p)", count, array_of_requests, array_of_statuses);
   return MPI_ERR_INTERN;
#endif // __IMPI_Waitall_FORWARD
}
#endif // __IMPI_Waitall


#ifndef __IMPI_Waitany
int IMPI_Waitany ( int count, MPI_Request array_of_requests[], int *index, MPI_Status *status )
{
#if __IMPI_Waitany_FORWARD >= 1
   return PMPI_Waitany(count, array_of_requests, index, status);
#else
   FATAL("NOT IMPLEMENTED: MPI_Waitany (int count=%d, MPI_Request array_of_requests[]=%p, int *index=%p, MPI_Status *status=%p)", count, array_of_requests, index, status);
   return MPI_ERR_INTERN;
#endif // __IMPI_Waitany_FORWARD
}
#endif // __IMPI_Waitany


#ifndef __IMPI_Wait
int IMPI_Wait ( MPI_Request *request, MPI_Status *status )
{
#if __IMPI_Wait_FORWARD >= 1
   return PMPI_Wait(request, status);
#else
   FATAL("NOT IMPLEMENTED: MPI_Wait (MPI_Request *request=%p, MPI_Status *status=%p)", request, status);
   return MPI_ERR_INTERN;
#endif // __IMPI_Wait_FORWARD
}
#endif // __IMPI_Wait


#ifndef __IMPI_Waitsome
int IMPI_Waitsome ( int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[] )
{
#if __IMPI_Waitsome_FORWARD >= 1
   return PMPI_Waitsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
#else
   FATAL("NOT IMPLEMENTED: MPI_Waitsome (int incount=%d, MPI_Request array_of_requests[]=%p, int *outcount=%p, int array_of_indices[]=%p, MPI_Status array_of_statuses[]=%p)", incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
   return MPI_ERR_INTERN;
#endif // __IMPI_Waitsome_FORWARD
}
#endif // __IMPI_Waitsome


#ifndef __IMPI_Win_call_errhandler
int IMPI_Win_call_errhandler ( MPI_Win win, int errorcode )
{
#if __IMPI_Win_call_errhandler_FORWARD >= 1
   return PMPI_Win_call_errhandler(win, errorcode);
#else
   FATAL("NOT IMPLEMENTED: MPI_Win_call_errhandler (MPI_Win win=%s, int errorcode=%d)", win_to_string(win), errorcode);
   return MPI_ERR_INTERN;
#endif // __IMPI_Win_call_errhandler_FORWARD
}
#endif // __IMPI_Win_call_errhandler


#ifndef __IMPI_Win_complete
int IMPI_Win_complete ( MPI_Win win )
{
#if __IMPI_Win_complete_FORWARD >= 1
   return PMPI_Win_complete(win);
#else
   FATAL("NOT IMPLEMENTED: MPI_Win_complete (MPI_Win win=%s)", win_to_string(win));
   return MPI_ERR_INTERN;
#endif // __IMPI_Win_complete_FORWARD
}
#endif // __IMPI_Win_complete


#ifndef __IMPI_Win_create_errhandler
int IMPI_Win_create_errhandler ( MPI_Win_errhandler_fn *function, MPI_Errhandler *errhandler )
{
#if __IMPI_Win_create_errhandler_FORWARD >= 1
   return PMPI_Win_create_errhandler(function, errhandler);
#else
   FATAL("NOT IMPLEMENTED: MPI_Win_create_errhandler (MPI_Win_errhandler_fn *function=%p, MPI_Errhandler *errhandler=%p)", function, errhandler);
   return MPI_ERR_INTERN;
#endif // __IMPI_Win_create_errhandler_FORWARD
}
#endif // __IMPI_Win_create_errhandler


#ifndef __IMPI_Win_create
int IMPI_Win_create ( void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win )
{
#if __IMPI_Win_create_FORWARD >= 1
   return PMPI_Win_create(base, size, disp_unit, info, comm, win);
#else
   FATAL("NOT IMPLEMENTED: MPI_Win_create (void *base=%p, MPI_Aint size=%p, int disp_unit=%d, MPI_Info info=%s, MPI_Comm comm=%s, MPI_Win *win=%p)", base, (void *) size, disp_unit, info_to_string(info), comm_to_string(comm), win);
   return MPI_ERR_INTERN;
#endif // __IMPI_Win_create_FORWARD
}
#endif // __IMPI_Win_create


#ifndef __IMPI_Win_create_keyval
int IMPI_Win_create_keyval ( MPI_Win_copy_attr_function *win_copy_attr_fn, MPI_Win_delete_attr_function *win_delete_attr_fn, int *win_keyval, void *extra_state )
{
#if __IMPI_Win_create_keyval_FORWARD >= 1
   return PMPI_Win_create_keyval(win_copy_attr_fn, win_delete_attr_fn, win_keyval, extra_state);
#else
   FATAL("NOT IMPLEMENTED: MPI_Win_create_keyval (MPI_Win_copy_attr_function *win_copy_attr_fn=%p, MPI_Win_delete_attr_function *win_delete_attr_fn=%p, int *win_keyval=%p, void *extra_state=%p)", win_copy_attr_fn, win_delete_attr_fn, win_keyval, extra_state);
   return MPI_ERR_INTERN;
#endif // __IMPI_Win_create_keyval_FORWARD
}
#endif // __IMPI_Win_create_keyval


#ifndef __IMPI_Win_delete_attr
int IMPI_Win_delete_attr ( MPI_Win win, int win_keyval )
{
#if __IMPI_Win_delete_attr_FORWARD >= 1
   return PMPI_Win_delete_attr(win, win_keyval);
#else
   FATAL("NOT IMPLEMENTED: MPI_Win_delete_attr (MPI_Win win=%s, int win_keyval=%d)", win_to_string(win), win_keyval);
   return MPI_ERR_INTERN;
#endif // __IMPI_Win_delete_attr_FORWARD
}
#endif // __IMPI_Win_delete_attr


#ifndef __IMPI_Win_fence
int IMPI_Win_fence ( int assert, MPI_Win win )
{
#if __IMPI_Win_fence_FORWARD >= 1
   return PMPI_Win_fence(assert, win);
#else
   FATAL("NOT IMPLEMENTED: MPI_Win_fence (int assert=%d, MPI_Win win=%s)", assert, win_to_string(win));
   return MPI_ERR_INTERN;
#endif // __IMPI_Win_fence_FORWARD
}
#endif // __IMPI_Win_fence


#ifndef __IMPI_Win_free
int IMPI_Win_free ( MPI_Win *win )
{
#if __IMPI_Win_free_FORWARD >= 1
   return PMPI_Win_free(win);
#else
   FATAL("NOT IMPLEMENTED: MPI_Win_free (MPI_Win *win=%p)", win);
   return MPI_ERR_INTERN;
#endif // __IMPI_Win_free_FORWARD
}
#endif // __IMPI_Win_free


#ifndef __IMPI_Win_free_keyval
int IMPI_Win_free_keyval ( int *win_keyval )
{
#if __IMPI_Win_free_keyval_FORWARD >= 1
   return PMPI_Win_free_keyval(win_keyval);
#else
   FATAL("NOT IMPLEMENTED: MPI_Win_free_keyval (int *win_keyval=%p)", win_keyval);
   return MPI_ERR_INTERN;
#endif // __IMPI_Win_free_keyval_FORWARD
}
#endif // __IMPI_Win_free_keyval


#ifndef __IMPI_Win_get_attr
int IMPI_Win_get_attr ( MPI_Win win, int win_keyval, void *attribute_val, int *flag )
{
#if __IMPI_Win_get_attr_FORWARD >= 1
   return PMPI_Win_get_attr(win, win_keyval, attribute_val, flag);
#else
   FATAL("NOT IMPLEMENTED: MPI_Win_get_attr (MPI_Win win=%s, int win_keyval=%d, void *attribute_val=%p, int *flag=%p)", win_to_string(win), win_keyval, attribute_val, flag);
   return MPI_ERR_INTERN;
#endif // __IMPI_Win_get_attr_FORWARD
}
#endif // __IMPI_Win_get_attr


#ifndef __IMPI_Win_get_errhandler
int IMPI_Win_get_errhandler ( MPI_Win win, MPI_Errhandler *errhandler )
{
#if __IMPI_Win_get_errhandler_FORWARD >= 1
   return PMPI_Win_get_errhandler(win, errhandler);
#else
   FATAL("NOT IMPLEMENTED: MPI_Win_get_errhandler (MPI_Win win=%s, MPI_Errhandler *errhandler=%p)", win_to_string(win), errhandler);
   return MPI_ERR_INTERN;
#endif // __IMPI_Win_get_errhandler_FORWARD
}
#endif // __IMPI_Win_get_errhandler


#ifndef __IMPI_Win_get_group
int IMPI_Win_get_group ( MPI_Win win, MPI_Group *group )
{
#if __IMPI_Win_get_group_FORWARD >= 1
   return PMPI_Win_get_group(win, group);
#else
   FATAL("NOT IMPLEMENTED: MPI_Win_get_group (MPI_Win win=%s, MPI_Group *group=%p)", win_to_string(win), group);
   return MPI_ERR_INTERN;
#endif // __IMPI_Win_get_group_FORWARD
}
#endif // __IMPI_Win_get_group


#ifndef __IMPI_Win_get_name
int IMPI_Win_get_name ( MPI_Win win, char *win_name, int *resultlen )
{
#if __IMPI_Win_get_name_FORWARD >= 1
   return PMPI_Win_get_name(win, win_name, resultlen);
#else
   FATAL("NOT IMPLEMENTED: MPI_Win_get_name (MPI_Win win=%s, char *win_name=%p, int *resultlen=%p)", win_to_string(win), win_name, resultlen);
   return MPI_ERR_INTERN;
#endif // __IMPI_Win_get_name_FORWARD
}
#endif // __IMPI_Win_get_name


#ifndef __IMPI_Win_lock
int IMPI_Win_lock ( int lock_type, int rank, int assert, MPI_Win win )
{
#if __IMPI_Win_lock_FORWARD >= 1
   return PMPI_Win_lock(lock_type, rank, assert, win);
#else
   FATAL("NOT IMPLEMENTED: MPI_Win_lock (int lock_type=%d, int rank=%d, int assert=%d, MPI_Win win=%s)", lock_type, rank, assert, win_to_string(win));
   return MPI_ERR_INTERN;
#endif // __IMPI_Win_lock_FORWARD
}
#endif // __IMPI_Win_lock


#ifndef __IMPI_Win_post
int IMPI_Win_post ( MPI_Group group, int assert, MPI_Win win )
{
#if __IMPI_Win_post_FORWARD >= 1
   return PMPI_Win_post(group, assert, win);
#else
   FATAL("NOT IMPLEMENTED: MPI_Win_post (MPI_Group group=%s, int assert=%d, MPI_Win win=%s)", group_to_string(group), assert, win_to_string(win));
   return MPI_ERR_INTERN;
#endif // __IMPI_Win_post_FORWARD
}
#endif // __IMPI_Win_post


#ifndef __IMPI_Win_set_attr
int IMPI_Win_set_attr ( MPI_Win win, int win_keyval, void *attribute_val )
{
#if __IMPI_Win_set_attr_FORWARD >= 1
   return PMPI_Win_set_attr(win, win_keyval, attribute_val);
#else
   FATAL("NOT IMPLEMENTED: MPI_Win_set_attr (MPI_Win win=%s, int win_keyval=%d, void *attribute_val=%p)", win_to_string(win), win_keyval, attribute_val);
   return MPI_ERR_INTERN;
#endif // __IMPI_Win_set_attr_FORWARD
}
#endif // __IMPI_Win_set_attr


#ifndef __IMPI_Win_set_errhandler
int IMPI_Win_set_errhandler ( MPI_Win win, MPI_Errhandler errhandler )
{
#if __IMPI_Win_set_errhandler_FORWARD >= 1
   return PMPI_Win_set_errhandler(win, errhandler);
#else
   FATAL("NOT IMPLEMENTED: MPI_Win_set_errhandler (MPI_Win win=%s, MPI_Errhandler errhandler=%p)", win_to_string(win), (void *) errhandler);
   return MPI_ERR_INTERN;
#endif // __IMPI_Win_set_errhandler_FORWARD
}
#endif // __IMPI_Win_set_errhandler


#ifndef __IMPI_Win_set_name
int IMPI_Win_set_name ( MPI_Win win, char *win_name )
{
#if __IMPI_Win_set_name_FORWARD >= 1
   return PMPI_Win_set_name(win, win_name);
#else
   FATAL("NOT IMPLEMENTED: MPI_Win_set_name (MPI_Win win=%s, char *win_name=%p)", win_to_string(win), win_name);
   return MPI_ERR_INTERN;
#endif // __IMPI_Win_set_name_FORWARD
}
#endif // __IMPI_Win_set_name


#ifndef __IMPI_Win_start
int IMPI_Win_start ( MPI_Group group, int assert, MPI_Win win )
{
#if __IMPI_Win_start_FORWARD >= 1
   return PMPI_Win_start(group, assert, win);
#else
   FATAL("NOT IMPLEMENTED: MPI_Win_start (MPI_Group group=%s, int assert=%d, MPI_Win win=%s)", group_to_string(group), assert, win_to_string(win));
   return MPI_ERR_INTERN;
#endif // __IMPI_Win_start_FORWARD
}
#endif // __IMPI_Win_start


#ifndef __IMPI_Win_test
int IMPI_Win_test ( MPI_Win win, int *flag )
{
#if __IMPI_Win_test_FORWARD >= 1
   return PMPI_Win_test(win, flag);
#else
   FATAL("NOT IMPLEMENTED: MPI_Win_test (MPI_Win win=%s, int *flag=%p)", win_to_string(win), flag);
   return MPI_ERR_INTERN;
#endif // __IMPI_Win_test_FORWARD
}
#endif // __IMPI_Win_test


#ifndef __IMPI_Win_unlock
int IMPI_Win_unlock ( int rank, MPI_Win win )
{
#if __IMPI_Win_unlock_FORWARD >= 1
   return PMPI_Win_unlock(rank, win);
#else
   FATAL("NOT IMPLEMENTED: MPI_Win_unlock (int rank=%d, MPI_Win win=%s)", rank, win_to_string(win));
   return MPI_ERR_INTERN;
#endif // __IMPI_Win_unlock_FORWARD
}
#endif // __IMPI_Win_unlock


#ifndef __IMPI_Win_wait
int IMPI_Win_wait ( MPI_Win win )
{
#if __IMPI_Win_wait_FORWARD >= 1
   return PMPI_Win_wait(win);
#else
   FATAL("NOT IMPLEMENTED: MPI_Win_wait (MPI_Win win=%s)", win_to_string(win));
   return MPI_ERR_INTERN;
#endif // __IMPI_Win_wait_FORWARD
}
#endif // __IMPI_Win_wait



#ifndef __IMPI_Group_comm_create
int IMPI_Group_comm_create ( MPI_Comm old_comm, MPI_Group group, int tag, MPI_Comm *new_comm )
{
#if __IMPI_Group_comm_create_FORWARD >= 1
   return PMPI_Group_comm_create(old_comm, group, tag, new_comm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Group_comm_create (MPI_Comm old_comm=%s, MPI_Group group=%s, int tag=%d, MPI_Comm *new_comm=%p)", comm_to_string(old_comm), group_to_string(group), tag, new_comm);
   return MPI_ERR_INTERN;
#endif // __IMPI_Group_comm_create_FORWARD
}
#endif // __IMPI_Group_comm_create


#ifndef __IMPI_Comm_c2f
MPI_Fint IMPI_Comm_c2f ( MPI_Comm comm )
{
#if __IMPI_Comm_c2f_FORWARD >= 1
   return PMPI_Comm_c2f(comm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Comm_c2f (MPI_Comm comm=%s)", comm_to_string(comm));
   return MPI_ERR_INTERN;
#endif // __IMPI_Comm_c2f_FORWARD
}
#endif // __IMPI_Comm_c2f


#ifndef __IMPI_Group_c2f
MPI_Fint IMPI_Group_c2f ( MPI_Group g )
{
#if __IMPI_Group_c2f_FORWARD >= 1
   return PMPI_Group_c2f(g);
#else
   FATAL("NOT IMPLEMENTED: MPI_Group_c2f (MPI_Group g=%s)", group_to_string(g));
   return MPI_ERR_INTERN;
#endif // __IMPI_Group_c2f_FORWARD
}
#endif // __IMPI_Group_c2f


#ifndef __IMPI_Request_c2f
MPI_Fint IMPI_Request_c2f ( MPI_Request request )
{
#if __IMPI_Request_c2f_FORWARD >= 1
   return PMPI_Request_c2f(request);
#else
   FATAL("NOT IMPLEMENTED: MPI_Request_c2f (MPI_Request request=%s)", request_to_string(request));
   return MPI_ERR_INTERN;
#endif // __IMPI_Request_c2f_FORWARD
}
#endif // __IMPI_Request_c2f


#ifndef __IMPI_Info_c2f
MPI_Fint IMPI_Info_c2f ( MPI_Info info )
{
#if __IMPI_Info_c2f_FORWARD >= 1
   return PMPI_Info_c2f(info);
#else
   FATAL("NOT IMPLEMENTED: MPI_Info_c2f (MPI_Info info=%s)", info_to_string(info));
   return MPI_ERR_INTERN;
#endif // __IMPI_Info_c2f_FORWARD
}
#endif // __IMPI_Info_c2f


#ifndef __IMPI_File_c2f
MPI_Fint IMPI_File_c2f ( MPI_File file )
{
#if __IMPI_File_c2f_FORWARD >= 1
   return PMPI_File_c2f(file);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_c2f (MPI_File file=%s)", file_to_string(file));
   return MPI_ERR_INTERN;
#endif // __IMPI_File_c2f_FORWARD
}
#endif // __IMPI_File_c2f


#ifndef __IMPI_Op_c2f
MPI_Fint IMPI_Op_c2f ( MPI_Op op )
{
#if __IMPI_Op_c2f_FORWARD >= 1
   return PMPI_Op_c2f(op);
#else
   FATAL("NOT IMPLEMENTED: MPI_Op_c2f (MPI_Op op=%s)", op_to_string(op));
   return MPI_ERR_INTERN;
#endif // __IMPI_Op_c2f_FORWARD
}
#endif // __IMPI_Op_c2f


#ifndef __IMPI_Win_c2f
MPI_Fint IMPI_Win_c2f ( MPI_Win Win )
{
#if __IMPI_Win_c2f_FORWARD >= 1
   return PMPI_Win_c2f(Win);
#else
   FATAL("NOT IMPLEMENTED: MPI_Win_c2f (MPI_Win Win=%s)", win_to_string(Win));
   return MPI_ERR_INTERN;
#endif // __IMPI_Win_c2f_FORWARD
}
#endif // __IMPI_Win_c2f


#ifndef __IMPI_Errhandler_c2f
MPI_Fint IMPI_Errhandler_c2f ( MPI_Errhandler Errhandler )
{
#if __IMPI_Errhandler_c2f_FORWARD >= 1
   return PMPI_Errhandler_c2f(Errhandler);
#else
   FATAL("NOT IMPLEMENTED: MPI_Errhandler_c2f (MPI_Errhandler Errhandler=%p)", (void *) Errhandler);
   return MPI_ERR_INTERN;
#endif // __IMPI_Errhandler_c2f_FORWARD
}
#endif // __IMPI_Errhandler_c2f


#ifndef __IMPI_Type_c2f
MPI_Fint IMPI_Type_c2f ( MPI_Datatype Type )
{
#if __IMPI_Type_c2f_FORWARD >= 1
   return PMPI_Type_c2f(Type);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_c2f (MPI_Datatype Type=%s)", type_to_string(Type));
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_c2f_FORWARD
}
#endif // __IMPI_Type_c2f


#ifndef __IMPI_Comm_f2c
MPI_Comm IMPI_Comm_f2c ( MPI_Fint comm )
{
#if __IMPI_Comm_f2c_FORWARD >= 1
   return PMPI_Comm_f2c(comm);
#else
   FATAL("NOT IMPLEMENTED: MPI_Comm_f2c (MPI_Fint comm=%d)", comm);
   return MPI_ERR_INTERN;
#endif // __IMPI_Comm_f2c_FORWARD
}
#endif // __IMPI_Comm_f2c


#ifndef __IMPI_Group_f2c
MPI_Group IMPI_Group_f2c ( MPI_Fint g )
{
#if __IMPI_Group_f2c_FORWARD >= 1
   return PMPI_Group_f2c(g);
#else
   FATAL("NOT IMPLEMENTED: MPI_Group_f2c (MPI_Fint g=%d)", g);
   return MPI_ERR_INTERN;
#endif // __IMPI_Group_f2c_FORWARD
}
#endif // __IMPI_Group_f2c


#ifndef __IMPI_Request_f2c
MPI_Request IMPI_Request_f2c ( MPI_Fint request )
{
#if __IMPI_Request_f2c_FORWARD >= 1
   return PMPI_Request_f2c(request);
#else
   FATAL("NOT IMPLEMENTED: MPI_Request_f2c (MPI_Fint request=%d)", request);
   return MPI_ERR_INTERN;
#endif // __IMPI_Request_f2c_FORWARD
}
#endif // __IMPI_Request_f2c


#ifndef __IMPI_Info_f2c
MPI_Info IMPI_Info_f2c ( MPI_Fint info )
{
#if __IMPI_Info_f2c_FORWARD >= 1
   return PMPI_Info_f2c(info);
#else
   FATAL("NOT IMPLEMENTED: MPI_Info_f2c (MPI_Fint info=%d)", info);
   return MPI_ERR_INTERN;
#endif // __IMPI_Info_f2c_FORWARD
}
#endif // __IMPI_Info_f2c


#ifndef __IMPI_File_f2c
MPI_File IMPI_File_f2c ( MPI_Fint file )
{
#if __IMPI_File_f2c_FORWARD >= 1
   return PMPI_File_f2c(file);
#else
   FATAL("NOT IMPLEMENTED: MPI_File_f2c (MPI_Fint file=%d)", file);
   return MPI_ERR_INTERN;
#endif // __IMPI_File_f2c_FORWARD
}
#endif // __IMPI_File_f2c


#ifndef __IMPI_Op_f2c
MPI_Op IMPI_Op_f2c ( MPI_Fint op )
{
#if __IMPI_Op_f2c_FORWARD >= 1
   return PMPI_Op_f2c(op);
#else
   FATAL("NOT IMPLEMENTED: MPI_Op_f2c (MPI_Fint op=%d)", op);
   return MPI_ERR_INTERN;
#endif // __IMPI_Op_f2c_FORWARD
}
#endif // __IMPI_Op_f2c


#ifndef __IMPI_Win_f2c
MPI_Win IMPI_Win_f2c ( MPI_Fint Win )
{
#if __IMPI_Win_f2c_FORWARD >= 1
   return PMPI_Win_f2c(Win);
#else
   FATAL("NOT IMPLEMENTED: MPI_Win_f2c (MPI_Fint Win=%d)", Win);
   return MPI_ERR_INTERN;
#endif // __IMPI_Win_f2c_FORWARD
}
#endif // __IMPI_Win_f2c


#ifndef __IMPI_Errhandler_f2c
MPI_Errhandler IMPI_Errhandler_f2c ( MPI_Fint Errhandler )
{
#if __IMPI_Errhandler_f2c_FORWARD >= 1
   return PMPI_Errhandler_f2c(Errhandler);
#else
   FATAL("NOT IMPLEMENTED: MPI_Errhandler_f2c (MPI_Fint Errhandler=%d)", Errhandler);
   return MPI_ERR_INTERN;
#endif // __IMPI_Errhandler_f2c_FORWARD
}
#endif // __IMPI_Errhandler_f2c


#ifndef __IMPI_Type_f2c
MPI_Datatype IMPI_Type_f2c ( MPI_Fint Type )
{
#if __IMPI_Type_f2c_FORWARD >= 1
   return PMPI_Type_f2c(Type);
#else
   FATAL("NOT IMPLEMENTED: MPI_Type_f2c (MPI_Fint Type=%d)", Type);
   return MPI_ERR_INTERN;
#endif // __IMPI_Type_f2c_FORWARD
}
#endif // __IMPI_Type_f2c


