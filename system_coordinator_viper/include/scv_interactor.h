/**
 * scv_interactor.h - System Coordinator VIPER Interactor Component
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * Interactor component implemented as a Moore Hierarchical State Machine.
 * Handles business logic, rules, and decision-making.
 */

#ifndef __SCV_INTERACTOR_H__
#define __SCV_INTERACTOR_H__

#include <stdint.h>
#include <stdbool.h>
#include "../../state_machine_extended/src/moore_hierarchical.h"
#include "scv_entity.h"

/* Forward declarations for VIPER components */
struct scv_presenter;
struct scv_view;
struct scv_router;

/**
 * Interactor States (Top-level)
 */
typedef enum {
    INTERACTOR_STATE_INIT,
    INTERACTOR_STATE_IDLE,
    INTERACTOR_STATE_RECORDING,
    INTERACTOR_STATE_UPLOADING,
    INTERACTOR_STATE_OTA,
    INTERACTOR_STATE_ERROR,
    INTERACTOR_STATE_SLEEP,
} scv_interactor_state_t;

/**
 * Interactor Sub-states for Recording
 */
typedef enum {
    RECORDING_SUBSTATE_STARTING,
    RECORDING_SUBSTATE_ACTIVE,
    RECORDING_SUBSTATE_PAUSED,
    RECORDING_SUBSTATE_STOPPING,
    RECORDING_SUBSTATE_PROCESSING,
} scv_recording_substate_t;

/**
 * Interactor Sub-states for Uploading
 */
typedef enum {
    UPLOADING_SUBSTATE_PREPARING,
    UPLOADING_SUBSTATE_CONNECTING,
    UPLOADING_SUBSTATE_TRANSFERRING,
    UPLOADING_SUBSTATE_COMPLETING,
    UPLOADING_SUBSTATE_RETRYING,
} scv_uploading_substate_t;

/**
 * Interactor Sub-states for OTA
 */
typedef enum {
    OTA_SUBSTATE_CHECKING,
    OTA_SUBSTATE_DOWNLOADING,
    OTA_SUBSTATE_VERIFYING,
    OTA_SUBSTATE_INSTALLING,
    OTA_SUBSTATE_REBOOTING,
} scv_ota_substate_t;

/**
 * Business Logic Events
 */
typedef enum {
    /* System events */
    INTERACTOR_EVT_SYSTEM_START,
    INTERACTOR_EVT_SYSTEM_STOP,
    INTERACTOR_EVT_SYSTEM_RESET,
    
    /* Recording events */
    INTERACTOR_EVT_RECORDING_START,
    INTERACTOR_EVT_RECORDING_STOP,
    INTERACTOR_EVT_RECORDING_PAUSE,
    INTERACTOR_EVT_RECORDING_RESUME,
    
    /* Upload events */
    INTERACTOR_EVT_UPLOAD_START,
    INTERACTOR_EVT_UPLOAD_STOP,
    INTERACTOR_EVT_UPLOAD_PAUSE,
    INTERACTOR_EVT_UPLOAD_RESUME,
    INTERACTOR_EVT_UPLOAD_COMPLETE,
    
    /* OTA events */
    INTERACTOR_EVT_OTA_CHECK,
    INTERACTOR_EVT_OTA_START,
    INTERACTOR_EVT_OTA_CANCEL,
    INTERACTOR_EVT_OTA_COMPLETE,
    
    /* Network events */
    INTERACTOR_EVT_NETWORK_CONNECTED,
    INTERACTOR_EVT_NETWORK_DISCONNECTED,
    
    /* Power events */
    INTERACTOR_EVT_POWER_LOW,
    INTERACTOR_EVT_POWER_CHARGING,
    INTERACTOR_EVT_POWER_CRITICAL,
    
    /* Error events */
    INTERACTOR_EVT_ERROR_OCCURRED,
    INTERACTOR_EVT_ERROR_RESOLVED,
    
    /* User interaction events */
    INTERACTOR_EVT_USER_REQUEST_RECORDING,
    INTERACTOR_EVT_USER_REQUEST_UPLOAD,
    INTERACTOR_EVT_USER_REQUEST_OTA,
    INTERACTOR_EVT_USER_CANCEL,
} scv_interactor_event_t;

/**
 * Business Logic Rules Configuration
 */
struct scv_business_rules {
    /* Recording rules */
    uint32_t min_battery_for_recording;      /* Minimum battery % to start recording */
    uint32_t min_storage_for_recording_mb;   /* Minimum free storage for recording */
    uint32_t max_recording_duration_sec;     /* Maximum recording duration */
    
