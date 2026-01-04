/**
 * linux_adapter.c - Linux POSIX Adapter Implementation
 *
 * Copyright (C) 2000-2025 All Right Reserved
 *
 * This file implements the Linux-specific adapter using POSIX APIs.
 */

#include "linux_adapter.h"
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* mqueue.h may not be available on all platforms */
#ifdef __linux__
#include <mqueue.h>
#else
/* Define dummy types for non-Linux */
typedef int mqd_t;
#define MQ_PRIO_MAX 16
#endif

/* Internal structures */
struct os_thread {
    pthread_t thread;
    char name[32];
    os_thread_entry_t entry;
    void *arg;
};

struct os_mutex {
    pthread_mutex_t mutex;
};

struct os_queue {
    pthread_mutex_t mutex;
    pthread_cond_t cond_not_empty;
    pthread_cond_t cond_not_full;
    size_t item_size;
    size_t queue_depth;
    char *buffer;
    size_t head;
    size_t tail;
    size_t count;
    bool closed;
};

struct os_timer {
    timer_t timerid;
    os_timer_callback_t callback;
    void *arg;
    bool repeat;
};

struct os_semaphore {
    sem_t *sem;
    char name[32];
    bool named;
};

struct os_event_group {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    uint32_t bits;
};

struct os_memory_pool {
    void *pool;
    size_t block_size;
    size_t block_count;
    uint8_t *used;
};

/* Helper functions */
int linux_to_posix_error(int err) {
    switch (err) {
        case 0: return 0;
        case EINVAL: return OS_ERROR_INVALID_PARAM;
        case ENOMEM: return OS_ERROR_NO_MEMORY;
        case ETIMEDOUT: return OS_ERROR_TIMEOUT;
        case EBUSY: return OS_ERROR_BUSY;
        case EAGAIN: return OS_ERROR_AGAIN;
        default: return OS_ERROR;
    }
}

int posix_to_os_error(int err) {
    return linux_to_posix_error(err);
}

/* Thread management */
static os_thread_t *linux_thread_create(os_thread_entry_t entry, void *arg,
                                        const char *name,
                                        size_t stack_size,
                                        os_thread_priority_t priority) {
    (void)stack_size; /* Not used in pthread */
    (void)priority;   /* TODO: map priority */

    os_thread_t *thread = malloc(sizeof(os_thread_t));
    if (!thread) return NULL;

    thread->entry = entry;
    thread->arg = arg;
    strncpy(thread->name, name ? name : "thread", sizeof(thread->name) - 1);
    thread->name[sizeof(thread->name) - 1] = '\0';

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    int ret = pthread_create(&thread->thread, &attr,
                             (void *(*)(void *))entry, arg);
    pthread_attr_destroy(&attr);

    if (ret != 0) {
        free(thread);
        return NULL;
    }

    return thread;
}

static os_error_t linux_thread_join(os_thread_t *thread, uint32_t timeout_ms) {
    (void)timeout_ms; /* pthread_join doesn't support timeout */
    if (!thread) return OS_ERROR_INVALID_PARAM;
    int ret = pthread_join(thread->thread, NULL);
    free(thread);
    return (ret == 0) ? OS_OK : OS_ERROR;
}

static os_error_t linux_thread_detach(os_thread_t *thread) {
    if (!thread) return OS_ERROR_INVALID_PARAM;
    int ret = pthread_detach(thread->thread);
    free(thread);
    return (ret == 0) ? OS_OK : OS_ERROR;
}

static void linux_thread_sleep(uint32_t milliseconds) {
    usleep(milliseconds * 1000);
}

static os_thread_t *linux_thread_self(void) {
    /* Not implemented for simplicity */
    return NULL;
}

static const char *linux_thread_get_name(os_thread_t *thread) {
    return thread ? thread->name : "unknown";
}

