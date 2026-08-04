/**
 * null_strategy.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <myobj.h>
#include "null_strategy.h"

static struct null_strategy g_null_strategy;
static int g_null_strategy_inited;

static void null_strategy_ops_execute(struct strategy *strategy, const char *input,
	char *output, size_t sz)
{
	_MY_TRACE_STR("null_strategy::execute() [passthrough]\n");
	(void)strategy;
	if (!output || !sz)
		return;
	snprintf(output, sz, "%s", input ? input : "");
}

static struct strategy_ops strategy_ops = {
	.execute = null_strategy_ops_execute,
};

/** constructor(). */
void null_strategy_init(struct null_strategy *null_strategy)
{
	_MY_TRACE_STR("null_strategy_init()\n");
	memset(null_strategy, 0, sizeof(*null_strategy));
	strategy_init(&null_strategy->strategy);
	CLASS_OPS_INIT(null_strategy->strategy.ops, strategy_ops);
}

struct strategy *null_strategy_instance(void)
{
	if (!g_null_strategy_inited) {
		null_strategy_init(&g_null_strategy);
		g_null_strategy_inited = 1;
	}
	return &g_null_strategy.strategy;
}
