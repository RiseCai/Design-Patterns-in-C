/**
 * scv_presenter.h - System Coordinator VIPER Presenter Component
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * Presenter component implemented as a Parallel Finite State Machine.
 * Coordinates multiple subsystem FSMs (recording, communication, power, audio, OTA).
 */

#ifndef __SCV_PRESENTER_H__
#define __SCV_PRESENTER_H__

#include <stdint.h>
#include <stdbool.h>
#include "../../state_machine_extended/src/parallel_fsm.h"
#include "scv_entity.h"
#include "scv_interactor.h"

/* Forward declarations for subsystem FSMs */
struct recording_fsm;
struct comm_fsm;
struct power_fsm;
struct audio_fsm;
struct ota_fsm;

/* Forward declarations for other VIPER components */
struct scv_view;
struct scv_router;

/**
 * Presenter States
 */
typedef enum {
    PRESENTER_STATE_INIT,
    PRESENTER_STATE_IDLE,
    PRESENTER_STATE_COORDINATING,
    PRESENTER_STATE_SYNCHRONIZING,
    PRESENTER_STATE_ERROR_HANDLING,
    PRESENTER_STATE_SHUTDOWN,
} scv_presenter_state_t;

/**
 * Subsystem Status
 */
typedef enum {
    SUBSYSTEM_STATUS_UNINITIALIZED,
    SUBSYSTEM_STATUS_INITIALIZING,
    SUBSYSTEM_STATUS_READY,
    SUBSYSTEM_STATUS_ACTIVE,
    SUBSYSTEM_STATUS_PAUSED,
    SUBSYSTEM_STATUS_ERROR,
    SUBSYSTEM_STATUS_SHUTDOWN,
} scv_subsystem_status_t;

/**
 * Subsystem Information
 */
struct scv_subsystem_info {
    /* Subsystem identifier */
    char subsystem_name[32];
    int subsystem_id;
    
    /* Current status */
    scv_subsystem_status_t status;
    
    /* Associated FSM */
    void *fsm_handle;
    
    /* Error information */
    int last_error_code;
    char last_error_msg[128];
    
    /* Performance metrics */
    uint32_t activation_count;
    uint64_t total_uptime_ms;
    uint32_t error_count;
};

/**
 * Coordination Mode
 */
typedef enum {
    COORDINATION_MODE_STANDALONE,    /* Each subsystem operates independently */
    COORDINATION_MODE_SEQUENTIAL,    /* Subsystems activated in sequence */
    COORDINATION_MODE_PARALLEL,      /* Subsystems activated in parallel */
    COORDINATION_MODE_PIPELINE,      /* Subsystems form a processing pipeline */
    COORDINATION_MODE_MASTER_SLAVE,  /* One master controls multiple slaves */
} scv_coordination_mode_t;

/**
 * Coordination Policy
 */
struct scv_coordination_policy {
    /* Mode of coordination */
    scv_coordination_mode_t mode;
    
    /* Priority levels for each subsystem (0-100, higher = more important) */
    uint32_t recording_priority;
    uint32_t communication_priority;
    uint32_t power_priority;
    uint32_t audio_priority;
    uint32_t ota_priority;
    
    /* Resource allocation limits */
    uint32_t max_cpu_usage_percent;      /* Maximum CPU usage across all subsystems */
    uint32_t max_memory_usage_kb;        /* Maximum memory usage across all subsystems */
    uint32_t max_power_consumption_mw;   /* Maximum power consumption */
    
    /* Timing constraints */
    uint32_t max_activation_delay_ms;    /* Maximum delay for subsystem activation */
    uint32_t max_synchronization_delay_ms; /* Maximum synchronization delay */
    
    /* Error handling policy */
    bool stop_all_on_subsystem_error;    /* Stop all subsystems if one fails */
    uint32_t error_recovery_attempts;    /* Number of recovery attempts */
    uint32_t error_recovery_delay_ms;    /* Delay between recovery attempts */
};

/**
 * System Coordinator VIPER Presenter
 */
struct scv_presenter {
    /* Parallel FSM for subsystem coordination */
    struct parallel_machine *parallel_fsm;
    
    /* Presenter state */
    scv_presenter_state_t state;
    
    /* Subsystem information */
    struct scv_subsystem_info recording_subsystem;
    struct scv_subsystem_info communication_subsystem;
    struct scv_subsystem_info power_subsystem;
    struct scv_subsystem_info audio_subsystem;
    struct scv_subsystem_info ota_subsystem;
    
    /* Coordination policy */
    struct scv_coordination_policy policy;
    
    /* Associated Entity and Interactor */
    struct scv_entity *entity;
    struct scv_interactor *interactor;
    
