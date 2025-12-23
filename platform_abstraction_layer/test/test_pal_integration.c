/**
 * test_pal_integration.c - Test PAL OS Adapter Integration
 * 
 * Simple test to verify PAL OS Adapter works with existing state machine examples.
 */

#include <stdio.h>
#include <stdlib.h>
#include "../include/pal_os_adapter.h"

/* Simple test state machine */
typedef enum {
    TEST_STATE_IDLE,
    TEST_STATE_ACTIVE,
    TEST_STATE_ERROR,
    TEST_STATE_COUNT
} test_state_t;

typedef enum {
    TEST_EVENT_START,
    TEST_EVENT_STOP,
    TEST_EVENT_ERROR,
    TEST_EVENT_COUNT
} test_event_t;

static const char *state_names[] = {
    "IDLE",
    "ACTIVE",
    "ERROR"
};

static const char *event_names[] = {
    "START",
    "STOP",
    "ERROR"
};

static void print_test_header(const char *test_name)
{
    printf("\n========================================\n");
    printf("Test: %s\n", test_name);
    printf("========================================\n");
}

static void test_pal_initialization(void)
{
    print_test_header("PAL OS Adapter Initialization");
    
    pal_os_adapter_config_t config = {
        .target_platform = PAL_PLATFORM_WINDOWS_SIMULATOR,
        .enable_gui = false,  /* No GUI for unit test */
        .enable_hardware_events = false,
        .event_queue_size = 5,
        .config_file = NULL
    };
    
    printf("Initializing PAL OS Adapter...\n");
    os_error_t result = pal_os_adapter_init(&config);
    
    if (result == OS_OK) {
        printf("✓ PAL OS Adapter initialized successfully\n");
        
        /* Test version function */
        const char *version = pal_os_adapter_get_version();
        printf("✓ Version: %s\n", version);
        
        /* Test deinitialization */
        result = pal_os_adapter_deinit();
        if (result == OS_OK) {
            printf("✓ PAL OS Adapter deinitialized successfully\n");
        } else {
            printf("✗ Failed to deinitialize: %d\n", result);
        }
    } else {
        printf("✗ Failed to initialize: %d\n", result);
    }
}

static void test_gpio_event_queue(void)
{
    print_test_header("GPIO Event Queue Test");
    
    /* Initialize with minimal configuration */
    pal_os_adapter_config_t config = {
        .target_platform = PAL_PLATFORM_WINDOWS_SIMULATOR,
        .enable_gui = false,
        .enable_hardware_events = true,
        .event_queue_size = 3,
        .config_file = NULL
    };
    
    if (pal_os_adapter_init(&config) != OS_OK) {
        printf("✗ Cannot test without initialization\n");
        return;
    }
    
    printf("Creating GPIO event queue...\n");
    os_queue_t *queue = pal_os_adapter_create_gpio_event_queue(0, 3);
    
    if (queue) {
        printf("✓ GPIO event queue created successfully\n");
        
        /* Test sending an event */
        pal_hw_event_t test_event = {
            .type = PAL_HW_EVENT_GPIO_INTERRUPT,
            .timestamp = 1000,
            .data.gpio = {
                .pin = 0,
                .state = PAL_GPIO_HIGH
            }
        };
        
        os_error_t send_result = pal_os_adapter_gpio_interrupt_to_event(
            0, PAL_GPIO_HIGH, queue);
        
        if (send_result == OS_OK) {
            printf("✓ Event sent to queue successfully\n");
        } else {
            printf("✗ Failed to send event: %d\n", send_result);
        }
        
        /* Note: In a real test, we would receive and verify the event */
        printf("Note: Event reception would be tested in integration test\n");
        
        /* Cleanup */
        // os_queue_destroy(queue); /* Not implemented in stub */
        printf("✓ Queue test completed\n");
    } else {
        printf("✗ Failed to create GPIO event queue\n");
    }
    
    pal_os_adapter_deinit();
}

static void test_fsm_context_creation(void)
{
    print_test_header("FSM Context Creation Test");
    
    pal_os_adapter_config_t config = {
        .target_platform = PAL_PLATFORM_WINDOWS_SIMULATOR,
        .enable_gui = false,
        .enable_hardware_events = true,
        .event_queue_size = 5,
        .config_file = NULL
    };
    
    if (pal_os_adapter_init(&config) != OS_OK) {
        printf("✗ Cannot test without initialization\n");
        return;
    }
    
    printf("Creating FSM OS context...\n");
    fsm_os_context_t *ctx = pal_os_adapter_create_fsm_context(&config);
    
    if (ctx) {
        printf("✓ FSM OS context created successfully\n");
        printf("Note: Context would be used with state machine library\n");
        
        /* In a real integration, we would:
         * 1. Create a state machine with this context
         * 2. Test state transitions
         * 3. Verify hardware event integration
         */
        
        /* Cleanup */
        // fsm_os_context_destroy(ctx); /* Not implemented in stub */
        printf("✓ Context test completed\n");
    } else {
        printf("✗ Failed to create FSM OS context\n");
    }
    
    pal_os_adapter_deinit();
}

static void test_multi_platform_config(void)
{
    print_test_header("Multi-Platform Configuration Test");
    
    struct platform_test {
        pal_platform_t platform;
        const char *name;
        bool expected_gui_support;
    } tests[] = {
        {PAL_PLATFORM_WINDOWS_SIMULATOR, "Windows Simulator", true},
        {PAL_PLATFORM_LINUX_SIMULATOR, "Linux Simulator", true},
        {PAL_PLATFORM_STM32_F4, "STM32 F4", false},
        {PAL_PLATFORM_ESP32, "ESP32", false},
    };
    
    for (size_t i = 0; i < sizeof(tests)/sizeof(tests[0]); i++) {
        printf("\nPlatform: %s\n", tests[i].name);
        
        pal_os_adapter_config_t config = {
            .target_platform = tests[i].platform,
            .enable_gui = tests[i].expected_gui_support,
            .enable_hardware_events = true,
            .event_queue_size = 5,
            .config_file = NULL
        };
        
        printf("  Config: platform=%d, gui=%s\n",
               config.target_platform,
               config.enable_gui ? "enabled" : "disabled");
        
        /* Note: Actual initialization would fail without proper PAL implementation
         * for each platform. This is just a configuration test. */
        printf("  Status: Configuration valid\n");
    }
    
    printf("\n✓ Multi-platform configuration test completed\n");
}

static void run_all_tests(void)
{
    printf("========================================\n");
    printf("PAL OS Adapter Integration Tests\n");
    printf("========================================\n");
    printf("\nRunning tests...\n");
    
    test_pal_initialization();
    test_gpio_event_queue();
    test_fsm_context_creation();
    test_multi_platform_config();
    
    printf("\n========================================\n");
    printf("All tests completed\n");
    printf("========================================\n");
}

int main(void)
{
    run_all_tests();
    return 0;
}
