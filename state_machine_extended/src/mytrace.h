#ifndef __MY_TRACE_H__
#define __MY_TRACE_H__

#include <stdio.h>   /* for printf */

/* Determine platform: execinfo.h is available on Linux/macOS but not Windows */
#if defined(_WIN32) || defined(_WIN64)
/* Windows platform */
#define _MY_TRACE_INIT_
#define _MY_TRACE_STR(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define _MY_TRACE_PTR(param) printf("%s(%p)\n", __FUNCTION__, param)
#define _MY_TRACE_2(param_p, param_int) printf("%s(%p, %d)\n", __FUNCTION__, param_p, param_int)
#define _MY_TRACE_ printf("%s()\n", __FUNCTION__)
#else
/* Assume Linux/macOS with execinfo.h */
#include <execinfo.h>   /* backtrace */

void _my_trace_backtrace_init(void);
int  _my_trace_backtrace_indent(void);

#define MY_TRACE_FLAG          "" /* trace log prefix */
#define MY_DEBUG_CHECK_LEVEL() 1  /* log level check */
#define __MY_DUMP printf          /* log implement */

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
#endif

#endif /* __MY_TRACE_H__ */
