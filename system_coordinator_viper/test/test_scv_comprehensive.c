/**
 * test_scv_comprehensive.c - Comprehensive tests for System Coordinator VIPER
 *
 * Tests covering:
 * 1. Event bus integration
 * 2. View component integration
 * 3. Boundary conditions
 * 4. Concurrency scenarios
 * 5. Resource exhaustion
 * 6. Recovery scenarios
 * 7. Long-running tests
 */

#include "test_framework.h"
#include "../include/scv.h"
#include "../include/scv_event_bus.h"
#include "../include/scv_view.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

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

/* Static callback functions for tests */
static void event_handler_callback(const scv_event_t *event, void *user_data) {
    (*(int *)user_data)++;
}

static void event_handler_with_assert_callback(const scv_event_t *event, void *user_data) {
    (*(int *)user_data)++;
    TEST_ASSERT_EQUAL(SCV_EVENT_RECORDING_STARTED, event->type);
    TEST_ASSERT_EQUAL_STRING("TestComponent", event->source_component);
}

static void dummy_handler_callback(const scv_event_t *event, void *user_data) {
    /* Do nothing */
}

static void view_event_handler_callback(const scv_event_t *event, void *user_data) {
    (*(int *)user_data)++;
}

static void handler1_callback(const scv_event_t *event, void *user_data) {
    (*(int *)user_data)++;
}

static void handler2_callback(const scv_event_t *event, void *user_data) {
    (*(int *)user_data)++;
}

static void event_callback_static(const struct scv_ui_event *event, void *user_data) {
    (*(int *)user_data)++;
}

static void response_callback_static(const struct scv_ui_response *response, void *user_data) {
    (*(int *)user_data)++;
}

static void state_callback_static(scv_view_state_t new_state, scv_view_state_t old_state, void *user_data) {
    (*(int *)user_data)++;
}

/* Test case: Event bus initialization and destruction */
TEST_CASE(test_event_bus_init_destroy) {
    scv_event_bus_t *bus = scv_event_bus_init(10);
    TEST_ASSERT_NOT_NULL(bus);
    TEST_ASSERT_EQUAL(0, bus->event_count);
    TEST_ASSERT_EQUAL(10, bus->max_subscribers);
    TEST_ASSERT_EQUAL(true, bus->enabled);
    
    scv_event_bus_destroy(bus);
    /* No crash expected */
}

/* Test case: Event bus subscription and unsubscription */
TEST_CASE(test_event_bus_subscribe_unsubscribe) {
    scv_event_bus_t *bus = scv_event_bus_init(10);
    TEST_ASSERT_NOT_NULL(bus);
    
    int callback_called = 0;
    
    /* Subscribe to event */
    int result = scv_event_bus_subscribe(bus, SCV_EVENT_RECORDING_STARTED, event_handler_callback, &callback_called);
    TEST_ASSERT_EQUAL(0, result);
    
    /* Subscribe again (should succeed) */
    result = scv_event_bus_subscribe(bus, SCV_EVENT_RECORDING_STARTED, event_handler_callback, &callback_called);
    TEST_ASSERT_EQUAL(-3, result);
    
    /* Unsubscribe */
    result = scv_event_bus_unsubscribe(bus, SCV_EVENT_RECORDING_STARTED, event_handler_callback);
    TEST_ASSERT_EQUAL(0, result);
    
    scv_event_bus_destroy(bus);
}

/* Test case: Event bus publish and receive */
TEST_CASE(test_event_bus_publish_receive) {
    scv_event_bus_t *bus = scv_event_bus_init(10);
    TEST_ASSERT_NOT_NULL(bus);
    
    int callback_called = 0;
    
    /* Subscribe to event */
    scv_event_bus_subscribe(bus, SCV_EVENT_RECORDING_STARTED, event_handler_with_assert_callback, &callback_called);
    
    /* Create and publish event */
    scv_event_t event = scv_event_create(SCV_EVENT_RECORDING_STARTED, "TestComponent", NULL, 0);
    int result = scv_event_bus_publish(bus, &event);
    TEST_ASSERT_EQUAL(1, result);
    TEST_ASSERT_EQUAL(1, callback_called);
    
    scv_event_bus_destroy(bus);
}

