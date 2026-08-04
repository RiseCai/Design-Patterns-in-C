/**
 * context.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include "context.h"
#include "null_strategy.h"

/** constructor(). */
void context_init(struct context *context, struct strategy *strategy)
{
	_MY_TRACE_STR("context_init()\n");
	memset(context, 0, sizeof(*context));
	context->_strategy = strategy ? strategy : null_strategy_instance();
}

void context_set_strategy(struct context *context, struct strategy *strategy)
{
	_MY_TRACE_STR("context::set_strategy()\n");
	context->_strategy = strategy ? strategy : null_strategy_instance();
}

void context_process(struct context *context, const char *input, char *output, size_t sz)
{
	_MY_TRACE_STR("context::process()\n");
	/* No null check: strategy is always a valid object. */
	strategy_execute(context->_strategy, input, output, sz);
}
