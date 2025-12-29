/**
 * scv_view.h - System Coordinator VIPER View Component
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * View component implemented as a Mealy State Machine.
 * Handles user interface events and produces immediate responses.
 */

#ifndef __SCV_VIEW_H__
#define __SCV_VIEW_H__

#include <stdint.h>
#include <stdbool.h>
#include "../../state_machine_extended/src/mealy_machine.h"
#include "scv_entity.h"
#include "scv_interactor.h"
#include "scv_presenter.h"

/* Forward declaration for router */
struct scv_router;

/**
 * View States
 */
typedef enum {
    VIEW_STATE_INIT,
    VIEW_STATE_IDLE,
    VIEW_STATE_DISPLAYING,
    VIEW_STATE_INTERACTING,
    VIEW_STATE_PROCESSING,
    VIEW_STATE_ERROR,
} scv_view_state_t;

/**
 * UI Event Types
 */
typedef enum {
    UI_EVT_BUTTON_PRESS,
    UI_EVT_BUTTON_RELEASE,
    UI_EVT_TOUCH_START,
    UI_EVT_TOUCH_MOVE,
    UI_EVT_TOUCH_END,
    UI_EVT_ENCODER_ROTATE,
    UI_EVT_ENCODER_PRESS,
    UI_EVT_SWITCH_TOGGLE,
    UI_EVT_GESTURE,
    UI_EVT_VOICE_COMMAND,
    UI_EVT_TIMEOUT,
    UI_EVT_SYSTEM_NOTIFICATION,
} scv_ui_event_type_t;

/**
 * UI Event Data
 */
struct scv_ui_event {
    scv_ui_event_type_t type;
    
    union {
        /* Button events */
        struct {
            uint32_t button_id;
            uint32_t press_duration_ms;
        } button;
        
        /* Touch events */
        struct {
            uint32_t x;
            uint32_t y;
            uint32_t pressure;
        } touch;
        
        /* Encoder events */
        struct {
            int32_t delta;  /* Positive for clockwise, negative for counter-clockwise */
            bool is_pressed;
        } encoder;
        
        /* Switch events */
        struct {
            uint32_t switch_id;
            bool new_state;  /* true = on, false = off */
        } switch_event;
        
        /* Gesture events */
        struct {
            uint32_t gesture_id;
            uint32_t confidence;  /* 0-100% */
        } gesture;
        
        /* Voice command events */
        struct {
            char command[64];
            uint32_t confidence;  /* 0-100% */
        } voice;
        
        /* Timeout events */
        struct {
            uint32_t timeout_id;
        } timeout;
        
        /* System notification events */
        struct {
            uint32_t notification_id;
            void *notification_data;
        } notification;
    } data;
    
    /* Timestamp */
    uint64_t timestamp_ms;
};

/**
 * UI Response Types
 */
typedef enum {
    UI_RESPONSE_NONE,
    UI_RESPONSE_VISUAL_FEEDBACK,
    UI_RESPONSE_AUDIO_FEEDBACK,
    UI_RESPONSE_HAPTIC_FEEDBACK,
    UI_RESPONSE_UPDATE_DISPLAY,
    UI_RESPONSE_SHOW_DIALOG,
    UI_RESPONSE_SHOW_NOTIFICATION,
    UI_RESPONSE_NAVIGATE,
    UI_RESPONSE_EXECUTE_COMMAND,
} scv_ui_response_type_t;

/**
 * UI Response Data
 */
struct scv_ui_response {
    scv_ui_response_type_t type;
    
    union {
        /* Visual feedback */
        struct {
            uint32_t led_pattern;
            uint32_t led_duration_ms;
            uint32_t led_color;
        } visual;
        
        /* Audio feedback */
        struct {
            uint32_t tone_frequency_hz;
            uint32_t tone_duration_ms;
            uint32_t volume;  /* 0-100% */
        } audio;
        
        /* Haptic feedback */
        struct {
            uint32_t vibration_pattern;
            uint32_t vibration_duration_ms;
            uint32_t intensity;  /* 0-100% */
        } haptic;
        
        /* Display update */
        struct {
            char display_text[256];
            uint32_t display_page;
            bool refresh_full;
        } display;
        
