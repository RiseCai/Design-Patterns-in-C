/**
 * data_stream_service.h  2026-01-05
 *
 * Copyright (C) 2000-2026 All Right Reserved
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Data Stream Service API
 * Manages data stream lifecycle, buffering, and synchronization
 * for parallel pipeline data flow operations.
 */

#ifndef __DATA_STREAM_SERVICE_H__
#define __DATA_STREAM_SERVICE_H__

#include "data_stream.h"

/* Data stream service */
struct data_stream_service *data_stream_service_create(void);
void data_stream_service_destroy(struct data_stream_service *service);
data_stream_t data_stream_service_create_stream(struct data_stream_service *service,
                                               const char *name,
                                               size_t element_size,
                                               int capacity);
void data_stream_service_destroy_stream(struct data_stream_service *service,
                                       data_stream_t stream);

/* Data stream service management */
struct data_stream_service *data_stream_service_create(void);
void data_stream_service_destroy(struct data_stream_service *service);
data_stream_t data_stream_service_create_stream(struct data_stream_service *service,
                                               const char *name,
                                               size_t element_size,
                                               int capacity);
void data_stream_service_destroy_stream(struct data_stream_service *service,
                                       data_stream_t stream);

#endif /* __DATA_STREAM_SERVICE_H__ */
