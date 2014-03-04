#include "logging.h"
#include "reduce_ops.h"
#include "../../include/settings.h"
#include "empi.h"

// According to the MPI 2.1 standard, the following combinations of reduce operations and types are supported:
//
// MPI_MAX, MPI_MIN              C integer, Fortran integer, Floating point
// MPI_SUM, MPI_PROD             C integer, Fortran integer, Floating point, Complex
// MPI_LAND, MPI_LOR, MPI_LXOR   C integer, Logical
// MPI_BAND, MPI_BOR, MPI_BXOR   C integer, Fortran integer, Byte
//
// We will generate the necessary reduce functions below.

// A few structs describing special datatypes.

// MPI_COMPLEX
typedef struct {
  float re;
  float im;
} empi_complex_type;

// MPI_DOUBLE_COMPLEX
typedef struct {
  double re;
  double im;
} empi_doublecomplex_type;

// MPI_2INT and MPI_2INTEGER
typedef struct {
  int value;
  int loc;
} empi_2int_type;

// MPI_2REAL
typedef struct {
  float value;
  float loc;
} empi_2real_type;

// MPI_2DOUBLE_PRECISION
typedef struct {
  double value;
  double loc;
} empi_2doubleprecision_type;

// MPI_SHORT_INT
typedef struct {
  short value;
  int loc;
} empi_shortint_type;

// MPI_LONG_INT
typedef struct {
  long value;
  int loc;
} empi_longint_type;

// MPI_FLOAT_INT
typedef struct {
  float value;
  int loc;
} empi_floatint_type;

// MPI_DOUBLE_INT
typedef struct {
  double value;
  int loc;
} empi_doubleint_type;

// MPI_LONG_DOUBLE_INT
typedef struct {
  double value;
  int loc;
} empi_longdoubleint_type;


// Variable to return errno.
int EMPI_Op_errno;

extern int FORTRAN_FALSE;
extern int FORTRAN_TRUE;

#define boolean_to_logical(b) (((b) == 0)? FORTRAN_FALSE : FORTRAN_TRUE)
#define logical_to_boolean(l) (((l) == FORTRAN_TRUE) ? 1 : 0)

#define MACRO_MAX(A,B) ((A > B) ? A : B)
#define MACRO_MIN(A,B) ((A < B) ? A : B)
#define MACRO_SUM(A,B) ((A) + (B))
#define MACRO_PROD(A,B) ((A) * (B))
#define MACRO_LAND(A,B) ((A) && (B))
#define MACRO_LOR(A,B) ((A) || (B))
#define MACRO_LXOR(A,B) ( ((A)&&(!B)) || ((!A)&&(B)) )
#define MACRO_BAND(A,B) ((A) & (B))
#define MACRO_BOR(A,B) ((A) | (B))
#define MACRO_BXOR(A,B) ((A) ^ (B))

#define REDUCE_FUNC(FUNC, NAME, TYPE) \
static void EMPI_##FUNC##_##NAME(TYPE *invec, TYPE *inoutvec, int len) \
{\
   int i;\
   for (i=0; i < len; i++) {\
      inoutvec[i] = MACRO_##FUNC(invec[i],inoutvec[i]);\
   }\
}

#define COMPLEX_SUM_FUNC(NAME, TYPE) \
static void EMPI_SUM_##NAME(TYPE *b, TYPE *a, int len) \
{\
   int i;\
   for ( i=0; i < len; i++ ) {\
      a[i].re = MACRO_SUM(a[i].re,b[i].re);\
      a[i].im = MACRO_SUM(a[i].im,b[i].im);\
   }\
}

#define COMPLEX_PROD_FUNC(NAME, TYPE) \
static void EMPI_PROD_##NAME(TYPE *b, TYPE *a, int len) \
{\
   int i;\
   TYPE c;\
   for ( i=0; i < len; i++ ){\
      c.re = a[i].re;\
      c.im = a[i].im;\
      a[i].re = c.re*b[i].re - c.im*b[i].im;\
      a[i].im = c.im*b[i].re + c.re*b[i].im;\
   }\
}

#define MAXLOC_FUNC(NAME, TYPE) \
static void EMPI_MAXLOC_##NAME(TYPE *invec, TYPE *inoutvec, int len) \
{\
   int i;\
   for (i=0; i < len; i++){\
      if ( inoutvec[i].value == invec[i].value ) {\
         inoutvec[i].loc = MACRO_MIN(inoutvec[i].loc,invec[i].loc);\
      } else if ( inoutvec[i].value < invec[i].value ) {\
         inoutvec[i].value = invec[i].value;\
         inoutvec[i].loc   = invec[i].loc;\
      }\
   }\
}

#define MINLOC_FUNC(NAME, TYPE) \
static void EMPI_MINLOC_##NAME(TYPE *invec, TYPE *inoutvec, int len) \
{\
   int i;\
   for (i=0; i < len; i++){\
      if ( inoutvec[i].value == invec[i].value ) {\
         inoutvec[i].loc = MACRO_MIN(inoutvec[i].loc,invec[i].loc);\
      } else if ( inoutvec[i].value > invec[i].value ) {\
         inoutvec[i].value = invec[i].value;\
         inoutvec[i].loc   = invec[i].loc;\
      }\
   }\
}

#define CALL_FUNC(FUNC, NAME, TYPE) EMPI_##FUNC##_##NAME((TYPE *)invec, (TYPE *)inoutvec, *len);
#define CALL_COMPLEX_SUM_FUNC(NAME, TYPE) EMPI_SUM_##NAME((TYPE *)invec, (TYPE *)inoutvec, *len);
#define CALL_COMPLEX_PROD_FUNC(NAME, TYPE) EMPI_PROD_##NAME((TYPE *)invec, (TYPE *)inoutvec, *len);
#define CALL_LOC_FUNC(FUNC, NAME, TYPE) EMPI_##FUNC##_##NAME((TYPE *)invec, (TYPE *)inoutvec, *len);

