/**
 * service_locator.h
 *
 * Service Locator: a global registry that resolves dependencies by name.
 * Often contrasted with DI -- convenient, but hides dependencies.
 */
#ifndef __SERVICE_LOCATOR_H__
#define __SERVICE_LOCATOR_H__

#include <mycommon.h>
#include <mytrace.h>

void service_locator_reset(void);
void service_locator_register(const char *name, void *service);
void *service_locator_resolve(const char *name);

#endif /* __SERVICE_LOCATOR_H__ */
