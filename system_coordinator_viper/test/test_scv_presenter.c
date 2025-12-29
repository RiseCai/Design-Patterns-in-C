/**
 * test_scv_presenter.c - Unit tests for System Coordinator VIPER Presenter component
 *
 * Tests initialization, destruction, subsystem coordination, and event processing.
 */

#include "test_framework.h"
#include "../include/scv_presenter.h"
#include "../include/scv_entity.h"
#include "../include/scv_interactor.h"
#include <string.h>
#include <stdlib.h>

/* Dummy callbacks for testing */
static void dummy_subsystem_status_changed_callback(const char *subsystem_name,
                                                    scv_subsystem_status_t old_status,
                                                    scv_subsystem_status_t new_status,
                                                    void *user_data) {
    (*(int *)user_data)++;
}

static void dummy_coordination_event_callback(const char *event_name,
                                              const void *event_data,
                                              void *user_data) {
    (*(int *)user_data)++;
}

static void dummy_error_callback(const char *subsystem_name,
                                 int error_code,
                                 const char *error_msg,
                                 void *user_data) {
    (*(int *)user_data)++;
}

/* Mock entity for testing */
static struct scv_entity mock_entity = {
    .efsm = NULL,
    .data = {
        .device_id = "TEST_DEVICE",
        .firmware_version = "1.0.0",
        .hardware_version = "HW1.0",
        .battery_level = 80,
        .storage_used_mb = 100,
        .storage_total_mb = 1000,
        .network_strength = 75,
        .is_recording = false,
        .recording_duration_sec = 0,
        .recording_file_size_mb = 0,
        .is_uploading = false,
        .upload_progress = 0,
        .upload_speed_kbps = 0,
        .ota_available = false,
        .ota_version = "",
        .ota_progress = 0,
        .last_error_code = 0,
        .last_error_msg = "",
        .last_error_timestamp = 0,
        .uptime_seconds = 0,
        .total_recordings = 0,
        .successful_uploads = 0,
        .failed_uploads = 0
    },
    .config = {
        .entity_name = "MockEntity",
        .entity_id = 1,
        .status_update_interval = 1000,
        .metrics_update_interval = 5000,
        .max_storage_mb = 1000,
        .max_recording_duration_sec = 3600,
        .wifi_ssid = "",
        .wifi_password = "",
        .server_url = "",
        .ota_server_url = "",
        .ota_check_interval_sec = 86400
    },
    .data_updated_callback = NULL,
    .error_callback = NULL,
    .user_data = NULL
};

/* Mock interactor for testing */
static struct scv_interactor mock_interactor = {
    .moore_fsm = NULL,
    .rules = {
        .min_battery_for_recording = 20,
        .min_storage_for_recording_mb = 50,
        .max_recording_duration_sec = 3600,
        .min_battery_for_upload = 30,
        .min_network_strength_for_upload = 50,
        .upload_only_on_wifi = true,
        .max_upload_retries = 3,
        .min_battery_for_ota = 50,
        .ota_only_on_wifi = true,
        .ota_only_when_charging = false,
        .sleep_battery_threshold = 10,
        .critical_battery_threshold = 5,
        .max_consecutive_errors = 5,
        .error_recovery_delay_ms = 1000
    },
    .context = {
        .current_state = INTERACTOR_STATE_IDLE,
        .current_substate.recording_substate = RECORDING_SUBSTATE_STARTING,
        .can_start_recording = true,
        .can_start_upload = true,
        .can_start_ota = true,
        .has_network_connectivity = true,
        .has_sufficient_battery = true,
        .has_sufficient_storage = true,
        .is_charging = false,
        .recording_attempts = 0,
        .upload_attempts = 0,
        .ota_attempts = 0,
        .consecutive_errors = 0,
        .last_recording_start = 0,
        .last_upload_start = 0,
        .last_ota_check = 0
    },
    .entity = &mock_entity,
    .state_changed_callback = NULL,
    .decision_made_callback = NULL,
    .action_required_callback = NULL,
    .user_data = NULL
};

