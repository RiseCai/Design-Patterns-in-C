/**
 * moore_hierarchical.h  2025-12-03
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
 * Moore hierarchical state machine for embedded control.
 * Output depends only on state.
 */
#ifndef __MOORE_HIERARCHICAL_H__
#define __MOORE_HIERARCHICAL_H__

#include <mycommon.h>
#include <mytrace.h>

/** Hierarchical state node */
struct moore_state {
    const char *name;
    struct moore_state *parent;  /* superstate */
    struct moore_state *children[5]; /* substates */
    int child_count;
    void (*entry_action)(void);
    void (*exit_action)(void);
    void (*do_action)(void); /* Moore output */
    int (*handle_event)(struct moore_state *self, int event); /* optional event handler */
};

/** Hierarchical state machine */
struct moore_hsm {
    struct moore_state *current;
    struct moore_state *root;
    int temperature; /* example context */
    int target_temp;
};

void moore_hsm_init(struct moore_hsm *hsm, struct moore_state *root);
void moore_hsm_transition(struct moore_hsm *hsm, struct moore_state *target);
void moore_hsm_dispatch_event(struct moore_hsm *hsm, int event);
void moore_hsm_set_temperature(struct moore_hsm *hsm, int temp);
void moore_hsm_set_target(struct moore_hsm *hsm, int target);
struct moore_state *moore_hsm_find_state(struct moore_hsm *hsm, const char *name);

/* Global state instances (defined in moore_hierarchical.c) */
extern struct moore_state state_off;
extern struct moore_state state_on;
extern struct moore_state state_heating;
extern struct moore_state state_cooling;
extern struct moore_state state_idle;
extern struct moore_state state_fan_only;

#endif /* __MOORE_HIERARCHICAL_H__ */
