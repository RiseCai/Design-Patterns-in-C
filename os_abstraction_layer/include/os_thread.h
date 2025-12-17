/**
 * os_thread.h - OS Abstraction Layer Thread Interface
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * This file defines the thread abstraction interface.
 */

#ifndef __OS_THREAD_H__
#define __OS_THREAD_H__

#include "os_abstract.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Additional thread-specific APIs */

/**
 * Set thread priority
 * @param thread Thread handle
 * @param priority New priority
 * @return OS_OK on success, error code otherwise
 */
os_error_t os_thread_set_priority(os_thread_t *thread, os_thread_priority_t priority);

/**
 * Get thread priority
 * @param thread Thread handle
 * @param[out] priority Current priority
 * @return OS_OK on success, error code otherwise
 */
os_error_t os_thread_get_priority(os_thread_t *thread, os_thread_priority_t *priority);

/**
 * Yield current thread
 */
void os_thread_yield(void);

/**
 * Get thread stack usage (if supported)
 * @param thread Thread handle
 * @param[out] used Used stack size in bytes
 * @param[out] total Total stack size in bytes
 * @return OS_OK on success, OS_ERROR_NOT_SUPPORTED if not supported
 */
os_error_t os_thread_get_stack_usage(os_thread_t *thread, size_t *used, size_t *total);

#ifdef __cplusplus
}
#endif

#endif /* __OS_THREAD_H__ */
