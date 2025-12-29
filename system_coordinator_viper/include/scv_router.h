/**
 * scv_router.h - System Coordinator VIPER Router Component
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * Router component implemented as an Acceptor FSM.
 * Handles navigation, subsystem routing, and pattern matching.
 */

#ifndef __SCV_ROUTER_H__
#define __SCV_ROUTER_H__

#include <stdint.h>
#include <stdbool.h>
#include "../../state_machine_extended/src/acceptor_regex.h"
#include "scv_entity.h"
#include "scv_interactor.h"
#include "scv_presenter.h"
#include "scv_view.h"

/**
 * Router States
 */
typedef enum {
    ROUTER_STATE_INIT,
    ROUTER_STATE_IDLE,
    ROUTER_STATE_ROUTING,
    ROUTER_STATE_PATTERN_MATCHING,
    ROUTER_STATE_ERROR,
} scv_router_state_t;

/**
 * Route Types
 */
typedef enum {
    ROUTE_TYPE_INTERNAL,      /* Internal component routing */
    ROUTE_TYPE_SUBSYSTEM,     /* Subsystem routing */
    ROUTE_TYPE_NAVIGATION,    /* UI navigation */
    ROUTE_TYPE_EVENT,         /* Event routing */
    ROUTE_TYPE_DATA,          /* Data routing */
    ROUTE_TYPE_COMMAND,       /* Command routing */
} scv_route_type_t;

/**
 * Route Pattern
 */
struct scv_route_pattern {
    char pattern[128];        /* Regex pattern for matching */
    scv_route_type_t type;    /* Type of route */
    uint32_t priority;        /* Priority (higher = matched first) */
    void *target;             /* Target component or function */
};

/**
 * Route Match Result
 */
struct scv_route_match {
    const struct scv_route_pattern *pattern;
    char matched_string[256];
    uint32_t match_start;
    uint32_t match_end;
    void *captured_data;
};

/**
 * Routing Decision
 */
typedef enum {
    ROUTING_DECISION_CONTINUE,    /* Continue routing */
    ROUTING_DECISION_STOP,        /* Stop routing (handled) */
    ROUTING_DECISION_REDIRECT,    /* Redirect to different route */
    ROUTING_DECISION_DROP,        /* Drop the route */
    ROUTING_DECISION_QUEUE,       /* Queue for later processing */
} scv_routing_decision_t;

/**
 * Routing Context
 */
struct scv_routing_context {
    /* Source of the route */
    char source_component[64];
    void *source_handle;
    
    /* Destination of the route */
    char destination_component[64];
    void *destination_handle;
    
    /* Route data */
    scv_route_type_t route_type;
    void *route_data;
    uint32_t route_data_size;
    
    /* Routing flags */
    bool requires_acknowledgment;
    bool is_high_priority;
    bool is_broadcast;
    
    /* Timing information */
    uint64_t route_start_time;
    uint64_t route_timeout_ms;
    
    /* Result */
    scv_routing_decision_t decision;
    struct scv_route_match match_result;
};

/**
 * Routing Table Entry
 */
struct scv_routing_table_entry {
    struct scv_route_pattern pattern;
    uint32_t usage_count;
    uint64_t last_used_time;
    bool is_active;
};

/**
 * Routing Configuration
 */
struct scv_routing_config {
    /* Routing table size */
    uint32_t max_routing_table_entries;
    uint32_t max_pattern_length;
    
    /* Routing policies */
    bool enable_pattern_caching;
    bool enable_route_optimization;
    uint32_t max_route_hops;
    uint32_t route_timeout_ms;
    
    /* Pattern matching settings */
    uint32_t max_pattern_matches;
    bool case_sensitive_matching;
    
    /* Performance settings */
    uint32_t routing_thread_priority;
    uint32_t max_concurrent_routes;
};

/**
 * System Coordinator VIPER Router
 */
struct scv_router {
    /* Acceptor FSM for pattern matching and routing */
    acceptor_regex *acceptor_fsm;
    
    /* Routing configuration */
    struct scv_routing_config config;
    
    /* Routing table */
    struct scv_routing_table_entry *routing_table;
    uint32_t routing_table_size;
    uint32_t routing_table_count;
    
    /* Associated components */
    struct scv_entity *entity;
    struct scv_interactor *interactor;
    struct scv_presenter *presenter;
    struct scv_view *view;
    
    /* Router state */
    scv_router_state_t current_state;
    uint32_t active_routes;
    uint32_t total_routes_processed;
    
    /* Callbacks */
    void (*route_matched_callback)(const struct scv_route_match *match, void *user_data);
    void (*routing_decision_callback)(const struct scv_routing_context *context, void *user_data);
    void (*route_completed_callback)(const struct scv_routing_context *context, 
                                     scv_routing_decision_t decision,
                                     void *user_data);
    
    /* User data for callbacks */
    void *user_data;
};

/* Function prototypes */

/**
 * Initialize a System Coordinator VIPER Router
 * 
 * @param config Routing configuration (can be NULL for defaults)
 * @param entity Associated entity (can be NULL, set later)
 * @param interactor Associated interactor (can be NULL, set later)
 * @param presenter Associated presenter (can be NULL, set later)
 * @param view Associated view (can be NULL, set later)
 * @return New router instance, NULL on error
 */
