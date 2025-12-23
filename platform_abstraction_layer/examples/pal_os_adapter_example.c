/**
 * pal_os_adapter_example.c - PAL OS Adapter Example
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
 * Example demonstrating integration of Platform Abstraction Layer (PAL)
 * with State Machine Extended framework through OS abstraction layer.
 */

#include <stdio.h>
#include <stdlib.h>
#include "../../state_machine_extended/src/fsm_os_adapter.h"
#include "../../state_machine_extended/src/mealy.h"
#include "../../state_machine_extended/src/moore_hierarchical.h"
#include "../../viper_architecture/include/viper.h"
#include "../include/pal_os_adapter.h"

/* ==================== Example State Machines ==================== */

/* Simple LED control state machine using Mealy machine */
typedef enum {
    LED_STATE_OFF,
    LED_STATE_ON,
    LED_STATE_BLINKING,
    LED_STATE_COUNT
} led_state_t;

typedef enum {
    LED_EVENT_BUTTON_PRESS,
    LED_EVENT_TIMER_EXPIRED,
    LED_EVENT_COUNT
} led_event_t;

static void led_state_machine_example(void)
{
    printf("\n=== LED State Machine Example ===\n");
    
    /* Initialize PAL OS adapter */
    pal_os_adapter_config_t config = {
        .target_platform = PAL_PLATFORM_WINDOWS_SIMULATOR,
        .enable_gui = true,
        .enable_hardware_events = true,
        .event_queue_size = 10,
        .config_file = NULL
    };
    
    if (pal_os_adapter_init(&config) != OS_OK) {
        printf("Failed to initialize PAL OS adapter\n");
        return;
    }
    
    /* Create FSM OS context using PAL backend */
    fsm_os_context_t *ctx = pal_os_adapter_create_fsm_context(&config);
    if (!ctx) {
        printf("Failed to create FSM OS context\n");
        pal_os_adapter_deinit();
        return;
    }
    
    /* Create a Mealy machine for LED control */
    mealy_machine_t *led_fsm = mealy_machine_create(LED_STATE_COUNT, LED_EVENT_COUNT, ctx);
    if (!led_fsm) {
        printf("Failed to create LED state machine\n");
        fsm_os_context_destroy(ctx);
        pal_os_adapter_deinit();
        return;
    }
    
    /* Define state transition table */
    // In a real implementation, you would define transitions and actions
    
    printf("LED state machine created successfully\n");
    printf("Running with PAL Windows simulator GUI...\n");
    
    /* Main loop */
    int iterations = 0;
    while (pal_os_adapter_process_events() >= 0 && iterations < 100) {
        /* Simulate button presses and timer events */
        if (iterations % 20 == 0) {
            printf("Simulating button press...\n");
            // mealy_machine_handle_event(led_fsm, LED_EVENT_BUTTON_PRESS);
        }
        
        if (iterations % 15 == 0) {
            printf("Simulating timer expired...\n");
            // mealy_machine_handle_event(led_fsm, LED_EVENT_TIMER_EXPIRED);
        }
        
        os_delay(100);
        iterations++;
    }
    
    /* Cleanup */
    mealy_machine_destroy(led_fsm);
    fsm_os_context_destroy(ctx);
    pal_os_adapter_deinit();
    
    printf("LED state machine example completed\n");
}

/* ==================== VIPER with PAL Integration Example ==================== */

