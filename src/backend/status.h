#ifndef _STATUS_H_
#define _STATUS_H_

#include "datatype.h"
#include "empi.h"

void clear_status(EMPI_Status *s);
void set_status(EMPI_Status *s, int source, int tag, int error, datatype *type, int count, int cancelled);
void set_status_error(EMPI_Status *s, int source, int tag, int error, datatype *type);
void set_status_count(EMPI_Status *s, datatype *type, int count);
void set_status_cancelled(EMPI_Status *s, int flag);

#endif // _STATUS_H_
