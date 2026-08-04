/**
 * originator.h
 *
 * Creates a memento containing a snapshot of its current internal state.
 * Uses the memento to restore its internal state.
 */
#ifndef __ORIGINATOR_H__
#define __ORIGINATOR_H__

#include <mycommon.h>
#include <mytrace.h>

#include "memento.h"

struct originator_ops;
struct originator {
	struct originator_ops *ops;
	int state;
};
struct originator_ops {
	void (*set_state)(struct originator *, int state);
	int (*get_state)(struct originator *);
	struct memento *(*create_memento)(struct originator *);
	void (*set_memento)(struct originator *, struct memento *);
};

/** constructor(). */
void originator_init(struct originator *originator);

static inline void originator_set_state(struct originator *originator, int state)
{
	originator->ops->set_state(originator, state);
}
static inline int originator_get_state(struct originator *originator)
{
	return originator->ops->get_state(originator);
}
static inline struct memento *originator_create_memento(struct originator *originator)
{
	return originator->ops->create_memento(originator);
}
static inline void originator_set_memento(struct originator *originator, struct memento *memento)
{
	originator->ops->set_memento(originator, memento);
}

#endif /* __ORIGINATOR_H__ */
