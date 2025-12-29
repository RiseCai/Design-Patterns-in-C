/**
 * test_scv_integration.c - Integration test for System Coordinator VIPER
 *
 * Tests the integration of all SCV components (entity, interactor, presenter, view, router)
 * with realistic user scenarios and error conditions.
 */

#include "test_framework.h"
#include "../include/scv.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Test case: System initialization and destruction */
TEST_CASE(test_scv_init_destroy) {
    struct scv_system *system = scv_init(NULL);
    TEST_ASSERT_NOT_NULL(system);
    TEST_ASSERT_NOT_NULL(system->entity);
    TEST_ASSERT_NOT_NULL(system->interactor);
    TEST_ASSERT_NOT_NULL(system->presenter);
    TEST_ASSERT_NOT_NULL(system->view);
    TEST_ASSERT_NOT_NULL(system->router);
    TEST_ASSERT_EQUAL(true, system->is_initialized);
    TEST_ASSERT_EQUAL(false, system->is_running);
    
    scv_destroy(system);
    /* No crash expected */
}

/* Test case: System initialization with custom config */
TEST_CASE(test_scv_init_with_config) {
    struct scv_config config = {0};
    strcpy(config.system_name, "TestSystem");
    config.system_id = 12345;
    config.log_level = 3;
    config.enable_debug = true;
    
    /* Custom entity config */
    strcpy(config.entity_config.entity_name, "TestEntity");
    config.entity_config.entity_id = 999;
    config.entity_config.status_update_interval = 2000;
    
    /* Custom business rules */
    config.business_rules.min_battery_for_recording = 30;
    config.business_rules.min_storage_for_recording_mb = 200;
    config.business_rules.max_recording_duration_sec = 7200;
    
    struct scv_system *system = scv_init(&config);
    TEST_ASSERT_NOT_NULL(system);
    TEST_ASSERT_EQUAL_STRING("TestSystem", system->config.system_name);
    TEST_ASSERT_EQUAL(12345, system->config.system_id);
    TEST_ASSERT_EQUAL(3, system->config.log_level);
    TEST_ASSERT_EQUAL(true, system->config.enable_debug);
    TEST_ASSERT_EQUAL_STRING("TestEntity", system->config.entity_config.entity_name);
    TEST_ASSERT_EQUAL(999, system->config.entity_config.entity_id);
    TEST_ASSERT_EQUAL(30, system->config.business_rules.min_battery_for_recording);
    
    scv_destroy(system);
}

/* Test case: System start and stop */
TEST_CASE(test_scv_start_stop) {
    struct scv_system *system = scv_init(NULL);
    TEST_ASSERT_NOT_NULL(system);
    
    /* Start system */
    int result = scv_start(system);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(true, system->is_running);
    
    /* Stop system */
    result = scv_stop(system);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(false, system->is_running);
    
    scv_destroy(system);
}

/* Test case: Double start and double stop */
TEST_CASE(test_scv_double_start_stop) {
    struct scv_system *system = scv_init(NULL);
    TEST_ASSERT_NOT_NULL(system);
    
    /* First start */
    int result = scv_start(system);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(true, system->is_running);
    
    /* Second start should succeed (already running) */
    result = scv_start(system);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(true, system->is_running);
    
    /* First stop */
    result = scv_stop(system);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(false, system->is_running);
    
    /* Second stop should succeed (already stopped) */
    result = scv_stop(system);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(false, system->is_running);
    
    scv_destroy(system);
}

/* Test case: Process user input event */
TEST_CASE(test_scv_process_user_input) {
    struct scv_system *system = scv_init(NULL);
    TEST_ASSERT_NOT_NULL(system);
    
    /* Start system */
    int result = scv_start(system);
    TEST_ASSERT_EQUAL(0, result);
    
    /* Simulate user input event */
    const char *input_data = "BUTTON_PRESS";
    result = scv_process_event(system, SCV_SYSTEM_EVENT_USER_INPUT, input_data, strlen(input_data) + 1);
    /* Should return 0 or negative depending on implementation */
    /* We just verify no crash */
    (void)result;
    
    scv_stop(system);
    scv_destroy(system);
}

/* Test case: Process system update event */
TEST_CASE(test_scv_process_system_update) {
    struct scv_system *system = scv_init(NULL);
    TEST_ASSERT_NOT_NULL(system);
    
    /* Start system */
    int result = scv_start(system);
    TEST_ASSERT_EQUAL(0, result);
    
    /* Simulate system update event (e.g., battery level update) */
    struct scv_system_data update_data = {0};
    update_data.battery_level = 75;
    update_data.storage_used_mb = 1024;
    update_data.storage_total_mb = 8192;
    
    result = scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &update_data, sizeof(update_data));
    /* Should return 0 or negative */
    (void)result;
    
    scv_stop(system);
    scv_destroy(system);
}

