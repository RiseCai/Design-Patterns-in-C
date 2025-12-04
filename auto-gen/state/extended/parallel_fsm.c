/**
 * parallel_fsm.c  2025-12-03
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
 * Parallel state machine implementation.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <mytrace.h>
#include "parallel_fsm.h"

/* Component A step function */
void component_a_step(struct parallel_component *comp, int event)
{
    enum comp_a_state *state = (enum comp_a_state *)comp->state;
    _MY_TRACE_STR("component_a_step: state=%d, event=%d\n", *state, event);
    switch (*state) {
        case A_IDLE:
            if (event == EVENT_START) {
                if (comp->exit) comp->exit(comp);
                *state = A_ACTIVE;
                if (comp->entry) comp->entry(comp);
                printf("Component A: IDLE -> ACTIVE\n");
            }
            break;
        case A_ACTIVE:
            if (event == EVENT_STOP) {
                if (comp->exit) comp->exit(comp);
                *state = A_IDLE;
                if (comp->entry) comp->entry(comp);
                printf("Component A: ACTIVE -> IDLE\n");
            }
            break;
        default:
            break;
    }
    if (comp->do_action) comp->do_action(comp);
}

/* Component B step function */
void component_b_step(struct parallel_component *comp, int event)
{
    enum comp_b_state *state = (enum comp_b_state *)comp->state;
    _MY_TRACE_STR("component_b_step: state=%d, event=%d\n", *state, event);
    switch (*state) {
        case B_IDLE:
            if (event == EVENT_DATA_READY) {
                if (comp->exit) comp->exit(comp);
                *state = B_PROCESSING;
                if (comp->entry) comp->entry(comp);
                printf("Component B: IDLE -> PROCESSING\n");
            }
            break;
        case B_PROCESSING:
            if (event == EVENT_FINISH) {
                if (comp->exit) comp->exit(comp);
                *state = B_DONE;
                if (comp->entry) comp->entry(comp);
                printf("Component B: PROCESSING -> DONE\n");
            }
            break;
        case B_DONE:
            if (event == EVENT_RESET) {
                if (comp->exit) comp->exit(comp);
                *state = B_IDLE;
                if (comp->entry) comp->entry(comp);
                printf("Component B: DONE -> IDLE\n");
            }
            break;
        default:
            break;
    }
    if (comp->do_action) comp->do_action(comp);
}

/* Component C step function */
void component_c_step(struct parallel_component *comp, int event)
{
    enum comp_c_state *state = (enum comp_c_state *)comp->state;
    _MY_TRACE_STR("component_c_step: state=%d, event=%d\n", *state, event);
    switch (*state) {
        case C_OFF:
            if (event == EVENT_ENABLE) {
                if (comp->exit) comp->exit(comp);
                *state = C_ON;
                if (comp->entry) comp->entry(comp);
                printf("Component C: OFF -> ON\n");
            }
            break;
        case C_ON:
            if (event == EVENT_DISABLE) {
                if (comp->exit) comp->exit(comp);
                *state = C_OFF;
                if (comp->entry) comp->entry(comp);
                printf("Component C: ON -> OFF\n");
            }
            break;
        default:
            break;
    }
    if (comp->do_action) comp->do_action(comp);
}

/* Entry/do/exit stubs for components */
void component_a_entry(struct parallel_component *comp)
{
    printf("Component A entry: initialize\n");
}

void component_a_do(struct parallel_component *comp)
{
    enum comp_a_state *state = (enum comp_a_state *)comp->state;
    if (*state == A_IDLE) {
        printf("Component A do: wait for start\n");
    } else if (*state == A_ACTIVE) {
        printf("Component A do: perform task\n");
    }
}

void component_a_exit(struct parallel_component *comp)
{
    printf("Component A exit: cleanup\n");
}

void component_b_entry(struct parallel_component *comp)
{
    enum comp_b_state *state = (enum comp_b_state *)comp->state;
    switch (*state) {
        case B_IDLE:
            printf("Component B entry: reset data\n");
            break;
        case B_PROCESSING:
            printf("Component B entry: start processing\n");
            break;
        case B_DONE:
            printf("Component B entry: set done\n");
            break;
    }
}

void component_b_do(struct parallel_component *comp)
{
    enum comp_b_state *state = (enum comp_b_state *)comp->state;
    switch (*state) {
        case B_IDLE:
            printf("Component B do: idle\n");
            break;
        case B_PROCESSING:
            printf("Component B do: process\n");
            break;
        case B_DONE:
            printf("Component B do: hold result\n");
            break;
    }
}