/* Mutex */
static os_mutex_t *linux_mutex_create(void) {
    os_mutex_t *mutex = malloc(sizeof(os_mutex_t));
    if (!mutex) return NULL;
    if (pthread_mutex_init(&mutex->mutex, NULL) != 0) {
        free(mutex);
        return NULL;
    }
    return mutex;
}

static os_error_t linux_mutex_lock(os_mutex_t *mutex, uint32_t timeout_ms) {
    if (!mutex) return OS_ERROR_INVALID_PARAM;
    if (timeout_ms == OS_WAIT_FOREVER) {
        int ret = pthread_mutex_lock(&mutex->mutex);
        return (ret == 0) ? OS_OK : OS_ERROR;
    } else {
        /* pthread_mutex_timedlock */
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += timeout_ms / 1000;
        ts.tv_nsec += (timeout_ms % 1000) * 1000000;
        if (ts.tv_nsec >= 1000000000) {
            ts.tv_sec += 1;
            ts.tv_nsec -= 1000000000;
        }
        int ret = pthread_mutex_timedlock(&mutex->mutex, &ts);
        return (ret == 0) ? OS_OK : OS_ERROR_TIMEOUT;
    }
}

static os_error_t linux_mutex_unlock(os_mutex_t *mutex) {
    if (!mutex) return OS_ERROR_INVALID_PARAM;
    int ret = pthread_mutex_unlock(&mutex->mutex);
    return (ret == 0) ? OS_OK : OS_ERROR;
}

static os_error_t linux_mutex_destroy(os_mutex_t *mutex) {
    if (!mutex) return OS_ERROR_INVALID_PARAM;
    pthread_mutex_destroy(&mutex->mutex);
    free(mutex);
    return OS_OK;
}

/* Queue */
static os_queue_t *linux_queue_create(size_t item_size, size_t queue_depth) {
    if (item_size == 0 || queue_depth == 0) {
        return NULL;
    }
    os_queue_t *queue = malloc(sizeof(os_queue_t));
    if (!queue) {
        return NULL;
    }
    if (pthread_mutex_init(&queue->mutex, NULL) != 0) {
        free(queue);
        return NULL;
    }
    if (pthread_cond_init(&queue->cond_not_empty, NULL) != 0) {
        pthread_mutex_destroy(&queue->mutex);
        free(queue);
        return NULL;
    }
    if (pthread_cond_init(&queue->cond_not_full, NULL) != 0) {
        pthread_cond_destroy(&queue->cond_not_empty);
        pthread_mutex_destroy(&queue->mutex);
        free(queue);
        return NULL;
    }
    queue->item_size = item_size;
    queue->queue_depth = queue_depth;
    queue->buffer = malloc(item_size * queue_depth);
    if (!queue->buffer) {
        pthread_cond_destroy(&queue->cond_not_full);
        pthread_cond_destroy(&queue->cond_not_empty);
        pthread_mutex_destroy(&queue->mutex);
        free(queue);
        return NULL;
    }
    queue->head = 0;
    queue->tail = 0;
    queue->count = 0;
    queue->closed = false;
    return queue;
}

static os_error_t linux_queue_send(os_queue_t *queue, const void *item, uint32_t timeout_ms) {
    if (!queue || !item || queue->closed) {
        return OS_ERROR_INVALID_PARAM;
    }
    struct timespec ts;
    if (timeout_ms != OS_WAIT_FOREVER) {
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += timeout_ms / 1000;
        ts.tv_nsec += (timeout_ms % 1000) * 1000000;
        if (ts.tv_nsec >= 1000000000) {
            ts.tv_sec += 1;
            ts.tv_nsec -= 1000000000;
        }
    }
    pthread_mutex_lock(&queue->mutex);
    while (queue->count == queue->queue_depth && !queue->closed) {
        if (timeout_ms == OS_WAIT_FOREVER) {
            pthread_cond_wait(&queue->cond_not_full, &queue->mutex);
        } else {
            int ret = pthread_cond_timedwait(&queue->cond_not_full, &queue->mutex, &ts);
            if (ret == ETIMEDOUT) {
                pthread_mutex_unlock(&queue->mutex);
                return OS_ERROR_TIMEOUT;
            }
        }
    }
    if (queue->closed) {
        pthread_mutex_unlock(&queue->mutex);
        return OS_ERROR;
    }
    /* Copy item into buffer */
    char *dest = queue->buffer + queue->tail * queue->item_size;
    memcpy(dest, item, queue->item_size);
    queue->tail = (queue->tail + 1) % queue->queue_depth;
    queue->count++;
    pthread_cond_signal(&queue->cond_not_empty);
    pthread_mutex_unlock(&queue->mutex);
    return OS_OK;
}

