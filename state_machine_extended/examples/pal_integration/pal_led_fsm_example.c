/**
 * pal_led_fsm_example.c - PAL LED FSM Integration Example
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * Demonstrates integration of Platform Abstraction Layer (PAL) with
 * State Machine Extended framework for LED control.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Include required headers */
#include "../../src/fsm_os_adapter.h"
#include "../../examples/product/ui/led/led_fsm.h"
#include "pal_fsm_adapter.h"

/* ==================== Configuration ==================== */

#define LOOP_COUNT 20
#define POLL_INTERVAL_MS 100

/* ==================== Helper Functions ==================== */

/**
 * LED FSM event handler (adapted for fsm_os_adapter)
 */
static void led_fsm_event_handler(fsm_os_context_t *context, 
                                  int event, 
                                  void *data)
{
    (void)data;  /* Unused parameter */
    
    printf("[LED FSM] Event %d received\n", event);
    
    /* Get LED HSM from context user data */
    led_hsm_t *led_hsm = (led_hsm_t *)context->user_data;
    if (led_hsm) {
        led_hsm_dispatch_event(led_hsm, event);
    }
}

/**
 * LED FSM state change callback (optional)
 */
static void led_fsm_state_change_callback(const char *state_name)
{
    printf("[LED FSM] State changed to: %s\n", state_name);
}

/**
 * LED hardware control callback (PWM update)
 */
static void led_hardware_control_callback(uint8_t channel, uint8_t value, void *user_data)
{
    pal_fsm_adapter_t *adapter = (pal_fsm_adapter_t *)user_data;
    if (!adapter) {
        return;
    }
    
    printf("[LED Hardware] Channel %u, Value %u\n", channel, value);
    
    /* Convert PWM value to LED state (simplified: only channel 0 controls LED) */
    if (channel == 0) {
        bool led_on = (value > 127);  /* 50% threshold */
        pal_fsm_adapter_set_led(adapter, led_on);
    }
}

/**
 * PWM update callback for LED HSM
 */
static void pwm_update_callback(uint8_t channel, uint8_t value)
{
    /* This would typically update hardware PWM, but we'll just log */
    printf("[PWM Update] Channel %u = %u\n", channel, value);
}

/* ==================== Main Example ==================== */

int main(int argc, char *argv[])
{
    printf("=== PAL LED FSM Integration Example ===\n");
    printf("Demonstrating hardware abstraction with state machines\n\n");
    
    /* Parse command line arguments */
    pal_platform_t platform = PAL_PLATFORM_WINDOWS_SIMULATOR;
    if (argc > 1) {
        platform = atoi(argv[1]);
    }
    
    printf("Using platform: %d\n", platform);
    
    /* Create PAL FSM adapter */
    pal_fsm_adapter_t *adapter = pal_fsm_adapter_create_led_fsm(platform);
    if (!adapter) {
        printf("Error: Failed to create PAL FSM adapter\n");
        return EXIT_FAILURE;
    }
    
    /* Initialize hardware */
    if (!pal_fsm_adapter_init_hardware(adapter)) {
        printf("Error: Failed to initialize hardware\n");
        pal_fsm_adapter_destroy(adapter);
        return EXIT_FAILURE;
    }
    
    /* Get FSM OS context */
    fsm_os_context_t *fsm_context = pal_fsm_adapter_get_context(adapter);
    if (!fsm_context) {
        printf("Error: Failed to get FSM OS context\n");
        pal_fsm_adapter_destroy(adapter);
        return EXIT_FAILURE;
    }
    
    /* Create LED HSM instance */
    led_hsm_t led_hsm;
    led_hsm_init(&led_hsm, pwm_update_callback);
    
    /* Set user data for context */
    fsm_context->user_data = &led_hsm;
    
    /* Note: Event handler is not set because fsm_os_set_event_handler doesn't exist.
     * In a real implementation, you would need to set up a thread to receive events
     * and call the handler. For this example, we'll directly dispatch events.
     */
    
    printf("\nStarting main loop...\n");
    printf("Press Ctrl+C to exit\n\n");
    
    /* Main loop */
    for (int i = 0; i < LOOP_COUNT; i++) {
        printf("\n--- Iteration %d/%d ---\n", i + 1, LOOP_COUNT);
        
        /* Process hardware events (button presses, etc.) */
        int events_processed = pal_fsm_adapter_process_events(adapter);
        if (events_processed > 0) {
            printf("Processed %d hardware event(s)\n", events_processed);
        }
        
        /* Simulate timer events periodically */
        if (i % 5 == 0) {
            printf("Simulating timer event...\n");
            int timer_event = EV_LED_TIMER_TICK;
            fsm_os_send_event(fsm_context, &timer_event, sizeof(timer_event), 0);
        }
        
        /* Toggle LED based on iteration */
        bool led_state = (i % 2 == 0);
        printf("Setting LED to %s\n", led_state ? "ON" : "OFF");
        pal_fsm_adapter_set_led(adapter, led_state);
        
        /* Read button state */
        bool button_pressed = pal_fsm_adapter_get_button(adapter);
        printf("Button is %s\n", button_pressed ? "PRESSED" : "RELEASED");
        
        /* Update LED HSM (simulate periodic update) */
        led_hsm_update_pwm(&led_hsm);
        
        /* Sleep for polling interval */
        usleep(POLL_INTERVAL_MS * 1000);
    }
    
    printf("\n=== Example Complete ===\n");
    
    /* Cleanup */
    printf("Cleaning up...\n");
    pal_fsm_adapter_destroy(adapter);
    
    return EXIT_SUCCESS;
}