    /* Upload rules */
    uint32_t min_battery_for_upload;         /* Minimum battery % to start upload */
    uint32_t min_network_strength_for_upload;/* Minimum network strength for upload */
    bool upload_only_on_wifi;                /* Only upload when connected to WiFi */
    uint32_t max_upload_retries;             /* Maximum upload retry attempts */
    
    /* OTA rules */
    uint32_t min_battery_for_ota;            /* Minimum battery % for OTA */
    bool ota_only_on_wifi;                   /* Only perform OTA on WiFi */
    bool ota_only_when_charging;             /* Only perform OTA when charging */
    
    /* Power management rules */
    uint32_t sleep_battery_threshold;        /* Battery % threshold to enter sleep */
    uint32_t critical_battery_threshold;     /* Battery % threshold for critical shutdown */
    
    /* Error handling rules */
    uint32_t max_consecutive_errors;         /* Maximum consecutive errors before shutdown */
    uint32_t error_recovery_delay_ms;        /* Delay before attempting recovery */
};

/**
 * Business Logic Context
 */
struct scv_business_context {
    /* Current state and sub-state */
    scv_interactor_state_t current_state;
    union {
        scv_recording_substate_t recording_substate;
        scv_uploading_substate_t uploading_substate;
        scv_ota_substate_t ota_substate;
    } current_substate;
    
    /* Decision data */
    bool can_start_recording;
    bool can_start_upload;
    bool can_start_ota;
    
    /* Conditions */
    bool has_network_connectivity;
    bool has_sufficient_battery;
    bool has_sufficient_storage;
    bool is_charging;
    
    /* Statistics */
    uint32_t recording_attempts;
    uint32_t upload_attempts;
    uint32_t ota_attempts;
    uint32_t consecutive_errors;
    
    /* Timestamps */
    uint64_t last_recording_start;
    uint64_t last_upload_start;
    uint64_t last_ota_check;
};

/**
 * System Coordinator VIPER Interactor
 */
struct scv_interactor {
    /* Moore Hierarchical State Machine */
    struct moore_hsm *moore_fsm;
    
    /* Business rules */
    struct scv_business_rules rules;
    
    /* Business context */
    struct scv_business_context context;
    
    /* Associated Entity (for data access) */
    struct scv_entity *entity;
    
    /* Associated VIPER components for coordination */
    struct scv_presenter *presenter;
    struct scv_view *view;
    struct scv_router *router;
    
    /* Callbacks */
    void (*state_changed_callback)(scv_interactor_state_t new_state, 
                                   scv_interactor_state_t old_state,
                                   void *user_data);
    void (*decision_made_callback)(const char *decision, 
                                   const void *decision_data,
                                   void *user_data);
    void (*action_required_callback)(const char *action,
                                     const void *action_data,
                                     void *user_data);
    
    /* User data for callbacks */
    void *user_data;
};

/* Function prototypes */

/**
 * Initialize a System Coordinator VIPER Interactor
 * 
 * @param entity Associated entity (can be NULL, set later)
 * @param rules Business rules (can be NULL for defaults)
 * @return New interactor instance, NULL on error
 */
struct scv_interactor *scv_interactor_init(struct scv_entity *entity,
                                           const struct scv_business_rules *rules);

/**
 * Destroy a System Coordinator VIPER Interactor
 * 
 * @param interactor Interactor to destroy
 */
void scv_interactor_destroy(struct scv_interactor *interactor);

/**
 * Process a business logic event
 * 
 * @param interactor Interactor instance
 * @param event Event to process
 * @param event_data Event data
 * @return 0 on success, negative on error
 */
int scv_interactor_process_event(struct scv_interactor *interactor,
                                 scv_interactor_event_t event,
                                 const void *event_data);

/**
 * Process a business rule with raw event data
 * 
 * @param interactor Interactor instance
 * @param event_data Rule-specific data
 * @param event_data_size Size of event_data in bytes
 * @return 0 on success, negative on error
 */
int scv_interactor_process_rule(struct scv_interactor *interactor,
                                const void *event_data,
                                uint32_t event_data_size);

/**
 * Evaluate business rules based on current system data
 * 
 * @param interactor Interactor instance
 * @return 0 on success, negative on error
 */
int scv_interactor_evaluate_rules(struct scv_interactor *interactor);

