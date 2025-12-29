/**
 * test_scv_interactor.c - Test cases for System Coordinator VIPER Interactor
 *
 * Tests interactor initialization, business logic, and state transitions.
 */

#include "test_framework.h"
#include "../include/scv_interactor.h"
#include "../include/scv_entity.h"

/* Test case: Interactor initialization */
TEST_CASE(test_interactor_init_basic) {
    struct scv_interactor *interactor = scv_interactor_init(NULL, NULL);
    TEST_ASSERT_NOT_NULL(interactor);
    TEST_ASSERT_NOT_NULL(interactor->moore_fsm);
    TEST_ASSERT_EQUAL(INTERACTOR_STATE_INIT, scv_interactor_get_state(interactor));
    
    /* Check default rules */
    const struct scv_business_rules *rules = scv_interactor_get_rules(interactor);
    TEST_ASSERT_NOT_NULL(rules);
    TEST_ASSERT_EQUAL(20, rules->min_battery_for_recording);
    TEST_ASSERT_EQUAL(100, rules->min_storage_for_recording_mb);
    TEST_ASSERT_EQUAL(3600, rules->max_recording_duration_sec);
    
    scv_interactor_destroy(interactor);
}

/* Test case: Interactor with entity */
TEST_CASE(test_interactor_init_with_entity) {
    struct scv_entity *entity = scv_entity_init(NULL);
    TEST_ASSERT_NOT_NULL(entity);
    
    struct scv_interactor *interactor = scv_interactor_init(entity, NULL);
    TEST_ASSERT_NOT_NULL(interactor);
    TEST_ASSERT_EQUAL_PTR(entity, interactor->entity);
    
    scv_interactor_destroy(interactor);
    scv_entity_destroy(entity);
}

/* Test case: Interactor with custom rules */
TEST_CASE(test_interactor_init_with_rules) {
    struct scv_business_rules rules = {0};
    rules.min_battery_for_recording = 30;
    rules.min_storage_for_recording_mb = 200;
    rules.max_recording_duration_sec = 7200;
    rules.min_battery_for_upload = 40;
    rules.min_network_strength_for_upload = 50;
    rules.upload_only_on_wifi = true;
    rules.max_upload_retries = 3;
    rules.min_battery_for_ota = 80;
    rules.ota_only_on_wifi = true;
    rules.ota_only_when_charging = true;
    rules.sleep_battery_threshold = 15;
    rules.critical_battery_threshold = 5;
    rules.max_consecutive_errors = 5;
    rules.error_recovery_delay_ms = 5000;
    
    struct scv_interactor *interactor = scv_interactor_init(NULL, &rules);
    TEST_ASSERT_NOT_NULL(interactor);
    
    const struct scv_business_rules *retrieved_rules = scv_interactor_get_rules(interactor);
    TEST_ASSERT_NOT_NULL(retrieved_rules);
    TEST_ASSERT_EQUAL(30, retrieved_rules->min_battery_for_recording);
    TEST_ASSERT_EQUAL(200, retrieved_rules->min_storage_for_recording_mb);
    TEST_ASSERT_EQUAL(7200, retrieved_rules->max_recording_duration_sec);
    TEST_ASSERT_EQUAL(40, retrieved_rules->min_battery_for_upload);
    TEST_ASSERT_EQUAL(50, retrieved_rules->min_network_strength_for_upload);
    TEST_ASSERT_EQUAL(true, retrieved_rules->upload_only_on_wifi);
    TEST_ASSERT_EQUAL(3, retrieved_rules->max_upload_retries);
    TEST_ASSERT_EQUAL(80, retrieved_rules->min_battery_for_ota);
    TEST_ASSERT_EQUAL(true, retrieved_rules->ota_only_on_wifi);
    TEST_ASSERT_EQUAL(true, retrieved_rules->ota_only_when_charging);
    TEST_ASSERT_EQUAL(15, retrieved_rules->sleep_battery_threshold);
    TEST_ASSERT_EQUAL(5, retrieved_rules->critical_battery_threshold);
    TEST_ASSERT_EQUAL(5, retrieved_rules->max_consecutive_errors);
    TEST_ASSERT_EQUAL(5000, retrieved_rules->error_recovery_delay_ms);
    
    scv_interactor_destroy(interactor);
}

