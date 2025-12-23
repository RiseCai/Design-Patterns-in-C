/**
 * viper_entity.c - VIPER Entity Component Implementation
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
 * VIPER Entity component implemented as an Extended Finite State Machine (EFSM).
 * Manages data with extended variables and conditions.
 */

#include "../include/viper_entity.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* EFSM states for entity */
static struct efsm_state initial_state = {
    .process = NULL,
    .on_entry = NULL,
    .on_exit = NULL
};

static struct efsm_state loaded_state = {
    .process = NULL,
    .on_entry = NULL,
    .on_exit = NULL
};

static struct efsm_state modified_state = {
    .process = NULL,
    .on_entry = NULL,
    .on_exit = NULL
};

static struct efsm_state saving_state = {
    .process = NULL,
    .on_entry = NULL,
    .on_exit = NULL
};

static struct efsm_state error_state = {
    .process = NULL,
    .on_entry = NULL,
    .on_exit = NULL
};

static struct efsm_state valid_state = {
    .process = NULL,
    .on_entry = NULL,
    .on_exit = NULL
};

static struct efsm_state invalid_state = {
    .process = NULL,
    .on_entry = NULL,
    .on_exit = NULL
};

/* State entry actions */
static void initial_entry(struct efsm_context *ctx)
{
    printf("[Entity] Entering INITIAL state\n");
}

static void loaded_entry(struct efsm_context *ctx)
{
    printf("[Entity] Entering LOADED state\n");
}

static void modified_entry(struct efsm_context *ctx)
{
    printf("[Entity] Entering MODIFIED state\n");
}

static void saving_entry(struct efsm_context *ctx)
{
    printf("[Entity] Entering SAVING state\n");
}

static void error_entry(struct efsm_context *ctx)
{
    printf("[Entity] Entering ERROR state\n");
}

static void valid_entry(struct efsm_context *ctx)
{
    printf("[Entity] Entering VALID state\n");
}

static void invalid_entry(struct efsm_context *ctx)
{
    printf("[Entity] Entering INVALID state\n");
}

/* State process actions */
static void initial_process(struct efsm_context *ctx)
{
    printf("[Entity] Processing in INITIAL state\n");
}

static void loaded_process(struct efsm_context *ctx)
{
    printf("[Entity] Processing in LOADED state\n");
}

static void modified_process(struct efsm_context *ctx)
{
    printf("[Entity] Processing in MODIFIED state\n");
}

static void saving_process(struct efsm_context *ctx)
{
    printf("[Entity] Processing in SAVING state\n");
}

static void error_process(struct efsm_context *ctx)
{
    printf("[Entity] Processing in ERROR state\n");
}

static void valid_process(struct efsm_context *ctx)
{
    printf("[Entity] Processing in VALID state\n");
}

static void invalid_process(struct efsm_context *ctx)
{
    printf("[Entity] Processing in INVALID state\n");
}

/* Initialize states with actions */
static void init_states(void)
{
    static int initialized = 0;
    if (initialized) return;
    
    initial_state.on_entry = initial_entry;
    initial_state.process = initial_process;
    
    loaded_state.on_entry = loaded_entry;
    loaded_state.process = loaded_process;
    
    modified_state.on_entry = modified_entry;
    modified_state.process = modified_process;
    
    saving_state.on_entry = saving_entry;
    saving_state.process = saving_process;
    
    error_state.on_entry = error_entry;
    error_state.process = error_process;
    
    valid_state.on_entry = valid_entry;
    valid_state.process = valid_process;
    
    invalid_state.on_entry = invalid_entry;
    invalid_state.process = invalid_process;
    
    initialized = 1;
}

/* Helper function to map entity state to EFSM state */
static struct efsm_state *state_to_efsm(viper_entity_state_t state)
{
    switch (state) {
        case ENTITY_STATE_INITIAL: return &initial_state;
        case ENTITY_STATE_LOADED: return &loaded_state;
        case ENTITY_STATE_MODIFIED: return &modified_state;
        case ENTITY_STATE_SAVING: return &saving_state;
        case ENTITY_STATE_ERROR: return &error_state;
        case ENTITY_STATE_VALID: return &valid_state;
        case ENTITY_STATE_INVALID: return &invalid_state;
        default: return &initial_state;
    }
}

/* Helper function to map EFSM state to entity state */
static viper_entity_state_t efsm_to_state(struct efsm_state *state)
{
    if (state == &initial_state) return ENTITY_STATE_INITIAL;
    if (state == &loaded_state) return ENTITY_STATE_LOADED;
    if (state == &modified_state) return ENTITY_STATE_MODIFIED;
    if (state == &saving_state) return ENTITY_STATE_SAVING;
    if (state == &error_state) return ENTITY_STATE_ERROR;
    if (state == &valid_state) return ENTITY_STATE_VALID;
    if (state == &invalid_state) return ENTITY_STATE_INVALID;
    return ENTITY_STATE_INITIAL;
}

