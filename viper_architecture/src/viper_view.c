/**
 * viper_view.c - VIPER View Component Implementation
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Auto generate for Design-Patterns in C *
 * 
 * VIPER View component implemented as a Mealy state machine.
 * The View handles user interface events and renders output.
 */

#include "viper.h"
#include "viper_view.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../state_machine_extended/src/mealy_machine.h"

/* View states */
struct viper_view_state_idle;
struct viper_view_state_loading;
struct viper_view_state_displaying;
struct viper_view_state_error;

/* View state structures */
struct viper_view_state_idle {
    struct mealy_state base;
    /* View-specific data */
    int refresh_count;
};

struct viper_view_state_loading {
    struct mealy_state base;
    /* Loading-specific data */
    float progress;
};

struct viper_view_state_displaying {
    struct mealy_state base;
    /* Displaying-specific data */
    void *display_data;
};

struct viper_view_state_error {
    struct mealy_state base;
    /* Error-specific data */
    char error_message[128];
};

/* State operations */
static void viper_view_state_idle_event_click(struct mealy_state *state, struct mealy_machine *machine);
static void viper_view_state_idle_render(struct mealy_state *state, struct mealy_machine *machine);

static void viper_view_state_loading_event_click(struct mealy_state *state, struct mealy_machine *machine);
static void viper_view_state_loading_render(struct mealy_state *state, struct mealy_machine *machine);

static void viper_view_state_displaying_event_click(struct mealy_state *state, struct mealy_machine *machine);
static void viper_view_state_displaying_render(struct mealy_state *state, struct mealy_machine *machine);

static void viper_view_state_error_event_click(struct mealy_state *state, struct mealy_machine *machine);
static void viper_view_state_error_render(struct mealy_state *state, struct mealy_machine *machine);

/* State operation tables */
static struct mealy_state_ops viper_view_state_idle_ops = {
    ._destructor = NULL,
    .free = NULL,
    .event_click = viper_view_state_idle_event_click,
    .event_hover = NULL,
    .event_keypress = NULL,
    .render = viper_view_state_idle_render,
    .__super = NULL
};

static struct mealy_state_ops viper_view_state_loading_ops = {
    ._destructor = NULL,
    .free = NULL,
    .event_click = viper_view_state_loading_event_click,
    .event_hover = NULL,
    .event_keypress = NULL,
    .render = viper_view_state_loading_render,
    .__super = NULL
};

static struct mealy_state_ops viper_view_state_displaying_ops = {
    ._destructor = NULL,
    .free = NULL,
    .event_click = viper_view_state_displaying_event_click,
    .event_hover = NULL,
    .event_keypress = NULL,
    .render = viper_view_state_displaying_render,
    .__super = NULL
};

static struct mealy_state_ops viper_view_state_error_ops = {
    ._destructor = NULL,
    .free = NULL,
    .event_click = viper_view_state_error_event_click,
    .event_hover = NULL,
    .event_keypress = NULL,
    .render = viper_view_state_error_render,
    .__super = NULL
};

/* Global state instances */
static struct viper_view_state_idle viper_view_idle_state = {
    .base = { .ops = &viper_view_state_idle_ops },
    .refresh_count = 0
};

static struct viper_view_state_loading viper_view_loading_state = {
    .base = { .ops = &viper_view_state_loading_ops },
    .progress = 0.0f
};

static struct viper_view_state_displaying viper_view_displaying_state = {
    .base = { .ops = &viper_view_state_displaying_ops },
    .display_data = NULL
};

static struct viper_view_state_error viper_view_error_state = {
    .base = { .ops = &viper_view_state_error_ops },
    .error_message = ""
};

/* State implementations */
static void viper_view_state_idle_event_click(struct mealy_state *state, struct mealy_machine *machine)
{
    struct viper_view_state_idle *idle_state = (struct viper_view_state_idle *)state;
    idle_state->refresh_count++;
    
    /* Transition to loading state on click */
    mealy_machine_set_state(machine, (struct mealy_state *)&viper_view_loading_state);
    
    /* In a real implementation, this would trigger data loading */
    printf("[View] Idle -> Loading (refresh count: %d)\n", idle_state->refresh_count);
}

