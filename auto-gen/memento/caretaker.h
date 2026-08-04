/**
 * caretaker.h
 *
 * Is responsible for the memento's safekeeping.
 * Never operates on or examines the contents of a memento.
 */
#ifndef __CARETAKER_H__
#define __CARETAKER_H__

#include <mycommon.h>
#include <mytrace.h>

#include "memento.h"

struct caretaker {
	struct memento *_memento;
};

/** constructor(). */
void caretaker_init(struct caretaker *caretaker);

void caretaker_set_memento(struct caretaker *caretaker, struct memento *memento);
struct memento *caretaker_get_memento(struct caretaker *caretaker);

#endif /* __CARETAKER_H__ */
