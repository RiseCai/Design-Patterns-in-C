/**
 * scv_view.c - System Coordinator VIPER View Component
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * View component implemented as a Mealy State Machine.
 * Handles user interface events and produces immediate responses.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scv_view.h"
#include "../../state_machine_extended/src/mealy_machine.h"

/* Default UI configuration */
static const struct scv_ui_config DEFAULT_CONFIG = {
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
    .theme_name = "default",
    .theme_color_primary = 0x007BFF,
    .theme_color_secondary = 0x6C757D,
    .theme_font_size = 12
};

/* Internal helper functions */
static void init_event_queue(struct scv_view *view, uint32_t capacity);
static void free_event_queue(struct scv_view *view);
static void init_response_queue(struct scv_view *view, uint32_t capacity);
static void free_response_queue(struct scv_view *view);
static int enqueue_event(struct scv_view *view, const struct scv_ui_event *event);
static int dequeue_event(struct scv_view *view, struct scv_ui_event *event);
static int enqueue_response(struct scv_view *view, const struct scv_ui_response *response);
static int dequeue_response(struct scv_view *view, struct scv_ui_response *response);
static void update_view_state(struct scv_view *view, scv_view_state_t new_state);

/* Mealy FSM state definitions - simplified */
static struct mealy_state *create_mealy_state(scv_view_state_t state);
static void destroy_mealy_state(struct mealy_state *state);

/* Mealy state operations stubs */
static void mealy_state_init_event_click(struct mealy_state *state, struct mealy_machine *machine) {}
static void mealy_state_init_event_hover(struct mealy_state *state, struct mealy_machine *machine) {}
static void mealy_state_init_event_keypress(struct mealy_state *state, struct mealy_machine *machine, int key) {}
static void mealy_state_init_render(struct mealy_state *state, struct mealy_machine *machine) {}

static void mealy_state_idle_event_click(struct mealy_state *state, struct mealy_machine *machine) {}
static void mealy_state_idle_event_hover(struct mealy_state *state, struct mealy_machine *machine) {}
static void mealy_state_idle_event_keypress(struct mealy_state *state, struct mealy_machine *machine, int key) {}
static void mealy_state_idle_render(struct mealy_state *state, struct mealy_machine *machine) {}

static void mealy_state_displaying_event_click(struct mealy_state *state, struct mealy_machine *machine) {}
static void mealy_state_displaying_event_hover(struct mealy_state *state, struct mealy_machine *machine) {}
static void mealy_state_displaying_event_keypress(struct mealy_state *state, struct mealy_machine *machine, int key) {}
static void mealy_state_displaying_render(struct mealy_state *state, struct mealy_machine *machine) {}

static void mealy_state_interacting_event_click(struct mealy_state *state, struct mealy_machine *machine) {}
static void mealy_state_interacting_event_hover(struct mealy_state *state, struct mealy_machine *machine) {}
static void mealy_state_interacting_event_keypress(struct mealy_state *state, struct mealy_machine *machine, int key) {}
static void mealy_state_interacting_render(struct mealy_state *state, struct mealy_machine *machine) {}

static void mealy_state_processing_event_click(struct mealy_state *state, struct mealy_machine *machine) {}
static void mealy_state_processing_event_hover(struct mealy_state *state, struct mealy_machine *machine) {}
static void mealy_state_processing_event_keypress(struct mealy_state *state, struct mealy_machine *machine, int key) {}
static void mealy_state_processing_render(struct mealy_state *state, struct mealy_machine *machine) {}

static void mealy_state_error_event_click(struct mealy_state *state, struct mealy_machine *machine) {}
static void mealy_state_error_event_hover(struct mealy_state *state, struct mealy_machine *machine) {}
static void mealy_state_error_event_keypress(struct mealy_state *state, struct mealy_machine *machine, int key) {}
static void mealy_state_error_render(struct mealy_state *state, struct mealy_machine *machine) {}

/* Mealy state operations tables */
static struct mealy_state_ops mealy_state_init_ops = {
    ._destructor = NULL,
    .free = NULL,
    .event_click = mealy_state_init_event_click,
    .event_hover = mealy_state_init_event_hover,
    .event_keypress = mealy_state_init_event_keypress,
    .render = mealy_state_init_render,
    .__super = NULL
};

static struct mealy_state_ops mealy_state_idle_ops = {
    ._destructor = NULL,
    .free = NULL,
    .event_click = mealy_state_idle_event_click,
    .event_hover = mealy_state_idle_event_hover,
    .event_keypress = mealy_state_idle_event_keypress,
    .render = mealy_state_idle_render,
    .__super = NULL
};

