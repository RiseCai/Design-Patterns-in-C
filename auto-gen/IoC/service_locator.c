/**
 * service_locator.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include "service_locator.h"

#define SERVICE_LOCATOR_CAP 8

struct locator_entry {
	char name[32];
	void *service;
};

static struct locator_entry g_entries[SERVICE_LOCATOR_CAP];
static int g_count;

void service_locator_reset(void)
{
	_MY_TRACE_STR("service_locator_reset()\n");
	memset(g_entries, 0, sizeof(g_entries));
	g_count = 0;
}

void service_locator_register(const char *name, void *service)
{
	_MY_TRACE_STR("service_locator::register()\n");
	if (!name || g_count >= SERVICE_LOCATOR_CAP)
		return;
	snprintf(g_entries[g_count].name, sizeof(g_entries[0].name), "%s", name);
	g_entries[g_count].service = service;
	g_count++;
}

void *service_locator_resolve(const char *name)
{
	int i;

	_MY_TRACE_STR("service_locator::resolve()\n");
	if (!name)
		return 0;
	for (i = 0; i < g_count; ++i) {
		if (strcmp(g_entries[i].name, name) == 0)
			return g_entries[i].service;
	}
	return 0;
}
