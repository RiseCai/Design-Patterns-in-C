/**
 * upper_strategy.h - concrete Strategy: uppercase transform
 */
#ifndef __UPPER_STRATEGY_H__
#define __UPPER_STRATEGY_H__

#include "strategy.h"

struct upper_strategy {
	struct strategy strategy;
};

/** constructor(). */
void upper_strategy_init(struct upper_strategy *upper_strategy);

#endif /* __UPPER_STRATEGY_H__ */
