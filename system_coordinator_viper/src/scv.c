/**
 * scv.c - System Coordinator VIPER Main Implementation
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * Main implementation file for System Coordinator VIPER architecture.
 * Integrates all VIPER components with state machine implementations.
 */

#include "scv.h"
#include <stdlib.h>
#include <string.h>

/* Default configuration */
static const struct scv_config default_config = {
    .entity_config = {
        .entity_name = "SystemCoordinator",
        .entity_id = 1000,
        .status_update_interval = 1000,
        .metrics_update_interval = 5000,
        .max_storage_mb = 4096,
        .max_recording_duration_sec = 3600,
        .wifi_ssid = "",
        .wifi_password = "",
        .server_url = "https://api.example.com",
        .ota_server_url = "https://ota.example.com",
        .ota_check_interval_sec = 86400
    },
    .business_rules = {
        .min_battery_for_recording = 20,
        .min_storage_for_recording_mb = 100,
        .max_recording_duration_sec = 3600,
        .min_battery_for_upload = 30,
        .min_network_strength_for_upload = 50,
        .upload_only_on_wifi = true,
        .max_upload_retries = 3,
        .min_battery_for_ota = 50,
        .ota_only_on_wifi = true,
        .ota_only_when_charging = true,
        .sleep_battery_threshold = 15,
        .critical_battery_threshold = 5,
        .max_consecutive_errors = 5,
        .error_recovery_delay_ms = 5000
    },
    .coordination_policy = {
        .mode = COORDINATION_MODE_PARALLEL,
        .recording_priority = 80,
        .communication_priority = 70,
        .power_priority = 90,
        .audio_priority = 60,
        .ota_priority = 50,
        .max_cpu_usage_percent = 80,
        .max_memory_usage_kb = 1024,
        .max_power_consumption_mw = 500,
        .max_activation_delay_ms = 1000,
        .max_synchronization_delay_ms = 500,
        .stop_all_on_subsystem_error = false,
        .error_recovery_attempts = 3,
        .error_recovery_delay_ms = 1000
    },
    .ui_config = {
        .screen_width = 320,
        .screen_height = 240,
        .screen_refresh_rate_hz = 60,
        .touch_sensitivity = 50,
        .button_debounce_ms = 20,
        .encoder_resolution = 24,
        .enable_visual_feedback = true,
        .enable_audio_feedback = false,
        .enable_haptic_feedback = true,
        .screen_timeout_ms = 30000,
        .input_timeout_ms = 5000,
        .theme_name = "Default",
        .theme_color_primary = 0x007BFF,
        .theme_color_secondary = 0x6C757D,
        .theme_font_size = 12
    },
    .routing_config = {
        .max_routing_table_entries = 50,
        .max_pattern_length = 128,
        .enable_pattern_caching = true,
        .enable_route_optimization = true,
        .max_route_hops = 10,
        .route_timeout_ms = 5000,
        .max_pattern_matches = 10,
        .case_sensitive_matching = false,
        .routing_thread_priority = 5,
        .max_concurrent_routes = 20
    },
    .system_name = "System Coordinator VIPER",
    .system_id = 1,
    .log_level = 2,
    .enable_debug = false
};

struct scv_system *scv_init(const struct scv_config *config)
{
    struct scv_system *system = (struct scv_system *)malloc(sizeof(struct scv_system));
    if (!system) {
        return NULL;
    }
    
    memset(system, 0, sizeof(struct scv_system));
    
    /* Copy configuration */
    if (config) {
        system->config = *config;
    } else {
        system->config = default_config;
    }
    
    /* Initialize components */
    system->entity = scv_entity_init(&system->config.entity_config);
    if (!system->entity) {
        free(system);
        return NULL;
    }
    
    system->interactor = scv_interactor_init(system->entity, &system->config.business_rules);
    if (!system->interactor) {
        scv_entity_destroy(system->entity);
        free(system);
        return NULL;
    }
    
    system->presenter = scv_presenter_init(system->entity, system->interactor, &system->config.coordination_policy);
    if (!system->presenter) {
        scv_interactor_destroy(system->interactor);
        scv_entity_destroy(system->entity);
        free(system);
        return NULL;
    }
    
    system->view = scv_view_init(&system->config.ui_config,
                                 system->entity,
                                 system->interactor,
                                 system->presenter);
    if (!system->view) {
        scv_presenter_destroy(system->presenter);
        scv_interactor_destroy(system->interactor);
        scv_entity_destroy(system->entity);
        free(system);
        return NULL;
    }
    
    system->router = scv_router_init(&system->config.routing_config,
                                     system->entity,
                                     system->interactor,
                                     system->presenter,
                                     system->view);
    if (!system->router) {
        scv_view_destroy(system->view);
        scv_presenter_destroy(system->presenter);
        scv_interactor_destroy(system->interactor);
        scv_entity_destroy(system->entity);
        free(system);
        return NULL;
    }
    
