/**
 * scv_router.c - System Coordinator VIPER Router Component
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * Router component implemented as an Acceptor FSM.
 * Handles navigation, subsystem routing, and pattern matching.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "scv_router.h"

/* Default routing configuration */
static const struct scv_routing_config default_config = {
    .max_routing_table_entries = 100,
    .max_pattern_length = 128,
    .enable_pattern_caching = true,
    .enable_route_optimization = true,
    .max_route_hops = 10,
    .route_timeout_ms = 5000,
    .max_pattern_matches = 10,
    .case_sensitive_matching = true,
    .routing_thread_priority = 5,
    .max_concurrent_routes = 20,
};

/* Internal helper functions */
static int router_state_init_process(struct scv_router *router);
static int router_state_idle_process(struct scv_router *router);
static int router_state_routing_process(struct scv_router *router);
static int router_state_pattern_matching_process(struct scv_router *router);
static int router_state_error_process(struct scv_router *router);

static void router_state_init_entry(struct scv_router *router);
static void router_state_init_exit(struct scv_router *router);
static void router_state_idle_entry(struct scv_router *router);
static void router_state_idle_exit(struct scv_router *router);
static void router_state_routing_entry(struct scv_router *router);
static void router_state_routing_exit(struct scv_router *router);
static void router_state_pattern_matching_entry(struct scv_router *router);
static void router_state_pattern_matching_exit(struct scv_router *router);
static void router_state_error_entry(struct scv_router *router);
static void router_state_error_exit(struct scv_router *router);

/* Acceptor FSM helpers */
static acceptor_regex *create_acceptor_for_pattern(const char *pattern);
static void destroy_acceptor(acceptor_regex *acceptor);
static int match_acceptor(acceptor_regex *acceptor, const char *input, uint32_t length);

/* Internal router context */
struct scv_router_internal {
    struct scv_router public;
    acceptor_regex **acceptors; /* Array of acceptors for each pattern */
    uint32_t acceptor_count;
    /* Separate user data for each callback */
    void *route_matched_user_data;
    void *routing_decision_user_data;
    void *route_completed_user_data;
};

/* State handlers */
static int (*state_process_funcs[])(struct scv_router *) = {
    router_state_init_process,
    router_state_idle_process,
    router_state_routing_process,
    router_state_pattern_matching_process,
    router_state_error_process,
};

static void (*state_entry_funcs[])(struct scv_router *) = {
    router_state_init_entry,
    router_state_idle_entry,
    router_state_routing_entry,
    router_state_pattern_matching_entry,
    router_state_error_entry,
};

static void (*state_exit_funcs[])(struct scv_router *) = {
    router_state_init_exit,
    router_state_idle_exit,
    router_state_routing_exit,
    router_state_pattern_matching_exit,
    router_state_error_exit,
};

/* Helper to get internal router */
static struct scv_router_internal *get_internal(struct scv_router *router) {
    return (struct scv_router_internal *)router;
}

/* State implementations */

