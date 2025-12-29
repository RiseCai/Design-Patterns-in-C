/**
 * test_scv_router.c - Unit tests for System Coordinator VIPER Router component
 *
 * Tests initialization, destruction, pattern matching, and routing.
 */

#include "test_framework.h"
#include "../include/scv_router.h"
#include "../include/scv_entity.h"
#include "../include/scv_interactor.h"
#include "../include/scv_presenter.h"
#include "../include/scv_view.h"
#include <string.h>
#include <stdlib.h>

/* Mock components for testing */
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

static struct scv_presenter mock_presenter = {
    .entity = &mock_entity,
    .interactor = &mock_interactor,
    .policy = {
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
    },
    .recording_subsystem = {0},
    .communication_subsystem = {0},
    .power_subsystem = {0},
    .audio_subsystem = {0},
    .ota_subsystem = {0},
    .state = PRESENTER_STATE_INIT,
    .subsystem_status_changed_callback = NULL,
    .coordination_event_callback = NULL,
    .error_callback = NULL,
    .user_data = NULL
};

static struct scv_view mock_view = {
    .mealy_fsm = NULL,
    .config = {
        .screen_width = 320,
        .screen_height = 240,
        .screen_refresh_rate_hz = 60,
        .touch_sensitivity = 50,
        .button_debounce_ms = 20,
        .encoder_resolution = 24,
        .enable_visual_feedback = true,
        .enable_audio_feedback = false,
        .enable_haptic_feedback = true,
        .screen_timeout_ms = 30000,
        .input_timeout_ms = 5000,
        .theme_name = "Default",
        .theme_color_primary = 0x007BFF,
        .theme_color_secondary = 0x6C757D,
        .theme_font_size = 12
    },
    .entity = &mock_entity,
    .interactor = &mock_interactor,
    .presenter = &mock_presenter,
    .current_state = VIEW_STATE_INIT,
    .active_screen_id = 0,
    .is_interactive = true,
    .event_queue = NULL,
    .event_queue_size = 0,
    .event_queue_head = 0,
    .event_queue_tail = 0,
    .response_queue = NULL,
    .response_queue_size = 0,
    .response_queue_head = 0,
    .response_queue_tail = 0,
    .ui_event_received_callback = NULL,
    .ui_response_ready_callback = NULL,
    .ui_state_changed_callback = NULL,
    .user_data = NULL
};

/* Dummy callbacks for testing */
static void dummy_route_matched_callback(const struct scv_route_match *match, void *user_data) {
    (*(int *)user_data)++;
}

static void dummy_routing_decision_callback(const struct scv_routing_context *context, void *user_data) {
    (*(int *)user_data)++;
}

static void dummy_route_completed_callback(const struct scv_routing_context *context,
                                           scv_routing_decision_t decision,
                                           void *user_data) {
    (*(int *)user_data)++;
}

/* Test case: initialization and destruction */
TEST_CASE(test_router_init_destroy) {
    struct scv_routing_config config = {
        .max_routing_table_entries = 10,
        .max_pattern_length = 128,
        .enable_pattern_caching = true,
        .enable_route_optimization = true,
        .max_route_hops = 5,
        .route_timeout_ms = 5000,
        .max_pattern_matches = 3,
        .case_sensitive_matching = true,
        .routing_thread_priority = 5,
        .max_concurrent_routes = 2
    };

    struct scv_router *router = scv_router_init(&config, &mock_entity, &mock_interactor, &mock_presenter, &mock_view);
    TEST_ASSERT_NOT_NULL(router);
    TEST_ASSERT_EQUAL_PTR(&mock_entity, router->entity);
    TEST_ASSERT_EQUAL_PTR(&mock_interactor, router->interactor);
    TEST_ASSERT_EQUAL_PTR(&mock_presenter, router->presenter);
    TEST_ASSERT_EQUAL_PTR(&mock_view, router->view);
    TEST_ASSERT_EQUAL(10, router->config.max_routing_table_entries);
    TEST_ASSERT_EQUAL(ROUTER_STATE_INIT, scv_router_get_state(router));

    scv_router_destroy(router);
    /* No crash expected */
}

/* Test case: initialization with NULL parameters */
TEST_CASE(test_router_init_null_params) {
    /* Test with all NULL */
    struct scv_router *router = scv_router_init(NULL, NULL, NULL, NULL, NULL);
    TEST_ASSERT_NOT_NULL(router);
    TEST_ASSERT_NULL(router->entity);
    TEST_ASSERT_NULL(router->interactor);
    TEST_ASSERT_NULL(router->presenter);
    TEST_ASSERT_NULL(router->view);
    TEST_ASSERT_EQUAL(ROUTER_STATE_INIT, scv_router_get_state(router));
    scv_router_destroy(router);
}

