/**
 * upper_strategy.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <mycommon.h>
#include <myobj.h>
#include "upper_strategy.h"

static void upper_strategy_ops_execute(struct strategy *strategy, const char *input,
	char *output, size_t sz)
{
	size_t i;

	_MY_TRACE_STR("upper_strategy::execute()\n");
	(void)strategy;
	if (!output || !sz)
		return;
	if (!input) {
		output[0] = '\0';
		return;
	}
	for (i = 0; i + 1 < sz && input[i]; ++i)
		output[i] = (char)toupper((unsigned char)input[i]);
	output[i] = '\0';
}

static struct strategy_ops strategy_ops = {
	.execute = upper_strategy_ops_execute,
};

/** constructor(). */
void upper_strategy_init(struct upper_strategy *upper_strategy)
{
	_MY_TRACE_STR("upper_strategy_init()\n");
	memset(upper_strategy, 0, sizeof(*upper_strategy));
	strategy_init(&upper_strategy->strategy);
	CLASS_OPS_INIT(upper_strategy->strategy.ops, strategy_ops);
}
