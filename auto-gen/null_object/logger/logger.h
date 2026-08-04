/**
 * logger.h
 *
 * Null Object: Logger interface.
 * Clients call log() without null-pointer checks.
 */
#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <mycommon.h>
#include <mytrace.h>

struct logger_ops;
struct logger {
	struct logger_ops *ops;
};
struct logger_ops {
	void (*log)(struct logger *, const char *msg);
};

/** constructor(). */
void logger_init(struct logger *logger);

static inline void logger_log(struct logger *logger, const char *msg)
{
	logger->ops->log(logger, msg);
}

#endif /* __LOGGER_H__ */