/* Test case: add and remove pattern */
TEST_CASE(test_router_add_remove_pattern) {
    struct scv_router *router = scv_router_init(NULL, NULL, NULL, NULL, NULL);
    TEST_ASSERT_NOT_NULL(router);

    struct scv_route_pattern pattern = {
        .pattern = "test.*pattern",
        .type = ROUTE_TYPE_INTERNAL,
        .priority = 10,
        .target = (void *)0x1234
    };

    /* Add pattern */
    int ret = scv_router_add_pattern(router, &pattern);
    TEST_ASSERT_EQUAL(0, ret);
    printf("DEBUG: routing_table_count after add = %u\n", router->routing_table_count);
    TEST_ASSERT_EQUAL(2, router->routing_table_count);

    /* Remove pattern */
    ret = scv_router_remove_pattern(router, &pattern);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(1, router->routing_table_count);

    scv_router_destroy(router);
}

/* Test case: route message */
TEST_CASE(test_router_route_message) {
    struct scv_router *router = scv_router_init(NULL, NULL, NULL, NULL, NULL);
    TEST_ASSERT_NOT_NULL(router);

    /* Add a pattern */
    struct scv_route_pattern pattern = {
        .pattern = "start.*recording",
        .type = ROUTE_TYPE_COMMAND,
        .priority = 10,
        .target = (void *)0x1234
    };
    scv_router_add_pattern(router, &pattern);

    /* Route a matching message */
    const char *message = "start recording now";
    scv_routing_decision_t decision = scv_router_route_message(router, message, strlen(message), NULL);
    /* Should return some decision (could be CONTINUE, STOP, etc.) */
    /* We just verify no crash */
    (void)decision;

    /* Route a non-matching message */
    decision = scv_router_route_message(router, "unknown", 7, NULL);
    /* Should return CONTINUE or DROP */
    (void)decision;

    scv_router_destroy(router);
}

/* Test case: match patterns */
TEST_CASE(test_router_match_patterns) {
    struct scv_router *router = scv_router_init(NULL, NULL, NULL, NULL, NULL);
    TEST_ASSERT_NOT_NULL(router);

    /* Add multiple patterns */
    struct scv_route_pattern patterns[] = {
        {.pattern = "test.*", .type = ROUTE_TYPE_INTERNAL, .priority = 5, .target = (void *)0x1},
        {.pattern = ".*recording.*", .type = ROUTE_TYPE_COMMAND, .priority = 10, .target = (void *)0x2},
        {.pattern = "error.*", .type = ROUTE_TYPE_EVENT, .priority = 3, .target = (void *)0x3},
    };
    for (int i = 0; i < 3; i++) {
        scv_router_add_pattern(router, &patterns[i]);
    }

    struct scv_route_match matches[5];
    const char *message = "test recording error";
    uint32_t num_matches = scv_router_match_patterns(router, message, strlen(message), matches, 5);
    /* Should match at least one pattern */
    TEST_ASSERT(num_matches > 0);

    scv_router_destroy(router);
}

/* Test case: update configuration */
TEST_CASE(test_router_update_config) {
    struct scv_router *router = scv_router_init(NULL, NULL, NULL, NULL, NULL);
    TEST_ASSERT_NOT_NULL(router);

    struct scv_routing_config new_config = {
        .max_routing_table_entries = 20,
        .max_pattern_length = 256,
        .enable_pattern_caching = false,
        .enable_route_optimization = false,
        .max_route_hops = 10,
        .route_timeout_ms = 10000,
        .max_pattern_matches = 5,
        .case_sensitive_matching = false,
        .routing_thread_priority = 10,
        .max_concurrent_routes = 5
    };

    int ret = scv_router_update_config(router, &new_config);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(20, router->config.max_routing_table_entries);
    TEST_ASSERT_EQUAL(false, router->config.enable_pattern_caching);
    TEST_ASSERT_EQUAL(false, router->config.case_sensitive_matching);

    /* Get config and verify */
    const struct scv_routing_config *retrieved = scv_router_get_config(router);
    TEST_ASSERT_NOT_NULL(retrieved);
    TEST_ASSERT_EQUAL(new_config.max_routing_table_entries, retrieved->max_routing_table_entries);

    scv_router_destroy(router);
}