static int router_state_init_process(struct scv_router *router) {
    /* Initialize routing table and acceptors */
    struct scv_router_internal *internal = get_internal(router);
    if (!internal) return -1;

    /* Allocate routing table */
    router->routing_table = (struct scv_routing_table_entry *)calloc(
        router->config.max_routing_table_entries,
        sizeof(struct scv_routing_table_entry));
    if (!router->routing_table) {
        return -1;
    }
    router->routing_table_size = router->config.max_routing_table_entries;
    router->routing_table_count = 0;

    /* Allocate acceptors array */
    internal->acceptors = (acceptor_regex **)calloc(
        router->config.max_routing_table_entries,
        sizeof(acceptor_regex *));
    if (!internal->acceptors) {
        free(router->routing_table);
        router->routing_table = NULL;
        return -1;
    }
    internal->acceptor_count = 0;

    /* Create acceptor FSM for pattern matching */
    router->acceptor_fsm = create_acceptor_for_pattern(".*");
    if (!router->acceptor_fsm) {
        free(internal->acceptors);
        free(router->routing_table);
        router->routing_table = NULL;
        return -1;
    }

    /* Add default catch-all pattern */
    struct scv_route_pattern default_pattern;
    memset(&default_pattern, 0, sizeof(default_pattern));
    strcpy(default_pattern.pattern, ".*");
    default_pattern.type = ROUTE_TYPE_EVENT;
    default_pattern.priority = 0;
    default_pattern.target = NULL;
    // fprintf(stderr, "[DEBUG] router_state_init_process: before add_pattern, routing_table=%p, size=%u, count=%u\n",
    //         (void*)router->routing_table, router->routing_table_size, router->routing_table_count);
    // fflush(stderr);
    int add_result = scv_router_add_pattern(router, &default_pattern);
    // if (add_result != 0) {
    //     // fprintf(stderr, "[DEBUG] router_state_init_process: failed to add default pattern, error=%d\n", add_result);
    //     /* If we can't add default pattern, still continue but log? */
    // } else {
    //     // fprintf(stderr, "[DEBUG] router_state_init_process: default pattern added successfully\n");
    // }
    // fflush(stderr);

    return 0;
}

static void router_state_init_entry(struct scv_router *router) {
    /* Entry actions for INIT state */
    router->current_state = ROUTER_STATE_INIT;
    router->active_routes = 0;
    router->total_routes_processed = 0;
}

static void router_state_init_exit(struct scv_router *router) {
    /* Exit actions for INIT state */
}

static int router_state_idle_process(struct scv_router *router) {
    /* Idle state - wait for routing requests */
    return 0;
}

static void router_state_idle_entry(struct scv_router *router) {
    router->current_state = ROUTER_STATE_IDLE;
}

static void router_state_idle_exit(struct scv_router *router) {
}

static int router_state_routing_process(struct scv_router *router) {
    /* Routing state - process active routes */
    /* For simplicity, just decrement active routes over time */
    if (router->active_routes > 0) {
        router->active_routes--;
        router->total_routes_processed++;
    }
    return 0;
}

static void router_state_routing_entry(struct scv_router *router) {
    router->current_state = ROUTER_STATE_ROUTING;
}

static void router_state_routing_exit(struct scv_router *router) {
}

static int router_state_pattern_matching_process(struct scv_router *router) {
    /* Pattern matching state - match patterns against incoming messages */
    return 0;
}

static void router_state_pattern_matching_entry(struct scv_router *router) {
    router->current_state = ROUTER_STATE_PATTERN_MATCHING;
}

static void router_state_pattern_matching_exit(struct scv_router *router) {
}

static int router_state_error_process(struct scv_router *router) {
    /* Error state - handle routing errors */
    return 0;
}

static void router_state_error_entry(struct scv_router *router) {
    router->current_state = ROUTER_STATE_ERROR;
}

static void router_state_error_exit(struct scv_router *router) {
}

/* Acceptor FSM helpers - simplified implementations */

static acceptor_regex *create_acceptor_for_pattern(const char *pattern) {
    // fprintf(stderr, "[DEBUG] create_acceptor_for_pattern: pattern='%s'\n", pattern);
    /* Simplified: create a dummy acceptor */
    acceptor_regex *am = (acceptor_regex *)malloc(sizeof(acceptor_regex));
    if (!am) {
        // fprintf(stderr, "[DEBUG] create_acceptor_for_pattern: malloc am failed\n");
        return NULL;
    }
    memset(am, 0, sizeof(acceptor_regex));
    /* Create a single accepting state */
    struct acceptor_state *state = (struct acceptor_state *)malloc(sizeof(struct acceptor_state));
    if (!state) {
        // fprintf(stderr, "[DEBUG] create_acceptor_for_pattern: malloc state failed\n");
        free(am);
        return NULL;
    }
    memset(state, 0, sizeof(struct acceptor_state));
    state->id = 0;
    state->is_accepting = 1;
    am->start = state;
    am->current = state;
    // fprintf(stderr, "[DEBUG] create_acceptor_for_pattern: success, am=%p, state=%p\n", (void*)am, (void*)state);
    return am;
}

