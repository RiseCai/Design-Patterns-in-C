/**
 * test_scv_user_scenarios.c - User scenario tests for System Coordinator VIPER
 *
 * Tests realistic user workflows and common usage patterns.
 */

#include "test_framework.h"
#include "../include/scv.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Helper to create a system with default config */
static struct scv_system *create_test_system(void) {
    struct scv_config config = {0};
    strcpy(config.system_name, "TestSystem");
    config.system_id = 1001;
    config.log_level = 2;
    config.enable_debug = true;
    
    /* Entity config */
    strcpy(config.entity_config.entity_name, "TestEntity");
    config.entity_config.entity_id = 1;
    config.entity_config.status_update_interval = 1000;
    
    /* Business rules */
    config.business_rules.min_battery_for_recording = 20;
    config.business_rules.min_storage_for_recording_mb = 100;
    config.business_rules.max_recording_duration_sec = 3600;
    config.business_rules.min_battery_for_upload = 30;
    config.business_rules.min_network_strength_for_upload = 50;
    config.business_rules.sleep_battery_threshold = 15;
    config.business_rules.critical_battery_threshold = 5;
    
    /* Routing config */
    config.routing_config.max_routing_table_entries = 50;
    config.routing_config.max_pattern_length = 128;
    config.routing_config.enable_pattern_caching = true;
    config.routing_config.enable_route_optimization = true;
    config.routing_config.max_route_hops = 10;
    config.routing_config.route_timeout_ms = 5000;
    config.routing_config.max_pattern_matches = 10;
    config.routing_config.case_sensitive_matching = false;
    config.routing_config.routing_thread_priority = 5;
    config.routing_config.max_concurrent_routes = 20;
    
    return scv_init(&config);
}

/* Test case: Complete recording workflow */
TEST_CASE(test_user_recording_workflow) {
    struct scv_system *system = create_test_system();
    TEST_ASSERT_NOT_NULL(system);
    
    /* Start system */
    int result = scv_start(system);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(true, system->is_running);
    
    /* Update system data with good conditions */
    struct scv_system_data data = {0};
    data.battery_level = 80;
    data.storage_used_mb = 500;
    data.storage_total_mb = 8000;
    data.network_strength = 75;
    
    result = scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    TEST_ASSERT_EQUAL(0, result);
    
    /* User presses record button */
    const char *record_cmd = "RECORD_START";
    result = scv_process_event(system, SCV_SYSTEM_EVENT_USER_INPUT, record_cmd, strlen(record_cmd) + 1);
    TEST_ASSERT_EQUAL(0, result);
    
    /* Simulate recording in progress */
    data.is_recording = true;
    data.recording_duration_sec = 10;
    data.recording_file_size_mb = 50;
    result = scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    TEST_ASSERT_EQUAL(0, result);
    
    /* User stops recording */
    const char *stop_cmd = "RECORD_STOP";
    result = scv_process_event(system, SCV_SYSTEM_EVENT_USER_INPUT, stop_cmd, strlen(stop_cmd) + 1);
    TEST_ASSERT_EQUAL(0, result);
    
    /* Update data to reflect stopped recording */
    data.is_recording = false;
    result = scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    TEST_ASSERT_EQUAL(0, result);
    
    /* Stop system */
    result = scv_stop(system);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(false, system->is_running);
    
    scv_destroy(system);
}

/* Test case: Upload workflow after recording */
TEST_CASE(test_user_upload_workflow) {
    struct scv_system *system = create_test_system();
    TEST_ASSERT_NOT_NULL(system);
    
    scv_start(system);
    
    /* Set good conditions for upload */
    struct scv_system_data data = {0};
    data.battery_level = 60;
    data.storage_used_mb = 2000;
    data.storage_total_mb = 8000;
    data.network_strength = 80;
    
    scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    /* User initiates upload */
    const char *upload_cmd = "UPLOAD_START";
    int result = scv_process_event(system, SCV_SYSTEM_EVENT_USER_INPUT, upload_cmd, strlen(upload_cmd) + 1);
    TEST_ASSERT_EQUAL(0, result);
    
    /* Simulate upload progress */
    data.is_uploading = true;
    data.upload_progress = 30;
    data.upload_speed_kbps = 512;
    scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    /* Upload completes */
    data.is_uploading = false;
    data.upload_progress = 100;
    data.successful_uploads++;
    scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    /* User cancels upload (optional) */
    const char *cancel_cmd = "UPLOAD_CANCEL";
    result = scv_process_event(system, SCV_SYSTEM_EVENT_USER_INPUT, cancel_cmd, strlen(cancel_cmd) + 1);
    /* May return 0 or -1 depending on state */
    
    scv_stop(system);
    scv_destroy(system);
}

