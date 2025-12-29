/**
 * scv_interactor.c - System Coordinator VIPER Interactor Component
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * Interactor component implemented as a Moore Hierarchical State Machine.
 * Handles business logic, rules, and decision-making.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "scv_interactor.h"
#include "../../state_machine_extended/src/moore_hierarchical.h"

/* Internal helper functions */
static void interactor_state_init_do_action(void);
static void interactor_state_init_entry_action(void);
static void interactor_state_init_exit_action(void);

static void interactor_state_idle_do_action(void);
static void interactor_state_idle_entry_action(void);
static void interactor_state_idle_exit_action(void);

static void interactor_state_recording_do_action(void);
static void interactor_state_recording_entry_action(void);
static void interactor_state_recording_exit_action(void);

static void interactor_state_uploading_do_action(void);
static void interactor_state_uploading_entry_action(void);
static void interactor_state_uploading_exit_action(void);

static void interactor_state_ota_do_action(void);
static void interactor_state_ota_entry_action(void);
static void interactor_state_ota_exit_action(void);

static void interactor_state_error_do_action(void);
static void interactor_state_error_entry_action(void);
static void interactor_state_error_exit_action(void);

static void interactor_state_sleep_do_action(void);
static void interactor_state_sleep_entry_action(void);
static void interactor_state_sleep_exit_action(void);

/* State objects */
static struct moore_state interactor_state_init = {
    .name = "INIT",
    .parent = NULL,
    .child_count = 0,
    .do_action = interactor_state_init_do_action,
    .entry_action = interactor_state_init_entry_action,
    .exit_action = interactor_state_init_exit_action,
    .handle_event = NULL,
};

static struct moore_state interactor_state_idle = {
    .name = "IDLE",
    .parent = NULL,
    .child_count = 0,
    .do_action = interactor_state_idle_do_action,
    .entry_action = interactor_state_idle_entry_action,
    .exit_action = interactor_state_idle_exit_action,
    .handle_event = NULL,
};

static struct moore_state interactor_state_recording = {
    .name = "RECORDING",
    .parent = NULL,
    .child_count = 0,
    .do_action = interactor_state_recording_do_action,
    .entry_action = interactor_state_recording_entry_action,
    .exit_action = interactor_state_recording_exit_action,
    .handle_event = NULL,
};

static struct moore_state interactor_state_uploading = {
    .name = "UPLOADING",
    .parent = NULL,
    .child_count = 0,
    .do_action = interactor_state_uploading_do_action,
    .entry_action = interactor_state_uploading_entry_action,
    .exit_action = interactor_state_uploading_exit_action,
    .handle_event = NULL,
};

static struct moore_state interactor_state_ota = {
    .name = "OTA",
    .parent = NULL,
    .child_count = 0,
    .do_action = interactor_state_ota_do_action,
    .entry_action = interactor_state_ota_entry_action,
    .exit_action = interactor_state_ota_exit_action,
    .handle_event = NULL,
};

static struct moore_state interactor_state_error = {
    .name = "ERROR",
    .parent = NULL,
    .child_count = 0,
    .do_action = interactor_state_error_do_action,
    .entry_action = interactor_state_error_entry_action,
    .exit_action = interactor_state_error_exit_action,
    .handle_event = NULL,
};

static struct moore_state interactor_state_sleep = {
    .name = "SLEEP",
    .parent = NULL,
    .child_count = 0,
    .do_action = interactor_state_sleep_do_action,
    .entry_action = interactor_state_sleep_entry_action,
    .exit_action = interactor_state_sleep_exit_action,
    .handle_event = NULL,
};

/* Interactor-specific context extension */
struct scv_interactor_context {
    struct scv_interactor *interactor; /* back pointer */
};

/* Shared static variable for current interactor */
static struct scv_interactor *current_interactor_global = NULL;

/* Internal helper to get interactor from context */
static struct scv_interactor *get_interactor_from_context(void) {
    return current_interactor_global;
}

/* Internal helper to set current interactor */
static void set_current_interactor(struct scv_interactor *interactor) {
    current_interactor_global = interactor;
}

