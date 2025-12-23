/**
 * test_viper_module.c - Unit tests for VIPER module
 */

#include "test_framework.h"
#include "../include/viper.h"
#include <stdlib.h>
#include <string.h>

TEST_CASE(test_viper_module_init_basic) {
    struct viper_config config = {
        .enable_view = 1,
        .enable_presenter = 1,
        .enable_interactor = 1,
        .enable_entity = 1,
        .enable_router = 1,
        .max_events = 10
    };
    strcpy(config.module_name, "TestModule");

    struct viper_module *module = viper_module_init(&config);
    TEST_ASSERT_NOT_NULL(module);
    TEST_ASSERT_EQUAL_STRING("TestModule", module->module_name);
    TEST_ASSERT(module->module_id >= 1000 && module->module_id <= 9999);
    TEST_ASSERT_NOT_NULL(module->view);
    TEST_ASSERT_NOT_NULL(module->presenter);
    TEST_ASSERT_NOT_NULL(module->interactor);
    TEST_ASSERT_NOT_NULL(module->entity);
    TEST_ASSERT_NOT_NULL(module->router);

    viper_module_destroy(module);
}

TEST_CASE(test_viper_module_init_partial) {
    struct viper_config config = {
        .enable_view = 1,
        .enable_presenter = 0,
        .enable_interactor = 1,
        .enable_entity = 0,
        .enable_router = 0,
        .max_events = 5
    };
    strcpy(config.module_name, "PartialModule");

    struct viper_module *module = viper_module_init(&config);
    TEST_ASSERT_NOT_NULL(module);
    TEST_ASSERT_NOT_NULL(module->view);
    TEST_ASSERT_NULL(module->presenter);
    TEST_ASSERT_NOT_NULL(module->interactor);
    TEST_ASSERT_NULL(module->entity);
    TEST_ASSERT_NULL(module->router);

    viper_module_destroy(module);
}

TEST_CASE(test_viper_module_init_null_config) {
    struct viper_module *module = viper_module_init(NULL);
    TEST_ASSERT_NULL(module);
}

TEST_CASE(test_viper_module_destroy_null) {
    /* Should not crash */
    viper_module_destroy(NULL);
}

TEST_CASE(test_viper_event_create_destroy) {
    int data = 42;
    struct viper_event *event = viper_event_create(
        VIPER_EVENT_DATA_UPDATE,
        &data,
        sizeof(data),
        VIPER_COMPONENT_ENTITY,
        VIPER_COMPONENT_PRESENTER
    );
    TEST_ASSERT_NOT_NULL(event);
    TEST_ASSERT_EQUAL(VIPER_EVENT_DATA_UPDATE, event->type);
    TEST_ASSERT_EQUAL(VIPER_COMPONENT_ENTITY, event->source_component);
    TEST_ASSERT_EQUAL(VIPER_COMPONENT_PRESENTER, event->target_component);
    TEST_ASSERT_EQUAL(sizeof(data), event->data_size);
    TEST_ASSERT_NOT_NULL(event->data);
    TEST_ASSERT_EQUAL(42, *(int*)event->data);

    viper_event_destroy(event);
}

TEST_CASE(test_viper_event_create_no_data) {
    struct viper_event *event = viper_event_create(
        VIPER_EVENT_VIEW_ACTION,
        NULL,
        0,
        VIPER_COMPONENT_VIEW,
        VIPER_COMPONENT_INTERACTOR
    );
    TEST_ASSERT_NOT_NULL(event);
    TEST_ASSERT_EQUAL(VIPER_EVENT_VIEW_ACTION, event->type);
    TEST_ASSERT_NULL(event->data);
    TEST_ASSERT_EQUAL(0, event->data_size);

    viper_event_destroy(event);
}

TEST_CASE(test_viper_module_process_event) {
    struct viper_config config = {
        .enable_view = 1,
        .enable_presenter = 1,
        .enable_interactor = 1,
        .enable_entity = 1,
        .enable_router = 1,
        .max_events = 10
    };
    strcpy(config.module_name, "ProcessTest");

    struct viper_module *module = viper_module_init(&config);
    TEST_ASSERT_NOT_NULL(module);

    /* Create an event targeting presenter */
    struct viper_event *event = viper_event_create(
        VIPER_EVENT_PRESENTATION,
        NULL,
        0,
        VIPER_COMPONENT_VIEW,
        VIPER_COMPONENT_PRESENTER
    );
    TEST_ASSERT_NOT_NULL(event);

    int result = viper_module_process_event(module, event);
    /* Currently viper_module_process_event returns 0 for success, -1 for error */
    TEST_ASSERT_EQUAL(0, result);

    viper_event_destroy(event);
    viper_module_destroy(module);
}

