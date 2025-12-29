/**
 * scv_entity.c - System Coordinator VIPER Entity Component
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * Entity component implemented as an EFSM (Extended Finite State Machine).
 * Manages system data models, configurations, and extended state.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "scv_entity.h"
#include "../../state_machine_extended/src/efsm_protocol.h"

/* Internal helper functions */
static void entity_state_init_process(struct efsm_context *ctx);
static void entity_state_init_entry(struct efsm_context *ctx);
static void entity_state_init_exit(struct efsm_context *ctx);

static void entity_state_ready_process(struct efsm_context *ctx);
static void entity_state_ready_entry(struct efsm_context *ctx);
static void entity_state_ready_exit(struct efsm_context *ctx);

static void entity_state_updating_process(struct efsm_context *ctx);
static void entity_state_updating_entry(struct efsm_context *ctx);
static void entity_state_updating_exit(struct efsm_context *ctx);

static void entity_state_error_process(struct efsm_context *ctx);
static void entity_state_error_entry(struct efsm_context *ctx);
static void entity_state_error_exit(struct efsm_context *ctx);

static void entity_state_syncing_process(struct efsm_context *ctx);
static void entity_state_syncing_entry(struct efsm_context *ctx);
static void entity_state_syncing_exit(struct efsm_context *ctx);

/* State objects */
static struct efsm_state entity_state_init = {
    .process = entity_state_init_process,
    .on_entry = entity_state_init_entry,
    .on_exit = entity_state_init_exit,
};

static struct efsm_state entity_state_ready = {
    .process = entity_state_ready_process,
    .on_entry = entity_state_ready_entry,
    .on_exit = entity_state_ready_exit,
};

static struct efsm_state entity_state_updating = {
    .process = entity_state_updating_process,
    .on_entry = entity_state_updating_entry,
    .on_exit = entity_state_updating_exit,
};

static struct efsm_state entity_state_error = {
    .process = entity_state_error_process,
    .on_entry = entity_state_error_entry,
    .on_exit = entity_state_error_exit,
};

static struct efsm_state entity_state_syncing = {
    .process = entity_state_syncing_process,
    .on_entry = entity_state_syncing_entry,
    .on_exit = entity_state_syncing_exit,
};

/* Entity-specific context extension */
struct scv_entity_context {
    struct efsm_context base;
    struct scv_entity *entity; /* back pointer */
};

/* Internal helper to get entity from context */
static struct scv_entity *get_entity_from_context(struct efsm_context *ctx) {
    if (!ctx || !ctx->buffer) return NULL;
    struct scv_entity_context *ent_ctx = (struct scv_entity_context *)ctx->buffer;
    return ent_ctx->entity;
}

/* Internal helper to change state */
static void entity_change_state(struct efsm_processor *p, struct efsm_state *new_state) {
    if (p->current_state && p->current_state->on_exit) {
        p->current_state->on_exit(&p->context);
    }
    p->current_state = new_state;
    if (new_state && new_state->on_entry) {
        new_state->on_entry(&p->context);
    }
}


/* Process events based on current state */
static void handle_entity_event(struct scv_entity *entity, scv_entity_event_t event, const void *event_data) {
    struct efsm_processor *efsm = entity->efsm;
    if (!efsm) return;

    struct efsm_state *current = efsm->current_state;

    if (current == &entity_state_init) {
        if (event == ENTITY_EVENT_INIT_COMPLETE) {
            entity_change_state(efsm, &entity_state_ready);
        } else if (event == ENTITY_EVENT_ERROR_OCCURRED) {
            entity_change_state(efsm, &entity_state_error);
        }
    } else if (current == &entity_state_ready) {
        if (event == ENTITY_EVENT_UPDATE_REQUEST) {
            entity_change_state(efsm, &entity_state_updating);
        } else if (event == ENTITY_EVENT_SYNC_REQUEST) {
            entity_change_state(efsm, &entity_state_syncing);
        } else if (event == ENTITY_EVENT_ERROR_OCCURRED) {
            entity_change_state(efsm, &entity_state_error);
        } else if (event == ENTITY_EVENT_RESET) {
            entity_change_state(efsm, &entity_state_init);
        }
    } else if (current == &entity_state_updating) {
        if (event == ENTITY_EVENT_UPDATE_COMPLETE) {
            entity_change_state(efsm, &entity_state_ready);
        } else if (event == ENTITY_EVENT_ERROR_OCCURRED) {
            entity_change_state(efsm, &entity_state_error);
        }
    } else if (current == &entity_state_error) {
        if (event == ENTITY_EVENT_ERROR_RESOLVED) {
            entity_change_state(efsm, &entity_state_ready);
        } else if (event == ENTITY_EVENT_RESET) {
            entity_change_state(efsm, &entity_state_init);
        }
    } else if (current == &entity_state_syncing) {
        if (event == ENTITY_EVENT_SYNC_COMPLETE) {
            entity_change_state(efsm, &entity_state_ready);
        } else if (event == ENTITY_EVENT_ERROR_OCCURRED) {
            entity_change_state(efsm, &entity_state_error);
        }
    }
}