/* Internal helper to change state */
static void interactor_change_state(struct scv_interactor *interactor, struct moore_state *new_state) {
    if (!interactor || !interactor->moore_fsm) return;
    
    struct moore_hsm *hsm = interactor->moore_fsm;
    struct moore_state *current = hsm->current;
    
    /* Set current interactor for entry/exit actions */
    set_current_interactor(interactor);
    
    if (current && current->exit_action) {
        current->exit_action();
    }
    
    hsm->current = new_state;
    
    if (new_state && new_state->entry_action) {
        new_state->entry_action();
    }
}

/* Process events based on current state */
static void handle_interactor_event(struct scv_interactor *interactor, scv_interactor_event_t event, const void *event_data) {
    if (!interactor || !interactor->moore_fsm) return;
    
    struct moore_hsm *hsm = interactor->moore_fsm;
    struct moore_state *current = hsm->current;

    /* Debug logging */
    printf("[DEBUG] handle_interactor_event: current state pointer = %p, event = %d\n", (void*)current, event);
    if (current == &interactor_state_init) printf("  -> INIT\n");
    else if (current == &interactor_state_idle) printf("  -> IDLE\n");
    else if (current == &interactor_state_recording) printf("  -> RECORDING\n");
    else if (current == &interactor_state_uploading) printf("  -> UPLOADING\n");
    else if (current == &interactor_state_ota) printf("  -> OTA\n");
    else if (current == &interactor_state_error) printf("  -> ERROR\n");
    else if (current == &interactor_state_sleep) printf("  -> SLEEP\n");
    else printf("  -> UNKNOWN\n");

    /* Simple state transition logic - to be expanded */
    if (current == &interactor_state_init) {
        if (event == INTERACTOR_EVT_SYSTEM_START) {
            printf("  Transition INIT -> IDLE\n");
            interactor_change_state(interactor, &interactor_state_idle);
        } else if (event == INTERACTOR_EVT_ERROR_OCCURRED) {
            printf("  Transition INIT -> ERROR\n");
            interactor_change_state(interactor, &interactor_state_error);
        }
    } else if (current == &interactor_state_idle) {
        if (event == INTERACTOR_EVT_RECORDING_START) {
            printf("  Transition IDLE -> RECORDING\n");
            interactor_change_state(interactor, &interactor_state_recording);
        } else if (event == INTERACTOR_EVT_UPLOAD_START) {
            printf("  Transition IDLE -> UPLOADING\n");
            interactor_change_state(interactor, &interactor_state_uploading);
        } else if (event == INTERACTOR_EVT_OTA_START) {
            printf("  Transition IDLE -> OTA\n");
            interactor_change_state(interactor, &interactor_state_ota);
        } else if (event == INTERACTOR_EVT_POWER_LOW) {
            printf("  Transition IDLE -> SLEEP\n");
            interactor_change_state(interactor, &interactor_state_sleep);
        } else if (event == INTERACTOR_EVT_ERROR_OCCURRED) {
            printf("  Transition IDLE -> ERROR\n");
            interactor_change_state(interactor, &interactor_state_error);
        } else if (event == INTERACTOR_EVT_SYSTEM_STOP) {
            printf("  Transition IDLE -> INIT\n");
            interactor_change_state(interactor, &interactor_state_init);
        }
    } else if (current == &interactor_state_recording) {
        if (event == INTERACTOR_EVT_RECORDING_STOP) {
            printf("  Transition RECORDING -> IDLE\n");
            interactor_change_state(interactor, &interactor_state_idle);
        } else if (event == INTERACTOR_EVT_ERROR_OCCURRED) {
            printf("  Transition RECORDING -> ERROR\n");
            interactor_change_state(interactor, &interactor_state_error);
        }
    } else if (current == &interactor_state_uploading) {
        if (event == INTERACTOR_EVT_UPLOAD_COMPLETE) {
            printf("  Transition UPLOADING -> IDLE\n");
            interactor_change_state(interactor, &interactor_state_idle);
        } else if (event == INTERACTOR_EVT_ERROR_OCCURRED) {
            printf("  Transition UPLOADING -> ERROR\n");
            interactor_change_state(interactor, &interactor_state_error);
        }
    } else if (current == &interactor_state_ota) {
        if (event == INTERACTOR_EVT_OTA_COMPLETE) {
            printf("  Transition OTA -> IDLE\n");
            interactor_change_state(interactor, &interactor_state_idle);
        } else if (event == INTERACTOR_EVT_ERROR_OCCURRED) {
            printf("  Transition OTA -> ERROR\n");
            interactor_change_state(interactor, &interactor_state_error);
        }
    } else if (current == &interactor_state_error) {
        if (event == INTERACTOR_EVT_ERROR_RESOLVED) {
            printf("  Transition ERROR -> IDLE\n");
            interactor_change_state(interactor, &interactor_state_idle);
        } else if (event == INTERACTOR_EVT_SYSTEM_RESET) {
            printf("  Transition ERROR -> INIT\n");
            interactor_change_state(interactor, &interactor_state_init);
        }
    } else if (current == &interactor_state_sleep) {
        if (event == INTERACTOR_EVT_POWER_CHARGING) {
            printf("  Transition SLEEP -> IDLE\n");
            interactor_change_state(interactor, &interactor_state_idle);
        } else if (event == INTERACTOR_EVT_SYSTEM_RESET) {
            printf("  Transition SLEEP -> INIT\n");
            interactor_change_state(interactor, &interactor_state_init);
        }
    }
}

