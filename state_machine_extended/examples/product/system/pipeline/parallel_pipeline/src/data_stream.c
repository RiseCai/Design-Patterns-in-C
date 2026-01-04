/**
 * data_stream.c  2026-01-05
 * 
 * Copyright (C) 2000-2026 All Right Reserved
 * 
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Data stream service implementation.
 */

#include "data_stream.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Macro to suppress unused parameter warnings */
#ifndef UNUSED
#if defined(__GNUC__) || defined(__clang__)
#define UNUSED(x) (void)(x)
#else
#define UNUSED(x) (void)(x)
#endif
#endif

/* Internal data stream structure */
struct data_stream {
    /* Configuration */
    char name[64];
    size_t element_size;
    int capacity;
    enum data_stream_mode mode;
    int window_size;
    
    /* Buffer management */
    void *buffer;               /* Circular buffer */
    int head;                   /* Read position */
    int tail;                   /* Write position */
    int count;                  /* Number of elements in buffer */
    
    /* Synchronization */
    os_mutex_t *mutex;
    os_semaphore_t *data_available;  /* Semaphore for readers */
    os_semaphore_t *space_available; /* Semaphore for writers */
    
    /* Metadata */
    int stream_id;
    int producer_task_id;
    int consumer_count;
    size_t total_bytes_transferred;
    timestamp_t creation_time;
    timestamp_t last_activity;
    
    /* Statistics */
    struct data_stream_stats stats;
    
    /* State */
    bool initialized;
    bool closed;
};

/* Stream ID counter */
static int next_stream_id = 1;
static os_mutex_t *id_mutex = NULL;

/* Internal helper functions */
static int ensure_id_mutex_initialized(void);
static void update_latency_stat(struct data_stream_stats *stats, timestamp_t latency);
static int calculate_available_space(struct data_stream *stream);
static int calculate_available_data(struct data_stream *stream);
static void copy_element(struct data_stream *stream, void *dest, const void *src);

/* Initialize ID mutex once */
static int ensure_id_mutex_initialized(void)
{
    if (!id_mutex) {
        id_mutex = os_mutex_create();
        if (!id_mutex) {
            TRACE_ERROR("Failed to create ID mutex for data streams");
            return -1;
        }
    }
    return 0;
}

/* Create a new data stream */
data_stream_t data_stream_create(const struct data_stream_config *config)
{
    if (!config || config->element_size == 0 || config->capacity <= 0) {
        TRACE_ERROR("Invalid data stream configuration");
        return NULL;
    }
    
    if (ensure_id_mutex_initialized() != 0) {
        return NULL;
    }
    
    /* Allocate stream structure */
    struct data_stream *stream = (struct data_stream *)malloc(sizeof(struct data_stream));
    if (!stream) {
        TRACE_ERROR("Failed to allocate data stream structure");
        return NULL;
    }
    
    memset(stream, 0, sizeof(struct data_stream));
    
    /* Copy configuration */
    if (config->name) {
        strncpy(stream->name, config->name, sizeof(stream->name) - 1);
        stream->name[sizeof(stream->name) - 1] = '\0';
    } else {
        strcpy(stream->name, "unnamed_stream");
    }
    
    stream->element_size = config->element_size;
    stream->capacity = config->capacity;
    stream->mode = config->mode;
    stream->window_size = config->window_size;
    
    /* Validate window size for window mode */
    if (stream->mode == STREAM_MODE_WINDOW) {
        if (stream->window_size <= 0 || stream->window_size > stream->capacity) {
            TRACE_ERROR("Invalid window size %d for capacity %d", 
                       stream->window_size, stream->capacity);
            free(stream);
            return NULL;
        }
    }
    
    /* Allocate buffer */
    stream->buffer = malloc(stream->element_size * stream->capacity);
    if (!stream->buffer) {
        TRACE_ERROR("Failed to allocate buffer for data stream");
        free(stream);
        return NULL;
    }
    
    /* Initialize buffer indices */
    stream->head = 0;
    stream->tail = 0;
    stream->count = 0;
    
    /* Create synchronization primitives */
    stream->mutex = os_mutex_create();
    stream->data_available = os_semaphore_create(0, stream->capacity);
    stream->space_available = os_semaphore_create(stream->capacity, stream->capacity);
    
    if (!stream->mutex || !stream->data_available || !stream->space_available) {
        TRACE_ERROR("Failed to create synchronization primitives for data stream");
        if (stream->mutex) os_mutex_destroy(stream->mutex);
        if (stream->data_available) os_semaphore_destroy(stream->data_available);
        if (stream->space_available) os_semaphore_destroy(stream->space_available);
        free(stream->buffer);
        free(stream);
        return NULL;
    }
    
    /* Assign unique stream ID */
    os_mutex_lock(id_mutex, OS_WAIT_FOREVER);
    stream->stream_id = next_stream_id++;
    os_mutex_unlock(id_mutex);
    
    /* Initialize metadata */
    stream->producer_task_id = -1;  /* No producer yet */
    stream->consumer_count = 0;
    stream->total_bytes_transferred = 0;
    stream->creation_time = os_get_timestamp();
    stream->last_activity = stream->creation_time;
    
    /* Initialize statistics */
    memset(&stream->stats, 0, sizeof(struct data_stream_stats));
    stream->stats.min_latency = (timestamp_t)-1;  /* Max value */
    
    /* Set state */
    stream->initialized = true;
    stream->closed = false;
    
    TRACE_INFO("Data stream '%s' created (id=%d, element_size=%zu, capacity=%d)",
               stream->name, stream->stream_id, stream->element_size, stream->capacity);
    
    return (data_stream_t)stream;
}

