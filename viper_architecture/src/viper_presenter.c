/**
 * viper_presenter.c - VIPER Presenter Component Implementation
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
 * VIPER Presenter component implemented as a Parallel FSM.
 * Coordinates multiple components and manages data flow.
 */

#include "../include/viper_presenter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Presenter component data */
struct presenter_component_data {
    int id;
    viper_presenter_state_t state;
    void *data;
};

/* Presenter component step function */
static void presenter_component_step(struct parallel_component *comp, int event)
{
    struct presenter_component_data *data = (struct presenter_component_data *)comp->state;
    printf("[Presenter Component %d] Step with event: %d, current state: %d\n", 
           data->id, event, data->state);
    
    /* Simple state transitions based on events */
    switch (event) {
        case EVENT_START:
            data->state = PRESENTER_STATE_PROCESSING;
            break;
        case EVENT_DATA_READY:
            data->state = PRESENTER_STATE_UPDATING_VIEW;
            break;
        case EVENT_FINISH:
            data->state = PRESENTER_STATE_IDLE;
            break;
        case EVENT_RESET:
            data->state = PRESENTER_STATE_IDLE;
            break;
        default:
            break;
    }
}

/* Presenter component entry function */
static void presenter_component_entry(struct parallel_component *comp)
{
    struct presenter_component_data *data = (struct presenter_component_data *)comp->state;
    printf("[Presenter Component %d] Entry, state: %d\n", data->id, data->state);
}

/* Presenter component do function */
static void presenter_component_do(struct parallel_component *comp)
{
    struct presenter_component_data *data = (struct presenter_component_data *)comp->state;
    printf("[Presenter Component %d] Do action, state: %d\n", data->id, data->state);
}

/* Presenter component exit function */
static void presenter_component_exit(struct parallel_component *comp)
{
    struct presenter_component_data *data = (struct presenter_component_data *)comp->state;
    printf("[Presenter Component %d] Exit, state: %d\n", data->id, data->state);
}

/* VIPER Presenter implementation */
struct viper_presenter *viper_presenter_init(const char *presenter_name,
                                             int presenter_id,
                                             struct viper_view *view,
                                             struct viper_interactor *interactor)
{
    struct viper_presenter *presenter = (struct viper_presenter *)malloc(sizeof(struct viper_presenter));
    if (!presenter) return NULL;
    
    memset(presenter, 0, sizeof(struct viper_presenter));
    
    /* Initialize presenter data */
    presenter->data.current_state = PRESENTER_STATE_IDLE;
    presenter->data.presentation_data = NULL;
    presenter->data.data_size = 0;
    presenter->data.view_context = NULL;
    presenter->data.interactor_context = NULL;
    presenter->data.error_code = 0;
    presenter->data.error_message[0] = '\0';
    
    /* Set name and ID */
    if (presenter_name) {
        strncpy(presenter->presenter_name, presenter_name, sizeof(presenter->presenter_name) - 1);
        presenter->presenter_name[sizeof(presenter->presenter_name) - 1] = '\0';
    } else {
        strcpy(presenter->presenter_name, "VIPER_Presenter");
    }
    presenter->presenter_id = presenter_id;
    
    /* Set component references */
    presenter->view = view;
    presenter->interactor = interactor;
    
    /* Initialize Parallel FSM (parallel machine) */
    presenter->fsm = (struct parallel_machine *)malloc(sizeof(struct parallel_machine));
    if (!presenter->fsm) {
        free(presenter);
        return NULL;
    }
    
    parallel_machine_init(presenter->fsm, 3); /* 3 components: view, interactor, router coordination */
    
    /* Create presenter components */
    struct presenter_component_data *comp1_data = malloc(sizeof(struct presenter_component_data));
    struct presenter_component_data *comp2_data = malloc(sizeof(struct presenter_component_data));
    struct presenter_component_data *comp3_data = malloc(sizeof(struct presenter_component_data));
    
    if (!comp1_data || !comp2_data || !comp3_data) {
        free(comp1_data);
        free(comp2_data);
        free(comp3_data);
        free(presenter->fsm);
        free(presenter);
        return NULL;
    }
    
    comp1_data->id = 1;
    comp1_data->state = PRESENTER_STATE_IDLE;
    comp1_data->data = NULL;
    
    comp2_data->id = 2;
    comp2_data->state = PRESENTER_STATE_IDLE;
    comp2_data->data = NULL;
    
    comp3_data->id = 3;
    comp3_data->state = PRESENTER_STATE_IDLE;
    comp3_data->data = NULL;
    
    struct parallel_component *comp1 = parallel_component_create(
        1, comp1_data, presenter_component_step, 
        presenter_component_entry, presenter_component_do, presenter_component_exit);
    
    struct parallel_component *comp2 = parallel_component_create(
        2, comp2_data, presenter_component_step,
        presenter_component_entry, presenter_component_do, presenter_component_exit);
    
