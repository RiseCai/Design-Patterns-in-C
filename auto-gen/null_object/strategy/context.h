/**
 * context.h - always holds a Strategy (Null Object when unset)
 */
#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include "strategy.h"

struct context {
	struct strategy *_strategy;
};

/** constructor(): NULL strategy becomes Null Object. */
void context_init(struct context *context, struct strategy *strategy);

void context_set_strategy(struct context *context, struct strategy *strategy);
void context_process(struct context *context, const char *input, char *output, size_t sz);

#endif /* __CONTEXT_H__ */
