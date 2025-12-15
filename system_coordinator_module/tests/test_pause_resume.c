/**
 * test_pause_resume.c
 * Test pause/resume functionality in system coordinator.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "src/system_coordinator.h"
#include "src/recording_fsm.h"

int main(void) {
    printf("Testing pause/resume integration between system coordinator and recording FSM\n");
    
    struct system_coordinator sys;
    system_coordinator_init(&sys);
    
    /* Create a recording FSM instance */
    struct recording_fsm rec_fsm;
    recording_fsm_init(&rec_fsm);
    
    /* Link recording FSM to system coordinator */
    system_coordinator_set_recording_fsm(&sys, &rec_fsm);
    
    /* Start recording */
    printf("1. Starting recording...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_REC_START, NULL);
    printf("   System state: %s\n", system_coordinator_get_state_name(system_coordinator_get_state(&sys)));
    printf("   Recording FSM state: %s\n", recording_fsm_get_state_name(recording_fsm_get_state(&rec_fsm)));
    
    /* Simulate device ready to move recording FSM to REC_RECORDING state */
    printf("   Simulating device ready...\n");
    recording_fsm_set_earphone_ready(&rec_fsm, 1);
    recording_fsm_set_case_ready(&rec_fsm, 1);
    /* This should trigger REC_EVT_SYNC_COMPLETE internally */
    
    /* Pause recording */
    printf("2. Pausing recording...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_REC_PAUSE, NULL);
    printf("   System state: %s\n", system_coordinator_get_state_name(system_coordinator_get_state(&sys)));
    printf("   Recording FSM state: %s\n", recording_fsm_get_state_name(recording_fsm_get_state(&rec_fsm)));
    
    /* Resume recording */
    printf("3. Resuming recording...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_REC_RESUME, NULL);
    printf("   System state: %s\n", system_coordinator_get_state_name(system_coordinator_get_state(&sys)));
    printf("   Recording FSM state: %s\n", recording_fsm_get_state_name(recording_fsm_get_state(&rec_fsm)));
    
    /* Stop recording */
    printf("4. Stopping recording...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_REC_STOP, NULL);
    printf("   System state: %s\n", system_coordinator_get_state_name(system_coordinator_get_state(&sys)));
    printf("   Recording FSM state: %s\n", recording_fsm_get_state_name(recording_fsm_get_state(&rec_fsm)));
    
    /* Clean up */
    recording_fsm_destroy(&rec_fsm);
    system_coordinator_destroy(&sys);
    
    printf("\nTest completed.\n");
    return 0;
}
