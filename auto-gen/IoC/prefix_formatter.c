/**
 * prefix_formatter.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <myobj.h>
#include "prefix_formatter.h"

static void prefix_formatter_ops_format(struct formatter *formatter, const char *in,
	char *out, size_t sz)
{
	struct prefix_formatter *fmt =
		container_of(formatter, typeof(*fmt), formatter);

	_MY_TRACE_STR("prefix_formatter::format()\n");
	if (!out || !sz)
		return;
	snprintf(out, sz, "%s%s", fmt->prefix, in ? in : "");
}

static struct formatter_ops formatter_ops = {
	.format = prefix_formatter_ops_format,
};

/** constructor(). */
void prefix_formatter_init(struct prefix_formatter *prefix_formatter, const char *prefix)
{
	_MY_TRACE_STR("prefix_formatter_init()\n");
	memset(prefix_formatter, 0, sizeof(*prefix_formatter));
	formatter_init(&prefix_formatter->formatter);
	CLASS_OPS_INIT(prefix_formatter->formatter.ops, formatter_ops);
	snprintf(prefix_formatter->prefix, sizeof(prefix_formatter->prefix), "%s",
		prefix ? prefix : "");
}
