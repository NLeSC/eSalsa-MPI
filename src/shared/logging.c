#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <execinfo.h>

#include "logging.h"

// Indentation levels (single string with multiple pointers
// pointing into it at different locations).
static char *maxindent = "         ";
static char *indents[10];

void init_logging()
{
   int i;
   char *tmp = maxindent;

   for (i=9;i>=0;i--) {
      indents[i] = tmp++;
   }
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