    /* Coordination context */
    scv_coordination_mode_t current_mode;
    bool is_synchronized;
    uint32_t synchronization_level;  /* 0-100% */
    
    /* Callbacks */
    void (*subsystem_status_changed_callback)(const char *subsystem_name,
                                              scv_subsystem_status_t old_status,
                                              scv_subsystem_status_t new_status,
                                              void *user_data);
    void (*coordination_event_callback)(const char *event_name,
                                        const void *event_data,
                                        void *user_data);
    void (*error_callback)(const char *subsystem_name,
                           int error_code,
                           const char *error_msg,
                           void *user_data);
    
    /* User data for callbacks */
    void *user_data;
};

/* Function prototypes */

/**
 * Initialize a System Coordinator VIPER Presenter
 * 
 * @param entity Associated entity (can be NULL, set later)
 * @param interactor Associated interactor (can be NULL, set later)
 * @param policy Coordination policy (can be NULL for defaults)
 * @return New presenter instance, NULL on error
 */
struct scv_presenter *scv_presenter_init(struct scv_entity *entity,
                                         struct scv_interactor *interactor,
                                         const struct scv_coordination_policy *policy);

/**
 * Destroy a System Coordinator VIPER Presenter
 * 
 * @param presenter Presenter to destroy
 */
void scv_presenter_destroy(struct scv_presenter *presenter);

/**
 * Register a subsystem FSM with the presenter
 * 
 * @param presenter Presenter instance
 * @param subsystem_name Name of the subsystem ("recording", "communication", "power", "audio", "ota")
 * @param fsm_handle Pointer to the subsystem FSM
 * @return 0 on success, negative on error
 */
int scv_presenter_register_subsystem(struct scv_presenter *presenter,
                                     const char *subsystem_name,
                                     void *fsm_handle);

/**
 * Unregister a subsystem FSM from the presenter
 * 
 * @param presenter Presenter instance
 * @param subsystem_name Name of the subsystem
 * @return 0 on success, negative on error
 */
int scv_presenter_unregister_subsystem(struct scv_presenter *presenter,
                                       const char *subsystem_name);

/**
 * Coordinate subsystems based on current mode and policy
 * 
 * @param presenter Presenter instance
 * @return 0 on success, negative on error
 */
int scv_presenter_coordinate(struct scv_presenter *presenter);

/**
 * Synchronize subsystem states
 * 
 * @param presenter Presenter instance
 * @param target_synchronization_level Desired synchronization level (0-100%)
 * @return Actual synchronization level achieved
 */
uint32_t scv_presenter_synchronize(struct scv_presenter *presenter,
                                   uint32_t target_synchronization_level);

/**
 * Activate a subsystem
 * 
 * @param presenter Presenter instance
 * @param subsystem_name Name of the subsystem to activate
 * @param activation_data Subsystem-specific activation data
 * @return 0 on success, negative on error
 */
int scv_presenter_activate_subsystem(struct scv_presenter *presenter,
                                     const char *subsystem_name,
                                     const void *activation_data);

/**
 * Deactivate a subsystem
 * 
 * @param presenter Presenter instance
 * @param subsystem_name Name of the subsystem to deactivate
 * @param deactivation_data Subsystem-specific deactivation data
 * @return 0 on success, negative on error
 */
int scv_presenter_deactivate_subsystem(struct scv_presenter *presenter,
                                       const char *subsystem_name,
                                       const void *deactivation_data);

/**
 * Pause a subsystem
 * 
 * @param presenter Presenter instance
 * @param subsystem_name Name of the subsystem to pause
 * @return 0 on success, negative on error
 */
int scv_presenter_pause_subsystem(struct scv_presenter *presenter,
                                  const char *subsystem_name);

/**
 * Resume a subsystem
 * 
 * @param presenter Presenter instance
 * @param subsystem_name Name of the subsystem to resume
 * @return 0 on success, negative on error
 */
int scv_presenter_resume_subsystem(struct scv_presenter *presenter,
                                   const char *subsystem_name);

/**
 * Get subsystem status
 * 
 * @param presenter Presenter instance
 * @param subsystem_name Name of the subsystem
 * @return Subsystem status, SUBSYSTEM_STATUS_UNINITIALIZED if not found
 */
scv_subsystem_status_t scv_presenter_get_subsystem_status(const struct scv_presenter *presenter,
                                                          const char *subsystem_name);

/**
 * Get subsystem information
 * 
 * @param presenter Presenter instance
 * @param subsystem_name Name of the subsystem
 * @return Subsystem information, NULL if not found
 */
const struct scv_subsystem_info *scv_presenter_get_subsystem_info(const struct scv_presenter *presenter,
                                                                  const char *subsystem_name);

