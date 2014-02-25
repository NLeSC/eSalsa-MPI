#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <execinfo.h>

#include "flags.h"
#include "logging.h"

// Indentation levels (single string with multiple pointers
// pointing into it at different locations).
static char *maxindent = "         ";
static char *indents[10];

// Circular string buffer used for debugging output. Note that if the
// (total) output of a single print operation is larger than DEBUG_BUF_SIZE
// the debug output will not be printed correctly.
#define DEBUG_BUF_SIZE 4096

static char debug_buf[DEBUG_BUF_SIZE+1];
static int debug_buf_pos = 0;

static char *type_names[MPI_DEFINED_DATATYPES];

/***************************************************************************/
/*                        Conversion functions                             */
/***************************************************************************/

static char *copy_to_debug_buf(const char *tmp, int len)
{
   char *res;

   if (len > DEBUG_BUF_SIZE) {
      // Cannot store this string!
      WARN(0, "Debug buffer overflow!");
      return &debug_buf[debug_buf_pos];
   }

   if (debug_buf_pos + len > DEBUG_BUF_SIZE) {
      debug_buf_pos = 0;
   }

   res = strcpy(&debug_buf[debug_buf_pos], tmp);
   debug_buf_pos += len;
   return res;
}

char *comm_to_string(MPI_Comm comm)
{
   int len = 0;
   char tmp[64];

   if (comm == MPI_COMM_WORLD) {
      len = sprintf(tmp, "<WORLD>");
      return copy_to_debug_buf(tmp, len+1);
   }

   if (comm == MPI_COMM_SELF) {
      len = sprintf(tmp, "<SELF>");
      return copy_to_debug_buf(tmp, len+1);
   }

   if (comm == MPI_COMM_NULL) {
      len = sprintf(tmp, "<NULL>");
      return copy_to_debug_buf(tmp, len+1);
   }

   len = sprintf(tmp, "<%d>", comm);
   return copy_to_debug_buf(tmp, len+1);
}

char *request_to_string(MPI_Request r)
{
   int len;
   char tmp[64];

   if (r == MPI_REQUEST_NULL) {
      len = sprintf(tmp, "<NULL>");
      return copy_to_debug_buf(tmp, len+1);
   }

   len = sprintf(tmp, "<%d>", r);
   return copy_to_debug_buf(tmp, len+1);
}

char *group_to_string(MPI_Group g)
{
   int len;
   char tmp[64];

   if (g == MPI_GROUP_EMPTY) {
      len = sprintf(tmp, "<EMPTY>");
      return copy_to_debug_buf(tmp, len+1);
   }

   if (g == MPI_GROUP_NULL) {
      len = sprintf(tmp, "<NULL>");
      return copy_to_debug_buf(tmp, len+1);
   }

   len = sprintf(tmp, "<%d>", g);
   return copy_to_debug_buf(tmp, len+1);
}

int get_type_name(MPI_Datatype type, char *tmp, int *len)
{
  if (type < 0 || type > MPI_DEFINED_DATATYPES) {
      return MPI_ERR_TYPE;
  }

  if (type_names[type] == NULL) {
      return MPI_ERR_TYPE;
  }

  strcpy(type_names[type], tmp);
  *len = (int) strlen(type_names[type]);
  return MPI_SUCCESS;
}

char *type_to_string(MPI_Datatype type)
{
   int len = 0;
   char tmp[1024];

   int error = get_type_name(type, tmp, &len);

   if (error != MPI_SUCCESS || len <= 0) {
      len = sprintf(tmp, "<UNKNOWN>");
   }

   return copy_to_debug_buf(tmp, len+1);
}

char *op_to_string(MPI_Op o)
{
   int len;
   char tmp[64];

   // FIXME

   tmp[0] = '\0';
   len = 0;

   return copy_to_debug_buf(tmp, len+1);
}

char *info_to_string(MPI_Info i)
{
   int len;
   char tmp[64];

   len = sprintf(tmp, "<%d>", i);
   return copy_to_debug_buf(tmp, len+1);
}

char *file_to_string(MPI_File f)
{
   int len;
   char tmp[64];

   len = sprintf(tmp, "<%d>", f);
   return copy_to_debug_buf(tmp, len+1);
}

char *win_to_string(MPI_Win w)
{
   int len;
   char tmp[64];

   len = sprintf(tmp, "<%d>", w);
   return copy_to_debug_buf(tmp, len+1);
}

