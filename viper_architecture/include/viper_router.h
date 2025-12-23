/**
 * viper_router.h - VIPER Router Component Interface
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * VIPER Router component implemented as an Acceptor/Regex State Machine.
 * Handles navigation and routing patterns.
 */

#ifndef __VIPER_ROUTER_H__
#define __VIPER_ROUTER_H__

#include "viper.h"
#include "../../state_machine_extended/src/acceptor_regex.h"

/**
 * Router States
 */
typedef enum {
    ROUTER_STATE_IDLE,
    ROUTER_STATE_NAVIGATING,
    ROUTER_STATE_ROUTING,
    ROUTER_STATE_TRANSITIONING,
    ROUTER_STATE_ERROR,
} viper_router_state_t;

/**
 * Router Events
 */
typedef enum {
    ROUTER_EVENT_NAVIGATE,
    ROUTER_EVENT_BACK,
    ROUTER_EVENT_FORWARD,
    ROUTER_EVENT_TRANSITION,
    ROUTER_EVENT_ROUTE_CHANGED,
    ROUTER_EVENT_ERROR,
} viper_router_event_t;

/**
 * Route Definition
 */
struct viper_route {
    char pattern[128];          /* Route pattern (regex) */
    char destination[128];      /* Destination module/component */
    int destination_id;         /* Destination ID */
    void *context;              /* Route-specific context */
    size_t context_size;        /* Size of context */
};

/**
 * Navigation Context
 */
struct viper_navigation_context {
    /* Current route */
    struct viper_route current_route;
    
    /* Navigation history */
    struct viper_route history[32];
    int history_size;
    int history_index;
    
    /* Transition data */
    void *transition_data;
    size_t transition_data_size;
    
    /* Navigation parameters */
    char parameters[256];
};

/**
 * VIPER Router Component
 */
struct viper_router {
    /* Acceptor/Regex FSM for pattern matching */
    struct acceptor_machine *fsm;
    
    /* Navigation context */
    struct viper_navigation_context context;
    
    /* Configuration */
    char router_name[64];
    int router_id;
    
    /* Route table */
    struct viper_route *routes;
    int route_count;
    int route_capacity;
    
    /* Navigation callbacks */
    void (*navigation_callback)(const struct viper_route *route, void *user_data);
    void (*transition_callback)(const struct viper_route *from, 
                                const struct viper_route *to, 
                                void *transition_data);
};

/* Function prototypes */

/**
 * Initialize a VIPER Router component
 * 
 * @param router_name Name of the router
 * @param router_id Unique ID for the router
 * @return New router instance, NULL on error
 */
struct viper_router *viper_router_init(const char *router_name,
                                       int router_id);

/**
 * Destroy a VIPER Router component
 * 
 * @param router Router to destroy
 */
void viper_router_destroy(struct viper_router *router);

/**
 * Add a route to the router
 * 
 * @param router Router instance
 * @param pattern Route pattern (regex)
 * @param destination Destination identifier
 * @param destination_id Destination ID
 * @param context Route context (optional)
 * @param context_size Size of context
 * @return 0 on success, negative on error
 */
int viper_router_add_route(struct viper_router *router,
                           const char *pattern,
                           const char *destination,
                           int destination_id,
                           const void *context,
                           size_t context_size);

/**
 * Navigate to a route
 * 
 * @param router Router instance
 * @param path Navigation path
 * @param navigation_data Navigation data
 * @param data_size Size of navigation data
 * @return 0 on success, negative on error
 */
int viper_router_navigate(struct viper_router *router,
                          const char *path,
                          const void *navigation_data,
                          size_t data_size);

/**
 * Navigate back in history
 * 
 * @param router Router instance
 * @return 0 on success, negative on error
 */
int viper_router_navigate_back(struct viper_router *router);

/**
 * Navigate forward in history
 * 
 * @param router Router instance
 * @return 0 on success, negative on error
 */
int viper_router_navigate_forward(struct viper_router *router);

/**
 * Get current route
 * 
 * @param router Router instance
 * @return Current route, NULL if no route
 */
const struct viper_route *viper_router_get_current_route(const struct viper_router *router);

/**
 * Get router state
 * 
 * @param router Router instance
 * @return Current router state
 */
viper_router_state_t viper_router_get_state(const struct viper_router *router);

/**
 * Get navigation context for debugging
 * 
 * @param router Router instance
 * @return Navigation context structure
 */
const struct viper_navigation_context *viper_router_get_context(const struct viper_router *router);

/**
 * Process a router event
 * 
 * @param router Router instance
 * @param event Event to process
 * @param data Event data
 * @return 0 on success, negative on error
 */
int viper_router_process_event(struct viper_router *router,
                               viper_router_event_t event,
                               const void *data);

#endif /* __VIPER_ROUTER_H__ */
