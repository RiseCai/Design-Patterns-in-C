/**
 * scv_event_bus.c - System Coordinator VIPER Event Bus Implementation
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * Event bus for communication between VIPER components.
 * Provides publish-subscribe mechanism for loose coupling.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "scv_event_bus.h"

/* Default maximum subscribers if not specified */
#define DEFAULT_MAX_SUBSCRIBERS 100

/* Internal helper functions */
static scv_event_subscription_t *find_subscription(scv_event_subscription_t *list,
                                                   scv_event_type_t event_type,
                                                   scv_event_handler_t handler);
static void free_subscription_list(scv_event_subscription_t *list);

/**
 * Initialize event bus
 */
scv_event_bus_t *scv_event_bus_init(uint32_t max_subscribers)
{
    scv_event_bus_t *bus = (scv_event_bus_t *)malloc(sizeof(scv_event_bus_t));
    if (!bus) {
        return NULL;
    }
    
    memset(bus, 0, sizeof(scv_event_bus_t));
    
    if (max_subscribers == 0) {
        bus->max_subscribers = DEFAULT_MAX_SUBSCRIBERS;
    } else {
        bus->max_subscribers = max_subscribers;
    }
    
    bus->subscriptions = NULL;
    bus->event_count = 0;
    bus->enabled = true;
    
    return bus;
}

/**
 * Destroy event bus
 */
void scv_event_bus_destroy(scv_event_bus_t *bus)
{
    if (!bus) {
        return;
    }
    
    /* Free all subscriptions */
    free_subscription_list(bus->subscriptions);
    
    /* Free bus */
    free(bus);
}

/**
 * Subscribe to event type
 */
int scv_event_bus_subscribe(scv_event_bus_t *bus, 
                           scv_event_type_t event_type,
                           scv_event_handler_t handler,
                           void *user_data)
{
    if (!bus || !handler) {
        return -1;
    }
    
    if (!bus->enabled) {
        return -2;
    }
    
    /* Check if already subscribed */
    if (find_subscription(bus->subscriptions, event_type, handler)) {
        return -3; /* Already subscribed */
    }
    
    /* Count current subscribers for this event type */
    uint32_t count = 0;
    scv_event_subscription_t *current = bus->subscriptions;
    while (current) {
        if (current->event_type == event_type) {
            count++;
        }
        current = current->next;
    }
    
    /* Check max subscribers */
    if (count >= bus->max_subscribers) {
        return -4; /* Too many subscribers */
    }
    
    /* Create new subscription */
    scv_event_subscription_t *subscription = 
        (scv_event_subscription_t *)malloc(sizeof(scv_event_subscription_t));
    if (!subscription) {
        return -5; /* Memory allocation failed */
    }
    
    subscription->event_type = event_type;
    subscription->handler = handler;
    subscription->user_data = user_data;
    subscription->next = NULL;
    
    /* Add to list */
    if (!bus->subscriptions) {
        bus->subscriptions = subscription;
    } else {
        /* Add to end of list */
        scv_event_subscription_t *last = bus->subscriptions;
        while (last->next) {
            last = last->next;
        }
        last->next = subscription;
    }
    
    return 0; /* Success */
}

/**
 * Unsubscribe from event type
 */
int scv_event_bus_unsubscribe(scv_event_bus_t *bus,
                             scv_event_type_t event_type,
                             scv_event_handler_t handler)
{
    if (!bus || !handler) {
        return -1;
    }
    
    scv_event_subscription_t *prev = NULL;
    scv_event_subscription_t *current = bus->subscriptions;
    
    while (current) {
        if (current->event_type == event_type && current->handler == handler) {
            /* Found subscription to remove */
            if (prev) {
                prev->next = current->next;
            } else {
                bus->subscriptions = current->next;
            }
            free(current);
            return 0; /* Success */
        }
        prev = current;
        current = current->next;
    }
    
    return -2; /* Subscription not found */
}

/**
 * Publish event
 */
int scv_event_bus_publish(scv_event_bus_t *bus, const scv_event_t *event)
{
    if (!bus || !event) {
        return -1;
    }
    
    if (!bus->enabled) {
        return -2;
    }
    
    /* Increment event count */
    bus->event_count++;
    
    /* Notify all subscribers for this event type */
    scv_event_subscription_t *current = bus->subscriptions;
    int notified_count = 0;
    
    while (current) {
        if (current->event_type == event->type) {
            /* Call handler */
            current->handler(event, current->user_data);
            notified_count++;
        }
        current = current->next;
    }
    
    return notified_count;
}