REDUCE_FUNC(MAX, CHAR, char)
REDUCE_FUNC(MIN, CHAR, char)
REDUCE_FUNC(SUM, CHAR, char)
REDUCE_FUNC(PROD, CHAR, char)
REDUCE_FUNC(BAND, CHAR, char)
REDUCE_FUNC(BOR, CHAR, char)
REDUCE_FUNC(BXOR, CHAR, char)
REDUCE_FUNC(LAND, CHAR, char)
REDUCE_FUNC(LOR, CHAR, char)
REDUCE_FUNC(LXOR, CHAR, char)

REDUCE_FUNC(MAX, SHORT, short)
REDUCE_FUNC(MIN, SHORT, short)
REDUCE_FUNC(SUM, SHORT, short)
REDUCE_FUNC(PROD, SHORT, short)
REDUCE_FUNC(BAND, SHORT, short)
REDUCE_FUNC(BOR, SHORT, short)
REDUCE_FUNC(BXOR, SHORT, short)
REDUCE_FUNC(LAND, SHORT, short)
REDUCE_FUNC(LOR, SHORT, short)
REDUCE_FUNC(LXOR, SHORT, short)

REDUCE_FUNC(MAX, INT, int)
REDUCE_FUNC(MIN, INT, int)
REDUCE_FUNC(SUM, INT, int)
REDUCE_FUNC(PROD, INT, int)
REDUCE_FUNC(BAND, INT, int)
REDUCE_FUNC(BOR, INT, int)
REDUCE_FUNC(BXOR, INT, int)
REDUCE_FUNC(LAND, INT, int)
REDUCE_FUNC(LOR, INT, int)
REDUCE_FUNC(LXOR, INT, int)

REDUCE_FUNC(MAX, LONG, long)
REDUCE_FUNC(MIN, LONG, long)
REDUCE_FUNC(SUM, LONG, long)
REDUCE_FUNC(PROD, LONG, long)
REDUCE_FUNC(BAND, LONG, long)
REDUCE_FUNC(BOR, LONG, long)
REDUCE_FUNC(BXOR, LONG, long)
REDUCE_FUNC(LAND, LONG, long)
REDUCE_FUNC(LOR, LONG, long)
REDUCE_FUNC(LXOR, LONG, long)

REDUCE_FUNC(MAX, LONG_LONG, long long)
REDUCE_FUNC(MIN, LONG_LONG, long long)
REDUCE_FUNC(SUM, LONG_LONG, long long)
REDUCE_FUNC(PROD, LONG_LONG, long long)
REDUCE_FUNC(BAND, LONG_LONG, long long)
REDUCE_FUNC(BOR, LONG_LONG, long long)
REDUCE_FUNC(BXOR, LONG_LONG, long long)
REDUCE_FUNC(LAND, LONG_LONG, long long)
REDUCE_FUNC(LOR, LONG_LONG, long long)
REDUCE_FUNC(LXOR, LONG_LONG, long long)

REDUCE_FUNC(MAX, SIGNED_CHAR, signed char)
REDUCE_FUNC(MIN, SIGNED_CHAR, signed char)
REDUCE_FUNC(SUM, SIGNED_CHAR, signed char)
REDUCE_FUNC(PROD, SIGNED_CHAR, signed char)
REDUCE_FUNC(BAND, SIGNED_CHAR, signed char)
REDUCE_FUNC(BOR, SIGNED_CHAR, signed char)
REDUCE_FUNC(BXOR, SIGNED_CHAR, signed char)
REDUCE_FUNC(LAND, SIGNED_CHAR, signed char)
REDUCE_FUNC(LOR, SIGNED_CHAR, signed char)
REDUCE_FUNC(LXOR, SIGNED_CHAR, signed char)

REDUCE_FUNC(MAX, UNSIGNED_CHAR, unsigned char)
REDUCE_FUNC(MIN, UNSIGNED_CHAR, unsigned char)
REDUCE_FUNC(SUM, UNSIGNED_CHAR, unsigned char)
REDUCE_FUNC(PROD, UNSIGNED_CHAR, unsigned char)
REDUCE_FUNC(BAND, UNSIGNED_CHAR, unsigned char)
REDUCE_FUNC(BOR, UNSIGNED_CHAR, unsigned char)
REDUCE_FUNC(BXOR, UNSIGNED_CHAR, unsigned char)
REDUCE_FUNC(LAND, UNSIGNED_CHAR, unsigned char)
REDUCE_FUNC(LOR, UNSIGNED_CHAR, unsigned char)
REDUCE_FUNC(LXOR, UNSIGNED_CHAR, unsigned char)

REDUCE_FUNC(MAX, UNSIGNED_SHORT, unsigned short)
REDUCE_FUNC(MIN, UNSIGNED_SHORT, unsigned short)
REDUCE_FUNC(SUM, UNSIGNED_SHORT, unsigned short)
REDUCE_FUNC(PROD, UNSIGNED_SHORT, unsigned short)
REDUCE_FUNC(BAND, UNSIGNED_SHORT, unsigned short)
REDUCE_FUNC(BOR, UNSIGNED_SHORT, unsigned short)
REDUCE_FUNC(BXOR, UNSIGNED_SHORT, unsigned short)
REDUCE_FUNC(LAND, UNSIGNED_SHORT, unsigned short)
REDUCE_FUNC(LOR, UNSIGNED_SHORT, unsigned short)
REDUCE_FUNC(LXOR, UNSIGNED_SHORT, unsigned short)