static void destroy_acceptor(acceptor_regex *acceptor) {
    if (!acceptor) return;
    if (acceptor->start) {
        free(acceptor->start);
    }
    free(acceptor);
}

static int match_acceptor(acceptor_regex *acceptor, const char *input, uint32_t length) {
    /* Simplified: always accept */
    (void)input;
    (void)length;
    return acceptor_machine_is_accepting(acceptor);
}

/* Public API implementation */

struct scv_router *scv_router_init(const struct scv_routing_config *config,
                                   struct scv_entity *entity,
                                   struct scv_interactor *interactor,
                                   struct scv_presenter *presenter,
                                   struct scv_view *view) {
    struct scv_router_internal *router = (struct scv_router_internal *)malloc(sizeof(struct scv_router_internal));
    if (!router) {
        return NULL;
    }
    memset(router, 0, sizeof(struct scv_router_internal));

    /* Initialize configuration */
    if (config) {
        router->public.config = *config;
    } else {
        router->public.config = default_config;
    }

    /* Associate components */
    router->public.entity = entity;
    router->public.interactor = interactor;
    router->public.presenter = presenter;
    router->public.view = view;

    /* Initialize state */
    router->public.current_state = ROUTER_STATE_INIT;
    router->public.active_routes = 0;
    router->public.total_routes_processed = 0;

    /* Initialize routing table and acceptors */
    router->public.routing_table = NULL;
    router->public.routing_table_size = 0;
    router->public.routing_table_count = 0;
    router->public.acceptor_fsm = NULL;

    /* Call state entry for INIT */
    router_state_init_entry(&router->public);
    int result = router_state_init_process(&router->public);
    if (result != 0) {
        free(router);
        return NULL;
    }

    return &router->public;
}

void scv_router_destroy(struct scv_router *router) {
    if (!router) return;

    struct scv_router_internal *internal = get_internal(router);

    /* Destroy acceptors */
    if (internal->acceptors) {
        for (uint32_t i = 0; i < internal->acceptor_count; i++) {
            if (internal->acceptors[i]) {
                destroy_acceptor(internal->acceptors[i]);
            }
        }
        free(internal->acceptors);
    }

    /* Destroy acceptor FSM */
    if (router->acceptor_fsm) {
        destroy_acceptor(router->acceptor_fsm);
    }

    /* Free routing table */
    if (router->routing_table) {
        free(router->routing_table);
    }

    free(internal);
}

int scv_router_add_pattern(struct scv_router *router, const struct scv_route_pattern *pattern) {
    // fprintf(stderr, "[DEBUG] scv_router_add_pattern ENTER: router=%p, pattern='%s'\n", (void*)router, pattern->pattern);
    if (!router || !pattern) {
        // fprintf(stderr, "[DEBUG] scv_router_add_pattern: invalid params\n");
        return -1;
    }

    /* Check if routing table is full */
    // fprintf(stderr, "[DEBUG] scv_router_add_pattern: routing_table_count=%u, routing_table_size=%u\n",
    //         router->routing_table_count, router->routing_table_size);
    if (router->routing_table_count >= router->routing_table_size) {
        // fprintf(stderr, "[DEBUG] scv_router_add_pattern: routing table full\n");
        return -1;
    }

    /* Add to routing table */
    struct scv_routing_table_entry *entry = &router->routing_table[router->routing_table_count];
    memset(entry, 0, sizeof(struct scv_routing_table_entry));
    strncpy(entry->pattern.pattern, pattern->pattern, sizeof(entry->pattern.pattern) - 1);
    entry->pattern.type = pattern->type;
    entry->pattern.priority = pattern->priority;
    entry->pattern.target = pattern->target;
    entry->is_active = true;
    entry->usage_count = 0;
    entry->last_used_time = (uint64_t)time(NULL);

    /* Create acceptor for pattern */
    struct scv_router_internal *internal = get_internal(router);
    // fprintf(stderr, "[DEBUG] scv_router_add_pattern: internal->acceptor_count=%u\n", internal->acceptor_count);
    if (internal->acceptor_count < router->routing_table_size) {
        // fprintf(stderr, "[DEBUG] scv_router_add_pattern: calling create_acceptor_for_pattern\n");
        internal->acceptors[internal->acceptor_count] = create_acceptor_for_pattern(pattern->pattern);
        if (!internal->acceptors[internal->acceptor_count]) {
            // fprintf(stderr, "[DEBUG] scv_router_add_pattern: create_acceptor_for_pattern failed\n");
            /* Rollback */
            memset(entry, 0, sizeof(struct scv_routing_table_entry));
            return -1;
        }
        internal->acceptor_count++;
        // fprintf(stderr, "[DEBUG] scv_router_add_pattern: acceptor created, count=%u\n", internal->acceptor_count);
    } else {
        // fprintf(stderr, "[DEBUG] scv_router_add_pattern: acceptor array full\n");
    }

    router->routing_table_count++;
    // fprintf(stderr, "[DEBUG] scv_router_add_pattern: success, new count=%u\n", router->routing_table_count);
    return 0;
}

