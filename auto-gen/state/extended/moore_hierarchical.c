/**
 * moore_hierarchical.c  2025-12-03
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
 * Moore hierarchical state machine implementation.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <mytrace.h>
#include "moore_hierarchical.h"

static void off_do(void)
{
    printf("Moore output: Actuator OFF\n");
}

static void on_do(void)
{
    printf("Moore output: Actuator ON\n");
}

static struct moore_state off_state = {
    .name = "off",
    .parent = NULL,
    .child_count = 0,
    .do_action = off_do,
};

static struct moore_state on_state = {
    .name = "on",
    .parent = NULL,
    .child_count = 0,
    .do_action = on_do,
};

void moore_hsm_init(struct moore_hsm *hsm, struct moore_state *root)
{
    memset(hsm, 0, sizeof(*hsm));
    hsm->current = root;
    hsm->root = root;
}

void moore_hsm_transition(struct moore_hsm *hsm, struct moore_state *target)
{
    _MY_TRACE_STR("moore_hsm_transition\n");
    if (hsm->current && hsm->current->exit_action) {
        hsm->current->exit_action();
    }
    hsm->current = target;
    if (target && target->entry_action) {
        target->entry_action();
    }
}

void moore_hsm_dispatch_event(struct moore_hsm *hsm, int event)
{
    _MY_TRACE_STR("moore_hsm_dispatch_event\n");
    /* Simple example: toggle between off and on */
    if (event == 1) {
        if (hsm->current == &off_state) {
            moore_hsm_transition(hsm, &on_state);
        } else {
            moore_hsm_transition(hsm, &off_state);
        }
    }
    /* Execute Moore output */
    if (hsm->current && hsm->current->do_action) {
        hsm->current->do_action();
    }
}