REDUCE_FUNC(MAX, UNSIGNED, unsigned)
REDUCE_FUNC(MIN, UNSIGNED, unsigned)
REDUCE_FUNC(SUM, UNSIGNED, unsigned)
REDUCE_FUNC(PROD, UNSIGNED, unsigned)
REDUCE_FUNC(BAND, UNSIGNED, unsigned)
REDUCE_FUNC(BOR, UNSIGNED, unsigned)
REDUCE_FUNC(BXOR, UNSIGNED, unsigned)
REDUCE_FUNC(LAND, UNSIGNED, unsigned)
REDUCE_FUNC(LOR, UNSIGNED, unsigned)
REDUCE_FUNC(LXOR, UNSIGNED, unsigned)

REDUCE_FUNC(MAX, UNSIGNED_LONG, unsigned long)
REDUCE_FUNC(MIN, UNSIGNED_LONG, unsigned long)
REDUCE_FUNC(SUM, UNSIGNED_LONG, unsigned long)
REDUCE_FUNC(PROD, UNSIGNED_LONG, unsigned long)
REDUCE_FUNC(BAND, UNSIGNED_LONG, unsigned long)
REDUCE_FUNC(BOR, UNSIGNED_LONG, unsigned long)
REDUCE_FUNC(BXOR, UNSIGNED_LONG, unsigned long)
REDUCE_FUNC(LAND, UNSIGNED_LONG, unsigned long)
REDUCE_FUNC(LOR, UNSIGNED_LONG, unsigned long)
REDUCE_FUNC(LXOR, UNSIGNED_LONG, unsigned long)

REDUCE_FUNC(MAX, UNSIGNED_LONG_LONG, unsigned long long)
REDUCE_FUNC(MIN, UNSIGNED_LONG_LONG, unsigned long long)
REDUCE_FUNC(SUM, UNSIGNED_LONG_LONG, unsigned long long)
REDUCE_FUNC(PROD, UNSIGNED_LONG_LONG, unsigned long long)
REDUCE_FUNC(BAND, UNSIGNED_LONG_LONG, unsigned long long)
REDUCE_FUNC(BOR, UNSIGNED_LONG_LONG, unsigned long long)
REDUCE_FUNC(BXOR, UNSIGNED_LONG_LONG, unsigned long long)
REDUCE_FUNC(LAND, UNSIGNED_LONG_LONG, unsigned long long)
REDUCE_FUNC(LOR, UNSIGNED_LONG_LONG, unsigned long long)
REDUCE_FUNC(LXOR, UNSIGNED_LONG_LONG, unsigned long long)

REDUCE_FUNC(MAX, FLOAT, float)
REDUCE_FUNC(MIN, FLOAT, float)
REDUCE_FUNC(SUM, FLOAT, float)
REDUCE_FUNC(PROD, FLOAT, float)

REDUCE_FUNC(MAX, DOUBLE, double)
REDUCE_FUNC(MIN, DOUBLE, double)
REDUCE_FUNC(SUM, DOUBLE, double)
REDUCE_FUNC(PROD, DOUBLE, double)

REDUCE_FUNC(MAX, LONG_DOUBLE, long double)
REDUCE_FUNC(MIN, LONG_DOUBLE, long double)
REDUCE_FUNC(SUM, LONG_DOUBLE, long double)
REDUCE_FUNC(PROD, LONG_DOUBLE, long double)

COMPLEX_SUM_FUNC(COMPLEX, empi_complex_type)
COMPLEX_SUM_FUNC(DOUBLE_COMPLEX, empi_doublecomplex_type)

COMPLEX_PROD_FUNC(COMPLEX, empi_complex_type)
COMPLEX_PROD_FUNC(DOUBLE_COMPLEX, empi_doublecomplex_type)

MAXLOC_FUNC(2REAL, empi_2real_type)
MAXLOC_FUNC(2DOUBLE_PRECISION, empi_2doubleprecision_type)
MAXLOC_FUNC(2INT, empi_2int_type)
MAXLOC_FUNC(FLOAT_INT, empi_floatint_type)
MAXLOC_FUNC(DOUBLE_INT, empi_doubleint_type)
MAXLOC_FUNC(SHORT_INT, empi_shortint_type)
MAXLOC_FUNC(LONG_INT, empi_longint_type)
MAXLOC_FUNC(LONG_DOUBLE_INT, empi_longdoubleint_type)

MINLOC_FUNC(2REAL, empi_2real_type)
MINLOC_FUNC(2DOUBLE_PRECISION, empi_2doubleprecision_type)
MINLOC_FUNC(2INT, empi_2int_type)
MINLOC_FUNC(FLOAT_INT, empi_floatint_type)
MINLOC_FUNC(DOUBLE_INT, empi_doubleint_type)
MINLOC_FUNC(SHORT_INT, empi_shortint_type)
MINLOC_FUNC(LONG_INT, empi_longint_type)
MINLOC_FUNC(LONG_DOUBLE_INT, empi_longdoubleint_type)