TEST_CASE(test_viper_module_process_event_invalid_target) {
    struct viper_config config = {
        .enable_view = 1,
        .enable_presenter = 0,
        .enable_interactor = 0,
        .enable_entity = 0,
        .enable_router = 0,
        .max_events = 10
    };
    strcpy(config.module_name, "InvalidTargetTest");

    struct viper_module *module = viper_module_init(&config);
    TEST_ASSERT_NOT_NULL(module);

    struct viper_event *event = viper_event_create(
        VIPER_EVENT_VIEW_ACTION,
        NULL,
        0,
        VIPER_COMPONENT_VIEW,
        VIPER_COMPONENT_PRESENTER  /* presenter not enabled */
    );
    TEST_ASSERT_NOT_NULL(event);

    int result = viper_module_process_event(module, event);
    TEST_ASSERT_EQUAL(-1, result);  /* Should fail because presenter not enabled */

    viper_event_destroy(event);
    viper_module_destroy(module);
}

TEST_CASE(test_viper_module_get_status) {
    struct viper_config config = {
        .enable_view = 1,
        .enable_presenter = 1,
        .enable_interactor = 0,
        .enable_entity = 1,
        .enable_router = 0,
        .max_events = 5
    };
    strcpy(config.module_name, "StatusTest");

    struct viper_module *module = viper_module_init(&config);
    TEST_ASSERT_NOT_NULL(module);

    const char *status = viper_module_get_status(module);
    TEST_ASSERT_NOT_NULL(status);
    /* Check that status contains module name */
    TEST_ASSERT(strstr(status, "StatusTest") != NULL);
    /* Check that status contains component info */
    TEST_ASSERT(strstr(status, "View") != NULL);
    TEST_ASSERT(strstr(status, "Presenter") != NULL);
    TEST_ASSERT(strstr(status, "Entity") != NULL);

    viper_module_destroy(module);
}

TEST_CASE(test_viper_event_create_large_data) {
    /* Create a large data block (1KB) */
    size_t large_size = 1024;
    char *large_data = (char *)malloc(large_size);
    TEST_ASSERT_NOT_NULL(large_data);
    memset(large_data, 0xAB, large_size);

    struct viper_event *event = viper_event_create(
        VIPER_EVENT_DATA_UPDATE,
        large_data,
        large_size,
        VIPER_COMPONENT_ENTITY,
        VIPER_COMPONENT_PRESENTER
    );
    TEST_ASSERT_NOT_NULL(event);
    TEST_ASSERT_EQUAL(large_size, event->data_size);
    TEST_ASSERT_NOT_NULL(event->data);
    TEST_ASSERT(memcmp(large_data, event->data, large_size) == 0);

    viper_event_destroy(event);
    free(large_data);
}

TEST_CASE(test_viper_event_create_null_data_with_size) {
    /* data is NULL but data_size > 0 should still create event with NULL data */
    struct viper_event *event = viper_event_create(
        VIPER_EVENT_VIEW_ACTION,
        NULL,
        100,  /* size > 0 but data is NULL */
        VIPER_COMPONENT_VIEW,
        VIPER_COMPONENT_INTERACTOR
    );
    TEST_ASSERT_NOT_NULL(event);
    TEST_ASSERT_NULL(event->data);
    TEST_ASSERT_EQUAL(0, event->data_size); /* implementation sets data_size to 0 when data is NULL */
    viper_event_destroy(event);
}

TEST_CASE(test_viper_module_process_event_null_module) {
    struct viper_event *event = viper_event_create(
        VIPER_EVENT_VIEW_ACTION,
        NULL,
        0,
        VIPER_COMPONENT_VIEW,
        VIPER_COMPONENT_INTERACTOR
    );
    TEST_ASSERT_NOT_NULL(event);
    int result = viper_module_process_event(NULL, event);
    TEST_ASSERT_EQUAL(-1, result);
    viper_event_destroy(event);
}

TEST_CASE(test_viper_module_process_event_null_event) {
    struct viper_config config = {
        .enable_view = 1,
        .enable_presenter = 0,
        .enable_interactor = 0,
        .enable_entity = 0,
        .enable_router = 0,
        .max_events = 10
    };
    strcpy(config.module_name, "NullEventTest");
    struct viper_module *module = viper_module_init(&config);
    TEST_ASSERT_NOT_NULL(module);
    int result = viper_module_process_event(module, NULL);
    TEST_ASSERT_EQUAL(-1, result);
    viper_module_destroy(module);
}

/* New test cases for boundary conditions and concurrency */

TEST_CASE(test_viper_module_init_max_events_zero) {
    struct viper_config config = {
        .enable_view = 1,
        .enable_presenter = 0,
        .enable_interactor = 0,
        .enable_entity = 0,
        .enable_router = 0,
        .max_events = 0  /* zero max_events should still work */
    };
    strcpy(config.module_name, "ZeroMaxEvents");
    struct viper_module *module = viper_module_init(&config);
    TEST_ASSERT_NOT_NULL(module);
    TEST_ASSERT_NOT_NULL(module->view);
    viper_module_destroy(module);
}

