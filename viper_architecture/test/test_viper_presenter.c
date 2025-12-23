/**
 * test_viper_presenter.c - Unit tests for VIPER Presenter component
 *
 * Tests initialization, destruction, event processing, view updates,
 * data requests, and state transitions.
 */

#include "test_framework.h"
#include "../include/viper_presenter.h"
#include "../include/viper_view.h"
#include "../include/viper_interactor.h"
#include <string.h>
#include <stdlib.h>

/* Dummy callbacks for testing */
static void dummy_view_update_callback(void *ctx, const void *data, size_t size) {
    (*(int *)ctx)++;
}

static void dummy_interactor_request_callback(void *ctx, const void *request) {
    (*(int *)ctx)++;
}

/* Mock view for testing */
static struct viper_view mock_view = {
    .fsm = NULL,
    .data = {
        .current_state = VIEW_STATE_IDLE,
        .display_buffer = NULL,
        .buffer_size = 0,
        .last_input = "",
        .error_code = 0,
        .error_message = ""
    },
    .view_name = "MockView",
    .view_id = 100,
    .present_callback = NULL
};

/* Mock interactor for testing */
static struct viper_interactor mock_interactor = {
    .fsm = NULL,
    .context = {
        .input_data = NULL,
        .input_size = 0,
        .output_data = NULL,
        .output_size = 0,
        .rule_id = 0,
        .rule_context = NULL,
        .validation_status = 0,
        .validation_message = ""
    },
    .interactor_name = "MockInteractor",
    .interactor_id = 200,
    .entity = NULL,
    .process_data_callback = NULL,
    .validate_input_callback = NULL,
    .business_rule_callback = NULL
};

/* Test case: initialization and destruction */
TEST_CASE(test_presenter_init_destroy)
{
    struct viper_presenter *presenter = viper_presenter_init("TestPresenter", 1, &mock_view, &mock_interactor);
    TEST_ASSERT_NOT_NULL(presenter);
    TEST_ASSERT_EQUAL_STRING("TestPresenter", presenter->presenter_name);
    TEST_ASSERT_EQUAL(1, presenter->presenter_id);
    TEST_ASSERT_EQUAL_PTR(&mock_view, presenter->view);
    TEST_ASSERT_EQUAL_PTR(&mock_interactor, presenter->interactor);
    TEST_ASSERT_NOT_NULL(presenter->fsm);
    TEST_ASSERT_EQUAL(PRESENTER_STATE_IDLE, viper_presenter_get_state(presenter));

    viper_presenter_destroy(presenter);
    /* No crash expected */
}

/* Test case: process VIEW_UPDATE event */
TEST_CASE(test_presenter_process_view_update)
{
    struct viper_presenter *presenter = viper_presenter_init("TestPresenter", 2, &mock_view, &mock_interactor);
    TEST_ASSERT_NOT_NULL(presenter);

    int ret = viper_presenter_process_event(presenter, PRESENTER_EVENT_VIEW_UPDATE, NULL);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(PRESENTER_STATE_PROCESSING, viper_presenter_get_state(presenter));

    viper_presenter_destroy(presenter);
}

/* Test case: process DATA_READY event */
TEST_CASE(test_presenter_process_data_ready)
{
    struct viper_presenter *presenter = viper_presenter_init("TestPresenter", 3, &mock_view, &mock_interactor);
    TEST_ASSERT_NOT_NULL(presenter);

    int ret = viper_presenter_process_event(presenter, PRESENTER_EVENT_DATA_READY, NULL);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(PRESENTER_STATE_UPDATING_VIEW, viper_presenter_get_state(presenter));

    viper_presenter_destroy(presenter);
}

/* Test case: process ERROR event */
TEST_CASE(test_presenter_process_error)
{
    struct viper_presenter *presenter = viper_presenter_init("TestPresenter", 4, &mock_view, &mock_interactor);
    TEST_ASSERT_NOT_NULL(presenter);

    const char error_msg[] = "Something went wrong";
    int ret = viper_presenter_process_event(presenter, PRESENTER_EVENT_ERROR, error_msg);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(PRESENTER_STATE_ERROR, viper_presenter_get_state(presenter));

    const struct viper_presenter_data *data = viper_presenter_get_data(presenter);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL_STRING(error_msg, data->error_message);

    viper_presenter_destroy(presenter);
}