void EMPI_REDUCE_MAX(void *invec, void *inoutvec, int *len, EMPI_Datatype *type) {

   EMPI_Op_errno = EMPI_SUCCESS;

   switch (*type) {
   case EMPI_CHAR:
      CALL_FUNC(MAX, CHAR, char)
      break;
   case EMPI_SHORT:
      CALL_FUNC(MAX, SHORT, short)
      break;
   case EMPI_INT:
   case EMPI_INTEGER:
      CALL_FUNC(MAX, INT, int)
      break;
   case EMPI_LONG:
      CALL_FUNC(MAX, LONG, long)
      break;
   case EMPI_LONG_LONG_INT:
      CALL_FUNC(MAX, LONG_LONG, long long)
      break;
   case EMPI_SIGNED_CHAR:
      CALL_FUNC(MAX, SIGNED_CHAR, signed char)
      break;
   case EMPI_UNSIGNED_CHAR:
      CALL_FUNC(MAX, UNSIGNED_CHAR, unsigned char)
      break;
   case EMPI_UNSIGNED_SHORT:
   case EMPI_WCHAR:
      CALL_FUNC(MAX, UNSIGNED_SHORT, unsigned short)
      break;
   case EMPI_UNSIGNED:
      CALL_FUNC(MAX, UNSIGNED, unsigned)
      break;
   case EMPI_UNSIGNED_LONG:
      CALL_FUNC(MAX, UNSIGNED_LONG, unsigned long)
      break;
   case EMPI_UNSIGNED_LONG_LONG:
      CALL_FUNC(MAX, UNSIGNED_LONG_LONG, unsigned long long)
      break;
   case EMPI_FLOAT:
   case EMPI_REAL:
      CALL_FUNC(MAX, FLOAT, float)
      break;
   case EMPI_DOUBLE:
   case EMPI_DOUBLE_PRECISION:
      CALL_FUNC(MAX, DOUBLE, double)
      break;
   case EMPI_LONG_DOUBLE:
      CALL_FUNC(MAX, LONG_DOUBLE, long double)
      break;
   default:
      FATAL("REDUCTION OPERATION FAILED TO FIND CORRECT TYPE!\n");
      EMPI_Op_errno = EMPI_ERR_OP;
   }
}


void EMPI_REDUCE_MIN(void *invec, void *inoutvec, int *len, EMPI_Datatype *type) {

   EMPI_Op_errno = EMPI_SUCCESS;

   switch (*type) {
   case EMPI_CHAR:
      CALL_FUNC(MIN, CHAR, char)
      break;
   case EMPI_SHORT:
      CALL_FUNC(MIN, SHORT, short)
      break;
   case EMPI_INT:
   case EMPI_INTEGER:
      CALL_FUNC(MIN, INT, int)
      break;
   case EMPI_LONG:
      CALL_FUNC(MIN, LONG, long)
      break;
   case EMPI_LONG_LONG_INT:
      CALL_FUNC(MIN, LONG_LONG, long long)
      break;
   case EMPI_SIGNED_CHAR:
      CALL_FUNC(MIN, SIGNED_CHAR, signed char)
      break;
   case EMPI_UNSIGNED_CHAR:
      CALL_FUNC(MIN, UNSIGNED_CHAR, unsigned char)
      break;
   case EMPI_UNSIGNED_SHORT:
   case EMPI_WCHAR:
      CALL_FUNC(MIN, UNSIGNED_SHORT, unsigned short)
      break;
   case EMPI_UNSIGNED:
      CALL_FUNC(MIN, UNSIGNED, unsigned)
      break;
   case EMPI_UNSIGNED_LONG:
      CALL_FUNC(MIN, UNSIGNED_LONG, unsigned long)
      break;
   case EMPI_UNSIGNED_LONG_LONG:
      CALL_FUNC(MIN, UNSIGNED_LONG_LONG, unsigned long long)
      break;
   case EMPI_FLOAT:
   case EMPI_REAL:
      CALL_FUNC(MIN, FLOAT, float)
      break;
   case EMPI_DOUBLE:
   case EMPI_DOUBLE_PRECISION:
      CALL_FUNC(MIN, DOUBLE, double)
      break;
   case EMPI_LONG_DOUBLE:
      CALL_FUNC(MIN, LONG_DOUBLE, long double)
      break;
   default:
      FATAL("REDUCTION OPERATION FAILED TO FIND CORRECT TYPE!\n");
      EMPI_Op_errno = EMPI_ERR_OP;
   }
}


void EMPI_REDUCE_SUM(void *invec, void *inoutvec, int *len, EMPI_Datatype *type) {

   EMPI_Op_errno = EMPI_SUCCESS;

   switch (*type) {
   case EMPI_CHAR:
      CALL_FUNC(SUM, CHAR, char)
      break;
   case EMPI_SHORT:
      CALL_FUNC(SUM, SHORT, short)
      break;
   case EMPI_INT:
   case EMPI_INTEGER:
      CALL_FUNC(SUM, INT, int)
      break;
   case EMPI_LONG:
      CALL_FUNC(SUM, LONG, long)
      break;
   case EMPI_LONG_LONG_INT:
      CALL_FUNC(SUM, LONG_LONG, long long)
      break;
   case EMPI_SIGNED_CHAR:
      CALL_FUNC(SUM, SIGNED_CHAR, signed char)
      break;
   case EMPI_UNSIGNED_CHAR:
      CALL_FUNC(SUM, UNSIGNED_CHAR, unsigned char)
      break;
   case EMPI_UNSIGNED_SHORT:
   case EMPI_WCHAR:
      CALL_FUNC(SUM, UNSIGNED_SHORT, unsigned short)
      break;
   case EMPI_UNSIGNED:
      CALL_FUNC(SUM, UNSIGNED, unsigned)
      break;
   case EMPI_UNSIGNED_LONG:
      CALL_FUNC(SUM, UNSIGNED_LONG, unsigned long)
      break;
   case EMPI_UNSIGNED_LONG_LONG:
      CALL_FUNC(SUM, UNSIGNED_LONG_LONG, unsigned long long)
      break;
   case EMPI_FLOAT:
   case EMPI_REAL:
      CALL_FUNC(SUM, FLOAT, float)
      break;
   case EMPI_DOUBLE:
   case EMPI_DOUBLE_PRECISION:
      CALL_FUNC(SUM, DOUBLE, double)
      break;
   case EMPI_LONG_DOUBLE:
      CALL_FUNC(SUM, LONG_DOUBLE, long double)
      break;
   case EMPI_COMPLEX:
      CALL_COMPLEX_SUM_FUNC(COMPLEX, empi_complex_type);
      break;
   case EMPI_DOUBLE_COMPLEX:
      CALL_COMPLEX_SUM_FUNC(DOUBLE_COMPLEX, empi_doublecomplex_type);
      break;
   default:
      FATAL("REDUCTION OPERATION PROD FAILED TO FIND CORRECT TYPE!\n");
      EMPI_Op_errno = EMPI_ERR_OP;
   }
}


