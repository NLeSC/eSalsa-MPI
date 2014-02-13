#ifndef _REDUCE_OPS_H_
#define _REDUCE_OPS_H_

#include "flags.h"
#include "empi.h"

#define MAGPIE_HAS_LONG_LONG
#define MAGPIE_HAS_LONG_DOUBLE
#define MAGPIE_HAS_FORTRAN

void MAGPIE_MAX(void *invec, void *inoutvec, int *len, EMPI_Datatype *type);
void MAGPIE_MIN(void *invec, void *inoutvec, int *len, EMPI_Datatype *type);
void MAGPIE_SUM(void *invec, void *inoutvec, int *len, EMPI_Datatype *type);
void MAGPIE_PROD(void *invec, void *inoutvec, int *len, EMPI_Datatype *type);
void MAGPIE_LAND(void *invec, void *inoutvec, int *len, EMPI_Datatype *type);
void MAGPIE_LOR(void *invec, void *inoutvec, int *len, EMPI_Datatype *type);
void MAGPIE_LXOR(void *invec, void *inoutvec, int *len, EMPI_Datatype *type);
void MAGPIE_BAND(void *invec, void *inoutvec, int *len, EMPI_Datatype *type);
void MAGPIE_BOR(void *invec, void *inoutvec, int *len, EMPI_Datatype *type);
void MAGPIE_BXOR(void *invec, void *inoutvec, int *len, EMPI_Datatype *type);
void MAGPIE_MAXLOC(void *invec, void *inoutvec, int *len, EMPI_Datatype *type);
void MAGPIE_MINLOC(void *invec, void *inoutvec, int *len, EMPI_Datatype *type);

#endif // _REDUCE_OPS_H_
