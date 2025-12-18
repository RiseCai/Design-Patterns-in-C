/**
 * led_fsm_example.c - Example of using LED FSM with OS abstraction layer
 *
 * Copyright (C) 2000-2025 All Right Reserved
 *
 * Demonstrates how to integrate the LED hierarchical state machine
 * with the OS abstraction layer via the LED FSM adapter.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Include required headers with relative paths */
#include "led_fsm.h"
#include "fsm_os_adapter.h"
#include "moore_hierarchical.h"

/* Mock PWM callback */
static void mock_pwm_callback(uint8_t ch, uint8_t value)
{
    printf("[PWM] ch=%u value=%u\n", ch, value);
}

/* Thread entry for LED FSM */
static void led_fsm_thread_entry(void *arg)
{
    led_hsm_t *hsm = (led_hsm_t *)arg;
    fsm_os_context_t *ctx = (fsm_os_context_t *)hsm->user_data;
    
    printf("[LED FSM Thread] started\n");
    
    while (1) {
        /* Wait for an event from the queue */
        int event;
        os_error_t err = fsm_os_receive_event(ctx, &event, sizeof(event), OS_WAIT_FOREVER);
        if (err == OS_OK) {
            printf("[LED FSM Thread] received event %d\n", event);
            led_hsm_dispatch_event(hsm, event);
            led_hsm_update_pwm(hsm);
        }
        
        /* Simulate some processing */
        os_thread_sleep(10);
    }
}

int main(void)
{
    printf("=== LED FSM OS Adapter Example ===\n");
    
    /* Initialize OS abstraction layer */
    os_abstract_init();
    
    /* Create LED FSM adapter */
    fsm_os_context_t *adapter = led_fsm_adapter_create();
    if (!adapter) {
        printf("Failed to create LED FSM adapter\n");
        return 1;
    }
    
    /* Initialize LED FSM */
    led_hsm_t led_hsm;
    led_hsm_init(&led_hsm, mock_pwm_callback);
    led_hsm.user_data = adapter;  /* store adapter for thread access */
    
    /* Start LED FSM thread */
    os_error_t err = fsm_os_start_thread(adapter, led_fsm_thread_entry, &led_hsm);
    if (err != OS_OK) {
        printf("Failed to start LED FSM thread\n");
        fsm_os_context_destroy(adapter);
        return 1;
    }
    
    /* Start PWM update timer (e.g., 50 ms period) */
    err = fsm_os_set_timer(adapter, 50, true);
    if (err != OS_OK) {
        printf("Failed to start PWM timer\n");
    }
    
    /* Simulate sending commands to LED FSM */
    printf("[Main] Sending commands...\n");
    
    /* Power on */
    int event = EV_LED_POWER_ON;
    fsm_os_send_event(adapter, &event, sizeof(event), OS_WAIT_FOREVER);
    os_thread_sleep(100);
    
    /* Set static effect */
    led_hsm_set_effect(&led_hsm, EFFECT_STATIC);
    os_thread_sleep(100);
    
    /* Change color to red */
    led_hsm_set_color(&led_hsm, 255, 0, 0);
    os_thread_sleep(100);
    
    /* Set breath effect */
    led_hsm_set_effect(&led_hsm, EFFECT_BREATH);
    os_thread_sleep(500);
    
    /* Set flow effect */
    led_hsm_set_effect(&led_hsm, EFFECT_FLOW);
    os_thread_sleep(500);
    
    /* Power off */
    event = EV_LED_POWER_OFF;
    fsm_os_send_event(adapter, &event, sizeof(event), OS_WAIT_FOREVER);
    os_thread_sleep(100);
    
    /* Cleanup */
    printf("[Main] Cleaning up...\n");
    fsm_os_stop_thread(adapter, 100);
    fsm_os_context_destroy(adapter);
    
    os_abstract_deinit();
    printf("=== Example finished ===\n");
    return 0;
}
