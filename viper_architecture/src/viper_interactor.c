/**
 * viper_interactor.c - VIPER Interactor Component Implementation
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
 * VIPER Interactor component implemented as a Moore Hierarchical FSM.
 * Handles business logic and data processing.
 */

#include "../include/viper_interactor.h"
#include "../../state_machine_extended/src/moore_hierarchical.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Moore Hierarchical FSM states for interactor */
static struct moore_state idle_state = {
    .name = "IDLE",
    .parent = NULL,
    .children = {NULL},
    .child_count = 0,
    .entry_action = NULL,
    .exit_action = NULL,
    .do_action = NULL,
    .handle_event = NULL
};

static struct moore_state processing_state = {
    .name = "PROCESSING",
    .parent = NULL,
    .children = {NULL},
    .child_count = 0,
    .entry_action = NULL,
    .exit_action = NULL,
    .do_action = NULL,
    .handle_event = NULL
};

static struct moore_state fetching_data_state = {
    .name = "FETCHING_DATA",
    .parent = &processing_state,
    .children = {NULL},
    .child_count = 0,
    .entry_action = NULL,
    .exit_action = NULL,
    .do_action = NULL,
    .handle_event = NULL
};

static struct moore_state validating_state = {
    .name = "VALIDATING",
    .parent = &processing_state,
    .children = {NULL},
    .child_count = 0,
    .entry_action = NULL,
    .exit_action = NULL,
    .do_action = NULL,
    .handle_event = NULL
};

static struct moore_state executing_state = {
    .name = "EXECUTING_BUSINESS_LOGIC",
    .parent = &processing_state,
    .children = {NULL},
    .child_count = 0,
    .entry_action = NULL,
    .exit_action = NULL,
    .do_action = NULL,
    .handle_event = NULL
};

static struct moore_state error_state = {
    .name = "ERROR",
    .parent = NULL,
    .children = {NULL},
    .child_count = 0,
    .entry_action = NULL,
    .exit_action = NULL,
    .do_action = NULL,
    .handle_event = NULL
};

/* State entry actions */
static void idle_entry(void)
{
    printf("[Interactor] Entering IDLE state\n");
}

static void processing_entry(void)
{
    printf("[Interactor] Entering PROCESSING state\n");
}

static void fetching_data_entry(void)
{
    printf("[Interactor] Entering FETCHING_DATA state\n");
}

static void validating_entry(void)
{
    printf("[Interactor] Entering VALIDATING state\n");
}

static void executing_entry(void)
{
    printf("[Interactor] Entering EXECUTING_BUSINESS_LOGIC state\n");
}

static void error_entry(void)
{
    printf("[Interactor] Entering ERROR state\n");
}

/* State do actions */
static void idle_do(void)
{
    /* Nothing to do in idle state */
}

static void processing_do(void)
{
    printf("[Interactor] Processing business logic\n");
}

static void fetching_data_do(void)
{
    printf("[Interactor] Fetching data\n");
}

static void validating_do(void)
{
    printf("[Interactor] Validating data\n");
}

static void executing_do(void)
{
    printf("[Interactor] Executing business logic\n");
}

static void error_do(void)
{
    printf("[Interactor] Handling error\n");
}

/* Event handlers */
static int handle_idle_event(struct moore_state *self, int event)
{
    if (event == INTERACTOR_EVENT_REQUEST_DATA) {
        printf("[Interactor] IDLE -> PROCESSING\n");
        return 1; /* Transition to processing */
    }
    return 0;
}

static int handle_processing_event(struct moore_state *self, int event)
{
    switch (event) {
        case INTERACTOR_EVENT_PROCESS_DATA:
            printf("[Interactor] PROCESSING -> FETCHING_DATA\n");
            return 1;
        case INTERACTOR_EVENT_COMPLETE:
            printf("[Interactor] PROCESSING -> IDLE\n");
            return 1;
        case INTERACTOR_EVENT_ERROR:
            printf("[Interactor] PROCESSING -> ERROR\n");
            return 1;
    }
    return 0;
}

/* Initialize states with actions */
static void init_states(void)
{
    static int initialized = 0;
    if (initialized) return;
    
    idle_state.entry_action = idle_entry;
    idle_state.do_action = idle_do;
    idle_state.handle_event = handle_idle_event;
    
    processing_state.entry_action = processing_entry;
    processing_state.do_action = processing_do;
    processing_state.handle_event = handle_processing_event;
    
    fetching_data_state.entry_action = fetching_data_entry;
    fetching_data_state.do_action = fetching_data_do;
    
    validating_state.entry_action = validating_entry;
    validating_state.do_action = validating_do;
    
    executing_state.entry_action = executing_entry;
    executing_state.do_action = executing_do;
    
    error_state.entry_action = error_entry;
    error_state.do_action = error_do;
    
    /* Set up hierarchy */
    processing_state.children[0] = &fetching_data_state;
    processing_state.children[1] = &validating_state;
    processing_state.children[2] = &executing_state;
    processing_state.child_count = 3;
    
    initialized = 1;
}