        /* Dialog */
        struct {
            char dialog_title[64];
            char dialog_message[256];
            uint32_t dialog_type;  /* 0 = info, 1 = warning, 2 = error, 3 = confirmation */
        } dialog;
        
        /* Notification */
        struct {
            char notification_title[64];
            char notification_message[256];
            uint32_t notification_duration_ms;
        } notification;
        
        /* Navigation */
        struct {
            char target_screen[64];
            void *navigation_data;
        } navigate;
        
        /* Command execution */
        struct {
            char command_name[64];
            void *command_data;
        } command;
    } data;
    
    /* Priority (higher = more important) */
    uint32_t priority;
};

/**
 * UI Configuration
 */
struct scv_ui_config {
    /* Display settings */
    uint32_t screen_width;
    uint32_t screen_height;
    uint32_t screen_refresh_rate_hz;
    
    /* Input settings */
    uint32_t touch_sensitivity;
    uint32_t button_debounce_ms;
    uint32_t encoder_resolution;
    
    /* Feedback settings */
    bool enable_visual_feedback;
    bool enable_audio_feedback;
    bool enable_haptic_feedback;
    
    /* Timeout settings */
    uint32_t screen_timeout_ms;
    uint32_t input_timeout_ms;
    
    /* UI theme */
    char theme_name[32];
    uint32_t theme_color_primary;
    uint32_t theme_color_secondary;
    uint32_t theme_font_size;
};

/**
 * System Coordinator VIPER View
 */
struct scv_view {
    /* Mealy State Machine for UI event handling */
    struct mealy_machine *mealy_fsm;
    
    /* UI configuration */
    struct scv_ui_config config;
    
    /* Associated components */
    struct scv_entity *entity;
    struct scv_interactor *interactor;
    struct scv_presenter *presenter;
    struct scv_router *router;
    
    /* UI state */
    scv_view_state_t current_state;
    uint32_t active_screen_id;
    bool is_interactive;
    
    /* Event queue */
    struct scv_ui_event *event_queue;
    uint32_t event_queue_size;
    uint32_t event_queue_head;
    uint32_t event_queue_tail;
    
    /* Response queue */
    struct scv_ui_response *response_queue;
    uint32_t response_queue_size;
    uint32_t response_queue_head;
    uint32_t response_queue_tail;
    
    /* Callbacks */
    void (*ui_event_received_callback)(const struct scv_ui_event *event, void *user_data);
    void (*ui_response_ready_callback)(const struct scv_ui_response *response, void *user_data);
    void (*ui_state_changed_callback)(scv_view_state_t new_state, 
                                      scv_view_state_t old_state,
                                      void *user_data);
    
    /* User data for callbacks */
    void *user_data;
};

/* Function prototypes */

/**
 * Initialize a System Coordinator VIPER View
 * 
 * @param config UI configuration (can be NULL for defaults)
 * @param entity Associated entity (can be NULL, set later)
 * @param interactor Associated interactor (can be NULL, set later)
 * @param presenter Associated presenter (can be NULL, set later)
 * @return New view instance, NULL on error
 */
struct scv_view *scv_view_init(const struct scv_ui_config *config,
                               struct scv_entity *entity,
                               struct scv_interactor *interactor,
                               struct scv_presenter *presenter);

/**
 * Destroy a System Coordinator VIPER View
 * 
 * @param view View to destroy
 */
void scv_view_destroy(struct scv_view *view);

/**
 * Process a UI event
 * 
 * @param view View instance
 * @param event UI event to process
 * @return 0 on success, negative on error
 */
int scv_view_process_event(struct scv_view *view, const struct scv_ui_event *event);

/**
 * Queue a UI event for processing
 * 
 * @param view View instance
 * @param event UI event to queue
 * @return 0 on success, negative on error
 */
int scv_view_queue_event(struct scv_view *view, const struct scv_ui_event *event);

/**
 * Process all queued UI events
 * 
 * @param view View instance
 * @return Number of events processed
 */
uint32_t scv_view_process_queued_events(struct scv_view *view);

/**
 * Get next UI response
 * 
 * @param view View instance
 * @param response Output parameter for response
 * @return 0 if response available, negative if no response
 */
