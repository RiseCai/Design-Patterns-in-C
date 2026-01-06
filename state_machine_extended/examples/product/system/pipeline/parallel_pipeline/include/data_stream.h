/**
 * data_stream.h  2026-01-05
 * 
 * Copyright (C) 2000-2026 All Right Reserved
 * 
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Data stream service for parallel workflow extension.
 * 
 * Provides data flow capabilities for task-to-task communication.
 */

#ifndef __DATA_STREAM_H__
#define __DATA_STREAM_H__

#include "mycommon.h"
#include "mytrace.h"
#include "fsm_os_adapter.h"

/* Define TRACE_ERROR if not already defined */
#ifndef TRACE_ERROR
#define TRACE_ERROR(fmt, ...) _MY_TRACE_STR("[ERROR] " fmt "\n", ##__VA_ARGS__)
#endif

/* Timestamp type (using OS tick count) */
typedef uint32_t timestamp_t;

/* Helper to get current timestamp */
static inline timestamp_t os_get_timestamp(void) {
    return os_get_tick_count();
}

/* Data stream modes */
enum data_stream_mode {
    STREAM_MODE_FIFO,       /* FIFO queue */
    STREAM_MODE_LATEST,     /* Only keep latest element */
    STREAM_MODE_WINDOW,     /* Sliding window */
    STREAM_MODE_BROADCAST   /* Broadcast mode with reference counting */
};

/* Data stream configuration */
struct data_stream_config {
    const char *name;           /* Stream name */
    size_t element_size;        /* Size of each element in bytes */
    int capacity;               /* Buffer capacity (number of elements) */
    enum data_stream_mode mode; /* Stream mode */
    int window_size;            /* Window size (only for STREAM_MODE_WINDOW) */
};

/* Data stream metadata */
struct data_stream_metadata {
    int stream_id;                  /* Unique stream identifier */
    const char *name;               /* Stream name */
    int producer_task_id;           /* ID of task that produces to this stream */
    int consumer_count;             /* Number of tasks consuming from this stream */
    size_t total_bytes_transferred; /* Total bytes transferred */
    timestamp_t creation_time;      /* When the stream was created */
    timestamp_t last_activity;      /* Last read/write activity */
};

/* Data stream handle (opaque type) */
typedef struct data_stream *data_stream_t;

/* Data stream statistics */
struct data_stream_stats {
    /* Throughput statistics */
    size_t bytes_written;
    size_t bytes_read;
    int write_count;
    int read_count;
    
    /* Latency statistics */
    timestamp_t min_latency;
    timestamp_t max_latency;
    timestamp_t avg_latency;
    
    /* Queue statistics */
    int max_queue_depth;
    int current_queue_depth;
    int overflow_count;
    
    /* Error statistics */
    int timeout_errors;
    int buffer_overflow_errors;
    int corruption_errors;
};

/* Zero-copy data reference (for zero-copy operations) */
struct data_ref {
    const void *data;        /* Direct pointer to data (no copy) */
    size_t size;             /* Size of data */
    uint32_t ref_id;         /* Reference ID for tracking */
    timestamp_t timestamp;   /* When this reference was created */
};

/* Zero-copy read result */
struct data_read_result {
    struct data_ref ref;     /* Data reference */
    int status;              /* Status code */
};

/* Core API */
data_stream_t data_stream_create(const struct data_stream_config *config);
int data_stream_destroy(data_stream_t stream);

int data_stream_write(data_stream_t stream,
                     const void *data,
                     size_t size,
                     int timeout_ms);

int data_stream_read(data_stream_t stream,
                    void *buffer,
                    size_t buffer_size,
                    int timeout_ms);

/* Zero-copy API (experimental) */
int data_stream_write_zero_copy(data_stream_t stream,
                               const void *data,
                               size_t size,
                               int timeout_ms);

int data_stream_read_zero_copy(data_stream_t stream,
                              struct data_read_result *result,
                              int timeout_ms);

int data_stream_release_reference(data_stream_t stream,
                                 uint32_t ref_id);

int data_stream_peek(data_stream_t stream, 
                    void *buffer,
                    size_t buffer_size);

int data_stream_has_data(data_stream_t stream);
int data_stream_is_full(data_stream_t stream);

/* Metadata and statistics */
int data_stream_get_metadata(data_stream_t stream, 
                            struct data_stream_metadata *metadata);

int data_stream_get_stats(data_stream_t stream,
                         struct data_stream_stats *stats);

void data_stream_reset_stats(data_stream_t stream);

/* Stream management */
int data_stream_set_producer(data_stream_t stream, int task_id);
int data_stream_add_consumer(data_stream_t stream, int task_id);
int data_stream_remove_consumer(data_stream_t stream, int task_id);

/* Utility functions */
int data_stream_get_element_size(data_stream_t stream);
int data_stream_get_capacity(data_stream_t stream);
int data_stream_get_available(data_stream_t stream);
const char *data_stream_get_name(data_stream_t stream);

/* Error codes */
#define DATA_STREAM_SUCCESS 0
#define DATA_STREAM_ERROR_INVALID_PARAM -1
#define DATA_STREAM_ERROR_TIMEOUT -2
#define DATA_STREAM_ERROR_BUFFER_FULL -3
#define DATA_STREAM_ERROR_BUFFER_EMPTY -4
#define DATA_STREAM_ERROR_SIZE_MISMATCH -5
#define DATA_STREAM_ERROR_NOT_INITIALIZED -6
#define DATA_STREAM_ERROR_ALREADY_CLOSED -7

#endif /* __DATA_STREAM_H__ */
