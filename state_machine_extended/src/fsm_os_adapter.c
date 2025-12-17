/**
 * fsm_os_adapter.c - OS Abstraction Layer Adapter Implementation
 *
 * Copyright (C) 2000-2025 All Right Reserved
 *
 * This file implements the adapter interfaces defined in fsm_os_adapter.h.
 * It provides concrete integration of the five extended state machines
 * with the OS abstraction layer.
 */

#include "fsm_os_adapter.h"
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------------- */
/* Generic OS context management                                             */
/* ------------------------------------------------------------------------- */

fsm_os_context_t *fsm_os_context_create(void)
{
    fsm_os_context_t *ctx = (fsm_os_context_t *)malloc(sizeof(fsm_os_context_t));
    if (!ctx) {
        return NULL;
    }
    memset(ctx, 0, sizeof(fsm_os_context_t));

    /* Create default objects (optional, can be created on demand) */
    ctx->mutex = os_mutex_create();
    if (!ctx->mutex) {
        free(ctx);
        return NULL;
    }

    /* Other objects are left as NULL; they will be created when needed */
    return ctx;
}

void fsm_os_context_destroy(fsm_os_context_t *ctx)
{
    if (!ctx) {
        return;
    }

    /* Destroy each OS object if it exists */
    if (ctx->thread) {
        os_thread_detach(ctx->thread);
        /* Note: actual thread cleanup depends on OS; we assume detach is enough */
    }
    if (ctx->event_queue) {
        os_queue_destroy(ctx->event_queue);
    }
    if (ctx->timer) {
        os_timer_stop(ctx->timer);
        os_timer_destroy(ctx->timer);
    }
    if (ctx->mutex) {
        os_mutex_destroy(ctx->mutex);
    }
    if (ctx->semaphore) {
        os_semaphore_destroy(ctx->semaphore);
    }
    if (ctx->event_group) {
        os_event_group_destroy(ctx->event_group);
    }
    if (ctx->memory_pool) {
        os_memory_pool_destroy(ctx->memory_pool);
    }

    free(ctx);
}

os_error_t fsm_os_send_event(fsm_os_context_t *ctx, const void *event_data, size_t size, uint32_t timeout_ms)
{
    if (!ctx || !ctx->event_queue) {
        return OS_ERROR_INVALID_PARAM;
    }
    /* The OS queue expects item size as defined at creation.
     * We assume the event queue was created with the correct item size.
     * For simplicity, we copy the data into a temporary buffer.
     */
    return os_queue_send(ctx->event_queue, event_data, timeout_ms);
}

os_error_t fsm_os_receive_event(fsm_os_context_t *ctx, void *event_data, size_t size, uint32_t timeout_ms)
{
    if (!ctx || !ctx->event_queue) {
        return OS_ERROR_INVALID_PARAM;
    }
    return os_queue_receive(ctx->event_queue, event_data, timeout_ms);
}

/* ------------------------------------------------------------------------- */
/* Adapter factories                                                         */
/* ------------------------------------------------------------------------- */

fsm_os_context_t *mealy_ui_adapter_create(void)
{
    fsm_os_context_t *ctx = fsm_os_context_create();
    if (!ctx) {
        return NULL;
    }

    /* Create an event queue for UI events (e.g., clicks, hovers) */
    ctx->event_queue = os_queue_create(sizeof(int), 10); /* event type as int */
    if (!ctx->event_queue) {
        fsm_os_context_destroy(ctx);
        return NULL;
    }

    /* Create a timer for debouncing */
    ctx->timer = os_timer_create(NULL, ctx, "mealy_ui_timer");
    if (!ctx->timer) {
        fsm_os_context_destroy(ctx);
        return NULL;
    }

    return ctx;
}