/* State implementations */

static void interactor_state_init_do_action(void) {
    struct scv_interactor *interactor = get_interactor_from_context();
    if (!interactor) return;
    /* Perform initialization tasks */
    memset(&interactor->context, 0, sizeof(interactor->context));
    interactor->context.current_state = INTERACTOR_STATE_INIT;
    interactor->context.can_start_recording = false;
    interactor->context.can_start_upload = false;
    interactor->context.can_start_ota = false;
    interactor->context.has_network_connectivity = false;
    interactor->context.has_sufficient_battery = false;
    interactor->context.has_sufficient_storage = false;
    interactor->context.is_charging = false;
}

static void interactor_state_init_entry_action(void) {
    struct scv_interactor *interactor = get_interactor_from_context();
    if (!interactor) return;
    interactor->context.current_state = INTERACTOR_STATE_INIT;
    /* Notify state change */
    if (interactor->state_changed_callback) {
        /* old_state is unknown, pass INTERACTOR_STATE_SLEEP? but we don't have it */
        interactor->state_changed_callback(INTERACTOR_STATE_INIT, INTERACTOR_STATE_SLEEP, interactor->user_data);
    }
}

static void interactor_state_init_exit_action(void) {
    /* Exit actions for INIT state */
}

static void interactor_state_idle_do_action(void) {
    struct scv_interactor *interactor = get_interactor_from_context();
    if (!interactor) return;
    /* Update context based on entity data */
    if (interactor->entity) {
        const struct scv_system_data *data = scv_entity_get_data(interactor->entity);
        if (data) {
            interactor->context.has_sufficient_battery = (data->battery_level >= interactor->rules.min_battery_for_recording);
            interactor->context.has_sufficient_storage = (data->storage_total_mb - data->storage_used_mb >= interactor->rules.min_storage_for_recording_mb);
            interactor->context.has_network_connectivity = (data->network_strength > 0);
            interactor->context.is_charging = false; /* TODO: get from data */
        }
    }
    /* Evaluate rules */
    scv_interactor_evaluate_rules(interactor);
}

static void interactor_state_idle_entry_action(void) {
    struct scv_interactor *interactor = get_interactor_from_context();
    if (!interactor) return;
    interactor->context.current_state = INTERACTOR_STATE_IDLE;
    /* Notify state change */
    if (interactor->state_changed_callback) {
        interactor->state_changed_callback(INTERACTOR_STATE_IDLE, INTERACTOR_STATE_INIT, interactor->user_data);
    }
}

static void interactor_state_idle_exit_action(void) {
    /* Exit actions for IDLE state */
}

static void interactor_state_recording_do_action(void) {
    struct scv_interactor *interactor = get_interactor_from_context();
    if (!interactor) return;
    /* Update recording substate logic */
}

