#include <stdio.h>
#include <stdlib.h>
#include "include/scv_presenter.h"
#include "include/scv_entity.h"
#include "include/scv_interactor.h"

int main() {
    printf("Testing scv_presenter state and subsystem status...\n");
    
    // Create mock entity and interactor (just pointers)
    struct scv_entity *entity = NULL;
    struct scv_interactor *interactor = NULL;
    
    // Initialize presenter with default policy
    struct scv_presenter *presenter = scv_presenter_init(entity, interactor, NULL);
    if (!presenter) {
        printf("FAIL: presenter initialization failed\n");
        return 1;
    }
    
    // Check presenter state
    scv_presenter_state_t state = scv_presenter_get_state(presenter);
    if (state != PRESENTER_STATE_INIT) {
        printf("FAIL: expected PRESENTER_STATE_INIT, got %d\n", state);
        scv_presenter_destroy(presenter);
        return 1;
    }
    printf("PASS: presenter state is PRESENTER_STATE_INIT\n");
    
    // Check subsystem status before registration
    scv_subsystem_status_t status = scv_presenter_get_subsystem_status(presenter, "recording");
    if (status != SUBSYSTEM_STATUS_UNINITIALIZED) {
        printf("FAIL: recording subsystem status expected UNINITIALIZED, got %d\n", status);
        scv_presenter_destroy(presenter);
        return 1;
    }
    printf("PASS: recording subsystem status is UNINITIALIZED before registration\n");
    
    // Register a dummy subsystem
    void *dummy_fsm = (void*)0x1234;
    int ret = scv_presenter_register_subsystem(presenter, "recording", dummy_fsm);
    if (ret != 0) {
        printf("FAIL: register_subsystem returned %d\n", ret);
        scv_presenter_destroy(presenter);
        return 1;
    }
    printf("PASS: register_subsystem succeeded\n");
    
    // Check subsystem status after registration (should be INITIALIZING)
    status = scv_presenter_get_subsystem_status(presenter, "recording");
    if (status != SUBSYSTEM_STATUS_INITIALIZING) {
        printf("FAIL: recording subsystem status expected INITIALIZING, got %d\n", status);
        scv_presenter_destroy(presenter);
        return 1;
    }
    printf("PASS: recording subsystem status is INITIALIZING after registration\n");
    
    // Verify that the subsystem info is updated
    const struct scv_subsystem_info *info = scv_presenter_get_subsystem_info(presenter, "recording");
    if (!info) {
        printf("FAIL: get_subsystem_info returned NULL\n");
        scv_presenter_destroy(presenter);
        return 1;
    }
    if (info->fsm_handle != dummy_fsm) {
        printf("FAIL: fsm_handle mismatch\n");
        scv_presenter_destroy(presenter);
        return 1;
    }
    printf("PASS: subsystem info correctly updated\n");
    
    // Clean up
    scv_presenter_destroy(presenter);
    printf("All tests passed.\n");
    return 0;
}
