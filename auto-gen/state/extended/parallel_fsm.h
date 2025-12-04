/**
 * parallel_fsm.h  2025-12-03
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
 * Parallel state machine for concurrent systems.
 */
#ifndef __PARALLEL_FSM_H__
#define __PARALLEL_FSM_H__

#include <mycommon.h>
#include <mytrace.h>

/** Individual FSM in parallel */
struct parallel_component {
    int id;
    void *state;
    void (*step)(struct parallel_component *comp, int event);
};

/** Parallel machine containing multiple FSMs */
struct parallel_machine {
    struct parallel_component **components;
    int count;
    int capacity;
};

void parallel_machine_init(struct parallel_machine *pm, int capacity);
void parallel_machine_add_component(struct parallel_machine *pm, struct parallel_component *comp);
void parallel_machine_broadcast_event(struct parallel_machine *pm, int event);
void parallel_machine_sync(struct parallel_machine *pm);

#endif /* __PARALLEL_FSM_H__ */
