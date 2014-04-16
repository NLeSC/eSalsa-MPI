#ifndef _LOGGING_H_
#define _LOGGING_H_

// VERBOSE must be defined and >= 0. Usual values are:
//
// 0 only print fatal
// 1 print fatal and errors
// 2 print fatal, errors and warnings
// 3 print fatal, errors, warnings and info
// 4+ print all

#define VERBOSE 4

// Crash application when an error is printed ?
#define ERRORS_ARE_FATAL 1

// Print stacktrace when application is crashed ? (0=no, >0 is length of trace)
#define PRINT_STACK_TRACE 10

// Define several macros to print errors

#define FATAL(message, ...) (XERROR(0, 1, "FATAL", __FUNCTION__, __FILE__, __LINE__, message, ##__VA_ARGS__))

#if VERBOSE > 0
#define ERROR(indent, message, ...) XERROR(indent, ERRORS_ARE_FATAL, "ERROR", __FUNCTION__, __FILE__, __LINE__, message, ##__VA_ARGS__)
#define IERROR(indent, message, ...) XERROR(indent, 1, "INTERNAL ERROR", __FUNCTION__, __FILE__, __LINE__, message, ##__VA_ARGS__)
#else
#define ERROR(indent, error, message, ...)
#define IERROR(indent, error, message, ...)
#endif


// Define macro to print warning
#if VERBOSE > 1
#define WARN(indent, message, ...) XLOG(indent, 1, "WARN", __FUNCTION__, __FILE__, __LINE__, message, ##__VA_ARGS__)
#else
#define WARN(indent, message, ...)
#endif


// Define macro to print info
#if VERBOSE > 2
#define INFO(indent, message, ...) XLOG(indent, 0, "INFO", __FUNCTION__, __FILE__, __LINE__, message, ##__VA_ARGS__)
#else
#define INFO(indent, message, ...)
#endif


// Define macro to print debug
#if VERBOSE > 3
#define DEBUG(indent, message, ...) XLOG(indent, 1, "DEBUG", __FUNCTION__, __FILE__, __LINE__, message, ##__VA_ARGS__)
#else
#define DEBUG(indent, message, ...)
#endif


// Define macro to print debug
#if VERBOSE > 1
#define STACKTRACE(indent, message, ...) XSTACKTRACE(indent, "STACKTRACE", __FUNCTION__, __FILE__, __LINE__, message, ##__VA_ARGS__)
#else
#define STACKTRACE(indent, message, ...)
#endif

void init_logging();

// New format...
void XERROR(int indent, int fatal, const char *header, const char *func, const char *file, const int line, const char *fmt, ...);
void XLOG(int indent, int verbose, const char *header, const char *func, const char *file, const int line, const char *fmt, ...);
void XSTACKTRACE(int indent, const char *header, const char *func, const char *file, const int line, const char *fmt, ...);

#endif // _LOGGING_H_


