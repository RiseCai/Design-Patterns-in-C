/**
 * test_viper_view.c - Unit tests for VIPER View component
 */

#include "test_framework.h"
#include "../include/viper_view.h"
#include <stdlib.h>
#include <string.h>

/* Mock present callback for testing */
static int mock_present_called = 0;
static const void *mock_present_data = NULL;
static size_t mock_present_size = 0;

static void mock_present_callback(const void *data, size_t size) {
    mock_present_called++;
    mock_present_data = data;
    mock_present_size = size;
}

TEST_CASE(test_viper_view_init_basic) {
    struct viper_view *view = viper_view_init("TestView", 123, mock_present_callback);
    TEST_ASSERT_NOT_NULL(view);
    TEST_ASSERT_EQUAL_STRING("TestView", view->view_name);
    TEST_ASSERT_EQUAL(123, view->view_id);
    TEST_ASSERT_NOT_NULL(view->fsm);
    TEST_ASSERT_EQUAL(VIEW_STATE_IDLE, viper_view_get_state(view));
    
    viper_view_destroy(view);
}

TEST_CASE(test_viper_view_init_null_name) {
    struct viper_view *view = viper_view_init(NULL, 456, mock_present_callback);
    TEST_ASSERT_NOT_NULL(view);
    /* Should have default name */
    TEST_ASSERT(strlen(view->view_name) > 0);
    viper_view_destroy(view);
}

TEST_CASE(test_viper_view_init_null_callback) {
    struct viper_view *view = viper_view_init("NoCallback", 789, NULL);
    TEST_ASSERT_NOT_NULL(view);
    TEST_ASSERT_NULL(view->present_callback);
    viper_view_destroy(view);
}

TEST_CASE(test_viper_view_destroy_null) {
    /* Should not crash */
    viper_view_destroy(NULL);
}

TEST_CASE(test_viper_view_process_event_user_input) {
    struct viper_view *view = viper_view_init("EventTest", 111, mock_present_callback);
    TEST_ASSERT_NOT_NULL(view);
    
    int result = viper_view_process_event(view, VIEW_EVENT_USER_INPUT, "input data");
    TEST_ASSERT_EQUAL(0, result);
    /* State may have changed depending on implementation */
    
    viper_view_destroy(view);
}

TEST_CASE(test_viper_view_process_event_invalid) {
    struct viper_view *view = viper_view_init("EventTest", 222, NULL);
    TEST_ASSERT_NOT_NULL(view);
    
    /* Process with NULL view */
    int result = viper_view_process_event(NULL, VIEW_EVENT_USER_INPUT, NULL);
    TEST_ASSERT_EQUAL(-1, result);
    
    /* Process with invalid event (out of range) */
    result = viper_view_process_event(view, (viper_view_event_t)999, NULL);
    TEST_ASSERT_EQUAL(-1, result);
    
    viper_view_destroy(view);
}

TEST_CASE(test_viper_view_update) {
    struct viper_view *view = viper_view_init("UpdateTest", 333, mock_present_callback);
    TEST_ASSERT_NOT_NULL(view);
    
    const char *data = "Hello, World!";
    size_t size = strlen(data) + 1;
    
    int result = viper_view_update(view, data, size);
    TEST_ASSERT_EQUAL(0, result);
    /* Check that data was stored */
    const struct viper_view_data *view_data = viper_view_get_data(view);
    TEST_ASSERT_NOT_NULL(view_data);
    TEST_ASSERT_NOT_NULL(view_data->display_buffer);
    TEST_ASSERT_EQUAL(size, view_data->buffer_size);
    TEST_ASSERT_EQUAL_STRING(data, (char *)view_data->display_buffer);
    
    viper_view_destroy(view);
}

TEST_CASE(test_viper_view_update_null) {
    struct viper_view *view = viper_view_init("UpdateNull", 444, NULL);
    TEST_ASSERT_NOT_NULL(view);
    
    /* Update with NULL view */
    int result = viper_view_update(NULL, "data", 5);
    TEST_ASSERT_EQUAL(-1, result);
    
    /* Update with NULL data but size > 0 */
    result = viper_view_update(view, NULL, 10);
    TEST_ASSERT_EQUAL(-1, result);
    
    /* Update with zero size */
    result = viper_view_update(view, "data", 0);
    TEST_ASSERT_EQUAL(-1, result);
    
    viper_view_destroy(view);
}

TEST_CASE(test_viper_view_get_state) {
    struct viper_view *view = viper_view_init("StateTest", 555, NULL);
    TEST_ASSERT_NOT_NULL(view);
    
    viper_view_state_t state = viper_view_get_state(view);
    /* Initial state should be IDLE */
    TEST_ASSERT_EQUAL(VIEW_STATE_IDLE, state);
    
    /* After processing an event, state may change */
    viper_view_process_event(view, VIEW_EVENT_USER_INPUT, NULL);
    state = viper_view_get_state(view);
    /* Depending on implementation, state may be LOADING or still IDLE */
    /* We'll just ensure it's a valid state */
    TEST_ASSERT(state >= VIEW_STATE_IDLE && state <= VIEW_STATE_UPDATING);
    
    viper_view_destroy(view);
}

TEST_CASE(test_viper_view_get_data) {
    struct viper_view *view = viper_view_init("DataTest", 666, NULL);
    TEST_ASSERT_NOT_NULL(view);
    
    const struct viper_view_data *data = viper_view_get_data(view);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL(VIEW_STATE_IDLE, data->current_state);
    TEST_ASSERT_NULL(data->display_buffer);
    TEST_ASSERT_EQUAL(0, data->buffer_size);
    
    viper_view_destroy(view);
}

TEST_CASE(test_viper_view_get_data_null) {
    const struct viper_view_data *data = viper_view_get_data(NULL);
    TEST_ASSERT_NULL(data);
}

/* Test suite array */
void (*viper_view_tests[])(void) = {
    test_viper_view_init_basic,
    test_viper_view_init_null_name,
    test_viper_view_init_null_callback,
    test_viper_view_destroy_null,
    test_viper_view_process_event_user_input,
    test_viper_view_process_event_invalid,
    test_viper_view_update,
    test_viper_view_update_null,
    test_viper_view_get_state,
    test_viper_view_get_data,
    test_viper_view_get_data_null,
};

int main(void) {
    printf("VIPER View Unit Tests\n");
    printf("=====================\n");

    RUN_TEST_SUITE(VIPER_View, viper_view_tests);
    print_test_summary();

    return test_fail_count > 0 ? 1 : 0;
}
