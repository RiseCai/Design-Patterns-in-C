/**
 * test_ota_external_command.c
 * 
 * Test program to demonstrate external device sending OTA commands
 * to the System Coordinator FSM.
 * 
 * This simulates an external device (e.g., mobile app) sending
 * OTA control commands to the TWS earphone system.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/system_coordinator.h"
#include "../src/ota_fsm.h"

/* Helper to print system state */
static void print_system_state(struct system_coordinator *sys) {
    printf("System State: %s\n", 
           system_coordinator_get_state_name(system_coordinator_get_state(sys)));
}

/* Helper to print OTA state */
static void print_ota_state(struct ota_fsm *ota) {
    printf("OTA State: %s (Progress: %d%%)\n", 
           ota_fsm_get_state_name(ota_fsm_get_state(ota)),
           ota_fsm_get_overall_progress(ota));
}

int main(void) {
    printf("=== External Device OTA Command Test ===\n");
    
    /* Initialize system coordinator */
    struct system_coordinator sys;
    system_coordinator_init(&sys);
    print_system_state(&sys);
    
    /* Initialize OTA FSM */
    struct ota_fsm ota;
    ota_fsm_init(&ota);
    
    /* Link OTA FSM to system coordinator */
    system_coordinator_set_ota_fsm(&sys, &ota);
    
    printf("\n--- Scenario 1: External device sends OTA_START command ---\n");
    printf("External device: Sending OTA_START command...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_OTA_START, NULL);
    print_system_state(&sys);
    print_ota_state(&ota);
    
    /* Simulate OTA progress */
    printf("\n--- Scenario 2: External device sends OTA_PROGRESS updates ---\n");
    int progress = 30;
    printf("External device: Sending OTA_PROGRESS (%d%%)...\n", progress);
    system_coordinator_dispatch_event(&sys, SYS_EVT_OTA_PROGRESS, &progress);
    print_ota_state(&ota);
    
    progress = 70;
    printf("External device: Sending OTA_PROGRESS (%d%%)...\n", progress);
    system_coordinator_dispatch_event(&sys, SYS_EVT_OTA_PROGRESS, &progress);
    print_ota_state(&ota);
    
    printf("\n--- Scenario 3: External device sends OTA_COMPLETE ---\n");
    printf("External device: Sending OTA_COMPLETE command...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_OTA_COMPLETE, NULL);
    print_ota_state(&ota);
    
    printf("\n--- Scenario 4: External device sends OTA_CANCEL while in progress ---\n");
    /* First start another OTA */
    printf("Starting another OTA...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_OTA_START, NULL);
    print_ota_state(&ota);
    
    printf("External device: Sending OTA_CANCEL command...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_OTA_CANCEL, NULL);
    print_ota_state(&ota);
    
    printf("\n--- Scenario 5: External device sends OTA_ERROR ---\n");
    printf("External device: Sending OTA_ERROR command...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_OTA_ERROR, NULL);
    print_ota_state(&ota);
    
    printf("\n=== Test Complete ===\n");
    
    /* Cleanup */
    ota_fsm_destroy(&ota);
    system_coordinator_destroy(&sys);
    
    return 0;
}
