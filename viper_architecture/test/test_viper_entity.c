/**
 * test_viper_entity.c - Unit tests for VIPER Entity component
 *
 * Tests initialization, destruction, data operations, validation,
 * state transitions, and error handling.
 */

#include "test_framework.h"
#include "../include/viper_entity.h"
#include <string.h>
#include <stdlib.h>

/* Test case: initialization and destruction */
TEST_CASE(test_entity_init_destroy)
{
    struct viper_entity *entity = viper_entity_init("TestEntity", 1);
    TEST_ASSERT_NOT_NULL(entity);
    TEST_ASSERT_EQUAL_STRING("TestEntity", entity->entity_name);
    TEST_ASSERT_EQUAL(1, entity->entity_id);
    TEST_ASSERT_NOT_NULL(entity->fsm);
    TEST_ASSERT_EQUAL(ENTITY_STATE_INITIAL, viper_entity_get_state(entity));

    viper_entity_destroy(entity);
    /* No crash expected */
}

/* Test case: load data */
TEST_CASE(test_entity_load_data)
{
    struct viper_entity *entity = viper_entity_init("TestEntity", 2);
    TEST_ASSERT_NOT_NULL(entity);

    const char source[] = "memory";
    int ret = viper_entity_load_data(entity, source, NULL);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(ENTITY_STATE_LOADED, viper_entity_get_state(entity));

    const struct viper_entity_data *data = viper_entity_get_data(entity);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL(0, data->is_dirty);
    TEST_ASSERT_EQUAL(1, data->is_persisted);

    viper_entity_destroy(entity);
}

/* Test case: update data */
TEST_CASE(test_entity_update_data)
{
    struct viper_entity *entity = viper_entity_init("TestEntity", 3);
    TEST_ASSERT_NOT_NULL(entity);

    const char new_data[] = "Hello, VIPER!";
    int ret = viper_entity_update_data(entity, new_data, sizeof(new_data));
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(ENTITY_STATE_MODIFIED, viper_entity_get_state(entity));

    const struct viper_entity_data *data = viper_entity_get_data(entity);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL(1, data->is_dirty);
    TEST_ASSERT_EQUAL(0, data->is_persisted);
    TEST_ASSERT_EQUAL(sizeof(new_data), data->data_size);
    TEST_ASSERT_NOT_NULL(data->data_buffer);
    TEST_ASSERT_EQUAL_STRING(new_data, (const char *)data->data_buffer);

    viper_entity_destroy(entity);
}

/* Test case: validate data with no data (should fail) */
TEST_CASE(test_entity_validate_no_data)
{
    struct viper_entity *entity = viper_entity_init("TestEntity", 4);
    TEST_ASSERT_NOT_NULL(entity);

    const char rules[] = "some rules";
    int score = viper_entity_validate_data(entity, rules, sizeof(rules));
    /* Since there's no data buffer, validation score should be 0 */
    TEST_ASSERT_EQUAL(0, score);
    TEST_ASSERT_EQUAL(ENTITY_STATE_INVALID, viper_entity_get_state(entity));

    const struct viper_entity_data *data = viper_entity_get_data(entity);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL(0, data->is_valid);
    TEST_ASSERT_EQUAL_STRING("No data to validate", data->validation_errors);

    viper_entity_destroy(entity);
}

/* Test case: validate data with data (should succeed) */
TEST_CASE(test_entity_validate_with_data)
{
    struct viper_entity *entity = viper_entity_init("TestEntity", 5);
    TEST_ASSERT_NOT_NULL(entity);

    /* First load some data */
    const char new_data[] = "Valid data";
    viper_entity_update_data(entity, new_data, sizeof(new_data));

    const char rules[] = "rules";
    int score = viper_entity_validate_data(entity, rules, sizeof(rules));
    /* The mock validation returns 85 */
    TEST_ASSERT_EQUAL(85, score);
    TEST_ASSERT_EQUAL(ENTITY_STATE_VALID, viper_entity_get_state(entity));

    const struct viper_entity_data *data = viper_entity_get_data(entity);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL(1, data->is_valid);
    TEST_ASSERT_EQUAL_STRING("No critical errors", data->validation_errors);

    viper_entity_destroy(entity);
}

