/**
 * pal_fsm_adapter.c - PAL to State Machine Adapter Implementation
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * Implementation of the PAL FSM adapter that bridges hardware control
 * with state machine events.
 */

#include "pal_fsm_adapter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ==================== Internal Structures ==================== */

struct pal_fsm_adapter {
    pal_fsm_adapter_config_t config;      /* Adapter configuration */
    fsm_os_context_t *fsm_context;        /* FSM OS context */
    bool hardware_initialized;            /* Hardware initialization flag */
    
    /* Hardware state */
    pal_gpio_state_t last_button_state;   /* Last button state for edge detection */
    uint32_t event_count;                 /* Event counter for debugging */
};

/* ==================== Internal Functions ==================== */

/**
 * Default visualization callback
 */
static void default_visualization_callback(const char *state_name, uint32_t color)
{
    printf("[VISUAL] State: %s, Color: 0x%08X\n", state_name, color);
}

/**
 * Default hardware event to FSM event conversion
 */
static int default_hw_event_to_fsm_event(pal_fsm_adapter_t *adapter,
                                         uint32_t hw_event,
                                         const void *data)
{
    (void)adapter;  /* Unused parameter */
    (void)data;     /* Unused parameter */
    
    /* Simple mapping for POC */
    switch (hw_event) {
        case 0:  /* Button pressed */
            return 1;  /* EV_LED_POWER_ON */
        case 1:  /* Button released */
            return 2;  /* EV_LED_POWER_OFF */
        case 2:  /* Timer expired */
            return 3;  /* EV_LED_TIMER */
        default:
            return -1;
    }
}

/* ==================== Public Functions ==================== */

pal_fsm_adapter_t *pal_fsm_adapter_create(const pal_fsm_adapter_config_t *config)
{
    if (!config) {
        printf("[PAL FSM Adapter] Error: NULL configuration\n");
        return NULL;
    }
    
    /* Allocate adapter */
    pal_fsm_adapter_t *adapter = (pal_fsm_adapter_t *)malloc(sizeof(pal_fsm_adapter_t));
    if (!adapter) {
        printf("[PAL FSM Adapter] Error: Memory allocation failed\n");
        return NULL;
    }
    
    /* Initialize adapter */
    memset(adapter, 0, sizeof(pal_fsm_adapter_t));
    adapter->config = *config;
    
    /* Set default visualization callback if not provided */
    if (!adapter->config.visualization_callback) {
        adapter->config.visualization_callback = default_visualization_callback;
    }
    
    /* Create FSM OS context */
    adapter->fsm_context = fsm_os_context_create();
    if (!adapter->fsm_context) {
        printf("[PAL FSM Adapter] Error: Failed to create FSM OS context\n");
        free(adapter);
        return NULL;
    }
    
    /* Initialize PAL with configuration */
    pal_config_t pal_config = {
        .platform = adapter->config.target_platform,
        .config_file = NULL,
        .simulation_speed = 1,
        .enable_gui = adapter->config.enable_gui,
        .enable_logging = true
    };
    pal_status_t pal_status = pal_init(&pal_config);
    if (pal_status != PAL_STATUS_OK) {
        printf("[PAL FSM Adapter] Warning: PAL initialization failed with status %d\n", pal_status);
    }
    
    printf("[PAL FSM Adapter] Created for platform %d\n", adapter->config.target_platform);
    return adapter;
}

void pal_fsm_adapter_destroy(pal_fsm_adapter_t *adapter)
{
    if (!adapter) {
        return;
    }
    
    printf("[PAL FSM Adapter] Destroying adapter\n");
    
    /* Destroy FSM OS context */
    if (adapter->fsm_context) {
        fsm_os_context_destroy(adapter->fsm_context);
    }
    
    /* Deinitialize PAL */
    pal_deinit();
    
    /* Free adapter */
    free(adapter);
}

fsm_os_context_t *pal_fsm_adapter_get_context(pal_fsm_adapter_t *adapter)
{
    if (!adapter) {
        return NULL;
    }
    return adapter->fsm_context;
}

bool pal_fsm_adapter_init_hardware(pal_fsm_adapter_t *adapter)
{
    if (!adapter) {
        return false;
    }
    
    if (adapter->hardware_initialized) {
        return true;  /* Already initialized */
    }
    
    printf("[PAL FSM Adapter] Initializing hardware...\n");
    
    /* Initialize LED pin as output */
    pal_gpio_config_t led_config = {
        .mode = PAL_GPIO_MODE_OUTPUT,
        .interrupt = PAL_GPIO_INT_NONE,
        .debounce_ms = 0,
        .initial_state = false  /* Start with LOW */
    };
    
    if (pal_gpio_init(adapter->config.led_pin, &led_config) != PAL_STATUS_OK) {
        printf("[PAL FSM Adapter] Error: Failed to initialize LED pin %d\n", 
               adapter->config.led_pin);
        return false;
    }
    
    /* Initialize button pin as input with pull-up */
    if (adapter->config.enable_hardware_events) {
        pal_gpio_config_t button_config = {
            .mode = PAL_GPIO_MODE_INPUT_PULLUP,
            .interrupt = PAL_GPIO_INT_NONE,
            .debounce_ms = 50,  /* 50ms debounce */
            .initial_state = false
        };
        
        if (pal_gpio_init(adapter->config.button_pin, &button_config) != PAL_STATUS_OK) {
            printf("[PAL FSM Adapter] Warning: Failed to initialize button pin %d\n",
                   adapter->config.button_pin);
        } else {
            /* Read initial button state */
            pal_gpio_state_t state;
            if (pal_gpio_read(adapter->config.button_pin, &state) == PAL_STATUS_OK) {
                adapter->last_button_state = state;
            }
        }
    }
    
    adapter->hardware_initialized = true;
    printf("[PAL FSM Adapter] Hardware initialized successfully\n");
    return true;
}