void EMPI_REDUCE_PROD(void *invec, void *inoutvec, int *len, EMPI_Datatype *type) {

   EMPI_Op_errno = EMPI_SUCCESS;

   switch (*type) {
   case EMPI_CHAR:
      CALL_FUNC(PROD, CHAR, char)
      break;
   case EMPI_SHORT:
      CALL_FUNC(PROD, SHORT, short)
      break;
   case EMPI_INT:
   case EMPI_INTEGER:
      CALL_FUNC(PROD, INT, int)
      break;
   case EMPI_LONG:
      CALL_FUNC(PROD, LONG, long)
      break;
   case EMPI_LONG_LONG_INT:
      CALL_FUNC(PROD, LONG_LONG, long long)
      break;
   case EMPI_SIGNED_CHAR:
      CALL_FUNC(PROD, SIGNED_CHAR, signed char)
      break;
   case EMPI_UNSIGNED_CHAR:
      CALL_FUNC(PROD, UNSIGNED_CHAR, unsigned char)
      break;
   case EMPI_UNSIGNED_SHORT:
   case EMPI_WCHAR:
      CALL_FUNC(PROD, UNSIGNED_SHORT, unsigned short)
      break;
   case EMPI_UNSIGNED:
      CALL_FUNC(PROD, UNSIGNED, unsigned)
      break;
   case EMPI_UNSIGNED_LONG:
      CALL_FUNC(PROD, UNSIGNED_LONG, unsigned long)
      break;
   case EMPI_UNSIGNED_LONG_LONG:
      CALL_FUNC(PROD, UNSIGNED_LONG_LONG, unsigned long long)
      break;
   case EMPI_FLOAT:
   case EMPI_REAL:
      CALL_FUNC(PROD, FLOAT, float)
      break;
   case EMPI_DOUBLE:
   case EMPI_DOUBLE_PRECISION:
      CALL_FUNC(PROD, DOUBLE, double)
      break;
   case EMPI_LONG_DOUBLE:
      CALL_FUNC(PROD, LONG_DOUBLE, long double)
      break;
   case EMPI_COMPLEX:
      CALL_COMPLEX_PROD_FUNC(COMPLEX, empi_complex_type);
      break;
   case EMPI_DOUBLE_COMPLEX:
      CALL_COMPLEX_PROD_FUNC(DOUBLE_COMPLEX, empi_doublecomplex_type);
      break;
   default:
      FATAL("REDUCTION OPERATION PROD FAILED TO FIND CORRECT TYPE!\n");
      EMPI_Op_errno = EMPI_ERR_OP;
   }
}


void EMPI_REDUCE_LAND(void *invec, void *inoutvec, int *len, EMPI_Datatype *type) {

   EMPI_Op_errno = EMPI_SUCCESS;

   switch (*type) {
   case EMPI_CHAR:
      CALL_FUNC(LAND, CHAR, char)
      break;
   case EMPI_SHORT:
      CALL_FUNC(LAND, SHORT, short)
      break;
   case EMPI_INT:
   case EMPI_LOGICAL:
      CALL_FUNC(LAND, INT, int)
      break;
   case EMPI_LONG:
      CALL_FUNC(LAND, LONG, long)
      break;
   case EMPI_LONG_LONG_INT:
      CALL_FUNC(LAND, LONG_LONG, long long)
      break;
   case EMPI_SIGNED_CHAR:
      CALL_FUNC(LAND, SIGNED_CHAR, signed char)
      break;
   case EMPI_UNSIGNED_CHAR:
      CALL_FUNC(LAND, UNSIGNED_CHAR, unsigned char)
      break;
   case EMPI_UNSIGNED_SHORT:
   case EMPI_WCHAR:
      CALL_FUNC(LAND, UNSIGNED_SHORT, unsigned short)
      break;
   case EMPI_UNSIGNED:
      CALL_FUNC(LAND, UNSIGNED, unsigned)
      break;
   case EMPI_UNSIGNED_LONG:
      CALL_FUNC(LAND, UNSIGNED_LONG, unsigned long)
      break;
   case EMPI_UNSIGNED_LONG_LONG:
      CALL_FUNC(LAND, UNSIGNED_LONG_LONG, unsigned long long)
      break;
   default:
      FATAL("REDUCTION OPERATION LAND FAILED TO FIND CORRECT TYPE!\n");
      EMPI_Op_errno = EMPI_ERR_OP;
   }
}


void EMPI_REDUCE_LOR(void *invec, void *inoutvec, int *len, EMPI_Datatype *type) {

   EMPI_Op_errno = EMPI_SUCCESS;

   switch (*type) {
   case EMPI_CHAR:
      CALL_FUNC(LOR, CHAR, char)
      break;
   case EMPI_SHORT:
      CALL_FUNC(LOR, SHORT, short)
      break;
   case EMPI_INT:
   case EMPI_LOGICAL:
      CALL_FUNC(LOR, INT, int)
      break;
   case EMPI_LONG:
      CALL_FUNC(LOR, LONG, long)
      break;
   case EMPI_LONG_LONG_INT:
      CALL_FUNC(LOR, LONG_LONG, long long)
      break;
   case EMPI_SIGNED_CHAR:
      CALL_FUNC(LOR, SIGNED_CHAR, signed char)
      break;
   case EMPI_UNSIGNED_CHAR:
      CALL_FUNC(LOR, UNSIGNED_CHAR, unsigned char)
      break;
   case EMPI_UNSIGNED_SHORT:
   case EMPI_WCHAR:
      CALL_FUNC(LOR, UNSIGNED_SHORT, unsigned short)
      break;
   case EMPI_UNSIGNED:
      CALL_FUNC(LOR, UNSIGNED, unsigned)
      break;
   case EMPI_UNSIGNED_LONG:
      CALL_FUNC(LOR, UNSIGNED_LONG, unsigned long)
      break;
   case EMPI_UNSIGNED_LONG_LONG:
      CALL_FUNC(LOR, UNSIGNED_LONG_LONG, unsigned long long)
      break;
   default:
      FATAL("REDUCTION OPERATION LOR FAILED TO FIND CORRECT TYPE!\n");
      EMPI_Op_errno = EMPI_ERR_OP;
   }
}