static void interactor_state_recording_entry_action(void) {
    struct scv_interactor *interactor = get_interactor_from_context();
    if (!interactor) return;
    interactor->context.current_state = INTERACTOR_STATE_RECORDING;
    interactor->context.current_substate.recording_substate = RECORDING_SUBSTATE_STARTING;
    interactor->context.recording_attempts++;
    /* Notify state change */
    if (interactor->state_changed_callback) {
        interactor->state_changed_callback(INTERACTOR_STATE_RECORDING, INTERACTOR_STATE_IDLE, interactor->user_data);
    }
}

static void interactor_state_recording_exit_action(void) {
    /* Exit actions for RECORDING state */
}

static void interactor_state_uploading_do_action(void) {
    struct scv_interactor *interactor = get_interactor_from_context();
    if (!interactor) return;
    /* Update uploading substate logic */
}

static void interactor_state_uploading_entry_action(void) {
    struct scv_interactor *interactor = get_interactor_from_context();
    if (!interactor) return;
    interactor->context.current_state = INTERACTOR_STATE_UPLOADING;
    interactor->context.current_substate.uploading_substate = UPLOADING_SUBSTATE_PREPARING;
    interactor->context.upload_attempts++;
    /* Notify state change */
    if (interactor->state_changed_callback) {
        interactor->state_changed_callback(INTERACTOR_STATE_UPLOADING, INTERACTOR_STATE_IDLE, interactor->user_data);
    }
}

static void interactor_state_uploading_exit_action(void) {
    /* Exit actions for UPLOADING state */
}

static void interactor_state_ota_do_action(void) {
    struct scv_interactor *interactor = get_interactor_from_context();
    if (!interactor) return;
    /* Update OTA substate logic */
}

static void interactor_state_ota_entry_action(void) {
    struct scv_interactor *interactor = get_interactor_from_context();
    if (!interactor) return;
    interactor->context.current_state = INTERACTOR_STATE_OTA;
    interactor->context.current_substate.ota_substate = OTA_SUBSTATE_CHECKING;
    interactor->context.ota_attempts++;
    /* Notify state change */
    if (interactor->state_changed_callback) {
        interactor->state_changed_callback(INTERACTOR_STATE_OTA, INTERACTOR_STATE_IDLE, interactor->user_data);
    }
}

static void interactor_state_ota_exit_action(void) {
    /* Exit actions for OTA state */
}

static void interactor_state_error_do_action(void) {
    struct scv_interactor *interactor = get_interactor_from_context();
    if (!interactor) return;
    /* Error handling */
    interactor->context.consecutive_errors++;
}

static void interactor_state_error_entry_action(void) {
    struct scv_interactor *interactor = get_interactor_from_context();
    if (!interactor) return;
    interactor->context.current_state = INTERACTOR_STATE_ERROR;
    /* Notify state change */
    if (interactor->state_changed_callback) {
        interactor->state_changed_callback(INTERACTOR_STATE_ERROR, INTERACTOR_STATE_IDLE, interactor->user_data);
    }
}

static void interactor_state_error_exit_action(void) {
    /* Exit actions for ERROR state */
}

static void interactor_state_sleep_do_action(void) {
    struct scv_interactor *interactor = get_interactor_from_context();
    if (!interactor) return;
    /* Sleep mode processing */
}

static void interactor_state_sleep_entry_action(void) {
    struct scv_interactor *interactor = get_interactor_from_context();
    if (!interactor) return;
    interactor->context.current_state = INTERACTOR_STATE_SLEEP;
    /* Notify state change */
    if (interactor->state_changed_callback) {
        interactor->state_changed_callback(INTERACTOR_STATE_SLEEP, INTERACTOR_STATE_IDLE, interactor->user_data);
    }
}

static void interactor_state_sleep_exit_action(void) {
    /* Exit actions for SLEEP state */
}

/* Public API implementation */

