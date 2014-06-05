#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "settings.h"

#include "datatype.h"

#include "empi.h"
#include "mpi.h"

#include "debugging.h"
#include "logging.h"

#include "communicator.h"

static datatype *all_types[MAX_DATATYPES];

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


static int intern_add_datatype(EMPI_Datatype handle, MPI_Datatype type)
{
   datatype *d;

   if (handle < 0 || handle >= MAX_DATATYPES) {
      ERROR(1, "intern_add_datatype(handle=%d) Ran out of datatype storage!", handle);
      return EMPI_ERR_INTERN;
   }

   if (all_types[handle] != NULL) {
      ERROR(1, "intern_add_datatype(handle=%d) Handle already in use!", handle);
      return EMPI_ERR_INTERN;
   }

   d = malloc(sizeof(datatype));

   if (d == NULL) {
      ERROR(1, "intern_add_datatype(handle=%d) Failed to allocate space for type!", handle);
      return EMPI_ERR_INTERN;
   }

   d->handle = handle;
   d->type = type;
   all_types[handle] = d;

   return EMPI_SUCCESS;
}

int init_datatypes()
{
   int i, error;

   for (i=0;i<MAX_DATATYPES;i++) {
      all_types[i] = NULL;
   }

   error = intern_add_datatype(EMPI_DATATYPE_NULL, MPI_DATATYPE_NULL);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_DATATYPE_NULL!");
      return error;
   }

   error = intern_add_datatype(EMPI_CHAR, MPI_CHAR);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_CHAR!");
      return error;
   }

   error = intern_add_datatype(EMPI_SHORT, MPI_SHORT);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_SHORT!");
      return error;
   }

   error = intern_add_datatype(EMPI_INT, MPI_INT);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_INT!");
      return error;
   }

   error = intern_add_datatype(EMPI_LONG, MPI_LONG);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_LONG!");
      return error;
   }

   error = intern_add_datatype(EMPI_LONG_LONG_INT, MPI_LONG_LONG_INT);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_LONG_LONG_INT!");
      return error;
   }

   error = intern_add_datatype(EMPI_SIGNED_CHAR, MPI_SIGNED_CHAR);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_SIGNED_CHAR!");
      return error;
   }

   error = intern_add_datatype(EMPI_UNSIGNED_CHAR, MPI_UNSIGNED_CHAR);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_UNSIGNED_CHAR!");
      return error;
   }

   error = intern_add_datatype(EMPI_UNSIGNED_SHORT, MPI_UNSIGNED_SHORT);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_UNSIGNED_SHORT!");
      return error;
   }

   error = intern_add_datatype(EMPI_UNSIGNED, MPI_UNSIGNED);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_UNSIGNED!");
      return error;
   }

   error = intern_add_datatype(EMPI_UNSIGNED_LONG, MPI_UNSIGNED_LONG);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_UNSIGNED_LONG!");
      return error;
   }

   error = intern_add_datatype(EMPI_UNSIGNED_LONG_LONG, MPI_UNSIGNED_LONG_LONG);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_UNSIGNED_LONG_LONG!");
      return error;
   }

   error = intern_add_datatype(EMPI_FLOAT, MPI_FLOAT);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_FLOAT!");
      return error;
   }

   error = intern_add_datatype(EMPI_DOUBLE, MPI_DOUBLE);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_DOUBLE!");
      return error;
   }

   error = intern_add_datatype(EMPI_LONG_DOUBLE, MPI_LONG_DOUBLE);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_LONG_DOUBLE!");
      return error;
   }

   error = intern_add_datatype(EMPI_WCHAR, MPI_WCHAR);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_WCHAR!");
      return error;
   }

   error = intern_add_datatype(EMPI_BYTE, MPI_BYTE);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_BYTE!");
      return error;
   }

   error = intern_add_datatype(EMPI_PACKED, MPI_PACKED);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_PACKED!");
      return error;
   }

   error = intern_add_datatype(EMPI_FLOAT_INT, MPI_FLOAT_INT);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_FLOAT_INT!");
      return error;
   }

   error = intern_add_datatype(EMPI_DOUBLE_INT, MPI_DOUBLE_INT);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_DOUBLE_INT!");
      return error;
   }

   error = intern_add_datatype(EMPI_LONG_INT, MPI_LONG_INT);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_LONG_INT!");
      return error;
   }

   error = intern_add_datatype(EMPI_2INT, MPI_2INT);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_2INT!");
      return error;
   }

   error = intern_add_datatype(EMPI_SHORT_INT, MPI_SHORT_INT);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_SHORT_INT!");
      return error;
   }

   error = intern_add_datatype(EMPI_LONG_DOUBLE_INT, MPI_LONG_DOUBLE_INT);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_LONG_DOUBLE_INT!");
      return error;
   }

   error = intern_add_datatype(EMPI_INTEGER, MPI_INTEGER);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_INTEGER!");
      return error;
   }

   error = intern_add_datatype(EMPI_REAL, MPI_REAL);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_REAL!");
      return error;
   }

   error = intern_add_datatype(EMPI_DOUBLE_PRECISION, MPI_DOUBLE_PRECISION);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_DOUBLE_PRECISION!");
      return error;
   }

   error = intern_add_datatype(EMPI_COMPLEX, MPI_COMPLEX);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_COMPLEX!");
      return error;
   }

   error = intern_add_datatype(EMPI_LOGICAL, MPI_LOGICAL);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_LOGICAL!");
      return error;
   }

   error = intern_add_datatype(EMPI_CHARACTER, MPI_CHARACTER);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_CHARACTER!");
      return error;
   }

   error = intern_add_datatype(EMPI_DOUBLE_COMPLEX, MPI_DOUBLE_COMPLEX);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_DOUBLE_COMPLEX!");
      return error;
   }

   error = intern_add_datatype(EMPI_INTEGER1, MPI_INTEGER1);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_INTEGER1!");
      return error;
   }

   error = intern_add_datatype(EMPI_INTEGER2, MPI_INTEGER2);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_INTEGER2!");
      return error;
   }

   error = intern_add_datatype(EMPI_INTEGER4, MPI_INTEGER4);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_INTEGER4!");
      return error;
   }

   error = intern_add_datatype(EMPI_INTEGER8, MPI_INTEGER8);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_INTEGER8!");
      return error;
   }

