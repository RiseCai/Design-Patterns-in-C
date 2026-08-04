/**
 * connection.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include "connection.h"

/** constructor(). */
void connection_init(struct connection *connection, int id, const char *endpoint)
{
	_MY_TRACE_STR("connection_init()\n");
	memset(connection, 0, sizeof(*connection));
	connection->id = id;
	connection->in_use = 0;
	snprintf(connection->endpoint, sizeof(connection->endpoint), "%s",
		endpoint ? endpoint : "localhost");
}

void connection_open(struct connection *connection)
{
	_MY_TRACE_STR("connection::open()\n");
	connection->in_use = 1;
	printf("connection[%d] open -> %s\n", connection->id, connection->endpoint);
}

void connection_close(struct connection *connection)
{
	_MY_TRACE_STR("connection::close()\n");
	connection->in_use = 0;
	printf("connection[%d] close\n", connection->id);
}

void connection_execute(struct connection *connection, const char *sql)
{
	_MY_TRACE_STR("connection::execute()\n");
	printf("connection[%d] execute: %s\n", connection->id, sql ? sql : "");
}

int connection_is_in_use(struct connection *connection)
{
	return connection->in_use;
}
