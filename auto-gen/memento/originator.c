/**
 * originator.c
 *
 * Creates a memento containing a snapshot of its current internal state.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <myobj.h>
#include "originator.h"

static void originator_ops_set_state(struct originator *originator, int state)
{
	_MY_TRACE_STR("originator::set_state()\n");
	printf("originator state: %d -> %d\n", originator->state, state);
	originator->state = state;
}

static int originator_ops_get_state(struct originator *originator)
{
	_MY_TRACE_STR("originator::get_state()\n");
	return originator->state;
}

static struct memento *originator_ops_create_memento(struct originator *originator)
{
	struct memento *m;

	_MY_TRACE_STR("originator::create_memento()\n");
	m = malloc(sizeof(*m));
	if (!m)
		return 0;
	memento_init(m, originator->state);
	return m;
}

static void originator_ops_set_memento(struct originator *originator, struct memento *memento)
{
	_MY_TRACE_STR("originator::set_memento()\n");
	if (!memento)
		return;
	printf("originator restore state: %d -> %d\n",
		originator->state, memento_get_state(memento));
	originator->state = memento_get_state(memento);
}

static struct originator_ops originator_ops = {
	.set_state = originator_ops_set_state,
	.get_state = originator_ops_get_state,
	.create_memento = originator_ops_create_memento,
	.set_memento = originator_ops_set_memento,
};

/** constructor(). */
void originator_init(struct originator *originator)
{
	_MY_TRACE_STR("originator_init()\n");
	memset(originator, 0, sizeof(*originator));
	originator->ops = &originator_ops;
	originator->state = 0;
}