int scv_view_get_response(struct scv_view *view, struct scv_ui_response *response);

/**
 * Queue a UI response
 * 
 * @param view View instance
 * @param response UI response to queue
 * @return 0 on success, negative on error
 */
int scv_view_queue_response(struct scv_view *view, const struct scv_ui_response *response);

/**
 * Update UI configuration
 * 
 * @param view View instance
 * @param config New UI configuration
 * @return 0 on success, negative on error
 */
int scv_view_update_config(struct scv_view *view, const struct scv_ui_config *config);

/**
 * Get UI configuration
 * 
 * @param view View instance
 * @return Current UI configuration
 */
const struct scv_ui_config *scv_view_get_config(const struct scv_view *view);

/**
 * Set associated entity
 * 
 * @param view View instance
 * @param entity Entity to associate
 */
void scv_view_set_entity(struct scv_view *view, struct scv_entity *entity);

/**
 * Set associated interactor
 * 
 * @param view View instance
 * @param interactor Interactor to associate
 */
void scv_view_set_interactor(struct scv_view *view, struct scv_interactor *interactor);

/**
 * Set associated presenter
 * 
 * @param view View instance
 * @param presenter Presenter to associate
 */
void scv_view_set_presenter(struct scv_view *view, struct scv_presenter *presenter);

/**
 * Set associated router
 * 
 * @param view View instance
 * @param router Router to associate
 */
void scv_view_set_router(struct scv_view *view, struct scv_router *router);

/**
 * Get view state
 * 
 * @param view View instance
 * @return Current view state
 */
scv_view_state_t scv_view_get_state(const struct scv_view *view);

/**
 * Set active screen
 * 
 * @param view View instance
 * @param screen_id Screen identifier
 * @return 0 on success, negative on error
 */
int scv_view_set_active_screen(struct scv_view *view, uint32_t screen_id);

/**
 * Get active screen
 * 
 * @param view View instance
 * @return Active screen identifier
 */
uint32_t scv_view_get_active_screen(const struct scv_view *view);

/**
 * Set UI event received callback
 * 
 * @param view View instance
 * @param callback Callback function
 * @param user_data User data passed to callback
 */
void scv_view_set_ui_event_received_callback(struct scv_view *view,
                                             void (*callback)(const struct scv_ui_event *event, void *user_data),
                                             void *user_data);

/**
 * Set UI response ready callback
 * 
 * @param view View instance
 * @param callback Callback function
 * @param user_data User data passed to callback
 */
void scv_view_set_ui_response_ready_callback(struct scv_view *view,
                                             void (*callback)(const struct scv_ui_response *response, void *user_data),
                                             void *user_data);

/**
 * Set UI state changed callback
 * 
 * @param view View instance
 * @param callback Callback function
 * @param user_data User data passed to callback
 */
void scv_view_set_ui_state_changed_callback(struct scv_view *view,
                                            void (*callback)(scv_view_state_t new_state,
                                                             scv_view_state_t old_state,
                                                             void *user_data),
                                            void *user_data);

/**
 * Reset view to initial state
 *
 * @param view View instance
 * @return 0 on success, negative on error
 */
int scv_view_reset(struct scv_view *view);

/**
 * Start the view component
 *
 * @param view View instance
 * @return 0 on success, negative on error
 */
int scv_view_start(struct scv_view *view);

/**
 * Stop the view component
 *
 * @param view View instance
 * @return 0 on success, negative on error
 */
int scv_view_stop(struct scv_view *view);

/**
 * Process generic input (for compatibility with scv_process_event)
 *
 * @param view View instance
 * @param input_data Input data
 * @param input_size Size of input data
 * @return 0 on success, negative on error
 */
int scv_view_process_input(struct scv_view *view, const void *input_data, uint32_t input_size);

/**
 * Get view status string
 *
 * @param view View instance
 * @param status_buffer Buffer to store status string
 * @param buffer_size Size of buffer
 * @return 0 on success, negative on error
 */
int scv_view_get_status(const struct scv_view *view, char *status_buffer, uint32_t buffer_size);

#endif /* __SCV_VIEW_H__ */
