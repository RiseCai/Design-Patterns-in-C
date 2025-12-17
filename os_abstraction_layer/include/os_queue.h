/**
 * os_queue.h - OS Abstraction Layer Queue Interface
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * This file defines the queue abstraction interface.
 */

#ifndef __OS_QUEUE_H__
#define __OS_QUEUE_H__

#include "os_abstract.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Additional queue-specific APIs */

/**
 * Reset queue (remove all items)
 * @param queue Queue handle
 * @return OS_OK on success, error code otherwise
 */
os_error_t os_queue_reset(os_queue_t *queue);

/**
 * Check if queue is empty
 * @param queue Queue handle
 * @return true if empty, false otherwise
 */
bool os_queue_is_empty(os_queue_t *queue);

/**
 * Check if queue is full
 * @param queue Queue handle
 * @return true if full, false otherwise
 */
bool os_queue_is_full(os_queue_t *queue);

/**
 * Get queue item size
 * @param queue Queue handle
 * @return Item size in bytes
 */
size_t os_queue_item_size(os_queue_t *queue);

/**
 * Get queue depth (maximum number of items)
 * @param queue Queue handle
 * @return Queue depth
 */
size_t os_queue_depth(os_queue_t *queue);

#ifdef __cplusplus
}
#endif

#endif /* __OS_QUEUE_H__ */