/* State implementations */

static void entity_state_init_process(struct efsm_context *ctx) {
    struct scv_entity *entity = get_entity_from_context(ctx);
    if (!entity) return;
    /* Perform initialization tasks */
    memset(&entity->data, 0, sizeof(entity->data));
    strcpy(entity->data.device_id, "UNKNOWN");
    strcpy(entity->data.firmware_version, "1.0.0");
    strcpy(entity->data.hardware_version, "1.0");
    entity->data.battery_level = 100;
    entity->data.storage_total_mb = 4096;
    entity->data.storage_used_mb = 0;
    entity->data.network_strength = 0;
    entity->data.is_recording = false;
    entity->data.is_uploading = false;
    entity->data.ota_available = false;
    entity->data.last_error_code = 0;
    entity->data.last_error_msg[0] = '\0';
    entity->data.uptime_seconds = 0;
    entity->data.total_recordings = 0;
    entity->data.successful_uploads = 0;
    entity->data.failed_uploads = 0;
}

static void entity_state_init_entry(struct efsm_context *ctx) {
    /* Entry actions for INIT state */
}

static void entity_state_init_exit(struct efsm_context *ctx) {
    /* Exit actions for INIT state */
}

static void entity_state_ready_process(struct efsm_context *ctx) {
    struct scv_entity *entity = get_entity_from_context(ctx);
    if (!entity) return;
    /* Update uptime */
    entity->data.uptime_seconds++;
}

static void entity_state_ready_entry(struct efsm_context *ctx) {
    struct scv_entity *entity = get_entity_from_context(ctx);
    if (!entity) return;
    /* Notify that entity is ready */
    if (entity->data_updated_callback) {
        entity->data_updated_callback(&entity->data, entity->user_data);
    }
}

static void entity_state_ready_exit(struct efsm_context *ctx) {
    /* Exit actions for READY state */
}

static void entity_state_updating_process(struct efsm_context *ctx) {
    struct scv_entity *entity = get_entity_from_context(ctx);
    if (!entity) return;
    /* Simulate data update */
}

static void entity_state_updating_entry(struct efsm_context *ctx) {
    struct scv_entity *entity = get_entity_from_context(ctx);
    if (!entity) return;
    /* Notify start of update */
}

static void entity_state_updating_exit(struct efsm_context *ctx) {
    /* Exit actions for UPDATING state */
}

static void entity_state_error_process(struct efsm_context *ctx) {
    struct scv_entity *entity = get_entity_from_context(ctx);
    if (!entity) return;
    /* Error handling */
}

static void entity_state_error_entry(struct efsm_context *ctx) {
    struct scv_entity *entity = get_entity_from_context(ctx);
    if (!entity) return;
    /* Notify error */
    if (entity->error_callback) {
        entity->error_callback(entity->data.last_error_code,
                               entity->data.last_error_msg,
                               entity->user_data);
    }
}

static void entity_state_error_exit(struct efsm_context *ctx) {
    /* Exit actions for ERROR state */
}

static void entity_state_syncing_process(struct efsm_context *ctx) {
    struct scv_entity *entity = get_entity_from_context(ctx);
    if (!entity) return;
    /* Perform synchronization */
}

static void entity_state_syncing_entry(struct efsm_context *ctx) {
    struct scv_entity *entity = get_entity_from_context(ctx);
    if (!entity) return;
    /* Notify start of sync */
}

static void entity_state_syncing_exit(struct efsm_context *ctx) {
    /* Exit actions for SYNCING state */
}

/* Public API implementation */

struct scv_entity *scv_entity_init(const struct scv_entity_config *config) {
    struct scv_entity *entity = (struct scv_entity *)malloc(sizeof(struct scv_entity));
    if (!entity) {
        return NULL;
    }
    memset(entity, 0, sizeof(struct scv_entity));