struct scv_router *scv_router_init(const struct scv_routing_config *config,
                                   struct scv_entity *entity,
                                   struct scv_interactor *interactor,
                                   struct scv_presenter *presenter,
                                   struct scv_view *view);

/**
 * Destroy a System Coordinator VIPER Router
 * 
 * @param router Router to destroy
 */
void scv_router_destroy(struct scv_router *router);

/**
 * Add a route pattern to the routing table
 * 
 * @param router Router instance
 * @param pattern Route pattern to add
 * @return 0 on success, negative on error
 */
int scv_router_add_pattern(struct scv_router *router, const struct scv_route_pattern *pattern);

/**
 * Remove a route pattern from the routing table
 * 
 * @param router Router instance
 * @param pattern Pattern to remove (must match exactly)
 * @return 0 on success, negative on error
 */
int scv_router_remove_pattern(struct scv_router *router, const struct scv_route_pattern *pattern);

/**
 * Route a message based on patterns
 * 
 * @param router Router instance
 * @param message Message to route
 * @param message_length Length of message
 * @param context Routing context (can be NULL)
 * @return Routing decision
 */
scv_routing_decision_t scv_router_route_message(struct scv_router *router,
                                                const char *message,
                                                uint32_t message_length,
                                                struct scv_routing_context *context);

/**
 * Match a message against routing patterns
 * 
 * @param router Router instance
 * @param message Message to match
 * @param message_length Length of message
 * @param matches Array to store matches
 * @param max_matches Maximum number of matches to return
 * @return Number of matches found
 */
uint32_t scv_router_match_patterns(struct scv_router *router,
                                   const char *message,
                                   uint32_t message_length,
                                   struct scv_route_match *matches,
                                   uint32_t max_matches);

/**
 * Update routing configuration
 * 
 * @param router Router instance
 * @param config New routing configuration
 * @return 0 on success, negative on error
 */
int scv_router_update_config(struct scv_router *router, const struct scv_routing_config *config);

/**
 * Get routing configuration
 * 
 * @param router Router instance
 * @return Current routing configuration
 */
const struct scv_routing_config *scv_router_get_config(const struct scv_router *router);

/**
 * Set associated entity
 * 
 * @param router Router instance
 * @param entity Entity to associate
 */
void scv_router_set_entity(struct scv_router *router, struct scv_entity *entity);

/**
 * Set associated interactor
 * 
 * @param router Router instance
 * @param interactor Interactor to associate
 */
void scv_router_set_interactor(struct scv_router *router, struct scv_interactor *interactor);

/**
 * Set associated presenter
 * 
 * @param router Router instance
 * @param presenter Presenter to associate
 */
void scv_router_set_presenter(struct scv_router *router, struct scv_presenter *presenter);

/**
 * Set associated view
 * 
 * @param router Router instance
 * @param view View to associate
 */
void scv_router_set_view(struct scv_router *router, struct scv_view *view);

/**
 * Get router state
 * 
 * @param router Router instance
 * @return Current router state
 */
scv_router_state_t scv_router_get_state(const struct scv_router *router);

/**
 * Get routing table statistics
 * 
 * @param router Router instance
 * @param total_entries Output parameter for total entries
 * @param active_entries Output parameter for active entries
 * @return 0 on success, negative on error
 */
int scv_router_get_statistics(const struct scv_router *router,
                              uint32_t *total_entries,
                              uint32_t *active_entries);

/**
 * Clear routing table
 * 
 * @param router Router instance
 * @return 0 on success, negative on error
 */
int scv_router_clear_table(struct scv_router *router);

/**
 * Set route matched callback
 * 
 * @param router Router instance
 * @param callback Callback function
 * @param user_data User data passed to callback
 */
void scv_router_set_route_matched_callback(struct scv_router *router,
                                           void (*callback)(const struct scv_route_match *match, void *user_data),
                                           void *user_data);

/**
 * Set routing decision callback
 * 
 * @param router Router instance
 * @param callback Callback function
 * @param user_data User data passed to callback
 */
void scv_router_set_routing_decision_callback(struct scv_router *router,
                                              void (*callback)(const struct scv_routing_context *context, void *user_data),
                                              void *user_data);

/**
 * Set route completed callback
 * 
 * @param router Router instance
 * @param callback Callback function
 * @param user_data User data passed to callback
 */
void scv_router_set_route_completed_callback(struct scv_router *router,
                                             void (*callback)(const struct scv_routing_context *context,
                                                              scv_routing_decision_t decision,
                                                              void *user_data),
                                             void *user_data);

/**
 * Start the router component
 *
 * @param router Router instance
 * @return 0 on success, negative on error
 */
int scv_router_start(struct scv_router *router);

/**
 * Stop the router component
 *
 * @param router Router instance
 * @return 0 on success, negative on error
 */
int scv_router_stop(struct scv_router *router);

/**
 * Get router status string
 *
 * @param router Router instance
 * @param status_buffer Buffer to store status string
 * @param buffer_size Size of buffer
 * @return 0 on success, negative on error
 */
int scv_router_get_status(const struct scv_router *router, char *status_buffer, uint32_t buffer_size);

/**
 * Reset router to initial state
 * 
 * @param router Router instance
 * @return 0 on success, negative on error
 */
int scv_router_reset(struct scv_router *router);

#endif /* __SCV_ROUTER_H__ */