static struct mealy_state_ops mealy_state_displaying_ops = {
    ._destructor = NULL,
    .free = NULL,
    .event_click = mealy_state_displaying_event_click,
    .event_hover = mealy_state_displaying_event_hover,
    .event_keypress = mealy_state_displaying_event_keypress,
    .render = mealy_state_displaying_render,
    .__super = NULL
};

static struct mealy_state_ops mealy_state_interacting_ops = {
    ._destructor = NULL,
    .free = NULL,
    .event_click = mealy_state_interacting_event_click,
    .event_hover = mealy_state_interacting_event_hover,
    .event_keypress = mealy_state_interacting_event_keypress,
    .render = mealy_state_interacting_render,
    .__super = NULL
};

static struct mealy_state_ops mealy_state_processing_ops = {
    ._destructor = NULL,
    .free = NULL,
    .event_click = mealy_state_processing_event_click,
    .event_hover = mealy_state_processing_event_hover,
    .event_keypress = mealy_state_processing_event_keypress,
    .render = mealy_state_processing_render,
    .__super = NULL
};

static struct mealy_state_ops mealy_state_error_ops = {
    ._destructor = NULL,
    .free = NULL,
    .event_click = mealy_state_error_event_click,
    .event_hover = mealy_state_error_event_hover,
    .event_keypress = mealy_state_error_event_keypress,
    .render = mealy_state_error_render,
    .__super = NULL
};

/**
 * Initialize a System Coordinator VIPER View
 */
struct scv_view *scv_view_init(const struct scv_ui_config *config,
                               struct scv_entity *entity,
                               struct scv_interactor *interactor,
                               struct scv_presenter *presenter)
{
    struct scv_view *view = (struct scv_view *)malloc(sizeof(struct scv_view));
    if (!view) {
        return NULL;
    }
    
    memset(view, 0, sizeof(struct scv_view));
    
    /* Set UI configuration */
    if (config) {
        view->config = *config;
    } else {
        view->config = DEFAULT_CONFIG;
    }
    
    /* Initialize Mealy FSM */
    view->mealy_fsm = (struct mealy_machine *)malloc(sizeof(struct mealy_machine));
    if (!view->mealy_fsm) {
        free(view);
        return NULL;
    }
    
    /* Create initial state (INIT) */
    struct mealy_state *initial_state = create_mealy_state(VIEW_STATE_INIT);
    if (!initial_state) {
        free(view->mealy_fsm);
        free(view);
        return NULL;
    }
    
    mealy_machine_init(view->mealy_fsm, initial_state);
    
    /* Set associated components */
    view->entity = entity;
    view->interactor = interactor;
    view->presenter = presenter;
    view->router = NULL;
    
    /* Initialize state */
    view->current_state = VIEW_STATE_INIT;
    view->active_screen_id = 0;
    view->is_interactive = false;
    
    /* Initialize event queue */
    init_event_queue(view, 32); /* capacity 32 events */
    
    /* Initialize response queue */
    init_response_queue(view, 32); /* capacity 32 responses */
    
    /* Initialize callbacks to NULL */
    view->ui_event_received_callback = NULL;
    view->ui_response_ready_callback = NULL;
    view->ui_state_changed_callback = NULL;
    view->user_data = NULL;
    
    return view;
}

/**
 * Destroy a System Coordinator VIPER View
 */
void scv_view_destroy(struct scv_view *view)
{
    if (!view) {
        return;
    }
    
    /* Free Mealy FSM */
    if (view->mealy_fsm) {
        struct mealy_state *state = mealy_machine_get_state(view->mealy_fsm);
        if (state) {
            destroy_mealy_state(state);
        }
        free(view->mealy_fsm);
    }
    
    /* Free queues */
    free_event_queue(view);
    free_response_queue(view);
    
    /* Free view */
    free(view);
}

/**
 * Process a UI event
 */