/*
   error = intern_add_datatype(EMPI_REAL2, MPI_REAL2);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_REAL2!");
      return error;
   }
*/

   error = intern_add_datatype(EMPI_REAL4, MPI_REAL4);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_REAL4!");
      return error;
   }

   error = intern_add_datatype(EMPI_REAL8, MPI_REAL8);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_REAL8!");
      return error;
   }

   /* NOTE: Needed because MPI on DAS4 is ancient! */
   error = intern_add_datatype(EMPI_REAL16, MPI_LONG_DOUBLE);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_REAL16!");
      return error;
   }

   error = intern_add_datatype(EMPI_2REAL, MPI_2REAL);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_2REAL!");
      return error;
   }

   error = intern_add_datatype(EMPI_2DOUBLE_PRECISION, MPI_2DOUBLE_PRECISION);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_2DOUBLE_PRECISION!");
      return error;
   }

   error = intern_add_datatype(EMPI_2INTEGER, MPI_2INTEGER);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_2INTEGER!");
      return error;
   }

/*
#ifdef HAVE_MPI_2_2

   error = intern_add_datatype(EMPI_C_BOOL, sizeof(_Bool), MPI_C_BOOL);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_C_BOOL!");
      return error;
   }

   error = intern_add_datatype(EMPI_INT8_T, sizeof(int8_t), MPI_INT8_T);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_INT8_T!");
      return error;
   }

   error = intern_add_datatype(EMPI_INT16_T, sizeof(int16_t), MPI_INT16_T);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_INT16_T!");
      return error;
   }

   error = intern_add_datatype(EMPI_INT32_T, sizeof(int32_t), MPI_INT32_T);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_INT32_T!");
      return error;
   }

   error = intern_add_datatype(EMPI_INT64_T, sizeof(int64_t), MPI_INT64_T);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_INT64_T!");
      return error;
   }

   error = intern_add_datatype(EMPI_UINT8_T, sizeof(uint8_t), MPI_UINT8_T);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_UINT64_T!");
      return error;
   }

   error = intern_add_datatype(EMPI_UINT16_T, sizeof(uint16_t), MPI_UINT16_T);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_UINT16_T!");
      return error;
   }

   error = intern_add_datatype(EMPI_UINT32_T, sizeof(uint32_t), MPI_UINT32_T);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_UINT32_T!");
      return error;
   }

   error = intern_add_datatype(EMPI_UINT64_T, sizeof(uint64_t), MPI_UINT64_T);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_UINT64_T!");
      return error;
   }

   error = intern_add_datatype(EMPI_C_COMPLEX, sizeof(float _Complex), MPI_C_COMPLEX);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_C_COMPLEX!");
      return error;
   }

   error = intern_add_datatype(EMPI_C_DOUBLE_COMPLEX, sizeof(double _Complex), MPI_C_DOUBLE_COMPLEX);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_C_DOUBLE_COMPLEX!");
      return error;
   }

   error = intern_add_datatype(EMPI_C_LONG_DOUBLE_COMPLEX, sizeof(long double _Complex), MPI_C_LONG_DOUBLE_COMPLEX);

   if (error != EMPI_SUCCESS) {
      ERROR(1, "init_datatypes() Failed to init type EMPI_C_LONG_DOUBLE_COMPLEX!");
      return error;
   }

#endif
*/

   return EMPI_SUCCESS;
}

