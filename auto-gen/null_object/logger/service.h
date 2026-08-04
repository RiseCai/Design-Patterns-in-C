/**
 * service.h - client that always uses a Logger (never NULL)
 */
#ifndef __SERVICE_H__
#define __SERVICE_H__

#include "logger.h"

struct service {
	struct logger *_logger;
};

/** constructor(): if logger is NULL, installs Null Object. */
void service_init(struct service *service, struct logger *logger);

void service_run(struct service *service, const char *job);

#endif /* __SERVICE_H__ */
