#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <execinfo.h>

#include "flags.h"
#include "logging.h"

#include "empi.h"

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

char *comm_to_string(EMPI_Comm comm)
{
   int len = 0;
   char tmp[64];

   if (comm == EMPI_COMM_WORLD) {
      len = sprintf(tmp, "<WORLD>");
      return copy_to_debug_buf(tmp, len+1);
   }

   if (comm == EMPI_COMM_SELF) {
      len = sprintf(tmp, "<SELF>");
      return copy_to_debug_buf(tmp, len+1);
   }

   if (comm == EMPI_COMM_NULL) {
      len = sprintf(tmp, "<NULL>");
      return copy_to_debug_buf(tmp, len+1);
   }

   len = sprintf(tmp, "<%d>", comm);
   return copy_to_debug_buf(tmp, len+1);
}

char *request_to_string(EMPI_Request r)
{
   int len;
   char tmp[64];

   if (r == EMPI_REQUEST_NULL) {
      len = sprintf(tmp, "<NULL>");
      return copy_to_debug_buf(tmp, len+1);
   }

   len = sprintf(tmp, "<%d>", r);
   return copy_to_debug_buf(tmp, len+1);
}

char *group_to_string(EMPI_Group g)
{
   int len;
   char tmp[64];

   if (g == EMPI_GROUP_EMPTY) {
      len = sprintf(tmp, "<EMPTY>");
      return copy_to_debug_buf(tmp, len+1);
   }

   if (g == EMPI_GROUP_NULL) {
      len = sprintf(tmp, "<NULL>");
      return copy_to_debug_buf(tmp, len+1);
   }

   len = sprintf(tmp, "<%d>", g);
   return copy_to_debug_buf(tmp, len+1);
}


char *type_to_string(EMPI_Datatype type)
{
   int len = 0;
   char tmp[1024];

   int error = EMPI_Type_get_name(type, tmp, &len);

   if (error != EMPI_SUCCESS || len <= 0) {
      len = sprintf(tmp, "<UNKNOWN>");
   }

   return copy_to_debug_buf(tmp, len+1);
}

char *op_to_string(EMPI_Op o)
{
   int len;
   char tmp[64];

   // FIXME

   tmp[0] = '\0';
   len = 0;

   return copy_to_debug_buf(tmp, len+1);
}

char *info_to_string(EMPI_Info i)
{
   int len;
   char tmp[64];

   len = sprintf(tmp, "<%d>", i);
   return copy_to_debug_buf(tmp, len+1);
}

char *file_to_string(EMPI_File f)
{
   int len;
   char tmp[64];

   len = sprintf(tmp, "<%d>", f);
   return copy_to_debug_buf(tmp, len+1);
}

char *win_to_string(EMPI_Win w)
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


void init_debug()
{
   int i;
   char *tmp = maxindent;

   for (i=9;i>=0;i--) {
      indents[i] = tmp++;
   }

   debug_buf[DEBUG_BUF_SIZE] = '\0';
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