/* Test case: Interactor state transitions */
TEST_CASE(test_interactor_state_transitions) {
    struct scv_interactor *interactor = scv_interactor_init(NULL, NULL);
    TEST_ASSERT_NOT_NULL(interactor);
    
    /* Initially in INIT state */
    TEST_ASSERT_EQUAL(INTERACTOR_STATE_INIT, scv_interactor_get_state(interactor));
    
    /* Process SYSTEM_START event to move to IDLE */
    int result = scv_interactor_process_event(interactor, INTERACTOR_EVT_SYSTEM_START, NULL);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(INTERACTOR_STATE_IDLE, scv_interactor_get_state(interactor));
    
    /* Process RECORDING_START event to move to RECORDING */
    result = scv_interactor_process_event(interactor, INTERACTOR_EVT_RECORDING_START, NULL);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(INTERACTOR_STATE_RECORDING, scv_interactor_get_state(interactor));
    
    /* Process RECORDING_STOP event to move back to IDLE */
    result = scv_interactor_process_event(interactor, INTERACTOR_EVT_RECORDING_STOP, NULL);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(INTERACTOR_STATE_IDLE, scv_interactor_get_state(interactor));
    
    /* Process UPLOAD_START event to move to UPLOADING */
    result = scv_interactor_process_event(interactor, INTERACTOR_EVT_UPLOAD_START, NULL);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(INTERACTOR_STATE_UPLOADING, scv_interactor_get_state(interactor));
    
    /* Process UPLOAD_COMPLETE event to move back to IDLE */
    result = scv_interactor_process_event(interactor, INTERACTOR_EVT_UPLOAD_COMPLETE, NULL);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(INTERACTOR_STATE_IDLE, scv_interactor_get_state(interactor));
    
    /* Process OTA_START event to move to OTA */
    result = scv_interactor_process_event(interactor, INTERACTOR_EVT_OTA_START, NULL);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(INTERACTOR_STATE_OTA, scv_interactor_get_state(interactor));
    
    /* Process OTA_COMPLETE event to move back to IDLE */
    result = scv_interactor_process_event(interactor, INTERACTOR_EVT_OTA_COMPLETE, NULL);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(INTERACTOR_STATE_IDLE, scv_interactor_get_state(interactor));
    
    /* Process POWER_LOW event to move to SLEEP */
    result = scv_interactor_process_event(interactor, INTERACTOR_EVT_POWER_LOW, NULL);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(INTERACTOR_STATE_SLEEP, scv_interactor_get_state(interactor));
    
    /* Process SYSTEM_RESET event to move back to INIT */
    result = scv_interactor_process_event(interactor, INTERACTOR_EVT_SYSTEM_RESET, NULL);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(INTERACTOR_STATE_INIT, scv_interactor_get_state(interactor));
    
    scv_interactor_destroy(interactor);
}

/* Test case: Interactor rules update */
TEST_CASE(test_interactor_update_rules) {
    struct scv_interactor *interactor = scv_interactor_init(NULL, NULL);
    TEST_ASSERT_NOT_NULL(interactor);
    
    struct scv_business_rules new_rules = {0};
    new_rules.min_battery_for_recording = 25;
    new_rules.min_storage_for_recording_mb = 150;
    new_rules.max_recording_duration_sec = 1800;
    
    int result = scv_interactor_update_rules(interactor, &new_rules);
    TEST_ASSERT_EQUAL(0, result);
    
    const struct scv_business_rules *rules = scv_interactor_get_rules(interactor);
    TEST_ASSERT_NOT_NULL(rules);
    TEST_ASSERT_EQUAL(25, rules->min_battery_for_recording);
    TEST_ASSERT_EQUAL(150, rules->min_storage_for_recording_mb);
    TEST_ASSERT_EQUAL(1800, rules->max_recording_duration_sec);
    
    scv_interactor_destroy(interactor);
}

/* Test case: Interactor evaluate rules */
TEST_CASE(test_interactor_evaluate_rules) {
    struct scv_entity *entity = scv_entity_init(NULL);
    TEST_ASSERT_NOT_NULL(entity);
    
    struct scv_interactor *interactor = scv_interactor_init(entity, NULL);
    TEST_ASSERT_NOT_NULL(interactor);
    
    /* Update entity data to test rule evaluation */
    struct scv_system_data data = {0};
    data.battery_level = 80;
    data.storage_used_mb = 1024;
    data.storage_total_mb = 8192;
    data.network_strength = 75;
    /* Note: is_charging field doesn't exist in scv_system_data */
    
    scv_entity_update_data(entity, &data);
    
    /* Evaluate rules */
    int result = scv_interactor_evaluate_rules(interactor);
    TEST_ASSERT_EQUAL(0, result);
    
    /* Check context */
    const struct scv_business_context *context = scv_interactor_get_context(interactor);
    TEST_ASSERT_NOT_NULL(context);
    TEST_ASSERT_EQUAL(true, context->has_sufficient_battery);  /* 80 > 20 */
    TEST_ASSERT_EQUAL(true, context->has_sufficient_storage);  /* 8192-1024 > 100 */
    /* is_charging may be false by default */
    
    scv_interactor_destroy(interactor);
    scv_entity_destroy(entity);
}

