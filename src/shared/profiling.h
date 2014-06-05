#ifndef __PROFILING_H_
#define __PROFILING_H_

#define MAX_PROFILES 1024
#define FIELDS_PER_PROFILE 33

#include <stdint.h>

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

void profile_init(char *field_names[FIELDS_PER_PROFILE+1]);
void profile_finalize();
void profile_add_statistics(int comm, int field, uint64_t ticks);
void dump_profile_info();

#endif // __PROFILING_H_