void EMPI_REDUCE_LXOR(void *invec, void *inoutvec, int *len, EMPI_Datatype *type) {
   switch (*type) {
   case EMPI_CHAR:
      CALL_FUNC(LXOR, CHAR, char)
      break;
   case EMPI_SHORT:
      CALL_FUNC(LXOR, SHORT, short)
      break;
   case EMPI_INT:
   case EMPI_LOGICAL:
      CALL_FUNC(LXOR, INT, int)
      break;
   case EMPI_LONG:
      CALL_FUNC(LXOR, LONG, long)
      break;
   case EMPI_LONG_LONG_INT:
      CALL_FUNC(LXOR, LONG_LONG, long long)
      break;
   case EMPI_SIGNED_CHAR:
      CALL_FUNC(LXOR, SIGNED_CHAR, signed char)
      break;
   case EMPI_UNSIGNED_CHAR:
      CALL_FUNC(LXOR, UNSIGNED_CHAR, unsigned char)
      break;
   case EMPI_UNSIGNED_SHORT:
   case EMPI_WCHAR:
      CALL_FUNC(LXOR, UNSIGNED_SHORT, unsigned short)
      break;
   case EMPI_UNSIGNED:
      CALL_FUNC(LXOR, UNSIGNED, unsigned)
      break;
   case EMPI_UNSIGNED_LONG:
      CALL_FUNC(LXOR, UNSIGNED_LONG, unsigned long)
      break;
   case EMPI_UNSIGNED_LONG_LONG:
      CALL_FUNC(LXOR, UNSIGNED_LONG_LONG, unsigned long long)
      break;
   default:
      FATAL("REDUCTION OPERATION LXOR FAILED TO FIND CORRECT TYPE!\n");
      EMPI_Op_errno = EMPI_ERR_OP;
   }
}



void EMPI_REDUCE_BAND(void *invec, void *inoutvec, int *len, EMPI_Datatype *type) {

   EMPI_Op_errno = EMPI_SUCCESS;

   switch (*type) {
   case EMPI_CHAR:
      CALL_FUNC(BAND, CHAR, char)
      break;
   case EMPI_SHORT:
      CALL_FUNC(BAND, SHORT, short)
      break;
   case EMPI_INT:
   case EMPI_INTEGER:
      CALL_FUNC(BAND, INT, int)
      break;
   case EMPI_LONG:
      CALL_FUNC(BAND, LONG, long)
      break;
   case EMPI_LONG_LONG_INT:
      CALL_FUNC(BAND, LONG_LONG, long long)
      break;
   case EMPI_SIGNED_CHAR:
      CALL_FUNC(BAND, SIGNED_CHAR, signed char)
      break;
   case EMPI_UNSIGNED_CHAR:
   case EMPI_BYTE:
      CALL_FUNC(BAND, UNSIGNED_CHAR, unsigned char)
      break;
   case EMPI_UNSIGNED_SHORT:
   case EMPI_WCHAR:
      CALL_FUNC(BAND, UNSIGNED_SHORT, unsigned short)
      break;
   case EMPI_UNSIGNED:
      CALL_FUNC(BAND, UNSIGNED, unsigned)
      break;
   case EMPI_UNSIGNED_LONG:
      CALL_FUNC(BAND, UNSIGNED_LONG, unsigned long)
      break;
   case EMPI_UNSIGNED_LONG_LONG:
      CALL_FUNC(BAND, UNSIGNED_LONG_LONG, unsigned long long)
      break;
   default:
      FATAL("REDUCTION OPERATION BAND FAILED TO FIND CORRECT TYPE!\n");
      EMPI_Op_errno = EMPI_ERR_OP;
   }
}


void EMPI_REDUCE_BOR(void *invec, void *inoutvec, int *len, EMPI_Datatype *type) {

   EMPI_Op_errno = EMPI_SUCCESS;

   switch (*type) {
   case EMPI_CHAR:
      CALL_FUNC(BOR, CHAR, char)
      break;
   case EMPI_SHORT:
      CALL_FUNC(BOR, SHORT, short)
      break;
   case EMPI_INT:
   case EMPI_INTEGER:
      CALL_FUNC(BOR, INT, int)
      break;
   case EMPI_LONG:
      CALL_FUNC(BOR, LONG, long)
      break;
   case EMPI_LONG_LONG_INT:
      CALL_FUNC(BOR, LONG_LONG, long long)
      break;
   case EMPI_SIGNED_CHAR:
      CALL_FUNC(BOR, SIGNED_CHAR, signed char)
      break;
   case EMPI_UNSIGNED_CHAR:
   case EMPI_BYTE:
      CALL_FUNC(BOR, UNSIGNED_CHAR, unsigned char)
      break;
   case EMPI_UNSIGNED_SHORT:
   case EMPI_WCHAR:
      CALL_FUNC(BOR, UNSIGNED_SHORT, unsigned short)
      break;
   case EMPI_UNSIGNED:
      CALL_FUNC(BOR, UNSIGNED, unsigned)
      break;
   case EMPI_UNSIGNED_LONG:
      CALL_FUNC(BOR, UNSIGNED_LONG, unsigned long)
      break;
   case EMPI_UNSIGNED_LONG_LONG:
      CALL_FUNC(BOR, UNSIGNED_LONG_LONG, unsigned long long)
      break;
   default:
      FATAL("REDUCTION OPERATION BOR FAILED TO FIND CORRECT TYPE!\n");
      EMPI_Op_errno = EMPI_ERR_OP;
   }
}


