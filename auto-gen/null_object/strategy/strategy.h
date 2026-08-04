/**
 * strategy.h
 *
 * Null Object applied to Strategy: Context always holds a strategy pointer.
 */
#ifndef __NO_STRATEGY_H__
#define __NO_STRATEGY_H__

#include <stddef.h>

#include <mycommon.h>
#include <mytrace.h>

struct strategy_ops;
struct strategy {
	struct strategy_ops *ops;
};
struct strategy_ops {
	void (*execute)(struct strategy *, const char *input, char *output, size_t sz);
};

/** constructor(). */
void strategy_init(struct strategy *strategy);

static inline void strategy_execute(struct strategy *strategy, const char *input,
	char *output, size_t sz)
{
	strategy->ops->execute(strategy, input, output, sz);
}

#endif /* __NO_STRATEGY_H__ */