/* Test case: save data */
TEST_CASE(test_entity_save_data)
{
    struct viper_entity *entity = viper_entity_init("TestEntity", 6);
    TEST_ASSERT_NOT_NULL(entity);

    /* Load data first */
    const char new_data[] = "Data to save";
    viper_entity_update_data(entity, new_data, sizeof(new_data));

    const char dest[] = "file";
    int ret = viper_entity_save_data(entity, dest, NULL);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(ENTITY_STATE_LOADED, viper_entity_get_state(entity));

    const struct viper_entity_data *data = viper_entity_get_data(entity);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL(0, data->is_dirty);
    TEST_ASSERT_EQUAL(1, data->is_persisted);

    viper_entity_destroy(entity);
}

/* Test case: save data with no data (should fail) */
TEST_CASE(test_entity_save_no_data)
{
    struct viper_entity *entity = viper_entity_init("TestEntity", 7);
    TEST_ASSERT_NOT_NULL(entity);

    const char dest[] = "file";
    int ret = viper_entity_save_data(entity, dest, NULL);
    TEST_ASSERT_EQUAL(-1, ret);
    TEST_ASSERT_EQUAL(ENTITY_STATE_ERROR, viper_entity_get_state(entity));

    viper_entity_destroy(entity);
}

/* Test case: query data */
TEST_CASE(test_entity_query_data)
{
    struct viper_entity *entity = viper_entity_init("TestEntity", 8);
    TEST_ASSERT_NOT_NULL(entity);

    /* Load data */
    const char new_data[] = "Queryable data";
    viper_entity_update_data(entity, new_data, sizeof(new_data));

    const char query[] = "SELECT *";
    void *result = NULL;
    size_t result_size = 0;
    int ret = viper_entity_query_data(entity, query, sizeof(query), &result, &result_size);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_NOT_NULL(result);
    TEST_ASSERT_EQUAL(sizeof(new_data), result_size);
    TEST_ASSERT_EQUAL_STRING(new_data, (const char *)result);

    free(result);
    viper_entity_destroy(entity);
}

/* Test case: query data with no data (should fail) */
TEST_CASE(test_entity_query_no_data)
{
    struct viper_entity *entity = viper_entity_init("TestEntity", 9);
    TEST_ASSERT_NOT_NULL(entity);

    const char query[] = "SELECT *";
    void *result = NULL;
    size_t result_size = 0;
    int ret = viper_entity_query_data(entity, query, sizeof(query), &result, &result_size);
    TEST_ASSERT_EQUAL(-1, ret);
    TEST_ASSERT_NULL(result);
    TEST_ASSERT_EQUAL(0, result_size);

    viper_entity_destroy(entity);
}

/* Test case: state transitions sequence */
TEST_CASE(test_entity_state_transitions)
{
    struct viper_entity *entity = viper_entity_init("TestEntity", 10);
    TEST_ASSERT_NOT_NULL(entity);

    /* Initial state */
    TEST_ASSERT_EQUAL(ENTITY_STATE_INITIAL, viper_entity_get_state(entity));

    /* Load -> LOADED */
    viper_entity_load_data(entity, "src", NULL);
    TEST_ASSERT_EQUAL(ENTITY_STATE_LOADED, viper_entity_get_state(entity));

    /* Update -> MODIFIED */
    const char data[] = "update";
    viper_entity_update_data(entity, data, sizeof(data));
    TEST_ASSERT_EQUAL(ENTITY_STATE_MODIFIED, viper_entity_get_state(entity));

    /* Validate -> VALID */
    viper_entity_validate_data(entity, NULL, 0);
    TEST_ASSERT_EQUAL(ENTITY_STATE_VALID, viper_entity_get_state(entity));

    /* Save -> LOADED */
    viper_entity_save_data(entity, "dest", NULL);
    TEST_ASSERT_EQUAL(ENTITY_STATE_LOADED, viper_entity_get_state(entity));

    viper_entity_destroy(entity);
}

/* Test suite runner */
int main(void)
{
    test_pass_count = 0;
    test_fail_count = 0;

    void (*test_cases[])(void) = {
        test_entity_init_destroy,
        test_entity_load_data,
        test_entity_update_data,
        test_entity_validate_no_data,
        test_entity_validate_with_data,
        test_entity_save_data,
        test_entity_save_no_data,
        test_entity_query_data,
        test_entity_query_no_data,
        test_entity_state_transitions,
    };

    RUN_TEST_SUITE(VIPER_Entity, test_cases);
    print_test_summary();

    return test_fail_count == 0 ? 0 : 1;
}
