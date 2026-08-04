/**
 * app_service.h
 *
 * IoC client. Dependencies are supplied from outside (DI), not created inside.
 *
 *   - Constructor injection: repository via app_service_init()
 *   - Setter injection:      notifier via app_service_set_notifier()
 *   - Interface injection:   notifier via injectable_inject_notifier()
 *   - Method injection:      formatter argument of app_service_create_user()
 */
#ifndef __APP_SERVICE_H__
#define __APP_SERVICE_H__

#include "repository.h"
#include "notifier.h"
#include "formatter.h"
#include "injectable.h"

struct app_service {
	struct injectable injectable; /* interface-injection facade */
	struct repository *_repository;
	struct notifier *_notifier;
};

/** Constructor injection: repository is required. */
void app_service_init(struct app_service *app_service, struct repository *repository);

/** Setter injection. */
void app_service_set_notifier(struct app_service *app_service, struct notifier *notifier);

/**
 * Create user:
 *   - uses ctor-injected repository
 *   - uses setter/interface-injected notifier (optional)
 *   - method-injects formatter for this call only (optional -> identity)
 */
int app_service_create_user(struct app_service *app_service, const char *name,
	struct formatter *formatter);

#endif /* __APP_SERVICE_H__ */