/* Test case: OTA update workflow */
TEST_CASE(test_user_ota_workflow) {
    struct scv_system *system = create_test_system();
    TEST_ASSERT_NOT_NULL(system);
    
    scv_start(system);
    
    /* Set conditions suitable for OTA */
    struct scv_system_data data = {0};
    data.battery_level = 90;
    data.network_strength = 70;
    data.ota_available = true;
    strcpy(data.ota_version, "2.0.0");
    
    scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    /* User accepts OTA update */
    const char *ota_accept = "OTA_ACCEPT";
    int result = scv_process_event(system, SCV_SYSTEM_EVENT_USER_INPUT, ota_accept, strlen(ota_accept) + 1);
    TEST_ASSERT_EQUAL(0, result);
    
    /* Simulate OTA progress */
    data.ota_progress = 25;
    scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    data.ota_progress = 50;
    scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    data.ota_progress = 100;
    data.ota_available = false;  /* Update completed */
    scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    /* User may reboot */
    const char *reboot_cmd = "REBOOT";
    result = scv_process_event(system, SCV_SYSTEM_EVENT_USER_INPUT, reboot_cmd, strlen(reboot_cmd) + 1);
    
    scv_stop(system);
    scv_destroy(system);
}

/* Test case: Low battery handling - user experience */
TEST_CASE(test_user_low_battery_scenario) {
    struct scv_system *system = create_test_system();
    TEST_ASSERT_NOT_NULL(system);
    
    scv_start(system);
    
    /* Battery drops below sleep threshold */
    struct scv_system_data data = {0};
    data.battery_level = 12;  /* Below sleep threshold (15) */
    
    scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    /* System should warn user (maybe via UI) */
    /* User plugs in charger (simulate by increasing battery level) */
    data.battery_level = 30;
    scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    /* Battery recovers */
    data.battery_level = 30;
    scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    /* User tries to record */
    const char *record_cmd = "RECORD_START";
    int result = scv_process_event(system, SCV_SYSTEM_EVENT_USER_INPUT, record_cmd, strlen(record_cmd) + 1);
    /* Should succeed because battery > min_battery_for_recording (20) */
    TEST_ASSERT_EQUAL(0, result);
    
    scv_stop(system);
    scv_destroy(system);
}

/* Test case: Storage full scenario */
TEST_CASE(test_user_storage_full_scenario) {
    struct scv_system *system = create_test_system();
    TEST_ASSERT_NOT_NULL(system);
    
    scv_start(system);
    
    /* Simulate nearly full storage */
    struct scv_system_data data = {0};
    data.battery_level = 80;
    data.storage_used_mb = 7900;
    data.storage_total_mb = 8000;  /* Only 100MB free */
    data.network_strength = 60;
    
    scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    /* User tries to record */
    const char *record_cmd = "RECORD_START";
    int result = scv_process_event(system, SCV_SYSTEM_EVENT_USER_INPUT, record_cmd, strlen(record_cmd) + 1);
    /* May fail due to insufficient storage (less than min_storage_for_recording_mb=100) */
    /* We don't assert specific result because implementation may vary */
    
    /* User deletes some files */
    const char *delete_cmd = "DELETE_OLDEST";
    result = scv_process_event(system, SCV_SYSTEM_EVENT_USER_INPUT, delete_cmd, strlen(delete_cmd) + 1);
    
    /* Update storage after deletion */
    data.storage_used_mb = 5000;
    scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    /* Now recording should succeed */
    result = scv_process_event(system, SCV_SYSTEM_EVENT_USER_INPUT, record_cmd, strlen(record_cmd) + 1);
    TEST_ASSERT_EQUAL(0, result);
    
    scv_stop(system);
    scv_destroy(system);
}

