/**
 * formatter.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include "formatter.h"

static void formatter_ops_format(struct formatter *formatter, const char *in,
	char *out, size_t sz)
{
	(void)formatter;
	if (!out || !sz)
		return;
	snprintf(out, sz, "%s", in ? in : "");
}

static struct formatter_ops formatter_ops = {
	.format = formatter_ops_format,
};

/** constructor(). */
void formatter_init(struct formatter *formatter)
{
	_MY_TRACE_STR("formatter_init()\n");
	memset(formatter, 0, sizeof(*formatter));
	formatter->ops = &formatter_ops;
}
