/**
 * connection.h
 *
 * Pooled resource: a reusable DB/network style connection.
 */
#ifndef __CONNECTION_H__
#define __CONNECTION_H__

#include <mycommon.h>
#include <mytrace.h>

struct connection {
	int id;
	int in_use;
	char endpoint[64];
};

/** constructor(). */
void connection_init(struct connection *connection, int id, const char *endpoint);

void connection_open(struct connection *connection);
void connection_close(struct connection *connection);
void connection_execute(struct connection *connection, const char *sql);
int connection_is_in_use(struct connection *connection);

#endif /* __CONNECTION_H__ */