/**
 * Update coordination policy
 * 
 * @param presenter Presenter instance
 * @param policy New coordination policy
 * @return 0 on success, negative on error
 */
int scv_presenter_update_policy(struct scv_presenter *presenter,
                                const struct scv_coordination_policy *policy);

/**
 * Get coordination policy
 * 
 * @param presenter Presenter instance
 * @return Current coordination policy
 */
const struct scv_coordination_policy *scv_presenter_get_policy(const struct scv_presenter *presenter);

/**
 * Set associated entity
 * 
 * @param presenter Presenter instance
 * @param entity Entity to associate
 */
void scv_presenter_set_entity(struct scv_presenter *presenter, struct scv_entity *entity);

/**
 * Set associated interactor
 * 
 * @param presenter Presenter instance
 * @param interactor Interactor to associate
 */
void scv_presenter_set_interactor(struct scv_presenter *presenter, struct scv_interactor *interactor);

/**
 * Process presenter event (for Parallel FSM)
 * 
 * @param presenter Presenter instance
 * @param event Event to process
 * @param event_data Event data
 * @return 0 on success, negative on error
 */
int scv_presenter_process_event(struct scv_presenter *presenter, int event, const void *event_data);

/**
 * Get presenter state
 * 
 * @param presenter Presenter instance
 * @return Current presenter state
 */
scv_presenter_state_t scv_presenter_get_state(const struct scv_presenter *presenter);

/**
 * Reset presenter to initial state
 * 
 * @param presenter Presenter instance
 * @return 0 on success, negative on error
 */
int scv_presenter_reset(struct scv_presenter *presenter);

/**
 * Set subsystem status changed callback
 * 
 * @param presenter Presenter instance
 * @param callback Callback function
 * @param user_data User data passed to callback
 */
void scv_presenter_set_subsystem_status_changed_callback(struct scv_presenter *presenter,
                                                         void (*callback)(const char *subsystem_name,
                                                                          scv_subsystem_status_t old_status,
                                                                          scv_subsystem_status_t new_status,
                                                                          void *user_data),
                                                         void *user_data);

/**
 * Set coordination event callback
 * 
 * @param presenter Presenter instance
 * @param callback Callback function
 * @param user_data User data passed to callback
 */
void scv_presenter_set_coordination_event_callback(struct scv_presenter *presenter,
                                                   void (*callback)(const char *event_name,
                                                                    const void *event_data,
                                                                    void *user_data),
                                                   void *user_data);

/**
 * Set error callback
 * 
 * @param presenter Presenter instance
 * @param callback Callback function
 * @param user_data User data passed to callback
 */
void scv_presenter_set_error_callback(struct scv_presenter *presenter,
                                      void (*callback)(const char *subsystem_name,
                                                       int error_code,
                                                       const char *error_msg,
                                                       void *user_data),
                                      void *user_data);

/**
 * Start presenter component
 * 
 * @param presenter Presenter instance
 * @return 0 on success, negative on error
 */
int scv_presenter_start(struct scv_presenter *presenter);

/**
 * Stop presenter component
 * 
 * @param presenter Presenter instance
 * @return 0 on success, negative on error
 */
int scv_presenter_stop(struct scv_presenter *presenter);

/**
 * Get presenter status string
 * 
 * @param presenter Presenter instance
 * @param status_buffer Buffer to store status string
 * @param buffer_size Size of buffer
 * @return 0 on success, negative on error
 */
int scv_presenter_get_status(const struct scv_presenter *presenter,
                             char *status_buffer,
                             uint32_t buffer_size);

/**
 * Update presenter configuration
 * 
 * @param presenter Presenter instance
 * @param config New configuration (coordination policy)
 * @return 0 on success, negative on error
 */
int scv_presenter_update_config(struct scv_presenter *presenter,
                                const struct scv_coordination_policy *config);

/**
 * Set associated view
 * 
 * @param presenter Presenter instance
 * @param view View to associate
 */
void scv_presenter_set_view(struct scv_presenter *presenter, struct scv_view *view);

/**
 * Set associated router
 * 
 * @param presenter Presenter instance
 * @param router Router to associate
 */
void scv_presenter_set_router(struct scv_presenter *presenter, struct scv_router *router);

/**
 * Process coordination event
 * 
 * @param presenter Presenter instance
 * @param event_data Event data
 * @param event_data_size Size of event data
 * @return 0 on success, negative on error
 */
int scv_presenter_process_coordination(struct scv_presenter *presenter,
                                       const void *event_data,
                                       uint32_t event_data_size);

#endif /* __SCV_PRESENTER_H__ */