char *ranks_to_string(int *ranks, int n)
{
   int i;
   int len = 0;
   char buf[1024];

   len += sprintf(buf, "[");

   for (i=0;i<n;i++) {
      len += sprintf(&buf[len], "%d", ranks[i]);

      if (i != n-1) {
         len += sprintf(&buf[len], ",");
      }
   }

   len += sprintf(&buf[len], "]");
   return copy_to_debug_buf(buf, len+1);
}

char *ranges_to_string(int range[][3], int n)
{
   int i;
   int len = 0;
   char buf[1024];

   len += sprintf(buf, "[");

   for (i=0;i<n;i++) {
      len += sprintf(&buf[len], "(%d,%d,%d)", range[i][0], range[i][1], range[i][2]);

      if (i != n-1) {
         len += sprintf(&buf[len], ",");
      }
   }

   len += sprintf(&buf[len], "]");
   return copy_to_debug_buf(buf, len+1);
}


void init_logging()
{
   int i;
   char *tmp = maxindent;

   for (i=9;i>=0;i--) {
      indents[i] = tmp++;
   }

   debug_buf[DEBUG_BUF_SIZE] = '\0';

   type_names[MPI_DATATYPE_NULL]      = MPI_DATATYPE_NULL_NAME;
   type_names[MPI_CHAR]               = MPI_CHAR_NAME;
   type_names[MPI_SHORT]              = MPI_SHORT_NAME;
   type_names[MPI_INT]                = MPI_INT_NAME;
   type_names[MPI_LONG]               = MPI_LONG_NAME;
   type_names[MPI_LONG_LONG_INT]      = MPI_LONG_LONG_INT_NAME;
//   type_names[MPI_LONG_LONG]             (MPI_LONG_LONG_INT)
   type_names[MPI_SIGNED_CHAR]        = MPI_SIGNED_CHAR_NAME;
   type_names[MPI_UNSIGNED_CHAR]      = MPI_UNSIGNED_CHAR_NAME;
   type_names[MPI_UNSIGNED_SHORT]     = MPI_UNSIGNED_SHORT_NAME;
   type_names[MPI_UNSIGNED]           = MPI_UNSIGNED_NAME;
   type_names[MPI_UNSIGNED_LONG]      = MPI_UNSIGNED_LONG_NAME;
   type_names[MPI_UNSIGNED_LONG_LONG] = MPI_UNSIGNED_LONG_LONG_NAME;
   type_names[MPI_FLOAT]              = MPI_FLOAT_NAME;
   type_names[MPI_DOUBLE]             = MPI_DOUBLE_NAME;
   type_names[MPI_LONG_DOUBLE]        = MPI_LONG_DOUBLE_NAME;
   type_names[MPI_WCHAR]              = MPI_WCHAR_NAME;

#ifdef HAVE_MPI_2_2

   type_names[MPI_C_BOOL]             = MPI_C_BOOL_NAME;
   type_names[MPI_INT8_T]             = MPI_INT8_T_NAME;
   type_names[MPI_INT16_T]            = MPI_INT16_T_NAME;
   type_names[MPI_INT32_T]            = MPI_INT32_T_NAME;
   type_names[MPI_INT64_T]            = MPI_INT64_T_NAME;
   type_names[MPI_UINT8_T]            = MPI_UINT8_T_NAME;
   type_names[MPI_UINT16_T]           = MPI_UINT16_T_NAME;
   type_names[MPI_UINT32_T]           = MPI_UINT32_T_NAME;
   type_names[MPI_UINT64_T]           = MPI_UINT64_T_NAME;
   type_names[MPI_C_COMPLEX]          = MPI_C_COMPLEX_NAME;
//   type_names[MPI_C_FLOAT_COMPLEX]       (MPI_C_COMPLEX)
   type_names[MPI_C_DOUBLE_COMPLEX]   = MPI_C_DOUBLE_COMPLEX_NAME;
   type_names[MPI_C_LONG_DOUBLE_COMPLEX] = MPI_C_LONG_DOUBLE_COMPLEX_NAME;

//   type_names[MPI_COMPLEX8]              (MPI_C_FLOAT_COMPLEX)
//   type_names[MPI_COMPLEX16]             (MPI_C_DOUBLE_COMPLEX)
//   type_names[MPI_COMPLEX32]             (MPI_C_LONG_DOUBLE_COMPLEX)
#endif

   type_names[MPI_BYTE]                = MPI_BYTE_NAME;
   type_names[MPI_PACKED]              = MPI_PACKED_NAME;

/* Special datatypes for reduction operations -- as defined in MPI 2.2 standard. */

   type_names[MPI_FLOAT_INT]           = MPI_FLOAT_INT_NAME;
   type_names[MPI_DOUBLE_INT]          = MPI_DOUBLE_INT_NAME;
   type_names[MPI_LONG_INT]            = MPI_LONG_INT_NAME;
   type_names[MPI_2INT]                = MPI_2INT_NAME;
   type_names[MPI_SHORT_INT]           = MPI_SHORT_INT_NAME;
   type_names[MPI_LONG_DOUBLE_INT]     = MPI_LONG_DOUBLE_INT_NAME;

/* BASIC Fortran Datatypes -- as defined in MPI 2.1 standard. */

//   type_names[MPI_INTEGER]               (MPI_INT)
//   type_names[MPI_REAL]                  (MPI_FLOAT)
//   type_names[MPI_DOUBLE_PRECISION]      (MPI_DOUBLE)
//   type_names[MPI_LOGICAL]               (MPI_INT) // FIXME -- check!
//   type_names[MPI_CHARACTER]             (MPI_CHAR)
   type_names[MPI_COMPLEX]             = MPI_COMPLEX_NAME;
   type_names[MPI_DOUBLE_COMPLEX]      = MPI_DOUBLE_COMPLEX_NAME;

//   type_names[MPI_INTEGER1]              (MPI_SIGNED_CHAR)
//   type_names[MPI_INTEGER2]              (MPI_SHORT)
//   type_names[MPI_INTEGER4]              (MPI_INT)
//   type_names[MPI_INTEGER8]              (MPI_LONG_LONG_INT)

//   type_names[MPI_UNSIGNED_INTEGER1]     (MPI_UNSIGNED_CHAR)
//   type_names[MPI_UNSIGNED_INTEGER2]     (MPI_UNSIGNED_SHORT)
//   type_names[MPI_UNSIGNED_INTEGER4]     (MPI_UNSIGNED)
//   type_names[MPI_UNSIGNED_INTEGER8]     (MPI_UNSIGNED_LONG_LONG)

//   type_names[MPI_REAL4]                 (MPI_FLOAT)
//   type_names[MPI_REAL8]                 (MPI_DOUBLE)
//   type_names[MPI_REAL16]                (MPI_LONG_DOUBLE)

   type_names[MPI_2REAL]                 = MPI_2REAL_NAME;
   type_names[MPI_2DOUBLE_PRECISION]     = MPI_2DOUBLE_PRECISION_NAME;
//   type_names[MPI_2INTEGER]              (MPI_2INT)

}

