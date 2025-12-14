/**
 * test_system_coordinator_integration.c
 * 
 * Comprehensive integration test for System Coordinator FSM.
 * Tests typical TWS earphone usage scenarios.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "src/system_coordinator.h"
#include "src/mycommon.h"

/* Mock subsystem structures for testing */
struct recording_fsm {
    int recording;
    int paused;
};

struct comm_fsm {
    int connected;
    int uploading;
};

struct power_fsm {
    int battery_level;
    int charging;
};

struct audio_fsm {
    int playing;
    int volume;
};

struct efsm_protocol {
    int initialized;
};

struct ota_fsm {
    int updating;
    int progress;
};

/* Mock implementations */
static struct recording_fsm mock_recording = {0};
static struct comm_fsm mock_comm = {0};
static struct power_fsm mock_power = {80, 0}; /* 80% battery, not charging */
static struct audio_fsm mock_audio = {0, 50};
static struct efsm_protocol mock_efsm = {0};
static struct ota_fsm mock_ota = {0, 0};

void print_state_info(struct system_coordinator *sys) {
    enum system_state state = system_coordinator_get_state(sys);
    printf("  Current State: %s\n", system_coordinator_get_state_name(state));
    
    if (system_coordinator_get_error_code(sys) != 0) {
        printf("  Error: %s\n", system_coordinator_get_error_msg(sys));
    }
}

void test_scenario_1_normal_bootup(void) {
    printf("\n=== Scenario 1: Normal Bootup ===\n");
    
    struct system_coordinator sys;
    system_coordinator_init(&sys);
    
    printf("1. System initialized\n");
    print_state_info(&sys);
    
    /* Attach mock subsystems */
    system_coordinator_set_recording_fsm(&sys, (struct recording_fsm *)&mock_recording);
    system_coordinator_set_comm_fsm(&sys, (struct comm_fsm *)&mock_comm);
    system_coordinator_set_power_fsm(&sys, (struct power_fsm *)&mock_power);
    system_coordinator_set_audio_fsm(&sys, (struct audio_fsm *)&mock_audio);
    system_coordinator_set_efsm_protocol(&sys, (struct efsm_protocol *)&mock_efsm);
    system_coordinator_set_ota_fsm(&sys, (struct ota_fsm *)&mock_ota);
    
    printf("2. All subsystems attached\n");
    
    /* Power on event */
    printf("3. Sending POWER_ON event...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_POWER_ON, NULL);
    print_state_info(&sys);
    
    /* Bluetooth connected */
    printf("4. Sending BLUETOOTH_CONNECTED event...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_BLUETOOTH_CONNECTED, NULL);
    print_state_info(&sys);
    
    system_coordinator_destroy(&sys);
    printf("Scenario 1 completed.\n");
}

void test_scenario_2_recording_workflow(void) {
    printf("\n=== Scenario 2: Recording Workflow ===\n");
    
    struct system_coordinator sys;
    system_coordinator_init(&sys);
    
    /* Quick initialization */
    system_coordinator_dispatch_event(&sys, SYS_EVT_POWER_ON, NULL);
    system_coordinator_dispatch_event(&sys, SYS_EVT_BLUETOOTH_CONNECTED, NULL);
    
    printf("1. System in IDLE state\n");
    print_state_info(&sys);
    
    /* Start recording */
    printf("2. Sending REC_START event...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_REC_START, NULL);
    print_state_info(&sys);
    
    /* Network becomes available */
    printf("3. Sending NETWORK_CONNECTED event...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_NETWORK_CONNECTED, NULL);
    print_state_info(&sys);
    
    /* Start uploading while recording */
    printf("4. Sending UPLOAD_START event...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_UPLOAD_START, NULL);
    print_state_info(&sys);
    
    /* Upload complete */
    printf("5. Sending UPLOAD_COMPLETE event...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_UPLOAD_COMPLETE, NULL);
    print_state_info(&sys);
    
    /* Stop recording */
    printf("6. Sending REC_STOP event...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_REC_STOP, NULL);
    print_state_info(&sys);
    
    system_coordinator_destroy(&sys);
    printf("Scenario 2 completed.\n");
}

void test_scenario_3_error_recovery(void) {
    printf("\n=== Scenario 3: Error Recovery ===\n");
    
    struct system_coordinator sys;
    system_coordinator_init(&sys);
    
    /* Initialize to IDLE */
    system_coordinator_dispatch_event(&sys, SYS_EVT_POWER_ON, NULL);
    
    printf("1. System in IDLE state\n");
    print_state_info(&sys);
    
    /* Simulate an error */
    printf("2. Sending ERROR event...\n");
    system_coordinator_set_error(&sys, 1001, "Network timeout");
    system_coordinator_dispatch_event(&sys, SYS_EVT_ERROR, NULL);
    print_state_info(&sys);
    
    /* Try to reset */
    printf("3. Sending RESET event...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_RESET, NULL);
    print_state_info(&sys);
    
    system_coordinator_destroy(&sys);
    printf("Scenario 3 completed.\n");
}

void test_scenario_4_power_management(void) {
    printf("\n=== Scenario 4: Power Management ===\n");
    
    struct system_coordinator sys;
    system_coordinator_init(&sys);
    
    /* Initialize to IDLE */
    system_coordinator_dispatch_event(&sys, SYS_EVT_POWER_ON, NULL);
    
    printf("1. System in IDLE state\n");
    print_state_info(&sys);
    
    /* Low battery warning */
    printf("2. Sending LOW_BATTERY event...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_LOW_BATTERY, NULL);
    print_state_info(&sys);
    
    /* Enter sleep mode */
    printf("3. Sending POWER_OFF event...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_POWER_OFF, NULL);
    print_state_info(&sys);
    
    /* Wake up */
    printf("4. Sending POWER_ON event...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_POWER_ON, NULL);
    print_state_info(&sys);
    
    system_coordinator_destroy(&sys);
    printf("Scenario 4 completed.\n");
}

void test_scenario_5_ota_update(void) {
    printf("\n=== Scenario 5: OTA Update ===\n");
    
    struct system_coordinator sys;
    system_coordinator_init(&sys);
    
    /* Initialize to IDLE */
    system_coordinator_dispatch_event(&sys, SYS_EVT_POWER_ON, NULL);
    
    printf("1. System in IDLE state\n");
    print_state_info(&sys);
    
    /* Start OTA update */
    printf("2. Sending OTA_START event...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_OTA_START, NULL);
    print_state_info(&sys);
    
    /* Simulate OTA progress */
    printf("3. Sending OTA_PROGRESS event...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_OTA_PROGRESS, NULL);
    print_state_info(&sys);
    
    /* OTA complete */
    printf("4. Sending OTA_COMPLETE event...\n");
    system_coordinator_dispatch_event(&sys, SYS_EVT_OTA_COMPLETE, NULL);
    print_state_info(&sys);
    
    system_coordinator_destroy(&sys);
    printf("Scenario 5 completed.\n");
}

int main(void) {
    printf("========================================\n");
    printf("System Coordinator Integration Test Suite\n");
    printf("========================================\n");
    
    printf("\nTesting typical TWS earphone usage scenarios...\n");
    
    test_scenario_1_normal_bootup();
    test_scenario_2_recording_workflow();
    test_scenario_3_error_recovery();
    test_scenario_4_power_management();
    test_scenario_5_ota_update();
    
    printf("\n========================================\n");
    printf("All test scenarios completed successfully!\n");
    printf("========================================\n");
    
    return 0;
}
