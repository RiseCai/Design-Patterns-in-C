/**
 * mealy_machine.h  2025-12-03
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Auto generate for Design Patterns in C *
 * 
 * Mealy event-driven state machine for UI control.
 * Output depends on both current state and input event.
 */
#ifndef __MEALY_MACHINE_H__
#define __MEALY_MACHINE_H__

#include "mycommon.h"
#include "mytrace.h"

struct mealy_state;

/** Mealy machine context */
struct mealy_machine {
	struct mealy_state *_state;
	void *_user_data; /* optional user data for UI context */
};

/** Mealy state operations */
struct mealy_state_ops {
	void (*_destructor)(struct mealy_state *);
	void (*free)(struct mealy_state *);
	/* Event handlers */
	void (*event_click)(struct mealy_state *, struct mealy_machine *);
	void (*event_hover)(struct mealy_state *, struct mealy_machine *);
	void (*event_keypress)(struct mealy_state *, struct mealy_machine *, int key);
	/* UI output action */
	void (*render)(struct mealy_state *, struct mealy_machine *);
	struct mealy_state_ops *__super;
};

/** Mealy state base */
struct mealy_state {
	struct mealy_state_ops *ops;
};

/** constructor */
void mealy_machine_init(struct mealy_machine *m, struct mealy_state *initial);

/** event triggers */
void mealy_machine_event_click(struct mealy_machine *m);
void mealy_machine_event_hover(struct mealy_machine *m);
void mealy_machine_event_keypress(struct mealy_machine *m, int key);

/** state transition */
void mealy_machine_set_state(struct mealy_machine *m, struct mealy_state *new_state);

/** get current state */
struct mealy_state *mealy_machine_get_state(struct mealy_machine *m);

#endif /* __MEALY_MACHINE_H__ */
