#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "flags.h"

#include "datatype.h"

#include "empi.h"
#include "mpi.h"

#include "debugging.h"
#include "logging.h"

#include "communicator.h"

static datatype *primitive_types[EMPI_DEFINED_DATATYPES];

typedef struct {
  float re;
  float im;
} f_complex;

typedef struct {
  double re;
  double im;
} d_complex;

typedef struct {
  float f;
  int i;
} float_int;

typedef struct {
  double d;
  int i;
} double_int;

typedef struct {
  long l;
  int i;
} long_int;

typedef struct {
  int i1;
  int i2;
} two_int;

typedef struct {
  short s;
  int i;
} short_int;

typedef struct {
  long double ld;
  int i;
} long_double_int;

typedef struct {
  float f;
  float i;
} two_float;

typedef struct {
  double f;
  double i;
} two_double;


static int add_datatype(EMPI_Datatype handle, int size, MPI_Datatype type)
{
   int msize, error;
   datatype *d;

   if (handle < 0 || handle >= EMPI_DEFINED_DATATYPES) {
      ERROR(1, "add_datatype(handle=%d, size=%d, ...) Ran out of datatype storage!", handle, size);
      return EMPI_ERR_INTERN;
   }

   if (primitive_types[handle] != NULL) {
      ERROR(1, "add_datatype(handle=%d, size=%d...) Handle already in use!", handle, size);
      return EMPI_ERR_INTERN;
   }

   error = MPI_Type_size(type, &msize);

   if (error != MPI_SUCCESS) {
      ERROR(1, "add_datatype(handle=%d, size=%d...) Failed to retrieve native MPI size of datatype!", handle, size);
      return EMPI_ERR_INTERN;
   }

   if (size != msize) {
      ERROR(1, "add_datatype(handle=%d, size=%d...) Mismatch between EMPI and native MPI size of datatype! %d != %d", handle, size, size, msize);
      return EMPI_ERR_INTERN;
   }

   d = malloc(sizeof(datatype));

   if (d == NULL) {
      ERROR(1, "add_datatype(handle=%d, size=%d...) Failed to allocate space for type!", handle, size);
      return EMPI_ERR_INTERN;
   }

   d->handle = handle;
   d->size = size;
   d->type = type;

   primitive_types[handle] = d;

   return EMPI_SUCCESS;
}

int init_datatypes()
{
   int i, error;

   for (i=0;i<EMPI_DEFINED_DATATYPES;i++) {
      primitive_types[i] = NULL;
   }

   error = add_datatype(EMPI_DATATYPE_NULL, 0, MPI_DATATYPE_NULL);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_DATATYPE_NULL!");
      return error;
   }

   error = add_datatype(EMPI_CHAR, sizeof(signed char), MPI_CHAR);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_CHAR!");
      return error;
   }

   error = add_datatype(EMPI_SHORT, sizeof(signed short int), MPI_SHORT);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_SHORT!");
      return error;
   }

   error = add_datatype(EMPI_INT, sizeof(signed int), MPI_INT);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_INT!");
      return error;
   }

   error = add_datatype(EMPI_LONG, sizeof(signed long), MPI_LONG);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_LONG!");
      return error;
   }

   error = add_datatype(EMPI_LONG_LONG_INT, sizeof(signed long long), MPI_LONG_LONG_INT);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_LONG_LONG_INT!");
      return error;
   }

   error = add_datatype(EMPI_SIGNED_CHAR, sizeof(signed char), MPI_SIGNED_CHAR);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_SIGNED_CHAR!");
      return error;
   }

   error = add_datatype(EMPI_UNSIGNED_CHAR, sizeof(unsigned char), MPI_UNSIGNED_CHAR);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_UNSIGNED_CHAR!");
      return error;
   }

   error = add_datatype(EMPI_UNSIGNED_SHORT, sizeof(unsigned short int), MPI_UNSIGNED_SHORT);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_UNSIGNED_SHORT!");
      return error;
   }

   error = add_datatype(EMPI_UNSIGNED, sizeof(unsigned int), MPI_UNSIGNED);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_UNSIGNED!");
      return error;
   }

   error = add_datatype(EMPI_UNSIGNED_LONG, sizeof(unsigned long), MPI_UNSIGNED_LONG);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_UNSIGNED_LONG!");
      return error;
   }

   error = add_datatype(EMPI_UNSIGNED_LONG_LONG, sizeof(unsigned long long), MPI_UNSIGNED_LONG_LONG);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_UNSIGNED_LONG_LONG!");
      return error;
   }

   error = add_datatype(EMPI_FLOAT, sizeof(float), MPI_FLOAT);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_FLOAT!");
      return error;
   }

   error = add_datatype(EMPI_DOUBLE, sizeof(double), MPI_DOUBLE);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_DOUBLE!");
      return error;
   }

   error = add_datatype(EMPI_LONG_DOUBLE, sizeof(long double), MPI_LONG_DOUBLE);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_LONG_DOUBLE!");
      return error;
   }

   error = add_datatype(EMPI_WCHAR, sizeof(wchar_t), MPI_WCHAR);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_WCHAR!");
      return error;
   }

