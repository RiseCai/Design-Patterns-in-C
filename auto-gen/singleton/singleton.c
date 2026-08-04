/**
 * singleton.c
 *
 * Ensure a class has only one instance and provide a global point of access.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <myobj.h>
#include "singleton.h"

static struct singleton *g_instance;

static void singleton_ops_set_name(struct singleton *singleton, const char *name)
{
	_MY_TRACE_STR("singleton::set_name()\n");
	snprintf(singleton->name, sizeof(singleton->name), "%s", name ? name : "");
}

static void singleton_ops_set_value(struct singleton *singleton, int value)
{
	_MY_TRACE_STR("singleton::set_value()\n");
	singleton->value = value;
}

static const char *singleton_ops_get_name(struct singleton *singleton)
{
	_MY_TRACE_STR("singleton::get_name()\n");
	return singleton->name;
}

static int singleton_ops_get_value(struct singleton *singleton)
{
	_MY_TRACE_STR("singleton::get_value()\n");
	return singleton->value;
}

static struct singleton_ops singleton_ops = {
	.set_name = singleton_ops_set_name,
	.set_value = singleton_ops_set_value,
	.get_name = singleton_ops_get_name,
	.get_value = singleton_ops_get_value,
};

static void singleton_init(struct singleton *singleton)
{
	_MY_TRACE_STR("singleton_init()\n");
	memset(singleton, 0, sizeof(*singleton));
	singleton->ops = &singleton_ops;
	snprintf(singleton->name, sizeof(singleton->name), "%s", "default");
	singleton->value = 0;
}

struct singleton *singleton_get_instance(void)
{
	_MY_TRACE_STR("singleton_get_instance()\n");
	if (!g_instance) {
		g_instance = malloc(sizeof(*g_instance));
		if (g_instance)
			singleton_init(g_instance);
	}
	return g_instance;
}