static void viper_view_state_idle_render(struct mealy_state *state, struct mealy_machine *machine)
{
    struct viper_view_state_idle *idle_state = (struct viper_view_state_idle *)state;
    printf("[View] Rendering idle state (refresh count: %d)\n", idle_state->refresh_count);
}

static void viper_view_state_loading_event_click(struct mealy_state *state, struct mealy_machine *machine)
{
    struct viper_view_state_loading *loading_state = (struct viper_view_state_loading *)state;
    
    /* Cancel loading on click */
    printf("[View] Loading cancelled\n");
    mealy_machine_set_state(machine, (struct mealy_state *)&viper_view_idle_state);
}

static void viper_view_state_loading_render(struct mealy_state *state, struct mealy_machine *machine)
{
    struct viper_view_state_loading *loading_state = (struct viper_view_state_loading *)state;
    
    /* Simulate progress */
    loading_state->progress += 0.1f;
    if (loading_state->progress >= 1.0f) {
        loading_state->progress = 1.0f;
        /* Transition to displaying state when loading complete */
        mealy_machine_set_state(machine, (struct mealy_state *)&viper_view_displaying_state);
        printf("[View] Loading complete -> Displaying\n");
    }
    
    printf("[View] Rendering loading state (progress: %.1f%%)\n", loading_state->progress * 100);
}

static void viper_view_state_displaying_event_click(struct mealy_state *state, struct mealy_machine *machine)
{
    /* Go back to idle on click */
    printf("[View] Displaying -> Idle\n");
    mealy_machine_set_state(machine, (struct mealy_state *)&viper_view_idle_state);
}

static void viper_view_state_displaying_render(struct mealy_state *state, struct mealy_machine *machine)
{
    printf("[View] Rendering displaying state\n");
}

static void viper_view_state_error_event_click(struct mealy_state *state, struct mealy_machine *machine)
{
    /* Retry on click */
    printf("[View] Error -> Loading (retry)\n");
    mealy_machine_set_state(machine, (struct mealy_state *)&viper_view_loading_state);
}

static void viper_view_state_error_render(struct mealy_state *state, struct mealy_machine *machine)
{
    struct viper_view_state_error *error_state = (struct viper_view_state_error *)state;
    printf("[View] Rendering error state: %s\n", error_state->error_message);
}

/* VIPER View public interface */
struct viper_view *viper_view_create(void)
{
    struct viper_view *view = (struct viper_view *)malloc(sizeof(struct viper_view));
    if (!view) return NULL;
    
    memset(view, 0, sizeof(struct viper_view));
    
    /* Allocate and initialize Mealy FSM */
    view->fsm = (struct mealy_machine *)malloc(sizeof(struct mealy_machine));
    if (!view->fsm) {
        free(view);
        return NULL;
    }
    
    mealy_machine_init(view->fsm, (struct mealy_state *)&viper_view_idle_state);
    
    return view;
}

void viper_view_destroy(struct viper_view *view)
{
    if (view) {
        if (view->fsm) {
            free(view->fsm);
        }
        free(view);
    }
}

void viper_view_handle_click(struct viper_view *view)
{
    if (!view || !view->fsm) return;
    mealy_machine_event_click(view->fsm);
}

void viper_view_render(struct viper_view *view)
{
    if (!view || !view->fsm) return;
    
    struct mealy_state *current_state = mealy_machine_get_state(view->fsm);
    if (current_state && current_state->ops && current_state->ops->render) {
        current_state->ops->render(current_state, view->fsm);
    }
}

const char *viper_view_get_state_name(struct viper_view *view)
{
    if (!view || !view->fsm) return "NULL";
    
    struct mealy_state *current_state = mealy_machine_get_state(view->fsm);
    if (current_state == (struct mealy_state *)&viper_view_idle_state) {
        return "IDLE";
    } else if (current_state == (struct mealy_state *)&viper_view_loading_state) {
        return "LOADING";
    } else if (current_state == (struct mealy_state *)&viper_view_displaying_state) {
        return "DISPLAYING";
    } else if (current_state == (struct mealy_state *)&viper_view_error_state) {
        return "ERROR";
    }
    
    return "UNKNOWN";
}