int scv_view_process_event(struct scv_view *view, const struct scv_ui_event *event)
{
    if (!view || !event) {
        return -1;
    }
    
    /* Map UI event to Mealy FSM event */
    switch (event->type) {
        case UI_EVT_BUTTON_PRESS:
        case UI_EVT_BUTTON_RELEASE:
        case UI_EVT_ENCODER_PRESS:
        case UI_EVT_SWITCH_TOGGLE:
            mealy_machine_event_click(view->mealy_fsm);
            break;
            
        case UI_EVT_TOUCH_START:
        case UI_EVT_TOUCH_MOVE:
        case UI_EVT_TOUCH_END:
        case UI_EVT_GESTURE:
            mealy_machine_event_hover(view->mealy_fsm);
            break;
            
        case UI_EVT_ENCODER_ROTATE:
        case UI_EVT_VOICE_COMMAND:
        case UI_EVT_TIMEOUT:
        case UI_EVT_SYSTEM_NOTIFICATION:
            /* Map to keypress with a key code */
            mealy_machine_event_keypress(view->mealy_fsm, 0);
            break;
            
        default:
            /* Unknown event type */
            return -2;
    }
    
    /* Update view state based on Mealy FSM state */
    struct mealy_state *current_state = mealy_machine_get_state(view->mealy_fsm);
    /* TODO: Map mealy state to view state */
    
    /* Notify event received */
    if (view->ui_event_received_callback) {
        view->ui_event_received_callback(event, view->user_data);
    }
    
    return 0;
}

/**
 * Queue a UI event for processing
 */
int scv_view_queue_event(struct scv_view *view, const struct scv_ui_event *event)
{
    if (!view || !event) {
        return -1;
    }
    
    return enqueue_event(view, event);
}

/**
 * Process all queued UI events
 */
uint32_t scv_view_process_queued_events(struct scv_view *view)
{
    if (!view) {
        return 0;
    }
    
    uint32_t processed = 0;
    struct scv_ui_event event;
    
    while (dequeue_event(view, &event) == 0) {
        if (scv_view_process_event(view, &event) == 0) {
            processed++;
        }
    }
    
    return processed;
}

/**
 * Get next UI response
 */
int scv_view_get_response(struct scv_view *view, struct scv_ui_response *response)
{
    if (!view || !response) {
        return -1;
    }
    
    return dequeue_response(view, response);
}

/**
 * Queue a UI response
 */
int scv_view_queue_response(struct scv_view *view, const struct scv_ui_response *response)
{
    if (!view || !response) {
        return -1;
    }
    
    return enqueue_response(view, response);
}

/**
 * Update UI configuration
 */
int scv_view_update_config(struct scv_view *view, const struct scv_ui_config *config)
{
    if (!view || !config) {
        return -1;
    }
    
    view->config = *config;
    return 0;
}

/**
 * Get UI configuration
 */
const struct scv_ui_config *scv_view_get_config(const struct scv_view *view)
{
    if (!view) {
        return NULL;
    }
    return &view->config;
}

/**
 * Set associated entity
 */
void scv_view_set_entity(struct scv_view *view, struct scv_entity *entity)
{
    if (!view) {
        return;
    }
    view->entity = entity;
}

/**
 * Set associated interactor
 */
void scv_view_set_interactor(struct scv_view *view, struct scv_interactor *interactor)
{
    if (!view) {
        return;
    }
    view->interactor = interactor;
}

/**
 * Set associated presenter
 */
void scv_view_set_presenter(struct scv_view *view, struct scv_presenter *presenter)
{
    if (!view) {
        return;
    }
    view->presenter = presenter;
}

/**
 * Set associated router
 */
void scv_view_set_router(struct scv_view *view, struct scv_router *router)
{
    if (!view) {
        return;
    }
    view->router = router;
}

/**
 * Get view state
 */
scv_view_state_t scv_view_get_state(const struct scv_view *view)
{
    if (!view) {
        return VIEW_STATE_INIT;
    }
    return view->current_state;
}

/**
 * Set active screen
 */
int scv_view_set_active_screen(struct scv_view *view, uint32_t screen_id)
{
    if (!view) {
        return -1;
    }
    
    view->active_screen_id = screen_id;
    
    /* Generate a response to update display */
    struct scv_ui_response response;
    memset(&response, 0, sizeof(response));
    response.type = UI_RESPONSE_UPDATE_DISPLAY;
    snprintf(response.data.display.display_text, sizeof(response.data.display.display_text),
             "Screen %u", screen_id);
    response.data.display.display_page = screen_id;
    response.data.display.refresh_full = true;
    response.priority = 50;
    
    enqueue_response(view, &response);
    
    return 0;
}

/**
 * Get active screen
 */
uint32_t scv_view_get_active_screen(const struct scv_view *view)
{
    if (!view) {
        return 0;
    }
    return view->active_screen_id;
}

/**
 * Set UI event received callback
 */
void scv_view_set_ui_event_received_callback(struct scv_view *view,
                                             void (*callback)(const struct scv_ui_event *event, void *user_data),
                                             void *user_data)
{
    if (!view) {
        return;
    }
    view->ui_event_received_callback = callback;
    view->user_data = user_data;
}

/**
 * Set UI response ready callback
 */