/**
 * Create event
 */
scv_event_t scv_event_create(scv_event_type_t type, 
                            const char *source_component,
                            void *data,
                            size_t data_size)
{
    scv_event_t event;
    event.type = type;
    event.timestamp = (uint32_t)time(NULL);
    event.source_component = source_component;
    event.data = data;
    event.data_size = data_size;
    
    return event;
}

/**
 * Get event bus statistics
 */
int scv_event_bus_get_stats(const scv_event_bus_t *bus,
                           uint32_t *total_events,
                           uint32_t *total_subscribers)
{
    if (!bus || !total_events || !total_subscribers) {
        return -1;
    }
    
    *total_events = bus->event_count;
    
    /* Count subscribers */
    uint32_t count = 0;
    scv_event_subscription_t *current = bus->subscriptions;
    while (current) {
        count++;
        current = current->next;
    }
    *total_subscribers = count;
    
    return 0;
}

/**
 * Enable/disable event bus
 */
void scv_event_bus_set_enabled(scv_event_bus_t *bus, bool enabled)
{
    if (!bus) {
        return;
    }
    bus->enabled = enabled;
}

/**
 * Check if event bus is enabled
 */
bool scv_event_bus_is_enabled(const scv_event_bus_t *bus)
{
    if (!bus) {
        return false;
    }
    return bus->enabled;
}

/* Internal helper functions */

static scv_event_subscription_t *find_subscription(scv_event_subscription_t *list,
                                                   scv_event_type_t event_type,
                                                   scv_event_handler_t handler)
{
    scv_event_subscription_t *current = list;
    while (current) {
        if (current->event_type == event_type && current->handler == handler) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

static void free_subscription_list(scv_event_subscription_t *list)
{
    scv_event_subscription_t *current = list;
    while (current) {
        scv_event_subscription_t *next = current->next;
        free(current);
        current = next;
    }
}

/* Convenience functions for common event types */

/**
 * Publish system startup event
 */
int scv_event_bus_publish_system_startup(scv_event_bus_t *bus, const char *source_component)
{
    scv_event_t event = scv_event_create(SCV_EVENT_SYSTEM_STARTUP, source_component, NULL, 0);
    return scv_event_bus_publish(bus, &event);
}

/**
 * Publish system shutdown event
 */
int scv_event_bus_publish_system_shutdown(scv_event_bus_t *bus, const char *source_component)
{
    scv_event_t event = scv_event_create(SCV_EVENT_SYSTEM_SHUTDOWN, source_component, NULL, 0);
    return scv_event_bus_publish(bus, &event);
}

/**
 * Publish recording started event
 */
int scv_event_bus_publish_recording_started(scv_event_bus_t *bus, 
                                           const char *source_component,
                                           const void *recording_data)
{
    scv_event_t event = scv_event_create(SCV_EVENT_RECORDING_STARTED, 
                                        source_component, 
                                        (void *)recording_data, 
                                        0); /* data_size would need to be known */
    return scv_event_bus_publish(bus, &event);
}

/**
 * Publish error occurred event
 */
int scv_event_bus_publish_error(scv_event_bus_t *bus,
                               const char *source_component,
                               int error_code,
                               const char *error_message)
{
    /* Create error data structure */
    struct {
        int error_code;
        const char *error_message;
    } error_data = {
        .error_code = error_code,
        .error_message = error_message
    };
    
    scv_event_t event = scv_event_create(SCV_EVENT_ERROR_OCCURRED,
                                        source_component,
                                        &error_data,
                                        sizeof(error_data));
    return scv_event_bus_publish(bus, &event);
}

/**
 * Publish subsystem status changed event
 */
int scv_event_bus_publish_subsystem_status_changed(scv_event_bus_t *bus,
                                                  const char *source_component,
                                                  const char *subsystem_name,
                                                  int old_status,
                                                  int new_status)
{
    /* Create status change data */
    struct {
        const char *subsystem_name;
        int old_status;
        int new_status;
    } status_data = {
        .subsystem_name = subsystem_name,
        .old_status = old_status,
        .new_status = new_status
    };
    
    scv_event_t event = scv_event_create(SCV_EVENT_SUBSYSTEM_STATUS_CHANGED,
                                        source_component,
                                        &status_data,
                                        sizeof(status_data));
    return scv_event_bus_publish(bus, &event);
}
