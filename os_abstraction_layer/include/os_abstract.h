/**
 * os_abstract.h - OS Abstraction Layer Core Interface
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * This file defines the core abstract interfaces for OS services,
 * allowing state machines and VIPER components to be portable across
 * different operating systems (Linux, FreeRTOS, RT-Thread, Bare Metal).
 */

#ifndef __OS_ABSTRACT_H__
#define __OS_ABSTRACT_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Error codes */
typedef enum {
    OS_OK = 0,
    OS_ERROR = -1,
    OS_ERROR_TIMEOUT = -2,
    OS_ERROR_INVALID_PARAM = -3,
    OS_ERROR_NO_MEMORY = -4,
    OS_ERROR_NOT_SUPPORTED = -5,
    OS_ERROR_BUSY = -6,
    OS_ERROR_AGAIN = -7,
} os_error_t;

/* Wait forever timeout value */
#define OS_WAIT_FOREVER (0xFFFFFFFFU)

/* Forward declarations */
typedef struct os_thread os_thread_t;
typedef struct os_mutex os_mutex_t;
typedef struct os_queue os_queue_t;
typedef struct os_timer os_timer_t;
typedef struct os_semaphore os_semaphore_t;
typedef struct os_event_group os_event_group_t;
typedef struct os_memory_pool os_memory_pool_t;
typedef struct os_file os_file_t;
typedef struct os_socket os_socket_t;

/**
 * Thread priority levels
 */
typedef enum {
    OS_THREAD_PRIORITY_IDLE = 0,
    OS_THREAD_PRIORITY_LOW,
    OS_THREAD_PRIORITY_NORMAL,
    OS_THREAD_PRIORITY_HIGH,
    OS_THREAD_PRIORITY_REALTIME,
    OS_THREAD_PRIORITY_MAX
} os_thread_priority_t;

/**
 * Thread entry function prototype
 */
typedef void (*os_thread_entry_t)(void *arg);

/**
 * Timer callback function prototype
 */
typedef void (*os_timer_callback_t)(void *arg);

/**
 * OS Abstraction Layer API
 */

/* Thread Management */
os_thread_t *os_thread_create(os_thread_entry_t entry, void *arg,
                              const char *name,
                              size_t stack_size,
                              os_thread_priority_t priority);
os_error_t os_thread_join(os_thread_t *thread, uint32_t timeout_ms);
os_error_t os_thread_detach(os_thread_t *thread);
void os_thread_sleep(uint32_t milliseconds);
os_thread_t *os_thread_self(void);
const char *os_thread_get_name(os_thread_t *thread);

/* Mutex */
os_mutex_t *os_mutex_create(void);
os_error_t os_mutex_lock(os_mutex_t *mutex, uint32_t timeout_ms);
os_error_t os_mutex_unlock(os_mutex_t *mutex);
os_error_t os_mutex_destroy(os_mutex_t *mutex);

/* Queue */
os_queue_t *os_queue_create(size_t item_size, size_t queue_depth);
os_error_t os_queue_send(os_queue_t *queue, const void *item, uint32_t timeout_ms);
os_error_t os_queue_receive(os_queue_t *queue, void *item, uint32_t timeout_ms);
os_error_t os_queue_peek(os_queue_t *queue, void *item, uint32_t timeout_ms);
size_t os_queue_count(os_queue_t *queue);
size_t os_queue_space(os_queue_t *queue);
os_error_t os_queue_destroy(os_queue_t *queue);

/* Timer */
os_timer_t *os_timer_create(os_timer_callback_t callback, void *arg, const char *name);
os_error_t os_timer_start(os_timer_t *timer, uint32_t period_ms, bool repeat);
os_error_t os_timer_stop(os_timer_t *timer);
os_error_t os_timer_reset(os_timer_t *timer);
os_error_t os_timer_destroy(os_timer_t *timer);

/* Semaphore (optional) */
os_semaphore_t *os_semaphore_create(uint32_t initial_count, uint32_t max_count);
os_error_t os_semaphore_take(os_semaphore_t *sem, uint32_t timeout_ms);
os_error_t os_semaphore_give(os_semaphore_t *sem);
os_error_t os_semaphore_destroy(os_semaphore_t *sem);

/* Event Group (optional) */
os_event_group_t *os_event_group_create(void);
uint32_t os_event_group_wait(os_event_group_t *group, uint32_t bits,
                             bool clear_on_exit, bool wait_for_all,
                             uint32_t timeout_ms);
uint32_t os_event_group_set(os_event_group_t *group, uint32_t bits);
uint32_t os_event_group_clear(os_event_group_t *group, uint32_t bits);
os_error_t os_event_group_destroy(os_event_group_t *group);

/* Memory Pool (optional) */
os_memory_pool_t *os_memory_pool_create(size_t block_size, size_t block_count);
void *os_memory_pool_alloc(os_memory_pool_t *pool, uint32_t timeout_ms);
os_error_t os_memory_pool_free(os_memory_pool_t *pool, void *block);
os_error_t os_memory_pool_destroy(os_memory_pool_t *pool);

/* System */
uint32_t os_get_tick_count(void);
uint32_t os_get_tick_frequency(void);
void os_delay(uint32_t milliseconds);

/* Initialization */
os_error_t os_abstract_init(void);
os_error_t os_abstract_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* __OS_ABSTRACT_H__ */