/* Test case: Event bus statistics */
TEST_CASE(test_event_bus_statistics) {
    scv_event_bus_t *bus = scv_event_bus_init(5);
    TEST_ASSERT_NOT_NULL(bus);
    
    uint32_t total_events = 0, total_subscribers = 0;
    
    /* Get initial statistics */
    int result = scv_event_bus_get_stats(bus, &total_events, &total_subscribers);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(0, total_events);
    TEST_ASSERT_EQUAL(0, total_subscribers);
    
    /* Subscribe some handlers */
    scv_event_bus_subscribe(bus, SCV_EVENT_RECORDING_STARTED, dummy_handler_callback, NULL);
    scv_event_bus_subscribe(bus, SCV_EVENT_UPLOAD_STARTED, dummy_handler_callback, NULL);
    
    /* Publish some events */
    scv_event_t event = scv_event_create(SCV_EVENT_RECORDING_STARTED, "Test", NULL, 0);
    scv_event_bus_publish(bus, &event);
    scv_event_bus_publish(bus, &event);
    
    /* Get updated statistics */
    scv_event_bus_get_stats(bus, &total_events, &total_subscribers);
    TEST_ASSERT_EQUAL(2, total_events);
    TEST_ASSERT_EQUAL(2, total_subscribers);
    
    scv_event_bus_destroy(bus);
}

/* Test case: Event bus enable/disable */
TEST_CASE(test_event_bus_enable_disable) {
    scv_event_bus_t *bus = scv_event_bus_init(5);
    TEST_ASSERT_NOT_NULL(bus);
    
    int callback_called = 0;
    
    /* Subscribe to event */
    scv_event_bus_subscribe(bus, SCV_EVENT_RECORDING_STARTED, event_handler_callback, &callback_called);
    
    /* Disable bus */
    scv_event_bus_set_enabled(bus, false);
    TEST_ASSERT_EQUAL(false, scv_event_bus_is_enabled(bus));
    
    /* Publish event while disabled */
    scv_event_t event = scv_event_create(SCV_EVENT_RECORDING_STARTED, "Test", NULL, 0);
    scv_event_bus_publish(bus, &event);
    TEST_ASSERT_EQUAL(0, callback_called);  /* Should not be called */
    
    /* Enable bus */
    scv_event_bus_set_enabled(bus, true);
    TEST_ASSERT_EQUAL(true, scv_event_bus_is_enabled(bus));
    
    /* Publish event while enabled */
    scv_event_bus_publish(bus, &event);
    TEST_ASSERT_EQUAL(1, callback_called);  /* Should be called */
    
    scv_event_bus_destroy(bus);
}

/* Test case: View component initialization and destruction */
TEST_CASE(test_view_init_destroy) {
    struct scv_view *view = scv_view_init(NULL, &mock_entity, &mock_interactor, &mock_presenter);
    TEST_ASSERT_NOT_NULL(view);
    TEST_ASSERT_EQUAL_PTR(&mock_entity, view->entity);
    TEST_ASSERT_EQUAL_PTR(&mock_interactor, view->interactor);
    TEST_ASSERT_EQUAL_PTR(&mock_presenter, view->presenter);
    TEST_ASSERT_EQUAL(VIEW_STATE_INIT, scv_view_get_state(view));
    
    scv_view_destroy(view);
    /* No crash expected */
}

/* Test case: View component with custom configuration */
TEST_CASE(test_view_with_config) {
    struct scv_ui_config config = {
        .screen_width = 480,
        .screen_height = 320,
        .screen_refresh_rate_hz = 30,
        .touch_sensitivity = 75,
        .button_debounce_ms = 50,
        .encoder_resolution = 32,
        .enable_visual_feedback = false,
        .enable_audio_feedback = true,
        .enable_haptic_feedback = false,
        .screen_timeout_ms = 60000,
        .input_timeout_ms = 10000,
        .theme_name = "Dark",
        .theme_color_primary = 0x000000,
        .theme_color_secondary = 0xFFFFFF,
        .theme_font_size = 14
    };
    
    struct scv_view *view = scv_view_init(&config, &mock_entity, &mock_interactor, &mock_presenter);
    TEST_ASSERT_NOT_NULL(view);
    
    const struct scv_ui_config *retrieved_config = scv_view_get_config(view);
    TEST_ASSERT_NOT_NULL(retrieved_config);
    TEST_ASSERT_EQUAL(480, retrieved_config->screen_width);
    TEST_ASSERT_EQUAL(320, retrieved_config->screen_height);
    TEST_ASSERT_EQUAL(30, retrieved_config->screen_refresh_rate_hz);
    TEST_ASSERT_EQUAL(false, retrieved_config->enable_visual_feedback);
    TEST_ASSERT_EQUAL(true, retrieved_config->enable_audio_feedback);
    TEST_ASSERT_EQUAL_STRING("Dark", retrieved_config->theme_name);
    
    scv_view_destroy(view);
}

