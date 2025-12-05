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

/** Events for parallel FSMs */
enum parallel_event {
    EVENT_START,
    EVENT_STOP,
    EVENT_DATA_READY,
    EVENT_FINISH,
    EVENT_RESET,
    EVENT_ENABLE,
    EVENT_DISABLE,
    EVENT_MAX
};

/** Component A states */
enum comp_a_state {
    A_IDLE,
    A_ACTIVE
};

/** Component B states */
enum comp_b_state {
    B_IDLE,
    B_PROCESSING,
    B_DONE
};

/** Component C states */
enum comp_c_state {
    C_OFF,
    C_ON
};

/** Individual FSM in parallel */
struct parallel_component {
    int id;
    void *state;  /* pointer to component-specific state variable */
    void (*step)(struct parallel_component *comp, int event);
    void (*entry)(struct parallel_component *comp);
    void (*do_action)(struct parallel_component *comp);
    void (*exit)(struct parallel_component *comp);
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

/* Component-specific functions */
void component_a_step(struct parallel_component *comp, int event);
void component_b_step(struct parallel_component *comp, int event);
void component_c_step(struct parallel_component *comp, int event);

/* Component entry/do/exit functions */
void component_a_entry(struct parallel_component *comp);
void component_a_do(struct parallel_component *comp);
void component_a_exit(struct parallel_component *comp);
void component_b_entry(struct parallel_component *comp);
void component_b_do(struct parallel_component *comp);
void component_b_exit(struct parallel_component *comp);
void component_c_entry(struct parallel_component *comp);
void component_c_do(struct parallel_component *comp);
void component_c_exit(struct parallel_component *comp);

/* Component creation */
struct parallel_component *parallel_component_create(int id, void *state,
        void (*step)(struct parallel_component *, int),
        void (*entry)(struct parallel_component *),
        void (*do_action)(struct parallel_component *),
        void (*exit)(struct parallel_component *));

#endif /* __PARALLEL_FSM_H__ */