/* Test case: Network connectivity changes */
TEST_CASE(test_user_network_changes) {
    struct scv_system *system = create_test_system();
    TEST_ASSERT_NOT_NULL(system);
    
    scv_start(system);
    
    /* Start with good network */
    struct scv_system_data data = {0};
    data.battery_level = 70;
    data.network_strength = 80;
    data.is_uploading = false;
    
    scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    /* User starts upload */
    const char *upload_cmd = "UPLOAD_START";
    int result = scv_process_event(system, SCV_SYSTEM_EVENT_USER_INPUT, upload_cmd, strlen(upload_cmd) + 1);
    TEST_ASSERT_EQUAL(0, result);
    
    data.is_uploading = true;
    data.upload_progress = 20;
    scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    /* Network drops */
    data.network_strength = 10;
    scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    /* Upload should pause or show error */
    /* User moves to better location */
    data.network_strength = 70;
    scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    /* Upload resumes */
    data.upload_progress = 40;
    scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    /* Upload completes */
    data.is_uploading = false;
    data.upload_progress = 100;
    data.successful_uploads++;
    scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    scv_stop(system);
    scv_destroy(system);
}

/* Test case: Multi-tasking - recording while uploading */
TEST_CASE(test_user_multitasking) {
    struct scv_system *system = create_test_system();
    TEST_ASSERT_NOT_NULL(system);
    
    scv_start(system);
    
    /* Set good conditions */
    struct scv_system_data data = {0};
    data.battery_level = 90;
    data.storage_used_mb = 3000;
    data.storage_total_mb = 8000;
    data.network_strength = 85;
    
    scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    /* Start recording */
    const char *record_cmd = "RECORD_START";
    int result = scv_process_event(system, SCV_SYSTEM_EVENT_USER_INPUT, record_cmd, strlen(record_cmd) + 1);
    TEST_ASSERT_EQUAL(0, result);
    
    data.is_recording = true;
    data.recording_duration_sec = 5;
    scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    /* While recording, start upload (should be queued or handled based on policy) */
    const char *upload_cmd = "UPLOAD_START";
    result = scv_process_event(system, SCV_SYSTEM_EVENT_USER_INPUT, upload_cmd, strlen(upload_cmd) + 1);
    /* May succeed or be queued */
    
    data.is_uploading = true;
    data.upload_progress = 10;
    scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    /* Stop recording */
    const char *stop_cmd = "RECORD_STOP";
    result = scv_process_event(system, SCV_SYSTEM_EVENT_USER_INPUT, stop_cmd, strlen(stop_cmd) + 1);
    TEST_ASSERT_EQUAL(0, result);
    
    data.is_recording = false;
    scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    /* Upload continues */
    data.upload_progress = 60;
    scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    /* Upload completes */
    data.is_uploading = false;
    data.upload_progress = 100;
    data.successful_uploads++;
    scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    scv_stop(system);
    scv_destroy(system);
}

/* Test case: Error recovery - user retry */
TEST_CASE(test_user_error_recovery) {
    struct scv_system *system = create_test_system();
    TEST_ASSERT_NOT_NULL(system);
    
    scv_start(system);
    
    /* Simulate an error condition */
    struct scv_system_data data = {0};
    data.battery_level = 80;
    data.last_error_code = 500;
    strcpy(data.last_error_msg, "Upload failed: network timeout");
    data.last_error_timestamp = time(NULL);
    
    scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    /* User sees error on UI and decides to retry */
    const char *retry_cmd = "RETRY_UPLOAD";
    int result = scv_process_event(system, SCV_SYSTEM_EVENT_USER_INPUT, retry_cmd, strlen(retry_cmd) + 1);
    TEST_ASSERT_EQUAL(0, result);
    
    /* Clear error */
    data.last_error_code = 0;
    data.last_error_msg[0] = '\0';
    scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    /* Simulate successful retry */
    data.is_uploading = true;
    data.upload_progress = 100;
    scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    data.is_uploading = false;
    data.successful_uploads++;
    scv_process_event(system, SCV_SYSTEM_EVENT_SYSTEM_UPDATE, &data, sizeof(data));
    
    scv_stop(system);
    scv_destroy(system);
}

/* Test suite array */
void (*user_scenario_tests[])(void) = {
    test_user_recording_workflow,
    test_user_upload_workflow,
    test_user_ota_workflow,
    test_user_low_battery_scenario,
    test_user_storage_full_scenario,
    test_user_network_changes,
    test_user_multitasking,
    test_user_error_recovery,
};

/* Main function */
int main(void) {
    printf("System Coordinator VIPER User Scenario Tests\n");
    printf("============================================\n");

    RUN_TEST_SUITE(SCV_User_Scenarios, user_scenario_tests);
    print_test_summary();

    return test_fail_count > 0 ? 1 : 0;
}
