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

static void component_step(struct parallel_component *comp, int event)
{
    _MY_TRACE_STR("component_step\n");
    printf("Component %d processing event %d\n", comp->id, event);
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
    _MY_TRACE_STR("parallel_machine_broadcast_event\n");
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
