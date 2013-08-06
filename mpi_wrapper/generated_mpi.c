#include "flags.h"

#ifdef ENABLE_INTERCEPT

#include "profiling.h"
#include "logging.h"
#include "debugging.h"
#include "mpi.h"
#include "generated_header.h"

#ifdef IBIS_INTERCEPT
#include "request.h"
#endif

int MPI_Init ( int *argc, char ***argv )
{
   init_debug();

#ifdef TRACE_CALLS
   INFO(0, "MPI_Init(int *argc=%p, char ***argv=%p)", argc, argv);
#endif // TRACE_CALLS

#ifdef IBIS_INTERCEPT
   int error = IMPI_Init(argc, argv);
#else
   int error = PMPI_Init(argc, argv);
#endif // IBIS_INTERCEPT

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Init failed (%d)!", error);
   }
#endif // TRACE_ERRORS

   profile_init();

   return error;
}


int MPI_Type_create_f90_complex ( int p, int r, MPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_create_f90_complex(int p=%d, int r=%d, MPI_Datatype *newtype=%p)", p, r, newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_create_f90_complex(p, r, newtype);
#else
   int error = PMPI_Type_create_f90_complex(p, r, newtype);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_create_f90_complex failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_create_f90_integer ( int r, MPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_create_f90_integer(int r=%d, MPI_Datatype *newtype=%p)", r, newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_create_f90_integer(r, newtype);
#else
   int error = PMPI_Type_create_f90_integer(r, newtype);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_create_f90_integer failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_create_f90_real ( int p, int r, MPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_create_f90_real(int p=%d, int r=%d, MPI_Datatype *newtype=%p)", p, r, newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_create_f90_real(p, r, newtype);
#else
   int error = PMPI_Type_create_f90_real(p, r, newtype);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_create_f90_real failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Abort ( MPI_Comm comm, int errorcode )
{
#ifdef TRACE_CALLS
   INFO(0, "MPI_Abort(MPI_Comm comm=%s, int errorcode=%d)", comm_to_string(comm), errorcode);
#endif // TRACE_CALLS

#ifdef IBIS_INTERCEPT
   int error = IMPI_Abort(comm, errorcode);
#else
   int error = PMPI_Abort(comm, errorcode);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_finalize();
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Abort failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Accumulate ( void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Op op, MPI_Win win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Accumulate(void *origin_addr=%p, int origin_count=%d, MPI_Datatype origin_datatype=%s, int target_rank=%d, MPI_Aint target_disp=%p, int target_count=%d, MPI_Datatype target_datatype=%s, MPI_Op op=%s, MPI_Win win=%s)", origin_addr, origin_count, type_to_string(origin_datatype), target_rank, (void *) target_disp, target_count, type_to_string(target_datatype), op_to_string(op), win_to_string(win));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(origin_datatype);
   CHECK_TYPE(target_datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Accumulate(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, op, win);
#else
   int error = PMPI_Accumulate(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, op, win);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Accumulate failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Add_error_class ( int *errorclass )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Add_error_class(int *errorclass=%p)", errorclass);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Add_error_class(errorclass);
#else
   int error = PMPI_Add_error_class(errorclass);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Add_error_class failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Add_error_code ( int errorclass, int *errorcode )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Add_error_code(int errorclass=%d, int *errorcode=%p)", errorclass, errorcode);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Add_error_code(errorclass, errorcode);
#else
   int error = PMPI_Add_error_code(errorclass, errorcode);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Add_error_code failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Add_error_string ( int errorcode, char *string )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Add_error_string(int errorcode=%d, char *string=%p)", errorcode, string);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Add_error_string(errorcode, string);
#else
   int error = PMPI_Add_error_string(errorcode, string);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Add_error_string failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Address ( void *location, MPI_Aint *address )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Address(void *location=%p, MPI_Aint *address=%p)", location, address);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Address(location, address);
#else
   int error = PMPI_Address(location, address);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Address failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Allgather ( void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Allgather(void *sendbuf=%p, int sendcount=%d, MPI_Datatype sendtype=%s, void *recvbuf=%p, int recvcount=%d, MPI_Datatype recvtype=%s, MPI_Comm comm=%s)", sendbuf, sendcount, type_to_string(sendtype), recvbuf, recvcount, type_to_string(recvtype), comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(sendtype);
   CHECK_TYPE(recvtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Allgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
#else
   int error = PMPI_Allgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_ALLGATHER, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Allgather failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Allgatherv ( void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int *recvcounts, int *displs, MPI_Datatype recvtype, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Allgatherv(void *sendbuf=%p, int sendcount=%d, MPI_Datatype sendtype=%s, void *recvbuf=%p, int *recvcounts=%p, int *displs=%p, MPI_Datatype recvtype=%s, MPI_Comm comm=%s)", sendbuf, sendcount, type_to_string(sendtype), recvbuf, recvcounts, displs, type_to_string(recvtype), comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(sendtype);
   CHECK_TYPE(recvtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Allgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);
#else
   int error = PMPI_Allgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_ALLGATHER, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Allgatherv failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Alloc_mem ( MPI_Aint size, MPI_Info info, void *baseptr )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Alloc_mem(MPI_Aint size=%p, MPI_Info info=%s, void *baseptr=%p)", (void *) size, info_to_string(info), baseptr);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Alloc_mem(size, info, baseptr);
#else
   int error = PMPI_Alloc_mem(size, info, baseptr);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Alloc_mem failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Allreduce ( void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Allreduce(void *sendbuf=%p, void *recvbuf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Op op=%s, MPI_Comm comm=%s)", sendbuf, recvbuf, count, type_to_string(datatype), op_to_string(op), comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);
#else
   int error = PMPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_ALLREDUCE, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Allreduce failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Alltoall ( void *sendbuf, int sendcount, MPI_Datatype sendtype, void *recvbuf, int recvcount, MPI_Datatype recvtype, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Alltoall(void *sendbuf=%p, int sendcount=%d, MPI_Datatype sendtype=%s, void *recvbuf=%p, int recvcount=%d, MPI_Datatype recvtype=%s, MPI_Comm comm=%s)", sendbuf, sendcount, type_to_string(sendtype), recvbuf, recvcount, type_to_string(recvtype), comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(sendtype);
   CHECK_TYPE(recvtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Alltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
#else
   int error = PMPI_Alltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_ALLTOALL, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Alltoall failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Alltoallv ( void *sendbuf, int *sendcnts, int *sdispls, MPI_Datatype sendtype, void *recvbuf, int *recvcnts, int *rdispls, MPI_Datatype recvtype, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Alltoallv(void *sendbuf=%p, int *sendcnts=%p, int *sdispls=%p, MPI_Datatype sendtype=%s, void *recvbuf=%p, int *recvcnts=%p, int *rdispls=%p, MPI_Datatype recvtype=%s, MPI_Comm comm=%s)", sendbuf, sendcnts, sdispls, type_to_string(sendtype), recvbuf, recvcnts, rdispls, type_to_string(recvtype), comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(sendtype);
   CHECK_TYPE(recvtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Alltoallv(sendbuf, sendcnts, sdispls, sendtype, recvbuf, recvcnts, rdispls, recvtype, comm);
#else
   int error = PMPI_Alltoallv(sendbuf, sendcnts, sdispls, sendtype, recvbuf, recvcnts, rdispls, recvtype, comm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_ALLTOALL, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Alltoallv failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Alltoallw ( void *sendbuf, int *sendcnts, int *sdispls, MPI_Datatype *sendtypes, void *recvbuf, int *recvcnts, int *rdispls, MPI_Datatype *recvtypes, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Alltoallw(void *sendbuf=%p, int *sendcnts=%p, int *sdispls=%p, MPI_Datatype *sendtypes=%p, void *recvbuf=%p, int *recvcnts=%p, int *rdispls=%p, MPI_Datatype *recvtypes=%p, MPI_Comm comm=%s)", sendbuf, sendcnts, sdispls, sendtypes, recvbuf, recvcnts, rdispls, recvtypes, comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(*sendtypes);
   CHECK_TYPE(*recvtypes);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Alltoallw(sendbuf, sendcnts, sdispls, sendtypes, recvbuf, recvcnts, rdispls, recvtypes, comm);
#else
   int error = PMPI_Alltoallw(sendbuf, sendcnts, sdispls, sendtypes, recvbuf, recvcnts, rdispls, recvtypes, comm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_ALLTOALL, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Alltoallw failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Attr_delete ( MPI_Comm comm, int keyval )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Attr_delete(MPI_Comm comm=%s, int keyval=%d)", comm_to_string(comm), keyval);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Attr_delete(comm, keyval);
#else
   int error = PMPI_Attr_delete(comm, keyval);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Attr_delete failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Attr_get ( MPI_Comm comm, int keyval, void *attr_value, int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Attr_get(MPI_Comm comm=%s, int keyval=%d, void *attr_value=%p, int *flag=%p)", comm_to_string(comm), keyval, attr_value, flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Attr_get(comm, keyval, attr_value, flag);
#else
   int error = PMPI_Attr_get(comm, keyval, attr_value, flag);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Attr_get failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Attr_put ( MPI_Comm comm, int keyval, void *attr_value )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Attr_put(MPI_Comm comm=%s, int keyval=%d, void *attr_value=%p)", comm_to_string(comm), keyval, attr_value);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Attr_put(comm, keyval, attr_value);
#else
   int error = PMPI_Attr_put(comm, keyval, attr_value);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Attr_put failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Barrier ( MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Barrier(MPI_Comm comm=%s)", comm_to_string(comm));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Barrier(comm);
#else
   int error = PMPI_Barrier(comm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_BARRIER, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Barrier failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Bcast ( void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Bcast(void *buffer=%p, int count=%d, MPI_Datatype datatype=%s, int root=%d, MPI_Comm comm=%s)", buffer, count, type_to_string(datatype), root, comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Bcast(buffer, count, datatype, root, comm);
#else
   int error = PMPI_Bcast(buffer, count, datatype, root, comm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_BCAST, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Bcast failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Bsend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Bsend(void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Bsend(buf, count, datatype, dest, tag, comm);
#else
   int error = PMPI_Bsend(buf, count, datatype, dest, tag, comm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_BSEND, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Bsend failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Bsend_init ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Bsend_init(void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s, MPI_Request *r=%p)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Bsend_init(buf, count, datatype, dest, tag, comm, r);
#else
   int error = PMPI_Bsend_init(buf, count, datatype, dest, tag, comm, r);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Bsend_init failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Buffer_attach ( void *buffer, int size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Buffer_attach(void *buffer=%p, int size=%d)", buffer, size);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Buffer_attach(buffer, size);
#else
   int error = PMPI_Buffer_attach(buffer, size);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Buffer_attach failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Buffer_detach ( void *buffer, int *size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Buffer_detach(void *buffer=%p, int *size=%p)", buffer, size);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Buffer_detach(buffer, size);
#else
   int error = PMPI_Buffer_detach(buffer, size);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Buffer_detach failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Cancel ( MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Cancel(MPI_Request *r=%p)", r);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Cancel(r);
#else
   int error = PMPI_Cancel(r);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Cancel failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Cart_coords ( MPI_Comm comm, int rank, int maxdims, int *coords )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Cart_coords(MPI_Comm comm=%s, int rank=%d, int maxdims=%d, int *coords=%p)", comm_to_string(comm), rank, maxdims, coords);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Cart_coords(comm, rank, maxdims, coords);
#else
   int error = PMPI_Cart_coords(comm, rank, maxdims, coords);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Cart_coords failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Cart_create ( MPI_Comm comm_old, int ndims, int *dims, int *periods, int reorder, MPI_Comm *comm_cart )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Cart_create(MPI_Comm comm_old=%s, int ndims=%d, int *dims=%p, int *periods=%p, int reorder=%d, MPI_Comm *comm_cart=%p)", comm_to_string(comm_old), ndims, dims, periods, reorder, comm_cart);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Cart_create(comm_old, ndims, dims, periods, reorder, comm_cart);
#else
   int error = PMPI_Cart_create(comm_old, ndims, dims, periods, reorder, comm_cart);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm_old, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Cart_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Cartdim_get ( MPI_Comm comm, int *ndims )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Cartdim_get(MPI_Comm comm=%s, int *ndims=%p)", comm_to_string(comm), ndims);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Cartdim_get(comm, ndims);
#else
   int error = PMPI_Cartdim_get(comm, ndims);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Cartdim_get failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Cart_get ( MPI_Comm comm, int maxdims, int *dims, int *periods, int *coords )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Cart_get(MPI_Comm comm=%s, int maxdims=%d, int *dims=%p, int *periods=%p, int *coords=%p)", comm_to_string(comm), maxdims, dims, periods, coords);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Cart_get(comm, maxdims, dims, periods, coords);
#else
   int error = PMPI_Cart_get(comm, maxdims, dims, periods, coords);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Cart_get failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Cart_map ( MPI_Comm comm_old, int ndims, int *dims, int *periods, int *newrank )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Cart_map(MPI_Comm comm_old=%s, int ndims=%d, int *dims=%p, int *periods=%p, int *newrank=%p)", comm_to_string(comm_old), ndims, dims, periods, newrank);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Cart_map(comm_old, ndims, dims, periods, newrank);
#else
   int error = PMPI_Cart_map(comm_old, ndims, dims, periods, newrank);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm_old, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Cart_map failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Cart_rank ( MPI_Comm comm, int *coords, int *rank )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Cart_rank(MPI_Comm comm=%s, int *coords=%p, int *rank=%p)", comm_to_string(comm), coords, rank);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Cart_rank(comm, coords, rank);
#else
   int error = PMPI_Cart_rank(comm, coords, rank);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Cart_rank failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Cart_shift ( MPI_Comm comm, int direction, int displ, int *source, int *dest )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Cart_shift(MPI_Comm comm=%s, int direction=%d, int displ=%d, int *source=%p, int *dest=%p)", comm_to_string(comm), direction, displ, source, dest);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Cart_shift(comm, direction, displ, source, dest);
#else
   int error = PMPI_Cart_shift(comm, direction, displ, source, dest);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Cart_shift failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Cart_sub ( MPI_Comm comm, int *remain_dims, MPI_Comm *comm_new )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Cart_sub(MPI_Comm comm=%s, int *remain_dims=%p, MPI_Comm *comm_new=%p)", comm_to_string(comm), remain_dims, comm_new);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Cart_sub(comm, remain_dims, comm_new);
#else
   int error = PMPI_Cart_sub(comm, remain_dims, comm_new);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Cart_sub failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Close_port ( char *port_name )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Close_port(char *port_name=%p)", port_name);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Close_port(port_name);
#else
   int error = PMPI_Close_port(port_name);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Close_port failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_accept ( char *port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *newcomm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_accept(char *port_name=%p, MPI_Info info=%s, int root=%d, MPI_Comm comm=%s, MPI_Comm *newcomm=%p)", port_name, info_to_string(info), root, comm_to_string(comm), newcomm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Comm_accept(port_name, info, root, comm, newcomm);
#else
   int error = PMPI_Comm_accept(port_name, info, root, comm, newcomm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_accept failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_call_errhandler ( MPI_Comm comm, int errorcode )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_call_errhandler(MPI_Comm comm=%s, int errorcode=%d)", comm_to_string(comm), errorcode);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Comm_call_errhandler(comm, errorcode);
#else
   int error = PMPI_Comm_call_errhandler(comm, errorcode);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_call_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_compare ( MPI_Comm comm1, MPI_Comm comm2, int *result )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_compare(MPI_Comm comm1=%s, MPI_Comm comm2=%s, int *result=%p)", comm_to_string(comm1), comm_to_string(comm2), result);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Comm_compare(comm1, comm2, result);
#else
   int error = PMPI_Comm_compare(comm1, comm2, result);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm1, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_compare failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_connect ( char *port_name, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *newcomm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_connect(char *port_name=%p, MPI_Info info=%s, int root=%d, MPI_Comm comm=%s, MPI_Comm *newcomm=%p)", port_name, info_to_string(info), root, comm_to_string(comm), newcomm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Comm_connect(port_name, info, root, comm, newcomm);
#else
   int error = PMPI_Comm_connect(port_name, info, root, comm, newcomm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_connect failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_create_errhandler ( MPI_Comm_errhandler_fn *function, MPI_Errhandler *errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_create_errhandler(MPI_Comm_errhandler_fn *function=%p, MPI_Errhandler *errhandler=%p)", function, errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Comm_create_errhandler(function, errhandler);
#else
   int error = PMPI_Comm_create_errhandler(function, errhandler);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_create_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_create ( MPI_Comm comm, MPI_Group g, MPI_Comm *newcomm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_create(MPI_Comm comm=%s, MPI_Group g=%s, MPI_Comm *newcomm=%p)", comm_to_string(comm), group_to_string(g), newcomm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Comm_create(comm, g, newcomm);
#else
   int error = PMPI_Comm_create(comm, g, newcomm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_create_keyval ( MPI_Comm_copy_attr_function *comm_copy_attr_fn, MPI_Comm_delete_attr_function *comm_delete_attr_fn, int *comm_keyval, void *extra_state )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_create_keyval(MPI_Comm_copy_attr_function *comm_copy_attr_fn=%p, MPI_Comm_delete_attr_function *comm_delete_attr_fn=%p, int *comm_keyval=%p, void *extra_state=%p)", comm_copy_attr_fn, comm_delete_attr_fn, comm_keyval, extra_state);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Comm_create_keyval(comm_copy_attr_fn, comm_delete_attr_fn, comm_keyval, extra_state);
#else
   int error = PMPI_Comm_create_keyval(comm_copy_attr_fn, comm_delete_attr_fn, comm_keyval, extra_state);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_create_keyval failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_delete_attr ( MPI_Comm comm, int comm_keyval )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_delete_attr(MPI_Comm comm=%s, int comm_keyval=%d)", comm_to_string(comm), comm_keyval);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Comm_delete_attr(comm, comm_keyval);
#else
   int error = PMPI_Comm_delete_attr(comm, comm_keyval);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_delete_attr failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_disconnect ( MPI_Comm *comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_disconnect(MPI_Comm *comm=%p)", comm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Comm_disconnect(comm);
#else
   int error = PMPI_Comm_disconnect(comm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_disconnect failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_dup ( MPI_Comm comm, MPI_Comm *newcomm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_dup(MPI_Comm comm=%s, MPI_Comm *newcomm=%p)", comm_to_string(comm), newcomm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Comm_dup(comm, newcomm);
#else
   int error = PMPI_Comm_dup(comm, newcomm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_dup failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_free ( MPI_Comm *comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_free(MPI_Comm *comm=%p)", comm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Comm_free(comm);
#else
   int error = PMPI_Comm_free(comm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_free failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_free_keyval ( int *comm_keyval )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_free_keyval(int *comm_keyval=%p)", comm_keyval);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Comm_free_keyval(comm_keyval);
#else
   int error = PMPI_Comm_free_keyval(comm_keyval);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_free_keyval failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_get_attr ( MPI_Comm comm, int comm_keyval, void *attribute_val, int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_get_attr(MPI_Comm comm=%s, int comm_keyval=%d, void *attribute_val=%p, int *flag=%p)", comm_to_string(comm), comm_keyval, attribute_val, flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Comm_get_attr(comm, comm_keyval, attribute_val, flag);
#else
   int error = PMPI_Comm_get_attr(comm, comm_keyval, attribute_val, flag);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_get_attr failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_get_errhandler ( MPI_Comm comm, MPI_Errhandler *errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_get_errhandler(MPI_Comm comm=%s, MPI_Errhandler *errhandler=%p)", comm_to_string(comm), errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Comm_get_errhandler(comm, errhandler);
#else
   int error = PMPI_Comm_get_errhandler(comm, errhandler);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_get_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_get_name ( MPI_Comm comm, char *comm_name, int *resultlen )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_get_name(MPI_Comm comm=%s, char *comm_name=%p, int *resultlen=%p)", comm_to_string(comm), comm_name, resultlen);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Comm_get_name(comm, comm_name, resultlen);
#else
   int error = PMPI_Comm_get_name(comm, comm_name, resultlen);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_get_name failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_get_parent ( MPI_Comm *parent )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_get_parent(MPI_Comm *parent=%p)", parent);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Comm_get_parent(parent);
#else
   int error = PMPI_Comm_get_parent(parent);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_get_parent failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_group ( MPI_Comm comm, MPI_Group *g )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_group(MPI_Comm comm=%s, MPI_Group *g=%p)", comm_to_string(comm), g);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Comm_group(comm, g);
#else
   int error = PMPI_Comm_group(comm, g);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_group failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_join ( int fd, MPI_Comm *intercomm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_join(int fd=%d, MPI_Comm *intercomm=%p)", fd, intercomm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Comm_join(fd, intercomm);
#else
   int error = PMPI_Comm_join(fd, intercomm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_join failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_rank ( MPI_Comm comm, int *rank )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_rank(MPI_Comm comm=%s, int *rank=%p)", comm_to_string(comm), rank);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Comm_rank(comm, rank);
#else
   int error = PMPI_Comm_rank(comm, rank);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_rank failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_remote_group ( MPI_Comm comm, MPI_Group *g )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_remote_group(MPI_Comm comm=%s, MPI_Group *g=%p)", comm_to_string(comm), g);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Comm_remote_group(comm, g);
#else
   int error = PMPI_Comm_remote_group(comm, g);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_remote_group failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_remote_size ( MPI_Comm comm, int *size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_remote_size(MPI_Comm comm=%s, int *size=%p)", comm_to_string(comm), size);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Comm_remote_size(comm, size);
#else
   int error = PMPI_Comm_remote_size(comm, size);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_remote_size failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_set_attr ( MPI_Comm comm, int comm_keyval, void *attribute_val )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_set_attr(MPI_Comm comm=%s, int comm_keyval=%d, void *attribute_val=%p)", comm_to_string(comm), comm_keyval, attribute_val);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Comm_set_attr(comm, comm_keyval, attribute_val);
#else
   int error = PMPI_Comm_set_attr(comm, comm_keyval, attribute_val);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_set_attr failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_set_errhandler ( MPI_Comm comm, MPI_Errhandler errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_set_errhandler(MPI_Comm comm=%s, MPI_Errhandler errhandler=%p)", comm_to_string(comm), (void *) errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Comm_set_errhandler(comm, errhandler);
#else
   int error = PMPI_Comm_set_errhandler(comm, errhandler);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_set_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_set_name ( MPI_Comm comm, char *comm_name )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_set_name(MPI_Comm comm=%s, char *comm_name=%p)", comm_to_string(comm), comm_name);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Comm_set_name(comm, comm_name);
#else
   int error = PMPI_Comm_set_name(comm, comm_name);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_set_name failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_size ( MPI_Comm comm, int *size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_size(MPI_Comm comm=%s, int *size=%p)", comm_to_string(comm), size);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Comm_size(comm, size);
#else
   int error = PMPI_Comm_size(comm, size);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_size failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_spawn ( char *command, char *argv[], int maxprocs, MPI_Info info, int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[] )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_spawn(char *command=%p, char *argv[]=%p, int maxprocs=%d, MPI_Info info=%s, int root=%d, MPI_Comm comm=%s, MPI_Comm *intercomm=%p, int array_of_errcodes[]=%p)", command, argv, maxprocs, info_to_string(info), root, comm_to_string(comm), intercomm, array_of_errcodes);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Comm_spawn(command, argv, maxprocs, info, root, comm, intercomm, array_of_errcodes);
#else
   int error = PMPI_Comm_spawn(command, argv, maxprocs, info, root, comm, intercomm, array_of_errcodes);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_spawn failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_spawn_multiple ( int count, char *array_of_commands[], char* *array_of_argv[], int array_of_maxprocs[], MPI_Info array_of_info[], int root, MPI_Comm comm, MPI_Comm *intercomm, int array_of_errcodes[] )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_spawn_multiple(int count=%d, char *array_of_commands[]=%p, char* *array_of_argv[]=%p, int array_of_maxprocs[]=%p, MPI_Info array_of_info[]=%p, int root=%d, MPI_Comm comm=%s, MPI_Comm *intercomm=%p, int array_of_errcodes[]=%p)", count, array_of_commands, array_of_argv, array_of_maxprocs, array_of_info, root, comm_to_string(comm), intercomm, array_of_errcodes);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Comm_spawn_multiple(count, array_of_commands, array_of_argv, array_of_maxprocs, array_of_info, root, comm, intercomm, array_of_errcodes);
#else
   int error = PMPI_Comm_spawn_multiple(count, array_of_commands, array_of_argv, array_of_maxprocs, array_of_info, root, comm, intercomm, array_of_errcodes);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_spawn_multiple failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_split ( MPI_Comm comm, int color, int key, MPI_Comm *newcomm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_split(MPI_Comm comm=%s, int color=%d, int key=%d, MPI_Comm *newcomm=%p)", comm_to_string(comm), color, key, newcomm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Comm_split(comm, color, key, newcomm);
#else
   int error = PMPI_Comm_split(comm, color, key, newcomm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_split failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Comm_test_inter ( MPI_Comm comm, int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Comm_test_inter(MPI_Comm comm=%s, int *flag=%p)", comm_to_string(comm), flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Comm_test_inter(comm, flag);
#else
   int error = PMPI_Comm_test_inter(comm, flag);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Comm_test_inter failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Dims_create ( int nnodes, int ndims, int *dims )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Dims_create(int nnodes=%d, int ndims=%d, int *dims=%p)", nnodes, ndims, dims);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Dims_create(nnodes, ndims, dims);
#else
   int error = PMPI_Dims_create(nnodes, ndims, dims);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Dims_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

/*
int MPI_Dist_graph_create_adjacent ( MPI_Comm comm_old, int indegree, int sources[], int sourceweights[], int outdegree, int destinations[], int destweights[], MPI_Info info, int reorder, MPI_Comm *comm_dist_graph )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Dist_graph_create_adjacent(MPI_Comm comm_old=%s, int indegree=%d, int sources[]=%p, int sourceweights[]=%p, int outdegree=%d, int destinations[]=%p, int destweights[]=%p, MPI_Info info=%s, int reorder=%d, MPI_Comm *comm_dist_graph=%p)", comm_to_string(comm_old), indegree, sources, sourceweights, outdegree, destinations, destweights, info_to_string(info), reorder, comm_dist_graph);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Dist_graph_create_adjacent(comm_old, indegree, sources, sourceweights, outdegree, destinations, destweights, info, reorder, comm_dist_graph);
#else
   int error = PMPI_Dist_graph_create_adjacent(comm_old, indegree, sources, sourceweights, outdegree, destinations, destweights, info, reorder, comm_dist_graph);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm_old, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Dist_graph_create_adjacent failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}
*/
/*
int MPI_Dist_graph_create ( MPI_Comm comm_old, int n, int sources[], int degrees[], int destinations[], int weights[], MPI_Info info, int reorder, MPI_Comm *comm_dist_graph )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Dist_graph_create(MPI_Comm comm_old=%s, int n=%d, int sources[]=%p, int degrees[]=%p, int destinations[]=%p, int weights[]=%p, MPI_Info info=%s, int reorder=%d, MPI_Comm *comm_dist_graph=%p)", comm_to_string(comm_old), n, sources, degrees, destinations, weights, info_to_string(info), reorder, comm_dist_graph);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Dist_graph_create(comm_old, n, sources, degrees, destinations, weights, info, reorder, comm_dist_graph);
#else
   int error = PMPI_Dist_graph_create(comm_old, n, sources, degrees, destinations, weights, info, reorder, comm_dist_graph);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm_old, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Dist_graph_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}
*/
/*
int MPI_Dist_graph_neighbors_count ( MPI_Comm comm, int *indegree, int *outdegree, int *weighted )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Dist_graph_neighbors_count(MPI_Comm comm=%s, int *indegree=%p, int *outdegree=%p, int *weighted=%p)", comm_to_string(comm), indegree, outdegree, weighted);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Dist_graph_neighbors_count(comm, indegree, outdegree, weighted);
#else
   int error = PMPI_Dist_graph_neighbors_count(comm, indegree, outdegree, weighted);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Dist_graph_neighbors_count failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}
*/
/*
int MPI_Dist_graph_neighbors ( MPI_Comm comm, int maxindegree, int sources[], int sourceweights[], int maxoutdegree, int destinations[], int destweights[] )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Dist_graph_neighbors(MPI_Comm comm=%s, int maxindegree=%d, int sources[]=%p, int sourceweights[]=%p, int maxoutdegree=%d, int destinations[]=%p, int destweights[]=%p)", comm_to_string(comm), maxindegree, sources, sourceweights, maxoutdegree, destinations, destweights);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Dist_graph_neighbors(comm, maxindegree, sources, sourceweights, maxoutdegree, destinations, destweights);
#else
   int error = PMPI_Dist_graph_neighbors(comm, maxindegree, sources, sourceweights, maxoutdegree, destinations, destweights);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Dist_graph_neighbors failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}
*/

int MPI_Errhandler_create ( MPI_Handler_function *function, MPI_Errhandler *errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Errhandler_create(MPI_Handler_function *function=%p, MPI_Errhandler *errhandler=%p)", function, errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Errhandler_create(function, errhandler);
#else
   int error = PMPI_Errhandler_create(function, errhandler);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Errhandler_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Errhandler_free ( MPI_Errhandler *errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Errhandler_free(MPI_Errhandler *errhandler=%p)", errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Errhandler_free(errhandler);
#else
   int error = PMPI_Errhandler_free(errhandler);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Errhandler_free failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Errhandler_get ( MPI_Comm comm, MPI_Errhandler *errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Errhandler_get(MPI_Comm comm=%s, MPI_Errhandler *errhandler=%p)", comm_to_string(comm), errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Errhandler_get(comm, errhandler);
#else
   int error = PMPI_Errhandler_get(comm, errhandler);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Errhandler_get failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Errhandler_set ( MPI_Comm comm, MPI_Errhandler errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Errhandler_set(MPI_Comm comm=%s, MPI_Errhandler errhandler=%p)", comm_to_string(comm), (void *) errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Errhandler_set(comm, errhandler);
#else
   int error = PMPI_Errhandler_set(comm, errhandler);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Errhandler_set failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Error_class ( int errorcode, int *errorclass )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Error_class(int errorcode=%d, int *errorclass=%p)", errorcode, errorclass);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Error_class(errorcode, errorclass);
#else
   int error = PMPI_Error_class(errorcode, errorclass);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Error_class failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Error_string ( int errorcode, char *string, int *resultlen )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Error_string(int errorcode=%d, char *string=%p, int *resultlen=%p)", errorcode, string, resultlen);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Error_string(errorcode, string, resultlen);
#else
   int error = PMPI_Error_string(errorcode, string, resultlen);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Error_string failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Exscan ( void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Exscan(void *sendbuf=%p, void *recvbuf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Op op=%s, MPI_Comm comm=%s)", sendbuf, recvbuf, count, type_to_string(datatype), op_to_string(op), comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Exscan(sendbuf, recvbuf, count, datatype, op, comm);
#else
   int error = PMPI_Exscan(sendbuf, recvbuf, count, datatype, op, comm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_EXSCAN, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Exscan failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_call_errhandler ( MPI_File fh, int errorcode )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_call_errhandler(MPI_File fh=%s, int errorcode=%d)", file_to_string(fh), errorcode);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_call_errhandler(fh, errorcode);
#else
   int error = PMPI_File_call_errhandler(fh, errorcode);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_call_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_close ( MPI_File *mpi_fh )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_close(MPI_File *mpi_fh=%p)", mpi_fh);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_close(mpi_fh);
#else
   int error = PMPI_File_close(mpi_fh);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_close failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_create_errhandler ( MPI_File_errhandler_fn *function, MPI_Errhandler *errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_create_errhandler(MPI_File_errhandler_fn *function=%p, MPI_Errhandler *errhandler=%p)", function, errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_create_errhandler(function, errhandler);
#else
   int error = PMPI_File_create_errhandler(function, errhandler);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_create_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_delete ( char *filename, MPI_Info info )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_delete(char *filename=%p, MPI_Info info=%s)", filename, info_to_string(info));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_delete(filename, info);
#else
   int error = PMPI_File_delete(filename, info);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_delete failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_get_amode ( MPI_File mpi_fh, int *amode )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_get_amode(MPI_File mpi_fh=%s, int *amode=%p)", file_to_string(mpi_fh), amode);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_get_amode(mpi_fh, amode);
#else
   int error = PMPI_File_get_amode(mpi_fh, amode);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_get_amode failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_get_atomicity ( MPI_File mpi_fh, int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_get_atomicity(MPI_File mpi_fh=%s, int *flag=%p)", file_to_string(mpi_fh), flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_get_atomicity(mpi_fh, flag);
#else
   int error = PMPI_File_get_atomicity(mpi_fh, flag);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_get_atomicity failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_get_byte_offset ( MPI_File mpi_fh, MPI_Offset offset, MPI_Offset *disp )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_get_byte_offset(MPI_File mpi_fh=%s, MPI_Offset offset=%p, MPI_Offset *disp=%p)", file_to_string(mpi_fh), (void *) offset, disp);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_get_byte_offset(mpi_fh, offset, disp);
#else
   int error = PMPI_File_get_byte_offset(mpi_fh, offset, disp);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_get_byte_offset failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_get_errhandler ( MPI_File file, MPI_Errhandler *errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_get_errhandler(MPI_File file=%s, MPI_Errhandler *errhandler=%p)", file_to_string(file), errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_get_errhandler(file, errhandler);
#else
   int error = PMPI_File_get_errhandler(file, errhandler);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_get_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_get_group ( MPI_File mpi_fh, MPI_Group *g )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_get_group(MPI_File mpi_fh=%s, MPI_Group *g=%p)", file_to_string(mpi_fh), g);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_get_group(mpi_fh, g);
#else
   int error = PMPI_File_get_group(mpi_fh, g);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_get_group failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_get_info ( MPI_File mpi_fh, MPI_Info *info_used )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_get_info(MPI_File mpi_fh=%s, MPI_Info *info_used=%p)", file_to_string(mpi_fh), info_used);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_get_info(mpi_fh, info_used);
#else
   int error = PMPI_File_get_info(mpi_fh, info_used);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_get_info failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_get_position ( MPI_File mpi_fh, MPI_Offset *offset )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_get_position(MPI_File mpi_fh=%s, MPI_Offset *offset=%p)", file_to_string(mpi_fh), offset);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_get_position(mpi_fh, offset);
#else
   int error = PMPI_File_get_position(mpi_fh, offset);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_get_position failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_get_position_shared ( MPI_File mpi_fh, MPI_Offset *offset )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_get_position_shared(MPI_File mpi_fh=%s, MPI_Offset *offset=%p)", file_to_string(mpi_fh), offset);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_get_position_shared(mpi_fh, offset);
#else
   int error = PMPI_File_get_position_shared(mpi_fh, offset);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_get_position_shared failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_get_size ( MPI_File mpi_fh, MPI_Offset *size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_get_size(MPI_File mpi_fh=%s, MPI_Offset *size=%p)", file_to_string(mpi_fh), size);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_get_size(mpi_fh, size);
#else
   int error = PMPI_File_get_size(mpi_fh, size);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_get_size failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_get_type_extent ( MPI_File mpi_fh, MPI_Datatype datatype, MPI_Aint *extent )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_get_type_extent(MPI_File mpi_fh=%s, MPI_Datatype datatype=%s, MPI_Aint *extent=%p)", file_to_string(mpi_fh), type_to_string(datatype), extent);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_get_type_extent(mpi_fh, datatype, extent);
#else
   int error = PMPI_File_get_type_extent(mpi_fh, datatype, extent);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_get_type_extent failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_get_view ( MPI_File mpi_fh, MPI_Offset *disp, MPI_Datatype *etype, MPI_Datatype *filetype, char *datarep )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_get_view(MPI_File mpi_fh=%s, MPI_Offset *disp=%p, MPI_Datatype *etype=%p, MPI_Datatype *filetype=%p, char *datarep=%p)", file_to_string(mpi_fh), disp, etype, filetype, datarep);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(*etype);
   CHECK_TYPE(*filetype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_get_view(mpi_fh, disp, etype, filetype, datarep);
#else
   int error = PMPI_File_get_view(mpi_fh, disp, etype, filetype, datarep);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_get_view failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_iread_at ( MPI_File mpi_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPIO_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_iread_at(MPI_File mpi_fh=%s, MPI_Offset offset=%p, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPIO_Request *r=%p)", file_to_string(mpi_fh), (void *) offset, buf, count, type_to_string(datatype), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_iread_at(mpi_fh, offset, buf, count, datatype, r);
#else
   int error = PMPI_File_iread_at(mpi_fh, offset, buf, count, datatype, r);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_iread_at failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_iread ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_iread(MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Request *r=%p)", file_to_string(mpi_fh), buf, count, type_to_string(datatype), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_iread(mpi_fh, buf, count, datatype, r);
#else
   int error = PMPI_File_iread(mpi_fh, buf, count, datatype, r);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_iread failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_iread_shared ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_iread_shared(MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Request *r=%p)", file_to_string(mpi_fh), buf, count, type_to_string(datatype), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_iread_shared(mpi_fh, buf, count, datatype, r);
#else
   int error = PMPI_File_iread_shared(mpi_fh, buf, count, datatype, r);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_iread_shared failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_iwrite_at ( MPI_File mpi_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPIO_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_iwrite_at(MPI_File mpi_fh=%s, MPI_Offset offset=%p, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPIO_Request *r=%p)", file_to_string(mpi_fh), (void *) offset, buf, count, type_to_string(datatype), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_iwrite_at(mpi_fh, offset, buf, count, datatype, r);
#else
   int error = PMPI_File_iwrite_at(mpi_fh, offset, buf, count, datatype, r);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_iwrite_at failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_iwrite ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_iwrite(MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Request *r=%p)", file_to_string(mpi_fh), buf, count, type_to_string(datatype), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_iwrite(mpi_fh, buf, count, datatype, r);
#else
   int error = PMPI_File_iwrite(mpi_fh, buf, count, datatype, r);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_iwrite failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_iwrite_shared ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPIO_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_iwrite_shared(MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPIO_Request *r=%p)", file_to_string(mpi_fh), buf, count, type_to_string(datatype), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_iwrite_shared(mpi_fh, buf, count, datatype, r);
#else
   int error = PMPI_File_iwrite_shared(mpi_fh, buf, count, datatype, r);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_iwrite_shared failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_open ( MPI_Comm comm, char *filename, int amode, MPI_Info info, MPI_File *fh )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_open(MPI_Comm comm=%s, char *filename=%p, int amode=%d, MPI_Info info=%s, MPI_File *fh=%p)", comm_to_string(comm), filename, amode, info_to_string(info), fh);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_open(comm, filename, amode, info, fh);
#else
   int error = PMPI_File_open(comm, filename, amode, info, fh);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_open failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_preallocate ( MPI_File mpi_fh, MPI_Offset size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_preallocate(MPI_File mpi_fh=%s, MPI_Offset size=%p)", file_to_string(mpi_fh), (void *) size);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_preallocate(mpi_fh, size);
#else
   int error = PMPI_File_preallocate(mpi_fh, size);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_preallocate failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_read_all_begin ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_read_all_begin(MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s)", file_to_string(mpi_fh), buf, count, type_to_string(datatype));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_read_all_begin(mpi_fh, buf, count, datatype);
#else
   int error = PMPI_File_read_all_begin(mpi_fh, buf, count, datatype);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_read_all_begin failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_read_all_end ( MPI_File mpi_fh, void *buf, MPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_read_all_end(MPI_File mpi_fh=%s, void *buf=%p, MPI_Status *status=%p)", file_to_string(mpi_fh), buf, status);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_read_all_end(mpi_fh, buf, status);
#else
   int error = PMPI_File_read_all_end(mpi_fh, buf, status);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_read_all_end failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_read_all ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_read_all(MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Status *status=%p)", file_to_string(mpi_fh), buf, count, type_to_string(datatype), status);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_read_all(mpi_fh, buf, count, datatype, status);
#else
   int error = PMPI_File_read_all(mpi_fh, buf, count, datatype, status);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_read_all failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_read_at_all_begin ( MPI_File mpi_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_read_at_all_begin(MPI_File mpi_fh=%s, MPI_Offset offset=%p, void *buf=%p, int count=%d, MPI_Datatype datatype=%s)", file_to_string(mpi_fh), (void *) offset, buf, count, type_to_string(datatype));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_read_at_all_begin(mpi_fh, offset, buf, count, datatype);
#else
   int error = PMPI_File_read_at_all_begin(mpi_fh, offset, buf, count, datatype);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_read_at_all_begin failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_read_at_all_end ( MPI_File mpi_fh, void *buf, MPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_read_at_all_end(MPI_File mpi_fh=%s, void *buf=%p, MPI_Status *status=%p)", file_to_string(mpi_fh), buf, status);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_read_at_all_end(mpi_fh, buf, status);
#else
   int error = PMPI_File_read_at_all_end(mpi_fh, buf, status);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_read_at_all_end failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_read_at_all ( MPI_File mpi_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_read_at_all(MPI_File mpi_fh=%s, MPI_Offset offset=%p, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Status *status=%p)", file_to_string(mpi_fh), (void *) offset, buf, count, type_to_string(datatype), status);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_read_at_all(mpi_fh, offset, buf, count, datatype, status);
#else
   int error = PMPI_File_read_at_all(mpi_fh, offset, buf, count, datatype, status);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_read_at_all failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_read_at ( MPI_File mpi_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_read_at(MPI_File mpi_fh=%s, MPI_Offset offset=%p, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Status *status=%p)", file_to_string(mpi_fh), (void *) offset, buf, count, type_to_string(datatype), status);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_read_at(mpi_fh, offset, buf, count, datatype, status);
#else
   int error = PMPI_File_read_at(mpi_fh, offset, buf, count, datatype, status);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_read_at failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_read ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_read(MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Status *status=%p)", file_to_string(mpi_fh), buf, count, type_to_string(datatype), status);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_read(mpi_fh, buf, count, datatype, status);
#else
   int error = PMPI_File_read(mpi_fh, buf, count, datatype, status);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_read failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_read_ordered_begin ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_read_ordered_begin(MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s)", file_to_string(mpi_fh), buf, count, type_to_string(datatype));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_read_ordered_begin(mpi_fh, buf, count, datatype);
#else
   int error = PMPI_File_read_ordered_begin(mpi_fh, buf, count, datatype);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_read_ordered_begin failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_read_ordered_end ( MPI_File mpi_fh, void *buf, MPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_read_ordered_end(MPI_File mpi_fh=%s, void *buf=%p, MPI_Status *status=%p)", file_to_string(mpi_fh), buf, status);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_read_ordered_end(mpi_fh, buf, status);
#else
   int error = PMPI_File_read_ordered_end(mpi_fh, buf, status);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_read_ordered_end failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_read_ordered ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_read_ordered(MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Status *status=%p)", file_to_string(mpi_fh), buf, count, type_to_string(datatype), status);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_read_ordered(mpi_fh, buf, count, datatype, status);
#else
   int error = PMPI_File_read_ordered(mpi_fh, buf, count, datatype, status);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_read_ordered failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_read_shared ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_read_shared(MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Status *status=%p)", file_to_string(mpi_fh), buf, count, type_to_string(datatype), status);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_read_shared(mpi_fh, buf, count, datatype, status);
#else
   int error = PMPI_File_read_shared(mpi_fh, buf, count, datatype, status);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_read_shared failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_seek ( MPI_File mpi_fh, MPI_Offset offset, int whence )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_seek(MPI_File mpi_fh=%s, MPI_Offset offset=%p, int whence=%d)", file_to_string(mpi_fh), (void *) offset, whence);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_seek(mpi_fh, offset, whence);
#else
   int error = PMPI_File_seek(mpi_fh, offset, whence);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_seek failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_seek_shared ( MPI_File mpi_fh, MPI_Offset offset, int whence )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_seek_shared(MPI_File mpi_fh=%s, MPI_Offset offset=%p, int whence=%d)", file_to_string(mpi_fh), (void *) offset, whence);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_seek_shared(mpi_fh, offset, whence);
#else
   int error = PMPI_File_seek_shared(mpi_fh, offset, whence);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_seek_shared failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_set_atomicity ( MPI_File mpi_fh, int flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_set_atomicity(MPI_File mpi_fh=%s, int flag=%d)", file_to_string(mpi_fh), flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_set_atomicity(mpi_fh, flag);
#else
   int error = PMPI_File_set_atomicity(mpi_fh, flag);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_set_atomicity failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_set_errhandler ( MPI_File file, MPI_Errhandler errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_set_errhandler(MPI_File file=%s, MPI_Errhandler errhandler=%p)", file_to_string(file), (void *) errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_set_errhandler(file, errhandler);
#else
   int error = PMPI_File_set_errhandler(file, errhandler);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_set_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_set_info ( MPI_File mpi_fh, MPI_Info info )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_set_info(MPI_File mpi_fh=%s, MPI_Info info=%s)", file_to_string(mpi_fh), info_to_string(info));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_set_info(mpi_fh, info);
#else
   int error = PMPI_File_set_info(mpi_fh, info);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_set_info failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_set_size ( MPI_File mpi_fh, MPI_Offset size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_set_size(MPI_File mpi_fh=%s, MPI_Offset size=%p)", file_to_string(mpi_fh), (void *) size);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_set_size(mpi_fh, size);
#else
   int error = PMPI_File_set_size(mpi_fh, size);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_set_size failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_set_view ( MPI_File mpi_fh, MPI_Offset disp, MPI_Datatype etype, MPI_Datatype filetype, char *datarep, MPI_Info info )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_set_view(MPI_File mpi_fh=%s, MPI_Offset disp=%p, MPI_Datatype etype=%s, MPI_Datatype filetype=%s, char *datarep=%p, MPI_Info info=%s)", file_to_string(mpi_fh), (void *) disp, type_to_string(etype), type_to_string(filetype), datarep, info_to_string(info));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(etype);
   CHECK_TYPE(filetype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_set_view(mpi_fh, disp, etype, filetype, datarep, info);
#else
   int error = PMPI_File_set_view(mpi_fh, disp, etype, filetype, datarep, info);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_set_view failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_sync ( MPI_File mpi_fh )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_sync(MPI_File mpi_fh=%s)", file_to_string(mpi_fh));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_sync(mpi_fh);
#else
   int error = PMPI_File_sync(mpi_fh);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_sync failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_write_all_begin ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_write_all_begin(MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s)", file_to_string(mpi_fh), buf, count, type_to_string(datatype));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_write_all_begin(mpi_fh, buf, count, datatype);
#else
   int error = PMPI_File_write_all_begin(mpi_fh, buf, count, datatype);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_write_all_begin failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_write_all_end ( MPI_File mpi_fh, void *buf, MPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_write_all_end(MPI_File mpi_fh=%s, void *buf=%p, MPI_Status *status=%p)", file_to_string(mpi_fh), buf, status);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_write_all_end(mpi_fh, buf, status);
#else
   int error = PMPI_File_write_all_end(mpi_fh, buf, status);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_write_all_end failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_write_all ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_write_all(MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Status *status=%p)", file_to_string(mpi_fh), buf, count, type_to_string(datatype), status);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_write_all(mpi_fh, buf, count, datatype, status);
#else
   int error = PMPI_File_write_all(mpi_fh, buf, count, datatype, status);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_write_all failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_write_at_all_begin ( MPI_File mpi_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_write_at_all_begin(MPI_File mpi_fh=%s, MPI_Offset offset=%p, void *buf=%p, int count=%d, MPI_Datatype datatype=%s)", file_to_string(mpi_fh), (void *) offset, buf, count, type_to_string(datatype));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_write_at_all_begin(mpi_fh, offset, buf, count, datatype);
#else
   int error = PMPI_File_write_at_all_begin(mpi_fh, offset, buf, count, datatype);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_write_at_all_begin failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_write_at_all_end ( MPI_File mpi_fh, void *buf, MPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_write_at_all_end(MPI_File mpi_fh=%s, void *buf=%p, MPI_Status *status=%p)", file_to_string(mpi_fh), buf, status);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_write_at_all_end(mpi_fh, buf, status);
#else
   int error = PMPI_File_write_at_all_end(mpi_fh, buf, status);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_write_at_all_end failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_write_at_all ( MPI_File mpi_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_write_at_all(MPI_File mpi_fh=%s, MPI_Offset offset=%p, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Status *status=%p)", file_to_string(mpi_fh), (void *) offset, buf, count, type_to_string(datatype), status);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_write_at_all(mpi_fh, offset, buf, count, datatype, status);
#else
   int error = PMPI_File_write_at_all(mpi_fh, offset, buf, count, datatype, status);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_write_at_all failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_write_at ( MPI_File mpi_fh, MPI_Offset offset, void *buf, int count, MPI_Datatype datatype, MPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_write_at(MPI_File mpi_fh=%s, MPI_Offset offset=%p, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Status *status=%p)", file_to_string(mpi_fh), (void *) offset, buf, count, type_to_string(datatype), status);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_write_at(mpi_fh, offset, buf, count, datatype, status);
#else
   int error = PMPI_File_write_at(mpi_fh, offset, buf, count, datatype, status);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_write_at failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_write ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_write(MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Status *status=%p)", file_to_string(mpi_fh), buf, count, type_to_string(datatype), status);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_write(mpi_fh, buf, count, datatype, status);
#else
   int error = PMPI_File_write(mpi_fh, buf, count, datatype, status);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_write failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_write_ordered_begin ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_write_ordered_begin(MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s)", file_to_string(mpi_fh), buf, count, type_to_string(datatype));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_write_ordered_begin(mpi_fh, buf, count, datatype);
#else
   int error = PMPI_File_write_ordered_begin(mpi_fh, buf, count, datatype);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_write_ordered_begin failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_write_ordered_end ( MPI_File mpi_fh, void *buf, MPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_write_ordered_end(MPI_File mpi_fh=%s, void *buf=%p, MPI_Status *status=%p)", file_to_string(mpi_fh), buf, status);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_write_ordered_end(mpi_fh, buf, status);
#else
   int error = PMPI_File_write_ordered_end(mpi_fh, buf, status);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_write_ordered_end failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_write_ordered ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_write_ordered(MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Status *status=%p)", file_to_string(mpi_fh), buf, count, type_to_string(datatype), status);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_write_ordered(mpi_fh, buf, count, datatype, status);
#else
   int error = PMPI_File_write_ordered(mpi_fh, buf, count, datatype, status);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_write_ordered failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_File_write_shared ( MPI_File mpi_fh, void *buf, int count, MPI_Datatype datatype, MPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_File_write_shared(MPI_File mpi_fh=%s, void *buf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Status *status=%p)", file_to_string(mpi_fh), buf, count, type_to_string(datatype), status);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_File_write_shared(mpi_fh, buf, count, datatype, status);
#else
   int error = PMPI_File_write_shared(mpi_fh, buf, count, datatype, status);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_File_write_shared failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Finalized ( int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Finalized(int *flag=%p)", flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Finalized(flag);
#else
   int error = PMPI_Finalized(flag);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Finalized failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Finalize (  )
{
#ifdef TRACE_CALLS
   INFO(0, "MPI_Finalize()");
#endif // TRACE_CALLS

#ifdef IBIS_INTERCEPT
   int error = IMPI_Finalize();
#else
   int error = PMPI_Finalize();
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_finalize();
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Finalize failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Free_mem ( void *base )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Free_mem(void *base=%p)", base);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Free_mem(base);
#else
   int error = PMPI_Free_mem(base);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Free_mem failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Gather ( void *sendbuf, int sendcnt, MPI_Datatype sendtype, void *recvbuf, int recvcnt, MPI_Datatype recvtype, int root, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Gather(void *sendbuf=%p, int sendcnt=%d, MPI_Datatype sendtype=%s, void *recvbuf=%p, int recvcnt=%d, MPI_Datatype recvtype=%s, int root=%d, MPI_Comm comm=%s)", sendbuf, sendcnt, type_to_string(sendtype), recvbuf, recvcnt, type_to_string(recvtype), root, comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(sendtype);
   CHECK_TYPE(recvtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Gather(sendbuf, sendcnt, sendtype, recvbuf, recvcnt, recvtype, root, comm);
#else
   int error = PMPI_Gather(sendbuf, sendcnt, sendtype, recvbuf, recvcnt, recvtype, root, comm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_GATHER, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Gather failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Gatherv ( void *sendbuf, int sendcnt, MPI_Datatype sendtype, void *recvbuf, int *recvcnts, int *displs, MPI_Datatype recvtype, int root, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Gatherv(void *sendbuf=%p, int sendcnt=%d, MPI_Datatype sendtype=%s, void *recvbuf=%p, int *recvcnts=%p, int *displs=%p, MPI_Datatype recvtype=%s, int root=%d, MPI_Comm comm=%s)", sendbuf, sendcnt, type_to_string(sendtype), recvbuf, recvcnts, displs, type_to_string(recvtype), root, comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(sendtype);
   CHECK_TYPE(recvtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Gatherv(sendbuf, sendcnt, sendtype, recvbuf, recvcnts, displs, recvtype, root, comm);
#else
   int error = PMPI_Gatherv(sendbuf, sendcnt, sendtype, recvbuf, recvcnts, displs, recvtype, root, comm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_GATHER, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Gatherv failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Get_address ( void *location, MPI_Aint *address )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Get_address(void *location=%p, MPI_Aint *address=%p)", location, address);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Get_address(location, address);
#else
   int error = PMPI_Get_address(location, address);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Get_address failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Get_count ( MPI_Status *status, MPI_Datatype datatype, int *count )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Get_count(MPI_Status *status=%p, MPI_Datatype datatype=%s, int *count=%p)", status, type_to_string(datatype), count);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Get_count(status, datatype, count);
#else
   int error = PMPI_Get_count(status, datatype, count);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Get_count failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Get_elements ( MPI_Status *status, MPI_Datatype datatype, int *elements )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Get_elements(MPI_Status *status=%p, MPI_Datatype datatype=%s, int *elements=%p)", status, type_to_string(datatype), elements);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Get_elements(status, datatype, elements);
#else
   int error = PMPI_Get_elements(status, datatype, elements);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Get_elements failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Get ( void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Get(void *origin_addr=%p, int origin_count=%d, MPI_Datatype origin_datatype=%s, int target_rank=%d, MPI_Aint target_disp=%p, int target_count=%d, MPI_Datatype target_datatype=%s, MPI_Win win=%s)", origin_addr, origin_count, type_to_string(origin_datatype), target_rank, (void *) target_disp, target_count, type_to_string(target_datatype), win_to_string(win));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(origin_datatype);
   CHECK_TYPE(target_datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Get(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
#else
   int error = PMPI_Get(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Get failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Get_processor_name ( char *name, int *resultlen )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Get_processor_name(char *name=%p, int *resultlen=%p)", name, resultlen);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Get_processor_name(name, resultlen);
#else
   int error = PMPI_Get_processor_name(name, resultlen);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Get_processor_name failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Get_version ( int *version, int *subversion )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Get_version(int *version=%p, int *subversion=%p)", version, subversion);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Get_version(version, subversion);
#else
   int error = PMPI_Get_version(version, subversion);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Get_version failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Graph_create ( MPI_Comm comm_old, int nnodes, int *indx, int *edges, int reorder, MPI_Comm *comm_graph )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Graph_create(MPI_Comm comm_old=%s, int nnodes=%d, int *indx=%p, int *edges=%p, int reorder=%d, MPI_Comm *comm_graph=%p)", comm_to_string(comm_old), nnodes, indx, edges, reorder, comm_graph);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Graph_create(comm_old, nnodes, indx, edges, reorder, comm_graph);
#else
   int error = PMPI_Graph_create(comm_old, nnodes, indx, edges, reorder, comm_graph);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm_old, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Graph_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Graphdims_get ( MPI_Comm comm, int *nnodes, int *nedges )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Graphdims_get(MPI_Comm comm=%s, int *nnodes=%p, int *nedges=%p)", comm_to_string(comm), nnodes, nedges);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Graphdims_get(comm, nnodes, nedges);
#else
   int error = PMPI_Graphdims_get(comm, nnodes, nedges);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Graphdims_get failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Graph_get ( MPI_Comm comm, int maxindex, int maxedges, int *indx, int *edges )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Graph_get(MPI_Comm comm=%s, int maxindex=%d, int maxedges=%d, int *indx=%p, int *edges=%p)", comm_to_string(comm), maxindex, maxedges, indx, edges);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Graph_get(comm, maxindex, maxedges, indx, edges);
#else
   int error = PMPI_Graph_get(comm, maxindex, maxedges, indx, edges);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Graph_get failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Graph_map ( MPI_Comm comm_old, int nnodes, int *indx, int *edges, int *newrank )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Graph_map(MPI_Comm comm_old=%s, int nnodes=%d, int *indx=%p, int *edges=%p, int *newrank=%p)", comm_to_string(comm_old), nnodes, indx, edges, newrank);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Graph_map(comm_old, nnodes, indx, edges, newrank);
#else
   int error = PMPI_Graph_map(comm_old, nnodes, indx, edges, newrank);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm_old, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Graph_map failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Graph_neighbors_count ( MPI_Comm comm, int rank, int *nneighbors )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Graph_neighbors_count(MPI_Comm comm=%s, int rank=%d, int *nneighbors=%p)", comm_to_string(comm), rank, nneighbors);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Graph_neighbors_count(comm, rank, nneighbors);
#else
   int error = PMPI_Graph_neighbors_count(comm, rank, nneighbors);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Graph_neighbors_count failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Graph_neighbors ( MPI_Comm comm, int rank, int maxneighbors, int *neighbors )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Graph_neighbors(MPI_Comm comm=%s, int rank=%d, int maxneighbors=%d, int *neighbors=%p)", comm_to_string(comm), rank, maxneighbors, neighbors);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Graph_neighbors(comm, rank, maxneighbors, neighbors);
#else
   int error = PMPI_Graph_neighbors(comm, rank, maxneighbors, neighbors);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Graph_neighbors failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Grequest_complete ( MPI_Request r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Grequest_complete(MPI_Request r=%s)", request_to_string(r));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Grequest_complete(r);
#else
   int error = PMPI_Grequest_complete(r);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Grequest_complete failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Grequest_start ( MPI_Grequest_query_function *query_fn, MPI_Grequest_free_function *free_fn, MPI_Grequest_cancel_function *cancel_fn, void *extra_state, MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Grequest_start(MPI_Grequest_query_function *query_fn=%p, MPI_Grequest_free_function *free_fn=%p, MPI_Grequest_cancel_function *cancel_fn=%p, void *extra_state=%p, MPI_Request *r=%p)", query_fn, free_fn, cancel_fn, extra_state, r);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Grequest_start(query_fn, free_fn, cancel_fn, extra_state, r);
#else
   int error = PMPI_Grequest_start(query_fn, free_fn, cancel_fn, extra_state, r);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Grequest_start failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Group_compare ( MPI_Group group1, MPI_Group group2, int *result )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Group_compare(MPI_Group group1=%s, MPI_Group group2=%s, int *result=%p)", group_to_string(group1), group_to_string(group2), result);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Group_compare(group1, group2, result);
#else
   int error = PMPI_Group_compare(group1, group2, result);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Group_compare failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Group_difference ( MPI_Group group1, MPI_Group group2, MPI_Group *newgroup )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Group_difference(MPI_Group group1=%s, MPI_Group group2=%s, MPI_Group *newgroup=%p)", group_to_string(group1), group_to_string(group2), newgroup);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Group_difference(group1, group2, newgroup);
#else
   int error = PMPI_Group_difference(group1, group2, newgroup);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Group_difference failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Group_excl ( MPI_Group g, int n, int *ranks, MPI_Group *newgroup )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Group_excl(MPI_Group g=%s, int n=%d, int *ranks=%p, MPI_Group *newgroup=%p)", group_to_string(g), n, ranks, newgroup);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Group_excl(g, n, ranks, newgroup);
#else
   int error = PMPI_Group_excl(g, n, ranks, newgroup);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Group_excl failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Group_free ( MPI_Group *g )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Group_free(MPI_Group *g=%p)", g);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Group_free(g);
#else
   int error = PMPI_Group_free(g);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Group_free failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Group_incl ( MPI_Group g, int n, int *ranks, MPI_Group *newgroup )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Group_incl(MPI_Group g=%s, int n=%d, int *ranks=%p, MPI_Group *newgroup=%p)", group_to_string(g), n, ranks, newgroup);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Group_incl(g, n, ranks, newgroup);
#else
   int error = PMPI_Group_incl(g, n, ranks, newgroup);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Group_incl failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Group_intersection ( MPI_Group group1, MPI_Group group2, MPI_Group *newgroup )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Group_intersection(MPI_Group group1=%s, MPI_Group group2=%s, MPI_Group *newgroup=%p)", group_to_string(group1), group_to_string(group2), newgroup);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Group_intersection(group1, group2, newgroup);
#else
   int error = PMPI_Group_intersection(group1, group2, newgroup);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Group_intersection failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Group_range_excl ( MPI_Group g, int n, int ranges[][3], MPI_Group *newgroup )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Group_range_excl(MPI_Group g=%s, int n=%d, int ranges[][3]=%p, MPI_Group *newgroup=%p)", group_to_string(g), n, ranges, newgroup);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Group_range_excl(g, n, ranges, newgroup);
#else
   int error = PMPI_Group_range_excl(g, n, ranges, newgroup);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Group_range_excl failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Group_range_incl ( MPI_Group g, int n, int ranges[][3], MPI_Group *newgroup )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Group_range_incl(MPI_Group g=%s, int n=%d, int ranges[][3]=%p, MPI_Group *newgroup=%p)", group_to_string(g), n, ranges, newgroup);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Group_range_incl(g, n, ranges, newgroup);
#else
   int error = PMPI_Group_range_incl(g, n, ranges, newgroup);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Group_range_incl failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Group_rank ( MPI_Group g, int *rank )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Group_rank(MPI_Group g=%s, int *rank=%p)", group_to_string(g), rank);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Group_rank(g, rank);
#else
   int error = PMPI_Group_rank(g, rank);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Group_rank failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Group_size ( MPI_Group g, int *size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Group_size(MPI_Group g=%s, int *size=%p)", group_to_string(g), size);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Group_size(g, size);
#else
   int error = PMPI_Group_size(g, size);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Group_size failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Group_translate_ranks ( MPI_Group group1, int n, int *ranks1, MPI_Group group2, int *ranks2 )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Group_translate_ranks(MPI_Group group1=%s, int n=%d, int *ranks1=%p, MPI_Group group2=%s, int *ranks2=%p)", group_to_string(group1), n, ranks1, group_to_string(group2), ranks2);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Group_translate_ranks(group1, n, ranks1, group2, ranks2);
#else
   int error = PMPI_Group_translate_ranks(group1, n, ranks1, group2, ranks2);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Group_translate_ranks failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Group_union ( MPI_Group group1, MPI_Group group2, MPI_Group *newgroup )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Group_union(MPI_Group group1=%s, MPI_Group group2=%s, MPI_Group *newgroup=%p)", group_to_string(group1), group_to_string(group2), newgroup);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Group_union(group1, group2, newgroup);
#else
   int error = PMPI_Group_union(group1, group2, newgroup);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Group_union failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Ibsend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Ibsend(void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s, MPI_Request *r=%p)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Ibsend(buf, count, datatype, dest, tag, comm, r);
#else
   int error = PMPI_Ibsend(buf, count, datatype, dest, tag, comm, r);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_IBSEND, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Ibsend failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Info_create ( MPI_Info *info )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Info_create(MPI_Info *info=%p)", info);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Info_create(info);
#else
   int error = PMPI_Info_create(info);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Info_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Info_delete ( MPI_Info info, char *key )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Info_delete(MPI_Info info=%s, char *key=%p)", info_to_string(info), key);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Info_delete(info, key);
#else
   int error = PMPI_Info_delete(info, key);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Info_delete failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Info_dup ( MPI_Info info, MPI_Info *newinfo )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Info_dup(MPI_Info info=%s, MPI_Info *newinfo=%p)", info_to_string(info), newinfo);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Info_dup(info, newinfo);
#else
   int error = PMPI_Info_dup(info, newinfo);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Info_dup failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Info_free ( MPI_Info *info )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Info_free(MPI_Info *info=%p)", info);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Info_free(info);
#else
   int error = PMPI_Info_free(info);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Info_free failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Info_get ( MPI_Info info, char *key, int valuelen, char *value, int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Info_get(MPI_Info info=%s, char *key=%p, int valuelen=%d, char *value=%p, int *flag=%p)", info_to_string(info), key, valuelen, value, flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Info_get(info, key, valuelen, value, flag);
#else
   int error = PMPI_Info_get(info, key, valuelen, value, flag);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Info_get failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Info_get_nkeys ( MPI_Info info, int *nkeys )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Info_get_nkeys(MPI_Info info=%s, int *nkeys=%p)", info_to_string(info), nkeys);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Info_get_nkeys(info, nkeys);
#else
   int error = PMPI_Info_get_nkeys(info, nkeys);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Info_get_nkeys failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Info_get_nthkey ( MPI_Info info, int n, char *key )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Info_get_nthkey(MPI_Info info=%s, int n=%d, char *key=%p)", info_to_string(info), n, key);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Info_get_nthkey(info, n, key);
#else
   int error = PMPI_Info_get_nthkey(info, n, key);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Info_get_nthkey failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Info_get_valuelen ( MPI_Info info, char *key, int *valuelen, int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Info_get_valuelen(MPI_Info info=%s, char *key=%p, int *valuelen=%p, int *flag=%p)", info_to_string(info), key, valuelen, flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Info_get_valuelen(info, key, valuelen, flag);
#else
   int error = PMPI_Info_get_valuelen(info, key, valuelen, flag);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Info_get_valuelen failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Info_set ( MPI_Info info, char *key, char *value )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Info_set(MPI_Info info=%s, char *key=%p, char *value=%p)", info_to_string(info), key, value);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Info_set(info, key, value);
#else
   int error = PMPI_Info_set(info, key, value);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Info_set failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Initialized ( int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Initialized(int *flag=%p)", flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Initialized(flag);
#else
   int error = PMPI_Initialized(flag);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Initialized failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Init_thread ( int *argc, char ***argv, int required, int *provided )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Init_thread(int *argc=%p, char ***argv=%p, int required=%d, int *provided=%p)", argc, argv, required, provided);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Init_thread(argc, argv, required, provided);
#else
   int error = PMPI_Init_thread(argc, argv, required, provided);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Init_thread failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Intercomm_create ( MPI_Comm local_comm, int local_leader, MPI_Comm peer_comm, int remote_leader, int tag, MPI_Comm *newintercomm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Intercomm_create(MPI_Comm local_comm=%s, int local_leader=%d, MPI_Comm peer_comm=%s, int remote_leader=%d, int tag=%d, MPI_Comm *newintercomm=%p)", comm_to_string(local_comm), local_leader, comm_to_string(peer_comm), remote_leader, tag, newintercomm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Intercomm_create(local_comm, local_leader, peer_comm, remote_leader, tag, newintercomm);
#else
   int error = PMPI_Intercomm_create(local_comm, local_leader, peer_comm, remote_leader, tag, newintercomm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(local_comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Intercomm_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Intercomm_merge ( MPI_Comm intercomm, int high, MPI_Comm *newintracomm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Intercomm_merge(MPI_Comm intercomm=%s, int high=%d, MPI_Comm *newintracomm=%p)", comm_to_string(intercomm), high, newintracomm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Intercomm_merge(intercomm, high, newintracomm);
#else
   int error = PMPI_Intercomm_merge(intercomm, high, newintracomm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(intercomm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Intercomm_merge failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Iprobe ( int source, int tag, MPI_Comm comm, int *flag, MPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Iprobe(int source=%d, int tag=%d, MPI_Comm comm=%s, int *flag=%p, MPI_Status *status=%p)", source, tag, comm_to_string(comm), flag, status);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Iprobe(source, tag, comm, flag, status);
#else
   int error = PMPI_Iprobe(source, tag, comm, flag, status);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_IPROBE, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Iprobe failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Irecv ( void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Irecv(void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int source=%d, int tag=%d, MPI_Comm comm=%s, MPI_Request *r=%p)", buf, count, type_to_string(datatype), source, tag, comm_to_string(comm), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Irecv(buf, count, datatype, source, tag, comm, r);
#else
   int error = PMPI_Irecv(buf, count, datatype, source, tag, comm, r);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_IRECV, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Irecv failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Irsend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Irsend(void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s, MPI_Request *r=%p)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Irsend(buf, count, datatype, dest, tag, comm, r);
#else
   int error = PMPI_Irsend(buf, count, datatype, dest, tag, comm, r);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_IRSEND, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Irsend failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Isend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Isend(void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s, MPI_Request *r=%p)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Isend(buf, count, datatype, dest, tag, comm, r);
#else
   int error = PMPI_Isend(buf, count, datatype, dest, tag, comm, r);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_ISEND, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Isend failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Issend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Issend(void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s, MPI_Request *r=%p)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Issend(buf, count, datatype, dest, tag, comm, r);
#else
   int error = PMPI_Issend(buf, count, datatype, dest, tag, comm, r);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_ISSEND, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Issend failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Is_thread_main ( int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Is_thread_main(int *flag=%p)", flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Is_thread_main(flag);
#else
   int error = PMPI_Is_thread_main(flag);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Is_thread_main failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Keyval_create ( MPI_Copy_function *copy_fn, MPI_Delete_function *delete_fn, int *keyval, void *extra_state )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Keyval_create(MPI_Copy_function *copy_fn=%p, MPI_Delete_function *delete_fn=%p, int *keyval=%p, void *extra_state=%p)", copy_fn, delete_fn, keyval, extra_state);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Keyval_create(copy_fn, delete_fn, keyval, extra_state);
#else
   int error = PMPI_Keyval_create(copy_fn, delete_fn, keyval, extra_state);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Keyval_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Keyval_free ( int *keyval )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Keyval_free(int *keyval=%p)", keyval);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Keyval_free(keyval);
#else
   int error = PMPI_Keyval_free(keyval);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Keyval_free failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Lookup_name ( char *service_name, MPI_Info info, char *port_name )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Lookup_name(char *service_name=%p, MPI_Info info=%s, char *port_name=%p)", service_name, info_to_string(info), port_name);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Lookup_name(service_name, info, port_name);
#else
   int error = PMPI_Lookup_name(service_name, info, port_name);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Lookup_name failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

/*
int MPI_Op_commutative ( MPI_Op op, int *commute )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Op_commutative(MPI_Op op=%s, int *commute=%p)", op_to_string(op), commute);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Op_commutative(op, commute);
#else
   int error = PMPI_Op_commutative(op, commute);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Op_commutative failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}
*/

int MPI_Op_create ( MPI_User_function *function, int commute, MPI_Op *op )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Op_create(MPI_User_function *function=%p, int commute=%d, MPI_Op *op=%p)", function, commute, op);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Op_create(function, commute, op);
#else
   int error = PMPI_Op_create(function, commute, op);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Op_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Open_port ( MPI_Info info, char *port_name )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Open_port(MPI_Info info=%s, char *port_name=%p)", info_to_string(info), port_name);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Open_port(info, port_name);
#else
   int error = PMPI_Open_port(info, port_name);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Open_port failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Op_free ( MPI_Op *op )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Op_free(MPI_Op *op=%p)", op);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Op_free(op);
#else
   int error = PMPI_Op_free(op);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Op_free failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Pack_external ( char *datarep, void *inbuf, int incount, MPI_Datatype datatype, void *outbuf, MPI_Aint outcount, MPI_Aint *position )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Pack_external(char *datarep=%p, void *inbuf=%p, int incount=%d, MPI_Datatype datatype=%s, void *outbuf=%p, MPI_Aint outcount=%p, MPI_Aint *position=%p)", datarep, inbuf, incount, type_to_string(datatype), outbuf, (void *) outcount, position);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Pack_external(datarep, inbuf, incount, datatype, outbuf, outcount, position);
#else
   int error = PMPI_Pack_external(datarep, inbuf, incount, datatype, outbuf, outcount, position);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Pack_external failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Pack_external_size ( char *datarep, int incount, MPI_Datatype datatype, MPI_Aint *size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Pack_external_size(char *datarep=%p, int incount=%d, MPI_Datatype datatype=%s, MPI_Aint *size=%p)", datarep, incount, type_to_string(datatype), size);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Pack_external_size(datarep, incount, datatype, size);
#else
   int error = PMPI_Pack_external_size(datarep, incount, datatype, size);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Pack_external_size failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Pack ( void *inbuf, int incount, MPI_Datatype datatype, void *outbuf, int outcount, int *position, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Pack(void *inbuf=%p, int incount=%d, MPI_Datatype datatype=%s, void *outbuf=%p, int outcount=%d, int *position=%p, MPI_Comm comm=%s)", inbuf, incount, type_to_string(datatype), outbuf, outcount, position, comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Pack(inbuf, incount, datatype, outbuf, outcount, position, comm);
#else
   int error = PMPI_Pack(inbuf, incount, datatype, outbuf, outcount, position, comm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Pack failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Pack_size ( int incount, MPI_Datatype datatype, MPI_Comm comm, int *size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Pack_size(int incount=%d, MPI_Datatype datatype=%s, MPI_Comm comm=%s, int *size=%p)", incount, type_to_string(datatype), comm_to_string(comm), size);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Pack_size(incount, datatype, comm, size);
#else
   int error = PMPI_Pack_size(incount, datatype, comm, size);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Pack_size failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Probe ( int source, int tag, MPI_Comm comm, MPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Probe(int source=%d, int tag=%d, MPI_Comm comm=%s, MPI_Status *status=%p)", source, tag, comm_to_string(comm), status);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Probe(source, tag, comm, status);
#else
   int error = PMPI_Probe(source, tag, comm, status);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_PROBE, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Probe failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Publish_name ( char *service_name, MPI_Info info, char *port_name )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Publish_name(char *service_name=%p, MPI_Info info=%s, char *port_name=%p)", service_name, info_to_string(info), port_name);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Publish_name(service_name, info, port_name);
#else
   int error = PMPI_Publish_name(service_name, info, port_name);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Publish_name failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Put ( void *origin_addr, int origin_count, MPI_Datatype origin_datatype, int target_rank, MPI_Aint target_disp, int target_count, MPI_Datatype target_datatype, MPI_Win win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Put(void *origin_addr=%p, int origin_count=%d, MPI_Datatype origin_datatype=%s, int target_rank=%d, MPI_Aint target_disp=%p, int target_count=%d, MPI_Datatype target_datatype=%s, MPI_Win win=%s)", origin_addr, origin_count, type_to_string(origin_datatype), target_rank, (void *) target_disp, target_count, type_to_string(target_datatype), win_to_string(win));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(origin_datatype);
   CHECK_TYPE(target_datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Put(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
#else
   int error = PMPI_Put(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Put failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Query_thread ( int *provided )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Query_thread(int *provided=%p)", provided);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Query_thread(provided);
#else
   int error = PMPI_Query_thread(provided);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Query_thread failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Recv ( void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Recv(void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int source=%d, int tag=%d, MPI_Comm comm=%s, MPI_Status *status=%p)", buf, count, type_to_string(datatype), source, tag, comm_to_string(comm), status);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Recv(buf, count, datatype, source, tag, comm, status);
#else
   int error = PMPI_Recv(buf, count, datatype, source, tag, comm, status);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_RECV, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Recv failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Recv_init ( void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Recv_init(void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int source=%d, int tag=%d, MPI_Comm comm=%s, MPI_Request *r=%p)", buf, count, type_to_string(datatype), source, tag, comm_to_string(comm), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Recv_init(buf, count, datatype, source, tag, comm, r);
#else
   int error = PMPI_Recv_init(buf, count, datatype, source, tag, comm, r);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Recv_init failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Reduce ( void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Reduce(void *sendbuf=%p, void *recvbuf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Op op=%s, int root=%d, MPI_Comm comm=%s)", sendbuf, recvbuf, count, type_to_string(datatype), op_to_string(op), root, comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm);
#else
   int error = PMPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_REDUCE, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Reduce failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

/*
int MPI_Reduce_local ( void *inbuf, void *inoutbuf, int count, MPI_Datatype datatype, MPI_Op op )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Reduce_local(void *inbuf=%p, void *inoutbuf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Op op=%s)", inbuf, inoutbuf, count, type_to_string(datatype), op_to_string(op));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Reduce_local(inbuf, inoutbuf, count, datatype, op);
#else
   int error = PMPI_Reduce_local(inbuf, inoutbuf, count, datatype, op);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Reduce_local failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}
*/
/*
int MPI_Reduce_scatter_block ( void *sendbuf, void *recvbuf, int recvcount, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Reduce_scatter_block(void *sendbuf=%p, void *recvbuf=%p, int recvcount=%d, MPI_Datatype datatype=%s, MPI_Op op=%s, MPI_Comm comm=%s)", sendbuf, recvbuf, recvcount, type_to_string(datatype), op_to_string(op), comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Reduce_scatter_block(sendbuf, recvbuf, recvcount, datatype, op, comm);
#else
   int error = PMPI_Reduce_scatter_block(sendbuf, recvbuf, recvcount, datatype, op, comm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Reduce_scatter_block failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}
*/

int MPI_Reduce_scatter ( void *sendbuf, void *recvbuf, int *recvcnts, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Reduce_scatter(void *sendbuf=%p, void *recvbuf=%p, int *recvcnts=%p, MPI_Datatype datatype=%s, MPI_Op op=%s, MPI_Comm comm=%s)", sendbuf, recvbuf, recvcnts, type_to_string(datatype), op_to_string(op), comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Reduce_scatter(sendbuf, recvbuf, recvcnts, datatype, op, comm);
#else
   int error = PMPI_Reduce_scatter(sendbuf, recvbuf, recvcnts, datatype, op, comm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_REDUCE_SCATTER, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Reduce_scatter failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Register_datarep ( char *name, MPI_Datarep_conversion_function *read_conv_fn, MPI_Datarep_conversion_function *write_conv_fn, MPI_Datarep_extent_function *extent_fn, void *state )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Register_datarep(char *name=%p, MPI_Datarep_conversion_function *read_conv_fn=%p, MPI_Datarep_conversion_function *write_conv_fn=%p, MPI_Datarep_extent_function *extent_fn=%p, void *state=%p)", name, read_conv_fn, write_conv_fn, extent_fn, state);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Register_datarep(name, read_conv_fn, write_conv_fn, extent_fn, state);
#else
   int error = PMPI_Register_datarep(name, read_conv_fn, write_conv_fn, extent_fn, state);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Register_datarep failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Request_free ( MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Request_free(MPI_Request *r=%p)", r);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Request_free(r);
#else
   int error = PMPI_Request_free(r);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Request_free failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Request_get_status ( MPI_Request r, int *flag, MPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Request_get_status(MPI_Request r=%s, int *flag=%p, MPI_Status *status=%p)", request_to_string(r), flag, status);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Request_get_status(r, flag, status);
#else
   int error = PMPI_Request_get_status(r, flag, status);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Request_get_status failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Rsend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Rsend(void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Rsend(buf, count, datatype, dest, tag, comm);
#else
   int error = PMPI_Rsend(buf, count, datatype, dest, tag, comm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_RSEND, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Rsend failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Rsend_init ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Rsend_init(void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s, MPI_Request *r=%p)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Rsend_init(buf, count, datatype, dest, tag, comm, r);
#else
   int error = PMPI_Rsend_init(buf, count, datatype, dest, tag, comm, r);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Rsend_init failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Scan ( void *sendbuf, void *recvbuf, int count, MPI_Datatype datatype, MPI_Op op, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Scan(void *sendbuf=%p, void *recvbuf=%p, int count=%d, MPI_Datatype datatype=%s, MPI_Op op=%s, MPI_Comm comm=%s)", sendbuf, recvbuf, count, type_to_string(datatype), op_to_string(op), comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Scan(sendbuf, recvbuf, count, datatype, op, comm);
#else
   int error = PMPI_Scan(sendbuf, recvbuf, count, datatype, op, comm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_SCAN, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Scan failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Scatter ( void *sendbuf, int sendcnt, MPI_Datatype sendtype, void *recvbuf, int recvcnt, MPI_Datatype recvtype, int root, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Scatter(void *sendbuf=%p, int sendcnt=%d, MPI_Datatype sendtype=%s, void *recvbuf=%p, int recvcnt=%d, MPI_Datatype recvtype=%s, int root=%d, MPI_Comm comm=%s)", sendbuf, sendcnt, type_to_string(sendtype), recvbuf, recvcnt, type_to_string(recvtype), root, comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(sendtype);
   CHECK_TYPE(recvtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Scatter(sendbuf, sendcnt, sendtype, recvbuf, recvcnt, recvtype, root, comm);
#else
   int error = PMPI_Scatter(sendbuf, sendcnt, sendtype, recvbuf, recvcnt, recvtype, root, comm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_SCATTER, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Scatter failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Scatterv ( void *sendbuf, int *sendcnts, int *displs, MPI_Datatype sendtype, void *recvbuf, int recvcnt, MPI_Datatype recvtype, int root, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Scatterv(void *sendbuf=%p, int *sendcnts=%p, int *displs=%p, MPI_Datatype sendtype=%s, void *recvbuf=%p, int recvcnt=%d, MPI_Datatype recvtype=%s, int root=%d, MPI_Comm comm=%s)", sendbuf, sendcnts, displs, type_to_string(sendtype), recvbuf, recvcnt, type_to_string(recvtype), root, comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(sendtype);
   CHECK_TYPE(recvtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Scatterv(sendbuf, sendcnts, displs, sendtype, recvbuf, recvcnt, recvtype, root, comm);
#else
   int error = PMPI_Scatterv(sendbuf, sendcnts, displs, sendtype, recvbuf, recvcnt, recvtype, root, comm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_SCATTER, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Scatterv failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Send ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Send(void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Send(buf, count, datatype, dest, tag, comm);
#else
   int error = PMPI_Send(buf, count, datatype, dest, tag, comm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_SEND, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Send failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Send_init ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Send_init(void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s, MPI_Request *r=%p)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Send_init(buf, count, datatype, dest, tag, comm, r);
#else
   int error = PMPI_Send_init(buf, count, datatype, dest, tag, comm, r);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Send_init failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Sendrecv ( void *sendbuf, int sendcount, MPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, MPI_Datatype recvtype, int source, int recvtag, MPI_Comm comm, MPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Sendrecv(void *sendbuf=%p, int sendcount=%d, MPI_Datatype sendtype=%s, int dest=%d, int sendtag=%d, void *recvbuf=%p, int recvcount=%d, MPI_Datatype recvtype=%s, int source=%d, int recvtag=%d, MPI_Comm comm=%s, MPI_Status *status=%p)", sendbuf, sendcount, type_to_string(sendtype), dest, sendtag, recvbuf, recvcount, type_to_string(recvtype), source, recvtag, comm_to_string(comm), status);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(sendtype);
   CHECK_TYPE(recvtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Sendrecv(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status);
#else
   int error = PMPI_Sendrecv(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_SEND_RECV, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Sendrecv failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Sendrecv_replace ( void *buf, int count, MPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, MPI_Comm comm, MPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Sendrecv_replace(void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int dest=%d, int sendtag=%d, int source=%d, int recvtag=%d, MPI_Comm comm=%s, MPI_Status *status=%p)", buf, count, type_to_string(datatype), dest, sendtag, source, recvtag, comm_to_string(comm), status);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Sendrecv_replace(buf, count, datatype, dest, sendtag, source, recvtag, comm, status);
#else
   int error = PMPI_Sendrecv_replace(buf, count, datatype, dest, sendtag, source, recvtag, comm, status);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_SEND_RECV, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Sendrecv_replace failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Ssend ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Ssend(void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Ssend(buf, count, datatype, dest, tag, comm);
#else
   int error = PMPI_Ssend(buf, count, datatype, dest, tag, comm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_SSEND, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Ssend failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Ssend_init ( void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm, MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Ssend_init(void *buf=%p, int count=%d, MPI_Datatype datatype=%s, int dest=%d, int tag=%d, MPI_Comm comm=%s, MPI_Request *r=%p)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Ssend_init(buf, count, datatype, dest, tag, comm, r);
#else
   int error = PMPI_Ssend_init(buf, count, datatype, dest, tag, comm, r);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Ssend_init failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Startall ( int count, MPI_Request array_of_requests[] )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Startall(int count=%d, MPI_Request array_of_requests[]=%p)", count, array_of_requests);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Startall(count, array_of_requests);
#else
   int error = PMPI_Startall(count, array_of_requests);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Startall failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Start ( MPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Start(MPI_Request *r=%p)", r);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Start(r);
#else
   int error = PMPI_Start(r);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Start failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Status_set_cancelled ( MPI_Status *status, int flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Status_set_cancelled(MPI_Status *status=%p, int flag=%d)", status, flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Status_set_cancelled(status, flag);
#else
   int error = PMPI_Status_set_cancelled(status, flag);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Status_set_cancelled failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Status_set_elements ( MPI_Status *status, MPI_Datatype datatype, int count )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Status_set_elements(MPI_Status *status=%p, MPI_Datatype datatype=%s, int count=%d)", status, type_to_string(datatype), count);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Status_set_elements(status, datatype, count);
#else
   int error = PMPI_Status_set_elements(status, datatype, count);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Status_set_elements failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Testall ( int count, MPI_Request array_of_requests[], int *flag, MPI_Status array_of_statuses[] )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Testall(int count=%d, MPI_Request array_of_requests[]=%p, int *flag=%p, MPI_Status array_of_statuses[]=%p)", count, array_of_requests, flag, array_of_statuses);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Testall(count, array_of_requests, flag, array_of_statuses);
#else
   int error = PMPI_Testall(count, array_of_requests, flag, array_of_statuses);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_TESTALL, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Testall failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Testany ( int count, MPI_Request array_of_requests[], int *index, int *flag, MPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Testany(int count=%d, MPI_Request array_of_requests[]=%p, int *index=%p, int *flag=%p, MPI_Status *status=%p)", count, array_of_requests, index, flag, status);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Testany(count, array_of_requests, index, flag, status);
#else
   int error = PMPI_Testany(count, array_of_requests, index, flag, status);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_TESTANY, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Testany failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Test_cancelled ( MPI_Status *status, int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Test_cancelled(MPI_Status *status=%p, int *flag=%p)", status, flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Test_cancelled(status, flag);
#else
   int error = PMPI_Test_cancelled(status, flag);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Test_cancelled failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Test ( MPI_Request *r, int *flag, MPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Test(MPI_Request *r=%p, int *flag=%p, MPI_Status *status=%p)", r, flag, status);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Test(r, flag, status);
#else
   int error = PMPI_Test(r, flag, status);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_TEST, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Test failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Testsome ( int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[] )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Testsome(int incount=%d, MPI_Request array_of_requests[]=%p, int *outcount=%p, int array_of_indices[]=%p, MPI_Status array_of_statuses[]=%p)", incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Testsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
#else
   int error = PMPI_Testsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_TESTSOME, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Testsome failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Topo_test ( MPI_Comm comm, int *topo_type )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Topo_test(MPI_Comm comm=%s, int *topo_type=%p)", comm_to_string(comm), topo_type);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Topo_test(comm, topo_type);
#else
   int error = PMPI_Topo_test(comm, topo_type);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Topo_test failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_commit ( MPI_Datatype *datatype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_commit(MPI_Datatype *datatype=%p)", datatype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(*datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_commit(datatype);
#else
   int error = PMPI_Type_commit(datatype);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_commit failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_contiguous ( int count, MPI_Datatype old_type, MPI_Datatype *new_type_p )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_contiguous(int count=%d, MPI_Datatype old_type=%s, MPI_Datatype *new_type_p=%p)", count, type_to_string(old_type), new_type_p);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(old_type);
   CHECK_TYPE(*new_type_p);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_contiguous(count, old_type, new_type_p);
#else
   int error = PMPI_Type_contiguous(count, old_type, new_type_p);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_contiguous failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_create_darray ( int size, int rank, int ndims, int array_of_gsizes[], int array_of_distribs[], int array_of_dargs[], int array_of_psizes[], int order, MPI_Datatype oldtype, MPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_create_darray(int size=%d, int rank=%d, int ndims=%d, int array_of_gsizes[]=%p, int array_of_distribs[]=%p, int array_of_dargs[]=%p, int array_of_psizes[]=%p, int order=%d, MPI_Datatype oldtype=%s, MPI_Datatype *newtype=%p)", size, rank, ndims, array_of_gsizes, array_of_distribs, array_of_dargs, array_of_psizes, order, type_to_string(oldtype), newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(oldtype);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_create_darray(size, rank, ndims, array_of_gsizes, array_of_distribs, array_of_dargs, array_of_psizes, order, oldtype, newtype);
#else
   int error = PMPI_Type_create_darray(size, rank, ndims, array_of_gsizes, array_of_distribs, array_of_dargs, array_of_psizes, order, oldtype, newtype);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_create_darray failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_create_hindexed ( int count, int blocklengths[], MPI_Aint displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_create_hindexed(int count=%d, int blocklengths[]=%p, MPI_Aint displacements[]=%p, MPI_Datatype oldtype=%s, MPI_Datatype *newtype=%p)", count, blocklengths, displacements, type_to_string(oldtype), newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(oldtype);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_create_hindexed(count, blocklengths, displacements, oldtype, newtype);
#else
   int error = PMPI_Type_create_hindexed(count, blocklengths, displacements, oldtype, newtype);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_create_hindexed failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_create_hvector ( int count, int blocklength, MPI_Aint stride, MPI_Datatype oldtype, MPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_create_hvector(int count=%d, int blocklength=%d, MPI_Aint stride=%p, MPI_Datatype oldtype=%s, MPI_Datatype *newtype=%p)", count, blocklength, (void *) stride, type_to_string(oldtype), newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(oldtype);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_create_hvector(count, blocklength, stride, oldtype, newtype);
#else
   int error = PMPI_Type_create_hvector(count, blocklength, stride, oldtype, newtype);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_create_hvector failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_create_indexed_block ( int count, int blocklength, int array_of_displacements[], MPI_Datatype oldtype, MPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_create_indexed_block(int count=%d, int blocklength=%d, int array_of_displacements[]=%p, MPI_Datatype oldtype=%s, MPI_Datatype *newtype=%p)", count, blocklength, array_of_displacements, type_to_string(oldtype), newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(oldtype);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_create_indexed_block(count, blocklength, array_of_displacements, oldtype, newtype);
#else
   int error = PMPI_Type_create_indexed_block(count, blocklength, array_of_displacements, oldtype, newtype);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_create_indexed_block failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_create_keyval ( MPI_Type_copy_attr_function *type_copy_attr_fn, MPI_Type_delete_attr_function *type_delete_attr_fn, int *type_keyval, void *extra_state )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_create_keyval(MPI_Type_copy_attr_function *type_copy_attr_fn=%p, MPI_Type_delete_attr_function *type_delete_attr_fn=%p, int *type_keyval=%p, void *extra_state=%p)", type_copy_attr_fn, type_delete_attr_fn, type_keyval, extra_state);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_create_keyval(type_copy_attr_fn, type_delete_attr_fn, type_keyval, extra_state);
#else
   int error = PMPI_Type_create_keyval(type_copy_attr_fn, type_delete_attr_fn, type_keyval, extra_state);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_create_keyval failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_create_resized ( MPI_Datatype oldtype, MPI_Aint lb, MPI_Aint extent, MPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_create_resized(MPI_Datatype oldtype=%s, MPI_Aint lb=%p, MPI_Aint extent=%p, MPI_Datatype *newtype=%p)", type_to_string(oldtype), (void *) lb, (void *) extent, newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(oldtype);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_create_resized(oldtype, lb, extent, newtype);
#else
   int error = PMPI_Type_create_resized(oldtype, lb, extent, newtype);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_create_resized failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_create_struct ( int count, int array_of_blocklengths[], MPI_Aint array_of_displacements[], MPI_Datatype array_of_types[], MPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_create_struct(int count=%d, int array_of_blocklengths[]=%p, MPI_Aint array_of_displacements[]=%p, MPI_Datatype array_of_types[]=%p, MPI_Datatype *newtype=%p)", count, array_of_blocklengths, array_of_displacements, array_of_types, newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(array_of_types[]);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_create_struct(count, array_of_blocklengths, array_of_displacements, array_of_types, newtype);
#else
   int error = PMPI_Type_create_struct(count, array_of_blocklengths, array_of_displacements, array_of_types, newtype);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_create_struct failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_create_subarray ( int ndims, int array_of_sizes[], int array_of_subsizes[], int array_of_starts[], int order, MPI_Datatype oldtype, MPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_create_subarray(int ndims=%d, int array_of_sizes[]=%p, int array_of_subsizes[]=%p, int array_of_starts[]=%p, int order=%d, MPI_Datatype oldtype=%s, MPI_Datatype *newtype=%p)", ndims, array_of_sizes, array_of_subsizes, array_of_starts, order, type_to_string(oldtype), newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(oldtype);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_create_subarray(ndims, array_of_sizes, array_of_subsizes, array_of_starts, order, oldtype, newtype);
#else
   int error = PMPI_Type_create_subarray(ndims, array_of_sizes, array_of_subsizes, array_of_starts, order, oldtype, newtype);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_create_subarray failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_delete_attr ( MPI_Datatype type, int type_keyval )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_delete_attr(MPI_Datatype type=%s, int type_keyval=%d)", type_to_string(type), type_keyval);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(type);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_delete_attr(type, type_keyval);
#else
   int error = PMPI_Type_delete_attr(type, type_keyval);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_delete_attr failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_dup ( MPI_Datatype datatype, MPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_dup(MPI_Datatype datatype=%s, MPI_Datatype *newtype=%p)", type_to_string(datatype), newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_dup(datatype, newtype);
#else
   int error = PMPI_Type_dup(datatype, newtype);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_dup failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_extent ( MPI_Datatype datatype, MPI_Aint *extent )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_extent(MPI_Datatype datatype=%s, MPI_Aint *extent=%p)", type_to_string(datatype), extent);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_extent(datatype, extent);
#else
   int error = PMPI_Type_extent(datatype, extent);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_extent failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_free ( MPI_Datatype *datatype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_free(MPI_Datatype *datatype=%p)", datatype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(*datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_free(datatype);
#else
   int error = PMPI_Type_free(datatype);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_free failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_free_keyval ( int *type_keyval )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_free_keyval(int *type_keyval=%p)", type_keyval);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_free_keyval(type_keyval);
#else
   int error = PMPI_Type_free_keyval(type_keyval);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_free_keyval failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_get_attr ( MPI_Datatype type, int type_keyval, void *attribute_val, int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_get_attr(MPI_Datatype type=%s, int type_keyval=%d, void *attribute_val=%p, int *flag=%p)", type_to_string(type), type_keyval, attribute_val, flag);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(type);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_get_attr(type, type_keyval, attribute_val, flag);
#else
   int error = PMPI_Type_get_attr(type, type_keyval, attribute_val, flag);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_get_attr failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_get_contents ( MPI_Datatype datatype, int max_integers, int max_addresses, int max_datatypes, int array_of_integers[], MPI_Aint array_of_addresses[], MPI_Datatype array_of_datatypes[] )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_get_contents(MPI_Datatype datatype=%s, int max_integers=%d, int max_addresses=%d, int max_datatypes=%d, int array_of_integers[]=%p, MPI_Aint array_of_addresses[]=%p, MPI_Datatype array_of_datatypes[]=%p)", type_to_string(datatype), max_integers, max_addresses, max_datatypes, array_of_integers, array_of_addresses, array_of_datatypes);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
   CHECK_TYPE(array_of_datatypes[]);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_get_contents(datatype, max_integers, max_addresses, max_datatypes, array_of_integers, array_of_addresses, array_of_datatypes);
#else
   int error = PMPI_Type_get_contents(datatype, max_integers, max_addresses, max_datatypes, array_of_integers, array_of_addresses, array_of_datatypes);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_get_contents failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_get_envelope ( MPI_Datatype datatype, int *num_integers, int *num_addresses, int *num_datatypes, int *combiner )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_get_envelope(MPI_Datatype datatype=%s, int *num_integers=%p, int *num_addresses=%p, int *num_datatypes=%p, int *combiner=%p)", type_to_string(datatype), num_integers, num_addresses, num_datatypes, combiner);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_get_envelope(datatype, num_integers, num_addresses, num_datatypes, combiner);
#else
   int error = PMPI_Type_get_envelope(datatype, num_integers, num_addresses, num_datatypes, combiner);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_get_envelope failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_get_extent ( MPI_Datatype datatype, MPI_Aint *lb, MPI_Aint *extent )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_get_extent(MPI_Datatype datatype=%s, MPI_Aint *lb=%p, MPI_Aint *extent=%p)", type_to_string(datatype), lb, extent);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_get_extent(datatype, lb, extent);
#else
   int error = PMPI_Type_get_extent(datatype, lb, extent);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_get_extent failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_get_name ( MPI_Datatype datatype, char *type_name, int *resultlen )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_get_name(MPI_Datatype datatype=%s, char *type_name=%p, int *resultlen=%p)", type_to_string(datatype), type_name, resultlen);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_get_name(datatype, type_name, resultlen);
#else
   int error = PMPI_Type_get_name(datatype, type_name, resultlen);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_get_name failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_get_true_extent ( MPI_Datatype datatype, MPI_Aint *true_lb, MPI_Aint *true_extent )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_get_true_extent(MPI_Datatype datatype=%s, MPI_Aint *true_lb=%p, MPI_Aint *true_extent=%p)", type_to_string(datatype), true_lb, true_extent);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_get_true_extent(datatype, true_lb, true_extent);
#else
   int error = PMPI_Type_get_true_extent(datatype, true_lb, true_extent);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_get_true_extent failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_hindexed ( int count, int blocklens[], MPI_Aint indices[], MPI_Datatype old_type, MPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_hindexed(int count=%d, int blocklens[]=%p, MPI_Aint indices[]=%p, MPI_Datatype old_type=%s, MPI_Datatype *newtype=%p)", count, blocklens, indices, type_to_string(old_type), newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(old_type);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_hindexed(count, blocklens, indices, old_type, newtype);
#else
   int error = PMPI_Type_hindexed(count, blocklens, indices, old_type, newtype);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_hindexed failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_hvector ( int count, int blocklen, MPI_Aint stride, MPI_Datatype old_type, MPI_Datatype *newtype_p )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_hvector(int count=%d, int blocklen=%d, MPI_Aint stride=%p, MPI_Datatype old_type=%s, MPI_Datatype *newtype_p=%p)", count, blocklen, (void *) stride, type_to_string(old_type), newtype_p);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(old_type);
   CHECK_TYPE(*newtype_p);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_hvector(count, blocklen, stride, old_type, newtype_p);
#else
   int error = PMPI_Type_hvector(count, blocklen, stride, old_type, newtype_p);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_hvector failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_indexed ( int count, int blocklens[], int indices[], MPI_Datatype old_type, MPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_indexed(int count=%d, int blocklens[]=%p, int indices[]=%p, MPI_Datatype old_type=%s, MPI_Datatype *newtype=%p)", count, blocklens, indices, type_to_string(old_type), newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(old_type);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_indexed(count, blocklens, indices, old_type, newtype);
#else
   int error = PMPI_Type_indexed(count, blocklens, indices, old_type, newtype);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_indexed failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_lb ( MPI_Datatype datatype, MPI_Aint *displacement )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_lb(MPI_Datatype datatype=%s, MPI_Aint *displacement=%p)", type_to_string(datatype), displacement);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_lb(datatype, displacement);
#else
   int error = PMPI_Type_lb(datatype, displacement);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_lb failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_match_size ( int typeclass, int size, MPI_Datatype *datatype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_match_size(int typeclass=%d, int size=%d, MPI_Datatype *datatype=%p)", typeclass, size, datatype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(*datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_match_size(typeclass, size, datatype);
#else
   int error = PMPI_Type_match_size(typeclass, size, datatype);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_match_size failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_set_attr ( MPI_Datatype type, int type_keyval, void *attribute_val )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_set_attr(MPI_Datatype type=%s, int type_keyval=%d, void *attribute_val=%p)", type_to_string(type), type_keyval, attribute_val);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(type);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_set_attr(type, type_keyval, attribute_val);
#else
   int error = PMPI_Type_set_attr(type, type_keyval, attribute_val);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_set_attr failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_set_name ( MPI_Datatype type, char *type_name )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_set_name(MPI_Datatype type=%s, char *type_name=%p)", type_to_string(type), type_name);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(type);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_set_name(type, type_name);
#else
   int error = PMPI_Type_set_name(type, type_name);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_set_name failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_size ( MPI_Datatype datatype, int *size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_size(MPI_Datatype datatype=%s, int *size=%p)", type_to_string(datatype), size);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_size(datatype, size);
#else
   int error = PMPI_Type_size(datatype, size);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_size failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_struct ( int count, int blocklens[], MPI_Aint indices[], MPI_Datatype old_types[], MPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_struct(int count=%d, int blocklens[]=%p, MPI_Aint indices[]=%p, MPI_Datatype old_types[]=%p, MPI_Datatype *newtype=%p)", count, blocklens, indices, old_types, newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(old_types[]);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_struct(count, blocklens, indices, old_types, newtype);
#else
   int error = PMPI_Type_struct(count, blocklens, indices, old_types, newtype);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_struct failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_ub ( MPI_Datatype datatype, MPI_Aint *displacement )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_ub(MPI_Datatype datatype=%s, MPI_Aint *displacement=%p)", type_to_string(datatype), displacement);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_ub(datatype, displacement);
#else
   int error = PMPI_Type_ub(datatype, displacement);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_ub failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Type_vector ( int count, int blocklength, int stride, MPI_Datatype old_type, MPI_Datatype *newtype_p )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Type_vector(int count=%d, int blocklength=%d, int stride=%d, MPI_Datatype old_type=%s, MPI_Datatype *newtype_p=%p)", count, blocklength, stride, type_to_string(old_type), newtype_p);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(old_type);
   CHECK_TYPE(*newtype_p);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Type_vector(count, blocklength, stride, old_type, newtype_p);
#else
   int error = PMPI_Type_vector(count, blocklength, stride, old_type, newtype_p);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Type_vector failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Unpack_external ( char *datarep, void *inbuf, MPI_Aint insize, MPI_Aint *position, void *outbuf, int outcount, MPI_Datatype datatype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Unpack_external(char *datarep=%p, void *inbuf=%p, MPI_Aint insize=%p, MPI_Aint *position=%p, void *outbuf=%p, int outcount=%d, MPI_Datatype datatype=%s)", datarep, inbuf, (void *) insize, position, outbuf, outcount, type_to_string(datatype));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Unpack_external(datarep, inbuf, insize, position, outbuf, outcount, datatype);
#else
   int error = PMPI_Unpack_external(datarep, inbuf, insize, position, outbuf, outcount, datatype);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Unpack_external failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Unpack ( void *inbuf, int insize, int *position, void *outbuf, int outcount, MPI_Datatype datatype, MPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Unpack(void *inbuf=%p, int insize=%d, int *position=%p, void *outbuf=%p, int outcount=%d, MPI_Datatype datatype=%s, MPI_Comm comm=%s)", inbuf, insize, position, outbuf, outcount, type_to_string(datatype), comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Unpack(inbuf, insize, position, outbuf, outcount, datatype, comm);
#else
   int error = PMPI_Unpack(inbuf, insize, position, outbuf, outcount, datatype, comm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Unpack failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Unpublish_name ( char *service_name, MPI_Info info, char *port_name )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Unpublish_name(char *service_name=%p, MPI_Info info=%s, char *port_name=%p)", service_name, info_to_string(info), port_name);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Unpublish_name(service_name, info, port_name);
#else
   int error = PMPI_Unpublish_name(service_name, info, port_name);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Unpublish_name failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Waitall ( int count, MPI_Request array_of_requests[], MPI_Status array_of_statuses[] )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Waitall(int count=%d, MPI_Request array_of_requests[]=%p, MPI_Status array_of_statuses[]=%p)", count, array_of_requests, array_of_statuses);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
#ifndef IBIS_INTERCEPT
   profile_start = profile_start_ticks();
#endif // IBIS_INTERCEPT
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Waitall(count, array_of_requests, array_of_statuses);
#else
   int error = PMPI_Waitall(count, array_of_requests, array_of_statuses);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
#ifndef IBIS_INTERCEPT
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_WAITALL, profile_end-profile_start);
#endif // IBIS_INTERCEPT
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Waitall failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Waitany ( int count, MPI_Request array_of_requests[], int *index, MPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Waitany(int count=%d, MPI_Request array_of_requests[]=%p, int *index=%p, MPI_Status *status=%p)", count, array_of_requests, index, status);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Waitany(count, array_of_requests, index, status);
#else
   int error = PMPI_Waitany(count, array_of_requests, index, status);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_WAITANY, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Waitany failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Wait ( MPI_Request *r, MPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
   MPI_Comm comm = MPI_COMM_SELF;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Wait(MPI_Request *r=%p, MPI_Status *status=%p)", r, status);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();

#ifdef IBIS_INTERCEPT
   comm = request_get_mpi_comm(*r, MPI_COMM_SELF);
#endif

#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Wait(r, status);
#else
   int error = PMPI_Wait(r, status);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_WAIT, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Wait failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Waitsome ( int incount, MPI_Request array_of_requests[], int *outcount, int array_of_indices[], MPI_Status array_of_statuses[] )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Waitsome(int incount=%d, MPI_Request array_of_requests[]=%p, int *outcount=%p, int array_of_indices[]=%p, MPI_Status array_of_statuses[]=%p)", incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Waitsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
#else
   int error = PMPI_Waitsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_WAITSOME, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Waitsome failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_call_errhandler ( MPI_Win win, int errorcode )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_call_errhandler(MPI_Win win=%s, int errorcode=%d)", win_to_string(win), errorcode);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Win_call_errhandler(win, errorcode);
#else
   int error = PMPI_Win_call_errhandler(win, errorcode);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_call_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_complete ( MPI_Win win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_complete(MPI_Win win=%s)", win_to_string(win));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Win_complete(win);
#else
   int error = PMPI_Win_complete(win);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_complete failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_create_errhandler ( MPI_Win_errhandler_fn *function, MPI_Errhandler *errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_create_errhandler(MPI_Win_errhandler_fn *function=%p, MPI_Errhandler *errhandler=%p)", function, errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Win_create_errhandler(function, errhandler);
#else
   int error = PMPI_Win_create_errhandler(function, errhandler);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_create_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_create ( void *base, MPI_Aint size, int disp_unit, MPI_Info info, MPI_Comm comm, MPI_Win *win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_create(void *base=%p, MPI_Aint size=%p, int disp_unit=%d, MPI_Info info=%s, MPI_Comm comm=%s, MPI_Win *win=%p)", base, (void *) size, disp_unit, info_to_string(info), comm_to_string(comm), win);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Win_create(base, size, disp_unit, info, comm, win);
#else
   int error = PMPI_Win_create(base, size, disp_unit, info, comm, win);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_create_keyval ( MPI_Win_copy_attr_function *win_copy_attr_fn, MPI_Win_delete_attr_function *win_delete_attr_fn, int *win_keyval, void *extra_state )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_create_keyval(MPI_Win_copy_attr_function *win_copy_attr_fn=%p, MPI_Win_delete_attr_function *win_delete_attr_fn=%p, int *win_keyval=%p, void *extra_state=%p)", win_copy_attr_fn, win_delete_attr_fn, win_keyval, extra_state);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Win_create_keyval(win_copy_attr_fn, win_delete_attr_fn, win_keyval, extra_state);
#else
   int error = PMPI_Win_create_keyval(win_copy_attr_fn, win_delete_attr_fn, win_keyval, extra_state);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_create_keyval failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_delete_attr ( MPI_Win win, int win_keyval )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_delete_attr(MPI_Win win=%s, int win_keyval=%d)", win_to_string(win), win_keyval);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Win_delete_attr(win, win_keyval);
#else
   int error = PMPI_Win_delete_attr(win, win_keyval);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_delete_attr failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_fence ( int assert, MPI_Win win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_fence(int assert=%d, MPI_Win win=%s)", assert, win_to_string(win));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Win_fence(assert, win);
#else
   int error = PMPI_Win_fence(assert, win);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_fence failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_free ( MPI_Win *win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_free(MPI_Win *win=%p)", win);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Win_free(win);
#else
   int error = PMPI_Win_free(win);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_free failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_free_keyval ( int *win_keyval )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_free_keyval(int *win_keyval=%p)", win_keyval);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Win_free_keyval(win_keyval);
#else
   int error = PMPI_Win_free_keyval(win_keyval);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_free_keyval failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_get_attr ( MPI_Win win, int win_keyval, void *attribute_val, int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_get_attr(MPI_Win win=%s, int win_keyval=%d, void *attribute_val=%p, int *flag=%p)", win_to_string(win), win_keyval, attribute_val, flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Win_get_attr(win, win_keyval, attribute_val, flag);
#else
   int error = PMPI_Win_get_attr(win, win_keyval, attribute_val, flag);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_get_attr failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_get_errhandler ( MPI_Win win, MPI_Errhandler *errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_get_errhandler(MPI_Win win=%s, MPI_Errhandler *errhandler=%p)", win_to_string(win), errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Win_get_errhandler(win, errhandler);
#else
   int error = PMPI_Win_get_errhandler(win, errhandler);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_get_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_get_group ( MPI_Win win, MPI_Group *g )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_get_group(MPI_Win win=%s, MPI_Group *g=%p)", win_to_string(win), g);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Win_get_group(win, g);
#else
   int error = PMPI_Win_get_group(win, g);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_get_group failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_get_name ( MPI_Win win, char *win_name, int *resultlen )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_get_name(MPI_Win win=%s, char *win_name=%p, int *resultlen=%p)", win_to_string(win), win_name, resultlen);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Win_get_name(win, win_name, resultlen);
#else
   int error = PMPI_Win_get_name(win, win_name, resultlen);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_get_name failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_lock ( int lock_type, int rank, int assert, MPI_Win win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_lock(int lock_type=%d, int rank=%d, int assert=%d, MPI_Win win=%s)", lock_type, rank, assert, win_to_string(win));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Win_lock(lock_type, rank, assert, win);
#else
   int error = PMPI_Win_lock(lock_type, rank, assert, win);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_lock failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_post ( MPI_Group g, int assert, MPI_Win win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_post(MPI_Group g=%s, int assert=%d, MPI_Win win=%s)", group_to_string(g), assert, win_to_string(win));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Win_post(g, assert, win);
#else
   int error = PMPI_Win_post(g, assert, win);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_post failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_set_attr ( MPI_Win win, int win_keyval, void *attribute_val )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_set_attr(MPI_Win win=%s, int win_keyval=%d, void *attribute_val=%p)", win_to_string(win), win_keyval, attribute_val);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Win_set_attr(win, win_keyval, attribute_val);
#else
   int error = PMPI_Win_set_attr(win, win_keyval, attribute_val);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_set_attr failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_set_errhandler ( MPI_Win win, MPI_Errhandler errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_set_errhandler(MPI_Win win=%s, MPI_Errhandler errhandler=%p)", win_to_string(win), (void *) errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Win_set_errhandler(win, errhandler);
#else
   int error = PMPI_Win_set_errhandler(win, errhandler);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_set_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_set_name ( MPI_Win win, char *win_name )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_set_name(MPI_Win win=%s, char *win_name=%p)", win_to_string(win), win_name);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Win_set_name(win, win_name);
#else
   int error = PMPI_Win_set_name(win, win_name);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_set_name failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_start ( MPI_Group g, int assert, MPI_Win win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_start(MPI_Group g=%s, int assert=%d, MPI_Win win=%s)", group_to_string(g), assert, win_to_string(win));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Win_start(g, assert, win);
#else
   int error = PMPI_Win_start(g, assert, win);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_start failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_test ( MPI_Win win, int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_test(MPI_Win win=%s, int *flag=%p)", win_to_string(win), flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Win_test(win, flag);
#else
   int error = PMPI_Win_test(win, flag);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_test failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_unlock ( int rank, MPI_Win win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_unlock(int rank=%d, MPI_Win win=%s)", rank, win_to_string(win));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Win_unlock(rank, win);
#else
   int error = PMPI_Win_unlock(rank, win);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_unlock failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int MPI_Win_wait ( MPI_Win win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Win_wait(MPI_Win win=%s)", win_to_string(win));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Win_wait(win);
#else
   int error = PMPI_Win_wait(win);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(MPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Win_wait failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

/*
int MPI_Group_comm_create ( MPI_Comm old_comm, MPI_Group g, int tag, MPI_Comm *new_comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "MPI_Group_comm_create(MPI_Comm old_comm=%s, MPI_Group g=%s, int tag=%d, MPI_Comm *new_comm=%p)", comm_to_string(old_comm), group_to_string(g), tag, new_comm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

#ifdef IBIS_INTERCEPT
   int error = IMPI_Group_comm_create(old_comm, g, tag, new_comm);
#else
   int error = PMPI_Group_comm_create(old_comm, g, tag, new_comm);
#endif // IBIS_INTERCEPT

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(old_comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != MPI_SUCCESS) {
      ERROR(0, "MPI_Group_comm_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}
*/

MPI_Comm MPI_Comm_f2c ( MPI_Fint comm )
{
#ifdef IBIS_INTERCEPT
   return IMPI_Comm_f2c(comm);
#else
   return PMPI_Comm_f2c(comm);
#endif // IBIS_INTERCEPT

}


MPI_Group MPI_Group_f2c ( MPI_Fint g )
{
#ifdef IBIS_INTERCEPT
   return IMPI_Group_f2c(g);
#else
   return PMPI_Group_f2c(g);
#endif // IBIS_INTERCEPT

}


MPI_Request MPI_Request_f2c ( MPI_Fint r )
{
#ifdef IBIS_INTERCEPT
   return IMPI_Request_f2c(r);
#else
   return PMPI_Request_f2c(r);
#endif // IBIS_INTERCEPT

}


MPI_Info MPI_Info_f2c ( MPI_Fint info )
{
#ifdef IBIS_INTERCEPT
   return IMPI_Info_f2c(info);
#else
   return PMPI_Info_f2c(info);
#endif // IBIS_INTERCEPT

}


MPI_File MPI_File_f2c ( MPI_Fint file )
{
#ifdef IBIS_INTERCEPT
   return IMPI_File_f2c(file);
#else
   return PMPI_File_f2c(file);
#endif // IBIS_INTERCEPT

}


MPI_Op MPI_Op_f2c ( MPI_Fint op )
{
#ifdef IBIS_INTERCEPT
   return IMPI_Op_f2c(op);
#else
   return PMPI_Op_f2c(op);
#endif // IBIS_INTERCEPT

}


MPI_Win MPI_Win_f2c ( MPI_Fint Win )
{
#ifdef IBIS_INTERCEPT
   return IMPI_Win_f2c(Win);
#else
   return PMPI_Win_f2c(Win);
#endif // IBIS_INTERCEPT

}


MPI_Errhandler MPI_Errhandler_f2c ( MPI_Fint Errhandler )
{
#ifdef IBIS_INTERCEPT
   return IMPI_Errhandler_f2c(Errhandler);
#else
   return PMPI_Errhandler_f2c(Errhandler);
#endif // IBIS_INTERCEPT

}


MPI_Datatype MPI_Type_f2c ( MPI_Fint Type )
{
#ifdef IBIS_INTERCEPT
   return IMPI_Type_f2c(Type);
#else
   return PMPI_Type_f2c(Type);
#endif // IBIS_INTERCEPT

}


MPI_Fint MPI_Comm_c2f ( MPI_Comm comm )
{
#ifdef IBIS_INTERCEPT
   return IMPI_Comm_c2f(comm);
#else
   return PMPI_Comm_c2f(comm);
#endif // IBIS_INTERCEPT

}


MPI_Fint MPI_Group_c2f ( MPI_Group g )
{
#ifdef IBIS_INTERCEPT
   return IMPI_Group_c2f(g);
#else
   return PMPI_Group_c2f(g);
#endif // IBIS_INTERCEPT

}


MPI_Fint MPI_Request_c2f ( MPI_Request r )
{
#ifdef IBIS_INTERCEPT
   return IMPI_Request_c2f(r);
#else
   return PMPI_Request_c2f(r);
#endif // IBIS_INTERCEPT

}


MPI_Fint MPI_Info_c2f ( MPI_Info info )
{
#ifdef IBIS_INTERCEPT
   return IMPI_Info_c2f(info);
#else
   return PMPI_Info_c2f(info);
#endif // IBIS_INTERCEPT

}


MPI_Fint MPI_File_c2f ( MPI_File file )
{
#ifdef IBIS_INTERCEPT
   return IMPI_File_c2f(file);
#else
   return PMPI_File_c2f(file);
#endif // IBIS_INTERCEPT

}


MPI_Fint MPI_Op_c2f ( MPI_Op op )
{
#ifdef IBIS_INTERCEPT
   return IMPI_Op_c2f(op);
#else
   return PMPI_Op_c2f(op);
#endif // IBIS_INTERCEPT

}


MPI_Fint MPI_Win_c2f ( MPI_Win Win )
{
#ifdef IBIS_INTERCEPT
   return IMPI_Win_c2f(Win);
#else
   return PMPI_Win_c2f(Win);
#endif // IBIS_INTERCEPT

}


MPI_Fint MPI_Errhandler_c2f ( MPI_Errhandler Errhandler )
{
#ifdef IBIS_INTERCEPT
   return IMPI_Errhandler_c2f(Errhandler);
#else
   return PMPI_Errhandler_c2f(Errhandler);
#endif // IBIS_INTERCEPT

}


MPI_Fint MPI_Type_c2f ( MPI_Datatype Type )
{
#ifdef IBIS_INTERCEPT
   return IMPI_Type_c2f(Type);
#else
   return PMPI_Type_c2f(Type);
#endif // IBIS_INTERCEPT

}


#endif // ENABLE_INTERCEPT

