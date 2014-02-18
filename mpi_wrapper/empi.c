#include "flags.h"
#include "profiling.h"
#include "logging.h"
#include "debugging.h"
//#include "generated_header.h"
#include "empi.h"
#include "request.h"

int EMPI_Init ( int *argc, char ***argv )
{
   init_debug();

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Init(int *argc=%p, char ***argv=%p)", argc, argv);
#endif // TRACE_CALLS

   int error = xEMPI_Init(argc, argv);

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Init failed (%d)!", error);
   }
#endif // TRACE_ERRORS

   profile_init();

   return error;
}

int EMPI_Abort ( EMPI_Comm comm, int errorcode )
{
#ifdef TRACE_CALLS
   INFO(0, "EMPI_Abort(EMPI_Comm comm=%s, int errorcode=%d)", comm_to_string(comm), errorcode);
#endif // TRACE_CALLS

   int error = xEMPI_Abort(comm, errorcode);

#if PROFILE_LEVEL > 0
   profile_finalize();
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Abort failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int EMPI_Finalize ( void )
{
#ifdef TRACE_CALLS
   INFO(0, "EMPI_Finalize()");
#endif // TRACE_CALLS

   int error = xEMPI_Finalize();

#if PROFILE_LEVEL > 0
   profile_finalize();
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Finalize failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int EMPI_Finalized ( int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Finalized(int *flag=%p)", flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Finalized(flag);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Finalized failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

double EMPI_Wtime ( void )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Wtime( void )");
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   double result = xEMPI_Wtime();

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

   return result;
}

#if 0


int EMPI_Type_create_f90_complex ( int p, int r, EMPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_create_f90_complex(int p=%d, int r=%d, EMPI_Datatype *newtype=%p)", p, r, newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_create_f90_complex(p, r, newtype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_create_f90_complex failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_create_f90_integer ( int r, EMPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_create_f90_integer(int r=%d, EMPI_Datatype *newtype=%p)", r, newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_create_f90_integer(r, newtype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_create_f90_integer failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_create_f90_real ( int p, int r, EMPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_create_f90_real(int p=%d, int r=%d, EMPI_Datatype *newtype=%p)", p, r, newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_create_f90_real(p, r, newtype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_create_f90_real failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}




int EMPI_Accumulate ( void *origin_addr, int origin_count, EMPI_Datatype origin_datatype, int target_rank, EMPI_Aint target_disp, int target_count, EMPI_Datatype target_datatype, EMPI_Op op, EMPI_Win win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Accumulate(void *origin_addr=%p, int origin_count=%d, EMPI_Datatype origin_datatype=%s, int target_rank=%d, EMPI_Aint target_disp=%p, int target_count=%d, EMPI_Datatype target_datatype=%s, EMPI_Op op=%s, EMPI_Win win=%s)", origin_addr, origin_count, type_to_string(origin_datatype), target_rank, (void *) target_disp, target_count, type_to_string(target_datatype), op_to_string(op), win_to_string(win));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(origin_datatype);
   CHECK_TYPE(target_datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Accumulate(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, op, win);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Accumulate failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Add_error_class ( int *errorclass )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Add_error_class(int *errorclass=%p)", errorclass);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Add_error_class(errorclass);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Add_error_class failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Add_error_code ( int errorclass, int *errorcode )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Add_error_code(int errorclass=%d, int *errorcode=%p)", errorclass, errorcode);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Add_error_code(errorclass, errorcode);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Add_error_code failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Add_error_string ( int errorcode, char *string )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Add_error_string(int errorcode=%d, char *string=%p)", errorcode, string);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Add_error_string(errorcode, string);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Add_error_string failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Address ( void *location, EMPI_Aint *address )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Address(void *location=%p, EMPI_Aint *address=%p)", location, address);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Address(location, address);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Address failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Allgather ( void *sendbuf, int sendcount, EMPI_Datatype sendtype, void *recvbuf, int recvcount, EMPI_Datatype recvtype, EMPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Allgather(void *sendbuf=%p, int sendcount=%d, EMPI_Datatype sendtype=%s, void *recvbuf=%p, int recvcount=%d, EMPI_Datatype recvtype=%s, EMPI_Comm comm=%s)", sendbuf, sendcount, type_to_string(sendtype), recvbuf, recvcount, type_to_string(recvtype), comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(sendtype);
   CHECK_TYPE(recvtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Allgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_ALLGATHER, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Allgather failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Allgatherv ( void *sendbuf, int sendcount, EMPI_Datatype sendtype, void *recvbuf, int *recvcounts, int *displs, EMPI_Datatype recvtype, EMPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Allgatherv(void *sendbuf=%p, int sendcount=%d, EMPI_Datatype sendtype=%s, void *recvbuf=%p, int *recvcounts=%p, int *displs=%p, EMPI_Datatype recvtype=%s, EMPI_Comm comm=%s)", sendbuf, sendcount, type_to_string(sendtype), recvbuf, recvcounts, displs, type_to_string(recvtype), comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(sendtype);
   CHECK_TYPE(recvtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Allgatherv(sendbuf, sendcount, sendtype, recvbuf, recvcounts, displs, recvtype, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_ALLGATHER, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Allgatherv failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Alloc_mem ( EMPI_Aint size, EMPI_Info info, void *baseptr )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Alloc_mem(EMPI_Aint size=%p, EMPI_Info info=%s, void *baseptr=%p)", (void *) size, info_to_string(info), baseptr);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Alloc_mem(size, info, baseptr);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Alloc_mem failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Allreduce ( void *sendbuf, void *recvbuf, int count, EMPI_Datatype datatype, EMPI_Op op, EMPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Allreduce(void *sendbuf=%p, void *recvbuf=%p, int count=%d, EMPI_Datatype datatype=%s, EMPI_Op op=%s, EMPI_Comm comm=%s)", sendbuf, recvbuf, count, type_to_string(datatype), op_to_string(op), comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Allreduce(sendbuf, recvbuf, count, datatype, op, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_ALLREDUCE, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Allreduce failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Alltoall ( void *sendbuf, int sendcount, EMPI_Datatype sendtype, void *recvbuf, int recvcount, EMPI_Datatype recvtype, EMPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Alltoall(void *sendbuf=%p, int sendcount=%d, EMPI_Datatype sendtype=%s, void *recvbuf=%p, int recvcount=%d, EMPI_Datatype recvtype=%s, EMPI_Comm comm=%s)", sendbuf, sendcount, type_to_string(sendtype), recvbuf, recvcount, type_to_string(recvtype), comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(sendtype);
   CHECK_TYPE(recvtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Alltoall(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_ALLTOALL, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Alltoall failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Alltoallv ( void *sendbuf, int *sendcnts, int *sdispls, EMPI_Datatype sendtype, void *recvbuf, int *recvcnts, int *rdispls, EMPI_Datatype recvtype, EMPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Alltoallv(void *sendbuf=%p, int *sendcnts=%p, int *sdispls=%p, EMPI_Datatype sendtype=%s, void *recvbuf=%p, int *recvcnts=%p, int *rdispls=%p, EMPI_Datatype recvtype=%s, EMPI_Comm comm=%s)", sendbuf, sendcnts, sdispls, type_to_string(sendtype), recvbuf, recvcnts, rdispls, type_to_string(recvtype), comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(sendtype);
   CHECK_TYPE(recvtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Alltoallv(sendbuf, sendcnts, sdispls, sendtype, recvbuf, recvcnts, rdispls, recvtype, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_ALLTOALL, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Alltoallv failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Alltoallw ( void *sendbuf, int *sendcnts, int *sdispls, EMPI_Datatype *sendtypes, void *recvbuf, int *recvcnts, int *rdispls, EMPI_Datatype *recvtypes, EMPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Alltoallw(void *sendbuf=%p, int *sendcnts=%p, int *sdispls=%p, EMPI_Datatype *sendtypes=%p, void *recvbuf=%p, int *recvcnts=%p, int *rdispls=%p, EMPI_Datatype *recvtypes=%p, EMPI_Comm comm=%s)", sendbuf, sendcnts, sdispls, sendtypes, recvbuf, recvcnts, rdispls, recvtypes, comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(*sendtypes);
   CHECK_TYPE(*recvtypes);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Alltoallw(sendbuf, sendcnts, sdispls, sendtypes, recvbuf, recvcnts, rdispls, recvtypes, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_ALLTOALL, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Alltoallw failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Attr_delete ( EMPI_Comm comm, int keyval )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Attr_delete(EMPI_Comm comm=%s, int keyval=%d)", comm_to_string(comm), keyval);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Attr_delete(comm, keyval);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Attr_delete failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Attr_get ( EMPI_Comm comm, int keyval, void *attr_value, int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Attr_get(EMPI_Comm comm=%s, int keyval=%d, void *attr_value=%p, int *flag=%p)", comm_to_string(comm), keyval, attr_value, flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Attr_get(comm, keyval, attr_value, flag);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Attr_get failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Attr_put ( EMPI_Comm comm, int keyval, void *attr_value )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Attr_put(EMPI_Comm comm=%s, int keyval=%d, void *attr_value=%p)", comm_to_string(comm), keyval, attr_value);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Attr_put(comm, keyval, attr_value);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Attr_put failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Barrier ( EMPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Barrier(EMPI_Comm comm=%s)", comm_to_string(comm));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Barrier(comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_BARRIER, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Barrier failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Bcast ( void *buffer, int count, EMPI_Datatype datatype, int root, EMPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Bcast(void *buffer=%p, int count=%d, EMPI_Datatype datatype=%s, int root=%d, EMPI_Comm comm=%s)", buffer, count, type_to_string(datatype), root, comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Bcast(buffer, count, datatype, root, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_BCAST, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Bcast failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Bsend ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Bsend(void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, int dest=%d, int tag=%d, EMPI_Comm comm=%s)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Bsend(buf, count, datatype, dest, tag, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_BSEND, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Bsend failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Bsend_init ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm, EMPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Bsend_init(void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, int dest=%d, int tag=%d, EMPI_Comm comm=%s, EMPI_Request *r=%p)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Bsend_init(buf, count, datatype, dest, tag, comm, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Bsend_init failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Buffer_attach ( void *buffer, int size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Buffer_attach(void *buffer=%p, int size=%d)", buffer, size);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Buffer_attach(buffer, size);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Buffer_attach failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Buffer_detach ( void *buffer, int *size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Buffer_detach(void *buffer=%p, int *size=%p)", buffer, size);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Buffer_detach(buffer, size);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Buffer_detach failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Cancel ( EMPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Cancel(EMPI_Request *r=%p)", r);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Cancel(r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Cancel failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Cart_coords ( EMPI_Comm comm, int rank, int maxdims, int *coords )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Cart_coords(EMPI_Comm comm=%s, int rank=%d, int maxdims=%d, int *coords=%p)", comm_to_string(comm), rank, maxdims, coords);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Cart_coords(comm, rank, maxdims, coords);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Cart_coords failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Cart_create ( EMPI_Comm comm_old, int ndims, int *dims, int *periods, int reorder, EMPI_Comm *comm_cart )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Cart_create(EMPI_Comm comm_old=%s, int ndims=%d, int *dims=%p, int *periods=%p, int reorder=%d, EMPI_Comm *comm_cart=%p)", comm_to_string(comm_old), ndims, dims, periods, reorder, comm_cart);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Cart_create(comm_old, ndims, dims, periods, reorder, comm_cart);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm_old, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Cart_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Cartdim_get ( EMPI_Comm comm, int *ndims )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Cartdim_get(EMPI_Comm comm=%s, int *ndims=%p)", comm_to_string(comm), ndims);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Cartdim_get(comm, ndims);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Cartdim_get failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Cart_get ( EMPI_Comm comm, int maxdims, int *dims, int *periods, int *coords )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Cart_get(EMPI_Comm comm=%s, int maxdims=%d, int *dims=%p, int *periods=%p, int *coords=%p)", comm_to_string(comm), maxdims, dims, periods, coords);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Cart_get(comm, maxdims, dims, periods, coords);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Cart_get failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Cart_map ( EMPI_Comm comm_old, int ndims, int *dims, int *periods, int *newrank )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Cart_map(EMPI_Comm comm_old=%s, int ndims=%d, int *dims=%p, int *periods=%p, int *newrank=%p)", comm_to_string(comm_old), ndims, dims, periods, newrank);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Cart_map(comm_old, ndims, dims, periods, newrank);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm_old, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Cart_map failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Cart_rank ( EMPI_Comm comm, int *coords, int *rank )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Cart_rank(EMPI_Comm comm=%s, int *coords=%p, int *rank=%p)", comm_to_string(comm), coords, rank);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Cart_rank(comm, coords, rank);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Cart_rank failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Cart_shift ( EMPI_Comm comm, int direction, int displ, int *source, int *dest )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Cart_shift(EMPI_Comm comm=%s, int direction=%d, int displ=%d, int *source=%p, int *dest=%p)", comm_to_string(comm), direction, displ, source, dest);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Cart_shift(comm, direction, displ, source, dest);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Cart_shift failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Cart_sub ( EMPI_Comm comm, int *remain_dims, EMPI_Comm *comm_new )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Cart_sub(EMPI_Comm comm=%s, int *remain_dims=%p, EMPI_Comm *comm_new=%p)", comm_to_string(comm), remain_dims, comm_new);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Cart_sub(comm, remain_dims, comm_new);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Cart_sub failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Close_port ( char *port_name )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Close_port(char *port_name=%p)", port_name);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Close_port(port_name);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Close_port failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Comm_accept ( char *port_name, EMPI_Info info, int root, EMPI_Comm comm, EMPI_Comm *newcomm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Comm_accept(char *port_name=%p, EMPI_Info info=%s, int root=%d, EMPI_Comm comm=%s, EMPI_Comm *newcomm=%p)", port_name, info_to_string(info), root, comm_to_string(comm), newcomm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Comm_accept(port_name, info, root, comm, newcomm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Comm_accept failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Comm_call_errhandler ( EMPI_Comm comm, int errorcode )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Comm_call_errhandler(EMPI_Comm comm=%s, int errorcode=%d)", comm_to_string(comm), errorcode);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Comm_call_errhandler(comm, errorcode);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Comm_call_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Comm_compare ( EMPI_Comm comm1, EMPI_Comm comm2, int *result )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Comm_compare(EMPI_Comm comm1=%s, EMPI_Comm comm2=%s, int *result=%p)", comm_to_string(comm1), comm_to_string(comm2), result);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Comm_compare(comm1, comm2, result);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm1, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Comm_compare failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Comm_connect ( char *port_name, EMPI_Info info, int root, EMPI_Comm comm, EMPI_Comm *newcomm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Comm_connect(char *port_name=%p, EMPI_Info info=%s, int root=%d, EMPI_Comm comm=%s, EMPI_Comm *newcomm=%p)", port_name, info_to_string(info), root, comm_to_string(comm), newcomm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Comm_connect(port_name, info, root, comm, newcomm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Comm_connect failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Comm_create_errhandler ( EMPI_Comm_errhandler_fn *function, EMPI_Errhandler *errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Comm_create_errhandler(EMPI_Comm_errhandler_fn *function=%p, EMPI_Errhandler *errhandler=%p)", function, errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Comm_create_errhandler(function, errhandler);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Comm_create_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Comm_create ( EMPI_Comm comm, EMPI_Group g, EMPI_Comm *newcomm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Comm_create(EMPI_Comm comm=%s, EMPI_Group g=%s, EMPI_Comm *newcomm=%p)", comm_to_string(comm), group_to_string(g), newcomm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Comm_create(comm, g, newcomm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Comm_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Comm_create_keyval ( EMPI_Comm_copy_attr_function *comm_copy_attr_fn, EMPI_Comm_delete_attr_function *comm_delete_attr_fn, int *comm_keyval, void *extra_state )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Comm_create_keyval(EMPI_Comm_copy_attr_function *comm_copy_attr_fn=%p, EMPI_Comm_delete_attr_function *comm_delete_attr_fn=%p, int *comm_keyval=%p, void *extra_state=%p)", comm_copy_attr_fn, comm_delete_attr_fn, comm_keyval, extra_state);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Comm_create_keyval(comm_copy_attr_fn, comm_delete_attr_fn, comm_keyval, extra_state);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Comm_create_keyval failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Comm_delete_attr ( EMPI_Comm comm, int comm_keyval )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Comm_delete_attr(EMPI_Comm comm=%s, int comm_keyval=%d)", comm_to_string(comm), comm_keyval);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Comm_delete_attr(comm, comm_keyval);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Comm_delete_attr failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Comm_disconnect ( EMPI_Comm *comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Comm_disconnect(EMPI_Comm *comm=%p)", comm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Comm_disconnect(comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Comm_disconnect failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Comm_dup ( EMPI_Comm comm, EMPI_Comm *newcomm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Comm_dup(EMPI_Comm comm=%s, EMPI_Comm *newcomm=%p)", comm_to_string(comm), newcomm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Comm_dup(comm, newcomm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Comm_dup failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Comm_free ( EMPI_Comm *comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Comm_free(EMPI_Comm *comm=%p)", comm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Comm_free(comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Comm_free failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Comm_free_keyval ( int *comm_keyval )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Comm_free_keyval(int *comm_keyval=%p)", comm_keyval);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Comm_free_keyval(comm_keyval);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Comm_free_keyval failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Comm_get_attr ( EMPI_Comm comm, int comm_keyval, void *attribute_val, int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Comm_get_attr(EMPI_Comm comm=%s, int comm_keyval=%d, void *attribute_val=%p, int *flag=%p)", comm_to_string(comm), comm_keyval, attribute_val, flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Comm_get_attr(comm, comm_keyval, attribute_val, flag);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Comm_get_attr failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Comm_get_errhandler ( EMPI_Comm comm, EMPI_Errhandler *errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Comm_get_errhandler(EMPI_Comm comm=%s, EMPI_Errhandler *errhandler=%p)", comm_to_string(comm), errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Comm_get_errhandler(comm, errhandler);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Comm_get_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Comm_get_name ( EMPI_Comm comm, char *comm_name, int *resultlen )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Comm_get_name(EMPI_Comm comm=%s, char *comm_name=%p, int *resultlen=%p)", comm_to_string(comm), comm_name, resultlen);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Comm_get_name(comm, comm_name, resultlen);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Comm_get_name failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Comm_get_parent ( EMPI_Comm *parent )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Comm_get_parent(EMPI_Comm *parent=%p)", parent);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Comm_get_parent(parent);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Comm_get_parent failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Comm_group ( EMPI_Comm comm, EMPI_Group *g )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Comm_group(EMPI_Comm comm=%s, EMPI_Group *g=%p)", comm_to_string(comm), g);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Comm_group(comm, g);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Comm_group failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Comm_join ( int fd, EMPI_Comm *intercomm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Comm_join(int fd=%d, EMPI_Comm *intercomm=%p)", fd, intercomm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Comm_join(fd, intercomm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Comm_join failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Comm_rank ( EMPI_Comm comm, int *rank )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Comm_rank(EMPI_Comm comm=%s, int *rank=%p)", comm_to_string(comm), rank);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Comm_rank(comm, rank);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Comm_rank failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Comm_remote_group ( EMPI_Comm comm, EMPI_Group *g )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Comm_remote_group(EMPI_Comm comm=%s, EMPI_Group *g=%p)", comm_to_string(comm), g);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Comm_remote_group(comm, g);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Comm_remote_group failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Comm_remote_size ( EMPI_Comm comm, int *size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Comm_remote_size(EMPI_Comm comm=%s, int *size=%p)", comm_to_string(comm), size);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Comm_remote_size(comm, size);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Comm_remote_size failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Comm_set_attr ( EMPI_Comm comm, int comm_keyval, void *attribute_val )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Comm_set_attr(EMPI_Comm comm=%s, int comm_keyval=%d, void *attribute_val=%p)", comm_to_string(comm), comm_keyval, attribute_val);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Comm_set_attr(comm, comm_keyval, attribute_val);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Comm_set_attr failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Comm_set_errhandler ( EMPI_Comm comm, EMPI_Errhandler errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Comm_set_errhandler(EMPI_Comm comm=%s, EMPI_Errhandler errhandler=%p)", comm_to_string(comm), (void *) errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Comm_set_errhandler(comm, errhandler);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Comm_set_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Comm_set_name ( EMPI_Comm comm, char *comm_name )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Comm_set_name(EMPI_Comm comm=%s, char *comm_name=%p)", comm_to_string(comm), comm_name);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Comm_set_name(comm, comm_name);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Comm_set_name failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Comm_size ( EMPI_Comm comm, int *size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Comm_size(EMPI_Comm comm=%s, int *size=%p)", comm_to_string(comm), size);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Comm_size(comm, size);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Comm_size failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Comm_spawn ( char *command, char *argv[], int maxprocs, EMPI_Info info, int root, EMPI_Comm comm, EMPI_Comm *intercomm, int array_of_errcodes[] )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Comm_spawn(char *command=%p, char *argv[]=%p, int maxprocs=%d, EMPI_Info info=%s, int root=%d, EMPI_Comm comm=%s, EMPI_Comm *intercomm=%p, int array_of_errcodes[]=%p)", command, argv, maxprocs, info_to_string(info), root, comm_to_string(comm), intercomm, array_of_errcodes);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Comm_spawn(command, argv, maxprocs, info, root, comm, intercomm, array_of_errcodes);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Comm_spawn failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Comm_spawn_multiple ( int count, char *array_of_commands[], char* *array_of_argv[], int array_of_maxprocs[], EMPI_Info array_of_info[], int root, EMPI_Comm comm, EMPI_Comm *intercomm, int array_of_errcodes[] )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Comm_spawn_multiple(int count=%d, char *array_of_commands[]=%p, char* *array_of_argv[]=%p, int array_of_maxprocs[]=%p, EMPI_Info array_of_info[]=%p, int root=%d, EMPI_Comm comm=%s, EMPI_Comm *intercomm=%p, int array_of_errcodes[]=%p)", count, array_of_commands, array_of_argv, array_of_maxprocs, array_of_info, root, comm_to_string(comm), intercomm, array_of_errcodes);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Comm_spawn_multiple(count, array_of_commands, array_of_argv, array_of_maxprocs, array_of_info, root, comm, intercomm, array_of_errcodes);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Comm_spawn_multiple failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Comm_split ( EMPI_Comm comm, int color, int key, EMPI_Comm *newcomm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Comm_split(EMPI_Comm comm=%s, int color=%d, int key=%d, EMPI_Comm *newcomm=%p)", comm_to_string(comm), color, key, newcomm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Comm_split(comm, color, key, newcomm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Comm_split failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Comm_test_inter ( EMPI_Comm comm, int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Comm_test_inter(EMPI_Comm comm=%s, int *flag=%p)", comm_to_string(comm), flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Comm_test_inter(comm, flag);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Comm_test_inter failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Dims_create ( int nnodes, int ndims, int *dims )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Dims_create(int nnodes=%d, int ndims=%d, int *dims=%p)", nnodes, ndims, dims);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Dims_create(nnodes, ndims, dims);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Dims_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

/*
int EMPI_Dist_graph_create_adjacent ( EMPI_Comm comm_old, int indegree, int sources[], int sourceweights[], int outdegree, int destinations[], int destweights[], EMPI_Info info, int reorder, EMPI_Comm *comm_dist_graph )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Dist_graph_create_adjacent(EMPI_Comm comm_old=%s, int indegree=%d, int sources[]=%p, int sourceweights[]=%p, int outdegree=%d, int destinations[]=%p, int destweights[]=%p, EMPI_Info info=%s, int reorder=%d, EMPI_Comm *comm_dist_graph=%p)", comm_to_string(comm_old), indegree, sources, sourceweights, outdegree, destinations, destweights, info_to_string(info), reorder, comm_dist_graph);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Dist_graph_create_adjacent(comm_old, indegree, sources, sourceweights, outdegree, destinations, destweights, info, reorder, comm_dist_graph);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm_old, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Dist_graph_create_adjacent failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}
*/
/*
int EMPI_Dist_graph_create ( EMPI_Comm comm_old, int n, int sources[], int degrees[], int destinations[], int weights[], EMPI_Info info, int reorder, EMPI_Comm *comm_dist_graph )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Dist_graph_create(EMPI_Comm comm_old=%s, int n=%d, int sources[]=%p, int degrees[]=%p, int destinations[]=%p, int weights[]=%p, EMPI_Info info=%s, int reorder=%d, EMPI_Comm *comm_dist_graph=%p)", comm_to_string(comm_old), n, sources, degrees, destinations, weights, info_to_string(info), reorder, comm_dist_graph);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Dist_graph_create(comm_old, n, sources, degrees, destinations, weights, info, reorder, comm_dist_graph);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm_old, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Dist_graph_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}
*/
/*
int EMPI_Dist_graph_neighbors_count ( EMPI_Comm comm, int *indegree, int *outdegree, int *weighted )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Dist_graph_neighbors_count(EMPI_Comm comm=%s, int *indegree=%p, int *outdegree=%p, int *weighted=%p)", comm_to_string(comm), indegree, outdegree, weighted);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Dist_graph_neighbors_count(comm, indegree, outdegree, weighted);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Dist_graph_neighbors_count failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}
*/
/*
int EMPI_Dist_graph_neighbors ( EMPI_Comm comm, int maxindegree, int sources[], int sourceweights[], int maxoutdegree, int destinations[], int destweights[] )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Dist_graph_neighbors(EMPI_Comm comm=%s, int maxindegree=%d, int sources[]=%p, int sourceweights[]=%p, int maxoutdegree=%d, int destinations[]=%p, int destweights[]=%p)", comm_to_string(comm), maxindegree, sources, sourceweights, maxoutdegree, destinations, destweights);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Dist_graph_neighbors(comm, maxindegree, sources, sourceweights, maxoutdegree, destinations, destweights);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Dist_graph_neighbors failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}
*/

int EMPI_Errhandler_create ( EMPI_Handler_function *function, EMPI_Errhandler *errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Errhandler_create(EMPI_Handler_function *function=%p, EMPI_Errhandler *errhandler=%p)", function, errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Errhandler_create(function, errhandler);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Errhandler_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Errhandler_free ( EMPI_Errhandler *errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Errhandler_free(EMPI_Errhandler *errhandler=%p)", errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Errhandler_free(errhandler);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Errhandler_free failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Errhandler_get ( EMPI_Comm comm, EMPI_Errhandler *errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Errhandler_get(EMPI_Comm comm=%s, EMPI_Errhandler *errhandler=%p)", comm_to_string(comm), errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Errhandler_get(comm, errhandler);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Errhandler_get failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Errhandler_set ( EMPI_Comm comm, EMPI_Errhandler errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Errhandler_set(EMPI_Comm comm=%s, EMPI_Errhandler errhandler=%p)", comm_to_string(comm), (void *) errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Errhandler_set(comm, errhandler);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Errhandler_set failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Error_class ( int errorcode, int *errorclass )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Error_class(int errorcode=%d, int *errorclass=%p)", errorcode, errorclass);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Error_class(errorcode, errorclass);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Error_class failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Error_string ( int errorcode, char *string, int *resultlen )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Error_string(int errorcode=%d, char *string=%p, int *resultlen=%p)", errorcode, string, resultlen);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Error_string(errorcode, string, resultlen);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Error_string failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Exscan ( void *sendbuf, void *recvbuf, int count, EMPI_Datatype datatype, EMPI_Op op, EMPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Exscan(void *sendbuf=%p, void *recvbuf=%p, int count=%d, EMPI_Datatype datatype=%s, EMPI_Op op=%s, EMPI_Comm comm=%s)", sendbuf, recvbuf, count, type_to_string(datatype), op_to_string(op), comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Exscan(sendbuf, recvbuf, count, datatype, op, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_EXSCAN, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Exscan failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_call_errhandler ( EMPI_File fh, int errorcode )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_call_errhandler(EMPI_File fh=%s, int errorcode=%d)", file_to_string(fh), errorcode);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_call_errhandler(fh, errorcode);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_call_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_close ( EMPI_File *mpi_fh )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_close(EMPI_File *mpi_fh=%p)", EMPI_fh);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_close(EMPI_fh);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_close failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_create_errhandler ( EMPI_File_errhandler_fn *function, EMPI_Errhandler *errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_create_errhandler(EMPI_File_errhandler_fn *function=%p, EMPI_Errhandler *errhandler=%p)", function, errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_create_errhandler(function, errhandler);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_create_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_delete ( char *filename, EMPI_Info info )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_delete(char *filename=%p, EMPI_Info info=%s)", filename, info_to_string(info));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_delete(filename, info);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_delete failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_get_amode ( EMPI_File EMPI_fh, int *amode )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_get_amode(EMPI_File EMPI_fh=%s, int *amode=%p)", file_to_string(EMPI_fh), amode);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_get_amode(EMPI_fh, amode);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_get_amode failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_get_atomicity ( EMPI_File EMPI_fh, int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_get_atomicity(EMPI_File EMPI_fh=%s, int *flag=%p)", file_to_string(EMPI_fh), flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_get_atomicity(EMPI_fh, flag);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_get_atomicity failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_get_byte_offset ( EMPI_File EMPI_fh, EMPI_Offset offset, EMPI_Offset *disp )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_get_byte_offset(EMPI_File EMPI_fh=%s, EMPI_Offset offset=%p, EMPI_Offset *disp=%p)", file_to_string(EMPI_fh), (void *) offset, disp);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_get_byte_offset(EMPI_fh, offset, disp);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_get_byte_offset failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_get_errhandler ( EMPI_File file, EMPI_Errhandler *errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_get_errhandler(EMPI_File file=%s, EMPI_Errhandler *errhandler=%p)", file_to_string(file), errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_get_errhandler(file, errhandler);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_get_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_get_group ( EMPI_File EMPI_fh, EMPI_Group *g )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_get_group(EMPI_File EMPI_fh=%s, EMPI_Group *g=%p)", file_to_string(EMPI_fh), g);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_get_group(EMPI_fh, g);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_get_group failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_get_info ( EMPI_File EMPI_fh, EMPI_Info *info_used )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_get_info(EMPI_File EMPI_fh=%s, EMPI_Info *info_used=%p)", file_to_string(EMPI_fh), info_used);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_get_info(EMPI_fh, info_used);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_get_info failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_get_position ( EMPI_File EMPI_fh, EMPI_Offset *offset )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_get_position(EMPI_File EMPI_fh=%s, EMPI_Offset *offset=%p)", file_to_string(EMPI_fh), offset);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_get_position(EMPI_fh, offset);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_get_position failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_get_position_shared ( EMPI_File EMPI_fh, EMPI_Offset *offset )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_get_position_shared(EMPI_File EMPI_fh=%s, EMPI_Offset *offset=%p)", file_to_string(EMPI_fh), offset);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_get_position_shared(EMPI_fh, offset);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_get_position_shared failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_get_size ( EMPI_File EMPI_fh, EMPI_Offset *size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_get_size(EMPI_File EMPI_fh=%s, EMPI_Offset *size=%p)", file_to_string(EMPI_fh), size);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_get_size(EMPI_fh, size);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_get_size failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_get_type_extent ( EMPI_File EMPI_fh, EMPI_Datatype datatype, EMPI_Aint *extent )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_get_type_extent(EMPI_File EMPI_fh=%s, EMPI_Datatype datatype=%s, EMPI_Aint *extent=%p)", file_to_string(EMPI_fh), type_to_string(datatype), extent);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_get_type_extent(EMPI_fh, datatype, extent);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_get_type_extent failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_get_view ( EMPI_File EMPI_fh, EMPI_Offset *disp, EMPI_Datatype *etype, EMPI_Datatype *filetype, char *datarep )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_get_view(EMPI_File EMPI_fh=%s, EMPI_Offset *disp=%p, EMPI_Datatype *etype=%p, EMPI_Datatype *filetype=%p, char *datarep=%p)", file_to_string(EMPI_fh), disp, etype, filetype, datarep);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(*etype);
   CHECK_TYPE(*filetype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_get_view(EMPI_fh, disp, etype, filetype, datarep);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_get_view failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_iread_at ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype datatype, MPIO_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_iread_at(EMPI_File EMPI_fh=%s, EMPI_Offset offset=%p, void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, MPIO_Request *r=%p)", file_to_string(EMPI_fh), (void *) offset, buf, count, type_to_string(datatype), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_iread_at(EMPI_fh, offset, buf, count, datatype, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_iread_at failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_iread ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_iread(EMPI_File EMPI_fh=%s, void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, EMPI_Request *r=%p)", file_to_string(EMPI_fh), buf, count, type_to_string(datatype), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_iread(EMPI_fh, buf, count, datatype, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_iread failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_iread_shared ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_iread_shared(EMPI_File EMPI_fh=%s, void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, EMPI_Request *r=%p)", file_to_string(EMPI_fh), buf, count, type_to_string(datatype), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_iread_shared(EMPI_fh, buf, count, datatype, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_iread_shared failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_iwrite_at ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype datatype, MPIO_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_iwrite_at(EMPI_File EMPI_fh=%s, EMPI_Offset offset=%p, void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, MPIO_Request *r=%p)", file_to_string(EMPI_fh), (void *) offset, buf, count, type_to_string(datatype), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_iwrite_at(EMPI_fh, offset, buf, count, datatype, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_iwrite_at failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_iwrite ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_iwrite(EMPI_File EMPI_fh=%s, void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, EMPI_Request *r=%p)", file_to_string(EMPI_fh), buf, count, type_to_string(datatype), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_iwrite(EMPI_fh, buf, count, datatype, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_iwrite failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_iwrite_shared ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, MPIO_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_iwrite_shared(EMPI_File EMPI_fh=%s, void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, MPIO_Request *r=%p)", file_to_string(EMPI_fh), buf, count, type_to_string(datatype), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_iwrite_shared(EMPI_fh, buf, count, datatype, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_iwrite_shared failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_open ( EMPI_Comm comm, char *filename, int amode, EMPI_Info info, EMPI_File *fh )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_open(EMPI_Comm comm=%s, char *filename=%p, int amode=%d, EMPI_Info info=%s, EMPI_File *fh=%p)", comm_to_string(comm), filename, amode, info_to_string(info), fh);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_open(comm, filename, amode, info, fh);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_open failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_preallocate ( EMPI_File EMPI_fh, EMPI_Offset size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_preallocate(EMPI_File EMPI_fh=%s, EMPI_Offset size=%p)", file_to_string(EMPI_fh), (void *) size);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_preallocate(EMPI_fh, size);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_preallocate failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_read_all_begin ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_read_all_begin(EMPI_File EMPI_fh=%s, void *buf=%p, int count=%d, EMPI_Datatype datatype=%s)", file_to_string(EMPI_fh), buf, count, type_to_string(datatype));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_read_all_begin(EMPI_fh, buf, count, datatype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_read_all_begin failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_read_all_end ( EMPI_File EMPI_fh, void *buf, EMPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_read_all_end(EMPI_File EMPI_fh=%s, void *buf=%p, EMPI_Status *status=%p)", file_to_string(EMPI_fh), buf, status);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_read_all_end(EMPI_fh, buf, status);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_read_all_end failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_read_all ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_read_all(EMPI_File EMPI_fh=%s, void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, EMPI_Status *status=%p)", file_to_string(EMPI_fh), buf, count, type_to_string(datatype), status);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_read_all(EMPI_fh, buf, count, datatype, status);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_read_all failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_read_at_all_begin ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype datatype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_read_at_all_begin(EMPI_File EMPI_fh=%s, EMPI_Offset offset=%p, void *buf=%p, int count=%d, EMPI_Datatype datatype=%s)", file_to_string(EMPI_fh), (void *) offset, buf, count, type_to_string(datatype));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_read_at_all_begin(EMPI_fh, offset, buf, count, datatype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_read_at_all_begin failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_read_at_all_end ( EMPI_File EMPI_fh, void *buf, EMPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_read_at_all_end(EMPI_File EMPI_fh=%s, void *buf=%p, EMPI_Status *status=%p)", file_to_string(EMPI_fh), buf, status);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_read_at_all_end(EMPI_fh, buf, status);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_read_at_all_end failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int EMPI_File_read_at_all ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_read_at_all(EMPI_File EMPI_fh=%s, EMPI_Offset offset=%p, void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, EMPI_Status *status=%p)", file_to_string(EMPI_fh), (void *) offset, buf, count, type_to_string(datatype), status);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_read_at_all(EMPI_fh, offset, buf, count, datatype, status);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_read_at_all failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_read_at ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_read_at(EMPI_File EMPI_fh=%s, EMPI_Offset offset=%p, void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, EMPI_Status *status=%p)", file_to_string(EMPI_fh), (void *) offset, buf, count, type_to_string(datatype), status);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_read_at(EMPI_fh, offset, buf, count, datatype, status);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_read_at failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_read ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_read(EMPI_File EMPI_fh=%s, void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, EMPI_Status *status=%p)", file_to_string(EMPI_fh), buf, count, type_to_string(datatype), status);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_read(EMPI_fh, buf, count, datatype, status);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_read failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_read_ordered_begin ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_read_ordered_begin(EMPI_File EMPI_fh=%s, void *buf=%p, int count=%d, EMPI_Datatype datatype=%s)", file_to_string(EMPI_fh), buf, count, type_to_string(datatype));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_read_ordered_begin(EMPI_fh, buf, count, datatype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_read_ordered_begin failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_read_ordered_end ( EMPI_File EMPI_fh, void *buf, EMPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_read_ordered_end(EMPI_File EMPI_fh=%s, void *buf=%p, EMPI_Status *status=%p)", file_to_string(EMPI_fh), buf, status);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_read_ordered_end(EMPI_fh, buf, status);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_read_ordered_end failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_read_ordered ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_read_ordered(EMPI_File EMPI_fh=%s, void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, EMPI_Status *status=%p)", file_to_string(EMPI_fh), buf, count, type_to_string(datatype), status);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_read_ordered(EMPI_fh, buf, count, datatype, status);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_read_ordered failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_read_shared ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_read_shared(EMPI_File EMPI_fh=%s, void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, EMPI_Status *status=%p)", file_to_string(EMPI_fh), buf, count, type_to_string(datatype), status);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_read_shared(EMPI_fh, buf, count, datatype, status);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_read_shared failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_seek ( EMPI_File EMPI_fh, EMPI_Offset offset, int whence )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_seek(EMPI_File EMPI_fh=%s, EMPI_Offset offset=%p, int whence=%d)", file_to_string(EMPI_fh), (void *) offset, whence);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_seek(EMPI_fh, offset, whence);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_seek failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_seek_shared ( EMPI_File EMPI_fh, EMPI_Offset offset, int whence )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_seek_shared(EMPI_File EMPI_fh=%s, EMPI_Offset offset=%p, int whence=%d)", file_to_string(EMPI_fh), (void *) offset, whence);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_seek_shared(EMPI_fh, offset, whence);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_seek_shared failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_set_atomicity ( EMPI_File EMPI_fh, int flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_set_atomicity(EMPI_File EMPI_fh=%s, int flag=%d)", file_to_string(EMPI_fh), flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_set_atomicity(EMPI_fh, flag);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_set_atomicity failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_set_errhandler ( EMPI_File file, EMPI_Errhandler errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_set_errhandler(EMPI_File file=%s, EMPI_Errhandler errhandler=%p)", file_to_string(file), (void *) errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_set_errhandler(file, errhandler);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_set_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_set_info ( EMPI_File EMPI_fh, EMPI_Info info )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_set_info(EMPI_File EMPI_fh=%s, EMPI_Info info=%s)", file_to_string(EMPI_fh), info_to_string(info));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_set_info(EMPI_fh, info);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_set_info failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_set_size ( EMPI_File EMPI_fh, EMPI_Offset size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_set_size(EMPI_File EMPI_fh=%s, EMPI_Offset size=%p)", file_to_string(EMPI_fh), (void *) size);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_set_size(EMPI_fh, size);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_set_size failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_set_view ( EMPI_File EMPI_fh, EMPI_Offset disp, EMPI_Datatype etype, EMPI_Datatype filetype, char *datarep, EMPI_Info info )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_set_view(EMPI_File EMPI_fh=%s, EMPI_Offset disp=%p, EMPI_Datatype etype=%s, EMPI_Datatype filetype=%s, char *datarep=%p, EMPI_Info info=%s)", file_to_string(EMPI_fh), (void *) disp, type_to_string(etype), type_to_string(filetype), datarep, info_to_string(info));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(etype);
   CHECK_TYPE(filetype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_set_view(EMPI_fh, disp, etype, filetype, datarep, info);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_set_view failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_sync ( EMPI_File EMPI_fh )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_sync(EMPI_File EMPI_fh=%s)", file_to_string(EMPI_fh));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_sync(EMPI_fh);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_sync failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_write_all_begin ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_write_all_begin(EMPI_File EMPI_fh=%s, void *buf=%p, int count=%d, EMPI_Datatype datatype=%s)", file_to_string(EMPI_fh), buf, count, type_to_string(datatype));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_write_all_begin(EMPI_fh, buf, count, datatype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_write_all_begin failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_write_all_end ( EMPI_File EMPI_fh, void *buf, EMPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_write_all_end(EMPI_File EMPI_fh=%s, void *buf=%p, EMPI_Status *status=%p)", file_to_string(EMPI_fh), buf, status);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_write_all_end(EMPI_fh, buf, status);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_write_all_end failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_write_all ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_write_all(EMPI_File EMPI_fh=%s, void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, EMPI_Status *status=%p)", file_to_string(EMPI_fh), buf, count, type_to_string(datatype), status);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_write_all(EMPI_fh, buf, count, datatype, status);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_write_all failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_write_at_all_begin ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype datatype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_write_at_all_begin(EMPI_File EMPI_fh=%s, EMPI_Offset offset=%p, void *buf=%p, int count=%d, EMPI_Datatype datatype=%s)", file_to_string(EMPI_fh), (void *) offset, buf, count, type_to_string(datatype));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_write_at_all_begin(EMPI_fh, offset, buf, count, datatype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_write_at_all_begin failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_write_at_all_end ( EMPI_File EMPI_fh, void *buf, EMPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_write_at_all_end(EMPI_File EMPI_fh=%s, void *buf=%p, EMPI_Status *status=%p)", file_to_string(EMPI_fh), buf, status);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_write_at_all_end(EMPI_fh, buf, status);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_write_at_all_end failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_write_at_all ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_write_at_all(EMPI_File EMPI_fh=%s, EMPI_Offset offset=%p, void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, EMPI_Status *status=%p)", file_to_string(EMPI_fh), (void *) offset, buf, count, type_to_string(datatype), status);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_write_at_all(EMPI_fh, offset, buf, count, datatype, status);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_write_at_all failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_write_at ( EMPI_File EMPI_fh, EMPI_Offset offset, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_write_at(EMPI_File EMPI_fh=%s, EMPI_Offset offset=%p, void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, EMPI_Status *status=%p)", file_to_string(EMPI_fh), (void *) offset, buf, count, type_to_string(datatype), status);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_write_at(EMPI_fh, offset, buf, count, datatype, status);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_write_at failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_write ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_write(EMPI_File EMPI_fh=%s, void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, EMPI_Status *status=%p)", file_to_string(EMPI_fh), buf, count, type_to_string(datatype), status);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_write(EMPI_fh, buf, count, datatype, status);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_write failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_write_ordered_begin ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_write_ordered_begin(EMPI_File EMPI_fh=%s, void *buf=%p, int count=%d, EMPI_Datatype datatype=%s)", file_to_string(EMPI_fh), buf, count, type_to_string(datatype));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_write_ordered_begin(EMPI_fh, buf, count, datatype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_write_ordered_begin failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_write_ordered_end ( EMPI_File EMPI_fh, void *buf, EMPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_write_ordered_end(EMPI_File EMPI_fh=%s, void *buf=%p, EMPI_Status *status=%p)", file_to_string(EMPI_fh), buf, status);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_write_ordered_end(EMPI_fh, buf, status);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_write_ordered_end failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_write_ordered ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_write_ordered(EMPI_File EMPI_fh=%s, void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, EMPI_Status *status=%p)", file_to_string(EMPI_fh), buf, count, type_to_string(datatype), status);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_write_ordered(EMPI_fh, buf, count, datatype, status);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_write_ordered failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_File_write_shared ( EMPI_File EMPI_fh, void *buf, int count, EMPI_Datatype datatype, EMPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_File_write_shared(EMPI_File EMPI_fh=%s, void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, EMPI_Status *status=%p)", file_to_string(EMPI_fh), buf, count, type_to_string(datatype), status);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_File_write_shared(EMPI_fh, buf, count, datatype, status);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_File_write_shared failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}



int EMPI_Free_mem ( void *base )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Free_mem(void *base=%p)", base);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Free_mem(base);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Free_mem failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Gather ( void *sendbuf, int sendcnt, EMPI_Datatype sendtype, void *recvbuf, int recvcnt, EMPI_Datatype recvtype, int root, EMPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Gather(void *sendbuf=%p, int sendcnt=%d, EMPI_Datatype sendtype=%s, void *recvbuf=%p, int recvcnt=%d, EMPI_Datatype recvtype=%s, int root=%d, EMPI_Comm comm=%s)", sendbuf, sendcnt, type_to_string(sendtype), recvbuf, recvcnt, type_to_string(recvtype), root, comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(sendtype);
   CHECK_TYPE(recvtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Gather(sendbuf, sendcnt, sendtype, recvbuf, recvcnt, recvtype, root, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_GATHER, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Gather failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Gatherv ( void *sendbuf, int sendcnt, EMPI_Datatype sendtype, void *recvbuf, int *recvcnts, int *displs, EMPI_Datatype recvtype, int root, EMPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Gatherv(void *sendbuf=%p, int sendcnt=%d, EMPI_Datatype sendtype=%s, void *recvbuf=%p, int *recvcnts=%p, int *displs=%p, EMPI_Datatype recvtype=%s, int root=%d, EMPI_Comm comm=%s)", sendbuf, sendcnt, type_to_string(sendtype), recvbuf, recvcnts, displs, type_to_string(recvtype), root, comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(sendtype);
   CHECK_TYPE(recvtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Gatherv(sendbuf, sendcnt, sendtype, recvbuf, recvcnts, displs, recvtype, root, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_GATHER, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Gatherv failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Get_address ( void *location, EMPI_Aint *address )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Get_address(void *location=%p, EMPI_Aint *address=%p)", location, address);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Get_address(location, address);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Get_address failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Get_count ( EMPI_Status *status, EMPI_Datatype datatype, int *count )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Get_count(EMPI_Status *status=%p, EMPI_Datatype datatype=%s, int *count=%p)", status, type_to_string(datatype), count);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Get_count(status, datatype, count);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Get_count failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Get_elements ( EMPI_Status *status, EMPI_Datatype datatype, int *elements )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Get_elements(EMPI_Status *status=%p, EMPI_Datatype datatype=%s, int *elements=%p)", status, type_to_string(datatype), elements);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Get_elements(status, datatype, elements);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Get_elements failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Get ( void *origin_addr, int origin_count, EMPI_Datatype origin_datatype, int target_rank, EMPI_Aint target_disp, int target_count, EMPI_Datatype target_datatype, EMPI_Win win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Get(void *origin_addr=%p, int origin_count=%d, EMPI_Datatype origin_datatype=%s, int target_rank=%d, EMPI_Aint target_disp=%p, int target_count=%d, EMPI_Datatype target_datatype=%s, EMPI_Win win=%s)", origin_addr, origin_count, type_to_string(origin_datatype), target_rank, (void *) target_disp, target_count, type_to_string(target_datatype), win_to_string(win));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(origin_datatype);
   CHECK_TYPE(target_datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Get(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Get failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Get_processor_name ( char *name, int *resultlen )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Get_processor_name(char *name=%p, int *resultlen=%p)", name, resultlen);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Get_processor_name(name, resultlen);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Get_processor_name failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Get_version ( int *version, int *subversion )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Get_version(int *version=%p, int *subversion=%p)", version, subversion);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Get_version(version, subversion);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Get_version failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Graph_create ( EMPI_Comm comm_old, int nnodes, int *indx, int *edges, int reorder, EMPI_Comm *comm_graph )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Graph_create(EMPI_Comm comm_old=%s, int nnodes=%d, int *indx=%p, int *edges=%p, int reorder=%d, EMPI_Comm *comm_graph=%p)", comm_to_string(comm_old), nnodes, indx, edges, reorder, comm_graph);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Graph_create(comm_old, nnodes, indx, edges, reorder, comm_graph);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm_old, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Graph_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Graphdims_get ( EMPI_Comm comm, int *nnodes, int *nedges )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Graphdims_get(EMPI_Comm comm=%s, int *nnodes=%p, int *nedges=%p)", comm_to_string(comm), nnodes, nedges);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Graphdims_get(comm, nnodes, nedges);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Graphdims_get failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Graph_get ( EMPI_Comm comm, int maxindex, int maxedges, int *indx, int *edges )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Graph_get(EMPI_Comm comm=%s, int maxindex=%d, int maxedges=%d, int *indx=%p, int *edges=%p)", comm_to_string(comm), maxindex, maxedges, indx, edges);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Graph_get(comm, maxindex, maxedges, indx, edges);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Graph_get failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Graph_map ( EMPI_Comm comm_old, int nnodes, int *indx, int *edges, int *newrank )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Graph_map(EMPI_Comm comm_old=%s, int nnodes=%d, int *indx=%p, int *edges=%p, int *newrank=%p)", comm_to_string(comm_old), nnodes, indx, edges, newrank);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Graph_map(comm_old, nnodes, indx, edges, newrank);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm_old, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Graph_map failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Graph_neighbors_count ( EMPI_Comm comm, int rank, int *nneighbors )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Graph_neighbors_count(EMPI_Comm comm=%s, int rank=%d, int *nneighbors=%p)", comm_to_string(comm), rank, nneighbors);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Graph_neighbors_count(comm, rank, nneighbors);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Graph_neighbors_count failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Graph_neighbors ( EMPI_Comm comm, int rank, int maxneighbors, int *neighbors )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Graph_neighbors(EMPI_Comm comm=%s, int rank=%d, int maxneighbors=%d, int *neighbors=%p)", comm_to_string(comm), rank, maxneighbors, neighbors);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Graph_neighbors(comm, rank, maxneighbors, neighbors);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Graph_neighbors failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Grequest_complete ( EMPI_Request r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Grequest_complete(EMPI_Request r=%s)", request_to_string(r));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Grequest_complete(r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Grequest_complete failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Grequest_start ( EMPI_Grequest_query_function *query_fn, EMPI_Grequest_free_function *free_fn, EMPI_Grequest_cancel_function *cancel_fn, void *extra_state, EMPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Grequest_start(EMPI_Grequest_query_function *query_fn=%p, EMPI_Grequest_free_function *free_fn=%p, EMPI_Grequest_cancel_function *cancel_fn=%p, void *extra_state=%p, EMPI_Request *r=%p)", query_fn, free_fn, cancel_fn, extra_state, r);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Grequest_start(query_fn, free_fn, cancel_fn, extra_state, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Grequest_start failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Group_compare ( EMPI_Group group1, EMPI_Group group2, int *result )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Group_compare(EMPI_Group group1=%s, EMPI_Group group2=%s, int *result=%p)", group_to_string(group1), group_to_string(group2), result);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Group_compare(group1, group2, result);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Group_compare failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Group_difference ( EMPI_Group group1, EMPI_Group group2, EMPI_Group *newgroup )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Group_difference(EMPI_Group group1=%s, EMPI_Group group2=%s, EMPI_Group *newgroup=%p)", group_to_string(group1), group_to_string(group2), newgroup);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Group_difference(group1, group2, newgroup);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Group_difference failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Group_excl ( EMPI_Group g, int n, int *ranks, EMPI_Group *newgroup )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Group_excl(EMPI_Group g=%s, int n=%d, int *ranks=%p, EMPI_Group *newgroup=%p)", group_to_string(g), n, ranks, newgroup);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Group_excl(g, n, ranks, newgroup);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Group_excl failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Group_free ( EMPI_Group *g )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Group_free(EMPI_Group *g=%p)", g);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Group_free(g);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Group_free failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Group_incl ( EMPI_Group g, int n, int *ranks, EMPI_Group *newgroup )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Group_incl(EMPI_Group g=%s, int n=%d, int *ranks=%p, EMPI_Group *newgroup=%p)", group_to_string(g), n, ranks, newgroup);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Group_incl(g, n, ranks, newgroup);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Group_incl failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Group_intersection ( EMPI_Group group1, EMPI_Group group2, EMPI_Group *newgroup )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Group_intersection(EMPI_Group group1=%s, EMPI_Group group2=%s, EMPI_Group *newgroup=%p)", group_to_string(group1), group_to_string(group2), newgroup);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Group_intersection(group1, group2, newgroup);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Group_intersection failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Group_range_excl ( EMPI_Group g, int n, int ranges[][3], EMPI_Group *newgroup )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Group_range_excl(EMPI_Group g=%s, int n=%d, int ranges[][3]=%p, EMPI_Group *newgroup=%p)", group_to_string(g), n, ranges, newgroup);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Group_range_excl(g, n, ranges, newgroup);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Group_range_excl failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Group_range_incl ( EMPI_Group g, int n, int ranges[][3], EMPI_Group *newgroup )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Group_range_incl(EMPI_Group g=%s, int n=%d, int ranges[][3]=%p, EMPI_Group *newgroup=%p)", group_to_string(g), n, ranges, newgroup);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Group_range_incl(g, n, ranges, newgroup);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Group_range_incl failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Group_rank ( EMPI_Group g, int *rank )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Group_rank(EMPI_Group g=%s, int *rank=%p)", group_to_string(g), rank);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Group_rank(g, rank);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Group_rank failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Group_size ( EMPI_Group g, int *size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Group_size(EMPI_Group g=%s, int *size=%p)", group_to_string(g), size);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Group_size(g, size);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Group_size failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Group_translate_ranks ( EMPI_Group group1, int n, int *ranks1, EMPI_Group group2, int *ranks2 )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Group_translate_ranks(EMPI_Group group1=%s, int n=%d, int *ranks1=%p, EMPI_Group group2=%s, int *ranks2=%p)", group_to_string(group1), n, ranks1, group_to_string(group2), ranks2);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL


   int error = xEMPI_Group_translate_ranks(group1, n, ranks1, group2, ranks2);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Group_translate_ranks failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Group_union ( EMPI_Group group1, EMPI_Group group2, EMPI_Group *newgroup )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Group_union(EMPI_Group group1=%s, EMPI_Group group2=%s, EMPI_Group *newgroup=%p)", group_to_string(group1), group_to_string(group2), newgroup);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Group_union(group1, group2, newgroup);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Group_union failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Ibsend ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm, EMPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Ibsend(void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, int dest=%d, int tag=%d, EMPI_Comm comm=%s, EMPI_Request *r=%p)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Ibsend(buf, count, datatype, dest, tag, comm, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_IBSEND, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Ibsend failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Info_create ( EMPI_Info *info )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Info_create(EMPI_Info *info=%p)", info);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL


   int error = xEMPI_Info_create(info);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Info_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Info_delete ( EMPI_Info info, char *key )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Info_delete(EMPI_Info info=%s, char *key=%p)", info_to_string(info), key);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Info_delete(info, key);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Info_delete failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Info_dup ( EMPI_Info info, EMPI_Info *newinfo )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Info_dup(EMPI_Info info=%s, EMPI_Info *newinfo=%p)", info_to_string(info), newinfo);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Info_dup(info, newinfo);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Info_dup failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Info_free ( EMPI_Info *info )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Info_free(EMPI_Info *info=%p)", info);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Info_free(info);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Info_free failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Info_get ( EMPI_Info info, char *key, int valuelen, char *value, int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Info_get(EMPI_Info info=%s, char *key=%p, int valuelen=%d, char *value=%p, int *flag=%p)", info_to_string(info), key, valuelen, value, flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Info_get(info, key, valuelen, value, flag);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Info_get failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Info_get_nkeys ( EMPI_Info info, int *nkeys )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Info_get_nkeys(EMPI_Info info=%s, int *nkeys=%p)", info_to_string(info), nkeys);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Info_get_nkeys(info, nkeys);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Info_get_nkeys failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Info_get_nthkey ( EMPI_Info info, int n, char *key )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Info_get_nthkey(EMPI_Info info=%s, int n=%d, char *key=%p)", info_to_string(info), n, key);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Info_get_nthkey(info, n, key);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Info_get_nthkey failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Info_get_valuelen ( EMPI_Info info, char *key, int *valuelen, int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Info_get_valuelen(EMPI_Info info=%s, char *key=%p, int *valuelen=%p, int *flag=%p)", info_to_string(info), key, valuelen, flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Info_get_valuelen(info, key, valuelen, flag);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Info_get_valuelen failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Info_set ( EMPI_Info info, char *key, char *value )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Info_set(EMPI_Info info=%s, char *key=%p, char *value=%p)", info_to_string(info), key, value);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Info_set(info, key, value);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Info_set failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Initialized ( int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Initialized(int *flag=%p)", flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Initialized(flag);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Initialized failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Init_thread ( int *argc, char ***argv, int required, int *provided )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Init_thread(int *argc=%p, char ***argv=%p, int required=%d, int *provided=%p)", argc, argv, required, provided);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Init_thread(argc, argv, required, provided);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Init_thread failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Intercomm_create ( EMPI_Comm local_comm, int local_leader, EMPI_Comm peer_comm, int remote_leader, int tag, EMPI_Comm *newintercomm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Intercomm_create(EMPI_Comm local_comm=%s, int local_leader=%d, EMPI_Comm peer_comm=%s, int remote_leader=%d, int tag=%d, EMPI_Comm *newintercomm=%p)", comm_to_string(local_comm), local_leader, comm_to_string(peer_comm), remote_leader, tag, newintercomm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Intercomm_create(local_comm, local_leader, peer_comm, remote_leader, tag, newintercomm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(local_comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Intercomm_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Intercomm_merge ( EMPI_Comm intercomm, int high, EMPI_Comm *newintracomm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Intercomm_merge(EMPI_Comm intercomm=%s, int high=%d, EMPI_Comm *newintracomm=%p)", comm_to_string(intercomm), high, newintracomm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Intercomm_merge(intercomm, high, newintracomm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(intercomm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Intercomm_merge failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Iprobe ( int source, int tag, EMPI_Comm comm, int *flag, EMPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Iprobe(int source=%d, int tag=%d, EMPI_Comm comm=%s, int *flag=%p, EMPI_Status *status=%p)", source, tag, comm_to_string(comm), flag, status);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Iprobe(source, tag, comm, flag, status);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_IPROBE, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Iprobe failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Irecv ( void *buf, int count, EMPI_Datatype datatype, int source, int tag, EMPI_Comm comm, EMPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Irecv(void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, int source=%d, int tag=%d, EMPI_Comm comm=%s, EMPI_Request *r=%p)", buf, count, type_to_string(datatype), source, tag, comm_to_string(comm), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Irecv(buf, count, datatype, source, tag, comm, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_IRECV, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Irecv failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Irsend ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm, EMPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Irsend(void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, int dest=%d, int tag=%d, EMPI_Comm comm=%s, EMPI_Request *r=%p)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Irsend(buf, count, datatype, dest, tag, comm, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_IRSEND, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Irsend failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Isend ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm, EMPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Isend(void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, int dest=%d, int tag=%d, EMPI_Comm comm=%s, EMPI_Request *r=%p)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Isend(buf, count, datatype, dest, tag, comm, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_ISEND, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Isend failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Issend ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm, EMPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Issend(void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, int dest=%d, int tag=%d, EMPI_Comm comm=%s, EMPI_Request *r=%p)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Issend(buf, count, datatype, dest, tag, comm, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_ISSEND, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Issend failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Is_thread_main ( int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Is_thread_main(int *flag=%p)", flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Is_thread_main(flag);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Is_thread_main failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Keyval_create ( EMPI_Copy_function *copy_fn, EMPI_Delete_function *delete_fn, int *keyval, void *extra_state )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Keyval_create(EMPI_Copy_function *copy_fn=%p, EMPI_Delete_function *delete_fn=%p, int *keyval=%p, void *extra_state=%p)", copy_fn, delete_fn, keyval, extra_state);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Keyval_create(copy_fn, delete_fn, keyval, extra_state);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Keyval_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Keyval_free ( int *keyval )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Keyval_free(int *keyval=%p)", keyval);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Keyval_free(keyval);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Keyval_free failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Lookup_name ( char *service_name, EMPI_Info info, char *port_name )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Lookup_name(char *service_name=%p, EMPI_Info info=%s, char *port_name=%p)", service_name, info_to_string(info), port_name);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Lookup_name(service_name, info, port_name);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Lookup_name failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

/*
int EMPI_Op_commutative ( EMPI_Op op, int *commute )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Op_commutative(EMPI_Op op=%s, int *commute=%p)", op_to_string(op), commute);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Op_commutative(op, commute);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Op_commutative failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}
*/

int EMPI_Op_create ( EMPI_User_function *function, int commute, EMPI_Op *op )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Op_create(EMPI_User_function *function=%p, int commute=%d, EMPI_Op *op=%p)", function, commute, op);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Op_create(function, commute, op);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Op_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Open_port ( EMPI_Info info, char *port_name )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Open_port(EMPI_Info info=%s, char *port_name=%p)", info_to_string(info), port_name);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Open_port(info, port_name);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Open_port failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Op_free ( EMPI_Op *op )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Op_free(EMPI_Op *op=%p)", op);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Op_free(op);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Op_free failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Pack_external ( char *datarep, void *inbuf, int incount, EMPI_Datatype datatype, void *outbuf, EMPI_Aint outcount, EMPI_Aint *position )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Pack_external(char *datarep=%p, void *inbuf=%p, int incount=%d, EMPI_Datatype datatype=%s, void *outbuf=%p, EMPI_Aint outcount=%p, EMPI_Aint *position=%p)", datarep, inbuf, incount, type_to_string(datatype), outbuf, (void *) outcount, position);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Pack_external(datarep, inbuf, incount, datatype, outbuf, outcount, position);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Pack_external failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Pack_external_size ( char *datarep, int incount, EMPI_Datatype datatype, EMPI_Aint *size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Pack_external_size(char *datarep=%p, int incount=%d, EMPI_Datatype datatype=%s, EMPI_Aint *size=%p)", datarep, incount, type_to_string(datatype), size);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Pack_external_size(datarep, incount, datatype, size);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Pack_external_size failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Pack ( void *inbuf, int incount, EMPI_Datatype datatype, void *outbuf, int outcount, int *position, EMPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Pack(void *inbuf=%p, int incount=%d, EMPI_Datatype datatype=%s, void *outbuf=%p, int outcount=%d, int *position=%p, EMPI_Comm comm=%s)", inbuf, incount, type_to_string(datatype), outbuf, outcount, position, comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Pack(inbuf, incount, datatype, outbuf, outcount, position, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Pack failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Pack_size ( int incount, EMPI_Datatype datatype, EMPI_Comm comm, int *size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Pack_size(int incount=%d, EMPI_Datatype datatype=%s, EMPI_Comm comm=%s, int *size=%p)", incount, type_to_string(datatype), comm_to_string(comm), size);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Pack_size(incount, datatype, comm, size);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Pack_size failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Probe ( int source, int tag, EMPI_Comm comm, EMPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Probe(int source=%d, int tag=%d, EMPI_Comm comm=%s, EMPI_Status *status=%p)", source, tag, comm_to_string(comm), status);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Probe(source, tag, comm, status);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_PROBE, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Probe failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Publish_name ( char *service_name, EMPI_Info info, char *port_name )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Publish_name(char *service_name=%p, EMPI_Info info=%s, char *port_name=%p)", service_name, info_to_string(info), port_name);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Publish_name(service_name, info, port_name);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Publish_name failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Put ( void *origin_addr, int origin_count, EMPI_Datatype origin_datatype, int target_rank, EMPI_Aint target_disp, int target_count, EMPI_Datatype target_datatype, EMPI_Win win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Put(void *origin_addr=%p, int origin_count=%d, EMPI_Datatype origin_datatype=%s, int target_rank=%d, EMPI_Aint target_disp=%p, int target_count=%d, EMPI_Datatype target_datatype=%s, EMPI_Win win=%s)", origin_addr, origin_count, type_to_string(origin_datatype), target_rank, (void *) target_disp, target_count, type_to_string(target_datatype), win_to_string(win));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(origin_datatype);
   CHECK_TYPE(target_datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Put(origin_addr, origin_count, origin_datatype, target_rank, target_disp, target_count, target_datatype, win);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Put failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Query_thread ( int *provided )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Query_thread(int *provided=%p)", provided);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Query_thread(provided);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Query_thread failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Recv ( void *buf, int count, EMPI_Datatype datatype, int source, int tag, EMPI_Comm comm, EMPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Recv(void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, int source=%d, int tag=%d, EMPI_Comm comm=%s, EMPI_Status *status=%p)", buf, count, type_to_string(datatype), source, tag, comm_to_string(comm), status);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Recv(buf, count, datatype, source, tag, comm, status);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_RECV, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Recv failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Recv_init ( void *buf, int count, EMPI_Datatype datatype, int source, int tag, EMPI_Comm comm, EMPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Recv_init(void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, int source=%d, int tag=%d, EMPI_Comm comm=%s, EMPI_Request *r=%p)", buf, count, type_to_string(datatype), source, tag, comm_to_string(comm), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Recv_init(buf, count, datatype, source, tag, comm, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Recv_init failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Reduce ( void *sendbuf, void *recvbuf, int count, EMPI_Datatype datatype, EMPI_Op op, int root, EMPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Reduce(void *sendbuf=%p, void *recvbuf=%p, int count=%d, EMPI_Datatype datatype=%s, EMPI_Op op=%s, int root=%d, EMPI_Comm comm=%s)", sendbuf, recvbuf, count, type_to_string(datatype), op_to_string(op), root, comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Reduce(sendbuf, recvbuf, count, datatype, op, root, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_REDUCE, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Reduce failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

/*
int EMPI_Reduce_local ( void *inbuf, void *inoutbuf, int count, EMPI_Datatype datatype, EMPI_Op op )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Reduce_local(void *inbuf=%p, void *inoutbuf=%p, int count=%d, EMPI_Datatype datatype=%s, EMPI_Op op=%s)", inbuf, inoutbuf, count, type_to_string(datatype), op_to_string(op));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Reduce_local(inbuf, inoutbuf, count, datatype, op);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Reduce_local failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}
*/
/*
int EMPI_Reduce_scatter_block ( void *sendbuf, void *recvbuf, int recvcount, EMPI_Datatype datatype, EMPI_Op op, EMPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Reduce_scatter_block(void *sendbuf=%p, void *recvbuf=%p, int recvcount=%d, EMPI_Datatype datatype=%s, EMPI_Op op=%s, EMPI_Comm comm=%s)", sendbuf, recvbuf, recvcount, type_to_string(datatype), op_to_string(op), comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Reduce_scatter_block(sendbuf, recvbuf, recvcount, datatype, op, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Reduce_scatter_block failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}
*/

int EMPI_Reduce_scatter ( void *sendbuf, void *recvbuf, int *recvcnts, EMPI_Datatype datatype, EMPI_Op op, EMPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Reduce_scatter(void *sendbuf=%p, void *recvbuf=%p, int *recvcnts=%p, EMPI_Datatype datatype=%s, EMPI_Op op=%s, EMPI_Comm comm=%s)", sendbuf, recvbuf, recvcnts, type_to_string(datatype), op_to_string(op), comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Reduce_scatter(sendbuf, recvbuf, recvcnts, datatype, op, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_REDUCE_SCATTER, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Reduce_scatter failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Register_datarep ( char *name, EMPI_Datarep_conversion_function *read_conv_fn, EMPI_Datarep_conversion_function *write_conv_fn, EMPI_Datarep_extent_function *extent_fn, void *state )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Register_datarep(char *name=%p, EMPI_Datarep_conversion_function *read_conv_fn=%p, EMPI_Datarep_conversion_function *write_conv_fn=%p, EMPI_Datarep_extent_function *extent_fn=%p, void *state=%p)", name, read_conv_fn, write_conv_fn, extent_fn, state);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Register_datarep(name, read_conv_fn, write_conv_fn, extent_fn, state);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Register_datarep failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Request_free ( EMPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Request_free(EMPI_Request *r=%p)", r);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Request_free(r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Request_free failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Request_get_status ( EMPI_Request r, int *flag, EMPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Request_get_status(EMPI_Request r=%s, int *flag=%p, EMPI_Status *status=%p)", request_to_string(r), flag, status);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Request_get_status(r, flag, status);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Request_get_status failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Rsend ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Rsend(void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, int dest=%d, int tag=%d, EMPI_Comm comm=%s)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Rsend(buf, count, datatype, dest, tag, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_RSEND, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Rsend failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Rsend_init ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm, EMPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Rsend_init(void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, int dest=%d, int tag=%d, EMPI_Comm comm=%s, EMPI_Request *r=%p)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Rsend_init(buf, count, datatype, dest, tag, comm, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Rsend_init failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Scan ( void *sendbuf, void *recvbuf, int count, EMPI_Datatype datatype, EMPI_Op op, EMPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Scan(void *sendbuf=%p, void *recvbuf=%p, int count=%d, EMPI_Datatype datatype=%s, EMPI_Op op=%s, EMPI_Comm comm=%s)", sendbuf, recvbuf, count, type_to_string(datatype), op_to_string(op), comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Scan(sendbuf, recvbuf, count, datatype, op, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_SCAN, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Scan failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Scatter ( void *sendbuf, int sendcnt, EMPI_Datatype sendtype, void *recvbuf, int recvcnt, EMPI_Datatype recvtype, int root, EMPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Scatter(void *sendbuf=%p, int sendcnt=%d, EMPI_Datatype sendtype=%s, void *recvbuf=%p, int recvcnt=%d, EMPI_Datatype recvtype=%s, int root=%d, EMPI_Comm comm=%s)", sendbuf, sendcnt, type_to_string(sendtype), recvbuf, recvcnt, type_to_string(recvtype), root, comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(sendtype);
   CHECK_TYPE(recvtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Scatter(sendbuf, sendcnt, sendtype, recvbuf, recvcnt, recvtype, root, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_SCATTER, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Scatter failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Scatterv ( void *sendbuf, int *sendcnts, int *displs, EMPI_Datatype sendtype, void *recvbuf, int recvcnt, EMPI_Datatype recvtype, int root, EMPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Scatterv(void *sendbuf=%p, int *sendcnts=%p, int *displs=%p, EMPI_Datatype sendtype=%s, void *recvbuf=%p, int recvcnt=%d, EMPI_Datatype recvtype=%s, int root=%d, EMPI_Comm comm=%s)", sendbuf, sendcnts, displs, type_to_string(sendtype), recvbuf, recvcnt, type_to_string(recvtype), root, comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(sendtype);
   CHECK_TYPE(recvtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Scatterv(sendbuf, sendcnts, displs, sendtype, recvbuf, recvcnt, recvtype, root, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_SCATTER, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Scatterv failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Send ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Send(void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, int dest=%d, int tag=%d, EMPI_Comm comm=%s)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Send(buf, count, datatype, dest, tag, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_SEND, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Send failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Send_init ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm, EMPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Send_init(void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, int dest=%d, int tag=%d, EMPI_Comm comm=%s, EMPI_Request *r=%p)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Send_init(buf, count, datatype, dest, tag, comm, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Send_init failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Sendrecv ( void *sendbuf, int sendcount, EMPI_Datatype sendtype, int dest, int sendtag, void *recvbuf, int recvcount, EMPI_Datatype recvtype, int source, int recvtag, EMPI_Comm comm, EMPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Sendrecv(void *sendbuf=%p, int sendcount=%d, EMPI_Datatype sendtype=%s, int dest=%d, int sendtag=%d, void *recvbuf=%p, int recvcount=%d, EMPI_Datatype recvtype=%s, int source=%d, int recvtag=%d, EMPI_Comm comm=%s, EMPI_Status *status=%p)", sendbuf, sendcount, type_to_string(sendtype), dest, sendtag, recvbuf, recvcount, type_to_string(recvtype), source, recvtag, comm_to_string(comm), status);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(sendtype);
   CHECK_TYPE(recvtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Sendrecv(sendbuf, sendcount, sendtype, dest, sendtag, recvbuf, recvcount, recvtype, source, recvtag, comm, status);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_SEND_RECV, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Sendrecv failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Sendrecv_replace ( void *buf, int count, EMPI_Datatype datatype, int dest, int sendtag, int source, int recvtag, EMPI_Comm comm, EMPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Sendrecv_replace(void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, int dest=%d, int sendtag=%d, int source=%d, int recvtag=%d, EMPI_Comm comm=%s, EMPI_Status *status=%p)", buf, count, type_to_string(datatype), dest, sendtag, source, recvtag, comm_to_string(comm), status);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Sendrecv_replace(buf, count, datatype, dest, sendtag, source, recvtag, comm, status);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_SEND_RECV, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Sendrecv_replace failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Ssend ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Ssend(void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, int dest=%d, int tag=%d, EMPI_Comm comm=%s)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Ssend(buf, count, datatype, dest, tag, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_SSEND, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Ssend failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Ssend_init ( void *buf, int count, EMPI_Datatype datatype, int dest, int tag, EMPI_Comm comm, EMPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Ssend_init(void *buf=%p, int count=%d, EMPI_Datatype datatype=%s, int dest=%d, int tag=%d, EMPI_Comm comm=%s, EMPI_Request *r=%p)", buf, count, type_to_string(datatype), dest, tag, comm_to_string(comm), r);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Ssend_init(buf, count, datatype, dest, tag, comm, r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Ssend_init failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Startall ( int count, EMPI_Request array_of_requests[] )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Startall(int count=%d, EMPI_Request array_of_requests[]=%p)", count, array_of_requests);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Startall(count, array_of_requests);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Startall failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Start ( EMPI_Request *r )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Start(EMPI_Request *r=%p)", r);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Start(r);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Start failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Status_set_cancelled ( EMPI_Status *status, int flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Status_set_cancelled(EMPI_Status *status=%p, int flag=%d)", status, flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Status_set_cancelled(status, flag);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Status_set_cancelled failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Status_set_elements ( EMPI_Status *status, EMPI_Datatype datatype, int count )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Status_set_elements(EMPI_Status *status=%p, EMPI_Datatype datatype=%s, int count=%d)", status, type_to_string(datatype), count);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Status_set_elements(status, datatype, count);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Status_set_elements failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Testall ( int count, EMPI_Request array_of_requests[], int *flag, EMPI_Status array_of_statuses[] )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Testall(int count=%d, EMPI_Request array_of_requests[]=%p, int *flag=%p, EMPI_Status array_of_statuses[]=%p)", count, array_of_requests, flag, array_of_statuses);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Testall(count, array_of_requests, flag, array_of_statuses);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_TESTALL, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Testall failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Testany ( int count, EMPI_Request array_of_requests[], int *index, int *flag, EMPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Testany(int count=%d, EMPI_Request array_of_requests[]=%p, int *index=%p, int *flag=%p, EMPI_Status *status=%p)", count, array_of_requests, index, flag, status);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Testany(count, array_of_requests, index, flag, status);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_TESTANY, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Testany failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Test_cancelled ( EMPI_Status *status, int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Test_cancelled(EMPI_Status *status=%p, int *flag=%p)", status, flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Test_cancelled(status, flag);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Test_cancelled failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Test ( EMPI_Request *r, int *flag, EMPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Test(EMPI_Request *r=%p, int *flag=%p, EMPI_Status *status=%p)", r, flag, status);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Test(r, flag, status);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_TEST, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Test failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Testsome ( int incount, EMPI_Request array_of_requests[], int *outcount, int array_of_indices[], EMPI_Status array_of_statuses[] )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Testsome(int incount=%d, EMPI_Request array_of_requests[]=%p, int *outcount=%p, int array_of_indices[]=%p, EMPI_Status array_of_statuses[]=%p)", incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Testsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_TESTSOME, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Testsome failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Topo_test ( EMPI_Comm comm, int *topo_type )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Topo_test(EMPI_Comm comm=%s, int *topo_type=%p)", comm_to_string(comm), topo_type);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Topo_test(comm, topo_type);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Topo_test failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_commit ( EMPI_Datatype *datatype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_commit(EMPI_Datatype *datatype=%p)", datatype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(*datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_commit(datatype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_commit failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_contiguous ( int count, EMPI_Datatype old_type, EMPI_Datatype *new_type_p )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_contiguous(int count=%d, EMPI_Datatype old_type=%s, EMPI_Datatype *new_type_p=%p)", count, type_to_string(old_type), new_type_p);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(old_type);
   CHECK_TYPE(*new_type_p);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_contiguous(count, old_type, new_type_p);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_contiguous failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_create_darray ( int size, int rank, int ndims, int array_of_gsizes[], int array_of_distribs[], int array_of_dargs[], int array_of_psizes[], int order, EMPI_Datatype oldtype, EMPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_create_darray(int size=%d, int rank=%d, int ndims=%d, int array_of_gsizes[]=%p, int array_of_distribs[]=%p, int array_of_dargs[]=%p, int array_of_psizes[]=%p, int order=%d, EMPI_Datatype oldtype=%s, EMPI_Datatype *newtype=%p)", size, rank, ndims, array_of_gsizes, array_of_distribs, array_of_dargs, array_of_psizes, order, type_to_string(oldtype), newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(oldtype);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_create_darray(size, rank, ndims, array_of_gsizes, array_of_distribs, array_of_dargs, array_of_psizes, order, oldtype, newtype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_create_darray failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_create_hindexed ( int count, int blocklengths[], EMPI_Aint displacements[], EMPI_Datatype oldtype, EMPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_create_hindexed(int count=%d, int blocklengths[]=%p, EMPI_Aint displacements[]=%p, EMPI_Datatype oldtype=%s, EMPI_Datatype *newtype=%p)", count, blocklengths, displacements, type_to_string(oldtype), newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(oldtype);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_create_hindexed(count, blocklengths, displacements, oldtype, newtype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_create_hindexed failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_create_hvector ( int count, int blocklength, EMPI_Aint stride, EMPI_Datatype oldtype, EMPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_create_hvector(int count=%d, int blocklength=%d, EMPI_Aint stride=%p, EMPI_Datatype oldtype=%s, EMPI_Datatype *newtype=%p)", count, blocklength, (void *) stride, type_to_string(oldtype), newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(oldtype);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_create_hvector(count, blocklength, stride, oldtype, newtype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_create_hvector failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_create_indexed_block ( int count, int blocklength, int array_of_displacements[], EMPI_Datatype oldtype, EMPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_create_indexed_block(int count=%d, int blocklength=%d, int array_of_displacements[]=%p, EMPI_Datatype oldtype=%s, EMPI_Datatype *newtype=%p)", count, blocklength, array_of_displacements, type_to_string(oldtype), newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(oldtype);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_create_indexed_block(count, blocklength, array_of_displacements, oldtype, newtype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_create_indexed_block failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_create_keyval ( EMPI_Type_copy_attr_function *type_copy_attr_fn, EMPI_Type_delete_attr_function *type_delete_attr_fn, int *type_keyval, void *extra_state )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_create_keyval(EMPI_Type_copy_attr_function *type_copy_attr_fn=%p, EMPI_Type_delete_attr_function *type_delete_attr_fn=%p, int *type_keyval=%p, void *extra_state=%p)", type_copy_attr_fn, type_delete_attr_fn, type_keyval, extra_state);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_create_keyval(type_copy_attr_fn, type_delete_attr_fn, type_keyval, extra_state);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_create_keyval failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_create_resized ( EMPI_Datatype oldtype, EMPI_Aint lb, EMPI_Aint extent, EMPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_create_resized(EMPI_Datatype oldtype=%s, EMPI_Aint lb=%p, EMPI_Aint extent=%p, EMPI_Datatype *newtype=%p)", type_to_string(oldtype), (void *) lb, (void *) extent, newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(oldtype);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_create_resized(oldtype, lb, extent, newtype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_create_resized failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_create_struct ( int count, int array_of_blocklengths[], EMPI_Aint array_of_displacements[], EMPI_Datatype array_of_types[], EMPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_create_struct(int count=%d, int array_of_blocklengths[]=%p, EMPI_Aint array_of_displacements[]=%p, EMPI_Datatype array_of_types[]=%p, EMPI_Datatype *newtype=%p)", count, array_of_blocklengths, array_of_displacements, array_of_types, newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(array_of_types[]);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_create_struct(count, array_of_blocklengths, array_of_displacements, array_of_types, newtype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_create_struct failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_create_subarray ( int ndims, int array_of_sizes[], int array_of_subsizes[], int array_of_starts[], int order, EMPI_Datatype oldtype, EMPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_create_subarray(int ndims=%d, int array_of_sizes[]=%p, int array_of_subsizes[]=%p, int array_of_starts[]=%p, int order=%d, EMPI_Datatype oldtype=%s, EMPI_Datatype *newtype=%p)", ndims, array_of_sizes, array_of_subsizes, array_of_starts, order, type_to_string(oldtype), newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(oldtype);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_create_subarray(ndims, array_of_sizes, array_of_subsizes, array_of_starts, order, oldtype, newtype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_create_subarray failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_delete_attr ( EMPI_Datatype type, int type_keyval )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_delete_attr(EMPI_Datatype type=%s, int type_keyval=%d)", type_to_string(type), type_keyval);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(type);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_delete_attr(type, type_keyval);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_delete_attr failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_dup ( EMPI_Datatype datatype, EMPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_dup(EMPI_Datatype datatype=%s, EMPI_Datatype *newtype=%p)", type_to_string(datatype), newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_dup(datatype, newtype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_dup failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_extent ( EMPI_Datatype datatype, EMPI_Aint *extent )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_extent(EMPI_Datatype datatype=%s, EMPI_Aint *extent=%p)", type_to_string(datatype), extent);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_extent(datatype, extent);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_extent failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_free ( EMPI_Datatype *datatype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_free(EMPI_Datatype *datatype=%p)", datatype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(*datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_free(datatype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_free failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_free_keyval ( int *type_keyval )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_free_keyval(int *type_keyval=%p)", type_keyval);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_free_keyval(type_keyval);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_free_keyval failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_get_attr ( EMPI_Datatype type, int type_keyval, void *attribute_val, int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_get_attr(EMPI_Datatype type=%s, int type_keyval=%d, void *attribute_val=%p, int *flag=%p)", type_to_string(type), type_keyval, attribute_val, flag);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(type);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_get_attr(type, type_keyval, attribute_val, flag);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_get_attr failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_get_contents ( EMPI_Datatype datatype, int max_integers, int max_addresses, int max_datatypes, int array_of_integers[], EMPI_Aint array_of_addresses[], EMPI_Datatype array_of_datatypes[] )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_get_contents(EMPI_Datatype datatype=%s, int max_integers=%d, int max_addresses=%d, int max_datatypes=%d, int array_of_integers[]=%p, EMPI_Aint array_of_addresses[]=%p, EMPI_Datatype array_of_datatypes[]=%p)", type_to_string(datatype), max_integers, max_addresses, max_datatypes, array_of_integers, array_of_addresses, array_of_datatypes);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
   CHECK_TYPE(array_of_datatypes[]);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_get_contents(datatype, max_integers, max_addresses, max_datatypes, array_of_integers, array_of_addresses, array_of_datatypes);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_get_contents failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_get_envelope ( EMPI_Datatype datatype, int *num_integers, int *num_addresses, int *num_datatypes, int *combiner )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_get_envelope(EMPI_Datatype datatype=%s, int *num_integers=%p, int *num_addresses=%p, int *num_datatypes=%p, int *combiner=%p)", type_to_string(datatype), num_integers, num_addresses, num_datatypes, combiner);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_get_envelope(datatype, num_integers, num_addresses, num_datatypes, combiner);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_get_envelope failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_get_extent ( EMPI_Datatype datatype, EMPI_Aint *lb, EMPI_Aint *extent )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_get_extent(EMPI_Datatype datatype=%s, EMPI_Aint *lb=%p, EMPI_Aint *extent=%p)", type_to_string(datatype), lb, extent);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_get_extent(datatype, lb, extent);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_get_extent failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_get_name ( EMPI_Datatype datatype, char *type_name, int *resultlen )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_get_name(EMPI_Datatype datatype=%s, char *type_name=%p, int *resultlen=%p)", type_to_string(datatype), type_name, resultlen);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_get_name(datatype, type_name, resultlen);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_get_name failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_get_true_extent ( EMPI_Datatype datatype, EMPI_Aint *true_lb, EMPI_Aint *true_extent )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_get_true_extent(EMPI_Datatype datatype=%s, EMPI_Aint *true_lb=%p, EMPI_Aint *true_extent=%p)", type_to_string(datatype), true_lb, true_extent);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_get_true_extent(datatype, true_lb, true_extent);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_get_true_extent failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_hindexed ( int count, int blocklens[], EMPI_Aint indices[], EMPI_Datatype old_type, EMPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_hindexed(int count=%d, int blocklens[]=%p, EMPI_Aint indices[]=%p, EMPI_Datatype old_type=%s, EMPI_Datatype *newtype=%p)", count, blocklens, indices, type_to_string(old_type), newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(old_type);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_hindexed(count, blocklens, indices, old_type, newtype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_hindexed failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_hvector ( int count, int blocklen, EMPI_Aint stride, EMPI_Datatype old_type, EMPI_Datatype *newtype_p )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_hvector(int count=%d, int blocklen=%d, EMPI_Aint stride=%p, EMPI_Datatype old_type=%s, EMPI_Datatype *newtype_p=%p)", count, blocklen, (void *) stride, type_to_string(old_type), newtype_p);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(old_type);
   CHECK_TYPE(*newtype_p);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_hvector(count, blocklen, stride, old_type, newtype_p);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_hvector failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_indexed ( int count, int blocklens[], int indices[], EMPI_Datatype old_type, EMPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_indexed(int count=%d, int blocklens[]=%p, int indices[]=%p, EMPI_Datatype old_type=%s, EMPI_Datatype *newtype=%p)", count, blocklens, indices, type_to_string(old_type), newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(old_type);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_indexed(count, blocklens, indices, old_type, newtype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_indexed failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_lb ( EMPI_Datatype datatype, EMPI_Aint *displacement )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_lb(EMPI_Datatype datatype=%s, EMPI_Aint *displacement=%p)", type_to_string(datatype), displacement);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_lb(datatype, displacement);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_lb failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_match_size ( int typeclass, int size, EMPI_Datatype *datatype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_match_size(int typeclass=%d, int size=%d, EMPI_Datatype *datatype=%p)", typeclass, size, datatype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(*datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_match_size(typeclass, size, datatype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_match_size failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_set_attr ( EMPI_Datatype type, int type_keyval, void *attribute_val )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_set_attr(EMPI_Datatype type=%s, int type_keyval=%d, void *attribute_val=%p)", type_to_string(type), type_keyval, attribute_val);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(type);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_set_attr(type, type_keyval, attribute_val);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_set_attr failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_set_name ( EMPI_Datatype type, char *type_name )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_set_name(EMPI_Datatype type=%s, char *type_name=%p)", type_to_string(type), type_name);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(type);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_set_name(type, type_name);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_set_name failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_size ( EMPI_Datatype datatype, int *size )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_size(EMPI_Datatype datatype=%s, int *size=%p)", type_to_string(datatype), size);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_size(datatype, size);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_size failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_struct ( int count, int blocklens[], EMPI_Aint indices[], EMPI_Datatype old_types[], EMPI_Datatype *newtype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_struct(int count=%d, int blocklens[]=%p, EMPI_Aint indices[]=%p, EMPI_Datatype old_types[]=%p, EMPI_Datatype *newtype=%p)", count, blocklens, indices, old_types, newtype);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(old_types[]);
   CHECK_TYPE(*newtype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_struct(count, blocklens, indices, old_types, newtype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_struct failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_ub ( EMPI_Datatype datatype, EMPI_Aint *displacement )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_ub(EMPI_Datatype datatype=%s, EMPI_Aint *displacement=%p)", type_to_string(datatype), displacement);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_ub(datatype, displacement);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_ub failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Type_vector ( int count, int blocklength, int stride, EMPI_Datatype old_type, EMPI_Datatype *newtype_p )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Type_vector(int count=%d, int blocklength=%d, int stride=%d, EMPI_Datatype old_type=%s, EMPI_Datatype *newtype_p=%p)", count, blocklength, stride, type_to_string(old_type), newtype_p);
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(old_type);
   CHECK_TYPE(*newtype_p);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Type_vector(count, blocklength, stride, old_type, newtype_p);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Type_vector failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Unpack_external ( char *datarep, void *inbuf, EMPI_Aint insize, EMPI_Aint *position, void *outbuf, int outcount, EMPI_Datatype datatype )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Unpack_external(char *datarep=%p, void *inbuf=%p, EMPI_Aint insize=%p, EMPI_Aint *position=%p, void *outbuf=%p, int outcount=%d, EMPI_Datatype datatype=%s)", datarep, inbuf, (void *) insize, position, outbuf, outcount, type_to_string(datatype));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Unpack_external(datarep, inbuf, insize, position, outbuf, outcount, datatype);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Unpack_external failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Unpack ( void *inbuf, int insize, int *position, void *outbuf, int outcount, EMPI_Datatype datatype, EMPI_Comm comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Unpack(void *inbuf=%p, int insize=%d, int *position=%p, void *outbuf=%p, int outcount=%d, EMPI_Datatype datatype=%s, EMPI_Comm comm=%s)", inbuf, insize, position, outbuf, outcount, type_to_string(datatype), comm_to_string(comm));
#endif // TRACE_CALLS

#ifdef CATCH_DERIVED_TYPES
   CHECK_TYPE(datatype);
#endif

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Unpack(inbuf, insize, position, outbuf, outcount, datatype, comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Unpack failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Unpublish_name ( char *service_name, EMPI_Info info, char *port_name )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Unpublish_name(char *service_name=%p, EMPI_Info info=%s, char *port_name=%p)", service_name, info_to_string(info), port_name);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Unpublish_name(service_name, info, port_name);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Unpublish_name failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Waitall ( int count, EMPI_Request array_of_requests[], EMPI_Status array_of_statuses[] )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Waitall(int count=%d, EMPI_Request array_of_requests[]=%p, EMPI_Status array_of_statuses[]=%p)", count, array_of_requests, array_of_statuses);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
#ifndef IBIS_INTERCEPT
   profile_start = profile_start_ticks();
#endif // IBIS_INTERCEPT
#endif // PROFILE_LEVEL

   int error = xEMPI_Waitall(count, array_of_requests, array_of_statuses);

#if PROFILE_LEVEL > 0
#ifndef IBIS_INTERCEPT
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_WAITALL, profile_end-profile_start);
#endif // IBIS_INTERCEPT
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Waitall failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Waitany ( int count, EMPI_Request array_of_requests[], int *index, EMPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Waitany(int count=%d, EMPI_Request array_of_requests[]=%p, int *index=%p, EMPI_Status *status=%p)", count, array_of_requests, index, status);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Waitany(count, array_of_requests, index, status);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_WAITANY, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Waitany failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Wait ( EMPI_Request *r, EMPI_Status *status )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
   EMPI_Comm comm = eEMPI_COMM_SELF;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Wait(EMPI_Request *r=%p, EMPI_Status *status=%p)", r, status);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
   // We need a communicator to attach the statistics to.
   comm = request_get_comm(r, EMPI_COMM_SELF);
#endif

#endif // PROFILE_LEVEL

   int error = xEMPI_Wait(r, status);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_WAIT, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Wait failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Waitsome ( int incount, EMPI_Request array_of_requests[], int *outcount, int array_of_indices[], EMPI_Status array_of_statuses[] )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Waitsome(int incount=%d, EMPI_Request array_of_requests[]=%p, int *outcount=%p, int array_of_indices[]=%p, EMPI_Status array_of_statuses[]=%p)", incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Waitsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_WAITSOME, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Waitsome failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Win_call_errhandler ( EMPI_Win win, int errorcode )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Win_call_errhandler(EMPI_Win win=%s, int errorcode=%d)", win_to_string(win), errorcode);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Win_call_errhandler(win, errorcode);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Win_call_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Win_complete ( EMPI_Win win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Win_complete(EMPI_Win win=%s)", win_to_string(win));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Win_complete(win);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Win_complete failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Win_create_errhandler ( EMPI_Win_errhandler_fn *function, EMPI_Errhandler *errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Win_create_errhandler(EMPI_Win_errhandler_fn *function=%p, EMPI_Errhandler *errhandler=%p)", function, errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Win_create_errhandler(function, errhandler);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Win_create_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Win_create ( void *base, EMPI_Aint size, int disp_unit, EMPI_Info info, EMPI_Comm comm, EMPI_Win *win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Win_create(void *base=%p, EMPI_Aint size=%p, int disp_unit=%d, EMPI_Info info=%s, EMPI_Comm comm=%s, EMPI_Win *win=%p)", base, (void *) size, disp_unit, info_to_string(info), comm_to_string(comm), win);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Win_create(base, size, disp_unit, info, comm, win);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Win_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Win_create_keyval ( EMPI_Win_copy_attr_function *win_copy_attr_fn, EMPI_Win_delete_attr_function *win_delete_attr_fn, int *win_keyval, void *extra_state )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Win_create_keyval(EMPI_Win_copy_attr_function *win_copy_attr_fn=%p, EMPI_Win_delete_attr_function *win_delete_attr_fn=%p, int *win_keyval=%p, void *extra_state=%p)", win_copy_attr_fn, win_delete_attr_fn, win_keyval, extra_state);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Win_create_keyval(win_copy_attr_fn, win_delete_attr_fn, win_keyval, extra_state);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Win_create_keyval failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Win_delete_attr ( EMPI_Win win, int win_keyval )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Win_delete_attr(EMPI_Win win=%s, int win_keyval=%d)", win_to_string(win), win_keyval);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Win_delete_attr(win, win_keyval);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Win_delete_attr failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

int EMPI_Win_fence ( int assert, EMPI_Win win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Win_fence(int assert=%d, EMPI_Win win=%s)", assert, win_to_string(win));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Win_fence(assert, win);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Win_fence failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Win_free ( EMPI_Win *win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Win_free(EMPI_Win *win=%p)", win);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Win_free(win);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Win_free failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Win_free_keyval ( int *win_keyval )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Win_free_keyval(int *win_keyval=%p)", win_keyval);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Win_free_keyval(win_keyval);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Win_free_keyval failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Win_get_attr ( EMPI_Win win, int win_keyval, void *attribute_val, int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Win_get_attr(EMPI_Win win=%s, int win_keyval=%d, void *attribute_val=%p, int *flag=%p)", win_to_string(win), win_keyval, attribute_val, flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Win_get_attr(win, win_keyval, attribute_val, flag);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Win_get_attr failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Win_get_errhandler ( EMPI_Win win, EMPI_Errhandler *errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Win_get_errhandler(EMPI_Win win=%s, EMPI_Errhandler *errhandler=%p)", win_to_string(win), errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Win_get_errhandler(win, errhandler);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Win_get_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Win_get_group ( EMPI_Win win, EMPI_Group *g )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Win_get_group(EMPI_Win win=%s, EMPI_Group *g=%p)", win_to_string(win), g);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Win_get_group(win, g);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Win_get_group failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Win_get_name ( EMPI_Win win, char *win_name, int *resultlen )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Win_get_name(EMPI_Win win=%s, char *win_name=%p, int *resultlen=%p)", win_to_string(win), win_name, resultlen);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Win_get_name(win, win_name, resultlen);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Win_get_name failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Win_lock ( int lock_type, int rank, int assert, EMPI_Win win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Win_lock(int lock_type=%d, int rank=%d, int assert=%d, EMPI_Win win=%s)", lock_type, rank, assert, win_to_string(win));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Win_lock(lock_type, rank, assert, win);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Win_lock failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Win_post ( EMPI_Group g, int assert, EMPI_Win win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Win_post(EMPI_Group g=%s, int assert=%d, EMPI_Win win=%s)", group_to_string(g), assert, win_to_string(win));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Win_post(g, assert, win);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Win_post failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Win_set_attr ( EMPI_Win win, int win_keyval, void *attribute_val )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Win_set_attr(EMPI_Win win=%s, int win_keyval=%d, void *attribute_val=%p)", win_to_string(win), win_keyval, attribute_val);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Win_set_attr(win, win_keyval, attribute_val);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Win_set_attr failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Win_set_errhandler ( EMPI_Win win, EMPI_Errhandler errhandler )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Win_set_errhandler(EMPI_Win win=%s, EMPI_Errhandler errhandler=%p)", win_to_string(win), (void *) errhandler);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Win_set_errhandler(win, errhandler);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Win_set_errhandler failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Win_set_name ( EMPI_Win win, char *win_name )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Win_set_name(EMPI_Win win=%s, char *win_name=%p)", win_to_string(win), win_name);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Win_set_name(win, win_name);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Win_set_name failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Win_start ( EMPI_Group g, int assert, EMPI_Win win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Win_start(EMPI_Group g=%s, int assert=%d, EMPI_Win win=%s)", group_to_string(g), assert, win_to_string(win));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Win_start(g, assert, win);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Win_start failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Win_test ( EMPI_Win win, int *flag )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Win_test(EMPI_Win win=%s, int *flag=%p)", win_to_string(win), flag);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Win_test(win, flag);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Win_test failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Win_unlock ( int rank, EMPI_Win win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Win_unlock(int rank=%d, EMPI_Win win=%s)", rank, win_to_string(win));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Win_unlock(rank, win);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Win_unlock failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}


int EMPI_Win_wait ( EMPI_Win win )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Win_wait(EMPI_Win win=%s)", win_to_string(win));
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Win_wait(win);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(EMPI_COMM_SELF, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Win_wait failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}

/*
int EMPI_Group_comm_create ( EMPI_Comm old_comm, EMPI_Group g, int tag, EMPI_Comm *new_comm )
{
#if PROFILE_LEVEL > 0
   uint64_t profile_start, profile_end;
#endif // PROFILE_LEVEL

#ifdef TRACE_CALLS
   INFO(0, "EMPI_Group_comm_create(EMPI_Comm old_comm=%s, EMPI_Group g=%s, int tag=%d, EMPI_Comm *new_comm=%p)", comm_to_string(old_comm), group_to_string(g), tag, new_comm);
#endif // TRACE_CALLS

#if PROFILE_LEVEL > 0
   profile_start = profile_start_ticks();
#endif // PROFILE_LEVEL

   int error = xEMPI_Group_comm_create(old_comm, g, tag, new_comm);

#if PROFILE_LEVEL > 0
   profile_end = profile_stop_ticks();
   profile_add_statistics(old_comm, STATS_MISC, profile_end-profile_start);
#endif // PROFILE_LEVEL

#ifdef TRACE_ERRORS
   if (error != EMPI_SUCCESS) {
      ERROR(0, "EMPI_Group_comm_create failed (%d)!", error);
   }
#endif // TRACE_ERRORS
   return error;
}
*/

EMPI_Comm EMPI_Comm_f2c ( EMPI_Fint comm )
{
   return xEMPI_Comm_f2c(comm);
}


EMPI_Group EMPI_Group_f2c ( EMPI_Fint g )
{
   return xEMPI_Group_f2c(g);

}


EMPI_Request EMPI_Request_f2c ( EMPI_Fint r )
{
   return xEMPI_Request_f2c(r);

}


EMPI_Info EMPI_Info_f2c ( EMPI_Fint info )
{

   return xEMPI_Info_f2c(info);
}


EMPI_File EMPI_File_f2c ( EMPI_Fint file )
{
   return xEMPI_File_f2c(file);
}


EMPI_Op EMPI_Op_f2c ( EMPI_Fint op )
{
   return xEMPI_Op_f2c(op);
}


EMPI_Win EMPI_Win_f2c ( EMPI_Fint Win )
{
   return xEMPI_Win_f2c(Win);
}


EMPI_Errhandler EMPI_Errhandler_f2c ( EMPI_Fint Errhandler )
{
   return xEMPI_Errhandler_f2c(Errhandler);
}


EMPI_Datatype EMPI_Type_f2c ( EMPI_Fint Type )
{
   return xEMPI_Type_f2c(Type);
}


EMPI_Fint EMPI_Comm_c2f ( EMPI_Comm comm )
{
   return xEMPI_Comm_c2f(comm);
}


EMPI_Fint EMPI_Group_c2f ( EMPI_Group g )
{
   return xEMPI_Group_c2f(g);
}


EMPI_Fint EMPI_Request_c2f ( EMPI_Request r )
{
   return xEMPI_Request_c2f(r);
}


EMPI_Fint EMPI_Info_c2f ( EMPI_Info info )
{
   return xEMPI_Info_c2f(info);
}


EMPI_Fint EMPI_File_c2f ( EMPI_File file )
{
   return xEMPI_File_c2f(file);
}


EMPI_Fint EMPI_Op_c2f ( EMPI_Op op )
{
   return xEMPI_Op_c2f(op);
}


EMPI_Fint EMPI_Win_c2f ( EMPI_Win Win )
{
   return xEMPI_Win_c2f(Win);
}


EMPI_Fint EMPI_Errhandler_c2f ( EMPI_Errhandler Errhandler )
{
   return xEMPI_Errhandler_c2f(Errhandler);
}


EMPI_Fint EMPI_Type_c2f ( EMPI_Datatype Type )
{
   return xEMPI_Type_c2f(Type);
}


#endif

