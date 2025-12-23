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
};

int main(void) {
    printf("VIPER Module Unit Tests\n");
    printf("=======================\n");

    RUN_TEST_SUITE(VIPER_Module, viper_module_tests);
    print_test_summary();

    return test_fail_count > 0 ? 1 : 0;
}