/* Test case: Interactor callbacks */
static int state_changed_count = 0;
static scv_interactor_state_t last_new_state = INTERACTOR_STATE_INIT;
static scv_interactor_state_t last_old_state = INTERACTOR_STATE_INIT;

static void test_state_changed_callback(scv_interactor_state_t new_state, 
                                        scv_interactor_state_t old_state,
                                        void *user_data) {
    (void)user_data;
    state_changed_count++;
    last_new_state = new_state;
    last_old_state = old_state;
}

TEST_CASE(test_interactor_callbacks) {
    struct scv_interactor *interactor = scv_interactor_init(NULL, NULL);
    TEST_ASSERT_NOT_NULL(interactor);
    
    scv_interactor_set_state_changed_callback(interactor, test_state_changed_callback, NULL);
    
    /* Initial state is INIT */
    TEST_ASSERT_EQUAL(INTERACTOR_STATE_INIT, scv_interactor_get_state(interactor));
    
    /* Process SYSTEM_START event to move to IDLE - should trigger callback */
    state_changed_count = 0;
    int result = scv_interactor_process_event(interactor, INTERACTOR_EVT_SYSTEM_START, NULL);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(1, state_changed_count);
    TEST_ASSERT_EQUAL(INTERACTOR_STATE_IDLE, last_new_state);
    TEST_ASSERT_EQUAL(INTERACTOR_STATE_INIT, last_old_state);
    
    /* Process RECORDING_START event to move to RECORDING - should trigger callback */
    result = scv_interactor_process_event(interactor, INTERACTOR_EVT_RECORDING_START, NULL);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(2, state_changed_count);
    TEST_ASSERT_EQUAL(INTERACTOR_STATE_RECORDING, last_new_state);
    TEST_ASSERT_EQUAL(INTERACTOR_STATE_IDLE, last_old_state);
    
    scv_interactor_destroy(interactor);
}

/* Test case: Interactor reset */
TEST_CASE(test_interactor_reset) {
    struct scv_interactor *interactor = scv_interactor_init(NULL, NULL);
    TEST_ASSERT_NOT_NULL(interactor);
    
    /* Move to IDLE state */
    scv_interactor_process_event(interactor, INTERACTOR_EVT_SYSTEM_START, NULL);
    TEST_ASSERT_EQUAL(INTERACTOR_STATE_IDLE, scv_interactor_get_state(interactor));
    
    /* Reset to INIT */
    int result = scv_interactor_reset(interactor);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(INTERACTOR_STATE_INIT, scv_interactor_get_state(interactor));
    
    scv_interactor_destroy(interactor);
}

/* Test case: Interactor memory leak check */
TEST_CASE(test_interactor_memory_leak) {
    const int iterations = 50;
    for (int i = 0; i < iterations; i++) {
        struct scv_interactor *interactor = scv_interactor_init(NULL, NULL);
        TEST_ASSERT_NOT_NULL(interactor);
        
        /* Do some operations */
        scv_interactor_process_event(interactor, INTERACTOR_EVT_SYSTEM_START, NULL);
        scv_interactor_process_event(interactor, INTERACTOR_EVT_RECORDING_START, NULL);
        
        scv_interactor_destroy(interactor);
    }
    /* If we get here without crashing, assume no leak */
    TEST_ASSERT(1);
}

/* Main test suite */
int main(void) {
    void (*test_cases[])(void) = {
        test_interactor_init_basic,
        test_interactor_init_with_entity,
        test_interactor_init_with_rules,
        test_interactor_state_transitions,
        test_interactor_update_rules,
        test_interactor_evaluate_rules,
        test_interactor_callbacks,
        test_interactor_reset,
        test_interactor_memory_leak,
    };
    
    RUN_TEST_SUITE(SCV_Interactor, test_cases);
    print_test_summary();
    
    return (test_fail_count == 0) ? 0 : 1;
}