void component_b_exit(struct parallel_component *comp)
{
    enum comp_b_state *state = (enum comp_b_state *)comp->state;
    switch (*state) {
        case B_IDLE:
            printf("Component B exit: prepare\n");
            break;
        case B_PROCESSING:
            printf("Component B exit: finish\n");
            break;
        case B_DONE:
            printf("Component B exit: reset\n");
            break;
    }
}

void component_c_entry(struct parallel_component *comp)
{
    enum comp_c_state *state = (enum comp_c_state *)comp->state;
    if (*state == C_OFF) {
        printf("Component C entry: power off\n");
    } else if (*state == C_ON) {
        printf("Component C entry: power on\n");
    }
}

void component_c_do(struct parallel_component *comp)
{
    enum comp_c_state *state = (enum comp_c_state *)comp->state;
    if (*state == C_OFF) {
        printf("Component C do: standby\n");
    } else if (*state == C_ON) {
        printf("Component C do: operate\n");
    }
}

void component_c_exit(struct parallel_component *comp)
{
    enum comp_c_state *state = (enum comp_c_state *)comp->state;
    if (*state == C_OFF) {
        printf("Component C exit: shutdown\n");
    } else if (*state == C_ON) {
        printf("Component C exit: turn off\n");
    }
}

/* Helper to create a component */
struct parallel_component *parallel_component_create(int id, void *state,
        void (*step)(struct parallel_component *, int),
        void (*entry)(struct parallel_component *),
        void (*do_action)(struct parallel_component *),
        void (*exit)(struct parallel_component *))
{
    struct parallel_component *comp = malloc(sizeof(*comp));
    if (!comp) return NULL;
    comp->id = id;
    comp->state = state;
    comp->step = step;
    comp->entry = entry;
    comp->do_action = do_action;
    comp->exit = exit;
    return comp;
}

void parallel_machine_init(struct parallel_machine *pm, int capacity)
{
    memset(pm, 0, sizeof(*pm));
    pm->capacity = capacity;
    pm->components = malloc(capacity * sizeof(*pm->components));
    pm->count = 0;
}

void parallel_machine_add_component(struct parallel_machine *pm, struct parallel_component *comp)
{
    if (pm->count < pm->capacity) {
        pm->components[pm->count] = comp;
        pm->count++;
    }
}

void parallel_machine_broadcast_event(struct parallel_machine *pm, int event)
{
    _MY_TRACE_STR("parallel_machine_broadcast_event: event=%d\n", event);
    for (int i = 0; i < pm->count; i++) {
        if (pm->components[i] && pm->components[i]->step) {
            pm->components[i]->step(pm->components[i], event);
        }
    }
}

void parallel_machine_sync(struct parallel_machine *pm)
{
    _MY_TRACE_STR("parallel_machine_sync\n");
    printf("Synchronizing %d components\n", pm->count);
}

/* Example usage (if needed) */
#ifdef TEST_PARALLEL_FSM
int main()
{
    struct parallel_machine pm;
    parallel_machine_init(&pm, 3);

    enum comp_a_state a_state = A_IDLE;
    enum comp_b_state b_state = B_IDLE;
    enum comp_c_state c_state = C_OFF;

    struct parallel_component *compA = parallel_component_create(1, &a_state,
            component_a_step, component_a_entry, component_a_do, component_a_exit);
    struct parallel_component *compB = parallel_component_create(2, &b_state,
            component_b_step, component_b_entry, component_b_do, component_b_exit);
    struct parallel_component *compC = parallel_component_create(3, &c_state,
            component_c_step, component_c_entry, component_c_do, component_c_exit);

    parallel_machine_add_component(&pm, compA);
    parallel_machine_add_component(&pm, compB);
    parallel_machine_add_component(&pm, compC);

    /* Simulate events */
    parallel_machine_broadcast_event(&pm, EVENT_START);
    parallel_machine_broadcast_event(&pm, EVENT_DATA_READY);
    parallel_machine_broadcast_event(&pm, EVENT_ENABLE);
    parallel_machine_broadcast_event(&pm, EVENT_FINISH);
    parallel_machine_broadcast_event(&pm, EVENT_STOP);
    parallel_machine_broadcast_event(&pm, EVENT_DISABLE);
    parallel_machine_broadcast_event(&pm, EVENT_RESET);

    parallel_machine_sync(&pm);

    free(compA);
    free(compB);
    free(compC);
    free(pm.components);
    return 0;
}
#endif
