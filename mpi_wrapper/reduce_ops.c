#include "flags.h"
#include "logging.h"
#include "reduce_ops.h"

typedef struct {
  float re;
  float im;
} s_complex;

typedef struct {
  double re;
  double im;
} d_complex;

int MAGPIE_Op_errno;

extern int FORTRAN_FALSE;
extern int FORTRAN_TRUE;

#define boolean_to_logical(b) (((b) == 0)? FORTRAN_FALSE : FORTRAN_TRUE)
#define logical_to_boolean(l) (((l) == FORTRAN_TRUE) ? 1 : 0)

#define MACRO_MAX(A,B) ((A > B) ? A : B)

void MAGPIE_MAX(void *invec, void *inoutvec, int *len, EMPI_Datatype *type) {
  int i;

INFO(2, "MAGPIE_MAX invec=%d inoutvec=%d len=%d type=%s\n", ((int*)invec)[0], ((int*)inoutvec)[0], *len, type_to_string(*type));

  if ( *type == EMPI_INT || *type == EMPI_INTEGER ){

//INFO(2, "MAGPIE_MAX  PERFORMING EMPI_INT MAX!\n");

    int *a = (int*)inoutvec;
    int *b = (int*)invec;
    for (i=0; i < *len; i++) {
      INFO(2, "MAGPIE_MAX  MAX(%d, %d) = %d\n", b[i], a[i], MACRO_MAX(b[i],a[i]));
      a[i] = MACRO_MAX(b[i],a[i]);
    }
    return;
  }
  else if ( *type == EMPI_UNSIGNED ){
    unsigned int *a = (unsigned int *)inoutvec;
    unsigned int *b = (unsigned int *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_MAX(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_LONG ){
    long *a = (long *)inoutvec;
    long *b = (long *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_MAX(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED_LONG ){
    unsigned long *a = (unsigned long *)inoutvec;
    unsigned long *b = (unsigned long *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_MAX(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_SHORT ){
    short *a = (short*)inoutvec;
    short *b = (short*)invec;
    for (i=0; i < *len; i++)
      a[i] = MACRO_MAX(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED_SHORT ){
    unsigned short *a = (unsigned short *)inoutvec;
    unsigned short *b = (unsigned short *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_MAX(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_CHAR ){
    char *a = (char *)inoutvec;
    char *b = (char *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_MAX(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED_CHAR ){
    unsigned char *a = (unsigned char *)inoutvec;
    unsigned char *b = (unsigned char *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_MAX(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_FLOAT || *type == EMPI_REAL4 ){
    float *a = (float *)inoutvec;
    float *b = (float *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_MAX(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_DOUBLE || *type == EMPI_REAL8 || *type == EMPI_DOUBLE_PRECISION ){
    double *a = (double *)inoutvec;
    double *b = (double *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_MAX(b[i],a[i]);
    return;
  }

#ifdef HAVE_MPI_2_2
  else if ( *type == EMPI_LONG_LONG_INT || *type == EMPI_INTEGER8 ){
    long long *a = (long long *)inoutvec;
    long long *b = (long long *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_MAX(b[i],a[i]);
    return;
  }
#endif

#ifdef MAGPIE_HAS_LONG_DOUBLE
  else if ( *type == EMPI_LONG_DOUBLE ){
    long double *a = (long double *)inoutvec;
    long double *b = (long double *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_MAX(b[i],a[i]);
    return;
  }
#endif
  else {
    FATAL("REDUCTION OPERATION FAILED TO FIND CORRECT TYPE!\n");
    MAGPIE_Op_errno = EMPI_ERR_OP;
  }
}

/* //////////////////////////////////////////////////////////////////////// */

#define MACRO_MIN(A,B) ((A < B) ? A : B)

void MAGPIE_MIN(void *invec, void *inoutvec, int *len, EMPI_Datatype *type){
  int i;

INFO(2, "MAGPIE_MIN invec=%d inoutvec=%d len=%d type=%s\n", ((int*)invec)[0], ((int*)inoutvec)[0], *len, type_to_string(*type));

  if ( *type == EMPI_INT || *type == EMPI_INTEGER ){
    int *a = (int*)inoutvec;
    int *b = (int*)invec;
    for (i=0; i < *len; i++)
      a[i] = MACRO_MIN(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED ){
    unsigned int *a = (unsigned int *)inoutvec; 
    unsigned int *b = (unsigned int *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_MIN(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_LONG ){
    long *a = (long *)inoutvec; 
    long *b = (long *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_MIN(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED_LONG ){
    unsigned long *a = (unsigned long *)inoutvec; 
    unsigned long *b = (unsigned long *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_MIN(b[i],a[i]);
    return;
  }
#ifdef HAVE_MPI_2_2
  else if ( *type == EMPI_LONG_LONG_INT || *type == EMPI_INTEGER8 ){
    long long *a = (long long *)inoutvec; 
    long long *b = (long long *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_MIN(b[i],a[i]);
    return;
  }
#endif
  else if ( *type == EMPI_SHORT ){
    short *a = (short*)inoutvec;
    short *b = (short*)invec;
    for (i=0; i < *len; i++)
      a[i] = MACRO_MIN(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED_SHORT ){
    unsigned short *a = (unsigned short *)inoutvec; 
    unsigned short *b = (unsigned short *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_MIN(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_CHAR ){
    char *a = (char *)inoutvec; 
    char *b = (char *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_MIN(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED_CHAR ){
    unsigned char *a = (unsigned char *)inoutvec; 
    unsigned char *b = (unsigned char *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_MIN(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_FLOAT || *type == EMPI_REAL4 ){
    float *a = (float *)inoutvec; 
    float *b = (float *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_MIN(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_DOUBLE || *type == EMPI_REAL8 || *type == EMPI_DOUBLE_PRECISION ){
    double *a = (double *)inoutvec; 
    double *b = (double *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_MIN(b[i],a[i]);
    return;
  }
#ifdef MAGPIE_HAS_LONG_DOUBLE
  else if ( *type == EMPI_LONG_DOUBLE ){
    long double *a = (long double *)inoutvec; 
    long double *b = (long double *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_MIN(b[i],a[i]);
    return;
  }
#endif
  else {
    FATAL("REDUCTION OPERATION FAILED TO FIND CORRECT TYPE!\n");
    MAGPIE_Op_errno = EMPI_ERR_OP;
  }
}


/* //////////////////////////////////////////////////////////////////////// */

#define MACRO_SUM(A,B) ((A) + (B))

void MAGPIE_SUM(void *invec, void *inoutvec, int *len, EMPI_Datatype *type){
  int i;

// BROKEN?

  INFO(2, "MAGPIE_SUM invec=%d inoutvec=%d len=%d type=%s", ((int*)invec)[0], ((int*)inoutvec)[0], *len, type_to_string(*type));

  if ( *type == EMPI_INT || *type == EMPI_INTEGER ){
    int *a = (int*)inoutvec;
    int *b = (int*)invec;
    for (i=0; i < *len; i++) {

      INFO(2, "MAGPIE_SUM in[%d]=%d inout[%d]=%d sum=%d", i, b[i], i, a[i], MACRO_SUM(b[i], a[i]));

      a[i] = MACRO_SUM(b[i],a[i]);
    }
    return;
  }
  else if ( *type == EMPI_UNSIGNED ){
    unsigned int *a = (unsigned int *)inoutvec; 
    unsigned int *b = (unsigned int *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_SUM(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_LONG ){
    long *a = (long *)inoutvec; 
    long *b = (long *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_SUM(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED_LONG ){
    unsigned long *a = (unsigned long *)inoutvec; 
    unsigned long *b = (unsigned long *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_SUM(b[i],a[i]);
    return;
  }
#ifdef HAVE_MPI_2_2
  else if ( *type == EMPI_LONG_LONG_INT || *type == EMPI_INTEGER8 ){
    long long *a = (long long *)inoutvec; 
    long long *b = (long long *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_SUM(b[i],a[i]);
    return;
  }
#endif
  else if ( *type == EMPI_SHORT ){
    short *a = (short*)inoutvec;
    short *b = (short*)invec;
    for (i=0; i < *len; i++)
      a[i] = MACRO_SUM(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED_SHORT ){
    unsigned short *a = (unsigned short *)inoutvec; 
    unsigned short *b = (unsigned short *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_SUM(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_CHAR ){
    char *a = (char *)inoutvec; 
    char *b = (char *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_SUM(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED_CHAR ){
    unsigned char *a = (unsigned char *)inoutvec; 
    unsigned char *b = (unsigned char *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_SUM(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_FLOAT || *type == EMPI_REAL4 ){
    float *a = (float *)inoutvec; 
    float *b = (float *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_SUM(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_DOUBLE || *type == EMPI_REAL8 || *type == EMPI_DOUBLE_PRECISION ){
    double *a = (double *)inoutvec; 
    double *b = (double *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_SUM(b[i],a[i]);
    return;
  }
#ifdef MAGPIE_HAS_LONG_DOUBLE
  else if ( *type == EMPI_LONG_DOUBLE ){
    long double *a = (long double *)inoutvec; 
    long double *b = (long double *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_SUM(b[i],a[i]);
    return;
  }
#endif
#ifdef MAGPIE_HAS_FORTRAN
  else if ( *type == EMPI_COMPLEX ){
    s_complex *a = (s_complex *)inoutvec;
    s_complex *b = (s_complex *)invec;
    for ( i=0; i < *len; i++ ){
      a[i].re = MACRO_SUM(a[i].re,b[i].re);
      a[i].im = MACRO_SUM(a[i].im,b[i].im);
    }
    return;
  }
  else if ( *type == EMPI_DOUBLE_COMPLEX ){
    d_complex *a = (d_complex *)inoutvec;
    d_complex *b = (d_complex *)invec;
    for ( i=0; i < *len; i++ ){
      a[i].re = MACRO_SUM(a[i].re,b[i].re);
      a[i].im = MACRO_SUM(a[i].im,b[i].im);
    }
    return;
  }
#endif
  else {
    FATAL("REDUCTION OPERATION FAILED TO FIND CORRECT TYPE!\n");
    MAGPIE_Op_errno = EMPI_ERR_OP;
  }
}


/* //////////////////////////////////////////////////////////////////////// */

#define MACRO_PROD(A,B) ((A) * (B))

void MAGPIE_PROD(void *invec, void *inoutvec, int *len, EMPI_Datatype *type){
  int i;

INFO(2, "MAGPIE_PROD invec=%d inoutvec=%d len=%d type=%s\n", ((int*)invec)[0], ((int*)inoutvec)[0], *len, type_to_string(*type));

  if ( *type == EMPI_INT || *type == EMPI_INTEGER ){
    int *a = (int*)inoutvec;
    int *b = (int*)invec;
    for (i=0; i < *len; i++)
      a[i] = MACRO_PROD(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED ){
    unsigned int *a = (unsigned int *)inoutvec; 
    unsigned int *b = (unsigned int *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_PROD(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_LONG ){
    long *a = (long *)inoutvec; 
    long *b = (long *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_PROD(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED_LONG ){
    unsigned long *a = (unsigned long *)inoutvec; 
    unsigned long *b = (unsigned long *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_PROD(b[i],a[i]);
    return;
  }
#ifdef HAVE_MPI_2_2
  else if ( *type == EMPI_LONG_LONG_INT || *type == EMPI_INTEGER8 ){
    long long *a = (long long *)inoutvec; 
    long long *b = (long long *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_PROD(b[i],a[i]);
    return;
  }
#endif
  else if ( *type == EMPI_SHORT ){
    short *a = (short*)inoutvec;
    short *b = (short*)invec;
    for (i=0; i < *len; i++)
      a[i] = MACRO_PROD(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED_SHORT ){
    unsigned short *a = (unsigned short *)inoutvec; 
    unsigned short *b = (unsigned short *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_PROD(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_CHAR ){
    char *a = (char *)inoutvec; 
    char *b = (char *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_PROD(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED_CHAR ){
    unsigned char *a = (unsigned char *)inoutvec; 
    unsigned char *b = (unsigned char *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_PROD(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_FLOAT || *type == EMPI_REAL4 ){
    float *a = (float *)inoutvec; 
    float *b = (float *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_PROD(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_DOUBLE || *type == EMPI_REAL8 || *type == EMPI_DOUBLE_PRECISION ){
    double *a = (double *)inoutvec; 
    double *b = (double *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_PROD(b[i],a[i]);
    return;
  }
#ifdef MAGPIE_HAS_LONG_DOUBLE
  else if ( *type == EMPI_LONG_DOUBLE ){
    long double *a = (long double *)inoutvec; 
    long double *b = (long double *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_PROD(b[i],a[i]);
    return;
  }
#endif
#ifdef MAGPIE_HAS_FORTRAN
  else if ( *type == EMPI_COMPLEX ){
    s_complex *a = (s_complex *)inoutvec;
    s_complex *b = (s_complex *)invec;
    s_complex c;
    for ( i=0; i < *len; i++ ){
      c.re = a[i].re; c.im = a[i].im;
      a[i].re = c.re*b[i].re - c.im*b[i].im;
      a[i].im = c.im*b[i].re + c.re*b[i].im;
    }
    return;
  }
  else if ( *type == EMPI_DOUBLE_COMPLEX ){
    d_complex *a = (d_complex *)inoutvec;
    d_complex *b = (d_complex *)invec;
    d_complex c;
    for ( i=0; i < *len; i++ ){
      c.re = a[i].re; c.im = a[i].im;
      a[i].re = c.re*b[i].re - c.im*b[i].im;
      a[i].im = c.im*b[i].re + c.re*b[i].im;
    }
    return;
  }
#endif
  else {
    FATAL("REDUCTION OPERATION FAILED TO FIND CORRECT TYPE!\n");
    MAGPIE_Op_errno = EMPI_ERR_OP;
  }
}


/* //////////////////////////////////////////////////////////////////////// */

#define MACRO_LAND(A,B) ((A) && (B))

void MAGPIE_LAND(void *invec, void *inoutvec, int *len, EMPI_Datatype *type){
  int i;

INFO(2, "MAGPIE_LAND invec=%d inoutvec=%d len=%d type=%s\n", ((int*)invec)[0], ((int*)inoutvec)[0], *len, type_to_string(*type));

  if ( *type == EMPI_INT || *type == EMPI_INTEGER ){
    int *a = (int*)inoutvec;
    int *b = (int*)invec;
    for (i=0; i < *len; i++)
      a[i] = MACRO_LAND(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED ){
    unsigned int *a = (unsigned int *)inoutvec; 
    unsigned int *b = (unsigned int *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_LAND(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_LONG ){
    long *a = (long *)inoutvec; 
    long *b = (long *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_LAND(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED_LONG ){
    unsigned long *a = (unsigned long *)inoutvec; 
    unsigned long *b = (unsigned long *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_LAND(b[i],a[i]);
    return;
  }
#ifdef HAVE_MPI_2_2
  else if ( *type == EMPI_LONG_LONG_INT || *type == EMPI_INTEGER8 ){
    long long *a = (long long *)inoutvec; 
    long long *b = (long long *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_LAND(b[i],a[i]);
    return;
  }
#endif
  else if ( *type == EMPI_SHORT ){
    short *a = (short*)inoutvec;
    short *b = (short*)invec;
    for (i=0; i < *len; i++)
      a[i] = MACRO_LAND(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED_SHORT ){
    unsigned short *a = (unsigned short *)inoutvec; 
    unsigned short *b = (unsigned short *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_LAND(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_CHAR ){
    char *a = (char *)inoutvec; 
    char *b = (char *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_LAND(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED_CHAR ){
    unsigned char *a = (unsigned char *)inoutvec; 
    unsigned char *b = (unsigned char *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_LAND(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_FLOAT || *type == EMPI_REAL4 ){
    float *a = (float *)inoutvec; 
    float *b = (float *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_LAND(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_DOUBLE || *type == EMPI_REAL8 || *type == EMPI_DOUBLE_PRECISION ){
    double *a = (double *)inoutvec; 
    double *b = (double *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_LAND(b[i],a[i]);
    return;
  }
#ifdef MAGPIE_HAS_LONG_DOUBLE
  else if ( *type == EMPI_LONG_DOUBLE ){
    long double *a = (long double *)inoutvec; 
    long double *b = (long double *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_LAND(b[i],a[i]);
    return;
  }
#endif
#ifdef MAGPIE_HAS_FORTRAN
  else if ( *type == EMPI_LOGICAL ){
    int *a = (int *)inoutvec;
    int *b = (int *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = boolean_to_logical(MACRO_LAND(logical_to_boolean(a[i]),
					   logical_to_boolean(b[i])));
    return;
  }
#endif
  else {
    FATAL("REDUCTION OPERATION FAILED TO FIND CORRECT TYPE!\n");
    MAGPIE_Op_errno = EMPI_ERR_OP;
  }
}


/* //////////////////////////////////////////////////////////////////////// */

#define MACRO_LOR(A,B) ((A) || (B))

void MAGPIE_LOR(void *invec, void *inoutvec, int *len, EMPI_Datatype *type){
  int i;

INFO(2, "MAGPIE_LOR invec=%d inoutvec=%d len=%d type=%s\n", ((int*)invec)[0], ((int*)inoutvec)[0], *len, type_to_string(*type));

  if ( *type == EMPI_INT || *type == EMPI_INTEGER ){
    int *a = (int*)inoutvec;
    int *b = (int*)invec;
    for (i=0; i < *len; i++)
      a[i] = MACRO_LOR(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED ){
    unsigned int *a = (unsigned int *)inoutvec; 
    unsigned int *b = (unsigned int *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_LOR(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_LONG ){
    long *a = (long *)inoutvec; 
    long *b = (long *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_LOR(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED_LONG ){
    unsigned long *a = (unsigned long *)inoutvec; 
    unsigned long *b = (unsigned long *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_LOR(b[i],a[i]);
    return;
  }
#ifdef HAVE_MPI_2_2
  else if ( *type == EMPI_LONG_LONG_INT || *type == EMPI_INTEGER8 ){
    long long *a = (long long *)inoutvec; 
    long long *b = (long long *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_LOR(b[i],a[i]);
    return;
  }
#endif
  else if ( *type == EMPI_SHORT ){
    short *a = (short*)inoutvec;
    short *b = (short*)invec;
    for (i=0; i < *len; i++)
      a[i] = MACRO_LOR(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED_SHORT ){
    unsigned short *a = (unsigned short *)inoutvec; 
    unsigned short *b = (unsigned short *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_LOR(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_CHAR ){
    char *a = (char *)inoutvec; 
    char *b = (char *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_LOR(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED_CHAR ){
    unsigned char *a = (unsigned char *)inoutvec; 
    unsigned char *b = (unsigned char *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_LOR(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_FLOAT || *type == EMPI_REAL4 ){
    float *a = (float *)inoutvec; 
    float *b = (float *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_LOR(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_DOUBLE || *type == EMPI_REAL8 || *type == EMPI_DOUBLE_PRECISION ){
    double *a = (double *)inoutvec; 
    double *b = (double *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_LOR(b[i],a[i]);
    return;
  }
#ifdef MAGPIE_HAS_LONG_DOUBLE
  else if ( *type == EMPI_LONG_DOUBLE ){
    long double *a = (long double *)inoutvec; 
    long double *b = (long double *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_LOR(b[i],a[i]);
    return;
  }
#endif
#ifdef MAGPIE_HAS_FORTRAN
  else if ( *type == EMPI_LOGICAL ){
    int *a = (int *)inoutvec;
    int *b = (int *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = boolean_to_logical(MACRO_LOR(logical_to_boolean(a[i]),
					  logical_to_boolean(b[i])));
    return;
  }
#endif
  else {
    FATAL("REDUCTION OPERATION FAILED TO FIND CORRECT TYPE!\n");
    MAGPIE_Op_errno = EMPI_ERR_OP;
  }
}


/* //////////////////////////////////////////////////////////////////////// */

#define MACRO_LXOR(A,B) ( ((A)&&(!B)) || ((!A)&&(B)) )

void MAGPIE_LXOR(void *invec, void *inoutvec, int *len, EMPI_Datatype *type){
  int i;

INFO(2, "MAGPIE_LXOR invec=%d inoutvec=%d len=%d type=%s\n", ((int*)invec)[0], ((int*)inoutvec)[0], *len, type_to_string(*type));

  if ( *type == EMPI_INT || *type == EMPI_INTEGER ){
    int *a = (int*)inoutvec;
    int *b = (int*)invec;
    for (i=0; i < *len; i++)
      a[i] = MACRO_LXOR(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED ){
    unsigned int *a = (unsigned int *)inoutvec; 
    unsigned int *b = (unsigned int *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_LXOR(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_LONG ){
    long *a = (long *)inoutvec; 
    long *b = (long *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_LXOR(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED_LONG ){
    unsigned long *a = (unsigned long *)inoutvec; 
    unsigned long *b = (unsigned long *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_LXOR(b[i],a[i]);
    return;
  }
#ifdef HAVE_MPI_2_2
  else if ( *type == EMPI_LONG_LONG_INT || *type == EMPI_INTEGER8 ){
    long long *a = (long long *)inoutvec; 
    long long *b = (long long *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_LXOR(b[i],a[i]);
    return;
  }
#endif
  else if ( *type == EMPI_SHORT ){
    short *a = (short*)inoutvec;
    short *b = (short*)invec;
    for (i=0; i < *len; i++)
      a[i] = MACRO_LXOR(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED_SHORT ){
    unsigned short *a = (unsigned short *)inoutvec; 
    unsigned short *b = (unsigned short *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_LXOR(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_CHAR ){
    char *a = (char *)inoutvec; 
    char *b = (char *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_LXOR(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED_CHAR ){
    unsigned char *a = (unsigned char *)inoutvec; 
    unsigned char *b = (unsigned char *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_LXOR(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_FLOAT || *type == EMPI_REAL4 ){
    float *a = (float *)inoutvec; 
    float *b = (float *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_LXOR(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_DOUBLE || *type == EMPI_REAL8 || *type == EMPI_DOUBLE_PRECISION ){
    double *a = (double *)inoutvec; 
    double *b = (double *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_LXOR(b[i],a[i]);
    return;
  }
#ifdef MAGPIE_HAS_LONG_DOUBLE
  else if ( *type == EMPI_LONG_DOUBLE ){
    long double *a = (long double *)inoutvec; 
    long double *b = (long double *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_LXOR(b[i],a[i]);
    return;
  }
#endif
#ifdef MAGPIE_HAS_FORTRAN
  else if ( *type == EMPI_LOGICAL ){
    int *a = (int *)inoutvec;
    int *b = (int *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = boolean_to_logical(MACRO_LXOR(logical_to_boolean(a[i]),
					   logical_to_boolean(b[i])));
    return;
  }
#endif
  else {
    FATAL("REDUCTION OPERATION FAILED TO FIND CORRECT TYPE!\n");
    MAGPIE_Op_errno = EMPI_ERR_OP;
  }
}

/* //////////////////////////////////////////////////////////////////////// */

#define MACRO_BAND(A,B) ((A) & (B))

void MAGPIE_BAND(void *invec, void *inoutvec, int *len, EMPI_Datatype *type){
  int i;

INFO(2, "MAGPIE_BAND invec=%d inoutvec=%d len=%d type=%s\n", ((int*)invec)[0], ((int*)inoutvec)[0], *len, type_to_string(*type));

  if ( *type == EMPI_INT || *type == EMPI_INTEGER ){
    int *a = (int*)inoutvec;
    int *b = (int*)invec;
    for (i=0; i < *len; i++)
      a[i] = MACRO_BAND(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED ){
    unsigned int *a = (unsigned int *)inoutvec; 
    unsigned int *b = (unsigned int *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_BAND(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_LONG ){
    long *a = (long *)inoutvec; 
    long *b = (long *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_BAND(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED_LONG ){
    unsigned long *a = (unsigned long *)inoutvec; 
    unsigned long *b = (unsigned long *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_BAND(b[i],a[i]);
    return;
  }
#ifdef HAVE_MPI_2_2
  else if ( *type == EMPI_LONG_LONG_INT || *type == EMPI_INTEGER8 ){
    long long *a = (long long *)inoutvec; 
    long long *b = (long long *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_BAND(b[i],a[i]);
    return;
  }
#endif
  else if ( *type == EMPI_SHORT ){
    short *a = (short*)inoutvec;
    short *b = (short*)invec;
    for (i=0; i < *len; i++)
      a[i] = MACRO_BAND(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED_SHORT ){
    unsigned short *a = (unsigned short *)inoutvec; 
    unsigned short *b = (unsigned short *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_BAND(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_CHAR ){
    char *a = (char *)inoutvec; 
    char *b = (char *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_BAND(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED_CHAR ){
    unsigned char *a = (unsigned char *)inoutvec; 
    unsigned char *b = (unsigned char *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_BAND(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_BYTE ){
    unsigned char *a = (unsigned char *)inoutvec; 
    unsigned char *b = (unsigned char *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_BAND(b[i],a[i]);
    return;
  }
  else {
    FATAL("REDUCTION OPERATION FAILED TO FIND CORRECT TYPE!\n");
    MAGPIE_Op_errno = EMPI_ERR_OP;
  }
}



/* //////////////////////////////////////////////////////////////////////// */

#define MACRO_BOR(A,B) ((A) | (B))

void MAGPIE_BOR(void *invec, void *inoutvec, int *len, EMPI_Datatype *type){
  int i;

INFO(2, "MAGPIE_BOR invec=%d inoutvec=%d len=%d type=%s\n", ((int*)invec)[0], ((int*)inoutvec)[0], *len, type_to_string(*type));

  if ( *type == EMPI_INT || *type == EMPI_INTEGER ){
    int *a = (int*)inoutvec;
    int *b = (int*)invec;
    for (i=0; i < *len; i++)
      a[i] = MACRO_BOR(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED ){
    unsigned int *a = (unsigned int *)inoutvec; 
    unsigned int *b = (unsigned int *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_BOR(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_LONG ){
    long *a = (long *)inoutvec; 
    long *b = (long *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_BOR(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED_LONG ){
    unsigned long *a = (unsigned long *)inoutvec; 
    unsigned long *b = (unsigned long *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_BOR(b[i],a[i]);
    return;
  }
#ifdef HAVE_MPI_2_2
  else if ( *type == EMPI_LONG_LONG_INT || *type == EMPI_INTEGER8 ){
    long long *a = (long long *)inoutvec; 
    long long *b = (long long *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_BOR(b[i],a[i]);
    return;
  }
#endif
  else if ( *type == EMPI_SHORT ){
    short *a = (short*)inoutvec;
    short *b = (short*)invec;
    for (i=0; i < *len; i++)
      a[i] = MACRO_BOR(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED_SHORT ){
    unsigned short *a = (unsigned short *)inoutvec; 
    unsigned short *b = (unsigned short *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_BOR(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_CHAR ){
    char *a = (char *)inoutvec; 
    char *b = (char *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_BOR(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED_CHAR ){
    unsigned char *a = (unsigned char *)inoutvec; 
    unsigned char *b = (unsigned char *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_BOR(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_BYTE ){
    unsigned char *a = (unsigned char *)inoutvec; 
    unsigned char *b = (unsigned char *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_BOR(b[i],a[i]);
    return;
  }
  else {
    FATAL("REDUCTION OPERATION FAILED TO FIND CORRECT TYPE!\n");
    MAGPIE_Op_errno = EMPI_ERR_OP;
  }
}


/* //////////////////////////////////////////////////////////////////////// */

#define MACRO_BXOR(A,B) ((A) ^ (B))

void MAGPIE_BXOR(void *invec, void *inoutvec, int *len, EMPI_Datatype *type){
  int i;

INFO(2, "MAGPIE_BXOR invec=%d inoutvec=%d len=%d type=%s\n", ((int*)invec)[0], ((int*)inoutvec)[0], *len, type_to_string(*type));

  if ( *type == EMPI_INT || *type == EMPI_INTEGER ){
    int *a = (int*)inoutvec;
    int *b = (int*)invec;
    for (i=0; i < *len; i++)
      a[i] = MACRO_BXOR(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED ){
    unsigned int *a = (unsigned int *)inoutvec; 
    unsigned int *b = (unsigned int *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_BXOR(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_LONG ){
    long *a = (long *)inoutvec; 
    long *b = (long *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_BXOR(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED_LONG ){
    unsigned long *a = (unsigned long *)inoutvec; 
    unsigned long *b = (unsigned long *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_BXOR(b[i],a[i]);
    return;
  }
#ifdef HAVE_MPI_2_2
  else if ( *type == EMPI_LONG_LONG_INT || *type == EMPI_INTEGER8 ){
    long long *a = (long long *)inoutvec; 
    long long *b = (long long *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_BXOR(b[i],a[i]);
    return;
  }
#endif
  else if ( *type == EMPI_SHORT ){
    short *a = (short*)inoutvec;
    short *b = (short*)invec;
    for (i=0; i < *len; i++)
      a[i] = MACRO_BXOR(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED_SHORT ){
    unsigned short *a = (unsigned short *)inoutvec; 
    unsigned short *b = (unsigned short *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_BXOR(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_CHAR ){
    char *a = (char *)inoutvec; 
    char *b = (char *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_BXOR(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_UNSIGNED_CHAR ){
    unsigned char *a = (unsigned char *)inoutvec; 
    unsigned char *b = (unsigned char *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_BXOR(b[i],a[i]);
    return;
  }
  else if ( *type == EMPI_BYTE ){
    unsigned char *a = (unsigned char *)inoutvec; 
    unsigned char *b = (unsigned char *)invec;
    for ( i=0; i < *len; i++ )
      a[i] = MACRO_BXOR(b[i],a[i]);
    return;
  }
  else {
    FATAL("REDUCTION OPERATION FAILED TO FIND CORRECT TYPE!\n");
    MAGPIE_Op_errno = EMPI_ERR_OP;
  }
}

/* //////////////////////////////////////////////////////////////////////// */

/* a few dummy structs for minloc and maxloc */

typedef struct {
  int value;
  int loc;
} magpie_2int_type;

typedef struct {
  short value;
  int loc;
} magpie_shortint_type;

typedef struct {
  long value;
  int loc;
} magpie_longint_type;

typedef struct {
  float value;
  int loc;
} magpie_floatint_type;

typedef struct {
  double value;
  int loc;
} magpie_doubleint_type;


void MAGPIE_MAXLOC(void *invec, void *inoutvec, int *len, EMPI_Datatype *type){
  int i;

INFO(2, "MAGPIE_MAXLOC invec=%d inoutvec=%d len=%d type=%s\n", ((int*)invec)[0], ((int*)inoutvec)[0], *len, type_to_string(*type));

  if ( *type == EMPI_2INT ){
    magpie_2int_type *a = (magpie_2int_type*)inoutvec;
    magpie_2int_type *b = (magpie_2int_type*)invec;

    for (i=0; i < *len; i++){
      if ( a[i].value == b[i].value ){
	a[i].loc = MACRO_MIN(a[i].loc,b[i].loc);
      }
      else if (a[i].value < b[i].value){
	a[i].value = b[i].value;
	a[i].loc   = b[i].loc;
      }
    }
    return;
  }
  else if ( *type == EMPI_SHORT_INT ){
    magpie_shortint_type *a = (magpie_shortint_type*)inoutvec;
    magpie_shortint_type *b = (magpie_shortint_type*)invec;

    for (i=0; i < *len; i++){
      if ( a[i].value == b[i].value ){
	a[i].loc = MACRO_MIN(a[i].loc,b[i].loc);
      }
      else if (a[i].value < b[i].value){
	a[i].value = b[i].value;
	a[i].loc   = b[i].loc;
      }
    }
    return;
  }
  else if ( *type == EMPI_LONG_INT ){
    magpie_longint_type *a = (magpie_longint_type*)inoutvec;
    magpie_longint_type *b = (magpie_longint_type*)invec;

    for (i=0; i < *len; i++){
      if ( a[i].value == b[i].value ){
	a[i].loc = MACRO_MIN(a[i].loc,b[i].loc);
      }
      else if (a[i].value < b[i].value){
	a[i].value = b[i].value;
	a[i].loc   = b[i].loc;
      }
    }
    return;
  }
  else if ( *type == EMPI_FLOAT_INT ){
    magpie_floatint_type *a = (magpie_floatint_type*)inoutvec;
    magpie_floatint_type *b = (magpie_floatint_type*)invec;

    for (i=0; i < *len; i++){
      if ( a[i].value == b[i].value ){
	a[i].loc = MACRO_MIN(a[i].loc,b[i].loc);
      }
      else if (a[i].value < b[i].value){
	a[i].value = b[i].value;
	a[i].loc   = b[i].loc;
      }
    }
    return;
  }
  else if ( *type == EMPI_DOUBLE_INT ){
    magpie_doubleint_type *a = (magpie_doubleint_type*)inoutvec;
    magpie_doubleint_type *b = (magpie_doubleint_type*)invec;

    for (i=0; i < *len; i++){
      if ( a[i].value == b[i].value ){
	a[i].loc = MACRO_MIN(a[i].loc,b[i].loc);
      }
      else if (a[i].value < b[i].value){
	a[i].value = b[i].value;
	a[i].loc   = b[i].loc;
      }
    }
    return;
  }
  else {
    FATAL("REDUCTION OPERATION FAILED TO FIND CORRECT TYPE!\n");
    MAGPIE_Op_errno = EMPI_ERR_OP;
  }
}



void MAGPIE_MINLOC(void *invec, void *inoutvec, int *len, EMPI_Datatype *type){
  int i;

INFO(2, "MAGPIE_MINLOC invec=%d inoutvec=%d len=%d type=%s\n", ((int*)invec)[0], ((int*)inoutvec)[0], *len, type_to_string(*type));

  if ( *type == EMPI_2INT ){
    magpie_2int_type *a = (magpie_2int_type*)inoutvec;
    magpie_2int_type *b = (magpie_2int_type*)invec;

    for (i=0; i < *len; i++){
      if ( a[i].value == b[i].value ){
	a[i].loc = MACRO_MIN(a[i].loc,b[i].loc);
      }
      else if (a[i].value > b[i].value){
	a[i].value = b[i].value;
	a[i].loc   = b[i].loc;
      }
    }
    return;
  }
  else if ( *type == EMPI_SHORT_INT ){
    magpie_shortint_type *a = (magpie_shortint_type*)inoutvec;
    magpie_shortint_type *b = (magpie_shortint_type*)invec;

    for (i=0; i < *len; i++){
      if ( a[i].value == b[i].value ){
	a[i].loc = MACRO_MIN(a[i].loc,b[i].loc);
      }
      else if (a[i].value > b[i].value){
	a[i].value = b[i].value;
	a[i].loc   = b[i].loc;
      }
    }
    return;
  }
  else if ( *type == EMPI_LONG_INT ){
    magpie_longint_type *a = (magpie_longint_type*)inoutvec;
    magpie_longint_type *b = (magpie_longint_type*)invec;

    for (i=0; i < *len; i++){
      if ( a[i].value == b[i].value ){
	a[i].loc = MACRO_MIN(a[i].loc,b[i].loc);
      }
      else if (a[i].value > b[i].value){
	a[i].value = b[i].value;
	a[i].loc   = b[i].loc;
      }
    }
    return;
  }
  else if ( *type == EMPI_FLOAT_INT ){
    magpie_floatint_type *a = (magpie_floatint_type*)inoutvec;
    magpie_floatint_type *b = (magpie_floatint_type*)invec;

    for (i=0; i < *len; i++){
      if ( a[i].value == b[i].value ){
	a[i].loc = MACRO_MIN(a[i].loc,b[i].loc);
      }
      else if (a[i].value > b[i].value){
	a[i].value = b[i].value;
	a[i].loc   = b[i].loc;
      }
    }
    return;
  }
  else if ( *type == EMPI_DOUBLE_INT ){
    magpie_doubleint_type *a = (magpie_doubleint_type*)inoutvec;
    magpie_doubleint_type *b = (magpie_doubleint_type*)invec;

    for (i=0; i < *len; i++){
      if ( a[i].value == b[i].value ){
	a[i].loc = MACRO_MIN(a[i].loc,b[i].loc);
      }
      else if (a[i].value > b[i].value){
	a[i].value = b[i].value;
	a[i].loc   = b[i].loc;
      }
    }
    return;
  }
  else {
    FATAL("REDUCTION OPERATION FAILED TO FIND CORRECT TYPE!\n");
    MAGPIE_Op_errno = EMPI_ERR_OP;
  }
}

