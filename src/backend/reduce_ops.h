#ifndef _REDUCE_OPS_H_
#define _REDUCE_OPS_H_

#include "empi.h"

void EMPI_REDUCE_MAX(void *invec, void *inoutvec, int *len, EMPI_Datatype *type);
void EMPI_REDUCE_MIN(void *invec, void *inoutvec, int *len, EMPI_Datatype *type);
void EMPI_REDUCE_SUM(void *invec, void *inoutvec, int *len, EMPI_Datatype *type);
void EMPI_REDUCE_PROD(void *invec, void *inoutvec, int *len, EMPI_Datatype *type);
void EMPI_REDUCE_LAND(void *invec, void *inoutvec, int *len, EMPI_Datatype *type);
void EMPI_REDUCE_LOR(void *invec, void *inoutvec, int *len, EMPI_Datatype *type);
void EMPI_REDUCE_LXOR(void *invec, void *inoutvec, int *len, EMPI_Datatype *type);
void EMPI_REDUCE_BAND(void *invec, void *inoutvec, int *len, EMPI_Datatype *type);
void EMPI_REDUCE_BOR(void *invec, void *inoutvec, int *len, EMPI_Datatype *type);
void EMPI_REDUCE_BXOR(void *invec, void *inoutvec, int *len, EMPI_Datatype *type);
void EMPI_REDUCE_MAXLOC(void *invec, void *inoutvec, int *len, EMPI_Datatype *type);
void EMPI_REDUCE_MINLOC(void *invec, void *inoutvec, int *len, EMPI_Datatype *type);

#endif // _REDUCE_OPS_H_
