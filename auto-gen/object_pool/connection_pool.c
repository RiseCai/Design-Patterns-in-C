/**
 * connection_pool.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include "connection_pool.h"

/** constructor(). */
void connection_pool_init(struct connection_pool *pool, int capacity,
	const char *endpoint)
{
	int i;

	_MY_TRACE_STR("connection_pool_init()\n");
	memset(pool, 0, sizeof(*pool));
	if (capacity < 1)
		capacity = 1;
	if (capacity > CONNECTION_POOL_MAX)
		capacity = CONNECTION_POOL_MAX;
	pool->capacity = capacity;
	pool->created = capacity;

	for (i = 0; i < capacity; ++i)
		connection_init(&pool->slots[i], i, endpoint);
}

struct connection *connection_pool_acquire(struct connection_pool *pool)
{
	int i;

	_MY_TRACE_STR("connection_pool::acquire()\n");
	for (i = 0; i < pool->capacity; ++i) {
		if (!connection_is_in_use(&pool->slots[i])) {
			connection_open(&pool->slots[i]);
			return &pool->slots[i];
		}
	}
	printf("connection_pool: no free connection\n");
	return 0;
}

void connection_pool_release(struct connection_pool *pool, struct connection *conn)
{
	_MY_TRACE_STR("connection_pool::release()\n");
	if (!conn)
		return;
	if (conn < pool->slots || conn >= pool->slots + pool->capacity) {
		printf("connection_pool: foreign connection ignored\n");
		return;
	}
	connection_close(conn);
}

int connection_pool_available(struct connection_pool *pool)
{
	int i, n = 0;

	for (i = 0; i < pool->capacity; ++i) {
		if (!connection_is_in_use(&pool->slots[i]))
			++n;
	}
	return n;
}

int connection_pool_in_use(struct connection_pool *pool)
{
	return pool->capacity - connection_pool_available(pool);
}