/* Destroy a data stream */
int data_stream_destroy(data_stream_t stream_handle)
{
    if (!stream_handle) {
        return DATA_STREAM_ERROR_INVALID_PARAM;
    }
    
    struct data_stream *stream = (struct data_stream *)stream_handle;
    
    os_mutex_lock(stream->mutex, OS_WAIT_FOREVER);
    
    if (stream->closed) {
        os_mutex_unlock(stream->mutex);
        return DATA_STREAM_ERROR_ALREADY_CLOSED;
    }
    
    stream->closed = true;
    
    /* Release any waiting threads */
    os_semaphore_give(stream->data_available);
    os_semaphore_give(stream->space_available);
    
    os_mutex_unlock(stream->mutex);
    
    /* Destroy synchronization primitives */
    if (stream->mutex) {
        os_mutex_destroy(stream->mutex);
    }
    
    if (stream->data_available) {
        os_semaphore_destroy(stream->data_available);
    }
    
    if (stream->space_available) {
        os_semaphore_destroy(stream->space_available);
    }
    
    /* Free buffer */
    if (stream->buffer) {
        free(stream->buffer);
    }
    
    /* Free stream structure */
    free(stream);
    
    TRACE_INFO("Data stream destroyed");
    return DATA_STREAM_SUCCESS;
}

/* Write data to stream */
int data_stream_write(data_stream_t stream_handle, 
                     const void *data, 
                     size_t size,
                     int timeout_ms)
{
    if (!stream_handle || !data || size == 0) {
        return DATA_STREAM_ERROR_INVALID_PARAM;
    }
    
    struct data_stream *stream = (struct data_stream *)stream_handle;
    
    /* Check if stream is closed */
    if (stream->closed) {
        return DATA_STREAM_ERROR_ALREADY_CLOSED;
    }
    
    /* Check size matches element size */
    if (size != stream->element_size) {
        TRACE_ERROR("Size mismatch: expected %zu, got %zu", stream->element_size, size);
        return DATA_STREAM_ERROR_SIZE_MISMATCH;
    }
    
    /* Wait for space available */
    if (os_semaphore_take(stream->space_available, timeout_ms) != OS_OK) {
        stream->stats.timeout_errors++;
        return DATA_STREAM_ERROR_TIMEOUT;
    }
    
    os_mutex_lock(stream->mutex, OS_WAIT_FOREVER);
    
    /* Handle different stream modes */
    switch (stream->mode) {
        case STREAM_MODE_FIFO:
            /* Standard FIFO behavior */
            break;
            
        case STREAM_MODE_LATEST:
            /* If buffer is full, discard oldest element */
            if (stream->count == stream->capacity) {
                stream->head = (stream->head + 1) % stream->capacity;
                stream->count--;
                /* Give back space semaphore since we're reusing a slot */
                os_semaphore_give(stream->space_available);
            }
            break;
            
        case STREAM_MODE_WINDOW:
            /* Maintain sliding window */
            if (stream->count == stream->window_size) {
                stream->head = (stream->head + 1) % stream->capacity;
                stream->count--;
                /* Give back space semaphore since we're reusing a slot */
                os_semaphore_give(stream->space_available);
            }
            break;
    }
    
    /* Copy data to buffer */
    void *dest = (char *)stream->buffer + (stream->tail * stream->element_size);
    memcpy(dest, data, size);
    
    /* Update buffer indices */
    stream->tail = (stream->tail + 1) % stream->capacity;
    stream->count++;
    
    /* Update statistics */
    stream->total_bytes_transferred += size;
    stream->stats.bytes_written += size;
    stream->stats.write_count++;
    stream->stats.current_queue_depth = stream->count;
    
    if (stream->count > stream->stats.max_queue_depth) {
        stream->stats.max_queue_depth = stream->count;
    }
    
    /* Update last activity timestamp */
    stream->last_activity = os_get_timestamp();
    
    os_mutex_unlock(stream->mutex);
    
    /* Signal data available */
    os_semaphore_give(stream->data_available);
    
    return DATA_STREAM_SUCCESS;
}