/**
 * Make a business decision based on current state and rules
 * 
 * @param interactor Interactor instance
 * @param decision_type Type of decision needed
 * @param decision_data Decision-specific data
 * @return Decision result (interpretation depends on decision_type)
 */
void *scv_interactor_make_decision(struct scv_interactor *interactor,
                                   const char *decision_type,
                                   const void *decision_data);

/**
 * Get current interactor state
 * 
 * @param interactor Interactor instance
 * @return Current state
 */
scv_interactor_state_t scv_interactor_get_state(const struct scv_interactor *interactor);

/**
 * Get current sub-state
 * 
 * @param interactor Interactor instance
 * @param sub_state Output parameter for sub-state
 * @return 0 on success, negative on error
 */
int scv_interactor_get_substate(const struct scv_interactor *interactor, void *sub_state);

/**
 * Update business rules
 * 
 * @param interactor Interactor instance
 * @param rules New business rules
 * @return 0 on success, negative on error
 */
int scv_interactor_update_rules(struct scv_interactor *interactor,
                                const struct scv_business_rules *rules);

/**
 * Get business rules
 * 
 * @param interactor Interactor instance
 * @return Current business rules
 */
const struct scv_business_rules *scv_interactor_get_rules(const struct scv_interactor *interactor);

/**
 * Set associated entity
 * 
 * @param interactor Interactor instance
 * @param entity Entity to associate
 */
void scv_interactor_set_entity(struct scv_interactor *interactor, struct scv_entity *entity);

/**
 * Set associated presenter
 * 
 * @param interactor Interactor instance
 * @param presenter Presenter to associate
 */
void scv_interactor_set_presenter(struct scv_interactor *interactor, struct scv_presenter *presenter);

/**
 * Set associated view
 * 
 * @param interactor Interactor instance
 * @param view View to associate
 */
void scv_interactor_set_view(struct scv_interactor *interactor, struct scv_view *view);

/**
 * Set associated router
 * 
 * @param interactor Interactor instance
 * @param router Router to associate
 */
void scv_interactor_set_router(struct scv_interactor *interactor, struct scv_router *router);

/**
 * Get business context for debugging
 * 
 * @param interactor Interactor instance
 * @return Business context
 */
const struct scv_business_context *scv_interactor_get_context(const struct scv_interactor *interactor);

/**
 * Set state changed callback
 * 
 * @param interactor Interactor instance
 * @param callback Callback function
 * @param user_data User data passed to callback
 */
void scv_interactor_set_state_changed_callback(struct scv_interactor *interactor,
                                               void (*callback)(scv_interactor_state_t new_state,
                                                                scv_interactor_state_t old_state,
                                                                void *user_data),
                                               void *user_data);

/**
 * Set decision made callback
 * 
 * @param interactor Interactor instance
 * @param callback Callback function
 * @param user_data User data passed to callback
 */
void scv_interactor_set_decision_made_callback(struct scv_interactor *interactor,
                                               void (*callback)(const char *decision,
                                                                const void *decision_data,
                                                                void *user_data),
                                               void *user_data);

/**
 * Set action required callback
 * 
 * @param interactor Interactor instance
 * @param callback Callback function
 * @param user_data User data passed to callback
 */
void scv_interactor_set_action_required_callback(struct scv_interactor *interactor,
                                                 void (*callback)(const char *action,
                                                                  const void *action_data,
                                                                  void *user_data),
                                                 void *user_data);

/**
 * Reset interactor to initial state
 * 
 * @param interactor Interactor instance
 * @return 0 on success, negative on error
 */
int scv_interactor_reset(struct scv_interactor *interactor);

/**
 * Start the interactor (e.g., start its internal state machine)
 * 
 * @param interactor Interactor instance
 * @return 0 on success, negative on error
 */
int scv_interactor_start(struct scv_interactor *interactor);

/**
 * Stop the interactor (e.g., stop its internal state machine)
 * 
 * @param interactor Interactor instance
 * @return 0 on success, negative on error
 */
int scv_interactor_stop(struct scv_interactor *interactor);

/**
 * Get interactor status as a human-readable string
 * 
 * @param interactor Interactor instance
 * @param status_buffer Buffer to store status string
 * @param buffer_size Size of buffer
 * @return 0 on success, negative on error
 */
int scv_interactor_get_status(const struct scv_interactor *interactor, char *status_buffer, uint32_t buffer_size);

#endif /* __SCV_INTERACTOR_H__ */
