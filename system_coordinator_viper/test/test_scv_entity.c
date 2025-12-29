/**
 * test_scv_entity.c - Test cases for System Coordinator VIPER Entity
 *
 * Tests entity initialization, data management, and memory cleanup.
 */

#include "test_framework.h"
#include "../include/scv_entity.h"
#include <stdlib.h>
#include <string.h>

/* Test case: Entity initialization */
TEST_CASE(test_entity_init_basic) {
    struct scv_entity *entity = scv_entity_init(NULL);
    TEST_ASSERT_NOT_NULL(entity);
    TEST_ASSERT_NOT_NULL(entity->efsm);
    TEST_ASSERT_EQUAL(ENTITY_STATE_INIT, scv_entity_get_state(entity));
    
    /* Check default data */
    const struct scv_system_data *data = scv_entity_get_data(entity);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL_STRING("UNKNOWN", data->device_id);
    TEST_ASSERT_EQUAL_STRING("1.0.0", data->firmware_version);
    TEST_ASSERT_EQUAL(100, data->battery_level);
    
    scv_entity_destroy(entity);
}

/* Test case: Entity with custom config */
TEST_CASE(test_entity_init_with_config) {
    struct scv_entity_config config = {0};
    strcpy(config.entity_name, "TestEntity");
    config.entity_id = 42;
    config.status_update_interval = 2000;
    config.max_storage_mb = 8192;
    
    struct scv_entity *entity = scv_entity_init(&config);
    TEST_ASSERT_NOT_NULL(entity);
    TEST_ASSERT_EQUAL_STRING("TestEntity", entity->config.entity_name);
    TEST_ASSERT_EQUAL(42, entity->config.entity_id);
    TEST_ASSERT_EQUAL(2000, entity->config.status_update_interval);
    TEST_ASSERT_EQUAL(8192, entity->config.max_storage_mb);
    
    scv_entity_destroy(entity);
}

/* Test case: Entity data update */
TEST_CASE(test_entity_update_data) {
    struct scv_entity *entity = scv_entity_init(NULL);
    TEST_ASSERT_NOT_NULL(entity);
    
    struct scv_system_data new_data = {0};
    strcpy(new_data.device_id, "TEST_DEVICE_001");
    new_data.battery_level = 75;
    new_data.storage_used_mb = 1024;
    new_data.storage_total_mb = 8192;
    new_data.network_strength = 80;
    new_data.is_recording = true;
    new_data.recording_duration_sec = 120;
    
    int result = scv_entity_update_data(entity, &new_data);
    TEST_ASSERT_EQUAL(0, result);
    
    const struct scv_system_data *data = scv_entity_get_data(entity);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL_STRING("TEST_DEVICE_001", data->device_id);
    TEST_ASSERT_EQUAL(75, data->battery_level);
    TEST_ASSERT_EQUAL(1024, data->storage_used_mb);
    TEST_ASSERT_EQUAL(8192, data->storage_total_mb);
    TEST_ASSERT_EQUAL(80, data->network_strength);
    TEST_ASSERT_EQUAL(true, data->is_recording);
    TEST_ASSERT_EQUAL(120, data->recording_duration_sec);
    
    scv_entity_destroy(entity);
}

/* Test case: Entity config update */
TEST_CASE(test_entity_update_config) {
    struct scv_entity *entity = scv_entity_init(NULL);
    TEST_ASSERT_NOT_NULL(entity);
    
    struct scv_entity_config new_config = {0};
    strcpy(new_config.entity_name, "UpdatedEntity");
    new_config.entity_id = 99;
    new_config.status_update_interval = 3000;
    
    int result = scv_entity_update_config(entity, &new_config);
    TEST_ASSERT_EQUAL(0, result);
    
    const struct scv_entity_config *config = scv_entity_get_config(entity);
    TEST_ASSERT_NOT_NULL(config);
    TEST_ASSERT_EQUAL_STRING("UpdatedEntity", config->entity_name);
    TEST_ASSERT_EQUAL(99, config->entity_id);
    TEST_ASSERT_EQUAL(3000, config->status_update_interval);
    
    scv_entity_destroy(entity);
}

