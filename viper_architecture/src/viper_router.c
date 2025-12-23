/**
 * viper_router.c - VIPER Router Component Implementation
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
 * VIPER Router component implemented as an Acceptor/Regex State Machine.
 * Handles navigation and routing patterns.
 */

#include "../include/viper_router.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Acceptor states for router */
static struct acceptor_state idle_state = {
    .id = 0,
    .is_accepting = 1,
    .transitions = {{0}},
    .transition_count = 0
};

static struct acceptor_state navigating_state = {
    .id = 1,
    .is_accepting = 0,
    .transitions = {{0}},
    .transition_count = 0
};

static struct acceptor_state routing_state = {
    .id = 2,
    .is_accepting = 0,
    .transitions = {{0}},
    .transition_count = 0
};

static struct acceptor_state transitioning_state = {
    .id = 3,
    .is_accepting = 0,
    .transitions = {{0}},
    .transition_count = 0
};

static struct acceptor_state error_state = {
    .id = 4,
    .is_accepting = 0,
    .transitions = {{0}},
    .transition_count = 0
};

/* Helper function to map router state to acceptor state */
static struct acceptor_state *state_to_acceptor(viper_router_state_t state)
{
    switch (state) {
        case ROUTER_STATE_IDLE: return &idle_state;
        case ROUTER_STATE_NAVIGATING: return &navigating_state;
        case ROUTER_STATE_ROUTING: return &routing_state;
        case ROUTER_STATE_TRANSITIONING: return &transitioning_state;
        case ROUTER_STATE_ERROR: return &error_state;
        default: return &idle_state;
    }
}

/* Helper function to map acceptor state to router state */
static viper_router_state_t acceptor_to_state(struct acceptor_state *state)
{
    if (state == &idle_state) return ROUTER_STATE_IDLE;
    if (state == &navigating_state) return ROUTER_STATE_NAVIGATING;
    if (state == &routing_state) return ROUTER_STATE_ROUTING;
    if (state == &transitioning_state) return ROUTER_STATE_TRANSITIONING;
    if (state == &error_state) return ROUTER_STATE_ERROR;
    return ROUTER_STATE_IDLE;
}

/* VIPER Router implementation */
struct viper_router *viper_router_init(const char *router_name,
                                       int router_id)
{
    struct viper_router *router = (struct viper_router *)malloc(sizeof(struct viper_router));
    if (!router) return NULL;
    
    memset(router, 0, sizeof(struct viper_router));
    
    /* Initialize navigation context */
    memset(&router->context, 0, sizeof(struct viper_navigation_context));
    router->context.history_size = 0;
    router->context.history_index = -1;
    
    /* Set name and ID */
    if (router_name) {
        strncpy(router->router_name, router_name, sizeof(router->router_name) - 1);
        router->router_name[sizeof(router->router_name) - 1] = '\0';
    } else {
        strcpy(router->router_name, "VIPER_Router");
    }
    router->router_id = router_id;
    
    /* Initialize acceptor FSM */
    router->fsm = (struct acceptor_machine *)malloc(sizeof(struct acceptor_machine));
    if (!router->fsm) {
        free(router);
        return NULL;
    }
    
    acceptor_machine_init(router->fsm, &idle_state);
    
    /* Initialize route table */
    router->route_capacity = 10;
    router->routes = (struct viper_route *)malloc(sizeof(struct viper_route) * router->route_capacity);
    if (!router->routes) {
        free(router->fsm);
        free(router);
        return NULL;
    }
    router->route_count = 0;
    
    printf("[Router] Initialized '%s' (ID: %d)\n", router->router_name, router->router_id);
    return router;
}

void viper_router_destroy(struct viper_router *router)
{
    if (!router) return;
    
    /* Clean up routes */
    if (router->routes) {
        free(router->routes);
    }
    
    /* Destroy acceptor FSM */
    if (router->fsm) {
        free(router->fsm);
    }
    
    free(router);
    printf("[Router] Destroyed\n");
}

