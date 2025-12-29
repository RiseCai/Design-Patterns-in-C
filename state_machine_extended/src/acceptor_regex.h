/**
 * acceptor_regex.h  2025-12-03
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
 * Acceptor FSM for regular expression matching.
 */
#ifndef __ACCEPTOR_REGEX_H__
#define __ACCEPTOR_REGEX_H__

#include <mycommon.h>
#include <mytrace.h>

/** State in acceptor */
struct acceptor_state {
    int id;
    int is_accepting;
    struct transition {
        char input;
        struct acceptor_state *next;
    } transitions[10];
    int transition_count;
};

/** Acceptor machine */
struct acceptor_machine {
    struct acceptor_state *current;
    struct acceptor_state *start;
};

/* Alias for compatibility */
typedef struct acceptor_machine acceptor_regex;

void acceptor_machine_init(struct acceptor_machine *am, struct acceptor_state *start);
int acceptor_machine_feed(struct acceptor_machine *am, char c);
int acceptor_machine_is_accepting(struct acceptor_machine *am);

#endif /* __ACCEPTOR_REGEX_H__ */