    /* Initialize configuration */
    if (config) {
        entity->config = *config;
    } else {
        /* Default configuration */
        strcpy(entity->config.entity_name, "SystemCoordinatorEntity");
        entity->config.entity_id = 1;
        entity->config.status_update_interval = 1000;
        entity->config.metrics_update_interval = 5000;
        entity->config.max_storage_mb = 4096;
        entity->config.max_recording_duration_sec = 3600;
        strcpy(entity->config.wifi_ssid, "");
        strcpy(entity->config.wifi_password, "");
        strcpy(entity->config.server_url, "http://localhost:8080");
        strcpy(entity->config.ota_server_url, "http://ota.example.com");
        entity->config.ota_check_interval_sec = 86400;
    }

    /* Initialize EFSM */
    struct efsm_processor *efsm = (struct efsm_processor *)malloc(sizeof(struct efsm_processor));
    if (!efsm) {
        free(entity);
        return NULL;
    }
    memset(efsm, 0, sizeof(struct efsm_processor));
    /* We need to set up our custom context */
    struct scv_entity_context *ctx = (struct scv_entity_context *)malloc(sizeof(struct scv_entity_context));
    if (!ctx) {
        free(efsm);
        free(entity);
        return NULL;
    }
    memset(ctx, 0, sizeof(struct scv_entity_context));
    ctx->entity = entity;
    /* Copy base context */
    efsm->context = ctx->base;
    efsm->context.buffer = ctx;  /* store pointer to our custom context */
    efsm->current_state = &entity_state_init;
    entity->efsm = efsm; /* no cast needed */

    /* Initialize data */
    entity_state_init_process(&efsm->context);

    /* Call entry for initial state */
    if (entity_state_init.on_entry) {
        entity_state_init.on_entry(&efsm->context);
    }

    return entity;
}

void scv_entity_destroy(struct scv_entity *entity) {
    if (!entity) return;

    /* Free EFSM */
    if (entity->efsm) {
        struct efsm_processor *efsm = entity->efsm;
        if (efsm->context.buffer) {
            free(efsm->context.buffer);
        }
        free(efsm);
    }

    free(entity);
}

int scv_entity_update_data(struct scv_entity *entity, const struct scv_system_data *data) {
    if (!entity || !data) {
        return -1;
    }

    /* Merge data (simple field-by-field update) */
    if (data->device_id[0] != '\0') {
        strncpy(entity->data.device_id, data->device_id, sizeof(entity->data.device_id) - 1);
    }
    if (data->firmware_version[0] != '\0') {
        strncpy(entity->data.firmware_version, data->firmware_version, sizeof(entity->data.firmware_version) - 1);
    }
    if (data->hardware_version[0] != '\0') {
        strncpy(entity->data.hardware_version, data->hardware_version, sizeof(entity->data.hardware_version) - 1);
    }
    if (data->battery_level <= 100) {
        entity->data.battery_level = data->battery_level;
    }
    if (data->storage_used_mb > 0) {
        entity->data.storage_used_mb = data->storage_used_mb;
    }
    if (data->storage_total_mb > 0) {
        entity->data.storage_total_mb = data->storage_total_mb;
    }
    if (data->network_strength <= 100) {
        entity->data.network_strength = data->network_strength;
    }
    entity->data.is_recording = data->is_recording;
    entity->data.recording_duration_sec = data->recording_duration_sec;
    entity->data.recording_file_size_mb = data->recording_file_size_mb;
    entity->data.is_uploading = data->is_uploading;
    entity->data.upload_progress = data->upload_progress;
    entity->data.upload_speed_kbps = data->upload_speed_kbps;
    entity->data.ota_available = data->ota_available;
    if (data->ota_version[0] != '\0') {
        strncpy(entity->data.ota_version, data->ota_version, sizeof(entity->data.ota_version) - 1);
    }
    entity->data.ota_progress = data->ota_progress;
    entity->data.last_error_code = data->last_error_code;
    if (data->last_error_msg[0] != '\0') {
        strncpy(entity->data.last_error_msg, data->last_error_msg, sizeof(entity->data.last_error_msg) - 1);
    }
    entity->data.last_error_timestamp = data->last_error_timestamp;
    entity->data.uptime_seconds = data->uptime_seconds;
    entity->data.total_recordings = data->total_recordings;
    entity->data.successful_uploads = data->successful_uploads;
    entity->data.failed_uploads = data->failed_uploads;

    /* Notify data updated */
    if (entity->data_updated_callback) {
        entity->data_updated_callback(&entity->data, entity->user_data);
    }

    return 0;
}

