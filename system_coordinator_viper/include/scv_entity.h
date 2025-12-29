/**
 * scv_entity.h - System Coordinator VIPER Entity Component
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * Entity component implemented as an EFSM (Extended Finite State Machine).
 * Manages system data models, configurations, and extended state.
 */

#ifndef __SCV_ENTITY_H__
#define __SCV_ENTITY_H__

#include <stdint.h>
#include <stdbool.h>
#include "../../state_machine_extended/src/efsm_protocol.h"

/**
 * Entity States
 */
typedef enum {
    ENTITY_STATE_INIT,
    ENTITY_STATE_READY,
    ENTITY_STATE_UPDATING,
    ENTITY_STATE_ERROR,
    ENTITY_STATE_SYNCING,
} scv_entity_state_t;

/**
 * Entity Events
 */
typedef enum {
    ENTITY_EVENT_INIT_COMPLETE,
    ENTITY_EVENT_UPDATE_REQUEST,
    ENTITY_EVENT_UPDATE_COMPLETE,
    ENTITY_EVENT_ERROR_OCCURRED,
    ENTITY_EVENT_ERROR_RESOLVED,
    ENTITY_EVENT_SYNC_REQUEST,
    ENTITY_EVENT_SYNC_COMPLETE,
    ENTITY_EVENT_RESET,
} scv_entity_event_t;

/**
 * Entity Data Model
 */
struct scv_system_data {
    /* System configuration */
    char device_id[64];
    char firmware_version[32];
    char hardware_version[32];
    
    /* System status */
    uint32_t battery_level;          /* 0-100% */
    uint32_t storage_used_mb;        /* MB used */
    uint32_t storage_total_mb;       /* MB total */
    uint32_t network_strength;       /* 0-100% */
    
    /* Recording status */
    bool is_recording;
    uint32_t recording_duration_sec;
    uint32_t recording_file_size_mb;
    
    /* Upload status */
    bool is_uploading;
    uint32_t upload_progress;        /* 0-100% */
    uint32_t upload_speed_kbps;
    
    /* OTA status */
    bool ota_available;
    char ota_version[32];
    uint32_t ota_progress;           /* 0-100% */
    
    /* Error information */
    int32_t last_error_code;
    char last_error_msg[128];
    uint64_t last_error_timestamp;
    
    /* System metrics */
    uint32_t uptime_seconds;
    uint32_t total_recordings;
    uint32_t successful_uploads;
    uint32_t failed_uploads;
};

/**
 * Entity Configuration
 */
struct scv_entity_config {
    char entity_name[64];
    uint32_t entity_id;
    
    /* Data update intervals (ms) */
    uint32_t status_update_interval;
    uint32_t metrics_update_interval;
    
    /* Storage limits */
    uint32_t max_storage_mb;
    uint32_t max_recording_duration_sec;
    
    /* Network settings */
    char wifi_ssid[64];
    char wifi_password[64];
    char server_url[256];
    
    /* OTA settings */
    char ota_server_url[256];
    uint32_t ota_check_interval_sec;
};

/**
 * System Coordinator VIPER Entity
 */
struct scv_entity {
    /* EFSM for extended state management */
    struct efsm_processor *efsm;
    
    /* Data model */
    struct scv_system_data data;
    
    /* Configuration */
    struct scv_entity_config config;
    
    /* Callbacks */
    void (*data_updated_callback)(const struct scv_system_data *data, void *user_data);
    void (*error_callback)(int error_code, const char *error_msg, void *user_data);
    
    /* User data for callbacks */
    void *user_data;
};

/* Function prototypes */

/**
 * Initialize a System Coordinator VIPER Entity
 * 
 * @param config Entity configuration (can be NULL for defaults)
 * @return New entity instance, NULL on error
 */
struct scv_entity *scv_entity_init(const struct scv_entity_config *config);

/**
 * Destroy a System Coordinator VIPER Entity
 * 
 * @param entity Entity to destroy
 */
void scv_entity_destroy(struct scv_entity *entity);

/**
 * Update system data in the entity
 * 
 * @param entity Entity instance
 * @param data New system data (partial updates allowed)
 * @return 0 on success, negative on error
 */
int scv_entity_update_data(struct scv_entity *entity, const struct scv_system_data *data);

/**
 * Get current system data
 * 
 * @param entity Entity instance
 * @return Current system data
 */
const struct scv_system_data *scv_entity_get_data(const struct scv_entity *entity);

/**
 * Update entity configuration
 * 
 * @param entity Entity instance
 * @param config New configuration
 * @return 0 on success, negative on error
 */
int scv_entity_update_config(struct scv_entity *entity, const struct scv_entity_config *config);

/**
 * Get entity configuration
 * 
 * @param entity Entity instance
 * @return Current configuration
 */
const struct scv_entity_config *scv_entity_get_config(const struct scv_entity *entity);

/**
 * Set data updated callback
 * 
 * @param entity Entity instance
 * @param callback Callback function
 * @param user_data User data passed to callback
 */
void scv_entity_set_data_updated_callback(struct scv_entity *entity,
                                          void (*callback)(const struct scv_system_data *data, void *user_data),
                                          void *user_data);

/**
 * Set error callback
 * 
 * @param entity Entity instance
 * @param callback Callback function
 * @param user_data User data passed to callback
 */
void scv_entity_set_error_callback(struct scv_entity *entity,
                                   void (*callback)(int error_code, const char *error_msg, void *user_data),
                                   void *user_data);

/**
 * Process entity event (for EFSM)
 * 
 * @param entity Entity instance
 * @param event Event to process
 * @param event_data Event data
 * @return 0 on success, negative on error
 */
int scv_entity_process_event(struct scv_entity *entity, int event, const void *event_data);

/**
 * Get entity state
 * 
 * @param entity Entity instance
 * @return Current entity state
 */
scv_entity_state_t scv_entity_get_state(const struct scv_entity *entity);

/**
 * Reset entity to initial state
 * 
 * @param entity Entity instance
 * @return 0 on success, negative on error
 */
int scv_entity_reset(struct scv_entity *entity);

/**
 * Save entity data to persistent storage
 * 
 * @param entity Entity instance
 * @param filepath Path to save file
 * @return 0 on success, negative on error
 */
int scv_entity_save_to_file(const struct scv_entity *entity, const char *filepath);

/**
 * Load entity data from persistent storage
 * 
 * @param entity Entity instance
 * @param filepath Path to load file
 * @return 0 on success, negative on error
 */
int scv_entity_load_from_file(struct scv_entity *entity, const char *filepath);

/**
 * Get entity status as a human-readable string
 * 
 * @param entity Entity instance
 * @param status_buffer Buffer to store status string
 * @param buffer_size Size of buffer
 * @return 0 on success, negative on error
 */
int scv_entity_get_status(const struct scv_entity *entity, char *status_buffer, uint32_t buffer_size);

#endif /* __SCV_ENTITY_H__ */
