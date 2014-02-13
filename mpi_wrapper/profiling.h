#ifndef __PROFILING_H_
#define __PROFILING_H_

#include <stdint.h>
#include "empi.h"

#define STATS_BARRIER         0
#define STATS_SEND            1
#define STATS_RSEND           2
#define STATS_BSEND           3
#define STATS_SSEND           4
#define STATS_ISEND           5
#define STATS_IRSEND          6
#define STATS_IBSEND          7
#define STATS_ISSEND          8
#define STATS_RECV            9
#define STATS_IRECV          10
#define STATS_SEND_RECV      11
#define STATS_IPROBE         12
#define STATS_PROBE          13

#define STATS_BCAST          14
#define STATS_SCATTER        15
#define STATS_GATHER         16
#define STATS_ALLGATHER      17
#define STATS_ALLTOALL       18
#define STATS_REDUCE         19
#define STATS_ALLREDUCE      20
#define STATS_REDUCE_SCATTER 21
#define STATS_SCAN           22
#define STATS_EXSCAN         23

#define STATS_WAIT           24
#define STATS_WAITALL        25
#define STATS_WAITANY        26
#define STATS_WAITSOME       27

#define STATS_TEST           28
#define STATS_TESTALL        29
#define STATS_TESTANY        30
#define STATS_TESTSOME       31

#define STATS_MISC           32
#define STATS_TOTAL          33

#define STATS_NAME_BARRIER        "barrier"
#define STATS_NAME_SEND           "send"
#define STATS_NAME_RSEND          "rsend"
#define STATS_NAME_BSEND          "bsend"
#define STATS_NAME_SSEND          "ssend"
#define STATS_NAME_ISEND          "isend"
#define STATS_NAME_IRSEND         "irsend"
#define STATS_NAME_IBSEND         "ibsend"
#define STATS_NAME_ISSEND         "issend"
#define STATS_NAME_RECV           "receive"
#define STATS_NAME_IRECV          "ireceive"
#define STATS_NAME_SEND_RECV      "send_receive"
#define STATS_NAME_PROBE          "ireceive"
#define STATS_NAME_IPROBE         "iprobe"
#define STATS_NAME_BCAST          "bcast"
#define STATS_NAME_SCATTER        "scatter"
#define STATS_NAME_GATHER         "gather"
#define STATS_NAME_ALLGATHER      "allgather"
#define STATS_NAME_ALLTOALL       "alltoall"
#define STATS_NAME_REDUCE         "reduce"
#define STATS_NAME_ALLREDUCE      "allreduce"
#define STATS_NAME_REDUCE_SCATTER "reduce_scatter"
#define STATS_NAME_SCAN           "scan"
#define STATS_NAME_EXSCAN         "exscan"
#define STATS_NAME_WAIT           "wait"
#define STATS_NAME_WAITALL        "waitall"
#define STATS_NAME_WAITANY        "waitany"
#define STATS_NAME_WAITSOME       "waitsome"
#define STATS_NAME_TEST           "test"
#define STATS_NAME_TESTALL        "testall"
#define STATS_NAME_TESTANY        "testany"
#define STATS_NAME_TESTSOME       "testsome"
#define STATS_NAME_MISC           "misc"
#define STATS_NAME_TOTAL          "total"

static __inline__ uint64_t profile_start_ticks (void) {
  uint32_t cycles_low, cycles_high;
  asm volatile ("CPUID\n\t"
		"RDTSC\n\t"
		"mov %%edx, %0\n\t"
		"mov %%eax, %1\n\t": "=r" (cycles_high), "=r" (cycles_low)::
		"%rax", "%rbx", "%rcx", "%rdx");
  return ((uint64_t)cycles_high << 32) | cycles_low;
}

static __inline__ uint64_t profile_stop_ticks (void) {
  uint32_t cycles_low, cycles_high;
  asm volatile("RDTSCP\n\t"
	       "mov %%edx, %0\n\t"
	       "mov %%eax, %1\n\t"
	       "CPUID\n\t": "=r" (cycles_high), "=r" (cycles_low):: "%rax",
	       "%rbx", "%rcx", "%rdx");

  return ((uint64_t)cycles_high << 32) | cycles_low;
}

void dump_profile_info();

void profile_init();
void profile_finalize();
void profile_add_statistics(EMPI_Comm comm, int field, uint64_t ticks);

//void profile_print_statistics(MPI_Comm comm);
//void profile_print_all_statistics();

#endif // __PROFILING_H_