TEST_CASE(test_viper_module_init_max_events_negative) {
    struct viper_config config = {
        .enable_view = 1,
        .enable_presenter = 0,
        .enable_interactor = 0,
        .enable_entity = 0,
        .enable_router = 0,
        .max_events = -5  /* negative max_events should still work (unused) */
    };
    strcpy(config.module_name, "NegativeMaxEvents");
    struct viper_module *module = viper_module_init(&config);
    TEST_ASSERT_NOT_NULL(module);
    TEST_ASSERT_NOT_NULL(module->view);
    viper_module_destroy(module);
}

TEST_CASE(test_viper_event_create_huge_data) {
    /* Create a 1MB data block */
    size_t huge_size = 1024 * 1024; /* 1 MB */
    char *huge_data = (char *)malloc(huge_size);
    TEST_ASSERT_NOT_NULL(huge_data);
    memset(huge_data, 0xCD, huge_size);

    struct viper_event *event = viper_event_create(
        VIPER_EVENT_DATA_UPDATE,
        huge_data,
        huge_size,
        VIPER_COMPONENT_ENTITY,
        VIPER_COMPONENT_PRESENTER
    );
    TEST_ASSERT_NOT_NULL(event);
    TEST_ASSERT_EQUAL(huge_size, event->data_size);
    TEST_ASSERT_NOT_NULL(event->data);
    TEST_ASSERT(memcmp(huge_data, event->data, huge_size) == 0);

    viper_event_destroy(event);
    free(huge_data);
}

TEST_CASE(test_viper_module_process_event_concurrent_simulation) {
    /* Simulate concurrent event processing by calling process_event multiple times
       from a single thread (since we don't have threading in unit tests).
       This tests that the module can handle multiple events sequentially without crashing. */
    struct viper_config config = {
        .enable_view = 1,
        .enable_presenter = 1,
        .enable_interactor = 1,
        .enable_entity = 1,
        .enable_router = 1,
        .max_events = 100
    };
    strcpy(config.module_name, "ConcurrentSim");
    struct viper_module *module = viper_module_init(&config);
    TEST_ASSERT_NOT_NULL(module);

    for (int i = 0; i < 50; i++) {
        char data[32];
        sprintf(data, "Event %d", i);
        struct viper_event *event = viper_event_create(
            VIPER_EVENT_VIEW_ACTION,
            data,
            strlen(data) + 1,
            VIPER_COMPONENT_VIEW,
            VIPER_COMPONENT_PRESENTER
        );
        TEST_ASSERT_NOT_NULL(event);
        int result = viper_module_process_event(module, event);
        /* Some events may fail due to mapping, but at least they shouldn't crash */
        (void)result; /* ignore result for this test */
        viper_event_destroy(event);
    }

    viper_module_destroy(module);
}

TEST_CASE(test_viper_module_get_status_null) {
    const char *status = viper_module_get_status(NULL);
    TEST_ASSERT_NOT_NULL(status);
    TEST_ASSERT_EQUAL_STRING("NULL module", status);
}

TEST_CASE(test_viper_module_process_event_invalid_event_type) {
    struct viper_config config = {
        .enable_view = 1,
        .enable_presenter = 1,
        .enable_interactor = 1,
        .enable_entity = 1,
        .enable_router = 1,
        .max_events = 10
    };
    strcpy(config.module_name, "InvalidEventType");
    struct viper_module *module = viper_module_init(&config);
    TEST_ASSERT_NOT_NULL(module);

    /* Create an event with an out-of-range event type (assuming enum values) */
    struct viper_event *event = viper_event_create(
        (viper_event_type_t)999,  /* invalid type */
        NULL,
        0,
        VIPER_COMPONENT_VIEW,
        VIPER_COMPONENT_PRESENTER
    );
    TEST_ASSERT_NOT_NULL(event);
    int result = viper_module_process_event(module, event);
    /* Should return -1 because mapping fails */
    TEST_ASSERT_EQUAL(-1, result);

    viper_event_destroy(event);
    viper_module_destroy(module);
}

/* Test suite array */
void (*viper_module_tests[])(void) = {
    test_viper_module_init_basic,
    test_viper_module_init_partial,
    test_viper_module_init_null_config,
    test_viper_module_destroy_null,
    test_viper_event_create_destroy,
    test_viper_event_create_no_data,
    test_viper_module_process_event,
    test_viper_module_process_event_invalid_target,
    test_viper_module_get_status,
    test_viper_event_create_large_data,
    test_viper_event_create_null_data_with_size,
    test_viper_module_process_event_null_module,
    test_viper_module_process_event_null_event,
    test_viper_module_init_max_events_zero,
    test_viper_module_init_max_events_negative,
    test_viper_event_create_huge_data,
    test_viper_module_process_event_concurrent_simulation,
    test_viper_module_get_status_null,
    test_viper_module_process_event_invalid_event_type,
};

int main(void) {
    printf("VIPER Module Unit Tests\n");
    printf("=======================\n");

    RUN_TEST_SUITE(VIPER_Module, viper_module_tests);
    print_test_summary();

    return test_fail_count > 0 ? 1 : 0;
}
