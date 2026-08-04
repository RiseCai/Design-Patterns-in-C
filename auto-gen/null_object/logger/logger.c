/**
 * logger.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include "logger.h"

static void logger_ops_log(struct logger *logger, const char *msg)
{
	(void)logger;
	(void)msg;
	/* abstract base: overridden by concrete loggers */
}

static struct logger_ops logger_ops = {
	.log = logger_ops_log,
};

/** constructor(). */
void logger_init(struct logger *logger)
{
	_MY_TRACE_STR("logger_init()\n");
	memset(logger, 0, sizeof(*logger));
	logger->ops = &logger_ops;
}
