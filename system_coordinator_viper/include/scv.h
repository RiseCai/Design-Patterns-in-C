/**
 * scv.h - System Coordinator VIPER Main Header
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * Main header file for System Coordinator VIPER architecture.
 * Integrates all VIPER components with state machine implementations.
 */

#ifndef __SCV_H__
#define __SCV_H__

#include "scv_entity.h"
#include "scv_interactor.h"
#include "scv_presenter.h"
#include "scv_view.h"
#include "scv_router.h"

/**
 * System Event Types (for scv_process_event)
 * Note: These are different from scv_event_bus.h event types
 */
typedef enum {
    SCV_SYSTEM_EVENT_USER_INPUT = 0,
    SCV_SYSTEM_EVENT_SYSTEM_UPDATE,
    SCV_SYSTEM_EVENT_BUSINESS_RULE,
    SCV_SYSTEM_EVENT_COORDINATION,
    SCV_SYSTEM_EVENT_NETWORK_CHANGE,
    SCV_SYSTEM_EVENT_POWER_CHANGE,
    SCV_SYSTEM_EVENT_ERROR,
    SCV_SYSTEM_EVENT_RESET,
} scv_system_event_type_t;


/**
 * System Coordinator VIPER Configuration
 */
struct scv_config {
    /* Entity configuration */
    struct scv_entity_config entity_config;
    
    /* Interactor configuration */
    struct scv_business_rules business_rules;
    
    /* Presenter configuration */
    struct scv_coordination_policy coordination_policy;
    
    /* View configuration */
    struct scv_ui_config ui_config;
    
    /* Router configuration */
    struct scv_routing_config routing_config;
    
    /* System-wide settings */
    char system_name[64];
    uint32_t system_id;
    uint32_t log_level;
    bool enable_debug;
};

/**
 * System Coordinator VIPER Instance
 */
struct scv_system {
    /* VIPER components */
    struct scv_entity *entity;
    struct scv_interactor *interactor;
    struct scv_presenter *presenter;
    struct scv_view *view;
    struct scv_router *router;
    
    /* System configuration */
    struct scv_config config;
    
    /* System state */
    bool is_initialized;
    bool is_running;
    uint32_t run_time_ms;
    
    /* Error handling */
    int last_error_code;
    char last_error_msg[256];
};

/* Function prototypes */

/**
 * Initialize a System Coordinator VIPER system
 * 
 * @param config System configuration (can be NULL for defaults)
 * @return New system instance, NULL on error
 */
struct scv_system *scv_init(const struct scv_config *config);

/**
 * Destroy a System Coordinator VIPER system
 * 
 * @param system System to destroy
 */
void scv_destroy(struct scv_system *system);

/**
 * Start the System Coordinator VIPER system
 * 
 * @param system System instance
 * @return 0 on success, negative on error
 */
int scv_start(struct scv_system *system);

/**
 * Stop the System Coordinator VIPER system
 * 
 * @param system System instance
 * @return 0 on success, negative on error
 */
int scv_stop(struct scv_system *system);

/**
 * Process a system event
 * 
 * @param system System instance
 * @param event_type Type of event
 * @param event_data Event data
 * @param event_data_size Size of event data
 * @return 0 on success, negative on error
 */
int scv_process_event(struct scv_system *system, 
                      int event_type, 
                      const void *event_data, 
                      uint32_t event_data_size);

/**
 * Get system status
 * 
 * @param system System instance
 * @param status_buffer Buffer to store status string
 * @param buffer_size Size of buffer
 * @return 0 on success, negative on error
 */
int scv_get_status(const struct scv_system *system, 
                   char *status_buffer, 
                   uint32_t buffer_size);

/**
 * Update system configuration
 * 
 * @param system System instance
 * @param config New configuration
 * @return 0 on success, negative on error
 */
int scv_update_config(struct scv_system *system, const struct scv_config *config);

/**
 * Get system configuration
 * 
 * @param system System instance
 * @return Current configuration
 */
const struct scv_config *scv_get_config(const struct scv_system *system);

/**
 * Reset system to initial state
 * 
 * @param system System instance
 * @return 0 on success, negative on error
 */
int scv_reset(struct scv_system *system);

/**
 * Get last error information
 * 
 * @param system System instance
 * @param error_code Output parameter for error code
 * @param error_msg Buffer for error message
 * @param error_msg_size Size of error message buffer
 * @return 0 on success, negative on error
 */
int scv_get_last_error(const struct scv_system *system, 
                       int *error_code, 
                       char *error_msg, 
                       uint32_t error_msg_size);

/**
 * Register system event callback
 * 
 * @param system System instance
 * @param callback Callback function
 * @param user_data User data passed to callback
 */
void scv_register_event_callback(struct scv_system *system,
                                 void (*callback)(int event_type, 
                                                  const void *event_data,
                                                  void *user_data),
                                 void *user_data);

/**
 * Register system status changed callback
 * 
 * @param system System instance
 * @param callback Callback function
 * @param user_data User data passed to callback
 */
void scv_register_status_changed_callback(struct scv_system *system,
                                          void (*callback)(const char *status,
                                                           void *user_data),
                                          void *user_data);

/**
 * Get component handles for advanced operations
 * 
 * @param system System instance
 * @param entity Output parameter for entity handle (can be NULL)
 * @param interactor Output parameter for interactor handle (can be NULL)
 * @param presenter Output parameter for presenter handle (can be NULL)
 * @param view Output parameter for view handle (can be NULL)
 * @param router Output parameter for router handle (can be NULL)
 */
void scv_get_components(const struct scv_system *system,
                        struct scv_entity **entity,
                        struct scv_interactor **interactor,
                        struct scv_presenter **presenter,
                        struct scv_view **view,
                        struct scv_router **router);

#endif /* __SCV_H__ */
