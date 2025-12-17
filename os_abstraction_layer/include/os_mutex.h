/**
 * os_mutex.h - OS Abstraction Layer Mutex Interface
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * This file defines the mutex abstraction interface.
 */

#ifndef __OS_MUTEX_H__
#define __OS_MUTEX_H__

#include "os_abstract.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Additional mutex-specific APIs */

/**
 * Try to lock mutex without blocking
 * @param mutex Mutex handle
 * @return OS_OK if locked, OS_ERROR_BUSY if already locked, error code otherwise
 */
os_error_t os_mutex_trylock(os_mutex_t *mutex);

/**
 * Check if mutex is locked by current thread (if supported)
 * @param mutex Mutex handle
 * @return true if locked by current thread, false otherwise
 */
bool os_mutex_is_locked(os_mutex_t *mutex);

#ifdef __cplusplus
}
#endif

#endif /* __OS_MUTEX_H__ */