static os_error_t linux_queue_receive(os_queue_t *queue, void *item, uint32_t timeout_ms) {
    if (!queue || !item) {
        return OS_ERROR_INVALID_PARAM;
    }
    struct timespec ts;
    if (timeout_ms != OS_WAIT_FOREVER) {
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += timeout_ms / 1000;
        ts.tv_nsec += (timeout_ms % 1000) * 1000000;
        if (ts.tv_nsec >= 1000000000) {
            ts.tv_sec += 1;
            ts.tv_nsec -= 1000000000;
        }
    }
    pthread_mutex_lock(&queue->mutex);
    while (queue->count == 0 && !queue->closed) {
        if (timeout_ms == OS_WAIT_FOREVER) {
            pthread_cond_wait(&queue->cond_not_empty, &queue->mutex);
        } else {
            int ret = pthread_cond_timedwait(&queue->cond_not_empty, &queue->mutex, &ts);
            if (ret == ETIMEDOUT) {
                pthread_mutex_unlock(&queue->mutex);
                return OS_ERROR_TIMEOUT;
            }
        }
    }
    if (queue->closed && queue->count == 0) {
        pthread_mutex_unlock(&queue->mutex);
        return OS_ERROR;
    }
    /* Copy item from buffer */
    char *src = queue->buffer + queue->head * queue->item_size;
    memcpy(item, src, queue->item_size);
    queue->head = (queue->head + 1) % queue->queue_depth;
    queue->count--;
    pthread_cond_signal(&queue->cond_not_full);
    pthread_mutex_unlock(&queue->mutex);
    return OS_OK;
}

static os_error_t linux_queue_peek(os_queue_t *queue, void *item, uint32_t timeout_ms) {
    if (!queue || !item) {
        return OS_ERROR_INVALID_PARAM;
    }
    struct timespec ts;
    if (timeout_ms != OS_WAIT_FOREVER) {
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += timeout_ms / 1000;
        ts.tv_nsec += (timeout_ms % 1000) * 1000000;
        if (ts.tv_nsec >= 1000000000) {
            ts.tv_sec += 1;
            ts.tv_nsec -= 1000000000;
        }
    }
    pthread_mutex_lock(&queue->mutex);
    while (queue->count == 0 && !queue->closed) {
        if (timeout_ms == OS_WAIT_FOREVER) {
            pthread_cond_wait(&queue->cond_not_empty, &queue->mutex);
        } else {
            int ret = pthread_cond_timedwait(&queue->cond_not_empty, &queue->mutex, &ts);
            if (ret == ETIMEDOUT) {
                pthread_mutex_unlock(&queue->mutex);
                return OS_ERROR_TIMEOUT;
            }
        }
    }
    if (queue->closed && queue->count == 0) {
        pthread_mutex_unlock(&queue->mutex);
        return OS_ERROR;
    }
    /* Copy item without removing */
    char *src = queue->buffer + queue->head * queue->item_size;
    memcpy(item, src, queue->item_size);
    pthread_mutex_unlock(&queue->mutex);
    return OS_OK;
}

