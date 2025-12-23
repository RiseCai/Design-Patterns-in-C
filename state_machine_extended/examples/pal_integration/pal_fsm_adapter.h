/**
 * pal_fsm_adapter.h - PAL to State Machine Adapter
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * Adapter that bridges Platform Abstraction Layer (PAL) with
 * State Machine Extended framework.
 * 
 * This adapter enables state machines to control hardware through PAL
 * and receive hardware events as state machine events.
 */

#ifndef __PAL_FSM_ADAPTER_H__
#define __PAL_FSM_ADAPTER_H__

#include <stdbool.h>
#include <stdint.h>

/* Include required headers */
#include "../../src/fsm_os_adapter.h"
#include "../../../platform_abstraction_layer/include/pal.h"
#include "../../../platform_abstraction_layer/include/pal_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== Configuration ==================== */

/**
 * PAL FSM Adapter configuration
 */
typedef struct {
    pal_platform_t target_platform;   /* Target hardware platform */
    bool enable_gui;                  /* Enable GUI visualization */
    bool enable_hardware_events;      /* Enable hardware event processing */
    
    /* GPIO configuration */
    pal_gpio_pin_t led_pin;           /* LED output pin */
    pal_gpio_pin_t button_pin;        /* Button input pin */
    
    /* Event queue configuration */
    uint32_t event_queue_size;        /* Size of event queue */
    
    /* Callback for visualization */
    void (*visualization_callback)(const char *state_name, uint32_t color);
} pal_fsm_adapter_config_t;

/* ==================== Adapter Types ==================== */

/**
 * PAL FSM Adapter handle
 */
typedef struct pal_fsm_adapter pal_fsm_adapter_t;

/* ==================== Adapter Functions ==================== */

/**
 * Create a PAL FSM adapter
 * 
 * @param config Adapter configuration
 * @return Adapter handle, NULL on error
 */
pal_fsm_adapter_t *pal_fsm_adapter_create(const pal_fsm_adapter_config_t *config);

/**
 * Destroy a PAL FSM adapter
 * 
 * @param adapter Adapter handle
 */
void pal_fsm_adapter_destroy(pal_fsm_adapter_t *adapter);

/**
 * Get the FSM OS context from the adapter
 * 
 * @param adapter Adapter handle
 * @return FSM OS context, NULL on error
 */
fsm_os_context_t *pal_fsm_adapter_get_context(pal_fsm_adapter_t *adapter);

/**
 * Initialize hardware (GPIO, etc.)
 * 
 * @param adapter Adapter handle
 * @return true on success, false on error
 */
bool pal_fsm_adapter_init_hardware(pal_fsm_adapter_t *adapter);

/**
 * Process hardware events (call periodically)
 * 
 * @param adapter Adapter handle
 * @return Number of events processed
 */
int pal_fsm_adapter_process_events(pal_fsm_adapter_t *adapter);

/**
 * Set LED state via PAL GPIO
 * 
 * @param adapter Adapter handle
 * @param on true to turn LED on, false to turn off
 * @return true on success, false on error
 */
bool pal_fsm_adapter_set_led(pal_fsm_adapter_t *adapter, bool on);

/**
 * Get button state via PAL GPIO
 * 
 * @param adapter Adapter handle
 * @return true if button pressed, false otherwise
 */
bool pal_fsm_adapter_get_button(pal_fsm_adapter_t *adapter);

/**
 * Convert hardware event to state machine event
 * 
 * @param adapter Adapter handle
 * @param hw_event Hardware event type
 * @param data Event data
 * @return State machine event ID, or -1 if no conversion
 */
int pal_fsm_adapter_hw_event_to_fsm_event(pal_fsm_adapter_t *adapter,
                                          uint32_t hw_event,
                                          const void *data);

/**
 * Send hardware event to state machine
 * 
 * @param adapter Adapter handle
 * @param hw_event Hardware event type
 * @param data Event data
 * @return true on success, false on error
 */
bool pal_fsm_adapter_send_hw_event(pal_fsm_adapter_t *adapter,
                                   uint32_t hw_event,
                                   const void *data);

/* ==================== LED FSM Specific Helpers ==================== */

/**
 * Create LED FSM adapter with default configuration
 * 
 * @param platform Target platform
 * @return Adapter handle, NULL on error
 */
pal_fsm_adapter_t *pal_fsm_adapter_create_led_fsm(pal_platform_t platform);

/**
 * LED FSM hardware control callback
 * 
 * @param channel PWM channel (unused for simple GPIO)
 * @param value PWM value (0-255)
 * @param user_data Adapter handle
 */
void pal_fsm_adapter_led_pwm_callback(uint8_t channel, uint8_t value, void *user_data);

#ifdef __cplusplus
}
#endif

#endif /* __PAL_FSM_ADAPTER_H__ */
