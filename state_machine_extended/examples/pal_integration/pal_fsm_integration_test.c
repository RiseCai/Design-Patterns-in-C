/**
 * pal_fsm_integration_test.c - PAL FSM Integration Test
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * Integration test for PAL to State Machine adapter.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Include required headers */
#include "../../src/fsm_os_adapter.h"
#include "../../examples/product/ui/led/led_fsm.h"
#include "pal_fsm_adapter.h"

/* ==================== Test Configuration ==================== */

#define TEST_ITERATIONS 10
#define TEST_PLATFORM PAL_PLATFORM_WINDOWS_SIMULATOR

/* ==================== Test Functions ==================== */

/**
 * Test 1: PAL FSM adapter creation and destruction
 */
static int test_adapter_creation(void)
{
    printf("Test 1: Adapter creation and destruction...\n");
    
    /* Create adapter */
    pal_fsm_adapter_t *adapter = pal_fsm_adapter_create_led_fsm(TEST_PLATFORM);
    assert(adapter != NULL);
    printf("  ✓ Adapter created successfully\n");
    
    /* Get context */
    fsm_os_context_t *context = pal_fsm_adapter_get_context(adapter);
    assert(context != NULL);
    printf("  ✓ FSM OS context obtained\n");
    
    /* Initialize hardware */
    bool hw_init = pal_fsm_adapter_init_hardware(adapter);
    assert(hw_init == true);
    printf("  ✓ Hardware initialized\n");
    
    /* Destroy adapter */
    pal_fsm_adapter_destroy(adapter);
    printf("  ✓ Adapter destroyed\n");
    
    printf("Test 1 PASSED\n\n");
    return 0;
}

/**
 * Test 2: LED control
 */
static int test_led_control(void)
{
    printf("Test 2: LED control...\n");
    
    /* Create adapter */
    pal_fsm_adapter_t *adapter = pal_fsm_adapter_create_led_fsm(TEST_PLATFORM);
    assert(adapter != NULL);
    
    /* Initialize hardware */
    bool hw_init = pal_fsm_adapter_init_hardware(adapter);
    assert(hw_init == true);
    
    /* Test LED on */
    bool led_on = pal_fsm_adapter_set_led(adapter, true);
    assert(led_on == true);
    printf("  ✓ LED turned ON\n");
    
    /* Test LED off */
    bool led_off = pal_fsm_adapter_set_led(adapter, false);
    assert(led_off == true);
    printf("  ✓ LED turned OFF\n");
    
    /* Toggle test */
    for (int i = 0; i < 3; i++) {
        bool state = (i % 2 == 0);
        bool result = pal_fsm_adapter_set_led(adapter, state);
        assert(result == true);
        printf("  ✓ LED toggled to %s\n", state ? "ON" : "OFF");
    }
    
    pal_fsm_adapter_destroy(adapter);
    printf("Test 2 PASSED\n\n");
    return 0;
}

/**
 * Test 3: Button reading
 */
static int test_button_reading(void)
{
    printf("Test 3: Button reading...\n");
    
    /* Create adapter */
    pal_fsm_adapter_t *adapter = pal_fsm_adapter_create_led_fsm(TEST_PLATFORM);
    assert(adapter != NULL);
    
    /* Initialize hardware */
    bool hw_init = pal_fsm_adapter_init_hardware(adapter);
    assert(hw_init == true);
    
    /* Read button state */
    bool button_state = pal_fsm_adapter_get_button(adapter);
    printf("  ✓ Button state read: %s\n", button_state ? "PRESSED" : "RELEASED");
    
    /* Process events (should detect no changes) */
    int events = pal_fsm_adapter_process_events(adapter);
    printf("  ✓ Processed %d events\n", events);
    
    pal_fsm_adapter_destroy(adapter);
    printf("Test 3 PASSED\n\n");
    return 0;
}

/**
 * Test 4: Event processing
 */
static int test_event_processing(void)
{
    printf("Test 4: Event processing...\n");
    
    /* Create adapter */
    pal_fsm_adapter_t *adapter = pal_fsm_adapter_create_led_fsm(TEST_PLATFORM);
    assert(adapter != NULL);
    
    /* Initialize hardware */
    bool hw_init = pal_fsm_adapter_init_hardware(adapter);
    assert(hw_init == true);
    
    /* Get context */
    fsm_os_context_t *context = pal_fsm_adapter_get_context(adapter);
    assert(context != NULL);
    
    /* Initialize LED FSM */
    led_fsm_init(context);
    printf("  ✓ LED FSM initialized\n");
    
    /* Send hardware event */
    uint32_t hw_event = 0;  /* Button pressed */
    bool event_sent = pal_fsm_adapter_send_hw_event(adapter, hw_event, NULL);
    assert(event_sent == true);
    printf("  ✓ Hardware event sent\n");
    
    /* Convert hardware event to FSM event */
    int fsm_event = pal_fsm_adapter_hw_event_to_fsm_event(adapter, hw_event, NULL);
    assert(fsm_event >= 0);
    printf("  ✓ Hardware event %u converted to FSM event %d\n", hw_event, fsm_event);
    
    pal_fsm_adapter_destroy(adapter);
    printf("Test 4 PASSED\n\n");
    return 0;
}

/**
 * Test 5: Integration with LED FSM
 */
static int test_led_fsm_integration(void)
{
    printf("Test 5: LED FSM integration...\n");
    
    /* Create adapter */
    pal_fsm_adapter_t *adapter = pal_fsm_adapter_create_led_fsm(TEST_PLATFORM);
    assert(adapter != NULL);
    
    /* Initialize hardware */
    bool hw_init = pal_fsm_adapter_init_hardware(adapter);
    assert(hw_init == true);
    
    /* Get context */
    fsm_os_context_t *context = pal_fsm_adapter_get_context(adapter);
    assert(context != NULL);
    
    /* Initialize LED FSM */
    led_fsm_init(context);
    
    /* Set up hardware control callback */
    led_fsm_set_hardware_control_callback(pal_fsm_adapter_led_pwm_callback, adapter);
    printf("  ✓ Hardware control callback set\n");
    
    /* Test PWM callback */
    pal_fsm_adapter_led_pwm_callback(0, 255, adapter);
    printf("  ✓ PWM callback executed\n");
    
    pal_fsm_adapter_destroy(adapter);
    printf("Test 5 PASSED\n\n");
    return 0;
}

/* ==================== Main Test Runner ==================== */

int main(int argc, char *argv[])
{
    printf("=== PAL FSM Integration Test Suite ===\n");
    printf("Running %d test cases...\n\n", 5);
    
    int failures = 0;
    
    /* Run all tests */
    failures += test_adapter_creation();
    failures += test_led_control();
    failures += test_button_reading();
    failures += test_event_processing();
    failures += test_led_fsm_integration();
    
    /* Summary */
    printf("=== Test Summary ===\n");
    if (failures == 0) {
        printf("All tests PASSED!\n");
        return EXIT_SUCCESS;
    } else {
        printf("%d test(s) FAILED\n", failures);
        return EXIT_FAILURE;
    }
}
