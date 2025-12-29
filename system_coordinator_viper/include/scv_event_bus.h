/**
 * scv_event_bus.h - System Coordinator VIPER Event Bus
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * Event bus for communication between VIPER components.
 * Provides publish-subscribe mechanism for loose coupling.
 */

#ifndef SCV_EVENT_BUS_H
#define SCV_EVENT_BUS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Event types */
typedef enum {
    SCV_EVENT_SYSTEM_STARTUP = 0,
    SCV_EVENT_SYSTEM_SHUTDOWN,
    SCV_EVENT_RECORDING_STARTED,
    SCV_EVENT_RECORDING_STOPPED,
    SCV_EVENT_UPLOAD_STARTED,
    SCV_EVENT_UPLOAD_COMPLETED,
    SCV_EVENT_UPLOAD_FAILED,
    SCV_EVENT_OTA_AVAILABLE,
    SCV_EVENT_OTA_STARTED,
    SCV_EVENT_OTA_COMPLETED,
    SCV_EVENT_OTA_FAILED,
    SCV_EVENT_BATTERY_LOW,
    SCV_EVENT_BATTERY_CRITICAL,
    SCV_EVENT_STORAGE_LOW,
    SCV_EVENT_NETWORK_CONNECTED,
    SCV_EVENT_NETWORK_DISCONNECTED,
    SCV_EVENT_ERROR_OCCURRED,
    SCV_EVENT_SUBSYSTEM_STATUS_CHANGED,
    SCV_EVENT_COORDINATION_EVENT,
    SCV_EVENT_USER_INTERACTION,
    SCV_EVENT_CUSTOM = 1000  /* For user-defined events */
} scv_event_type_t;

/* Event structure */
typedef struct {
    scv_event_type_t type;
    uint32_t timestamp;
    const char *source_component;
    void *data;
    size_t data_size;
} scv_event_t;

/* Event handler callback */
typedef void (*scv_event_handler_t)(const scv_event_t *event, void *user_data);

/* Event subscription */
typedef struct scv_event_subscription {
    scv_event_type_t event_type;
    scv_event_handler_t handler;
    void *user_data;
    struct scv_event_subscription *next;
} scv_event_subscription_t;

/* Event bus */
typedef struct {
    scv_event_subscription_t *subscriptions;
    uint32_t event_count;
    uint32_t max_subscribers;
    bool enabled;
} scv_event_bus_t;

/**
 * Initialize event bus
 */
scv_event_bus_t *scv_event_bus_init(uint32_t max_subscribers);

/**
 * Destroy event bus
 */
void scv_event_bus_destroy(scv_event_bus_t *bus);

/**
 * Subscribe to event type
 */
int scv_event_bus_subscribe(scv_event_bus_t *bus, 
                           scv_event_type_t event_type,
                           scv_event_handler_t handler,
                           void *user_data);

/**
 * Unsubscribe from event type
 */
int scv_event_bus_unsubscribe(scv_event_bus_t *bus,
                             scv_event_type_t event_type,
                             scv_event_handler_t handler);

/**
 * Publish event
 */
int scv_event_bus_publish(scv_event_bus_t *bus, const scv_event_t *event);

/**
 * Create event
 */
scv_event_t scv_event_create(scv_event_type_t type, 
                            const char *source_component,
                            void *data,
                            size_t data_size);

/**
 * Get event bus statistics
 */
int scv_event_bus_get_stats(const scv_event_bus_t *bus,
                           uint32_t *total_events,
                           uint32_t *total_subscribers);

/**
 * Enable/disable event bus
 */
void scv_event_bus_set_enabled(scv_event_bus_t *bus, bool enabled);

/**
 * Check if event bus is enabled
 */
bool scv_event_bus_is_enabled(const scv_event_bus_t *bus);

#ifdef __cplusplus
}
#endif

#endif /* SCV_EVENT_BUS_H */