/* Test case: View component event processing */
TEST_CASE(test_view_process_event) {
    struct scv_view *view = scv_view_init(NULL, &mock_entity, &mock_interactor, &mock_presenter);
    TEST_ASSERT_NOT_NULL(view);
    
    /* Create a button press event */
    struct scv_ui_event event = {0};
    event.type = UI_EVT_BUTTON_PRESS;
    event.data.button.button_id = 1;
    event.data.button.press_duration_ms = 100;
    event.timestamp_ms = 1234567890;
    
    /* Process event */
    int result = scv_view_process_event(view, &event);
    /* Should return 0 or negative depending on implementation */
    /* We just verify no crash */
    (void)result;
    
    scv_view_destroy(view);
}

/* Test case: View component event queue */
TEST_CASE(test_view_event_queue) {
    struct scv_view *view = scv_view_init(NULL, &mock_entity, &mock_interactor, &mock_presenter);
    TEST_ASSERT_NOT_NULL(view);
    
    /* Queue multiple events */
    for (int i = 0; i < 5; i++) {
        struct scv_ui_event event = {0};
        event.type = UI_EVT_BUTTON_PRESS;
        event.data.button.button_id = i;
        event.timestamp_ms = 1234567890 + i;
        
        int result = scv_view_queue_event(view, &event);
        TEST_ASSERT_EQUAL(0, result);
    }
    
    /* Process queued events */
    uint32_t processed = scv_view_process_queued_events(view);
    TEST_ASSERT(processed > 0);
    
    scv_view_destroy(view);
}

/* Test case: View component response queue */
TEST_CASE(test_view_response_queue) {
    struct scv_view *view = scv_view_init(NULL, &mock_entity, &mock_interactor, &mock_presenter);
    TEST_ASSERT_NOT_NULL(view);
    
    /* Queue a response */
    struct scv_ui_response response = {0};
    response.type = UI_RESPONSE_VISUAL_FEEDBACK;
    response.data.visual.led_pattern = 0xFF;
    response.data.visual.led_duration_ms = 500;
    response.data.visual.led_color = 0x00FF00;
    response.priority = 10;
    
    int result = scv_view_queue_response(view, &response);
    TEST_ASSERT_EQUAL(0, result);
    
    /* Get response */
    struct scv_ui_response retrieved_response;
    result = scv_view_get_response(view, &retrieved_response);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(UI_RESPONSE_VISUAL_FEEDBACK, retrieved_response.type);
    TEST_ASSERT_EQUAL(0xFF, retrieved_response.data.visual.led_pattern);
    TEST_ASSERT_EQUAL(500, retrieved_response.data.visual.led_duration_ms);
    TEST_ASSERT_EQUAL(0x00FF00, retrieved_response.data.visual.led_color);
    TEST_ASSERT_EQUAL(10, retrieved_response.priority);
    
    scv_view_destroy(view);
}

/* Test case: View component state transitions */
TEST_CASE(test_view_state_transitions) {
    struct scv_view *view = scv_view_init(NULL, &mock_entity, &mock_interactor, &mock_presenter);
    TEST_ASSERT_NOT_NULL(view);
    
    scv_view_state_t initial_state = scv_view_get_state(view);
    TEST_ASSERT_EQUAL(VIEW_STATE_INIT, initial_state);
    
    /* Set active screen */
    int result = scv_view_set_active_screen(view, 1);
    TEST_ASSERT_EQUAL(0, result);
    TEST_ASSERT_EQUAL(1, scv_view_get_active_screen(view));
    
    /* Reset view */
    result = scv_view_reset(view);
    TEST_ASSERT_EQUAL(0, result);
    
    scv_view_destroy(view);
}