/* VIPER Entity implementation */
struct viper_entity *viper_entity_init(const char *entity_name,
                                       int entity_id)
{
    struct viper_entity *entity = (struct viper_entity *)malloc(sizeof(struct viper_entity));
    if (!entity) return NULL;
    
    memset(entity, 0, sizeof(struct viper_entity));
    
    /* Initialize entity data */
    entity->data.data_buffer = NULL;
    entity->data.data_size = 0;
    strcpy(entity->data.data_type, "unknown");
    entity->data.version = 1;
    entity->data.last_modified = time(NULL);
    entity->data.is_valid = 0;
    entity->data.is_dirty = 0;
    entity->data.is_persisted = 0;
    entity->data.validation_score = 0;
    entity->data.validation_errors[0] = '\0';
    
    /* Set name and ID */
    if (entity_name) {
        strncpy(entity->entity_name, entity_name, sizeof(entity->entity_name) - 1);
        entity->entity_name[sizeof(entity->entity_name) - 1] = '\0';
    } else {
        strcpy(entity->entity_name, "VIPER_Entity");
    }
    entity->entity_id = entity_id;
    
    /* Initialize EFSM */
    init_states();
    entity->fsm = (struct efsm_processor *)malloc(sizeof(struct efsm_processor));
    if (!entity->fsm) {
        free(entity);
        return NULL;
    }
    
    struct efsm_processor *proc = entity->fsm;
    efsm_processor_init(proc);
    proc->current_state = &initial_state;
    
    printf("[Entity] Initialized '%s' (ID: %d)\n", entity->entity_name, entity->entity_id);
    return entity;
}

void viper_entity_destroy(struct viper_entity *entity)
{
    if (!entity) return;
    
    /* Clean up data buffer */
    if (entity->data.data_buffer) {
        free(entity->data.data_buffer);
    }
    
    /* Destroy EFSM */
    if (entity->fsm) {
        free(entity->fsm);
    }
    
    free(entity);
    printf("[Entity] Destroyed\n");
}

int viper_entity_load_data(struct viper_entity *entity,
                           const char *source,
                           const void *source_data)
{
    if (!entity || !entity->fsm) return -1;
    
    /* Simulate loading data */
    printf("[Entity] Loading data from source: %s\n", source ? source : "unknown");
    
    /* Update entity state */
    struct efsm_processor *proc = (struct efsm_processor *)entity->fsm;
    proc->current_state = &loaded_state;
    
    /* Call entry action */
    if (loaded_state.on_entry) {
        loaded_state.on_entry(&proc->context);
    }
    
    /* Update entity data */
    entity->data.is_dirty = 0;
    entity->data.is_persisted = 1;
    entity->data.last_modified = time(NULL);
    
    printf("[Entity] Data loaded successfully\n");
    return 0;
}

int viper_entity_update_data(struct viper_entity *entity,
                             const void *new_data,
                             size_t data_size)
{
    if (!entity) return -1;
    
    /* If no data provided, treat as no-op (e.g., delete event) */
    if (!new_data || data_size == 0) {
        printf("[Entity] Update with no data - no operation\n");
        return 0;
    }
    
    /* Update data buffer */
    if (entity->data.data_buffer) {
        free(entity->data.data_buffer);
    }
    
    entity->data.data_buffer = malloc(data_size);
    if (!entity->data.data_buffer) return -1;
    
    memcpy(entity->data.data_buffer, new_data, data_size);
    entity->data.data_size = data_size;
    
    /* Update entity state */
    struct efsm_processor *proc = (struct efsm_processor *)entity->fsm;
    proc->current_state = &modified_state;
    
    /* Call entry action */
    if (modified_state.on_entry) {
        modified_state.on_entry(&proc->context);
    }
    
    /* Update entity data */
    entity->data.is_dirty = 1;
    entity->data.is_persisted = 0;
    entity->data.last_modified = time(NULL);
    
    printf("[Entity] Data updated (%zu bytes)\n", data_size);
    return 0;
}