fsm_os_context_t *efsm_protocol_adapter_create(void)
{
    fsm_os_context_t *ctx = fsm_os_context_create();
    if (!ctx) {
        return NULL;
    }

    /* Create a larger queue for protocol messages */
    ctx->event_queue = os_queue_create(256, 20); /* message up to 256 bytes */
    if (!ctx->event_queue) {
        fsm_os_context_destroy(ctx);
        return NULL;
    }

    /* Create a timer for retransmission */
    ctx->timer = os_timer_create(NULL, ctx, "efsm_protocol_timer");
    if (!ctx->timer) {
        fsm_os_context_destroy(ctx);
        return NULL;
    }

    /* Create a semaphore for flow control */
    ctx->semaphore = os_semaphore_create(5, 5); /* allow up to 5 concurrent messages */
    if (!ctx->semaphore) {
        fsm_os_context_destroy(ctx);
        return NULL;
    }

    return ctx;
}

fsm_os_context_t *moore_hierarchical_adapter_create(void)
{
    fsm_os_context_t *ctx = fsm_os_context_create();
    if (!ctx) {
        return NULL;
    }

    /* Create a memory pool for state objects (each state ~ 64 bytes) */
    ctx->memory_pool = os_memory_pool_create(64, 10); /* 10 states */
    if (!ctx->memory_pool) {
        fsm_os_context_destroy(ctx);
        return NULL;
    }

    /* Mutex is already created in fsm_os_context_create() */
    return ctx;
}

fsm_os_context_t *parallel_fsm_adapter_create(int num_components)
{
    fsm_os_context_t *ctx = fsm_os_context_create();
    if (!ctx) {
        return NULL;
    }

    /* Create an event group for synchronizing multiple FSMs */
    ctx->event_group = os_event_group_create();
    if (!ctx->event_group) {
        fsm_os_context_destroy(ctx);
        return NULL;
    }

    /* Create a semaphore to limit concurrent active components */
    ctx->semaphore = os_semaphore_create(num_components, num_components);
    if (!ctx->semaphore) {
        fsm_os_context_destroy(ctx);
        return NULL;
    }

    /* Note: Threads for each component are not created here;
     * they should be created separately via fsm_os_start_thread().
     */
    return ctx;
}

fsm_os_context_t *acceptor_regex_adapter_create(void)
{
    fsm_os_context_t *ctx = fsm_os_context_create();
    if (!ctx) {
        return NULL;
    }

    /* Create a timer for matching timeout */
    ctx->timer = os_timer_create(NULL, ctx, "acceptor_regex_timer");
    if (!ctx->timer) {
        fsm_os_context_destroy(ctx);
        return NULL;
    }

    /* Note: File I/O is handled via os_file.h, not part of generic context.
     * The user can open files directly using os_file APIs.
     */
    return ctx;
}

/* ------------------------------------------------------------------------- */
/* Integration helpers                                                       */
/* ------------------------------------------------------------------------- */

os_error_t fsm_os_start_thread(fsm_os_context_t *ctx, os_thread_entry_t entry, void *arg)
{
    if (!ctx || !entry) {
        return OS_ERROR_INVALID_PARAM;
    }

    /* If a thread already exists, detach it first */
    if (ctx->thread) {
        os_thread_detach(ctx->thread);
    }

    /* Create a new thread with default parameters */
    ctx->thread = os_thread_create(entry, arg, "fsm_thread", 4096, OS_THREAD_PRIORITY_NORMAL);
    if (!ctx->thread) {
        return OS_ERROR;
    }

    return OS_OK;
}

os_error_t fsm_os_stop_thread(fsm_os_context_t *ctx, uint32_t timeout_ms)
{
    if (!ctx || !ctx->thread) {
        return OS_ERROR_INVALID_PARAM;
    }

    /* For simplicity, we just detach the thread.
     * In a real implementation, we might send a termination signal and join.
     */
    os_error_t err = os_thread_detach(ctx->thread);
    if (err == OS_OK) {
        ctx->thread = NULL;
    }
    return err;
}

os_error_t fsm_os_set_timer(fsm_os_context_t *ctx, uint32_t period_ms, bool repeat)
{
    if (!ctx || !ctx->timer) {
        return OS_ERROR_INVALID_PARAM;
    }
    return os_timer_start(ctx->timer, period_ms, repeat);
}

os_error_t fsm_os_cancel_timer(fsm_os_context_t *ctx)
{
    if (!ctx || !ctx->timer) {
        return OS_ERROR_INVALID_PARAM;
    }
    return os_timer_stop(ctx->timer);
}
