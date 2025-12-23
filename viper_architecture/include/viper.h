/**
 * viper.h - VIPER Architecture Main Interface
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Auto generate for Design-Patterns in C *
 * 
 * VIPER Architecture implementation using 5 types of state machines.
 */

#ifndef __VIPER_H__
#define __VIPER_H__

#include "../../state_machine_extended/src/mycommon.h"
#include "../../state_machine_extended/src/mytrace.h"

/* Forward declarations for state machines */
struct mealy_machine;
struct parallel_machine;
struct moore_hsm;
struct efsm_processor;
struct acceptor_machine;

/* Component structures (defined in component-specific headers) */
struct viper_view;
struct viper_presenter;
struct viper_interactor;
struct viper_entity;
struct viper_router;

/**
 * VIPER Module - Complete VIPER architecture module
 */
struct viper_module {
    struct viper_view *view;
    struct viper_presenter *presenter;
    struct viper_interactor *interactor;
    struct viper_entity *entity;
    struct viper_router *router;
    
    /* Module configuration */
    char module_name[64];
    int module_id;
};

/**
 * VIPER Event Types - Events that flow between VIPER components
 */
typedef enum {
    VIPER_EVENT_VIEW_ACTION,      /* User action from view */
    VIPER_EVENT_DATA_UPDATE,      /* Data update from entity */
    VIPER_EVENT_NAVIGATION,       /* Navigation request */
    VIPER_EVENT_BUSINESS_LOGIC,   /* Business logic request */
    VIPER_EVENT_PRESENTATION,     /* Presentation update */
    VIPER_EVENT_ERROR,            /* Error occurred */
    VIPER_EVENT_SUCCESS,          /* Operation successful */
} viper_event_type_t;

/**
 * VIPER Event - Generic event structure
 */
struct viper_event {
    viper_event_type_t type;
    void *data;
    size_t data_size;
    int source_component;  /* Component that generated the event */
    int target_component;  /* Component that should receive the event */
};

/**
 * VIPER Configuration
 */
struct viper_config {
    /* Component enable flags */
    unsigned int enable_view : 1;
    unsigned int enable_presenter : 1;
    unsigned int enable_interactor : 1;
    unsigned int enable_entity : 1;
    unsigned int enable_router : 1;
    
    /* Module settings */
    char module_name[64];
    int max_events;        /* Maximum events in queue */
};

/* Function prototypes */

/**
 * Initialize a VIPER module with given configuration
 * 
 * @param config Configuration for the VIPER module
 * @return Pointer to initialized VIPER module, NULL on error
 */
struct viper_module *viper_module_init(const struct viper_config *config);

/**
 * Destroy a VIPER module and free all resources
 * 
 * @param module VIPER module to destroy
 */
void viper_module_destroy(struct viper_module *module);

/**
 * Process an event through the VIPER architecture
 * 
 * @param module VIPER module
 * @param event Event to process
 * @return 0 on success, negative on error
 */
int viper_module_process_event(struct viper_module *module, 
                               const struct viper_event *event);

/**
 * Get current status of VIPER module
 * 
 * @param module VIPER module
 * @return Status string
 */
const char *viper_module_get_status(const struct viper_module *module);

/**
 * Create a VIPER event
 * 
 * @param type Event type
 * @param data Event data (will be copied)
 * @param data_size Size of data
 * @param source Source component ID
 * @param target Target component ID
 * @return New event, NULL on error
 */
struct viper_event *viper_event_create(viper_event_type_t type,
                                       const void *data,
                                       size_t data_size,
                                       int source,
                                       int target);

/**
 * Destroy a VIPER event
 * 
 * @param event Event to destroy
 */
void viper_event_destroy(struct viper_event *event);

/* Component IDs for source/target */
#define VIPER_COMPONENT_VIEW       1
#define VIPER_COMPONENT_PRESENTER  2
#define VIPER_COMPONENT_INTERACTOR 3
#define VIPER_COMPONENT_ENTITY     4
#define VIPER_COMPONENT_ROUTER     5

#endif /* __VIPER_H__ */