/* Test case: initialization and destruction */
TEST_CASE(test_presenter_init_destroy)
{
    struct scv_coordination_policy policy = {
        .mode = COORDINATION_MODE_PARALLEL,
        .recording_priority = 80,
        .communication_priority = 70,
        .power_priority = 90,
        .audio_priority = 60,
        .ota_priority = 50,
        .max_cpu_usage_percent = 80,
        .max_memory_usage_kb = 1024,
        .max_power_consumption_mw = 5000,
        .max_activation_delay_ms = 1000,
        .max_synchronization_delay_ms = 500,
        .stop_all_on_subsystem_error = true,
        .error_recovery_attempts = 3,
        .error_recovery_delay_ms = 1000
    };

    struct scv_presenter *presenter = scv_presenter_init(&mock_entity, &mock_interactor, &policy);
    TEST_ASSERT_NOT_NULL(presenter);
    TEST_ASSERT_EQUAL_PTR(&mock_entity, presenter->entity);
    TEST_ASSERT_EQUAL_PTR(&mock_interactor, presenter->interactor);
    TEST_ASSERT_EQUAL(COORDINATION_MODE_PARALLEL, presenter->policy.mode);
    TEST_ASSERT_EQUAL(80, presenter->policy.recording_priority);
    TEST_ASSERT_EQUAL(PRESENTER_STATE_INIT, scv_presenter_get_state(presenter));

    scv_presenter_destroy(presenter);
    /* No crash expected */
}

/* Test case: initialization with NULL parameters */
TEST_CASE(test_presenter_init_null_params)
{
    /* Test with all NULL */
    struct scv_presenter *presenter = scv_presenter_init(NULL, NULL, NULL);
    TEST_ASSERT_NOT_NULL(presenter);
    TEST_ASSERT_NULL(presenter->entity);
    TEST_ASSERT_NULL(presenter->interactor);
    TEST_ASSERT_EQUAL(COORDINATION_MODE_PARALLEL, presenter->policy.mode); /* default */
    scv_presenter_destroy(presenter);
}

/* Test case: register and unregister subsystem */
TEST_CASE(test_presenter_register_subsystem)
{
    struct scv_presenter *presenter = scv_presenter_init(&mock_entity, &mock_interactor, NULL);
    TEST_ASSERT_NOT_NULL(presenter);

    /* Register a dummy subsystem */
    void *dummy_fsm = (void *)0x1234;
    int ret = scv_presenter_register_subsystem(presenter, "recording", dummy_fsm);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL_PTR(dummy_fsm, presenter->recording_subsystem.fsm_handle);
    TEST_ASSERT_EQUAL_STRING("recording", presenter->recording_subsystem.subsystem_name);

    /* Unregister */
    ret = scv_presenter_unregister_subsystem(presenter, "recording");
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_NULL(presenter->recording_subsystem.fsm_handle);

    scv_presenter_destroy(presenter);
}

/* Test case: get subsystem status */
TEST_CASE(test_presenter_get_subsystem_status)
{
    struct scv_presenter *presenter = scv_presenter_init(&mock_entity, &mock_interactor, NULL);
    TEST_ASSERT_NOT_NULL(presenter);

    /* Before registration, status should be UNINITIALIZED */
    scv_subsystem_status_t status = scv_presenter_get_subsystem_status(presenter, "recording");
    TEST_ASSERT_EQUAL(SUBSYSTEM_STATUS_UNINITIALIZED, status);

    /* Register and check default status */
    void *dummy_fsm = (void *)0x1234;
    scv_presenter_register_subsystem(presenter, "recording", dummy_fsm);
    status = scv_presenter_get_subsystem_status(presenter, "recording");
    TEST_ASSERT_EQUAL(SUBSYSTEM_STATUS_INITIALIZING, status); /* default after registration */

    scv_presenter_destroy(presenter);
}

/* Test case: update policy */
TEST_CASE(test_presenter_update_policy)
{
    struct scv_presenter *presenter = scv_presenter_init(&mock_entity, &mock_interactor, NULL);
    TEST_ASSERT_NOT_NULL(presenter);

    struct scv_coordination_policy new_policy = {
        .mode = COORDINATION_MODE_SEQUENTIAL,
        .recording_priority = 90,
        .communication_priority = 80,
        .power_priority = 95,
        .audio_priority = 70,
        .ota_priority = 60,
        .max_cpu_usage_percent = 90,
        .max_memory_usage_kb = 2048,
        .max_power_consumption_mw = 6000,
        .max_activation_delay_ms = 2000,
        .max_synchronization_delay_ms = 1000,
        .stop_all_on_subsystem_error = false,
        .error_recovery_attempts = 5,
        .error_recovery_delay_ms = 2000
    };

    int ret = scv_presenter_update_policy(presenter, &new_policy);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(COORDINATION_MODE_SEQUENTIAL, presenter->policy.mode);
    TEST_ASSERT_EQUAL(90, presenter->policy.recording_priority);
    TEST_ASSERT_EQUAL(false, presenter->policy.stop_all_on_subsystem_error);

    /* Get policy and verify */
    const struct scv_coordination_policy *retrieved = scv_presenter_get_policy(presenter);
    TEST_ASSERT_NOT_NULL(retrieved);
    TEST_ASSERT_EQUAL(new_policy.mode, retrieved->mode);

    scv_presenter_destroy(presenter);
}