    /* Set component associations */
    scv_interactor_set_entity(system->interactor, system->entity);
    scv_interactor_set_presenter(system->interactor, system->presenter);
    scv_interactor_set_view(system->interactor, system->view);
    scv_interactor_set_router(system->interactor, system->router);
    
    scv_presenter_set_entity(system->presenter, system->entity);
    scv_presenter_set_interactor(system->presenter, system->interactor);
    scv_presenter_set_view(system->presenter, system->view);
    scv_presenter_set_router(system->presenter, system->router);
    
    scv_view_set_entity(system->view, system->entity);
    scv_view_set_interactor(system->view, system->interactor);
    scv_view_set_presenter(system->view, system->presenter);
    scv_view_set_router(system->view, system->router);
    
    scv_router_set_entity(system->router, system->entity);
    scv_router_set_interactor(system->router, system->interactor);
    scv_router_set_presenter(system->router, system->presenter);
    scv_router_set_view(system->router, system->view);
    
    system->is_initialized = true;
    system->is_running = false;
    system->run_time_ms = 0;
    system->last_error_code = 0;
    system->last_error_msg[0] = '\0';
    
    return system;
}

void scv_destroy(struct scv_system *system)
{
    if (!system) {
        return;
    }
    
    /* Destroy components in reverse order */
    if (system->router) {
        scv_router_destroy(system->router);
    }
    if (system->view) {
        scv_view_destroy(system->view);
    }
    if (system->presenter) {
        scv_presenter_destroy(system->presenter);
    }
    if (system->interactor) {
        scv_interactor_destroy(system->interactor);
    }
    if (system->entity) {
        scv_entity_destroy(system->entity);
    }
    
    free(system);
}

int scv_start(struct scv_system *system)
{
    if (!system || !system->is_initialized) {
        return -1;
    }
    
    if (system->is_running) {
        return 0; /* Already running */
    }
    
    /* Start components */
    int ret = scv_entity_process_event(system->entity, ENTITY_EVENT_INIT_COMPLETE, NULL);
    if (ret != 0) {
        snprintf(system->last_error_msg, sizeof(system->last_error_msg),
                 "Entity start failed: %d", ret);
        system->last_error_code = ret;
        return -1;
    }
    
    ret = scv_interactor_start(system->interactor);
    if (ret != 0) {
        snprintf(system->last_error_msg, sizeof(system->last_error_msg),
                 "Interactor start failed: %d", ret);
        system->last_error_code = ret;
        return -1;
    }
    
    ret = scv_presenter_start(system->presenter);
    if (ret != 0) {
        snprintf(system->last_error_msg, sizeof(system->last_error_msg),
                 "Presenter start failed: %d", ret);
        system->last_error_code = ret;
        return -1;
    }
    
    ret = scv_view_start(system->view);
    if (ret != 0) {
        snprintf(system->last_error_msg, sizeof(system->last_error_msg),
                 "View start failed: %d", ret);
        system->last_error_code = ret;
        return -1;
    }
    
    ret = scv_router_start(system->router);
    if (ret != 0) {
        snprintf(system->last_error_msg, sizeof(system->last_error_msg),
                 "Router start failed: %d", ret);
        system->last_error_code = ret;
        return -1;
    }
    
    system->is_running = true;
    return 0;
}

int scv_stop(struct scv_system *system)
{
    if (!system || !system->is_initialized) {
        return -1;
    }
    
    if (!system->is_running) {
        return 0; /* Already stopped */
    }
    
    /* Stop components */
    scv_router_stop(system->router);
    scv_view_stop(system->view);
    scv_presenter_stop(system->presenter);
    scv_interactor_stop(system->interactor);
    scv_entity_process_event(system->entity, ENTITY_EVENT_RESET, NULL);
    
    system->is_running = false;
    return 0;
}

int scv_process_event(struct scv_system *system, 
                      int event_type, 
                      const void *event_data, 
                      uint32_t event_data_size)
{
    if (!system || !system->is_initialized) {
        return -1;
    }
    
    /* Route event through router */
    char event_msg[256];
    snprintf(event_msg, sizeof(event_msg), "EVENT:%d", event_type);
    
    struct scv_routing_context context = {0};
    scv_routing_decision_t decision = scv_router_route_message(
        system->router, event_msg, strlen(event_msg), &context);
    
    if (decision == ROUTING_DECISION_STOP) {
        return 0; /* Event handled */
    } else if (decision == ROUTING_DECISION_DROP) {
        return -1; /* Event dropped */
    }
    
    /* Default handling based on event type */
    switch (event_type) {
        case SCV_SYSTEM_EVENT_USER_INPUT:
            return scv_view_process_input(system->view, event_data, event_data_size);
        case SCV_SYSTEM_EVENT_SYSTEM_UPDATE:
            return scv_entity_process_event(system->entity, ENTITY_EVENT_UPDATE_REQUEST, event_data);
        case SCV_SYSTEM_EVENT_BUSINESS_RULE:
            return scv_interactor_process_rule(system->interactor, event_data, event_data_size);
        case SCV_SYSTEM_EVENT_COORDINATION:
            return scv_presenter_process_coordination(system->presenter, event_data, event_data_size);
        default:
            snprintf(system->last_error_msg, sizeof(system->last_error_msg),
                     "Unknown event type: %d", event_type);
            system->last_error_code = -1;
            return -1;
    }
}

