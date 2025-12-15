/**
 * test_recording_fsm.c  2025-12-09
 * 
 * Test program for Recording FSM.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/mycommon.h"
#include "../src/mytrace.h"
#include "../src/recording_fsm.h"

int main(int argc, char *argv[])
{
    struct recording_fsm rec;
    
    printf("=== Recording FSM Test ===\n");
    
    /* Initialize FSM */
    recording_fsm_init(&rec);
    printf("Initial state: %s\n", recording_fsm_get_state_name(recording_fsm_get_state(&rec)));
    
    /* Test 1: Start recording */
    printf("\n--- Test 1: Start recording ---\n");
    recording_fsm_dispatch_event(&rec, REC_EVT_START, NULL);
    printf("State after START: %s\n", recording_fsm_get_state_name(recording_fsm_get_state(&rec)));
    
    /* Set devices ready */
    recording_fsm_set_earphone_ready(&rec, 1);
    recording_fsm_set_case_ready(&rec, 1);
    
    /* Process events */
    for (int i = 0; i < 3; i++) {
        recording_fsm_dispatch_event(&rec, REC_EVT_SYNC_COMPLETE, NULL);
        printf("State after sync: %s\n", recording_fsm_get_state_name(recording_fsm_get_state(&rec)));
        if (recording_fsm_is_recording(&rec)) {
            printf("Recording duration: %lu ms, size: %lu bytes\n", 
                   recording_fsm_get_duration(&rec), recording_fsm_get_file_size(&rec));
        }
    }
    
    /* Test 2: Pause and resume */
    printf("\n--- Test 2: Pause and resume ---\n");
    recording_fsm_dispatch_event(&rec, REC_EVT_PAUSE, NULL);
    printf("State after PAUSE: %s\n", recording_fsm_get_state_name(recording_fsm_get_state(&rec)));
    
    recording_fsm_dispatch_event(&rec, REC_EVT_RESUME, NULL);
    printf("State after RESUME: %s\n", recording_fsm_get_state_name(recording_fsm_get_state(&rec)));
    
    /* Test 3: Stop recording */
    printf("\n--- Test 3: Stop recording ---\n");
    recording_fsm_dispatch_event(&rec, REC_EVT_STOP, NULL);
    printf("State after STOP: %s\n", recording_fsm_get_state_name(recording_fsm_get_state(&rec)));
    
    /* Test 4: Error handling */
    printf("\n--- Test 4: Error handling ---\n");
    recording_fsm_set_error(&rec, 1001, "Storage full");
    printf("State after ERROR: %s\n", recording_fsm_get_state_name(recording_fsm_get_state(&rec)));
    printf("Error code: %d, message: %s\n", 
           recording_fsm_get_error_code(&rec), recording_fsm_get_error_msg(&rec));
    
    /* Test 5: Recover from error */
    printf("\n--- Test 5: Recover from error ---\n");
    recording_fsm_dispatch_event(&rec, REC_EVT_STOP, NULL);
    printf("State after STOP from error: %s\n", recording_fsm_get_state_name(recording_fsm_get_state(&rec)));
    
    /* Test 6: Configuration */
    printf("\n--- Test 6: Configuration ---\n");
    recording_fsm_set_source(&rec, REC_SOURCE_BOTH);
    recording_fsm_set_sample_rate(&rec, 48000);
    recording_fsm_set_bit_depth(&rec, 24);
    recording_fsm_set_channels(&rec, 2);
    
    printf("Configuration set: source=BOTH, sample_rate=48000, bit_depth=24, channels=2\n");
    
    /* Clean up */
    recording_fsm_destroy(&rec);
    
    printf("\n=== Test completed ===\n");
    return 0;
}