static void viper_pal_integration_example(void)
{
    printf("\n=== VIPER with PAL Integration Example ===\n");
    
    /* Initialize PAL for Windows simulator */
    pal_os_adapter_config_t config = {
        .target_platform = PAL_PLATFORM_WINDOWS_SIMULATOR,
        .enable_gui = true,
        .enable_hardware_events = true,
        .event_queue_size = 10,
        .config_file = NULL
    };
    
    if (pal_os_adapter_init(&config) != OS_OK) {
        printf("Failed to initialize PAL OS adapter\n");
        return;
    }
    
    /* Create a simple VIPER application */
    printf("Creating VIPER application with PAL hardware abstraction...\n");
    
    /* In a real implementation:
     * 1. View would use PAL GUI for display
     * 2. Interactor would use PAL GPIO for hardware control
     * 3. Presenter would coordinate between View and Interactor
     * 4. Entity would represent hardware state
     * 5. Router would handle navigation (if applicable)
     */
    
    printf("VIPER components:\n");
    printf("  - View: PAL Windows GUI\n");
    printf("  - Interactor: PAL GPIO control\n");
    printf("  - Presenter: State machine coordination\n");
    printf("  - Entity: Hardware state representation\n");
    printf("  - Router: Event routing\n");
    
    /* Simulate application run */
    printf("Running VIPER application for 5 seconds...\n");
    
    for (int i = 0; i < 50; i++) {
        if (pal_os_adapter_process_events() < 0) {
            break; /* User requested exit */
        }
        
        /* Simulate hardware events */
        if (i % 10 == 0) {
            printf("  Processing hardware event %d...\n", i/10);
        }
        
        os_delay(100);
    }
    
    /* Cleanup */
    pal_os_adapter_deinit();
    
    printf("VIPER with PAL integration example completed\n");
}

/* ==================== Multi-Platform Demonstration ==================== */

static void multi_platform_demo(void)
{
    printf("\n=== Multi-Platform PAL OS Adapter Demo ===\n");
    
    /* Demonstrate how the same code can run on different platforms */
    pal_platform_t platforms[] = {
        PAL_PLATFORM_WINDOWS_SIMULATOR,
        PAL_PLATFORM_LINUX_SIMULATOR,
        PAL_PLATFORM_STM32,
        PAL_PLATFORM_ESP32
    };
    
    const char *platform_names[] = {
        "Windows Simulator",
        "Linux Simulator",
        "STM32 (ARM Cortex-M)",
        "ESP32 (Xtensa)"
    };
    
    for (int i = 0; i < 4; i++) {
        printf("\nPlatform: %s\n", platform_names[i]);
        
        /* Note: In a real scenario, you would compile for each platform separately
         * This is just a demonstration of the concept */
        pal_os_adapter_config_t config = {
            .target_platform = platforms[i],
            .enable_gui = (i < 2), /* GUI only for simulators */
            .enable_hardware_events = true,
            .event_queue_size = 10,
            .config_file = NULL
        };
        
        printf("  Configuration:\n");
        printf("    - Target: %s\n", platform_names[i]);
        printf("    - GUI: %s\n", config.enable_gui ? "Enabled" : "Disabled");
        printf("    - Hardware Events: %s\n", config.enable_hardware_events ? "Enabled" : "Disabled");
        
        /* Platform-specific notes */
        switch (platforms[i]) {
            case PAL_PLATFORM_WINDOWS_SIMULATOR:
                printf("    - Uses SDL2 for GUI visualization\n");
                printf("    - Simulates GPIO, PWM, ADC, etc.\n");
                break;
            case PAL_PLATFORM_LINUX_SIMULATOR:
                printf("    - Uses terminal or X11 for display\n");
                printf("    - Simulates embedded hardware behavior\n");
                break;
            case PAL_PLATFORM_STM32:
                printf("    - Direct hardware access via HAL\n");
                printf("    - Real-time performance\n");
                break;
            case PAL_PLATFORM_ESP32:
                printf("    - ESP-IDF framework integration\n");
                printf("    - WiFi/BLE capabilities\n");
                break;
            default:
                break;
        }
    }
    
    printf("\nKey Benefit: Same state machine and VIPER code runs on all platforms!\n");
}

/* ==================== Main Function ==================== */

int main(int argc, char *argv[])
{
    printf("========================================\n");
    printf("PAL OS Adapter Integration Demo\n");
    printf("========================================\n");
    printf("\nThis demo shows how Platform Abstraction Layer (PAL) can be\n");
    printf("integrated with State Machine Extended and VIPER architecture.\n");
    
    /* Run examples */
    led_state_machine_example();
    viper_pal_integration_example();
    multi_platform_demo();
    
    printf("\n========================================\n");
    printf("Demo completed successfully!\n");
    printf("========================================\n");
    
    return 0;
}