void scv_view_set_ui_response_ready_callback(struct scv_view *view,
                                             void (*callback)(const struct scv_ui_response *response, void *user_data),
                                             void *user_data)
{
    if (!view) {
        return;
    }
    view->ui_response_ready_callback = callback;
    view->user_data = user_data;
}

/**
 * Set UI state changed callback
 */
void scv_view_set_ui_state_changed_callback(struct scv_view *view,
                                            void (*callback)(scv_view_state_t new_state,
                                                             scv_view_state_t old_state,
                                                             void *user_data),
                                            void *user_data)
{
    if (!view) {
        return;
    }
    view->ui_state_changed_callback = callback;
    view->user_data = user_data;
}

/**
 * Reset view to initial state
 */
int scv_view_reset(struct scv_view *view)
{
    if (!view) {
        return -1;
    }
    
    /* Reset Mealy FSM to INIT state */
    struct mealy_state *init_state = create_mealy_state(VIEW_STATE_INIT);
    if (!init_state) {
        return -2;
    }
    
    mealy_machine_set_state(view->mealy_fsm, init_state);
    
    /* Update view state */
    update_view_state(view, VIEW_STATE_INIT);
    
    /* Clear queues */
    free_event_queue(view);
    free_response_queue(view);
    init_event_queue(view, 32);
    init_response_queue(view, 32);
    
    /* Reset active screen */
    view->active_screen_id = 0;
    view->is_interactive = false;
    
    return 0;
}

/* Internal helper functions */

static void init_event_queue(struct scv_view *view, uint32_t capacity)
{
    view->event_queue = (struct scv_ui_event *)malloc(capacity * sizeof(struct scv_ui_event));
    view->event_queue_size = capacity;
    view->event_queue_head = 0;
    view->event_queue_tail = 0;
}

static void free_event_queue(struct scv_view *view)
{
    if (view->event_queue) {
        free(view->event_queue);
        view->event_queue = NULL;
    }
    view->event_queue_size = 0;
    view->event_queue_head = 0;
    view->event_queue_tail = 0;
}

static void init_response_queue(struct scv_view *view, uint32_t capacity)
{
    view->response_queue = (struct scv_ui_response *)malloc(capacity * sizeof(struct scv_ui_response));
    view->response_queue_size = capacity;
    view->response_queue_head = 0;
    view->response_queue_tail = 0;
}

static void free_response_queue(struct scv_view *view)
{
    if (view->response_queue) {
        free(view->response_queue);
        view->response_queue = NULL;
    }
    view->response_queue_size = 0;
    view->response_queue_head = 0;
    view->response_queue_tail = 0;
}

static int enqueue_event(struct scv_view *view, const struct scv_ui_event *event)
{
    if (!view->event_queue) {
        return -1;
    }
    uint32_t next_tail = (view->event_queue_tail + 1) % view->event_queue_size;
    if (next_tail == view->event_queue_head) {
        /* Queue full */
        return -2;
    }
    memcpy(&view->event_queue[view->event_queue_tail], event, sizeof(struct scv_ui_event));
    view->event_queue_tail = next_tail;
    return 0;
}

static int dequeue_event(struct scv_view *view, struct scv_ui_event *event)
{
    if (!view->event_queue) {
        return -1;
    }
    if (view->event_queue_head == view->event_queue_tail) {
        /* Queue empty */
        return -2;
    }
    memcpy(event, &view->event_queue[view->event_queue_head], sizeof(struct scv_ui_event));
    view->event_queue_head = (view->event_queue_head + 1) % view->event_queue_size;
    return 0;
}

static int enqueue_response(struct scv_view *view, const struct scv_ui_response *response)
{
    if (!view->response_queue) {
        return -1;
    }
    uint32_t next_tail = (view->response_queue_tail + 1) % view->response_queue_size;
    if (next_tail == view->response_queue_head) {
        /* Queue full */
        return -2;
    }
    memcpy(&view->response_queue[view->response_queue_tail], response, sizeof(struct scv_ui_response));
    view->response_queue_tail = next_tail;
    return 0;
}

static int dequeue_response(struct scv_view *view, struct scv_ui_response *response)
{
    if (!view->response_queue) {
        return -1;
    }
    if (view->response_queue_head == view->response_queue_tail) {
        /* Queue empty */
        return -2;
    }
    memcpy(response, &view->response_queue[view->response_queue_head], sizeof(struct scv_ui_response));
    view->response_queue_head = (view->response_queue_head + 1) % view->response_queue_size;
    return 0;
}

