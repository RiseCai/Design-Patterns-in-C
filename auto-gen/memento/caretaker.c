/**
 * caretaker.c
 *
 * Is responsible for the memento's safekeeping.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include "caretaker.h"

/** constructor(). */
void caretaker_init(struct caretaker *caretaker)
{
	_MY_TRACE_STR("caretaker_init()\n");
	memset(caretaker, 0, sizeof(*caretaker));
	caretaker->_memento = 0;
}

void caretaker_set_memento(struct caretaker *caretaker, struct memento *memento)
{
	_MY_TRACE_STR("caretaker::set_memento()\n");
	caretaker->_memento = memento;
}

struct memento *caretaker_get_memento(struct caretaker *caretaker)
{
	_MY_TRACE_STR("caretaker::get_memento()\n");
	return caretaker->_memento;
}
