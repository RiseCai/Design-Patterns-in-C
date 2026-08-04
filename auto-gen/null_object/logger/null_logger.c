/**
 * null_logger.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <myobj.h>
#include "null_logger.h"

static struct null_logger g_null_logger;
static int g_null_logger_inited;

static void null_logger_ops_log(struct logger *logger, const char *msg)
{
	(void)logger;
	(void)msg;
	_MY_TRACE_STR("null_logger::log() [noop]\n");
	/* intentionally empty */
}

static struct logger_ops logger_ops = {
	.log = null_logger_ops_log,
};

/** constructor(). */
void null_logger_init(struct null_logger *null_logger)
{
	_MY_TRACE_STR("null_logger_init()\n");
	memset(null_logger, 0, sizeof(*null_logger));
	logger_init(&null_logger->logger);
	CLASS_OPS_INIT(null_logger->logger.ops, logger_ops);
}

struct logger *null_logger_instance(void)
{
	if (!g_null_logger_inited) {
		null_logger_init(&g_null_logger);
		g_null_logger_inited = 1;
	}
	return &g_null_logger.logger;
}