static void update_view_state(struct scv_view *view, scv_view_state_t new_state)
{
    scv_view_state_t old_state = view->current_state;
    if (old_state == new_state) {
        return;
    }
    view->current_state = new_state;
    if (view->ui_state_changed_callback) {
        view->ui_state_changed_callback(new_state, old_state, view->user_data);
    }
}

static struct mealy_state *create_mealy_state(scv_view_state_t state)
{
    struct mealy_state *mealy_state = (struct mealy_state *)malloc(sizeof(struct mealy_state));
    if (!mealy_state) {
        return NULL;
    }
    memset(mealy_state, 0, sizeof(struct mealy_state));
    
    switch (state) {
        case VIEW_STATE_INIT:
            mealy_state->ops = &mealy_state_init_ops;
            break;
        case VIEW_STATE_IDLE:
            mealy_state->ops = &mealy_state_idle_ops;
            break;
        case VIEW_STATE_DISPLAYING:
            mealy_state->ops = &mealy_state_displaying_ops;
            break;
        case VIEW_STATE_INTERACTING:
            mealy_state->ops = &mealy_state_interacting_ops;
            break;
        case VIEW_STATE_PROCESSING:
            mealy_state->ops = &mealy_state_processing_ops;
            break;
        case VIEW_STATE_ERROR:
            mealy_state->ops = &mealy_state_error_ops;
            break;
        default:
            free(mealy_state);
            return NULL;
    }
    
    return mealy_state;
}

static void destroy_mealy_state(struct mealy_state *state)
{
    if (state) {
        free(state);
    }
}

/**
 * Start the view component
 */
int scv_view_start(struct scv_view *view)
{
    if (!view) {
        return -1;
    }
    
    /* Transition from INIT to IDLE state */
    struct mealy_state *idle_state = create_mealy_state(VIEW_STATE_IDLE);
    if (!idle_state) {
        return -2;
    }
    
    mealy_machine_set_state(view->mealy_fsm, idle_state);
    update_view_state(view, VIEW_STATE_IDLE);
    
    /* Mark as interactive */
    view->is_interactive = true;
    
    return 0;
}

/**
 * Stop the view component
 */
int scv_view_stop(struct scv_view *view)
{
    if (!view) {
        return -1;
    }
    
    /* Transition to INIT state */
    struct mealy_state *init_state = create_mealy_state(VIEW_STATE_INIT);
    if (!init_state) {
        return -2;
    }
    
    mealy_machine_set_state(view->mealy_fsm, init_state);
    update_view_state(view, VIEW_STATE_INIT);
    
    /* Mark as non-interactive */
    view->is_interactive = false;
    
    /* Clear queues */
    free_event_queue(view);
    free_response_queue(view);
    init_event_queue(view, 32);
    init_response_queue(view, 32);
    
    return 0;
}

/**
 * Process generic input (for compatibility with scv_process_event)
 */
int scv_view_process_input(struct scv_view *view, const void *input_data, uint32_t input_size)
{
    if (!view || !input_data || input_size == 0) {
        return -1;
    }
    
    /* For simplicity, treat input as a UI event of type SYSTEM_NOTIFICATION */
    struct scv_ui_event event;
    memset(&event, 0, sizeof(event));
    event.type = UI_EVT_SYSTEM_NOTIFICATION;
    event.data.notification.notification_id = 0;
    event.data.notification.notification_data = NULL;
    event.timestamp_ms = 0; /* TODO: get actual timestamp */
    
    return scv_view_process_event(view, &event);
}

/**
 * Get view status string
 */
int scv_view_get_status(const struct scv_view *view, char *status_buffer, uint32_t buffer_size)
{
    if (!view || !status_buffer || buffer_size == 0) {
        return -1;
    }
    
    const char *state_str = "UNKNOWN";
    switch (view->current_state) {
        case VIEW_STATE_INIT: state_str = "INIT"; break;
        case VIEW_STATE_IDLE: state_str = "IDLE"; break;
        case VIEW_STATE_DISPLAYING: state_str = "DISPLAYING"; break;
        case VIEW_STATE_INTERACTING: state_str = "INTERACTING"; break;
        case VIEW_STATE_PROCESSING: state_str = "PROCESSING"; break;
        case VIEW_STATE_ERROR: state_str = "ERROR"; break;
    }
    
    int written = snprintf(status_buffer, buffer_size,
                           "View state: %s, screen: %u, interactive: %s",
                           state_str,
                           view->active_screen_id,
                           view->is_interactive ? "yes" : "no");
    
    if (written < 0 || (uint32_t)written >= buffer_size) {
        return -2; /* buffer too small */
    }
    
    return 0;
}