/* Test case: set callbacks */
TEST_CASE(test_presenter_set_callbacks)
{
    struct scv_presenter *presenter = scv_presenter_init(&mock_entity, &mock_interactor, NULL);
    TEST_ASSERT_NOT_NULL(presenter);

    int callback1_called = 0;
    int callback2_called = 0;
    int callback3_called = 0;

    /* Set callbacks - note that user_data is shared, so the last set user_data will be used for all callbacks.
       We'll set the first callback last to ensure its user_data is active. */
    scv_presenter_set_coordination_event_callback(presenter,
                                                  dummy_coordination_event_callback,
                                                  &callback2_called);
    scv_presenter_set_error_callback(presenter,
                                     dummy_error_callback,
                                     &callback3_called);
    scv_presenter_set_subsystem_status_changed_callback(presenter,
                                                        dummy_subsystem_status_changed_callback,
                                                        &callback1_called);

    /* Simulate a status change (this would normally be triggered internally) */
    if (presenter->subsystem_status_changed_callback) {
        presenter->subsystem_status_changed_callback("recording",
                                                     SUBSYSTEM_STATUS_UNINITIALIZED,
                                                     SUBSYSTEM_STATUS_READY,
                                                     presenter->user_data);
    }
    TEST_ASSERT_EQUAL(1, callback1_called);

    scv_presenter_destroy(presenter);
}

/* Test case: coordinate subsystems */
TEST_CASE(test_presenter_coordinate)
{
    struct scv_presenter *presenter = scv_presenter_init(&mock_entity, &mock_interactor, NULL);
    TEST_ASSERT_NOT_NULL(presenter);

    /* Register dummy subsystems */
    scv_presenter_register_subsystem(presenter, "recording", (void *)0x1000);
    scv_presenter_register_subsystem(presenter, "communication", (void *)0x2000);
    scv_presenter_register_subsystem(presenter, "power", (void *)0x3000);
    scv_presenter_register_subsystem(presenter, "audio", (void *)0x4000);
    scv_presenter_register_subsystem(presenter, "ota", (void *)0x5000);

    /* Coordinate - should succeed */
    int ret = scv_presenter_coordinate(presenter);
    TEST_ASSERT_EQUAL(0, ret);

    scv_presenter_destroy(presenter);
}

/* Test case: synchronize */
TEST_CASE(test_presenter_synchronize)
{
    struct scv_presenter *presenter = scv_presenter_init(&mock_entity, &mock_interactor, NULL);
    TEST_ASSERT_NOT_NULL(presenter);

    /* Synchronize with target level 75% */
    uint32_t achieved = scv_presenter_synchronize(presenter, 75);
    /* Since we have no real FSMs, synchronization may return 0 or some default */
    /* Just ensure no crash */
    (void)achieved;

    scv_presenter_destroy(presenter);
}

/* Test case: process event */
TEST_CASE(test_presenter_process_event)
{
    struct scv_presenter *presenter = scv_presenter_init(&mock_entity, &mock_interactor, NULL);
    TEST_ASSERT_NOT_NULL(presenter);

    /* Process a dummy event */
    int ret = scv_presenter_process_event(presenter, 1, NULL);
    /* Should return 0 (success) or negative (if event not handled) */
    /* We just verify no crash */
    (void)ret;

    scv_presenter_destroy(presenter);
}

/* Test case: reset presenter */
TEST_CASE(test_presenter_reset)
{
    struct scv_presenter *presenter = scv_presenter_init(&mock_entity, &mock_interactor, NULL);
    TEST_ASSERT_NOT_NULL(presenter);

    /* Change state from INIT to something else by processing an event */
    scv_presenter_process_event(presenter, 1, NULL);
    
    /* Reset */
    int ret = scv_presenter_reset(presenter);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(PRESENTER_STATE_INIT, scv_presenter_get_state(presenter));

    scv_presenter_destroy(presenter);
}

/* Test suite runner */
int main(void)
{
    test_pass_count = 0;
    test_fail_count = 0;

    void (*test_cases[])(void) = {
        test_presenter_init_destroy,
        test_presenter_init_null_params,
        test_presenter_register_subsystem,
        test_presenter_get_subsystem_status,
        test_presenter_update_policy,
        test_presenter_set_callbacks,
        test_presenter_coordinate,
        test_presenter_synchronize,
        test_presenter_process_event,
        test_presenter_reset,
    };

    RUN_TEST_SUITE(SCV_Presenter, test_cases);
    print_test_summary();

    return test_fail_count == 0 ? 0 : 1;
}
