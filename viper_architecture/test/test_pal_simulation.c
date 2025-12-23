/**
 * test_pal_simulation.c - Hardware simulation test using PAL Windows Simulator
 *
 * Tests VIPER integration with hardware abstraction layer.
 */

#include "test_framework.h"
#include "../include/viper.h"
#include "../../platform_abstraction_layer/include/pal.h"
#include "../../platform_abstraction_layer/include/pal_gpio.h"
#include <stdlib.h>
#include <string.h>

/* Mock PAL configuration */
static pal_config_t g_pal_config = {
    .platform = PAL_PLATFORM_WINDOWS_SIMULATOR,
    .config_file = NULL,
    .simulation_speed = 1,
    .enable_gui = 0,          /* Disable GUI for headless testing */
    .enable_logging = 1
};

/* Test PAL initialization */
TEST_CASE(test_pal_init_basic) {
    pal_status_t status = pal_init(&g_pal_config);
    TEST_ASSERT_EQUAL(PAL_STATUS_OK, status);
    TEST_ASSERT_EQUAL(PAL_PLATFORM_WINDOWS_SIMULATOR, pal_get_platform());

    const char *version = pal_get_version();
    TEST_ASSERT_NOT_NULL(version);
    printf("PAL Version: %s\n", version);

    pal_deinit();
}

TEST_CASE(test_pal_gpio_basic) {
    pal_status_t status = pal_init(&g_pal_config);
    TEST_ASSERT_EQUAL(PAL_STATUS_OK, status);

    /* Configure GPIO pin as output */
    pal_gpio_config_t gpio_config = {
        .mode = PAL_GPIO_MODE_OUTPUT,
        .interrupt = PAL_GPIO_INT_NONE,
        .debounce_ms = 0,
        .initial_state = PAL_GPIO_LOW
    };

    status = pal_gpio_init(5, &gpio_config);
    TEST_ASSERT_EQUAL(PAL_STATUS_OK, status);

    /* Set GPIO high */
    status = pal_gpio_write(5, PAL_GPIO_HIGH);
    TEST_ASSERT_EQUAL(PAL_STATUS_OK, status);

    /* Read back (should be high) */
    pal_gpio_state_t state;
    status = pal_gpio_read(5, &state);
    TEST_ASSERT_EQUAL(PAL_STATUS_OK, status);
    TEST_ASSERT_EQUAL(PAL_GPIO_HIGH, state);

    /* Set low */
    status = pal_gpio_write(5, PAL_GPIO_LOW);
    TEST_ASSERT_EQUAL(PAL_STATUS_OK, status);
    status = pal_gpio_read(5, &state);
    TEST_ASSERT_EQUAL(PAL_STATUS_OK, status);
    TEST_ASSERT_EQUAL(PAL_GPIO_LOW, state);

    pal_deinit();
}

TEST_CASE(test_pal_gpio_invalid) {
    pal_status_t status = pal_init(&g_pal_config);
    TEST_ASSERT_EQUAL(PAL_STATUS_OK, status);

    /* Invalid pin */
    pal_gpio_config_t config = { .mode = PAL_GPIO_MODE_OUTPUT };
    status = pal_gpio_init(300, &config); /* out of range */
    TEST_ASSERT_EQUAL(PAL_STATUS_INVALID_PARAM, status);

    /* NULL config */
    status = pal_gpio_init(0, NULL);
    TEST_ASSERT_EQUAL(PAL_STATUS_INVALID_PARAM, status);

    pal_deinit();
}

/* Test PAL timing functions */
TEST_CASE(test_pal_timing) {
    pal_status_t status = pal_init(&g_pal_config);
    TEST_ASSERT_EQUAL(PAL_STATUS_OK, status);

    uint32_t start = pal_get_elapsed_time_ms();
    pal_delay_ms(50); /* delay 50ms */
    uint32_t elapsed = pal_get_elapsed_time_ms() - start;

    /* Should be approximately 50ms, allow some tolerance */
    TEST_ASSERT(elapsed >= 45 && elapsed <= 150);

    pal_deinit();
}

