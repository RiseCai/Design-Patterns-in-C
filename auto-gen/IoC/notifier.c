/**
 * notifier.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include "notifier.h"

static void notifier_ops_notify(struct notifier *notifier, const char *msg)
{
	(void)notifier;
	(void)msg;
}

static struct notifier_ops notifier_ops = {
	.notify = notifier_ops_notify,
};

/** constructor(). */
void notifier_init(struct notifier *notifier)
{
	_MY_TRACE_STR("notifier_init()\n");
	memset(notifier, 0, sizeof(*notifier));
	notifier->ops = &notifier_ops;
}
