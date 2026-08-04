/**
 * connection_pool.h
 *
 * Object Pool (connection-pool style):
 *   Manage a fixed set of reusable Connection instances.
 *   Clients acquire a connection, use it, then release it back to the pool.
 */
#ifndef __CONNECTION_POOL_H__
#define __CONNECTION_POOL_H__

#include <mycommon.h>
#include <mytrace.h>

#include "connection.h"

#define CONNECTION_POOL_MAX 8

struct connection_pool {
	struct connection slots[CONNECTION_POOL_MAX];
	int capacity;
	int created;
};

/** constructor(): create up to 'capacity' idle connections. */
void connection_pool_init(struct connection_pool *pool, int capacity,
	const char *endpoint);

/** Borrow an idle connection; returns NULL if none available. */
struct connection *connection_pool_acquire(struct connection_pool *pool);

/** Return a connection to the pool for reuse. */
void connection_pool_release(struct connection_pool *pool, struct connection *conn);

int connection_pool_available(struct connection_pool *pool);
int connection_pool_in_use(struct connection_pool *pool);

#endif /* __CONNECTION_POOL_H__ */
