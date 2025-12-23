/**
 * pal_os_adapter.h - PAL to OS Abstraction Layer Adapter
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
 * Adapter that implements the OS abstraction layer interface using
 * Platform Abstraction Layer (PAL) as the backend.
 * This enables state machines and VIPER components to run on any
 * hardware platform supported by PAL.
 */

#ifndef __PAL_OS_ADAPTER_H__
#define __PAL_OS_ADAPTER_H__

#include "../../os_abstraction_layer/include/os_abstract.h"
#include "../../state_machine_extended/src/fsm_os_adapter.h"
#include "pal.h"
#include "pal_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== Configuration ==================== */

/**
 * PAL OS Adapter configuration
 */
typedef struct {
    pal_platform_t target_platform;   /* Target hardware platform */
    bool enable_gui;                  /* Enable GUI visualization (for simulators) */
    bool enable_hardware_events;      /* Enable hardware event processing */
    uint32_t event_queue_size;        /* Size of event queue for hardware events */
    const char *config_file;          /* Platform configuration file (optional) */
} pal_os_adapter_config_t;

/* ==================== Initialization ==================== */

/**
 * Initialize PAL OS Adapter
 * 
 * @param config Adapter configuration
 * @return OS_OK on success, error code otherwise
 */
os_error_t pal_os_adapter_init(const pal_os_adapter_config_t *config);

/**
 * Deinitialize PAL OS Adapter
 * 
 * @return OS_OK on success, error code otherwise
 */
os_error_t pal_os_adapter_deinit(void);

/**
 * Get PAL OS Adapter version
 * 
 * @return Version string
 */
const char *pal_os_adapter_get_version(void);

/* ==================== Hardware Event Integration ==================== */

/**
 * Hardware event types
 */
typedef enum {
    PAL_HW_EVENT_GPIO_INTERRUPT = 0,  /* GPIO interrupt event */
    PAL_HW_EVENT_TIMER_EXPIRED,       /* Timer expiration event */
    PAL_HW_EVENT_ADC_CONVERSION,      /* ADC conversion complete */
    PAL_HW_EVENT_PWM_PERIOD,          /* PWM period complete */
    PAL_HW_EVENT_UART_RECEIVE,        /* UART receive data available */
    PAL_HW_EVENT_I2C_TRANSFER,        /* I2C transfer complete */
    PAL_HW_EVENT_SPI_TRANSFER,        /* SPI transfer complete */
} pal_hw_event_type_t;

/**
 * Hardware event structure
 */
typedef struct {
    pal_hw_event_type_t type;         /* Event type */
    uint32_t timestamp;               /* Event timestamp in milliseconds */
    union {
        struct {
            pal_gpio_pin_t pin;       /* GPIO pin number */
            pal_gpio_state_t state;   /* Current pin state */
        } gpio;
        struct {
            uint32_t timer_id;        /* Timer identifier */
            void *user_data;          /* User data associated with timer */
        } timer;
        struct {
            uint8_t channel;          /* ADC channel */
            uint16_t value;           /* ADC value */
        } adc;
        struct {
            uint8_t uart_id;          /* UART identifier */
            uint8_t *data;            /* Received data */
            size_t length;            /* Data length */
        } uart;
    } data;
} pal_hw_event_t;

/**
 * Register hardware event callback
 * 
 * @param callback Callback function to call when hardware event occurs
 * @param context User context passed to callback
 * @return OS_OK on success, error code otherwise
 */
os_error_t pal_os_adapter_register_hw_event_callback(
    void (*callback)(const pal_hw_event_t *event, void *context),
    void *context);

/**
 * Process pending hardware events
 * Call this periodically in main loop
 * 
 * @return Number of events processed
 */
int pal_os_adapter_process_events(void);

/* ==================== GPIO to OS Abstraction Mapping ==================== */

/**
 * Create a GPIO event queue for state machine integration
 * 
 * @param pin GPIO pin number
 * @param queue_depth Depth of event queue
 * @return Queue handle, NULL on error
 */
os_queue_t *pal_os_adapter_create_gpio_event_queue(pal_gpio_pin_t pin, size_t queue_depth);

/**
 * Convert GPIO interrupt to OS event
 * 
 * @param pin GPIO pin number
 * @param state Current pin state
 * @param event_queue Queue to send event to
 * @return OS_OK on success, error code otherwise
 */
os_error_t pal_os_adapter_gpio_interrupt_to_event(pal_gpio_pin_t pin,
                                                  pal_gpio_state_t state,
                                                  os_queue_t *event_queue);

/* ==================== Timer Integration ==================== */

/**
 * Create a PAL-based timer that implements os_timer_t interface
 * 
 * @param callback Timer callback function
 * @param arg Callback argument
 * @param name Timer name
 * @return Timer handle, NULL on error
 */
os_timer_t *pal_os_adapter_timer_create(os_timer_callback_t callback,
                                        void *arg,
                                        const char *name);

/* ==================== Thread Integration ==================== */

/**
 * Create a thread using PAL timing and synchronization
 * 
 * @param entry Thread entry function
 * @param arg Thread argument
 * @param name Thread name
 * @param stack_size Stack size in bytes
 * @param priority Thread priority
 * @return Thread handle, NULL on error
 */
os_thread_t *pal_os_adapter_thread_create(os_thread_entry_t entry,
                                          void *arg,
                                          const char *name,
                                          size_t stack_size,
                                          os_thread_priority_t priority);

/* ==================== Example Integration Helpers ==================== */

/**
 * Create a complete FSM OS context using PAL backend
 * 
 * @param config PAL OS adapter configuration
 * @return FSM OS context, NULL on error
 */
fsm_os_context_t *pal_os_adapter_create_fsm_context(const pal_os_adapter_config_t *config);

/**
 * Example: LED FSM using PAL hardware
 */
void pal_os_adapter_led_fsm_example(void);

/**
 * Example: GPIO interrupt handling with state machine
 */
void pal_os_adapter_gpio_fsm_example(void);

#ifdef __cplusplus
}
#endif

#endif /* __PAL_OS_ADAPTER_H__ */
