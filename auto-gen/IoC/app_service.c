/**
 * app_service.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <myobj.h>
#include "app_service.h"

static void app_service_ops_inject_notifier(struct injectable *injectable,
	struct notifier *notifier)
{
	struct app_service *svc =
		container_of(injectable, typeof(*svc), injectable);

	_MY_TRACE_STR("app_service::inject_notifier() [interface injection]\n");
	svc->_notifier = notifier;
}

static struct injectable_ops injectable_ops = {
	.inject_notifier = app_service_ops_inject_notifier,
};

/** Constructor injection. */
void app_service_init(struct app_service *app_service, struct repository *repository)
{
	_MY_TRACE_STR("app_service_init() [constructor injection]\n");
	memset(app_service, 0, sizeof(*app_service));
	injectable_init(&app_service->injectable);
	CLASS_OPS_INIT(app_service->injectable.ops, injectable_ops);
	app_service->_repository = repository;
	app_service->_notifier = 0;
}

/** Setter injection. */
void app_service_set_notifier(struct app_service *app_service, struct notifier *notifier)
{
	_MY_TRACE_STR("app_service::set_notifier() [setter injection]\n");
	app_service->_notifier = notifier;
}

int app_service_create_user(struct app_service *app_service, const char *name,
	struct formatter *formatter)
{
	char stored[64];
	int rc;

	_MY_TRACE_STR("app_service::create_user()\n");
	if (!app_service->_repository)
		return -1;

	/* Method injection: formatter is provided by the caller for this call. */
	if (formatter)
		formatter_format(formatter, name, stored, sizeof(stored));
	else
		snprintf(stored, sizeof(stored), "%s", name ? name : "");

	rc = repository_save(app_service->_repository, stored);
	if (rc != 0)
		return rc;

	if (app_service->_notifier) {
		char msg[80];
		snprintf(msg, sizeof(msg), "user created: %s", stored);
		notifier_notify(app_service->_notifier, msg);
	}
	return 0;
}
