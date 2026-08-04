/**
 * memento.h
 *
 * Without violating encapsulation, capture and externalize an object's
 * internal state so that the object can be restored to this state later.
 * Participants:
 *   Memento
 *     - stores internal state of the Originator object
 *     - protects against access by objects other than the Originator
 */
#ifndef __MEMENTO_H__
#define __MEMENTO_H__

#include <mycommon.h>
#include <mytrace.h>

struct memento {
	int state;
};

/** constructor(). */
void memento_init(struct memento *memento, int state);

int memento_get_state(struct memento *memento);

#endif /* __MEMENTO_H__ */