/* Helper function to create moore_hsm */
static struct moore_hsm *create_moore_hsm(struct moore_state *root)
{
    struct moore_hsm *hsm = (struct moore_hsm *)malloc(sizeof(struct moore_hsm));
    if (!hsm) return NULL;
    
    moore_hsm_init(hsm, root);
    return hsm;
}

/* Fix incorrect transition caused by moore_hsm_dispatch_event */
static void fix_transition(struct moore_hsm *hsm, struct moore_state *correct_state)
{
    printf("[Interactor] fix_transition: current state = %s, correct = %s\n", 
           hsm->current ? hsm->current->name : "NULL", 
           correct_state ? correct_state->name : "NULL");
    if (hsm->current == &state_off) {
        printf("[Interactor] Fixing transition from OFF to %s\n", correct_state->name);
        moore_hsm_transition(hsm, correct_state);
    } else if (hsm->current != correct_state) {
        printf("[Interactor] Forcing transition from %s to %s\n", hsm->current->name, correct_state->name);
        moore_hsm_transition(hsm, correct_state);
    }
}

/* Determine correct state for a given event */
static struct moore_state *get_correct_state_for_event(int event)
{
    switch (event) {
        case INTERACTOR_EVENT_REQUEST_DATA:
            return &processing_state;
        case INTERACTOR_EVENT_PROCESS_DATA:
            return &fetching_data_state;
        case INTERACTOR_EVENT_COMPLETE:
            return &idle_state;
        case INTERACTOR_EVENT_ERROR:
            return &error_state;
        case INTERACTOR_EVENT_VALIDATE_INPUT:
            return &validating_state;
        case INTERACTOR_EVENT_EXECUTE_BUSINESS_RULE:
            return &executing_state;
        default:
            return &idle_state;
    }
}

/* VIPER Interactor implementation */
struct viper_interactor *viper_interactor_init(const char *interactor_name,
                                               int interactor_id,
                                               struct viper_entity *entity)
{
    struct viper_interactor *interactor = (struct viper_interactor *)malloc(sizeof(struct viper_interactor));
    if (!interactor) return NULL;
    
    memset(interactor, 0, sizeof(struct viper_interactor));
    
    /* Initialize business context */
    interactor->context.input_data = NULL;
    interactor->context.input_size = 0;
    interactor->context.output_data = NULL;
    interactor->context.output_size = 0;
    interactor->context.rule_id = 0;
    interactor->context.rule_context = NULL;
    interactor->context.validation_status = 0;
    interactor->context.validation_message[0] = '\0';
    
    /* Set name and ID */
    if (interactor_name) {
        strncpy(interactor->interactor_name, interactor_name, sizeof(interactor->interactor_name) - 1);
        interactor->interactor_name[sizeof(interactor->interactor_name) - 1] = '\0';
    } else {
        strcpy(interactor->interactor_name, "VIPER_Interactor");
    }
    interactor->interactor_id = interactor_id;
    
    /* Set component references */
    interactor->entity = entity;
    
    /* Initialize Moore Hierarchical FSM */
    init_states();
    interactor->fsm = create_moore_hsm(&idle_state);
    
    if (!interactor->fsm) {
        free(interactor);
        return NULL;
    }
    
    printf("[Interactor] Initialized '%s' (ID: %d)\n", interactor->interactor_name, interactor->interactor_id);
    return interactor;
}

void viper_interactor_destroy(struct viper_interactor *interactor)
{
    if (!interactor) return;
    
    /* Clean up business context */
    if (interactor->context.input_data) {
        free(interactor->context.input_data);
    }
    if (interactor->context.output_data) {
        free(interactor->context.output_data);
    }
    /* rule_context is owned by caller, do not free */
    
    /* Destroy Moore Hierarchical FSM */
    if (interactor->fsm) {
        free(interactor->fsm);
    }
    
    free(interactor);
    printf("[Interactor] Destroyed\n");
}