/* Read data from stream */
int data_stream_read(data_stream_t stream_handle,
                    void *buffer,
                    size_t buffer_size,
                    int timeout_ms)
{
    if (!stream_handle || !buffer || buffer_size == 0) {
        return DATA_STREAM_ERROR_INVALID_PARAM;
    }
    
    struct data_stream *stream = (struct data_stream *)stream_handle;
    
    /* Check if stream is closed */
    if (stream->closed) {
        return DATA_STREAM_ERROR_ALREADY_CLOSED;
    }
    
    /* Check buffer size */
    if (buffer_size < stream->element_size) {
        TRACE_ERROR("Buffer too small: need %zu, got %zu", 
                   stream->element_size, buffer_size);
        return DATA_STREAM_ERROR_SIZE_MISMATCH;
    }
    
    /* Wait for data available */
    timestamp_t start_time = os_get_timestamp();
    if (os_semaphore_take(stream->data_available, timeout_ms) != OS_OK) {
        stream->stats.timeout_errors++;
        return DATA_STREAM_ERROR_TIMEOUT;
    }
    
    os_mutex_lock(stream->mutex, OS_WAIT_FOREVER);
    
    /* Check if there's actually data (should be, but just in case) */
    if (stream->count == 0) {
        os_mutex_unlock(stream->mutex);
        /* Give back the semaphore since we didn't actually consume data */
        os_semaphore_give(stream->data_available);
        return DATA_STREAM_ERROR_BUFFER_EMPTY;
    }
    
    /* Copy data from buffer */
    const void *src = (char *)stream->buffer + (stream->head * stream->element_size);
    memcpy(buffer, src, stream->element_size);
    
    /* Update buffer indices */
    stream->head = (stream->head + 1) % stream->capacity;
    stream->count--;
    
    /* Update statistics */
    stream->total_bytes_transferred += stream->element_size;
    stream->stats.bytes_read += stream->element_size;
    stream->stats.read_count++;
    stream->stats.current_queue_depth = stream->count;
    
    /* Calculate latency */
    timestamp_t end_time = os_get_timestamp();
    timestamp_t latency = end_time - start_time;
    update_latency_stat(&stream->stats, latency);
    
    /* Update last activity timestamp */
    stream->last_activity = os_get_timestamp();
    
    os_mutex_unlock(stream->mutex);
    
    /* Signal space available */
    os_semaphore_give(stream->space_available);
    
    return DATA_STREAM_SUCCESS;
}