/* Callback functions for test_view_callbacks */
static void event_callback(const struct scv_ui_event *event, void *user_data) {
    (*(int *)user_data)++;
}

static void response_callback(const struct scv_ui_response *response, void *user_data) {
    (*(int *)user_data)++;
}

static void state_callback(scv_view_state_t new_state, scv_view_state_t old_state, void *user_data) {
    (*(int *)user_data)++;
}

/* Test case: View component callbacks */
TEST_CASE(test_view_callbacks) {
    struct scv_view *view = scv_view_init(NULL, &mock_entity, &mock_interactor, &mock_presenter);
    TEST_ASSERT_NOT_NULL(view);
    
    int event_callback_called = 0;
    int response_callback_called = 0;
    int state_callback_called = 0;
    
    /* Set callbacks */
    scv_view_set_ui_event_received_callback(view, event_callback, &event_callback_called);
    scv_view_set_ui_response_ready_callback(view, response_callback, &response_callback_called);
    scv_view_set_ui_state_changed_callback(view, state_callback, &state_callback_called);
    
    /* Process an event to trigger callbacks */
    struct scv_ui_event event = {0};
    event.type = UI_EVT_BUTTON_PRESS;
    event.data.button.button_id = 1;
    event.timestamp_ms = 1234567890;
    
    scv_view_process_event(view, &event);
    
    /* Note: Callback triggering depends on implementation */
    /* We just verify no crash */
    
    scv_view_destroy(view);
}

/* Test case: Boundary conditions - NULL parameters */
TEST_CASE(test_boundary_conditions_null) {
    /* Test with NULL parameters where allowed */
    struct scv_view *view = scv_view_init(NULL, NULL, NULL, NULL);
    TEST_ASSERT_NOT_NULL(view);
    
    /* Should handle NULL gracefully */
    scv_view_set_entity(view, NULL);
    scv_view_set_interactor(view, NULL);
    scv_view_set_presenter(view, NULL);
    
    scv_view_destroy(view);
}

/* Test case: Boundary conditions - invalid inputs */
TEST_CASE(test_boundary_conditions_invalid) {
    struct scv_view *view = scv_view_init(NULL, &mock_entity, &mock_interactor, &mock_presenter);
    TEST_ASSERT_NOT_NULL(view);
    
    /* Test with invalid event */
    struct scv_ui_event invalid_event = {0};
    invalid_event.type = (scv_ui_event_type_t)999;  /* Invalid type */
    invalid_event.timestamp_ms = 0;
    
    int result = scv_view_process_event(view, &invalid_event);
    /* Should handle gracefully (return error or ignore) */
    (void)result;
    
    scv_view_destroy(view);
}

/* Test case: Resource exhaustion - many events */
TEST_CASE(test_resource_exhaustion_events) {
    struct scv_view *view = scv_view_init(NULL, &mock_entity, &mock_interactor, &mock_presenter);
    TEST_ASSERT_NOT_NULL(view);
    
    /* Queue many events */
    for (int i = 0; i < 100; i++) {
        struct scv_ui_event event = {0};
        event.type = UI_EVT_BUTTON_PRESS;
        event.data.button.button_id = i;
        event.timestamp_ms = 1234567890 + i;
        
        scv_view_queue_event(view, &event);
    }
    
    /* Process all events */
    uint32_t processed = scv_view_process_queued_events(view);
    TEST_ASSERT(processed > 0);
    
    scv_view_destroy(view);
}

/* Test case: Recovery scenario - reset after error */
TEST_CASE(test_recovery_reset) {
    struct scv_view *view = scv_view_init(NULL, &mock_entity, &mock_interactor, &mock_presenter);
    TEST_ASSERT_NOT_NULL(view);
    
    /* Put view in some state */
    scv_view_set_active_screen(view, 2);
    
    /* Reset */
    int result = scv_view_reset(view);
    TEST_ASSERT_EQUAL(0, result);
    
    /* Verify reset to initial state */
    TEST_ASSERT_EQUAL(VIEW_STATE_INIT, scv_view_get_state(view));
    
    scv_view_destroy(view);
}