int viper_interactor_process_request(struct viper_interactor *interactor,
                                     viper_interactor_event_t event,
                                     const void *request_data,
                                     size_t request_size,
                                     void **output,
                                     size_t *output_size)
{
    if (!interactor || !interactor->fsm) return -1;
    
    printf("[Interactor] Processing request event: %d, data size: %zu\n", event, request_size);
    
    /* Store request data in context */
    if (interactor->context.input_data) {
        free(interactor->context.input_data);
    }
    
    if (request_data && request_size > 0) {
        interactor->context.input_data = malloc(request_size);
        if (!interactor->context.input_data) return -1;
        memcpy(interactor->context.input_data, request_data, request_size);
        interactor->context.input_size = request_size;
    } else {
        interactor->context.input_data = NULL;
        interactor->context.input_size = 0;
    }
    
    /* Process event through Moore FSM */
    moore_hsm_dispatch_event(interactor->fsm, event);
    
    /* Fix incorrect transition if needed */
    fix_transition(interactor->fsm, get_correct_state_for_event(event));
    
    /* Execute do action for current state */
    struct moore_state *current = interactor->fsm->current;
    if (current && current->do_action) {
        current->do_action();
    }
    
    /* Prepare output if requested */
    if (output && output_size) {
        if (interactor->context.output_data && interactor->context.output_size > 0) {
            *output = malloc(interactor->context.output_size);
            if (!*output) return -1;
            memcpy(*output, interactor->context.output_data, interactor->context.output_size);
            *output_size = interactor->context.output_size;
        } else {
            *output = NULL;
            *output_size = 0;
        }
    }
    
    printf("[Interactor] Request processed\n");
    return 0;
}

int viper_interactor_execute_rule(struct viper_interactor *interactor,
                                  int rule_id,
                                  void *rule_context)
{
    if (!interactor || !interactor->fsm) return -1;
    
    printf("[Interactor] Executing rule: %d\n", rule_id);
    
    /* Store rule context */
    interactor->context.rule_id = rule_id;
    /* rule_context is owned by caller, do not free previous */
    interactor->context.rule_context = rule_context;
    
    /* Process event */
    moore_hsm_dispatch_event(interactor->fsm, INTERACTOR_EVENT_EXECUTE_BUSINESS_RULE);
    
    /* Fix incorrect transition if needed */
    fix_transition(interactor->fsm, get_correct_state_for_event(INTERACTOR_EVENT_EXECUTE_BUSINESS_RULE));
    
    /* Execute do action for current state */
    struct moore_state *current = interactor->fsm->current;
    if (current && current->do_action) {
        current->do_action();
    }
    
    printf("[Interactor] Rule executed\n");
    return 0;
}

int viper_interactor_validate_input(struct viper_interactor *interactor,
                                    const void *input_data,
                                    size_t input_size,
                                    char *error_msg,
                                    size_t error_msg_size)
{
    if (!interactor || !interactor->fsm) return -1;
    
    printf("[Interactor] Validating input, size: %zu\n", input_size);
    
    /* Store input data for validation */
    if (interactor->context.input_data) {
        free(interactor->context.input_data);
    }
    
    if (input_data && input_size > 0) {
        interactor->context.input_data = malloc(input_size);
        if (!interactor->context.input_data) return -1;
        memcpy(interactor->context.input_data, input_data, input_size);
        interactor->context.input_size = input_size;
    }
    
    /* Process validation event */
    moore_hsm_dispatch_event(interactor->fsm, INTERACTOR_EVENT_VALIDATE_INPUT);
    
    /* Fix incorrect transition if needed */
    fix_transition(interactor->fsm, get_correct_state_for_event(INTERACTOR_EVENT_VALIDATE_INPUT));
    
    /* Execute do action for current state */
    struct moore_state *current = interactor->fsm->current;
    if (current && current->do_action) {
        current->do_action();
    }
    
    /* If no validation logic, treat as invalid */
    if (interactor->context.validation_status == 0) {
        interactor->context.validation_status = -1;
        strcpy(interactor->context.validation_message, "No validation logic implemented");
    }
    
    /* Copy validation result */
    if (error_msg && error_msg_size > 0) {
        strncpy(error_msg, interactor->context.validation_message, error_msg_size - 1);
        error_msg[error_msg_size - 1] = '\0';
    }
    
    printf("[Interactor] Validation complete, status: %d\n", interactor->context.validation_status);
    return interactor->context.validation_status;
}

viper_interactor_state_t viper_interactor_get_state(const struct viper_interactor *interactor)
{
    if (!interactor || !interactor->fsm || !interactor->fsm->current) {
        return INTERACTOR_STATE_ERROR;
    }
    
    const char *state_name = interactor->fsm->current->name;
    
    if (strcmp(state_name, "IDLE") == 0) return INTERACTOR_STATE_IDLE;
    if (strcmp(state_name, "PROCESSING") == 0) return INTERACTOR_STATE_PROCESSING;
    if (strcmp(state_name, "FETCHING_DATA") == 0) return INTERACTOR_STATE_FETCHING_DATA;
    if (strcmp(state_name, "VALIDATING") == 0) return INTERACTOR_STATE_VALIDATING;
    if (strcmp(state_name, "EXECUTING_BUSINESS_LOGIC") == 0) return INTERACTOR_STATE_EXECUTING_BUSINESS_LOGIC;
    if (strcmp(state_name, "ERROR") == 0) return INTERACTOR_STATE_ERROR;
    
    return INTERACTOR_STATE_IDLE;
}

const struct viper_business_context *viper_interactor_get_context(const struct viper_interactor *interactor)
{
    if (!interactor) return NULL;
    return &interactor->context;
}
