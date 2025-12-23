/**
 * pal_os_adapter.c - PAL to OS Abstraction Layer Adapter Implementation
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
 * Implementation of PAL OS Adapter that bridges Platform Abstraction Layer
 * with OS abstraction layer used by state machines and VIPER components.
 */

#include "pal_os_adapter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ==================== Internal Structures ==================== */

typedef struct {
    pal_os_adapter_config_t config;
    bool initialized;
    
    /* Hardware event callback */
    void (*hw_event_callback)(const pal_hw_event_t *event, void *context);
    void *hw_event_context;
    
    /* GPIO event queues */
    struct {
        pal_gpio_pin_t pin;
        os_queue_t *queue;
    } gpio_queues[32];
    int gpio_queue_count;
} pal_os_adapter_context_t;

static pal_os_adapter_context_t g_adapter_ctx = {0};

/* ==================== Internal Helper Functions ==================== */

static void gpio_interrupt_callback(void *context)
{
    pal_gpio_pin_t pin = (pal_gpio_pin_t)(uintptr_t)context;
    pal_gpio_state_t state;
    
    /* Read current pin state */
    if (pal_gpio_read(pin, &state) != PAL_STATUS_OK) {
        return;
    }
    
    /* Create hardware event */
    pal_hw_event_t event = {
        .type = PAL_HW_EVENT_GPIO_INTERRUPT,
        .timestamp = pal_get_elapsed_time_ms(),
        .data.gpio = {
            .pin = pin,
            .state = state
        }
    };
    
    /* Call registered callback */
    if (g_adapter_ctx.hw_event_callback) {
        g_adapter_ctx.hw_event_callback(&event, g_adapter_ctx.hw_event_context);
    }
    
    /* Send to GPIO event queue if registered */
    for (int i = 0; i < g_adapter_ctx.gpio_queue_count; i++) {
        if (g_adapter_ctx.gpio_queues[i].pin == pin && 
            g_adapter_ctx.gpio_queues[i].queue) {
            pal_os_adapter_gpio_interrupt_to_event(pin, state, 
                g_adapter_ctx.gpio_queues[i].queue);
        }
    }
}

/* ==================== Public API Implementation ==================== */

os_error_t pal_os_adapter_init(const pal_os_adapter_config_t *config)
{
    if (g_adapter_ctx.initialized) {
        return OS_OK;
    }
    
    if (!config) {
        return OS_ERROR_INVALID_PARAM;
    }
    
    /* Initialize PAL */
    pal_config_t pal_config = {
        .platform = config->target_platform,
        .config_file = config->config_file,
        .simulation_speed = 1,
        .enable_gui = config->enable_gui,
        .enable_logging = true
    };
    
    if (pal_init(&pal_config) != PAL_STATUS_OK) {
        return OS_ERROR;
    }
    
    /* Store configuration */
    memcpy(&g_adapter_ctx.config, config, sizeof(pal_os_adapter_config_t));
    g_adapter_ctx.initialized = true;
    
    printf("PAL OS Adapter initialized for platform: %d\n", config->target_platform);
    
    return OS_OK;
}

os_error_t pal_os_adapter_deinit(void)
{
    if (!g_adapter_ctx.initialized) {
        return OS_ERROR_NOT_SUPPORTED;
    }
    
    /* Deinitialize PAL */
    pal_deinit();
    
    /* Clear context */
    memset(&g_adapter_ctx, 0, sizeof(g_adapter_ctx));
    
    return OS_OK;
}

const char *pal_os_adapter_get_version(void)
{
    return "1.0.0-pal-os-adapter";
}

os_error_t pal_os_adapter_register_hw_event_callback(
    void (*callback)(const pal_hw_event_t *event, void *context),
    void *context)
{
    if (!g_adapter_ctx.initialized) {
        return OS_ERROR_NOT_SUPPORTED;
    }
    
    g_adapter_ctx.hw_event_callback = callback;
    g_adapter_ctx.hw_event_context = context;
    
    return OS_OK;
}

