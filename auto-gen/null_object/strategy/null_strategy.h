/**
 * null_strategy.h - Null Object: copies input unchanged (safe no-op policy)
 */
#ifndef __NULL_STRATEGY_H__
#define __NULL_STRATEGY_H__

#include "strategy.h"

struct null_strategy {
	struct strategy strategy;
};

/** constructor(). */
void null_strategy_init(struct null_strategy *null_strategy);

struct strategy *null_strategy_instance(void);

#endif /* __NULL_STRATEGY_H__ */