int viper_router_add_route(struct viper_router *router,
                           const char *pattern,
                           const char *destination,
                           int destination_id,
                           const void *context,
                           size_t context_size)
{
    if (!router || !pattern || !destination) return -1;
    
    /* Check capacity */
    if (router->route_count >= router->route_capacity) {
        int new_capacity = router->route_capacity * 2;
        struct viper_route *new_routes = (struct viper_route *)realloc(router->routes,
                                                                       sizeof(struct viper_route) * new_capacity);
        if (!new_routes) return -1;
        router->routes = new_routes;
        router->route_capacity = new_capacity;
    }
    
    struct viper_route *route = &router->routes[router->route_count];
    strncpy(route->pattern, pattern, sizeof(route->pattern) - 1);
    route->pattern[sizeof(route->pattern) - 1] = '\0';
    
    strncpy(route->destination, destination, sizeof(route->destination) - 1);
    route->destination[sizeof(route->destination) - 1] = '\0';
    
    route->destination_id = destination_id;
    
    /* Copy context if provided */
    if (context && context_size > 0) {
        route->context = malloc(context_size);
        if (!route->context) return -1;
        memcpy(route->context, context, context_size);
        route->context_size = context_size;
    } else {
        route->context = NULL;
        route->context_size = 0;
    }
    
    router->route_count++;
    
    printf("[Router] Added route: pattern='%s', destination='%s' (ID: %d)\n",
           pattern, destination, destination_id);
    return 0;
}

int viper_router_navigate(struct viper_router *router,
                          const char *path,
                          const void *navigation_data,
                          size_t data_size)
{
    if (!router || !path) return -1;
    
    printf("[Router] Navigating to path: %s\n", path);
    
    /* Update acceptor state */
    router->fsm->current = &navigating_state;
    
    /* Find matching route */
    struct viper_route *matched_route = NULL;
    for (int i = 0; i < router->route_count; i++) {
        struct viper_route *route = &router->routes[i];
        /* Simple string matching (should be regex) */
        if (strcmp(path, route->pattern) == 0) {
            matched_route = route;
            break;
        }
    }
    
    if (!matched_route) {
        printf("[Router] No route matches path: %s\n", path);
        router->fsm->current = &error_state;
        return -1;
    }
    
    /* Update navigation context */
    if (router->context.history_index < 31) {
        router->context.history_index++;
        router->context.history[router->context.history_index] = *matched_route;
        router->context.history_size = router->context.history_index + 1;
    }
    
    router->context.current_route = *matched_route;
    
    /* Copy navigation data */
    if (navigation_data && data_size > 0) {
        if (router->context.transition_data) {
            free(router->context.transition_data);
        }
        router->context.transition_data = malloc(data_size);
        if (!router->context.transition_data) return -1;
        memcpy(router->context.transition_data, navigation_data, data_size);
        router->context.transition_data_size = data_size;
    }
    
    /* Update state */
    router->fsm->current = &routing_state;
    
    /* Call navigation callback if set */
    if (router->navigation_callback) {
        router->navigation_callback(&router->context.current_route, NULL);
    }
    
    printf("[Router] Navigation successful to destination: %s\n", matched_route->destination);
    return 0;
}

int viper_router_navigate_back(struct viper_router *router)
{
    if (!router) return -1;
    
    if (router->context.history_index <= 0) {
        printf("[Router] No history to navigate back\n");
        return -1;
    }
    
    router->context.history_index--;
    router->context.current_route = router->context.history[router->context.history_index];
    
    printf("[Router] Navigated back to: %s\n", router->context.current_route.destination);
    return 0;
}

int viper_router_navigate_forward(struct viper_router *router)
{
    if (!router) return -1;
    
    if (router->context.history_index >= router->context.history_size - 1) {
        printf("[Router] No forward history\n");
        return -1;
    }
    
    router->context.history_index++;
    router->context.current_route = router->context.history[router->context.history_index];
    
    printf("[Router] Navigated forward to: %s\n", router->context.current_route.destination);
    return 0;
}

const struct viper_route *viper_router_get_current_route(const struct viper_router *router)
{
    if (!router || router->context.history_index < 0) return NULL;
    return &router->context.current_route;
}

viper_router_state_t viper_router_get_state(const struct viper_router *router)
{
    if (!router || !router->fsm) return ROUTER_STATE_ERROR;
    return acceptor_to_state(router->fsm->current);
}

const struct viper_navigation_context *viper_router_get_context(const struct viper_router *router)
{
    if (!router) return NULL;
    return &router->context;
}