int scv_router_remove_pattern(struct scv_router *router, const struct scv_route_pattern *pattern) {
    if (!router || !pattern) {
        return -1;
    }

    struct scv_router_internal *internal = get_internal(router);

    /* Find pattern in routing table */
    for (uint32_t i = 0; i < router->routing_table_count; i++) {
        struct scv_routing_table_entry *entry = &router->routing_table[i];
        if (strcmp(entry->pattern.pattern, pattern->pattern) == 0 &&
            entry->pattern.type == pattern->type &&
            entry->pattern.priority == pattern->priority &&
            entry->pattern.target == pattern->target) {
            /* Destroy acceptor for this pattern */
            if (i < internal->acceptor_count && internal->acceptors[i]) {
                destroy_acceptor(internal->acceptors[i]);
                /* Move last acceptor to this position */
                if (internal->acceptor_count > 1 && i != internal->acceptor_count - 1) {
                    internal->acceptors[i] = internal->acceptors[internal->acceptor_count - 1];
                }
                internal->acceptor_count--;
                internal->acceptors[internal->acceptor_count] = NULL;
            }

            /* Move last routing table entry to this position */
            if (router->routing_table_count > 1 && i != router->routing_table_count - 1) {
                router->routing_table[i] = router->routing_table[router->routing_table_count - 1];
                /* Also need to move acceptor pointer if we didn't already */
                if (i < internal->acceptor_count && internal->acceptors[i]) {
                    /* Already moved above */
                }
            }
            router->routing_table_count--;
            return 0;
        }
    }

    return -1; /* Not found */
}

scv_routing_decision_t scv_router_route_message(struct scv_router *router,
                                                const char *message,
                                                uint32_t message_length,
                                                struct scv_routing_context *context) {
    // fprintf(stderr, "[DEBUG] scv_router_route_message ENTER: router=%p, message='%s'\n", (void*)router, message);
    if (!router || !message) {
        // fprintf(stderr, "[DEBUG] scv_router_route_message: invalid params\n");
        return ROUTING_DECISION_DROP;
    }
    // fprintf(stderr, "[DEBUG] scv_router_route_message: router->routing_table=%p\n", (void*)router->routing_table);

    /* Match against patterns */
    struct scv_route_match matches[10];
    uint32_t num_matches = scv_router_match_patterns(router, message, message_length, matches, 10);

    /* Debug */
    // fprintf(stderr, "[DEBUG] scv_router_route_message: message='%s', num_matches=%u\n", message, num_matches);

    if (num_matches == 0) {
        return ROUTING_DECISION_DROP;
    }

    /* Use highest priority match */
    struct scv_route_match *best_match = &matches[0];
    for (uint32_t i = 1; i < num_matches; i++) {
        if (matches[i].pattern->priority > best_match->pattern->priority) {
            best_match = &matches[i];
        }
    }

    /* Update context if provided */
    if (context) {
        context->match_result = *best_match;
        context->decision = ROUTING_DECISION_CONTINUE;
    }

    /* Call route matched callback */
    if (router->route_matched_callback) {
        struct scv_router_internal *internal = get_internal(router);
        router->route_matched_callback(best_match, internal->route_matched_user_data);
    }

    /* For simplicity, always continue */
    return ROUTING_DECISION_CONTINUE;
}

