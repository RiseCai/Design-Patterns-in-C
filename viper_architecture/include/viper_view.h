/**
 * viper_view.h - VIPER View Component Interface
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * VIPER View component implemented as a Mealy Event-Driven State Machine.
 * Handles user interface events and updates.
 */

#ifndef __VIPER_VIEW_H__
#define __VIPER_VIEW_H__

#include "viper.h"
#include "../../state_machine_extended/src/mealy_machine.h"

/**
 * View States
 */
typedef enum {
    VIEW_STATE_IDLE,
    VIEW_STATE_LOADING,
    VIEW_STATE_DISPLAYING,
    VIEW_STATE_ERROR,
    VIEW_STATE_UPDATING,
} viper_view_state_t;

/**
 * View Events (User Actions)
 */
typedef enum {
    VIEW_EVENT_USER_INPUT,
    VIEW_EVENT_REFRESH,
    VIEW_EVENT_ERROR_OCCURRED,
    VIEW_EVENT_DATA_RECEIVED,
    VIEW_EVENT_NAVIGATE,
} viper_view_event_t;

/**
 * View Data Structure
 */
struct viper_view_data {
    /* UI state */
    viper_view_state_t current_state;
    
    /* Display data */
    void *display_buffer;
    size_t buffer_size;
    
    /* User input */
    char last_input[256];
    
    /* Error information */
    int error_code;
    char error_message[128];
};

/**
 * VIPER View Component
 */
struct viper_view {
    /* Base state machine */
    struct mealy_machine *fsm;
    
    /* View data */
    struct viper_view_data data;
    
    /* Configuration */
    char view_name[64];
    int view_id;
    
    /* Callback for presenting data to user */
    void (*present_callback)(const void *data, size_t size);
};

/* Function prototypes */

/**
 * Initialize a VIPER View component
 * 
 * @param view_name Name of the view
 * @param view_id Unique ID for the view
 * @param present_callback Callback for presenting data
 * @return New view instance, NULL on error
 */
struct viper_view *viper_view_init(const char *view_name,
                                   int view_id,
                                   void (*present_callback)(const void *data, size_t size));

/**
 * Destroy a VIPER View component
 * 
 * @param view View to destroy
 */
void viper_view_destroy(struct viper_view *view);

/**
 * Process a user event in the view
 * 
 * @param view View instance
 * @param event Event to process
 * @param event_data Event data
 * @return 0 on success, negative on error
 */
int viper_view_process_event(struct viper_view *view,
                             viper_view_event_t event,
                             const void *event_data);

/**
 * Update the view with new data from presenter
 * 
 * @param view View instance
 * @param data Data to display
 * @param data_size Size of data
 * @return 0 on success, negative on error
 */
int viper_view_update(struct viper_view *view,
                      const void *data,
                      size_t data_size);

/**
 * Get current view state
 * 
 * @param view View instance
 * @return Current view state
 */
viper_view_state_t viper_view_get_state(const struct viper_view *view);

/**
 * Get view data for debugging
 * 
 * @param view View instance
 * @return View data structure
 */
const struct viper_view_data *viper_view_get_data(const struct viper_view *view);

#endif /* __VIPER_VIEW_H__ */
