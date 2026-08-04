/**
 * service.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include "service.h"
#include "null_logger.h"

/** constructor(). */
void service_init(struct service *service, struct logger *logger)
{
	_MY_TRACE_STR("service_init()\n");
	memset(service, 0, sizeof(*service));
	/* Null Object removes the need for if (logger) checks later. */
	service->_logger = logger ? logger : null_logger_instance();
}

void service_run(struct service *service, const char *job)
{
	_MY_TRACE_STR("service::run()\n");
	logger_log(service->_logger, "service start");
	printf("service doing: %s\n", job ? job : "");
	logger_log(service->_logger, "service done");
}
