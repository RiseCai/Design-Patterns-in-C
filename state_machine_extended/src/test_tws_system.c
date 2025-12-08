/**
 * test_tws_system.c  2025-12-05
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Auto generate for Design Patterns in C *
 * 
 * Test program for TWS Earphone System state machine combination.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "system_coordinator.h"

/* Mock subsystem structures for testing */
struct recording_fsm {
    int dummy;
};

struct comm_fsm {
    int dummy;
};

struct power_fsm {
    int dummy;
};

struct audio_fsm {
    int dummy;
};

/* Helper function to simulate user interaction */
static void simulate_recording_scenario(struct system_coordinator *sys) {
    printf("\n=== Scenario 1: Normal Recording Flow ===\n");
    
    printf("1. System starts in IDLE state\n");
    printf("   Current state: %s\n", system_coordinator_get_state_name(system_coordinator_get_state(sys)));
    
    printf("2. User presses record button (SYS_EVT_REC_START)\n");
    system_coordinator_dispatch_event(sys, SYS_EVT_REC_START, NULL);
    printf("   Current state: %s\n", system_coordinator_get_state_name(system_coordinator_get_state(sys)));
    
    printf("3. Recording in progress...\n");
    printf("   Current state: %s\n", system_coordinator_get_state_name(system_coordinator_get_state(sys)));
    
    printf("4. User presses stop button (SYS_EVT_REC_STOP)\n");
    system_coordinator_dispatch_event(sys, SYS_EVT_REC_STOP, NULL);
    printf("   Current state: %s\n", system_coordinator_get_state_name(system_coordinator_get_state(sys)));
    
    printf("5. Upload completes (SYS_EVT_UPLOAD_COMPLETE)\n");
    system_coordinator_dispatch_event(sys, SYS_EVT_UPLOAD_COMPLETE, NULL);
    printf("   Current state: %s\n", system_coordinator_get_state_name(system_coordinator_get_state(sys)));
}

static void simulate_error_scenario(struct system_coordinator *sys) {
    printf("\n=== Scenario 2: Error Recovery Flow ===\n");
    
    printf("1. System in IDLE state\n");
    printf("   Current state: %s\n", system_coordinator_get_state_name(system_coordinator_get_state(sys)));
    
    printf("2. Start recording\n");
    system_coordinator_dispatch_event(sys, SYS_EVT_REC_START, NULL);
    printf("   Current state: %s\n", system_coordinator_get_state_name(system_coordinator_get_state(sys)));
    
    printf("3. Error occurs during recording (SYS_EVT_ERROR)\n");
    system_coordinator_set_error(sys, 1001, "Recording buffer overflow");
    system_coordinator_dispatch_event(sys, SYS_EVT_ERROR, NULL);
    printf("   Current state: %s\n", system_coordinator_get_state_name(system_coordinator_get_state(sys)));
    printf("   Error message: %s\n", system_coordinator_get_error_msg(sys));
    
    printf("4. Reset system (SYS_EVT_RESET)\n");
    system_coordinator_dispatch_event(sys, SYS_EVT_RESET, NULL);
    printf("   Current state: %s\n", system_coordinator_get_state_name(system_coordinator_get_state(sys)));
}

static void simulate_power_scenario(struct system_coordinator *sys) {
    printf("\n=== Scenario 3: Power Management Flow ===\n");
    
    printf("1. System in IDLE state\n");
    printf("   Current state: %s\n", system_coordinator_get_state_name(system_coordinator_get_state(sys)));
    
    printf("2. Low battery warning (SYS_EVT_LOW_BATTERY)\n");
    system_coordinator_dispatch_event(sys, SYS_EVT_LOW_BATTERY, NULL);
    printf("   Current state: %s\n", system_coordinator_get_state_name(system_coordinator_get_state(sys)));
    
    printf("3. Power off (SYS_EVT_POWER_OFF)\n");
    system_coordinator_dispatch_event(sys, SYS_EVT_POWER_OFF, NULL);
    printf("   Current state: %s\n", system_coordinator_get_state_name(system_coordinator_get_state(sys)));
}

/* Test state machine transitions */
static void test_state_transitions(void) {
    printf("\n=== Testing State Transitions ===\n");
    
    struct system_coordinator sys;
    system_coordinator_init(&sys);
    
    printf("Initial state: %s\n", system_coordinator_get_state_name(system_coordinator_get_state(&sys)));
    
    /* Test all state transitions */
    enum system_state states[] = {
        SYS_INIT, SYS_IDLE, SYS_RECORDING, SYS_UPLOADING, 
        SYS_ERROR, SYS_SLEEP, SYS_SHUTDOWN
    };
    
    for (int i = 0; i < sizeof(states)/sizeof(states[0]); i++) {
        printf("State %d: %s\n", i, system_coordinator_get_state_name(states[i]));
    }
    
    system_coordinator_destroy(&sys);
}

/* Test event names */
static void test_event_names(void) {
    printf("\n=== Testing Event Names ===\n");
    
    enum system_event events[] = {
        SYS_EVT_POWER_ON,
        SYS_EVT_POWER_OFF,
        SYS_EVT_REC_START,
        SYS_EVT_REC_STOP,
        SYS_EVT_REC_PAUSE,
        SYS_EVT_REC_RESUME,
        SYS_EVT_UPLOAD_START,
        SYS_EVT_UPLOAD_COMPLETE,
        SYS_EVT_NETWORK_CONNECTED,
        SYS_EVT_NETWORK_DISCONNECTED,
        SYS_EVT_BLUETOOTH_CONNECTED,
        SYS_EVT_BLUETOOTH_DISCONNECTED,
        SYS_EVT_LOW_BATTERY,
        SYS_EVT_CHARGING,
        SYS_EVT_ERROR,
        SYS_EVT_RESET
    };
    
    for (int i = 0; i < sizeof(events)/sizeof(events[0]); i++) {
        printf("Event %d: %s\n", i, system_coordinator_get_event_name(events[i]));
    }
}

/* Main test function */
int main(void) {
    printf("========================================\n");
    printf("TWS Earphone System State Machine Test\n");
    printf("========================================\n");
    
    /* Initialize system coordinator */
    struct system_coordinator sys;
    system_coordinator_init(&sys);
    
    /* Run test scenarios */
    simulate_recording_scenario(&sys);
    simulate_error_scenario(&sys);
    simulate_power_scenario(&sys);
    
    /* Additional tests */
    test_state_transitions();
    test_event_names();
    
    /* Clean up */
    system_coordinator_destroy(&sys);
    
    printf("\n========================================\n");
    printf("Test completed successfully!\n");
    printf("========================================\n");
    
    return 0;
}