#ifdef HAVE_MPI_2_2

   error = add_datatype(EMPI_C_BOOL, sizeof(_Bool), MPI_C_BOOL);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_C_BOOL!");
      return error;
   }

   error = add_datatype(EMPI_INT8_T, sizeof(int8_t), MPI_INT8_T);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_INT8_T!");
      return error;
   }

   error = add_datatype(EMPI_INT16_T, sizeof(int16_t), MPI_INT16_T);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_INT16_T!");
      return error;
   }

   error = add_datatype(EMPI_INT32_T, sizeof(int32_t), MPI_INT32_T);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_INT32_T!");
      return error;
   }

   error = add_datatype(EMPI_INT64_T, sizeof(int64_t), MPI_INT64_T);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_INT64_T!");
      return error;
   }

   error = add_datatype(EMPI_UINT8_T, sizeof(uint8_t), MPI_UINT8_T);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_UINT64_T!");
      return error;
   }

   error = add_datatype(EMPI_UINT16_T, sizeof(uint16_t), MPI_UINT16_T);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_UINT16_T!");
      return error;
   }

   error = add_datatype(EMPI_UINT32_T, sizeof(uint32_t), MPI_UINT32_T);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_UINT32_T!");
      return error;
   }

   error = add_datatype(EMPI_UINT64_T, sizeof(uint64_t), MPI_UINT64_T);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_UINT64_T!");
      return error;
   }

   error = add_datatype(EMPI_C_COMPLEX, sizeof(float _Complex), MPI_C_COMPLEX);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_C_COMPLEX!");
      return error;
   }

   error = add_datatype(EMPI_C_DOUBLE_COMPLEX, sizeof(double _Complex), MPI_C_DOUBLE_COMPLEX);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_C_DOUBLE_COMPLEX!");
      return error;
   }

   error = add_datatype(EMPI_C_LONG_DOUBLE_COMPLEX, sizeof(long double _Complex), MPI_C_LONG_DOUBLE_COMPLEX);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_C_LONG_DOUBLE_COMPLEX!");
      return error;
   }

#endif

   error = add_datatype(EMPI_BYTE, sizeof(unsigned char), MPI_BYTE);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_BYTE!");
      return error;
   }

   error = add_datatype(EMPI_PACKED, 0, MPI_PACKED);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_PACKED!");
      return error;
   }

   error = add_datatype(EMPI_FLOAT_INT, sizeof(float_int), MPI_FLOAT_INT);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_FLOAT_INT!");
      return error;
   }

   error = add_datatype(EMPI_DOUBLE_INT, sizeof(double_int), MPI_DOUBLE_INT);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_DOUBLE_INT!");
      return error;
   }

   error = add_datatype(EMPI_LONG_INT, sizeof(long_int), MPI_LONG_INT);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_LONG_INT!");
      return error;
   }

   error = add_datatype(EMPI_2INT, sizeof(two_int), MPI_2INT);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_2INT!");
      return error;
   }

   error = add_datatype(EMPI_SHORT_INT, sizeof(short_int), MPI_SHORT_INT);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_SHORT_INT!");
      return error;
   }

   error = add_datatype(EMPI_LONG_DOUBLE_INT, sizeof(long_double_int), MPI_LONG_DOUBLE_INT);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_LONG_DOUBLE_INT!");
      return error;
   }

   error = add_datatype(EMPI_2REAL, sizeof(two_float), MPI_2REAL);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_2REAL!");
      return error;
   }

   error = add_datatype(EMPI_2DOUBLE_PRECISION, sizeof(two_double), MPI_2DOUBLE_PRECISION);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_2DOUBLE_PRECISION!");
      return error;
   }

   return EMPI_SUCCESS;
}

int is_derived_datatype(EMPI_Datatype handle)
{
   return (handle >= EMPI_DEFINED_DATATYPES);
}

datatype *handle_to_datatype(EMPI_Datatype handle)
{
   if (handle < 0 || handle >= EMPI_DEFINED_DATATYPES) {
      ERROR(1, "handle_to_datatype(handle=%d) Handle out of bounds!", handle);
      return NULL;
   }

   if (primitive_types[handle] == NULL) {
      ERROR(1, "handle_to_datatype(handle=%d) Handle not in use!", handle);
      return NULL;
   }

   return primitive_types[handle];
}

EMPI_Datatype datatype_to_handle(datatype *d)
{
   if (d == NULL) {
      ERROR(1, "datatype_to_handle(d=NULL) Datatype is NULL!");
      return EMPI_DATATYPE_NULL;
   }

   return d->handle;
}



/*
void catch_derived_datatype(EMPI_Datatype datatype)
{
   int i;

   for (i=0;i<PRIMITIVE_TYPES;i++) {
      if (datatype == primitive_types[i]) {
         return;
      }
   }

   FATAL("Attempt to communicate using a derived datatype!");
}
*/