void EMPI_REDUCE_BXOR(void *invec, void *inoutvec, int *len, EMPI_Datatype *type) {

   EMPI_Op_errno = EMPI_SUCCESS;

   switch (*type) {
   case EMPI_CHAR:
      CALL_FUNC(BXOR, CHAR, char)
      break;
   case EMPI_SHORT:
      CALL_FUNC(BXOR, SHORT, short)
      break;
   case EMPI_INT:
   case EMPI_INTEGER:
      CALL_FUNC(BXOR, INT, int)
      break;
   case EMPI_LONG:
      CALL_FUNC(BXOR, LONG, long)
      break;
   case EMPI_LONG_LONG_INT:
      CALL_FUNC(BXOR, LONG_LONG, long long)
      break;
   case EMPI_SIGNED_CHAR:
      CALL_FUNC(BXOR, SIGNED_CHAR, signed char)
      break;
   case EMPI_UNSIGNED_CHAR:
   case EMPI_BYTE:
      CALL_FUNC(BXOR, UNSIGNED_CHAR, unsigned char)
      break;
   case EMPI_UNSIGNED_SHORT:
   case EMPI_WCHAR:
      CALL_FUNC(BXOR, UNSIGNED_SHORT, unsigned short)
      break;
   case EMPI_UNSIGNED:
      CALL_FUNC(BXOR, UNSIGNED, unsigned)
      break;
   case EMPI_UNSIGNED_LONG:
      CALL_FUNC(BXOR, UNSIGNED_LONG, unsigned long)
      break;
   case EMPI_UNSIGNED_LONG_LONG:
      CALL_FUNC(BXOR, UNSIGNED_LONG_LONG, unsigned long long)
      break;
   default:
      FATAL("REDUCTION OPERATION BXOR FAILED TO FIND CORRECT TYPE!\n");
      EMPI_Op_errno = EMPI_ERR_OP;
   }
}


void EMPI_REDUCE_MAXLOC(void *invec, void *inoutvec, int *len, EMPI_Datatype *type) {

   EMPI_Op_errno = EMPI_SUCCESS;

   switch (*type) {
   case EMPI_2REAL:
      CALL_LOC_FUNC(MAXLOC, 2REAL, empi_2real_type)
      break;
   case EMPI_2DOUBLE_PRECISION:
      CALL_LOC_FUNC(MAXLOC, 2DOUBLE_PRECISION, empi_2doubleprecision_type)
      break;
   case EMPI_2INT:
   case EMPI_2INTEGER:
      CALL_LOC_FUNC(MAXLOC, 2INT, empi_2int_type)
      break;
   case EMPI_FLOAT_INT:
      CALL_LOC_FUNC(MAXLOC, FLOAT_INT, empi_floatint_type)
      break;
   case EMPI_DOUBLE_INT:
      CALL_LOC_FUNC(MAXLOC, DOUBLE_INT, empi_doubleint_type)
      break;
   case EMPI_LONG_INT:
      CALL_LOC_FUNC(MAXLOC, LONG_INT, empi_longint_type)
      break;
   case EMPI_SHORT_INT:
      CALL_LOC_FUNC(MAXLOC, SHORT_INT, empi_shortint_type)
      break;
   case EMPI_LONG_DOUBLE_INT:
      CALL_LOC_FUNC(MAXLOC, LONG_DOUBLE_INT, empi_longdoubleint_type)
      break;
   default:
      FATAL("REDUCTION OPERATION MAXLOC FAILED TO FIND CORRECT TYPE!\n");
      EMPI_Op_errno = EMPI_ERR_OP;
   }
}



void EMPI_REDUCE_MINLOC(void *invec, void *inoutvec, int *len, EMPI_Datatype *type) {

   EMPI_Op_errno = EMPI_SUCCESS;

   switch (*type) {
   case EMPI_2REAL:
      CALL_LOC_FUNC(MINLOC, 2REAL, empi_2real_type)
      break;
   case EMPI_2DOUBLE_PRECISION:
      CALL_LOC_FUNC(MINLOC, 2DOUBLE_PRECISION, empi_2doubleprecision_type)
      break;
   case EMPI_2INT:
   case EMPI_2INTEGER:
      CALL_LOC_FUNC(MINLOC, 2INT, empi_2int_type)
      break;
   case EMPI_FLOAT_INT:
      CALL_LOC_FUNC(MINLOC, FLOAT_INT, empi_floatint_type)
      break;
   case EMPI_DOUBLE_INT:
      CALL_LOC_FUNC(MINLOC, DOUBLE_INT, empi_doubleint_type)
      break;
   case EMPI_LONG_INT:
      CALL_LOC_FUNC(MINLOC, LONG_INT, empi_longint_type)
      break;
   case EMPI_SHORT_INT:
      CALL_LOC_FUNC(MINLOC, SHORT_INT, empi_shortint_type)
      break;
   case EMPI_LONG_DOUBLE_INT:
      CALL_LOC_FUNC(MINLOC, LONG_DOUBLE_INT, empi_longdoubleint_type)
      break;
   default:
      FATAL("REDUCTION OPERATION MINLOC FAILED TO FIND CORRECT TYPE!\n");
      EMPI_Op_errno = EMPI_ERR_OP;
   }
}