struct scv_interactor *scv_interactor_init(struct scv_entity *entity,
                                           const struct scv_business_rules *rules) {
    struct scv_interactor *interactor = (struct scv_interactor *)malloc(sizeof(struct scv_interactor));
    if (!interactor) {
        return NULL;
    }
    memset(interactor, 0, sizeof(struct scv_interactor));

    /* Initialize business rules */
    if (rules) {
        interactor->rules = *rules;
    } else {
        /* Default rules */
        interactor->rules.min_battery_for_recording = 20;
        interactor->rules.min_storage_for_recording_mb = 100;
        interactor->rules.max_recording_duration_sec = 3600;
        interactor->rules.min_battery_for_upload = 15;
        interactor->rules.min_network_strength_for_upload = 30;
        interactor->rules.upload_only_on_wifi = true;
        interactor->rules.max_upload_retries = 3;
        interactor->rules.min_battery_for_ota = 50;
        interactor->rules.ota_only_on_wifi = true;
        interactor->rules.ota_only_when_charging = true;
        interactor->rules.sleep_battery_threshold = 10;
        interactor->rules.critical_battery_threshold = 5;
        interactor->rules.max_consecutive_errors = 5;
        interactor->rules.error_recovery_delay_ms = 5000;
    }

    /* Initialize business context */
    memset(&interactor->context, 0, sizeof(interactor->context));
    interactor->context.current_state = INTERACTOR_STATE_INIT;
    interactor->context.can_start_recording = false;
    interactor->context.can_start_upload = false;
    interactor->context.can_start_ota = false;
    interactor->context.has_network_connectivity = false;
    interactor->context.has_sufficient_battery = false;
    interactor->context.has_sufficient_storage = false;
    interactor->context.is_charging = false;

    /* Set associated entity */
    interactor->entity = entity;

    /* Initialize Moore HSM */
    interactor->moore_fsm = (struct moore_hsm *)malloc(sizeof(struct moore_hsm));
    if (!interactor->moore_fsm) {
        free(interactor);
        return NULL;
    }
    memset(interactor->moore_fsm, 0, sizeof(struct moore_hsm));
    
    /* Initialize HSM with root state */
    moore_hsm_init(interactor->moore_fsm, &interactor_state_init);
    
    /* Set current interactor for callbacks */
    set_current_interactor(interactor);

    return interactor;
}

void scv_interactor_destroy(struct scv_interactor *interactor) {
    if (!interactor) return;
    
    if (interactor->moore_fsm) {
        free(interactor->moore_fsm);
    }
    
    free(interactor);
}

int scv_interactor_process_event(struct scv_interactor *interactor,
                                 scv_interactor_event_t event,
                                 const void *event_data) {
    if (!interactor) return -1;

    printf("[DEBUG] scv_interactor_process_event: event=%d, current state pointer=%p\n", event, (void*)interactor->moore_fsm->current);
    set_current_interactor(interactor);
    handle_interactor_event(interactor, event, event_data);

    return 0;
}

int scv_interactor_process_rule(struct scv_interactor *interactor,
                                const void *event_data,
                                uint32_t event_data_size) {
    if (!interactor) return -1;
    
    /* For now, just log and treat as a generic rule processing */
    printf("[DEBUG] scv_interactor_process_rule: event_data_size=%u\n", event_data_size);
    
    /* Evaluate rules based on the raw event data */
    /* This is a placeholder - actual implementation would parse event_data and update business logic */
    scv_interactor_evaluate_rules(interactor);
    
    return 0;
}

static void update_context_from_entity(struct scv_interactor *interactor) {
    if (!interactor || !interactor->entity) return;
    
    const struct scv_system_data *data = scv_entity_get_data(interactor->entity);
    if (!data) return;
    
    /* Update battery condition */
    interactor->context.has_sufficient_battery = 
        (data->battery_level >= interactor->rules.min_battery_for_recording);
    
    /* Update storage condition */
    uint32_t free_storage_mb = data->storage_total_mb - data->storage_used_mb;
    interactor->context.has_sufficient_storage = 
        (free_storage_mb >= interactor->rules.min_storage_for_recording_mb);
    
    /* Update network connectivity */
    interactor->context.has_network_connectivity = (data->network_strength > 0);
    
    /* TODO: Update is_charging from data (currently not available) */
    interactor->context.is_charging = false;
}