const struct scv_system_data *scv_entity_get_data(const struct scv_entity *entity) {
    if (!entity) {
        return NULL;
    }
    return &entity->data;
}

int scv_entity_update_config(struct scv_entity *entity, const struct scv_entity_config *config) {
    if (!entity || !config) {
        return -1;
    }
    entity->config = *config;
    return 0;
}

const struct scv_entity_config *scv_entity_get_config(const struct scv_entity *entity) {
    if (!entity) {
        return NULL;
    }
    return &entity->config;
}

void scv_entity_set_data_updated_callback(struct scv_entity *entity,
                                          void (*callback)(const struct scv_system_data *data, void *user_data),
                                          void *user_data) {
    if (!entity) return;
    entity->data_updated_callback = callback;
    entity->user_data = user_data;
}

void scv_entity_set_error_callback(struct scv_entity *entity,
                                   void (*callback)(int error_code, const char *error_msg, void *user_data),
                                   void *user_data) {
    if (!entity) return;
    entity->error_callback = callback;
    entity->user_data = user_data;
}

int scv_entity_process_event(struct scv_entity *entity, int event, const void *event_data) {
    if (!entity) return -1;
    handle_entity_event(entity, (scv_entity_event_t)event, event_data);
    return 0;
}

scv_entity_state_t scv_entity_get_state(const struct scv_entity *entity) {
    if (!entity || !entity->efsm) return ENTITY_STATE_INIT;
    struct efsm_state *current = entity->efsm->current_state;
    if (current == &entity_state_init) return ENTITY_STATE_INIT;
    if (current == &entity_state_ready) return ENTITY_STATE_READY;
    if (current == &entity_state_updating) return ENTITY_STATE_UPDATING;
    if (current == &entity_state_error) return ENTITY_STATE_ERROR;
    if (current == &entity_state_syncing) return ENTITY_STATE_SYNCING;
    return ENTITY_STATE_INIT;
}

int scv_entity_reset(struct scv_entity *entity) {
    if (!entity || !entity->efsm) return -1;
    entity_change_state(entity->efsm, &entity_state_init);
    return 0;
}

int scv_entity_save_to_file(const struct scv_entity *entity, const char *filepath) {
    /* Stub implementation - would serialize entity data to file */
    (void)entity;
    (void)filepath;
    return 0;
}

int scv_entity_load_from_file(struct scv_entity *entity, const char *filepath) {
    /* Stub implementation - would deserialize entity data from file */
    (void)entity;
    (void)filepath;
    return 0;
}

int scv_entity_get_status(const struct scv_entity *entity, char *status_buffer, uint32_t buffer_size) {
    if (!entity || !status_buffer || buffer_size == 0) {
        return -1;
    }

    scv_entity_state_t state = scv_entity_get_state(entity);
    const struct scv_system_data *data = &entity->data;

    /* Format status string */
    int written = snprintf(status_buffer, buffer_size,
        "Entity: %s (ID: %u)\n"
        "State: %s\n"
        "Device: %s\n"
        "Firmware: %s\n"
        "Battery: %u%%\n"
        "Storage: %u/%u MB\n"
        "Network: %u%%\n"
        "Uptime: %u seconds\n"
        "Recordings: %u\n"
        "Uploads: %u successful, %u failed\n"
        "Last error: %d - %s",
        entity->config.entity_name,
        entity->config.entity_id,
        (state == ENTITY_STATE_INIT) ? "INIT" :
        (state == ENTITY_STATE_READY) ? "READY" :
        (state == ENTITY_STATE_UPDATING) ? "UPDATING" :
        (state == ENTITY_STATE_ERROR) ? "ERROR" :
        (state == ENTITY_STATE_SYNCING) ? "SYNCING" : "UNKNOWN",
        data->device_id,
        data->firmware_version,
        data->battery_level,
        data->storage_used_mb,
        data->storage_total_mb,
        data->network_strength,
        data->uptime_seconds,
        data->total_recordings,
        data->successful_uploads,
        data->failed_uploads,
        data->last_error_code,
        data->last_error_msg[0] ? data->last_error_msg : "None");

    if (written < 0 || (uint32_t)written >= buffer_size) {
        /* Truncation occurred, ensure null termination */
        status_buffer[buffer_size - 1] = '\0';
        return -2;
    }

    return 0;
}