datatype *add_datatype(MPI_Datatype mtype)
{
   int i, error;

   for (i=EMPI_DEFINED_DATATYPES;i<MAX_DATATYPES;i++) {
      if (all_types[i] == NULL) {
         error = intern_add_datatype(i, mtype);

         if (error != EMPI_SUCCESS) {
            ERROR(1, "add_datatype(...) Failed to add datatype!");
            return NULL;
         }

         return all_types[i];
      }
   }

   ERROR(1, "add_datatype(...) No space left for new datatype!");
   return NULL;
}



int free_datatype(datatype *type)
{
   if (type == NULL) {
      ERROR(1, "free_datatype(type=NULL) Received NULL!");
      return EMPI_ERR_TYPE;
   }

   if (type->handle < 0 || type->handle > MAX_DATATYPES) {
      ERROR(1, "free_datatype(type=%d) Type out of bounds!", type->handle);
      return EMPI_ERR_TYPE;
   }

   if (type->handle < EMPI_DEFINED_DATATYPES) {
      ERROR(1, "free_datatype(type=%d) Cannot free basic datatype!", type->handle);
      return EMPI_ERR_TYPE;
   }

   if (type != all_types[type->handle]) {
      ERROR(1, "free_datatype(type=%d) Pointer mismatch in type %d!", type->handle);
      return EMPI_ERR_TYPE;
   }

   DEBUG(1, "Freeing datatype %d", type->handle);

   all_types[type->handle] = NULL;

   free(type);

   return EMPI_SUCCESS;
}

int is_derived_datatype(EMPI_Datatype handle)
{
   return (handle >= EMPI_DEFINED_DATATYPES);
}

datatype *handle_to_datatype(EMPI_Datatype handle)
{
   if (handle < 0 || handle > MAX_DATATYPES) {
      ERROR(1, "handle_to_datatype(handle=%d) Handle out of bounds!", handle);
      return NULL;
   }

   if (all_types[handle] == NULL) {
      ERROR(1, "handle_to_datatype(handle=%d) Handle not in use!", handle);
      return NULL;
   }

   return all_types[handle];
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
      if (datatype == all_types[i]) {
         return;
      }
   }

   FATAL("Attempt to communicate using a derived datatype!");
}
*/


