/**
 * null_logger.h - Null Object: do-nothing Logger
 */
#ifndef __NULL_LOGGER_H__
#define __NULL_LOGGER_H__

#include "logger.h"

struct null_logger {
	struct logger logger;
};

/** constructor(). */
void null_logger_init(struct null_logger *null_logger);

/** Shared singleton-style null logger for convenience. */
struct logger *null_logger_instance(void);

#endif /* __NULL_LOGGER_H__ */
