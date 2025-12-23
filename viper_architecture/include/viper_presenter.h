/**
 * viper_presenter.h - VIPER Presenter Component Interface
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * VIPER Presenter component implemented as a Parallel FSM.
 * Coordinates multiple components and manages data flow.
 */

#ifndef __VIPER_PRESENTER_H__
#define __VIPER_PRESENTER_H__

#include "viper.h"
#include "../../state_machine_extended/src/parallel_fsm.h"

/**
 * Presenter States
 */
typedef enum {
    PRESENTER_STATE_IDLE,
    PRESENTER_STATE_PROCESSING,
    PRESENTER_STATE_UPDATING_VIEW,
    PRESENTER_STATE_FETCHING_DATA,
    PRESENTER_STATE_ERROR,
} viper_presenter_state_t;

/**
 * Presenter Events
 */
typedef enum {
    PRESENTER_EVENT_VIEW_UPDATE,
    PRESENTER_EVENT_DATA_READY,
    PRESENTER_EVENT_ERROR,
    PRESENTER_EVENT_NAVIGATION_REQUEST,
    PRESENTER_EVENT_BUSINESS_LOGIC_COMPLETE,
} viper_presenter_event_t;

/**
 * Presenter Data Structure
 */
struct viper_presenter_data {
    /* Current state */
    viper_presenter_state_t current_state;
    
    /* Data to present */
    void *presentation_data;
    size_t data_size;
    
    /* View reference */
    void *view_context;
    
    /* Interactor reference */
    void *interactor_context;
    
    /* Error information */
    int error_code;
    char error_message[128];
};

/**
 * VIPER Presenter Component
 */
struct viper_presenter {
    /* Parallel FSM for coordinating multiple flows */
    struct parallel_machine *fsm;
    
    /* Presenter data */
    struct viper_presenter_data data;
    
    /* Configuration */
    char presenter_name[64];
    int presenter_id;
    
    /* Component references */
    struct viper_view *view;
    struct viper_interactor *interactor;
    
    /* Callbacks */
    void (*view_update_callback)(void *view_context, const void *data, size_t size);
    void (*interactor_request_callback)(void *interactor_context, const void *request);
};

/* Function prototypes */

/**
 * Initialize a VIPER Presenter component
 * 
 * @param presenter_name Name of the presenter
 * @param presenter_id Unique ID for the presenter
 * @param view Associated view component
 * @param interactor Associated interactor component
 * @return New presenter instance, NULL on error
 */
struct viper_presenter *viper_presenter_init(const char *presenter_name,
                                             int presenter_id,
                                             struct viper_view *view,
                                             struct viper_interactor *interactor);

/**
 * Destroy a VIPER Presenter component
 * 
 * @param presenter Presenter to destroy
 */
void viper_presenter_destroy(struct viper_presenter *presenter);

/**
 * Process an event in the presenter
 * 
 * @param presenter Presenter instance
 * @param event Event to process
 * @param event_data Event data
 * @return 0 on success, negative on error
 */
int viper_presenter_process_event(struct viper_presenter *presenter,
                                  viper_presenter_event_t event,
                                  const void *event_data);

/**
 * Update view with data
 * 
 * @param presenter Presenter instance
 * @param data Data to present
 * @param data_size Size of data
 * @return 0 on success, negative on error
 */
int viper_presenter_update_view(struct viper_presenter *presenter,
                                const void *data,
                                size_t data_size);

/**
 * Request data from interactor
 * 
 * @param presenter Presenter instance
 * @param request Request data
 * @param request_size Size of request
 * @return 0 on success, negative on error
 */
int viper_presenter_request_data(struct viper_presenter *presenter,
                                 const void *request,
                                 size_t request_size);

/**
 * Get current presenter state
 * 
 * @param presenter Presenter instance
 * @return Current presenter state
 */
viper_presenter_state_t viper_presenter_get_state(const struct viper_presenter *presenter);

/**
 * Get presenter data for debugging
 * 
 * @param presenter Presenter instance
 * @return Presenter data structure
 */
const struct viper_presenter_data *viper_presenter_get_data(const struct viper_presenter *presenter);

#endif /* __VIPER_PRESENTER_H__ */
