#ifndef __MY_TRACE_H__
#define __MY_TRACE_H__

#include <stdio.h>   /* for printf */

/* Determine platform: execinfo.h is available on Linux/macOS but not Windows */
#if defined(_WIN32) || defined(_WIN64)
/* Windows platform */
#define MYTRACE_NO_EXECINFO 1
#else
/* Linux/macOS: check if execinfo.h is available */
#ifdef __has_include
# if __has_include(<execinfo.h>)
#  include <execinfo.h>   /* backtrace */
#  define MYTRACE_NO_EXECINFO 0
# else
#  define MYTRACE_NO_EXECINFO 1
# endif
#else
/* assume execinfo.h exists */
# include <execinfo.h>
# define MYTRACE_NO_EXECINFO 0
#endif
#endif

#if MYTRACE_NO_EXECINFO
/* execinfo.h not available, provide stub functions */
static inline void _my_trace_backtrace_init(void) {}
static inline int _my_trace_backtrace_indent(void) { return 0; }
#else
void _my_trace_backtrace_init(void);
int  _my_trace_backtrace_indent(void);
#endif

#define MY_TRACE_FLAG          "" /* trace log prefix */
#define MY_DEBUG_CHECK_LEVEL() 1  /* log level check */
#define __MY_DUMP printf          /* log implement */

#if MYTRACE_NO_EXECINFO
/* Simple tracing without backtrace indentation */
#define _MY_TRACE_INIT_
#define _MY_TRACE_STR(fmt, ...) \
    if (MY_DEBUG_CHECK_LEVEL()) { \
        __MY_DUMP(fmt, ##__VA_ARGS__); \
    }
#define _MY_TRACE_PTR(param) \
    if (MY_DEBUG_CHECK_LEVEL()) { \
        __MY_DUMP("%s(%p)\n", __FUNCTION__, param); \
    }
#define _MY_TRACE_2(param_p, param_int) \
    if (MY_DEBUG_CHECK_LEVEL()) { \
        __MY_DUMP("%s(%p, %d)\n", __FUNCTION__, param_p, param_int); \
    }
#define _MY_TRACE_ \
    if (MY_DEBUG_CHECK_LEVEL()) { \
        __MY_DUMP("%s()\n", __FUNCTION__); \
    }
#else
/* Full tracing with backtrace indentation */
#define _MY_TRACE_INIT_ \
    _my_trace_backtrace_init();

#define _MY_TRACE_STR(fmt, ...) \
    if (MY_DEBUG_CHECK_LEVEL()) {                                      \
        int _my_trace_indent;                                          \
        _my_trace_indent = _my_trace_backtrace_indent();               \
        __MY_DUMP(MY_TRACE_FLAG"%*s", 3*_my_trace_indent, " ");        \
        __MY_DUMP(fmt, ##__VA_ARGS__);                                 \
    }

#define _MY_TRACE_PTR(param) \
    if (MY_DEBUG_CHECK_LEVEL()) {                                      \
        int _my_trace_indent;                                          \
        _my_trace_indent = _my_trace_backtrace_indent();               \
        __MY_DUMP(MY_TRACE_FLAG"%*s%s(%p)\n", 3*_my_trace_indent, " ", \
            __FUNCTION__, param);                                      \
    }

#define _MY_TRACE_2(param_p, param_int) \
    if (MY_DEBUG_CHECK_LEVEL()) {                                      \
        int _my_trace_indent;                                          \
        _my_trace_indent = _my_trace_backtrace_indent();               \
        __MY_DUMP(MY_TRACE_FLAG"%*s%s(%p)\n", 3*_my_trace_indent, " ", \
                __FUNCTION__, param_p, param_int);                     \
    }

#define _MY_TRACE_ \
    if (MY_DEBUG_CHECK_LEVEL()) {                                      \
        int _my_trace_indent;                                          \
        _my_trace_indent = _my_trace_backtrace_indent();               \
        __MY_DUMP(MY_TRACE_FLAG"%*s%s()\n", 3*_my_trace_indent, " ",   \
            __FUNCTION__);                                             \
    }
#endif /* MYTRACE_NO_EXECINFO */

/* Convenience macro for pipeline logging */
#define TRACE_INFO(fmt, ...) _MY_TRACE_STR(fmt "\n", ##__VA_ARGS__)

#endif /* __MY_TRACE_H__ */