/* Test case: set associated components */
TEST_CASE(test_router_set_components) {
    struct scv_router *router = scv_router_init(NULL, NULL, NULL, NULL, NULL);
    TEST_ASSERT_NOT_NULL(router);

    /* Initially NULL */
    TEST_ASSERT_NULL(router->entity);
    TEST_ASSERT_NULL(router->interactor);
    TEST_ASSERT_NULL(router->presenter);
    TEST_ASSERT_NULL(router->view);

    /* Set components */
    scv_router_set_entity(router, &mock_entity);
    scv_router_set_interactor(router, &mock_interactor);
    scv_router_set_presenter(router, &mock_presenter);
    scv_router_set_view(router, &mock_view);

    TEST_ASSERT_EQUAL_PTR(&mock_entity, router->entity);
    TEST_ASSERT_EQUAL_PTR(&mock_interactor, router->interactor);
    TEST_ASSERT_EQUAL_PTR(&mock_presenter, router->presenter);
    TEST_ASSERT_EQUAL_PTR(&mock_view, router->view);

    scv_router_destroy(router);
}

/* Test case: get statistics */
TEST_CASE(test_router_get_statistics) {
    struct scv_router *router = scv_router_init(NULL, NULL, NULL, NULL, NULL);
    TEST_ASSERT_NOT_NULL(router);
    printf("DEBUG: init routing_table_count = %u\n", router->routing_table_count);

    uint32_t total_entries = 0, active_entries = 0;
    int ret = scv_router_get_statistics(router, &total_entries, &active_entries);
    TEST_ASSERT_EQUAL(0, ret);
    printf("DEBUG: total_entries = %u, active_entries = %u\n", total_entries, active_entries);
    TEST_ASSERT_EQUAL(1, total_entries);
    TEST_ASSERT_EQUAL(1, active_entries);

    /* Add a pattern */
    struct scv_route_pattern pattern = {
        .pattern = "test",
        .type = ROUTE_TYPE_INTERNAL,
        .priority = 1,
        .target = NULL
    };
    int add_ret = scv_router_add_pattern(router, &pattern);
    printf("DEBUG: add_ret = %d, routing_table_count = %u\n", add_ret, router->routing_table_count);

    scv_router_get_statistics(router, &total_entries, &active_entries);
    printf("DEBUG: after add total_entries = %u, active_entries = %u\n", total_entries, active_entries);
    TEST_ASSERT_EQUAL(2, total_entries);
    TEST_ASSERT_EQUAL(2, active_entries);

    scv_router_destroy(router);
}

/* Test case: clear routing table */
TEST_CASE(test_router_clear_table) {
    struct scv_router *router = scv_router_init(NULL, NULL, NULL, NULL, NULL);
    TEST_ASSERT_NOT_NULL(router);

    /* Add some patterns */
    struct scv_route_pattern patterns[] = {
        {.pattern = "pattern1", .type = ROUTE_TYPE_INTERNAL, .priority = 1, .target = NULL},
        {.pattern = "pattern2", .type = ROUTE_TYPE_INTERNAL, .priority = 2, .target = NULL},
        {.pattern = "pattern3", .type = ROUTE_TYPE_INTERNAL, .priority = 3, .target = NULL},
    };
    for (int i = 0; i < 3; i++) {
        scv_router_add_pattern(router, &patterns[i]);
    }
    TEST_ASSERT_EQUAL(4, router->routing_table_count);

    /* Clear table */
    int ret = scv_router_clear_table(router);
    TEST_ASSERT_EQUAL(0, ret);
    TEST_ASSERT_EQUAL(0, router->routing_table_count);

    scv_router_destroy(router);
}

/* Test case: set callbacks */
TEST_CASE(test_router_set_callbacks) {
    struct scv_router *router = scv_router_init(NULL, NULL, NULL, NULL, NULL);
    TEST_ASSERT_NOT_NULL(router);

    int callback1_called = 0;
    int callback2_called = 0;
    int callback3_called = 0;

    /* Set callbacks */
    scv_router_set_route_matched_callback(router, dummy_route_matched_callback, &callback1_called);
    scv_router_set_routing_decision_callback(router, dummy_routing_decision_callback, &callback2_called);
    scv_router_set_route_completed_callback(router, dummy_route_completed_callback, &callback3_called);

    /* Simulate a route match (this would normally be triggered internally) */
    if (router->route_matched_callback) {
        struct scv_route_match match = {0};
        router->route_matched_callback(&match, router->user_data);
    }
    TEST_ASSERT_EQUAL(1, callback1_called);

    scv_router_destroy(router);
}

/* Test suite runner */
int main(void)
{
    test_pass_count = 0;
    test_fail_count = 0;

    void (*test_cases[])(void) = {
        test_router_init_destroy,
        test_router_init_null_params,
        test_router_add_remove_pattern,
        test_router_route_message,
        test_router_match_patterns,
        test_router_update_config,
        test_router_set_components,
        test_router_get_statistics,
        test_router_clear_table,
        test_router_set_callbacks,
    };

    RUN_TEST_SUITE(SCV_Router, test_cases);
    print_test_summary();

    return test_fail_count == 0 ? 0 : 1;
}
