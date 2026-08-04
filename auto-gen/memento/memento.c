/**
 * memento.c
 *
 * Stores internal state of the Originator object.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include "memento.h"

/** constructor(). */
void memento_init(struct memento *memento, int state)
{
	_MY_TRACE_STR("memento_init()\n");
	memset(memento, 0, sizeof(*memento));
	memento->state = state;
}

int memento_get_state(struct memento *memento)
{
	_MY_TRACE_STR("memento::get_state()\n");
	return memento->state;
}
