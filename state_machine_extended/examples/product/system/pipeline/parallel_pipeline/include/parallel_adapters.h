/**
 * parallel_adapters.h  2026-01-05
 *
 * Copyright (C) 2000-2026 All Right Reserved
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Parallel Adapters Framework API
 * Provides integration with external systems including MQ, REST APIs,
 * databases, and file systems for parallel pipeline workflows.
 */

#ifndef __PARALLEL_ADAPTERS_H__
#define __PARALLEL_ADAPTERS_H__

#include "mycommon.h"
#include <stddef.h>

/* ============================================================================
 * FRAMEWORK CORE
 * ============================================================================ */

/**
 * Adapter types supported by the framework
 */
enum parallel_adapter_type {
    ADAPTER_TYPE_MQ,           /* Message Queue */
    ADAPTER_TYPE_REST,         /* REST API */
    ADAPTER_TYPE_DATABASE,     /* Database */
    ADAPTER_TYPE_FILESYSTEM,   /* File System */
    ADAPTER_TYPE_CUSTOM,       /* Custom Protocol */
    ADAPTER_TYPE_MAX
};

/**
 * Adapter configuration base structure
 */
struct parallel_adapter_config {
    enum parallel_adapter_type type;
    const char *name;                    /* Adapter instance name */
    int enabled;                        /* Enable/disable flag */
    int retry_count;                    /* Max retry attempts */
    int retry_delay_ms;                 /* Delay between retries */
    int timeout_ms;                     /* Operation timeout */
    void *user_data;                    /* User-specific data */
};

/* ============================================================================
 * MESSAGE QUEUE ADAPTERS
 * ============================================================================ */

/**
 * MQ adapter configuration
 */
struct parallel_mq_config {
    struct parallel_adapter_config base;
    const char *host;
    int port;
    const char *username;
    const char *password;
    const char *exchange;
    const char *routing_key;
    int heartbeat_interval;
    int prefetch_count;
};

/**
 * MQ message structure
 */
struct parallel_mq_message {
    const char *exchange;
    const char *routing_key;
    const void *body;
    size_t body_size;
    int priority;
    int delivery_mode;
    int expiration_ms;
    const char *content_type;
    const char *correlation_id;
    const char *reply_to;
    void *user_data;
};

/**
 * MQ adapter interface
 */
typedef struct parallel_mq_adapter {
    int (*configure)(struct parallel_mq_adapter *adapter,
                    const struct parallel_mq_config *config);
    int (*connect)(struct parallel_mq_adapter *adapter);
    int (*disconnect)(struct parallel_mq_adapter *adapter);
    int (*is_connected)(struct parallel_mq_adapter *adapter);
    int (*publish)(struct parallel_mq_adapter *adapter,
                  const struct parallel_mq_message *message);
    int (*consume)(struct parallel_mq_adapter *adapter,
                  const char *queue,
                  int (*callback)(const struct parallel_mq_message *message, void *user_data),
                  void *user_data);
    int (*declare_queue)(struct parallel_mq_adapter *adapter,
                        const char *queue_name,
                        int durable,
                        int auto_delete);
    int (*bind_queue)(struct parallel_mq_adapter *adapter,
                     const char *queue,
                     const char *exchange,
                     const char *routing_key);
    void (*destroy)(struct parallel_mq_adapter *adapter);
} parallel_mq_adapter_t;

/* ============================================================================
 * REST API ADAPTERS
 * ============================================================================ */

/**
 * HTTP methods
 */
enum parallel_http_method {
    HTTP_METHOD_GET,
    HTTP_METHOD_POST,
    HTTP_METHOD_PUT,
    HTTP_METHOD_DELETE,
    HTTP_METHOD_PATCH,
    HTTP_METHOD_HEAD,
    HTTP_METHOD_OPTIONS
};

/**
 * REST adapter configuration
 */
struct parallel_rest_config {
    struct parallel_adapter_config base;
    const char *base_url;
    const char *api_key;
    const char *auth_token;
    int max_connections;
    int keep_alive_timeout;
    const char *user_agent;
    const char *content_type;
};

/**
 * REST request structure
 */
struct parallel_rest_request {
    enum parallel_http_method method;
    const char *path;
    const char *query_string;
    const void *body;
    size_t body_size;
    const char *content_type;
    const char **headers;
    int header_count;
    int timeout_ms;
};

/**
 * REST response structure
 */
struct parallel_rest_response {
    int status_code;
    const void *body;
    size_t body_size;
    const char *content_type;
    const char **headers;
    int header_count;
    long response_time_ms;
};

/**
 * REST adapter interface
 */
typedef struct parallel_rest_adapter {
    int (*configure)(struct parallel_rest_adapter *adapter,
                    const struct parallel_rest_config *config);
    int (*execute)(struct parallel_rest_adapter *adapter,
                  const struct parallel_rest_request *request,
                  struct parallel_rest_response *response);
    int (*execute_async)(struct parallel_rest_adapter *adapter,
                        const struct parallel_rest_request *request,
                        void (*callback)(const struct parallel_rest_response *response, void *user_data),
                        void *user_data);
    int (*ping)(struct parallel_rest_adapter *adapter);
    void (*destroy)(struct parallel_rest_adapter *adapter);
} parallel_rest_adapter_t;

/* ============================================================================
 * DATABASE ADAPTERS
 * ============================================================================ */

/**
 * Database types
 */
enum parallel_db_type {
    DB_TYPE_SQLITE,
    DB_TYPE_POSTGRESQL,
    DB_TYPE_MYSQL,
    DB_TYPE_MONGODB,
    DB_TYPE_REDIS,
    DB_TYPE_CUSTOM
};

/**
 * Database adapter configuration
 */
struct parallel_db_config {
    struct parallel_adapter_config base;
    enum parallel_db_type db_type;
    const char *host;
    int port;
    const char *database;
    const char *username;
    const char *password;
    const char *connection_string;
    int max_connections;
    int connection_timeout;
    int query_timeout;
};

/**
 * Database adapter interface
 */
typedef struct parallel_db_adapter {
    int (*configure)(struct parallel_db_adapter *adapter,
                    const struct parallel_db_config *config);
    int (*connect)(struct parallel_db_adapter *adapter);
    int (*disconnect)(struct parallel_db_adapter *adapter);
    int (*is_connected)(struct parallel_db_adapter *adapter);
    void (*destroy)(struct parallel_db_adapter *adapter);
} parallel_db_adapter_t;

/* ============================================================================
 * FILESYSTEM ADAPTERS
 * ============================================================================ */

/**
 * Filesystem adapter configuration
 */
struct parallel_fs_config {
    struct parallel_adapter_config base;
    const char *root_path;
    int create_dirs;
    int max_file_size;
    int buffer_size;
    const char *temp_dir;
};

/**
 * Filesystem adapter interface
 */
typedef struct parallel_fs_adapter {
    int (*configure)(struct parallel_fs_adapter *adapter,
                    const struct parallel_fs_config *config);
    void (*destroy)(struct parallel_fs_adapter *adapter);
} parallel_fs_adapter_t;

/* ============================================================================
 * CONVENIENCE FUNCTIONS (BACKWARD COMPATIBILITY)
 * ============================================================================ */

/* Legacy MQ adapter functions */
typedef void (*parallel_mq_adapter_send)(const char *queue, const char *message);
void parallel_mq_send_task_status(const char *task_id, const char *status);

/* Legacy REST adapter functions */
typedef void (*parallel_rest_adapter_post)(const char *url, const char *json);
void parallel_rest_post_task_result(const char *url, const char *task_id, void *result);

#endif /* __PARALLEL_ADAPTERS_H__ */