int pal_os_adapter_process_events(void)
{
    if (!g_adapter_ctx.initialized) {
        return 0;
    }
    
    /* Process PAL events (for GUI) */
    if (g_adapter_ctx.config.enable_gui) {
        if (!pal_process_events()) {
            /* Application should exit */
            return -1;
        }
    }
    
    /* Process hardware events */
    // Note: In a real implementation, this would check for pending
    // hardware events and call callbacks
    
    return 0;
}

os_queue_t *pal_os_adapter_create_gpio_event_queue(pal_gpio_pin_t pin, size_t queue_depth)
{
    if (!g_adapter_ctx.initialized) {
        return NULL;
    }
    
    /* Create OS queue */
    os_queue_t *queue = os_queue_create(sizeof(pal_hw_event_t), queue_depth);
    if (!queue) {
        return NULL;
    }
    
    /* Configure GPIO for interrupt */
    pal_gpio_config_t gpio_config = {
        .mode = PAL_GPIO_MODE_INPUT,
        .interrupt = PAL_GPIO_INT_BOTH,
        .debounce_ms = 50,
        .initial_state = false
    };
    
    if (pal_gpio_init(pin, &gpio_config) != PAL_STATUS_OK) {
        os_queue_destroy(queue);
        return NULL;
    }
    
    /* Set interrupt callback */
    if (pal_gpio_set_interrupt_callback(pin, gpio_interrupt_callback, 
                                        (void *)(uintptr_t)pin) != PAL_STATUS_OK) {
        pal_gpio_deinit(pin);
        os_queue_destroy(queue);
        return NULL;
    }
    
    /* Enable interrupt */
    pal_gpio_interrupt_enable(pin, true);
    
    /* Store queue reference */
    if (g_adapter_ctx.gpio_queue_count < 32) {
        g_adapter_ctx.gpio_queues[g_adapter_ctx.gpio_queue_count].pin = pin;
        g_adapter_ctx.gpio_queues[g_adapter_ctx.gpio_queue_count].queue = queue;
        g_adapter_ctx.gpio_queue_count++;
    }
    
    return queue;
}

os_error_t pal_os_adapter_gpio_interrupt_to_event(pal_gpio_pin_t pin,
                                                  pal_gpio_state_t state,
                                                  os_queue_t *event_queue)
{
    if (!event_queue) {
        return OS_ERROR_INVALID_PARAM;
    }
    
    pal_hw_event_t event = {
        .type = PAL_HW_EVENT_GPIO_INTERRUPT,
        .timestamp = pal_get_elapsed_time_ms(),
        .data.gpio = {
            .pin = pin,
            .state = state
        }
    };
    
    return os_queue_send(event_queue, &event, 0);
}

os_timer_t *pal_os_adapter_timer_create(os_timer_callback_t callback,
                                        void *arg,
                                        const char *name)
{
    /* In a real implementation, this would create a PAL-based timer
     * For now, return a stub that uses the underlying OS abstraction */
    return os_timer_create(callback, arg, name);
}

os_thread_t *pal_os_adapter_thread_create(os_thread_entry_t entry,
                                          void *arg,
                                          const char *name,
                                          size_t stack_size,
                                          os_thread_priority_t priority)
{
    /* In a real implementation, this would create a thread using PAL timing
     * For now, use the underlying OS abstraction */
    return os_thread_create(entry, arg, name, stack_size, priority);
}

fsm_os_context_t *pal_os_adapter_create_fsm_context(const pal_os_adapter_config_t *config)
{
    if (!config) {
        return NULL;
    }
    
    /* Initialize adapter if not already initialized */
    if (!g_adapter_ctx.initialized) {
        if (pal_os_adapter_init(config) != OS_OK) {
            return NULL;
        }
    }
    
    /* Create FSM OS context */
    fsm_os_context_t *ctx = fsm_os_context_create();
    if (!ctx) {
        return NULL;
    }
    
    /* Configure context with PAL-based objects */
    // Note: In a full implementation, we would replace OS objects
    // with PAL-based implementations
    
    return ctx;
}

/* ==================== Example Implementations ==================== */

