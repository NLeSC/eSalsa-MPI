#ifndef _DEBUGGING_H_
#define _DEBUGGING_H_

#include <stdlib.h>
#include "settings.h"
#include "communicator.h"

// Macros to check parameters
#ifdef CHECK_PARAMETERS

#define CHECK_COUNT(C)         (check_count(C))
#define CHECK_SOURCE(C,R)      (check_source(C, R))
#define CHECK_DESTINATION(C,R) (check_destination(C, R))

void check_count(int count);
void check_source(communicator *c, int rank);
void check_destination(communicator *c, int rank);

#else

#define CHECK_COUNT(C)
#define CHECK_SOURCE(C,R)
#define CHECK_DESTINATION(C,R)

#endif

#endif