/* Peek at data without removing it */
int data_stream_peek(data_stream_t stream_handle, 
                    void *buffer,
                    size_t buffer_size)
{
    if (!stream_handle || !buffer || buffer_size == 0) {
        return DATA_STREAM_ERROR_INVALID_PARAM;
    }
    
    struct data_stream *stream = (struct data_stream *)stream_handle;
    
    os_mutex_lock(stream->mutex, OS_WAIT_FOREVER);
    
    if (stream->closed) {
        os_mutex_unlock(stream->mutex);
        return DATA_STREAM_ERROR_ALREADY_CLOSED;
    }
    
    if (stream->count == 0) {
        os_mutex_unlock(stream->mutex);
        return DATA_STREAM_ERROR_BUFFER_EMPTY;
    }
    
    if (buffer_size < stream->element_size) {
        os_mutex_unlock(stream->mutex);
        return DATA_STREAM_ERROR_SIZE_MISMATCH;
    }
    
    /* Copy data without updating indices */
    const void *src = (char *)stream->buffer + (stream->head * stream->element_size);
    memcpy(buffer, src, stream->element_size);
    
    os_mutex_unlock(stream->mutex);
    
    return DATA_STREAM_SUCCESS;
}

/* Check if stream has data */
int data_stream_has_data(data_stream_t stream_handle)
{
    if (!stream_handle) {
        return 0;
    }
    
    struct data_stream *stream = (struct data_stream *)stream_handle;
    
    os_mutex_lock(stream->mutex, OS_WAIT_FOREVER);
    int has_data = (stream->count > 0) && !stream->closed;
    os_mutex_unlock(stream->mutex);
    
    return has_data;
}

/* Check if stream is full */
int data_stream_is_full(data_stream_t stream_handle)
{
    if (!stream_handle) {
        return 0;
    }
    
    struct data_stream *stream = (struct data_stream *)stream_handle;
    
    os_mutex_lock(stream->mutex, OS_WAIT_FOREVER);
    int is_full;
    
    switch (stream->mode) {
        case STREAM_MODE_FIFO:
            is_full = (stream->count == stream->capacity);
            break;
        case STREAM_MODE_LATEST:
            /* Latest mode never reports full since it discards old data */
            is_full = 0;
            break;
        case STREAM_MODE_WINDOW:
            is_full = (stream->count == stream->window_size);
            break;
        default:
            is_full = 0;
            break;
    }
    
    os_mutex_unlock(stream->mutex);
    
    return is_full;
}

/* Get stream metadata */
int data_stream_get_metadata(data_stream_t stream_handle, 
                            struct data_stream_metadata *metadata)
{
    if (!stream_handle || !metadata) {
        return DATA_STREAM_ERROR_INVALID_PARAM;
    }
    
    struct data_stream *stream = (struct data_stream *)stream_handle;
    
    os_mutex_lock(stream->mutex, OS_WAIT_FOREVER);
    
    metadata->stream_id = stream->stream_id;
    metadata->name = stream->name;
    metadata->producer_task_id = stream->producer_task_id;
    metadata->consumer_count = stream->consumer_count;
    metadata->total_bytes_transferred = stream->total_bytes_transferred;
    metadata->creation_time = stream->creation_time;
    metadata->last_activity = stream->last_activity;
    
    os_mutex_unlock(stream->mutex);
    
    return DATA_STREAM_SUCCESS;
}

/* Get stream statistics */
int data_stream_get_stats(data_stream_t stream_handle,
                         struct data_stream_stats *stats)
{
    if (!stream_handle || !stats) {
        return DATA_STREAM_ERROR_INVALID_PARAM;
    }
    
    struct data_stream *stream = (struct data_stream *)stream_handle;
    
    os_mutex_lock(stream->mutex, OS_WAIT_FOREVER);
    memcpy(stats, &stream->stats, sizeof(struct data_stream_stats));
    os_mutex_unlock(stream->mutex);
    
    return DATA_STREAM_SUCCESS;
}

/* Reset stream statistics */
void data_stream_reset_stats(data_stream_t stream_handle)
{
    if (!stream_handle) {
        return;
    }
    
    struct data_stream *stream = (struct data_stream *)stream_handle;
    
    os_mutex_lock(stream->mutex, OS_WAIT_FOREVER);
    
    memset(&stream->stats, 0, sizeof(struct data_stream_stats));
    stream->stats.min_latency = (timestamp_t)-1;
    stream->stats.current_queue_depth = stream->count;
    
    os_mutex_unlock(stream->mutex);
}