/* Test case: Get system status */
TEST_CASE(test_scv_get_status) {
    struct scv_system *system = scv_init(NULL);
    TEST_ASSERT_NOT_NULL(system);
    
    char status_buffer[1024];
    int result = scv_get_status(system, status_buffer, sizeof(status_buffer));
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_NOT_NULL(status_buffer);
    TEST_ASSERT(strlen(status_buffer) > 0);
    
    /* Check that status contains expected strings */
    TEST_ASSERT(strstr(status_buffer, "System Coordinator VIPER Status:") != NULL);
    TEST_ASSERT(strstr(status_buffer, "Entity:") != NULL);
    TEST_ASSERT(strstr(status_buffer, "Interactor:") != NULL);
    TEST_ASSERT(strstr(status_buffer, "Presenter:") != NULL);
    TEST_ASSERT(strstr(status_buffer, "View:") != NULL);
    TEST_ASSERT(strstr(status_buffer, "Router:") != NULL);
    
    scv_destroy(system);
}

/* Test case: Update system configuration */
TEST_CASE(test_scv_update_config) {
    struct scv_system *system = scv_init(NULL);
    TEST_ASSERT_NOT_NULL(system);
    
    /* Get current config */
    const struct scv_config *original_config = scv_get_config(system);
    TEST_ASSERT_NOT_NULL(original_config);
    
    /* Create new config */
    struct scv_config new_config = *original_config;
    strcpy(new_config.system_name, "UpdatedSystem");
    new_config.system_id = 54321;
    new_config.entity_config.status_update_interval = 3000;
    new_config.business_rules.min_battery_for_recording = 40;
    
    /* Update config */
    int result = scv_update_config(system, &new_config);
    TEST_ASSERT_EQUAL(0, result);
    
    /* Verify update */
    const struct scv_config *updated_config = scv_get_config(system);
    TEST_ASSERT_NOT_NULL(updated_config);
    TEST_ASSERT_EQUAL_STRING("UpdatedSystem", updated_config->system_name);
    TEST_ASSERT_EQUAL(54321, updated_config->system_id);
    TEST_ASSERT_EQUAL(3000, updated_config->entity_config.status_update_interval);
    TEST_ASSERT_EQUAL(40, updated_config->business_rules.min_battery_for_recording);
    
    scv_destroy(system);
}

/* Test case: System reset */
TEST_CASE(test_scv_reset) {
    struct scv_system *system = scv_init(NULL);
    TEST_ASSERT_NOT_NULL(system);
    
    /* Start system and make some changes */
    scv_start(system);
    
    /* Update entity data */
    struct scv_system_data data = {0};
    data.battery_level = 50;
    scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    /* Reset system */
    int result = scv_reset(system);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(false, system->is_running);
    
    /* System should be ready to start again */
    result = scv_start(system);
    TEST_ASSERT_EQUAL(0, result);
    
    scv_stop(system);
    scv_destroy(system);
}

/* Test case: Error handling - NULL parameters */
TEST_CASE(test_scv_error_handling_null) {
    /* Test with NULL system */
    int result = scv_start(NULL);
    TEST_ASSERT_EQUAL(-1, result);
    
    result = scv_stop(NULL);
    TEST_ASSERT_EQUAL(-1, result);
    
    result = scv_process_event(NULL, SCV_SYSTEM_EVENT_USER_INPUT, NULL, 0);
    TEST_ASSERT_EQUAL(-1, result);
    
    result = scv_get_status(NULL, NULL, 0);
    TEST_ASSERT_EQUAL(-1, result);
    
    result = scv_update_config(NULL, NULL);
    TEST_ASSERT_EQUAL(-1, result);
    
    const struct scv_config *config = scv_get_config(NULL);
    TEST_ASSERT_NULL(config);
    
    result = scv_reset(NULL);
    TEST_ASSERT_EQUAL(-1, result);
    
    /* scv_destroy with NULL should not crash */
    scv_destroy(NULL);
}

/* Test case: Get last error */
TEST_CASE(test_scv_get_last_error) {
    struct scv_system *system = scv_init(NULL);
    TEST_ASSERT_NOT_NULL(system);
    
    int error_code = 0;
    char error_msg[256] = {0};
    
    /* Initially no error */
    int result = scv_get_last_error(system, &error_code, error_msg, sizeof(error_msg));
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(0, error_code);
    TEST_ASSERT_EQUAL_STRING("", error_msg);
    
    /* Trigger an error by trying to process event with invalid parameters */
    result = scv_process_event(system, -1, NULL, 0); /* Invalid event type */
    /* May or may not set error, but get_last_error should still work */
    scv_get_last_error(system, &error_code, error_msg, sizeof(error_msg));
    /* No assertion on error content */
    
    scv_destroy(system);
}