    struct parallel_component *comp3 = parallel_component_create(
        3, comp3_data, presenter_component_step,
        presenter_component_entry, presenter_component_do, presenter_component_exit);
    
    if (!comp1 || !comp2 || !comp3) {
        free(comp1_data);
        free(comp2_data);
        free(comp3_data);
        free(presenter->fsm);
        free(presenter);
        return NULL;
    }
    
    parallel_machine_add_component(presenter->fsm, comp1);
    parallel_machine_add_component(presenter->fsm, comp2);
    parallel_machine_add_component(presenter->fsm, comp3);
    
    printf("[Presenter] Initialized '%s' (ID: %d) with 3 parallel components\n", 
           presenter->presenter_name, presenter->presenter_id);
    return presenter;
}

void viper_presenter_destroy(struct viper_presenter *presenter)
{
    if (!presenter) return;
    
    /* Clean up presentation data */
    if (presenter->data.presentation_data) {
        free(presenter->data.presentation_data);
    }
    
    /* Destroy Parallel FSM components */
    if (presenter->fsm) {
        struct parallel_machine *pm = (struct parallel_machine *)presenter->fsm;
        /* Note: parallel_machine doesn't have a destroy function in the current API,
           so we just free the memory */
        free(pm);
    }
    
    free(presenter);
    printf("[Presenter] Destroyed\n");
}

int viper_presenter_process_event(struct viper_presenter *presenter,
                                  viper_presenter_event_t event,
                                  const void *event_data)
{
    if (!presenter || !presenter->fsm) return -1;
    
    /* Map VIPER presenter events to parallel FSM events */
    int parallel_event;
    switch (event) {
        case PRESENTER_EVENT_VIEW_UPDATE:
            parallel_event = EVENT_START;
            break;
        case PRESENTER_EVENT_DATA_READY:
            parallel_event = EVENT_DATA_READY;
            break;
        case PRESENTER_EVENT_ERROR:
            parallel_event = EVENT_RESET;
            break;
        case PRESENTER_EVENT_NAVIGATION_REQUEST:
            parallel_event = EVENT_ENABLE;
            break;
        case PRESENTER_EVENT_BUSINESS_LOGIC_COMPLETE:
            parallel_event = EVENT_FINISH;
            break;
        default:
            printf("[Presenter] Unknown event: %d\n", event);
            return -1;
    }
    
    /* Broadcast event to all components */
    struct parallel_machine *pm = (struct parallel_machine *)presenter->fsm;
    parallel_machine_broadcast_event(pm, parallel_event);
    
    /* Update presenter state based on event */
    switch (event) {
        case PRESENTER_EVENT_VIEW_UPDATE:
            presenter->data.current_state = PRESENTER_STATE_PROCESSING;
            break;
        case PRESENTER_EVENT_DATA_READY:
            presenter->data.current_state = PRESENTER_STATE_UPDATING_VIEW;
            break;
        case PRESENTER_EVENT_ERROR:
            presenter->data.current_state = PRESENTER_STATE_ERROR;
            if (event_data) {
                strncpy(presenter->data.error_message, (const char *)event_data, 
                        sizeof(presenter->data.error_message) - 1);
                presenter->data.error_message[sizeof(presenter->data.error_message) - 1] = '\0';
            }
            break;
        default:
            break;
    }
    
    printf("[Presenter] Processed event %d, new state: %d\n", event, presenter->data.current_state);
    return 0;
}

int viper_presenter_update_view(struct viper_presenter *presenter,
                                const void *data,
                                size_t data_size)
{
    if (!presenter || !data || data_size == 0) return -1;
    
    /* Update presentation data */
    if (presenter->data.presentation_data) {
        free(presenter->data.presentation_data);
    }
    
    presenter->data.presentation_data = malloc(data_size);
    if (!presenter->data.presentation_data) return -1;
    
    memcpy(presenter->data.presentation_data, data, data_size);
    presenter->data.data_size = data_size;
    
    /* Trigger view update */
    if (presenter->view_update_callback && presenter->data.view_context) {
        presenter->view_update_callback(presenter->data.view_context, data, data_size);
    }
    
    printf("[Presenter] Updated view with %zu bytes of data\n", data_size);
    return 0;
}

int viper_presenter_request_data(struct viper_presenter *presenter,
                                 const void *request,
                                 size_t request_size)
{
    if (!presenter || !request || request_size == 0) return -1;
    
    /* Trigger interactor request */
    if (presenter->interactor_request_callback && presenter->data.interactor_context) {
        presenter->interactor_request_callback(presenter->data.interactor_context, request);
    }
    
    printf("[Presenter] Requested data from interactor (%zu bytes)\n", request_size);
    return 0;
}

viper_presenter_state_t viper_presenter_get_state(const struct viper_presenter *presenter)
{
    if (!presenter) return PRESENTER_STATE_ERROR;
    return presenter->data.current_state;
}

const struct viper_presenter_data *viper_presenter_get_data(const struct viper_presenter *presenter)
{
    if (!presenter) return NULL;
    return &presenter->data;
}