int scv_get_status(const struct scv_system *system, 
                   char *status_buffer, 
                   uint32_t buffer_size)
{
    if (!system || !status_buffer || buffer_size == 0) {
        return -1;
    }
    
    char entity_status[256] = {0};
    char interactor_status[256] = {0};
    char presenter_status[256] = {0};
    char view_status[256] = {0};
    char router_status[256] = {0};
    
    /* Get component statuses */
    scv_entity_get_status(system->entity, entity_status, sizeof(entity_status));
    scv_interactor_get_status(system->interactor, interactor_status, sizeof(interactor_status));
    scv_presenter_get_status(system->presenter, presenter_status, sizeof(presenter_status));
    scv_view_get_status(system->view, view_status, sizeof(view_status));
    scv_router_get_status(system->router, router_status, sizeof(router_status));
    
    snprintf(status_buffer, buffer_size,
             "System Coordinator VIPER Status:\n"
             "  System: %s (ID: %u)\n"
             "  State: %s, Running: %s\n"
             "  Entity: %s\n"
             "  Interactor: %s\n"
             "  Presenter: %s\n"
             "  View: %s\n"
             "  Router: %s\n"
             "  Run Time: %u ms\n",
             system->config.system_name,
             system->config.system_id,
             system->is_initialized ? "Initialized" : "Not Initialized",
             system->is_running ? "Yes" : "No",
             entity_status,
             interactor_status,
             presenter_status,
             view_status,
             router_status,
             system->run_time_ms);
    
    return 0;
}

int scv_update_config(struct scv_system *system, const struct scv_config *config)
{
    if (!system || !config) {
        return -1;
    }
    
    /* Update component configurations */
    scv_entity_update_config(system->entity, &config->entity_config);
    scv_interactor_update_rules(system->interactor, &config->business_rules);
    scv_presenter_update_config(system->presenter, &config->coordination_policy);
    scv_view_update_config(system->view, &config->ui_config);
    scv_router_update_config(system->router, &config->routing_config);
    
    /* Update system configuration */
    system->config = *config;
    return 0;
}

const struct scv_config *scv_get_config(const struct scv_system *system)
{
    if (!system) {
        return NULL;
    }
    return &system->config;
}

int scv_reset(struct scv_system *system)
{
    if (!system) {
        return -1;
    }
    
    /* Stop if running */
    if (system->is_running) {
        scv_stop(system);
    }
    
    /* Reset components */
    scv_entity_reset(system->entity);
    scv_interactor_reset(system->interactor);
    scv_presenter_reset(system->presenter);
    scv_view_reset(system->view);
    scv_router_reset(system->router);
    
    system->run_time_ms = 0;
    system->last_error_code = 0;
    system->last_error_msg[0] = '\0';
    
    return 0;
}

int scv_get_last_error(const struct scv_system *system, 
                       int *error_code, 
                       char *error_msg, 
                       uint32_t error_msg_size)
{
    if (!system || !error_code || !error_msg) {
        return -1;
    }
    
    *error_code = system->last_error_code;
    strncpy(error_msg, system->last_error_msg, error_msg_size);
    if (error_msg_size > 0) {
        error_msg[error_msg_size - 1] = '\0';
    }
    
    return 0;
}

void scv_register_event_callback(struct scv_system *system,
                                 void (*callback)(int event_type, 
                                                  const void *event_data,
                                                  void *user_data),
                                 void *user_data)
{
    if (!system) {
        return;
    }
    
    /* Register with appropriate component based on system design */
    /* For simplicity, store in system structure for now */
    (void)callback;
    (void)user_data;
    /* TODO: Implement callback registration */
}

void scv_register_status_changed_callback(struct scv_system *system,
                                          void (*callback)(const char *status,
                                                           void *user_data),
                                          void *user_data)
{
    if (!system) {
        return;
    }
    
    /* Register with appropriate component */
    (void)callback;
    (void)user_data;
    /* TODO: Implement callback registration */
}

void scv_get_components(const struct scv_system *system,
                        struct scv_entity **entity,
                        struct scv_interactor **interactor,
                        struct scv_presenter **presenter,
                        struct scv_view **view,
                        struct scv_router **router)
{
    if (!system) {
        return;
    }
    
    if (entity) {
        *entity = system->entity;
    }
    if (interactor) {
        *interactor = system->interactor;
    }
    if (presenter) {
        *presenter = system->presenter;
    }
    if (view) {
        *view = system->view;
    }
    if (router) {
        *router = system->router;
    }
}