int viper_entity_validate_data(struct viper_entity *entity,
                               const void *validation_rules,
                               size_t rules_size)
{
    if (!entity || !entity->fsm) return -1;
    
    printf("[Entity] Validating data with rules (%zu bytes)\n", rules_size);
    
    /* Simple validation logic */
    int score = 0;
    char errors[256] = {0};
    
    if (entity->data.data_buffer && entity->data.data_size > 0) {
        score = 85; /* Simulated validation score */
        strcpy(errors, "No critical errors");
    } else {
        score = 0;
        strcpy(errors, "No data to validate");
    }
    
    /* Update entity state based on validation result */
    struct efsm_processor *proc = (struct efsm_processor *)entity->fsm;
    if (score >= 70) {
        proc->current_state = &valid_state;
        if (valid_state.on_entry) {
            valid_state.on_entry(&proc->context);
        }
        entity->data.is_valid = 1;
    } else {
        proc->current_state = &invalid_state;
        if (invalid_state.on_entry) {
            invalid_state.on_entry(&proc->context);
        }
        entity->data.is_valid = 0;
    }
    
    /* Update validation data */
    entity->data.validation_score = score;
    strncpy(entity->data.validation_errors, errors, sizeof(entity->data.validation_errors) - 1);
    entity->data.validation_errors[sizeof(entity->data.validation_errors) - 1] = '\0';
    
    printf("[Entity] Validation complete: score=%d, errors=%s\n", score, errors);
    return score;
}

int viper_entity_save_data(struct viper_entity *entity,
                           const char *destination,
                           const void *destination_data)
{
    if (!entity || !entity->fsm) return -1;
    
    printf("[Entity] Saving data to destination: %s\n", destination ? destination : "unknown");
    
    /* Update entity state */
    struct efsm_processor *proc = (struct efsm_processor *)entity->fsm;
    proc->current_state = &saving_state;
    
    /* Call entry action */
    if (saving_state.on_entry) {
        saving_state.on_entry(&proc->context);
    }
    
    /* Simulate save operation */
    if (entity->data.data_buffer && entity->data.data_size > 0) {
        printf("[Entity] Saved %zu bytes of data\n", entity->data.data_size);
        entity->data.is_dirty = 0;
        entity->data.is_persisted = 1;
        entity->data.last_modified = time(NULL);
        
        /* Return to loaded state after saving */
        proc->current_state = &loaded_state;
        if (loaded_state.on_entry) {
            loaded_state.on_entry(&proc->context);
        }
    } else {
        printf("[Entity] No data to save\n");
        proc->current_state = &error_state;
        if (error_state.on_entry) {
            error_state.on_entry(&proc->context);
        }
        return -1;
    }
    
    printf("[Entity] Data saved successfully\n");
    return 0;
}

int viper_entity_query_data(struct viper_entity *entity,
                            const void *query,
                            size_t query_size,
                            void **result,
                            size_t *result_size)
{
    if (!entity || !result || !result_size) return -1;
    
    printf("[Entity] Querying data (%zu bytes query)\n", query_size);
    
    /* Simple query implementation - just return the data */
    if (entity->data.data_buffer && entity->data.data_size > 0) {
        *result = malloc(entity->data.data_size);
        if (!*result) return -1;
        
        memcpy(*result, entity->data.data_buffer, entity->data.data_size);
        *result_size = entity->data.data_size;
        
        printf("[Entity] Query returned %zu bytes\n", *result_size);
        return 0;
    }
    
    printf("[Entity] No data available for query\n");
    *result = NULL;
    *result_size = 0;
    return -1;
}

viper_entity_state_t viper_entity_get_state(const struct viper_entity *entity)
{
    if (!entity || !entity->fsm) return ENTITY_STATE_ERROR;
    
    struct efsm_processor *proc = (struct efsm_processor *)entity->fsm;
    return efsm_to_state(proc->current_state);
}

const struct viper_entity_data *viper_entity_get_data(const struct viper_entity *entity)
{
    if (!entity) return NULL;
    return &entity->data;
}

int viper_entity_process_event(struct viper_entity *entity,
                               viper_entity_event_t event,
                               const void *data)
{
    if (!entity || !entity->fsm) return -1;

    printf("[Entity] Processing event: %d\n", event);

    switch (event) {
        case ENTITY_EVENT_LOAD_DATA:
            return viper_entity_load_data(entity, "event", data);
        case ENTITY_EVENT_UPDATE_DATA:
            /* data should be a pointer to a struct containing new_data and size */
            /* For simplicity, assume data points to a buffer and size is known */
            /* In real implementation, we would need to parse data */
            return viper_entity_update_data(entity, data, 0); /* size unknown */
        case ENTITY_EVENT_VALIDATE_DATA:
            return viper_entity_validate_data(entity, data, 0);
        case ENTITY_EVENT_SAVE_DATA:
            return viper_entity_save_data(entity, "event", data);
        case ENTITY_EVENT_DELETE_DATA:
            /* Not implemented yet */
            printf("[Entity] DELETE_DATA event not implemented\n");
            return -1;
        case ENTITY_EVENT_QUERY_DATA:
            /* Not implemented yet */
            printf("[Entity] QUERY_DATA event not implemented\n");
            return -1;
        default:
            printf("[Entity] Unknown event: %d\n", event);
            return -1;
    }
}