void viper_view_set_error(struct viper_view *view, const char *error_message)
{
    if (!view || !view->fsm || !error_message) return;
    
    /* Set error message */
    strncpy(viper_view_error_state.error_message, error_message,
            sizeof(viper_view_error_state.error_message) - 1);
    viper_view_error_state.error_message[sizeof(viper_view_error_state.error_message) - 1] = '\0';
    
    /* Transition to error state */
    mealy_machine_set_state(view->fsm, (struct mealy_state *)&viper_view_error_state);
}

/* Missing functions implementation */

struct viper_view *viper_view_init(const char *view_name,
                                   int view_id,
                                   void (*present_callback)(const void *data, size_t size))
{
    struct viper_view *view = viper_view_create();
    if (!view) return NULL;
    
    if (view_name) {
        strncpy(view->view_name, view_name, sizeof(view->view_name) - 1);
        view->view_name[sizeof(view->view_name) - 1] = '\0';
    } else {
        strncpy(view->view_name, "UnnamedView", sizeof(view->view_name) - 1);
        view->view_name[sizeof(view->view_name) - 1] = '\0';
    }
    
    view->view_id = view_id;
    view->present_callback = present_callback;
    
    return view;
}

int viper_view_process_event(struct viper_view *view,
                             viper_view_event_t event,
                             const void *event_data)
{
    if (!view || !view->fsm) return -1;
    
    switch (event) {
        case VIEW_EVENT_USER_INPUT:
            /* Simulate a click */
            viper_view_handle_click(view);
            break;
        case VIEW_EVENT_REFRESH:
            /* Transition to loading */
            mealy_machine_set_state(view->fsm, (struct mealy_state *)&viper_view_loading_state);
            break;
        case VIEW_EVENT_ERROR_OCCURRED:
            if (event_data) {
                viper_view_set_error(view, (const char *)event_data);
            }
            break;
        case VIEW_EVENT_DATA_RECEIVED:
            /* Transition to displaying */
            mealy_machine_set_state(view->fsm, (struct mealy_state *)&viper_view_displaying_state);
            break;
        case VIEW_EVENT_NAVIGATE:
            /* Transition to idle */
            mealy_machine_set_state(view->fsm, (struct mealy_state *)&viper_view_idle_state);
            break;
        default:
            return -1;
    }
    
    return 0;
}

int viper_view_update(struct viper_view *view,
                      const void *data,
                      size_t data_size)
{
    if (!view) return -1;
    if (!data || data_size == 0) return -1;
    
    /* Store data in view's display buffer */
    if (view->data.display_buffer) {
        free(view->data.display_buffer);
    }
    view->data.display_buffer = malloc(data_size);
    if (!view->data.display_buffer) return -1;
    memcpy(view->data.display_buffer, data, data_size);
    view->data.buffer_size = data_size;
    
    /* Update state to displaying */
    mealy_machine_set_state(view->fsm, (struct mealy_state *)&viper_view_displaying_state);
    
    return 0;
}

viper_view_state_t viper_view_get_state(const struct viper_view *view)
{
    if (!view || !view->fsm) return VIEW_STATE_ERROR;
    
    const char *state_name = viper_view_get_state_name((struct viper_view *)view);
    if (strcmp(state_name, "IDLE") == 0) return VIEW_STATE_IDLE;
    if (strcmp(state_name, "LOADING") == 0) return VIEW_STATE_LOADING;
    if (strcmp(state_name, "DISPLAYING") == 0) return VIEW_STATE_DISPLAYING;
    if (strcmp(state_name, "ERROR") == 0) return VIEW_STATE_ERROR;
    
    return VIEW_STATE_UPDATING; /* fallback */
}

const struct viper_view_data *viper_view_get_data(const struct viper_view *view)
{
    if (!view) return NULL;
    return &view->data;
}
