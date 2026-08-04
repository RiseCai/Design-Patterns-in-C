/**
 * console_notifier.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <myobj.h>
#include "console_notifier.h"

static void console_notifier_ops_notify(struct notifier *notifier, const char *msg)
{
	struct console_notifier *n =
		container_of(notifier, typeof(*n), notifier);

	_MY_TRACE_STR("console_notifier::notify()\n");
	n->notify_count++;
	printf("[notify] %s\n", msg ? msg : "");
}

static struct notifier_ops notifier_ops = {
	.notify = console_notifier_ops_notify,
};

/** constructor(). */
void console_notifier_init(struct console_notifier *console_notifier)
{
	_MY_TRACE_STR("console_notifier_init()\n");
	memset(console_notifier, 0, sizeof(*console_notifier));
	notifier_init(&console_notifier->notifier);
	CLASS_OPS_INIT(console_notifier->notifier.ops, notifier_ops);
}