/* Test case: update view */
TEST_CASE(test_presenter_update_view)
{
    struct viper_presenter *presenter = viper_presenter_init("TestPresenter", 5, &mock_view, &mock_interactor);
    TEST_ASSERT_NOT_NULL(presenter);

    /* Set a dummy view update callback to verify it's called */
    int callback_called = 0;
    void *callback_context = &callback_called;
    presenter->view_update_callback = dummy_view_update_callback;
    presenter->data.view_context = callback_context;

    const char view_data[] = "View data";
    int ret = viper_presenter_update_view(presenter, view_data, sizeof(view_data));
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(1, callback_called);

    const struct viper_presenter_data *data = viper_presenter_get_data(presenter);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_NOT_NULL(data->presentation_data);
    TEST_ASSERT_EQUAL(sizeof(view_data), data->data_size);
    TEST_ASSERT_EQUAL_STRING(view_data, (const char *)data->presentation_data);

    viper_presenter_destroy(presenter);
}

/* Test case: request data from interactor */
TEST_CASE(test_presenter_request_data)
{
    struct viper_presenter *presenter = viper_presenter_init("TestPresenter", 6, &mock_view, &mock_interactor);
    TEST_ASSERT_NOT_NULL(presenter);

    /* Set a dummy interactor request callback to verify it's called */
    int callback_called = 0;
    void *callback_context = &callback_called;
    presenter->interactor_request_callback = dummy_interactor_request_callback;
    presenter->data.interactor_context = callback_context;

    const char request[] = "Fetch data";
    int ret = viper_presenter_request_data(presenter, request, sizeof(request));
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(1, callback_called);

    viper_presenter_destroy(presenter);
}

/* Test case: state transitions sequence */
TEST_CASE(test_presenter_state_transitions)
{
    struct viper_presenter *presenter = viper_presenter_init("TestPresenter", 7, &mock_view, &mock_interactor);
    TEST_ASSERT_NOT_NULL(presenter);

    /* Start idle */
    TEST_ASSERT_EQUAL(PRESENTER_STATE_IDLE, viper_presenter_get_state(presenter));

    /* VIEW_UPDATE -> PROCESSING */
    viper_presenter_process_event(presenter, PRESENTER_EVENT_VIEW_UPDATE, NULL);
    TEST_ASSERT_EQUAL(PRESENTER_STATE_PROCESSING, viper_presenter_get_state(presenter));

    /* DATA_READY -> UPDATING_VIEW */
    viper_presenter_process_event(presenter, PRESENTER_EVENT_DATA_READY, NULL);
    TEST_ASSERT_EQUAL(PRESENTER_STATE_UPDATING_VIEW, viper_presenter_get_state(presenter));

    /* ERROR -> ERROR */
    viper_presenter_process_event(presenter, PRESENTER_EVENT_ERROR, "error");
    TEST_ASSERT_EQUAL(PRESENTER_STATE_ERROR, viper_presenter_get_state(presenter));

    viper_presenter_destroy(presenter);
}

/* Test case: get data */
TEST_CASE(test_presenter_get_data)
{
    struct viper_presenter *presenter = viper_presenter_init("TestPresenter", 8, &mock_view, &mock_interactor);
    TEST_ASSERT_NOT_NULL(presenter);

    const struct viper_presenter_data *data = viper_presenter_get_data(presenter);
    TEST_ASSERT_NOT_NULL(data);
    TEST_ASSERT_EQUAL(PRESENTER_STATE_IDLE, data->current_state);
    TEST_ASSERT_NULL(data->presentation_data);
    TEST_ASSERT_EQUAL(0, data->data_size);
    TEST_ASSERT_NULL(data->view_context);
    TEST_ASSERT_NULL(data->interactor_context);
    TEST_ASSERT_EQUAL(0, data->error_code);
    TEST_ASSERT_EQUAL_STRING("", data->error_message);

    viper_presenter_destroy(presenter);
}

/* Test suite runner */
int main(void)
{
    test_pass_count = 0;
    test_fail_count = 0;

    void (*test_cases[])(void) = {
        test_presenter_init_destroy,
        test_presenter_process_view_update,
        test_presenter_process_data_ready,
        test_presenter_process_error,
        test_presenter_update_view,
        test_presenter_request_data,
        test_presenter_state_transitions,
        test_presenter_get_data,
    };

    RUN_TEST_SUITE(VIPER_Presenter, test_cases);
    print_test_summary();

    return test_fail_count == 0 ? 0 : 1;
}