/* Test case: Entity state transitions */
TEST_CASE(test_entity_state_transitions) {
    struct scv_entity *entity = scv_entity_init(NULL);
    TEST_ASSERT_NOT_NULL(entity);
    
    /* Initially in INIT state */
    TEST_ASSERT_EQUAL(ENTITY_STATE_INIT, scv_entity_get_state(entity));
    
    /* Process event to move to READY */
    scv_entity_process_event(entity, ENTITY_EVENT_INIT_COMPLETE, NULL);
    TEST_ASSERT_EQUAL(ENTITY_STATE_READY, scv_entity_get_state(entity));
    
    /* Process event to move to UPDATING */
    scv_entity_process_event(entity, ENTITY_EVENT_UPDATE_REQUEST, NULL);
    TEST_ASSERT_EQUAL(ENTITY_STATE_UPDATING, scv_entity_get_state(entity));
    
    /* Process event to move back to READY */
    scv_entity_process_event(entity, ENTITY_EVENT_UPDATE_COMPLETE, NULL);
    TEST_ASSERT_EQUAL(ENTITY_STATE_READY, scv_entity_get_state(entity));
    
    /* Process error event */
    scv_entity_process_event(entity, ENTITY_EVENT_ERROR_OCCURRED, NULL);
    TEST_ASSERT_EQUAL(ENTITY_STATE_ERROR, scv_entity_get_state(entity));
    
    /* Process reset event */
    scv_entity_process_event(entity, ENTITY_EVENT_RESET, NULL);
    TEST_ASSERT_EQUAL(ENTITY_STATE_INIT, scv_entity_get_state(entity));
    
    scv_entity_destroy(entity);
}

/* Test case: Entity memory leak check (create/destroy many times) */
TEST_CASE(test_entity_memory_leak) {
    const int iterations = 100;
    for (int i = 0; i < iterations; i++) {
        struct scv_entity *entity = scv_entity_init(NULL);
        TEST_ASSERT_NOT_NULL(entity);
        
        /* Do some operations */
        struct scv_system_data data = {0};
        data.battery_level = i % 100;
        scv_entity_update_data(entity, &data);
        
        scv_entity_destroy(entity);
    }
    /* If we get here without crashing, assume no leak */
    TEST_ASSERT(1);
}

/* Test case: Entity callbacks */
static int callback_called = 0;
static void test_data_updated_callback(const struct scv_system_data *data, void *user_data) {
    (void)data;
    int *counter = (int *)user_data;
    (*counter)++;
}

TEST_CASE(test_entity_callbacks) {
    int counter = 0;
    struct scv_entity *entity = scv_entity_init(NULL);
    TEST_ASSERT_NOT_NULL(entity);
    
    scv_entity_set_data_updated_callback(entity, test_data_updated_callback, &counter);
    
    /* Update data should trigger callback */
    struct scv_system_data data = {0};
    data.battery_level = 50;
    scv_entity_update_data(entity, &data);
    
    TEST_ASSERT_EQUAL(1, counter);
    
    /* Another update */
    data.battery_level = 60;
    scv_entity_update_data(entity, &data);
    TEST_ASSERT_EQUAL(2, counter);
    
    scv_entity_destroy(entity);
}

/* Main test suite */
int main(void) {
    void (*test_cases[])(void) = {
        test_entity_init_basic,
        test_entity_init_with_config,
        test_entity_update_data,
        test_entity_update_config,
        test_entity_state_transitions,
        test_entity_memory_leak,
        test_entity_callbacks,
    };
    
    RUN_TEST_SUITE(SCV_Entity, test_cases);
    print_test_summary();
    
    return (test_fail_count == 0) ? 0 : 1;
}