void pal_os_adapter_led_fsm_example(void)
{
    printf("=== PAL OS Adapter LED FSM Example ===\n");
    
    /* Initialize adapter for Windows simulator */
    pal_os_adapter_config_t config = {
        .target_platform = PAL_PLATFORM_WINDOWS_SIMULATOR,
        .enable_gui = true,
        .enable_hardware_events = true,
        .event_queue_size = 10,
        .config_file = NULL
    };
    
    if (pal_os_adapter_init(&config) != OS_OK) {
        printf("Failed to initialize PAL OS Adapter\n");
        return;
    }
    
    /* Create GPIO event queue for LED control */
    os_queue_t *led_queue = pal_os_adapter_create_gpio_event_queue(0, 5);
    if (!led_queue) {
        printf("Failed to create LED GPIO queue\n");
        pal_os_adapter_deinit();
        return;
    }
    
    printf("LED FSM example initialized. Press ESC in GUI window to exit.\n");
    
    /* Main loop */
    while (pal_os_adapter_process_events() >= 0) {
        /* Process events from queue */
        pal_hw_event_t event;
        if (os_queue_receive(led_queue, &event, 100) == OS_OK) {
            printf("GPIO Event: pin=%d, state=%d\n", 
                   event.data.gpio.pin, event.data.gpio.state);
        }
        
        /* Simulate some work */
        os_delay(10);
    }
    
    /* Cleanup */
    os_queue_destroy(led_queue);
    pal_os_adapter_deinit();
    
    printf("LED FSM example completed.\n");
}

void pal_os_adapter_gpio_fsm_example(void)
{
    printf("=== PAL OS Adapter GPIO FSM Example ===\n");
    
    /* This example shows how to use PAL GPIO with a state machine */
    pal_os_adapter_config_t config = {
        .target_platform = PAL_PLATFORM_WINDOWS_SIMULATOR,
        .enable_gui = true,
        .enable_hardware_events = true,
        .event_queue_size = 10,
        .config_file = NULL
    };
    
    if (pal_os_adapter_init(&config) != OS_OK) {
        printf("Failed to initialize PAL OS Adapter\n");
        return;
    }
    
    /* Configure input and output pins */
    pal_gpio_config_t input_config = {
        .mode = PAL_GPIO_MODE_INPUT,
        .interrupt = PAL_GPIO_INT_BOTH,
        .debounce_ms = 20,
        .initial_state = false
    };
    
    pal_gpio_config_t output_config = {
        .mode = PAL_GPIO_MODE_OUTPUT,
        .interrupt = PAL_GPIO_INT_NONE,
        .debounce_ms = 0,
        .initial_state = false
    };
    
    /* Initialize pins */
    pal_gpio_init(0, &input_config);   /* Button input */
    pal_gpio_init(1, &output_config);  /* LED output */
    
    printf("GPIO FSM example initialized.\n");
    printf("  Pin 0: Input (button simulation)\n");
    printf("  Pin 1: Output (LED control)\n");
    printf("Press ESC in GUI window to exit.\n");
    
    /* Simple state machine: toggle LED on button press */
    bool led_state = false;
    pal_gpio_state_t last_button_state = PAL_GPIO_LOW;
    
    while (pal_os_adapter_process_events() >= 0) {
        /* Read button state */
        pal_gpio_state_t button_state;
        if (pal_gpio_read(0, &button_state) == PAL_STATUS_OK) {
            /* Detect rising edge */
            if (button_state == PAL_GPIO_HIGH && last_button_state == PAL_GPIO_LOW) {
                led_state = !led_state;
                pal_gpio_write(1, led_state ? PAL_GPIO_HIGH : PAL_GPIO_LOW);
                printf("Button pressed. LED: %s\n", led_state ? "ON" : "OFF");
            }
            last_button_state = button_state;
        }
        
        os_delay(10);
    }
    
    /* Cleanup */
    pal_gpio_deinit(0);
    pal_gpio_deinit(1);
    pal_os_adapter_deinit();
    
    printf("GPIO FSM example completed.\n");
}