static size_t linux_queue_count(os_queue_t *queue) {
    if (!queue) return 0;
    size_t count;
    pthread_mutex_lock(&queue->mutex);
    count = queue->count;
    pthread_mutex_unlock(&queue->mutex);
    return count;
}

static size_t linux_queue_space(os_queue_t *queue) {
    if (!queue) return 0;
    size_t space;
    pthread_mutex_lock(&queue->mutex);
    space = queue->queue_depth - queue->count;
    pthread_mutex_unlock(&queue->mutex);
    return space;
}

static os_error_t linux_queue_destroy(os_queue_t *queue) {
    if (!queue) {
        return OS_ERROR_INVALID_PARAM;
    }
    pthread_mutex_lock(&queue->mutex);
    queue->closed = true;
    pthread_cond_broadcast(&queue->cond_not_empty);
    pthread_cond_broadcast(&queue->cond_not_full);
    pthread_mutex_unlock(&queue->mutex);

    pthread_cond_destroy(&queue->cond_not_full);
    pthread_cond_destroy(&queue->cond_not_empty);
    pthread_mutex_destroy(&queue->mutex);
    free(queue->buffer);
    free(queue);
    return OS_OK;
}

/* Timer */
static os_timer_t *linux_timer_create(os_timer_callback_t callback, void *arg, const char *name) {
    (void)name;
    os_timer_t *timer = malloc(sizeof(os_timer_t));
    if (!timer) return NULL;
    timer->callback = callback;
    timer->arg = arg;
    timer->repeat = false;
    /* timer_create not implemented */
    return timer;
}

static os_error_t linux_timer_start(os_timer_t *timer, uint32_t period_ms, bool repeat) {
    (void)timer; (void)period_ms; (void)repeat;
    return OS_ERROR_NOT_SUPPORTED;
}

static os_error_t linux_timer_stop(os_timer_t *timer) {
    (void)timer;
    return OS_ERROR_NOT_SUPPORTED;
}

static os_error_t linux_timer_reset(os_timer_t *timer) {
    (void)timer;
    return OS_ERROR_NOT_SUPPORTED;
}

static os_error_t linux_timer_destroy(os_timer_t *timer) {
    free(timer);
    return OS_OK;
}

/* System */
static uint32_t linux_get_tick_count(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint32_t)(ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

static uint32_t linux_get_tick_frequency(void) {
    return 1000; /* milliseconds */
}

static void linux_delay(uint32_t milliseconds) {
    usleep(milliseconds * 1000);
}

/* Initialization */
static os_error_t linux_abstract_init(void) {
    return OS_OK;
}

static os_error_t linux_abstract_deinit(void) {
    return OS_OK;
}

/* OS implementation structure */
const struct os_impl linux_impl = {
    .thread_create = linux_thread_create,
    .thread_join = linux_thread_join,
    .thread_detach = linux_thread_detach,
    .thread_sleep = linux_thread_sleep,
    .thread_self = linux_thread_self,
    .thread_get_name = linux_thread_get_name,
    .mutex_create = linux_mutex_create,
    .mutex_lock = linux_mutex_lock,
    .mutex_unlock = linux_mutex_unlock,
    .mutex_destroy = linux_mutex_destroy,
    .queue_create = linux_queue_create,
    .queue_send = linux_queue_send,
    .queue_receive = linux_queue_receive,
    .queue_peek = linux_queue_peek,
    .queue_count = linux_queue_count,
    .queue_space = linux_queue_space,
    .queue_destroy = linux_queue_destroy,
    .timer_create = linux_timer_create,
    .timer_start = linux_timer_start,
    .timer_stop = linux_timer_stop,
    .timer_reset = linux_timer_reset,
    .timer_destroy = linux_timer_destroy,
    .get_tick_count = linux_get_tick_count,
    .get_tick_frequency = linux_get_tick_frequency,
    .delay = linux_delay,
    .abstract_init = linux_abstract_init,
    .abstract_deinit = linux_abstract_deinit,
};