int scv_interactor_evaluate_rules(struct scv_interactor *interactor) {
    if (!interactor) return -1;
    
    /* Update context from entity data */
    update_context_from_entity(interactor);
    
    /* Evaluate recording rules */
    interactor->context.can_start_recording = 
        interactor->context.has_sufficient_battery &&
        interactor->context.has_sufficient_storage;
    
    /* Evaluate upload rules */
    interactor->context.can_start_upload = 
        interactor->context.has_sufficient_battery &&
        interactor->context.has_network_connectivity &&
        (!interactor->rules.upload_only_on_wifi || true); /* TODO: check if WiFi */
    
    /* Evaluate OTA rules */
    interactor->context.can_start_ota = 
        interactor->context.has_sufficient_battery &&
        interactor->context.has_network_connectivity &&
        (!interactor->rules.ota_only_on_wifi || true) &&
        (!interactor->rules.ota_only_when_charging || interactor->context.is_charging);
    
    return 0;
}

void *scv_interactor_make_decision(struct scv_interactor *interactor,
                                   const char *decision_type,
                                   const void *decision_data) {
    if (!interactor || !decision_type) return NULL;
    
    /* Simple decision making based on current state and rules */
    if (strcmp(decision_type, "can_start_recording") == 0) {
        static bool result;
        result = interactor->context.can_start_recording;
        return &result;
    } else if (strcmp(decision_type, "can_start_upload") == 0) {
        static bool result;
        result = interactor->context.can_start_upload;
        return &result;
    } else if (strcmp(decision_type, "can_start_ota") == 0) {
        static bool result;
        result = interactor->context.can_start_ota;
        return &result;
    } else if (strcmp(decision_type, "should_enter_sleep") == 0) {
        static bool result;
        /* TODO: implement based on battery level */
        result = false;
        return &result;
    }
    
    return NULL;
}

scv_interactor_state_t scv_interactor_get_state(const struct scv_interactor *interactor) {
    if (!interactor) return INTERACTOR_STATE_INIT;
    return interactor->context.current_state;
}

int scv_interactor_get_substate(const struct scv_interactor *interactor, void *sub_state) {
    if (!interactor || !sub_state) return -1;
    
    switch (interactor->context.current_state) {
        case INTERACTOR_STATE_RECORDING:
            *(scv_recording_substate_t *)sub_state = interactor->context.current_substate.recording_substate;
            break;
        case INTERACTOR_STATE_UPLOADING:
            *(scv_uploading_substate_t *)sub_state = interactor->context.current_substate.uploading_substate;
            break;
        case INTERACTOR_STATE_OTA:
            *(scv_ota_substate_t *)sub_state = interactor->context.current_substate.ota_substate;
            break;
        default:
            return -1;
    }
    
    return 0;
}

int scv_interactor_update_rules(struct scv_interactor *interactor,
                                const struct scv_business_rules *rules) {
    if (!interactor || !rules) return -1;
    
    interactor->rules = *rules;
    return 0;
}

const struct scv_business_rules *scv_interactor_get_rules(const struct scv_interactor *interactor) {
    if (!interactor) return NULL;
    return &interactor->rules;
}

void scv_interactor_set_entity(struct scv_interactor *interactor, struct scv_entity *entity) {
    if (!interactor) return;
    interactor->entity = entity;
}

void scv_interactor_set_presenter(struct scv_interactor *interactor, struct scv_presenter *presenter) {
    if (!interactor) return;
    interactor->presenter = presenter;
}

void scv_interactor_set_view(struct scv_interactor *interactor, struct scv_view *view) {
    if (!interactor) return;
    interactor->view = view;
}

void scv_interactor_set_router(struct scv_interactor *interactor, struct scv_router *router) {
    if (!interactor) return;
    interactor->router = router;
}

const struct scv_business_context *scv_interactor_get_context(const struct scv_interactor *interactor) {
    if (!interactor) return NULL;
    return &interactor->context;
}

void scv_interactor_set_state_changed_callback(struct scv_interactor *interactor,
                                               void (*callback)(scv_interactor_state_t new_state,
                                                                scv_interactor_state_t old_state,
                                                                void *user_data),
                                               void *user_data) {
    if (!interactor) return;
    interactor->state_changed_callback = callback;
    interactor->user_data = user_data;
}