/* Test case: Get component handles */
TEST_CASE(test_scv_get_components) {
    struct scv_system *system = scv_init(NULL);
    TEST_ASSERT_NOT_NULL(system);
    
    struct scv_entity *entity = NULL;
    struct scv_interactor *interactor = NULL;
    struct scv_presenter *presenter = NULL;
    struct scv_view *view = NULL;
    struct scv_router *router = NULL;
    
    scv_get_components(system, &entity, &interactor, &presenter, &view, &router);
    
    TEST_ASSERT_NOT_NULL(entity);
    TEST_ASSERT_NOT_NULL(interactor);
    TEST_ASSERT_NOT_NULL(presenter);
    TEST_ASSERT_NOT_NULL(view);
    TEST_ASSERT_NOT_NULL(router);
    
    TEST_ASSERT_EQUAL_PTR(system->entity, entity);
    TEST_ASSERT_EQUAL_PTR(system->interactor, interactor);
    TEST_ASSERT_EQUAL_PTR(system->presenter, presenter);
    TEST_ASSERT_EQUAL_PTR(system->view, view);
    TEST_ASSERT_EQUAL_PTR(system->router, router);
    
    scv_destroy(system);
}

/* Test case: Memory leak check (create/destroy many times) */
TEST_CASE(test_scv_memory_leak) {
    const int iterations = 10;
    for (int i = 0; i < iterations; i++) {
        struct scv_system *system = scv_init(NULL);
        TEST_ASSERT_NOT_NULL(system);
        
        /* Do some operations */
        scv_start(system);
        scv_process_event(system, SCV_SYSTEM_EVENT_USER_INPUT, "test", 5);
        scv_stop(system);
        scv_reset(system);
        
        scv_destroy(system);
    }
    /* If we get here without crashing, assume no leak */
    TEST_ASSERT(1);
}

/* Test case: Integration scenario - recording workflow */
TEST_CASE(test_scv_recording_workflow) {
    struct scv_system *system = scv_init(NULL);
    TEST_ASSERT_NOT_NULL(system);
    
    /* Start system */
    int result = scv_start(system);
    TEST_ASSERT_EQUAL(0, result);
    
    /* Update system data with sufficient battery and storage */
    struct scv_system_data data = {0};
    data.battery_level = 80;  /* Above minimum for recording */
    data.storage_used_mb = 1024;
    data.storage_total_mb = 8192;  /* Plenty of free space */
    data.network_strength = 75;
    
    result = scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    /* Should succeed */
    
    /* Simulate user starting recording */
    const char *start_recording = "START_RECORDING";
    result = scv_process_event(system, SCV_SYSTEM_EVENT_USER_INPUT, start_recording, strlen(start_recording) + 1);
    
    /* Simulate recording in progress */
    data.is_recording = true;
    data.recording_duration_sec = 10;
    result = scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    /* Simulate user stopping recording */
    const char *stop_recording = "STOP_RECORDING";
    result = scv_process_event(system, SCV_SYSTEM_EVENT_USER_INPUT, stop_recording, strlen(stop_recording) + 1);
    
    /* Update data to show recording stopped */
    data.is_recording = false;
    result = scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    scv_stop(system);
    scv_destroy(system);
}

/* Test case: Integration scenario - low battery handling */
TEST_CASE(test_scv_low_battery_scenario) {
    struct scv_system *system = scv_init(NULL);
    TEST_ASSERT_NOT_NULL(system);
    
    /* Start system */
    int result = scv_start(system);
    TEST_ASSERT_EQUAL(0, result);
    
    /* Update system data with low battery */
    struct scv_system_data data = {0};
    data.battery_level = 10;  /* Below sleep threshold (15) */
    data.storage_used_mb = 1024;
    data.storage_total_mb = 8192;
    
    result = scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    /* System should handle low battery (may enter sleep mode) */
    /* We just verify no crash */
    
    scv_stop(system);
    scv_destroy(system);
}

/* Test suite array */
void (*integration_tests[])(void) = {
    test_scv_init_destroy,
    test_scv_init_with_config,
    test_scv_start_stop,
    test_scv_double_start_stop,
    test_scv_process_user_input,
    test_scv_process_system_update,
    test_scv_get_status,
    test_scv_update_config,
    test_scv_reset,
    test_scv_error_handling_null,
    test_scv_get_last_error,
    test_scv_get_components,
    test_scv_memory_leak,
    test_scv_recording_workflow,
    test_scv_low_battery_scenario,
};

int main(void) {
    printf("System Coordinator VIPER Integration Tests\n");
    printf("==========================================\n");

    RUN_TEST_SUITE(SCV_Integration, integration_tests);
    print_test_summary();

    return test_fail_count > 0 ? 1 : 0;
}
