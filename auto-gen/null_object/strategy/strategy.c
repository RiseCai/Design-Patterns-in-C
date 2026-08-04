/**
 * strategy.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include "strategy.h"

static void strategy_ops_execute(struct strategy *strategy, const char *input,
	char *output, size_t sz)
{
	(void)strategy;
	(void)input;
	if (output && sz)
		output[0] = '\0';
}

static struct strategy_ops strategy_ops = {
	.execute = strategy_ops_execute,
};

/** constructor(). */
void strategy_init(struct strategy *strategy)
{
	_MY_TRACE_STR("strategy_init()\n");
	memset(strategy, 0, sizeof(*strategy));
	strategy->ops = &strategy_ops;
}