void XERROR(int indent, int fatal, const char *header, const char *func, const char *file, const int line, const char *fmt, ...)
{
#if PRINT_STACK_TRACE > 0
   size_t size;
   void *array[PRINT_STACK_TRACE];
#endif

   int *ptr;
   va_list argp;

   va_start(argp, fmt);

   if (indent < 0) {
      indent = 0;
   } else if (indent > 9) {
      indent = 9;
   }

   fprintf(stderr, "%s%s %s : ", indents[indent], header, func);
   vfprintf(stderr, fmt, argp);
   fprintf(stderr, " (at %s:%d)\n", file, line);

   va_end(argp);

#if PRINT_STACK_TRACE > 0
   size = backtrace(array, 5);
   backtrace_symbols_fd(array, size, 2);
#endif

   if (fatal) {
      ptr = NULL;
      *ptr = 1;
   }
}

void XLOG(int indent, int verbose, const char *header, const char *func, const char *file, const int line, const char *fmt, ...)
{
   va_list argp;

   va_start(argp, fmt);

   if (indent < 0) {
      indent = 0;
   } else if (indent > 9) {
      indent = 9;
   }

   fprintf(stderr, "%s%s %s : ", indents[indent], header, func);
   vfprintf(stderr, fmt, argp);

   if (verbose) {
      fprintf(stderr, " (at %s:%d)\n", file, line);
   } else {
      fprintf(stderr, "\n");
   }

   va_end(argp);
}

void XSTACKTRACE(int indent, const char *header, const char *func, const char *file, const int line, const char *fmt, ...)
{
   size_t size;
   void *array[15];
   va_list argp;

   va_start(argp, fmt);

   if (indent < 0) {
      indent = 0;
   } else if (indent > 9) {
      indent = 9;
   }

   fprintf(stderr, "%s%s %s : ", indents[indent], header, func);
   vfprintf(stderr, fmt, argp);
   fprintf(stderr, " (at %s:%d)\n", file, line);

   va_end(argp);

   size = backtrace(array, 15);
   backtrace_symbols_fd(array, size, 2);
}

