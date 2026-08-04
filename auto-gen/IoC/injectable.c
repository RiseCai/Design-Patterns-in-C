/**
 * injectable.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include "injectable.h"

static void injectable_ops_inject_notifier(struct injectable *injectable,
	struct notifier *notifier)
{
	(void)injectable;
	(void)notifier;
}

static struct injectable_ops injectable_ops = {
	.inject_notifier = injectable_ops_inject_notifier,
};

/** constructor(). */
void injectable_init(struct injectable *injectable)
{
	_MY_TRACE_STR("injectable_init()\n");
	memset(injectable, 0, sizeof(*injectable));
	injectable->ops = &injectable_ops;
}