int pal_fsm_adapter_process_events(pal_fsm_adapter_t *adapter)
{
    if (!adapter || !adapter->hardware_initialized) {
        return 0;
    }
    
    int events_processed = 0;
    
    /* Process button events */
    if (adapter->config.enable_hardware_events) {
        pal_gpio_state_t current_state;
        if (pal_gpio_read(adapter->config.button_pin, &current_state) != PAL_STATUS_OK) {
            return 0;
        }
        
        /* Detect button press (falling edge) */
        if (adapter->last_button_state == PAL_GPIO_HIGH &&
            current_state == PAL_GPIO_LOW) {
            printf("[PAL FSM Adapter] Button pressed\n");
            
            /* Send button press event to state machine */
            uint32_t hw_event = 0;  /* Button pressed */
            pal_fsm_adapter_send_hw_event(adapter, hw_event, NULL);
            events_processed++;
        }
        /* Detect button release (rising edge) */
        else if (adapter->last_button_state == PAL_GPIO_LOW &&
                 current_state == PAL_GPIO_HIGH) {
            printf("[PAL FSM Adapter] Button released\n");
            
            /* Send button release event to state machine */
            uint32_t hw_event = 1;  /* Button released */
            pal_fsm_adapter_send_hw_event(adapter, hw_event, NULL);
            events_processed++;
        }
        
        adapter->last_button_state = current_state;
    }
    
    return events_processed;
}

bool pal_fsm_adapter_set_led(pal_fsm_adapter_t *adapter, bool on)
{
    if (!adapter || !adapter->hardware_initialized) {
        return false;
    }
    
    pal_gpio_state_t state = on ? PAL_GPIO_HIGH : PAL_GPIO_LOW;
    pal_status_t status = pal_gpio_write(adapter->config.led_pin, state);
    bool success = (status == PAL_STATUS_OK);
    
    if (success) {
        printf("[PAL FSM Adapter] LED %s\n", on ? "ON" : "OFF");
        
        /* Call visualization callback */
        if (adapter->config.visualization_callback) {
            uint32_t color = on ? 0x00FF00 : 0xFF0000;  /* Green for ON, Red for OFF */
            adapter->config.visualization_callback("LED", color);
        }
    }
    
    return success;
}

bool pal_fsm_adapter_get_button(pal_fsm_adapter_t *adapter)
{
    if (!adapter || !adapter->hardware_initialized) {
        return false;
    }
    
    pal_gpio_state_t state;
    if (pal_gpio_read(adapter->config.button_pin, &state) != PAL_STATUS_OK) {
        return false;
    }
    return (state == PAL_GPIO_LOW);  /* Active low */
}

int pal_fsm_adapter_hw_event_to_fsm_event(pal_fsm_adapter_t *adapter,
                                          uint32_t hw_event,
                                          const void *data)
{
    /* Use default conversion for POC */
    return default_hw_event_to_fsm_event(adapter, hw_event, data);
}

bool pal_fsm_adapter_send_hw_event(pal_fsm_adapter_t *adapter,
                                   uint32_t hw_event,
                                   const void *data)
{
    if (!adapter || !adapter->fsm_context) {
        return false;
    }
    
    /* Convert hardware event to FSM event */
    int fsm_event = pal_fsm_adapter_hw_event_to_fsm_event(adapter, hw_event, data);
    if (fsm_event < 0) {
        printf("[PAL FSM Adapter] Warning: No conversion for HW event %u\n", hw_event);
        return false;
    }
    
    /* Send event to state machine */
    os_error_t err = fsm_os_send_event(adapter->fsm_context, 
                                       &fsm_event, 
                                       sizeof(fsm_event), 
                                       0);  /* Non-blocking */
    
    if (err == OS_OK) {
        adapter->event_count++;
        printf("[PAL FSM Adapter] Sent HW event %u -> FSM event %d (total: %u)\n",
               hw_event, fsm_event, adapter->event_count);
        return true;
    } else {
        printf("[PAL FSM Adapter] Error: Failed to send event %d\n", fsm_event);
        return false;
    }
}

/* ==================== LED FSM Specific Functions ==================== */

pal_fsm_adapter_t *pal_fsm_adapter_create_led_fsm(pal_platform_t platform)
{
    /* Default configuration for LED FSM */
    pal_fsm_adapter_config_t config = {
        .target_platform = platform,
        .enable_gui = true,
        .enable_hardware_events = true,
        .led_pin = 13,      /* Common LED pin */
        .button_pin = 2,    /* Common button pin */
        .event_queue_size = 10,
        .visualization_callback = default_visualization_callback
    };
    
    return pal_fsm_adapter_create(&config);
}

void pal_fsm_adapter_led_pwm_callback(uint8_t channel, uint8_t value, void *user_data)
{
    pal_fsm_adapter_t *adapter = (pal_fsm_adapter_t *)user_data;
    if (!adapter) {
        return;
    }
    
    /* Convert PWM value to LED brightness */
    bool led_on = (value > 0);
    pal_fsm_adapter_set_led(adapter, led_on);
    
    printf("[PAL FSM Adapter] PWM callback: channel=%u, value=%u, LED=%s\n",
           channel, value, led_on ? "ON" : "OFF");
}