/* Set producer task ID */
int data_stream_set_producer(data_stream_t stream_handle, int task_id)
{
    if (!stream_handle) {
        return DATA_STREAM_ERROR_INVALID_PARAM;
    }
    
    struct data_stream *stream = (struct data_stream *)stream_handle;
    
    os_mutex_lock(stream->mutex, OS_WAIT_FOREVER);
    stream->producer_task_id = task_id;
    os_mutex_unlock(stream->mutex);
    
    return DATA_STREAM_SUCCESS;
}

/* Add consumer task ID */
int data_stream_add_consumer(data_stream_t stream_handle, int task_id)
{
    if (!stream_handle) {
        return DATA_STREAM_ERROR_INVALID_PARAM;
    }
    
    struct data_stream *stream = (struct data_stream *)stream_handle;
    
    os_mutex_lock(stream->mutex, OS_WAIT_FOREVER);
    stream->consumer_count++;
    os_mutex_unlock(stream->mutex);
    
    return DATA_STREAM_SUCCESS;
}

/* Remove consumer task ID */
int data_stream_remove_consumer(data_stream_t stream_handle, int task_id)
{
    UNUSED(task_id);
    
    if (!stream_handle) {
        return DATA_STREAM_ERROR_INVALID_PARAM;
    }
    
    struct data_stream *stream = (struct data_stream *)stream_handle;
    
    os_mutex_lock(stream->mutex, OS_WAIT_FOREVER);
    if (stream->consumer_count > 0) {
        stream->consumer_count--;
    }
    os_mutex_unlock(stream->mutex);
    
    return DATA_STREAM_SUCCESS;
}

/* Get element size */
int data_stream_get_element_size(data_stream_t stream_handle)
{
    if (!stream_handle) {
        return 0;
    }
    
    struct data_stream *stream = (struct data_stream *)stream_handle;
    return (int)stream->element_size;
}

/* Get capacity */
int data_stream_get_capacity(data_stream_t stream_handle)
{
    if (!stream_handle) {
        return 0;
    }
    
    struct data_stream *stream = (struct data_stream *)stream_handle;
    return stream->capacity;
}

/* Get available space */
int data_stream_get_available(data_stream_t stream_handle)
{
    if (!stream_handle) {
        return 0;
    }
    
    struct data_stream *stream = (struct data_stream *)stream_handle;
    
    os_mutex_lock(stream->mutex, OS_WAIT_FOREVER);
    int available;
    
    switch (stream->mode) {
        case STREAM_MODE_FIFO:
            available = stream->capacity - stream->count;
            break;
        case STREAM_MODE_LATEST:
            /* Latest mode always has at least 1 slot available */
            available = 1;
            break;
        case STREAM_MODE_WINDOW:
            available = stream->window_size - stream->count;
            if (available < 0) available = 0;
            break;
        default:
            available = 0;
            break;
    }
    
    os_mutex_unlock(stream->mutex);
    return available;
}

/* Get stream name */
const char *data_stream_get_name(data_stream_t stream_handle)
{
    if (!stream_handle) {
        return NULL;
    }
    
    struct data_stream *stream = (struct data_stream *)stream_handle;
    return stream->name;
}

/* Helper: Update latency statistics */
static void update_latency_stat(struct data_stream_stats *stats, timestamp_t latency)
{
    if (latency < stats->min_latency) {
        stats->min_latency = latency;
    }
    
    if (latency > stats->max_latency) {
        stats->max_latency = latency;
    }
    
    /* Simple moving average */
    if (stats->read_count == 1) {
        stats->avg_latency = latency;
    } else {
        stats->avg_latency = (stats->avg_latency * (stats->read_count - 1) + latency) / stats->read_count;
    }
}

/* Helper: Calculate available space */
static int calculate_available_space(struct data_stream *stream)
{
    switch (stream->mode) {
        case STREAM_MODE_FIFO:
            return stream->capacity - stream->count;
        case STREAM_MODE_LATEST:
            return 1; /* Always can write (discards old) */
        case STREAM_MODE_WINDOW:
            return stream->window_size - stream->count;
        default:
            return 0;
    }
}

/* Helper: Calculate available data */
static int calculate_available_data(struct data_stream *stream)
{
    return stream->count;
}

/* Helper: Copy element */
static void copy_element(struct data_stream *stream, void *dest, const void *src)
{
    memcpy(dest, src, stream->element_size);
}