uint32_t scv_router_match_patterns(struct scv_router *router,
                                   const char *message,
                                   uint32_t message_length,
                                   struct scv_route_match *matches,
                                   uint32_t max_matches) {
    if (!router || !message || !matches || max_matches == 0) {
        return 0;
    }

    uint32_t match_count = 0;
    struct scv_router_internal *internal = get_internal(router);

    // fprintf(stderr, "[DEBUG] scv_router_match_patterns: routing_table_count=%u, acceptor_count=%u\n",
    //         router->routing_table_count, internal->acceptor_count);

    for (uint32_t i = 0; i < router->routing_table_count && match_count < max_matches; i++) {
        struct scv_routing_table_entry *entry = &router->routing_table[i];
        if (!entry->is_active) {
            // fprintf(stderr, "[DEBUG] entry %u inactive\n", i);
            continue;
        }

        /* Try to match using acceptor */
        if (i < internal->acceptor_count && internal->acceptors[i]) {
            int result = match_acceptor(internal->acceptors[i], message, message_length);
            // fprintf(stderr, "[DEBUG] entry %u pattern='%s' result=%d\n", i, entry->pattern.pattern, result);
            if (result) {
                /* Match found */
                struct scv_route_match *match = &matches[match_count];
                match->pattern = &entry->pattern;
                strncpy(match->matched_string, message, sizeof(match->matched_string) - 1);
                match->match_start = 0;
                match->match_end = message_length;
                match->captured_data = NULL;
                match_count++;
                // fprintf(stderr, "[DEBUG] match added\n");
            }
        } else {
            // fprintf(stderr, "[DEBUG] entry %u no acceptor\n", i);
        }
    }

    // fprintf(stderr, "[DEBUG] total matches=%u\n", match_count);
    return match_count;
}

int scv_router_update_config(struct scv_router *router, const struct scv_routing_config *config) {
    if (!router || !config) {
        return -1;
    }
    router->config = *config;
    return 0;
}

const struct scv_routing_config *scv_router_get_config(const struct scv_router *router) {
    if (!router) {
        return NULL;
    }
    return &router->config;
}

void scv_router_set_entity(struct scv_router *router, struct scv_entity *entity) {
    if (!router) return;
    router->entity = entity;
}

void scv_router_set_interactor(struct scv_router *router, struct scv_interactor *interactor) {
    if (!router) return;
    router->interactor = interactor;
}

void scv_router_set_presenter(struct scv_router *router, struct scv_presenter *presenter) {
    if (!router) return;
    router->presenter = presenter;
}

void scv_router_set_view(struct scv_router *router, struct scv_view *view) {
    if (!router) return;
    router->view = view;
}

scv_router_state_t scv_router_get_state(const struct scv_router *router) {
    if (!router) {
        return ROUTER_STATE_INIT;
    }
    return router->current_state;
}

int scv_router_get_statistics(const struct scv_router *router,
                              uint32_t *total_entries,
                              uint32_t *active_entries) {
    if (!router || !total_entries || !active_entries) {
        return -1;
    }
    *total_entries = router->routing_table_count;
    *active_entries = 0;
    for (uint32_t i = 0; i < router->routing_table_count; i++) {
        if (router->routing_table[i].is_active) {
            (*active_entries)++;
        }
    }
    return 0;
}