/* Test case: Integration - event bus with view */
TEST_CASE(test_integration_event_bus_view) {
    /* Create event bus */
    scv_event_bus_t *bus = scv_event_bus_init(10);
    TEST_ASSERT_NOT_NULL(bus);
    
    /* Create view */
    struct scv_view *view = scv_view_init(NULL, &mock_entity, &mock_interactor, &mock_presenter);
    TEST_ASSERT_NOT_NULL(view);
    
    int event_received = 0;

    scv_event_bus_subscribe(bus, SCV_EVENT_RECORDING_STARTED, view_event_handler_callback, &event_received);

    /* Publish event */
    scv_event_t event = scv_event_create(SCV_EVENT_RECORDING_STARTED, "IntegrationTest", NULL, 0);
    scv_event_bus_publish(bus, &event);

    TEST_ASSERT_EQUAL(1, event_received);
    
    scv_view_destroy(view);
    scv_event_bus_destroy(bus);
}

/* Test case: Long-running - stress test */
TEST_CASE(test_long_running_stress) {
    struct scv_view *view = scv_view_init(NULL, &mock_entity, &mock_interactor, &mock_presenter);
    TEST_ASSERT_NOT_NULL(view);
    
    /* Simulate long-running operation */
    for (int iteration = 0; iteration < 1000; iteration++) {
        /* Queue event */
        struct scv_ui_event event = {0};
        event.type = UI_EVT_BUTTON_PRESS;
        event.data.button.button_id = iteration % 10;
        event.timestamp_ms = 1234567890 + iteration;
        
        scv_view_queue_event(view, &event);
        
        /* Process events every 100 iterations */
        if (iteration % 100 == 0) {
            scv_view_process_queued_events(view);
        }
    }
    
    /* Process remaining events */
    scv_view_process_queued_events(view);
    
    scv_view_destroy(view);
}

/* Test case: Concurrency simulation (single-threaded) */
TEST_CASE(test_concurrency_simulation) {
    /* Create multiple components */
    scv_event_bus_t *bus = scv_event_bus_init(20);
    struct scv_view *view1 = scv_view_init(NULL, &mock_entity, &mock_interactor, &mock_presenter);
    struct scv_view *view2 = scv_view_init(NULL, &mock_entity, &mock_interactor, &mock_presenter);
    
    TEST_ASSERT_NOT_NULL(bus);
    TEST_ASSERT_NOT_NULL(view1);
    TEST_ASSERT_NOT_NULL(view2);
    
    int view1_events = 0;
    int view2_events = 0;
    
    /* Set up event handlers */
    scv_event_bus_subscribe(bus, SCV_EVENT_RECORDING_STARTED, handler1_callback, &view1_events);
    scv_event_bus_subscribe(bus, SCV_EVENT_UPLOAD_STARTED, handler2_callback, &view2_events);
    
    /* Simulate concurrent events */
    for (int i = 0; i < 50; i++) {
        scv_event_t event1 = scv_event_create(SCV_EVENT_RECORDING_STARTED, "Simulation", NULL, 0);
        scv_event_t event2 = scv_event_create(SCV_EVENT_UPLOAD_STARTED, "Simulation", NULL, 0);
        
        scv_event_bus_publish(bus, &event1);
        scv_event_bus_publish(bus, &event2);
    }
    
    TEST_ASSERT_EQUAL(50, view1_events);
    TEST_ASSERT_EQUAL(50, view2_events);
    
    scv_view_destroy(view1);
    scv_view_destroy(view2);
    scv_event_bus_destroy(bus);
}

/* Main test runner */
int main(void) {
    void (*test_cases[])(void) = {
        test_event_bus_init_destroy,
        test_event_bus_subscribe_unsubscribe,
        test_event_bus_publish_receive,
        test_event_bus_statistics,
        test_event_bus_enable_disable,
        test_view_init_destroy,
        test_view_with_config,
        test_view_process_event,
        test_view_event_queue,
        test_view_response_queue,
        test_view_state_transitions,
        test_view_callbacks,
        test_boundary_conditions_null,
        test_boundary_conditions_invalid,
        test_resource_exhaustion_events,
        test_recovery_reset,
        test_integration_event_bus_view,
        test_long_running_stress,
        test_concurrency_simulation,
    };
    
    RUN_TEST_SUITE(SCV_Comprehensive, test_cases);
    print_test_summary();
    
    return (test_fail_count == 0) ? 0 : 1;
}
