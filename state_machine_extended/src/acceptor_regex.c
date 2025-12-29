/**
 * acceptor_regex.c  2025-12-03
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
 * Acceptor FSM for regex matching.
 */
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <mytrace.h>
#include "acceptor_regex.h"

/* Example states for regex "ab*" */
static struct acceptor_state q0, q1, accept_state, reject_state;

static void init_states(void)
{
    q0.id = 0;
    q0.is_accepting = 0;
    q0.transition_count = 2;
    q0.transitions[0].input = 'a';
    q0.transitions[0].next = &q1;
    q0.transitions[1].input = 'b';
    q0.transitions[1].next = &reject_state;

    q1.id = 1;
    q1.is_accepting = 1;
    q1.transition_count = 2;
    q1.transitions[0].input = 'b';
    q1.transitions[0].next = &q1;
    q1.transitions[1].input = 'a';
    q1.transitions[1].next = &reject_state;

    accept_state.id = 2;
    accept_state.is_accepting = 1;
    accept_state.transition_count = 0;

    reject_state.id = 3;
    reject_state.is_accepting = 0;
    reject_state.transition_count = 0;
}

void acceptor_machine_init(struct acceptor_machine *am, struct acceptor_state *start)
{
    memset(am, 0, sizeof(*am));
    init_states();
    am->start = start;
    am->current = start;
}

int acceptor_machine_feed(struct acceptor_machine *am, char c)
{
    _MY_TRACE_STR("acceptor_machine_feed\n");
    for (int i = 0; i < am->current->transition_count; i++) {
        if (am->current->transitions[i].input == c) {
            am->current = am->current->transitions[i].next;
            return 1;
        }
    }
    /* no transition -> go to reject */
    am->current = &reject_state;
    return 0;
}

int acceptor_machine_is_accepting(struct acceptor_machine *am)
{
    return am->current->is_accepting;
}