int scv_router_clear_table(struct scv_router *router) {
    if (!router) {
        return -1;
    }
    struct scv_router_internal *internal = get_internal(router);
    /* Destroy all acceptors */
    for (uint32_t i = 0; i < internal->acceptor_count; i++) {
        if (internal->acceptors[i]) {
            destroy_acceptor(internal->acceptors[i]);
            internal->acceptors[i] = NULL;
        }
    }
    internal->acceptor_count = 0;
    /* Clear routing table */
    for (uint32_t i = 0; i < router->routing_table_count; i++) {
        router->routing_table[i].is_active = false;
    }
    router->routing_table_count = 0;
    return 0;
}

void scv_router_set_route_matched_callback(struct scv_router *router,
                                           void (*callback)(const struct scv_route_match *match, void *user_data),
                                           void *user_data) {
    if (!router) return;
    struct scv_router_internal *internal = get_internal(router);
    router->route_matched_callback = callback;
    internal->route_matched_user_data = user_data;
    router->user_data = user_data; /* Keep for backward compatibility */
}

void scv_router_set_routing_decision_callback(struct scv_router *router,
                                              void (*callback)(const struct scv_routing_context *context, void *user_data),
                                              void *user_data) {
    if (!router) return;
    struct scv_router_internal *internal = get_internal(router);
    router->routing_decision_callback = callback;
    internal->routing_decision_user_data = user_data;
    /* Do NOT overwrite router->user_data to preserve first callback's user data */
}

void scv_router_set_route_completed_callback(struct scv_router *router,
                                             void (*callback)(const struct scv_routing_context *context,
                                                              scv_routing_decision_t decision,
                                                              void *user_data),
                                             void *user_data) {
    if (!router) return;
    struct scv_router_internal *internal = get_internal(router);
    router->route_completed_callback = callback;
    internal->route_completed_user_data = user_data;
    /* Do NOT overwrite router->user_data to preserve first callback's user data */
}

int scv_router_reset(struct scv_router *router) {
    if (!router) {
        return -1;
    }
    struct scv_router_internal *internal = get_internal(router);
    /* Destroy all acceptors */
    for (uint32_t i = 0; i < internal->acceptor_count; i++) {
        if (internal->acceptors[i]) {
            destroy_acceptor(internal->acceptors[i]);
            internal->acceptors[i] = NULL;
        }
    }
    internal->acceptor_count = 0;
    /* Reset routing table */
    for (uint32_t i = 0; i < router->routing_table_count; i++) {
        router->routing_table[i].is_active = false;
    }
    router->routing_table_count = 0;
    router->active_routes = 0;
    router->current_state = ROUTER_STATE_INIT;
    return 0;
}

/**
 * Start the router component
 */
int scv_router_start(struct scv_router *router) {
    if (!router) {
        return -1;
    }
    /* Transition from INIT to IDLE state */
    router_state_init_exit(router);
    router_state_idle_entry(router);
    return 0;
}

/**
 * Stop the router component
 */
int scv_router_stop(struct scv_router *router) {
    if (!router) {
        return -1;
    }
    /* Transition to INIT state */
    router_state_idle_exit(router);
    router_state_init_entry(router);
    return 0;
}

/**
 * Get router status string
 */
int scv_router_get_status(const struct scv_router *router, char *status_buffer, uint32_t buffer_size) {
    if (!router || !status_buffer || buffer_size == 0) {
        return -1;
    }
    const char *state_str = "UNKNOWN";
    switch (router->current_state) {
        case ROUTER_STATE_INIT: state_str = "INIT"; break;
        case ROUTER_STATE_IDLE: state_str = "IDLE"; break;
        case ROUTER_STATE_ROUTING: state_str = "ROUTING"; break;
        case ROUTER_STATE_PATTERN_MATCHING: state_str = "PATTERN_MATCHING"; break;
        case ROUTER_STATE_ERROR: state_str = "ERROR"; break;
    }
    int written = snprintf(status_buffer, buffer_size,
                           "Router state: %s, active routes: %u, total processed: %u",
                           state_str,
                           router->active_routes,
                           router->total_routes_processed);
    if (written < 0 || (uint32_t)written >= buffer_size) {
        return -2;
    }
    return 0;
}