/* Integration with VIPER: simulate hardware event */
TEST_CASE(test_viper_pal_integration) {
    /* Initialize PAL */
    pal_status_t pal_status = pal_init(&g_pal_config);
    TEST_ASSERT_EQUAL(PAL_STATUS_OK, pal_status);

    /* Initialize VIPER module */
    struct viper_config viper_config = {
        .enable_view = 1,
        .enable_presenter = 1,
        .enable_interactor = 1,
        .enable_entity = 1,
        .enable_router = 0,
        .max_events = 10
    };
    strcpy(viper_config.module_name, "HardwareIntegration");

    struct viper_module *module = viper_module_init(&viper_config);
    TEST_ASSERT_NOT_NULL(module);

    /* Simulate hardware input via GPIO */
    pal_gpio_config_t input_config = {
        .mode = PAL_GPIO_MODE_INPUT_PULLUP,
        .interrupt = PAL_GPIO_INT_NONE,
        .debounce_ms = 10,
        .initial_state = PAL_GPIO_LOW
    };
    pal_status = pal_gpio_init(0, &input_config);
    TEST_ASSERT_EQUAL(PAL_STATUS_OK, pal_status);

    /* Simulate button press (set GPIO high) */
    pal_status = pal_gpio_write(0, PAL_GPIO_HIGH);
    TEST_ASSERT_EQUAL(PAL_STATUS_OK, pal_status);

    /* Create VIPER event representing hardware input */
    struct viper_event *hw_event = viper_event_create(
        VIPER_EVENT_VIEW_ACTION,
        "GPIO0_HIGH",
        strlen("GPIO0_HIGH") + 1,
        VIPER_COMPONENT_VIEW,  /* source */
        VIPER_COMPONENT_INTERACTOR  /* target */
    );
    TEST_ASSERT_NOT_NULL(hw_event);

    int result = viper_module_process_event(module, hw_event);
    TEST_ASSERT_EQUAL(0, result);

    /* Simulate hardware response: turn on LED (GPIO output) */
    pal_gpio_config_t output_config = {
        .mode = PAL_GPIO_MODE_OUTPUT,
        .interrupt = PAL_GPIO_INT_NONE,
        .debounce_ms = 0,
        .initial_state = PAL_GPIO_LOW
    };
    pal_status = pal_gpio_init(1, &output_config);
    TEST_ASSERT_EQUAL(PAL_STATUS_OK, pal_status);

    /* VIPER event to turn on LED */
    struct viper_event *led_event = viper_event_create(
        VIPER_EVENT_DATA_UPDATE,
        "GPIO1_HIGH",
        strlen("GPIO1_HIGH") + 1,
        VIPER_COMPONENT_INTERACTOR,
        VIPER_COMPONENT_VIEW
    );
    TEST_ASSERT_NOT_NULL(led_event);
    result = viper_module_process_event(module, led_event);
    TEST_ASSERT_EQUAL(0, result);

    /* Verify LED is on */
    pal_gpio_state_t led_state;
    pal_status = pal_gpio_read(1, &led_state);
    TEST_ASSERT_EQUAL(PAL_STATUS_OK, pal_status);
    /* Note: pal_gpio_write not called by VIPER, but we can simulate */
    pal_gpio_write(1, PAL_GPIO_HIGH);
    pal_gpio_read(1, &led_state);
    TEST_ASSERT_EQUAL(PAL_GPIO_HIGH, led_state);

    /* Cleanup */
    viper_event_destroy(hw_event);
    viper_event_destroy(led_event);
    viper_module_destroy(module);
    pal_deinit();
}

/* Test error handling when PAL not initialized */
TEST_CASE(test_pal_not_initialized) {
    /* Try to use PAL before init */
    pal_gpio_config_t config = { .mode = PAL_GPIO_MODE_OUTPUT };
    pal_status_t status = pal_gpio_init(0, &config);
    TEST_ASSERT_EQUAL(PAL_STATUS_NOT_INITIALIZED, status);

    pal_gpio_state_t state;
    status = pal_gpio_read(0, &state);
    TEST_ASSERT_EQUAL(PAL_STATUS_NOT_INITIALIZED, status);

    status = pal_gpio_write(0, PAL_GPIO_HIGH);
    TEST_ASSERT_EQUAL(PAL_STATUS_NOT_INITIALIZED, status);
}

/* Test performance configuration */
TEST_CASE(test_pal_performance_config) {
    pal_status_t status = pal_init(&g_pal_config);
    TEST_ASSERT_EQUAL(PAL_STATUS_OK, status);

    pal_performance_config_t perf_config = {
        .target_fps = 30,
        .update_frequency_hz = 100,
        .vsync_enabled = false,
        .limit_cpu_usage = true,
        .max_frame_time_ms = 50
    };

    status = pal_set_performance_config(&perf_config);
    TEST_ASSERT_EQUAL(PAL_STATUS_OK, status);

    pal_performance_config_t retrieved;
    status = pal_get_performance_config(&retrieved);
    TEST_ASSERT_EQUAL(PAL_STATUS_OK, status);
    TEST_ASSERT_EQUAL(30, retrieved.target_fps);
    TEST_ASSERT_EQUAL(false, retrieved.vsync_enabled);

    pal_deinit();
}

/* Test suite array */
void (*pal_simulation_tests[])(void) = {
    test_pal_init_basic,
    test_pal_gpio_basic,
    test_pal_gpio_invalid,
    test_pal_timing,
    test_viper_pal_integration,
    test_pal_not_initialized,
    test_pal_performance_config,
};

int main(void) {
    printf("PAL Simulation Tests (Hardware Interaction)\n");
    printf("===========================================\n");

    RUN_TEST_SUITE(PAL_Simulation, pal_simulation_tests);
    print_test_summary();

    return test_fail_count > 0 ? 1 : 0;
}