#if 0

void MAGPIE_MAX(void *invec, void *inoutvec, int *len, EMPI_Datatype *type) {

   switch (*type) {
   case EMPI_CHAR:
      CALL_FUNC(MAX, CHAR, char)
      break;
   case EMPI_SHORT:
      CALL_FUNC(MAX, SHORT, short)
      break;
   case EMPI_INT:
   case EMPI_INTEGER:
      CALL_FUNC(MAX, INT, int)
      break;
   case EMPI_LONG:
      CALL_FUNC(MAX, LONG, long)
      break;
   case EMPI_LONG_LONG_INT:
      CALL_FUNC(MAX, LONG_LONG, long long)
      break;
   case EMPI_SIGNED_CHAR:
      CALL_FUNC(MAX, SIGNED_CHAR, signed char)
      break;
   case EMPI_UNSIGNED_CHAR:
   case EMPI_BYTE:
      CALL_FUNC(MAX, UNSIGNED_CHAR, unsigned char)
      break;
   case EMPI_UNSIGNED_SHORT:
   case EMPI_WCHAR:
      CALL_FUNC(MAX, UNSIGNED_SHORT, unsigned short)
      break;
   case EMPI_UNSIGNED:
      CALL_FUNC(MAX, UNSIGNED, unsigned)
      break;
   case EMPI_UNSIGNED_LONG:
      CALL_FUNC(MAX, UNSIGNED_LONG, unsigned long)
      break;
   case EMPI_FLOAT:
   case EMPI_REAL:
      CALL_FUNC(MAX, FLOAT, float)
      break;
   case EMPI_DOUBLE:
   case EMPI_DOUBLE_PRECISION:
      CALL_FUNC(MAX, DOUBLE, double)
      break;
   case EMPI_LONG_DOUBLE:
      CALL_FUNC(MAX, LONG_DOUBLE, long double)
      break;
   default:
      FATAL("REDUCTION OPERATION FAILED TO FIND CORRECT TYPE!\n");
      MAGPIE_Op_errno = EMPI_ERR_OP;
   }
}





void MAGPIE_MAX(void *invec, void *inoutvec, int *len, EMPI_Datatype *type) {
  int i;

INFO(2, "MAGPIE_MAX invec=%d inoutvec=%d len=%d type=%s\n", ((int*)invec)[0], ((int*)inoutvec)[0], *len, *type);

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


void MAGPIE_MIN(void *invec, void *inoutvec, int *len, EMPI_Datatype *type){
  int i;

INFO(2, "MAGPIE_MIN invec=%d inoutvec=%d len=%d type=%s\n", ((int*)invec)[0], ((int*)inoutvec)[0], *len, *type);

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


void MAGPIE_SUM(void *invec, void *inoutvec, int *len, EMPI_Datatype *type){
  int i;

// BROKEN?

  INFO(2, "MAGPIE_SUM invec=%d inoutvec=%d len=%d type=%s", ((int*)invec)[0], ((int*)inoutvec)[0], *len, *type);

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


void MAGPIE_PROD(void *invec, void *inoutvec, int *len, EMPI_Datatype *type){
  int i;

INFO(2, "MAGPIE_PROD invec=%d inoutvec=%d len=%d type=%s\n", ((int*)invec)[0], ((int*)inoutvec)[0], *len, *type);

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


void MAGPIE_LAND(void *invec, void *inoutvec, int *len, EMPI_Datatype *type){
  int i;

INFO(2, "MAGPIE_LAND invec=%d inoutvec=%d len=%d type=%s\n", ((int*)invec)[0], ((int*)inoutvec)[0], *len, *type);

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


void MAGPIE_LOR(void *invec, void *inoutvec, int *len, EMPI_Datatype *type){
  int i;

INFO(2, "MAGPIE_LOR invec=%d inoutvec=%d len=%d type=%s\n", ((int*)invec)[0], ((int*)inoutvec)[0], *len, *type);

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


void MAGPIE_LXOR(void *invec, void *inoutvec, int *len, EMPI_Datatype *type){
  int i;

INFO(2, "MAGPIE_LXOR invec=%d inoutvec=%d len=%d type=%s\n", ((int*)invec)[0], ((int*)inoutvec)[0], *len, *type);

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


void MAGPIE_BAND(void *invec, void *inoutvec, int *len, EMPI_Datatype *type){
  int i;

INFO(2, "MAGPIE_BAND invec=%d inoutvec=%d len=%d type=%s\n", ((int*)invec)[0], ((int*)inoutvec)[0], *len, *type);

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


void MAGPIE_BOR(void *invec, void *inoutvec, int *len, EMPI_Datatype *type){
  int i;

INFO(2, "MAGPIE_BOR invec=%d inoutvec=%d len=%d type=%s\n", ((int*)invec)[0], ((int*)inoutvec)[0], *len, *type);

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


void MAGPIE_BXOR(void *invec, void *inoutvec, int *len, EMPI_Datatype *type){
  int i;

INFO(2, "MAGPIE_BXOR invec=%d inoutvec=%d len=%d type=%s\n", ((int*)invec)[0], ((int*)inoutvec)[0], *len, *type);

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


void MAGPIE_MAXLOC(void *invec, void *inoutvec, int *len, EMPI_Datatype *type){
  int i;

INFO(2, "MAGPIE_MAXLOC invec=%d inoutvec=%d len=%d type=%s\n", ((int*)invec)[0], ((int*)inoutvec)[0], *len, *type);

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

INFO(2, "MAGPIE_MINLOC invec=%d inoutvec=%d len=%d type=%s\n", ((int*)invec)[0], ((int*)inoutvec)[0], *len, *type);

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

#endif
