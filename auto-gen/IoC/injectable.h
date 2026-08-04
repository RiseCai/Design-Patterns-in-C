/**
 * injectable.h
 *
 * Interface injection: client implements this interface so an assembler
 * can push dependencies without knowing the concrete client type.
 */
#ifndef __INJECTABLE_H__
#define __INJECTABLE_H__

#include <mycommon.h>
#include <mytrace.h>

#include "notifier.h"

struct injectable_ops;
struct injectable {
	struct injectable_ops *ops;
};
struct injectable_ops {
	void (*inject_notifier)(struct injectable *, struct notifier *notifier);
};

/** constructor(). */
void injectable_init(struct injectable *injectable);

static inline void injectable_inject_notifier(struct injectable *injectable,
	struct notifier *notifier)
{
	injectable->ops->inject_notifier(injectable, notifier);
}

#endif /* __INJECTABLE_H__ */
