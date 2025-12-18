/**
 * fsm_os_adapter.h - OS Abstraction Layer Adapter for State Machines
 *
 * Copyright (C) 2000-2025 All Right Reserved
 *
 * This file provides adapter interfaces to integrate the five extended
 * state machines with the OS abstraction layer (os_abstract.h).
 * It enables portable concurrency, timing, synchronization, and I/O
 * across different operating systems.
 */

#ifndef __FSM_OS_ADAPTER_H__
#define __FSM_OS_ADAPTER_H__

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* Include the OS abstraction layer */
#include "../../os_abstraction_layer/include/os_abstract.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declarations */
struct mealy_machine;
struct efsm_protocol;
struct moore_hierarchical;
struct parallel_machine;
struct acceptor_regex;

/**
 * Generic OS context for any state machine.
 * Contains commonly used OS objects.
 */
typedef struct {
    os_thread_t *thread;           /* Thread running the state machine */
    os_queue_t *event_queue;       /* Queue for incoming events */
    os_timer_t *timer;             /* Timer for timeouts */
    os_mutex_t *mutex;             /* Mutex for protecting shared data */
    os_semaphore_t *semaphore;     /* Semaphore for synchronization */
    os_event_group_t *event_group; /* Event group for multi‑FSM sync */
    os_memory_pool_t *memory_pool; /* Memory pool for state objects */
    void *user_data;               /* User‑specific data */
} fsm_os_context_t;

/**
 * Creates a generic OS context with default objects.
 * Returns NULL on failure.
 */
fsm_os_context_t *fsm_os_context_create(void);

/**
 * Destroys an OS context and releases all associated resources.
 */
void fsm_os_context_destroy(fsm_os_context_t *ctx);

/**
 * Sends an event to the context's event queue.
 * Returns OS_OK on success, error code otherwise.
 */
os_error_t fsm_os_send_event(fsm_os_context_t *ctx, const void *event_data, size_t size, uint32_t timeout_ms);

/**
 * Receives an event from the context's event queue.
 * Returns OS_OK on success, error code otherwise.
 */
os_error_t fsm_os_receive_event(fsm_os_context_t *ctx, void *event_data, size_t size, uint32_t timeout_ms);

/* ------------------------------------------------------------------------- */
/* Adapter factories for each state machine type                             */
/* ------------------------------------------------------------------------- */

/**
 * Mealy UI Adapter
 * Provides timer‑based debouncing and an event queue for UI events.
 */
fsm_os_context_t *mealy_ui_adapter_create(void);

/**
 * EFSM Protocol Adapter
 * Provides network I/O abstraction, message queues, and retransmission timers.
 */
fsm_os_context_t *efsm_protocol_adapter_create(void);

/**
 * Moore Hierarchical Adapter
 * Provides memory pools for state allocation and mutexes for state protection.
 */
fsm_os_context_t *moore_hierarchical_adapter_create(void);

/**
 * Parallel FSM Adapter
 * Creates a thread for each component, plus event groups for synchronization.
 * @param num_components Number of parallel components (threads to create).
 */
fsm_os_context_t *parallel_fsm_adapter_create(int num_components);

/**
 * Acceptor/Regex Adapter
 * Provides file I/O for input streams and a timeout timer for matching.
 */
fsm_os_context_t *acceptor_regex_adapter_create(void);

/**
 * LED FSM Adapter (Product‑specific)
 * Provides PWM update timer, command queue, and synchronization for LED control.
 */
fsm_os_context_t *led_fsm_adapter_create(void);

/* ------------------------------------------------------------------------- */
/* Integration helpers                                                       */
/* ------------------------------------------------------------------------- */

/**
 * Starts the state machine's dedicated thread (if any).
 * The thread will run the provided entry function.
 */
os_error_t fsm_os_start_thread(fsm_os_context_t *ctx, os_thread_entry_t entry, void *arg);

/**
 * Stops the state machine's thread (graceful shutdown).
 */
os_error_t fsm_os_stop_thread(fsm_os_context_t *ctx, uint32_t timeout_ms);

/**
 * Sets a periodic timer in the context.
 * @param period_ms Timer period in milliseconds.
 * @param repeat   True for periodic, false for one‑shot.
 */
os_error_t fsm_os_set_timer(fsm_os_context_t *ctx, uint32_t period_ms, bool repeat);

/**
 * Cancels the timer in the context.
 */
os_error_t fsm_os_cancel_timer(fsm_os_context_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* __FSM_OS_ADAPTER_H__ */
