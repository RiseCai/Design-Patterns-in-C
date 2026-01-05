/**
 * data_stream_service.c  2026-01-05
 *
 * Copyright (C) 2000-2026 All Right Reserved
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Data Stream Service Implementation
 * Manages data stream lifecycle, buffering, and synchronization.
 */

#include "data_stream_service.h"
#include <stdlib.h>
#include <string.h>

/* Data stream service */

struct data_stream_service {
    int stream_count;
    data_stream_t *streams;
    os_mutex_t *mutex;
};

struct data_stream_service *data_stream_service_create(void)
{
    struct data_stream_service *service = (struct data_stream_service *)malloc(sizeof(struct data_stream_service));
    if (!service) return NULL;

    memset(service, 0, sizeof(struct data_stream_service));
    service->mutex = os_mutex_create();
    if (!service->mutex) {
        free(service);
        return NULL;
    }

    TRACE_INFO("Data stream service created");
    return service;
}

void data_stream_service_destroy(struct data_stream_service *service)
{
    if (!service) return;

    /* Destroy all streams */
    if (service->streams) {
        for (int i = 0; i < service->stream_count; i++) {
            if (service->streams[i]) {
                data_stream_destroy(service->streams[i]);
            }
        }
        free(service->streams);
    }

    if (service->mutex) {
        os_mutex_destroy(service->mutex);
    }

    free(service);
    TRACE_INFO("Data stream service destroyed");
}

data_stream_t data_stream_service_create_stream(struct data_stream_service *service,
                                               const char *name,
                                               size_t element_size,
                                               int capacity)
{
    if (!service || !name) return NULL;

    struct data_stream_config config = {
        .name = name,
        .element_size = element_size,
        .capacity = capacity,
        .mode = STREAM_MODE_FIFO,
        .window_size = 0
    };

    data_stream_t stream = data_stream_create(&config);
    if (!stream) return NULL;

    os_mutex_lock(service->mutex, OS_WAIT_FOREVER);

    /* Add to service's stream list */
    service->stream_count++;
    service->streams = (data_stream_t *)realloc(service->streams,
                                                service->stream_count * sizeof(data_stream_t));
    if (!service->streams) {
        service->stream_count = 0;
        os_mutex_unlock(service->mutex);
        data_stream_destroy(stream);
        return NULL;
    }

    service->streams[service->stream_count - 1] = stream;
    os_mutex_unlock(service->mutex);

    TRACE_INFO("Data stream '%s' created via service", name);
    return stream;
}

void data_stream_service_destroy_stream(struct data_stream_service *service,
                                       data_stream_t stream)
{
    if (!service || !stream) return;

    os_mutex_lock(service->mutex, OS_WAIT_FOREVER);

    /* Find and remove stream from list */
    for (int i = 0; i < service->stream_count; i++) {
        if (service->streams[i] == stream) {
            /* Shift remaining streams */
            for (int j = i; j < service->stream_count - 1; j++) {
                service->streams[j] = service->streams[j + 1];
            }
            service->stream_count--;
            service->streams = (data_stream_t *)realloc(service->streams,
                                                        service->stream_count * sizeof(data_stream_t));
            break;
        }
    }

    os_mutex_unlock(service->mutex);

    /* Destroy the stream */
    data_stream_destroy(stream);
    TRACE_INFO("Data stream destroyed via service");
}
