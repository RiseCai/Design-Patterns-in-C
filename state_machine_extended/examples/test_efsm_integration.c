/**
 * test_efsm_integration.c  2025-12-09
 * 
 * Test integration of EFSM Protocol State Machine with System Coordinator.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/system_coordinator.h"
#include "../src/efsm_protocol.h"

/* Mock EFSM processor for testing */
struct efsm_protocol {
    struct efsm_processor processor;
    int initialized;
};

static struct efsm_protocol mock_efsm;

void mock_efsm_init(struct efsm_protocol *efsm) {
    if (!efsm) return;
    efsm_processor_init(&efsm->processor);
    efsm->initialized = 1;
    printf("Mock EFSM Protocol initialized\n");
}

void mock_efsm_dispatch_packet(struct efsm_protocol *efsm, void *packet) {
    if (!efsm || !efsm->initialized) return;
    efsm_processor_dispatch_packet(&efsm->processor, packet);
    printf("Mock EFSM Protocol dispatched packet\n");
}

void mock_efsm_tick(struct efsm_protocol *efsm) {
    if (!efsm || !efsm->initialized) return;
    efsm_processor_tick(&efsm->processor);
    printf("Mock EFSM Protocol tick\n");
}

int main(void) {
    printf("=== Testing EFSM Protocol Integration with System Coordinator ===\n");
    
    /* Create system coordinator */
    struct system_coordinator sys;
    system_coordinator_init(&sys);
    
    printf("System Coordinator initialized, current state: %s\n",
           system_coordinator_get_state_name(system_coordinator_get_state(&sys)));
    
    /* Initialize mock EFSM */
    mock_efsm_init(&mock_efsm);
    
    /* Attach EFSM to system coordinator */
    system_coordinator_set_efsm_protocol(&sys, (struct efsm_protocol *)&mock_efsm);
    printf("EFSM Protocol attached to System Coordinator\n");
    
    /* Test system events */
    printf("\n--- Testing system events ---\n");
    
    /* Start recording */
    printf("Sending REC_START event...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_REC_START, NULL);
    printf("Current state: %s\n", system_coordinator_get_state_name(system_coordinator_get_state(&sys)));
    
    /* Simulate EFSM packet processing during recording */
    printf("\nSimulating EFSM packet processing...\n");
    mock_efsm_dispatch_packet(&mock_efsm, (void *)0x1234);
    
    /* Stop recording */
    printf("\nSending REC_STOP event...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_REC_STOP, NULL);
    printf("Current state: %s\n", system_coordinator_get_state_name(system_coordinator_get_state(&sys)));
    
    /* Upload complete */
    printf("\nSending UPLOAD_COMPLETE event...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_UPLOAD_COMPLETE, NULL);
    printf("Current state: %s\n", system_coordinator_get_state_name(system_coordinator_get_state(&sys)));
    
    /* Test error handling */
    printf("\n--- Testing error handling ---\n");
    printf("Sending ERROR event...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_ERROR, NULL);
    printf("Current state: %s\n", system_coordinator_get_state_name(system_coordinator_get_state(&sys)));
    
    /* Reset from error */
    printf("\nSending RESET event...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_RESET, NULL);
    printf("Current state: %s\n", system_coordinator_get_state_name(system_coordinator_get_state(&sys)));
    
    printf("\n=== Test completed successfully ===\n");
    return 0;
}
