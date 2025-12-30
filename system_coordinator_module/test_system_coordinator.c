#include <stdio.h>
#include <stdlib.h>
#include "src/system_coordinator.h"

/* Dummy subsystem structures for testing */
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

struct efsm_protocol {
    int dummy;
};

struct ota_fsm {
    int dummy;
};

int main() {
    struct system_coordinator sys;
    struct recording_fsm rec_fsm;
    struct ota_fsm ota_fsm;
    
    printf("Testing System Coordinator API...\n");
    
    /* Set up dummy subsystems */
    system_coordinator_set_recording_fsm(&sys, &rec_fsm);
    system_coordinator_set_ota_fsm(&sys, &ota_fsm);
    
    /* Initialize system coordinator */
    system_coordinator_init(&sys);
    
    /* Check initial state (should be INIT or IDLE) */
    enum system_state state = system_coordinator_get_state(&sys);
    printf("Initial state: %s\n", system_coordinator_get_state_name(state));
    
    /* Send POWER_OFF event (should transition to SHUTDOWN) */
    printf("Sending POWER_OFF event...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_POWER_OFF, NULL);
    state = system_coordinator_get_state(&sys);
    printf("State after POWER_OFF: %s\n", system_coordinator_get_state_name(state));
    
    /* Send POWER_ON event (should transition to IDLE) */
    printf("Sending POWER_ON event...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_POWER_ON, NULL);
    state = system_coordinator_get_state(&sys);
    printf("State after POWER_ON: %s\n", system_coordinator_get_state_name(state));
    
    /* Send REC_START event (should transition to RECORDING) */
    printf("Sending REC_START event...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_REC_START, NULL);
    state = system_coordinator_get_state(&sys);
    printf("State after REC_START: %s\n", system_coordinator_get_state_name(state));
    
    /* Send REC_STOP event (should transition to UPLOADING) */
    printf("Sending REC_STOP event...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_REC_STOP, NULL);
    state = system_coordinator_get_state(&sys);
    printf("State after REC_STOP: %s\n", system_coordinator_get_state_name(state));
    
    /* Send UPLOAD_COMPLETE event (should transition to IDLE) */
    printf("Sending UPLOAD_COMPLETE event...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_UPLOAD_COMPLETE, NULL);
    state = system_coordinator_get_state(&sys);
    printf("State after UPLOAD_COMPLETE: %s\n", system_coordinator_get_state_name(state));
    
    /* Test error handling */
    printf("Setting error...\n");
    system_coordinator_set_error(&sys, -1, "Test error");
    printf("Error code: %d, message: %s\n", 
           system_coordinator_get_error_code(&sys),
           system_coordinator_get_error_msg(&sys));
    
    /* Send ERROR event (should transition to ERROR state) */
    printf("Sending ERROR event...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_ERROR, NULL);
    state = system_coordinator_get_state(&sys);
    printf("State after ERROR: %s\n", system_coordinator_get_state_name(state));
    
    /* Send RESET event (should transition to RESET then INIT) */
    printf("Sending RESET event...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_RESET, NULL);
    state = system_coordinator_get_state(&sys);
    printf("State after RESET: %s\n", system_coordinator_get_state_name(state));
    
    printf("Test completed.\n");
    return 0;
}
