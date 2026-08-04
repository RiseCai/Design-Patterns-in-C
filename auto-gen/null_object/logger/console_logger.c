/**
 * console_logger.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <myobj.h>
#include "console_logger.h"

static void console_logger_ops_log(struct logger *logger, const char *msg)
{
	_MY_TRACE_STR("console_logger::log()\n");
	printf("[console] %s\n", msg ? msg : "");
}

static struct logger_ops logger_ops = {
	.log = console_logger_ops_log,
};

/** constructor(). */
void console_logger_init(struct console_logger *console_logger)
{
	_MY_TRACE_STR("console_logger_init()\n");
	memset(console_logger, 0, sizeof(*console_logger));
	logger_init(&console_logger->logger);
	CLASS_OPS_INIT(console_logger->logger.ops, logger_ops);
}
