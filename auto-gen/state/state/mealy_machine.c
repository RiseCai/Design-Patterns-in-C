/**
 * mealy_machine.c  2025-12-03
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
 * Mealy event-driven state machine implementation.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include "mealy_machine.h"

/** constructor */
void mealy_machine_init(struct mealy_machine *m, struct mealy_state *initial)
{
	_MY_TRACE_STR("mealy_machine_init()\n");
	memset(m, 0, sizeof(*m));
	m->_state = initial;
	m->_user_data = NULL;
}

/** event triggers */
void mealy_machine_event_click(struct mealy_machine *m)
{
	_MY_TRACE_STR("mealy_machine_event_click()\n");
	if (m->_state && m->_state->ops && m->_state->ops->event_click) {
		m->_state->ops->event_click(m->_state, m);
	}
}

void mealy_machine_event_hover(struct mealy_machine *m)
{
	_MY_TRACE_STR("mealy_machine_event_hover()\n");
	if (m->_state && m->_state->ops && m->_state->ops->event_hover) {
		m->_state->ops->event_hover(m->_state, m);
	}
}

void mealy_machine_event_keypress(struct mealy_machine *m, int key)
{
	_MY_TRACE_STR("mealy_machine_event_keypress()\n");
	if (m->_state && m->_state->ops && m->_state->ops->event_keypress) {
		m->_state->ops->event_keypress(m->_state, m, key);
	}
}

/** state transition */
void mealy_machine_set_state(struct mealy_machine *m, struct mealy_state *new_state)
{
	_MY_TRACE_STR("mealy_machine_set_state()\n");
	m->_state = new_state;
}

/** get current state */
struct mealy_state *mealy_machine_get_state(struct mealy_machine *m)
{
	_MY_TRACE_STR("mealy_machine_get_state()\n");
	return m->_state;
}
