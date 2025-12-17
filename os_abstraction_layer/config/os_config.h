/**
 * os_config.h - OS Abstraction Layer Configuration
 *
 * Copyright (C) 2000-2025 All Right Reserved
 *
 * This file defines compile-time configuration for the OS abstraction layer.
 */

#ifndef __OS_CONFIG_H__
#define __OS_CONFIG_H__

/* OS type selection */
#if defined(OS_TYPE_LINUX)
    #define OS_ABSTRACTION_IMPL linux_impl
    #define OS_POSIX_COMPATIBLE 1
#elif defined(OS_TYPE_FREERTOS)
    #define OS_ABSTRACTION_IMPL freertos_impl
    #define OS_POSIX_COMPATIBLE 0
#elif defined(OS_TYPE_RT_THREAD)
    #define OS_ABSTRACTION_IMPL rt_thread_impl
    #define OS_POSIX_COMPATIBLE 0
#elif defined(OS_TYPE_BARE_METAL)
    #define OS_ABSTRACTION_IMPL bare_metal_impl
    #define OS_POSIX_COMPATIBLE 0
#else
    #error "No OS type defined. Please define OS_TYPE_LINUX, OS_TYPE_FREERTOS, OS_TYPE_RT_THREAD, or OS_TYPE_BARE_METAL"
#endif

/* Feature configuration */
#ifndef OS_ABSTRACTION_USE_SEMAPHORE
#define OS_ABSTRACTION_USE_SEMAPHORE 1
#endif

#ifndef OS_ABSTRACTION_USE_EVENT_GROUP
#define OS_ABSTRACTION_USE_EVENT_GROUP 1
#endif

#ifndef OS_ABSTRACTION_USE_MEMORY_POOL
#define OS_ABSTRACTION_USE_MEMORY_POOL 1
#endif

#ifndef OS_ABSTRACTION_USE_FILE
#define OS_ABSTRACTION_USE_FILE 1
#endif

#ifndef OS_ABSTRACTION_USE_NETWORK
#define OS_ABSTRACTION_USE_NETWORK 1
#endif

/* Debug configuration */
#ifndef OS_ABSTRACTION_DEBUG
#define OS_ABSTRACTION_DEBUG 0
#endif

#if OS_ABSTRACTION_DEBUG
#include <stdio.h>
#define OS_DEBUG_PRINT(fmt, ...) printf("[OS] " fmt, ##__VA_ARGS__)
#else
#define OS_DEBUG_PRINT(fmt, ...)
#endif

/* Platform-specific includes */
#if defined(OS_TYPE_LINUX)
#include <pthread.h>
#include <semaphore.h>
#include <mqueue.h>
#include <time.h>
#include <errno.h>
#endif

#endif /* __OS_CONFIG_H__ */