void scv_interactor_set_decision_made_callback(struct scv_interactor *interactor,
                                               void (*callback)(const char *decision,
                                                                const void *decision_data,
                                                                void *user_data),
                                               void *user_data) {
    if (!interactor) return;
    interactor->decision_made_callback = callback;
    interactor->user_data = user_data;
}

void scv_interactor_set_action_required_callback(struct scv_interactor *interactor,
                                                 void (*callback)(const char *action,
                                                                  const void *action_data,
                                                                  void *user_data),
                                                 void *user_data) {
    if (!interactor) return;
    interactor->action_required_callback = callback;
    interactor->user_data = user_data;
}

int scv_interactor_reset(struct scv_interactor *interactor) {
    if (!interactor) return -1;
    
    /* Reset context */
    memset(&interactor->context, 0, sizeof(interactor->context));
    interactor->context.current_state = INTERACTOR_STATE_INIT;
    
    /* Reset HSM */
    if (interactor->moore_fsm) {
        moore_hsm_init(interactor->moore_fsm, &interactor_state_init);
    }
    
    return 0;
}

/* Start the interactor (e.g., start its internal state machine) */
int scv_interactor_start(struct scv_interactor *interactor) {
    if (!interactor) return -1;

    /* Process SYSTEM_START event to transition from INIT to IDLE */
    return scv_interactor_process_event(interactor, INTERACTOR_EVT_SYSTEM_START, NULL);
}

/* Stop the interactor (e.g., stop its internal state machine) */
int scv_interactor_stop(struct scv_interactor *interactor) {
    if (!interactor) return -1;

    /* Process SYSTEM_STOP event to transition to INIT */
    return scv_interactor_process_event(interactor, INTERACTOR_EVT_SYSTEM_STOP, NULL);
}

int scv_interactor_get_status(const struct scv_interactor *interactor, char *status_buffer, uint32_t buffer_size) {
    if (!interactor || !status_buffer || buffer_size == 0) {
        return -1;
    }

    scv_interactor_state_t state = scv_interactor_get_state(interactor);
    const struct scv_business_context *ctx = &interactor->context;
    const struct scv_business_rules *rules = &interactor->rules;

    /* Format status string */
    int written = snprintf(status_buffer, buffer_size,
        "Interactor State: %s\n"
        "Can start recording: %s\n"
        "Can start upload: %s\n"
        "Can start OTA: %s\n"
        "Network connectivity: %s\n"
        "Sufficient battery: %s\n"
        "Sufficient storage: %s\n"
        "Charging: %s\n"
        "Recording attempts: %u\n"
        "Upload attempts: %u\n"
        "OTA attempts: %u\n"
        "Consecutive errors: %u\n"
        "Rules: min_battery_recording=%u%%, min_storage=%uMB, upload_only_on_wifi=%s",
        (state == INTERACTOR_STATE_INIT) ? "INIT" :
        (state == INTERACTOR_STATE_IDLE) ? "IDLE" :
        (state == INTERACTOR_STATE_RECORDING) ? "RECORDING" :
        (state == INTERACTOR_STATE_UPLOADING) ? "UPLOADING" :
        (state == INTERACTOR_STATE_OTA) ? "OTA" :
        (state == INTERACTOR_STATE_ERROR) ? "ERROR" :
        (state == INTERACTOR_STATE_SLEEP) ? "SLEEP" : "UNKNOWN",
        ctx->can_start_recording ? "YES" : "NO",
        ctx->can_start_upload ? "YES" : "NO",
        ctx->can_start_ota ? "YES" : "NO",
        ctx->has_network_connectivity ? "YES" : "NO",
        ctx->has_sufficient_battery ? "YES" : "NO",
        ctx->has_sufficient_storage ? "YES" : "NO",
        ctx->is_charging ? "YES" : "NO",
        ctx->recording_attempts,
        ctx->upload_attempts,
        ctx->ota_attempts,
        ctx->consecutive_errors,
        rules->min_battery_for_recording,
        rules->min_storage_for_recording_mb,
        rules->upload_only_on_wifi ? "YES" : "NO");

    if (written < 0 || (uint32_t)written >= buffer_size) {
        /* Truncation occurred, ensure null termination */
        status_buffer[buffer_size - 1] = '\0';
        return -2;
    }

    return 0;
}
