/**
 * test_led_fsm.c  2025-12-15
 *
 * Test program for LED FSM.
 * Demonstrates LED state machine operation with simulated PWM output.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "led_fsm.h"

/* ==================== PWM Callback Simulation ==================== */

static int pwm_callback_count = 0;
static uint8_t last_pwm_values[PWM_CHANNELS] = {0};

static void pwm_callback(uint8_t channel, uint8_t value)
{
    if (channel < PWM_CHANNELS) {
        last_pwm_values[channel] = value;
        pwm_callback_count++;
        
        /* Print only first few updates to avoid clutter */
        if (pwm_callback_count <= 12) {
            printf("PWM[%2d] = %3d\n", channel, value);
        }
    }
}

static void print_all_pwm(led_hsm_t *hsm)
{
    printf("\nCurrent PWM values:\n");
    for (int led = 0; led < LED_COUNT; led++) {
        printf("LED%d: R=%3d G=%3d B=%3d\n", 
               led,
               led_hsm_get_pwm(hsm, led * 3 + 0),
               led_hsm_get_pwm(hsm, led * 3 + 1),
               led_hsm_get_pwm(hsm, led * 3 + 2));
    }
}

/* ==================== Test Functions ==================== */

static void test_power_on_off(void)
{
    printf("\n=== Test 1: Power On/Off ===\n");
    
    led_hsm_t hsm;
    led_hsm_init(&hsm, pwm_callback);
    
    printf("Initial state: OFF\n");
    print_all_pwm(&hsm);
    
    /* Power on */
    printf("\nSending EV_LED_POWER_ON...\n");
    led_hsm_dispatch_event(&hsm, EV_LED_POWER_ON);
    print_all_pwm(&hsm);
    
    /* Power off */
    printf("\nSending EV_LED_POWER_OFF...\n");
    led_hsm_dispatch_event(&hsm, EV_LED_POWER_OFF);
    print_all_pwm(&hsm);
}

static void test_effect_static(void)
{
    printf("\n=== Test 2: Static Effect ===\n");
    
    led_hsm_t hsm;
    led_hsm_init(&hsm, pwm_callback);
    
    /* Power on */
    led_hsm_dispatch_event(&hsm, EV_LED_POWER_ON);
    
    /* Set to static effect */
    printf("Setting static effect...\n");
    led_hsm_set_effect(&hsm, EFFECT_STATIC);
    
    /* Set color to red */
    printf("Setting color to red...\n");
    led_hsm_set_color(&hsm, 255, 0, 0);
    print_all_pwm(&hsm);
    
    /* Change brightness */
    printf("Setting brightness to 128...\n");
    led_hsm_set_brightness(&hsm, 128);
    print_all_pwm(&hsm);
    
    /* Change color to green */
    printf("Setting color to green...\n");
    led_hsm_set_color(&hsm, 0, 255, 0);
    print_all_pwm(&hsm);
}

static void test_effect_breath(void)
{
    printf("\n=== Test 3: Breath Effect ===\n");
    
    led_hsm_t hsm;
    led_hsm_init(&hsm, pwm_callback);
    
    /* Power on */
    led_hsm_dispatch_event(&hsm, EV_LED_POWER_ON);
    
    /* Set to breath effect */
    printf("Setting breath effect...\n");
    led_hsm_set_effect(&hsm, EFFECT_BREATH);
    
    /* Set color to blue */
    led_hsm_set_color(&hsm, 0, 0, 255);
    
    /* Simulate a few breath steps */
    for (int i = 0; i < 5; i++) {
        printf("\nBreath step %d:\n", i);
        led_hsm_dispatch_event(&hsm, EV_LED_BREATH_STEP);
        led_hsm_update_pwm(&hsm);
        print_all_pwm(&hsm);
        usleep(100000); /* 100ms */
    }
}

static void test_effect_flow(void)
{
    printf("\n=== Test 4: Flow Effect ===\n");
    
    led_hsm_t hsm;
    led_hsm_init(&hsm, pwm_callback);
    
    /* Power on */
    led_hsm_dispatch_event(&hsm, EV_LED_POWER_ON);
    
    /* Set to flow effect */
    printf("Setting flow effect...\n");
    led_hsm_set_effect(&hsm, EFFECT_FLOW);
    
    /* Set color to yellow */
    led_hsm_set_color(&hsm, 255, 255, 0);
    
    /* Simulate flow steps */
    for (int i = 0; i < 8; i++) {
        printf("\nFlow step %d:\n", i);
        led_hsm_dispatch_event(&hsm, EV_LED_FLOW_STEP);
        led_hsm_update_pwm(&hsm);
        print_all_pwm(&hsm);
        usleep(100000); /* 100ms */
    }
}

static void test_external_commands(void)
{
    printf("\n=== Test 5: External Commands ===\n");
    
    led_hsm_t hsm;
    led_hsm_init(&hsm, pwm_callback);
    
    /* Command: Power on (0x01) */
    printf("Command: Power on\n");
    uint8_t cmd_power_on = 0x01;
    led_hsm_process_command(&hsm, cmd_power_on, NULL, 0);
    print_all_pwm(&hsm);
    
    /* Command: Set effect to blink (0x03) */
    printf("\nCommand: Set effect to blink\n");
    uint8_t cmd_set_effect[] = {EFFECT_BLINK};
    led_hsm_process_command(&hsm, 0x03, cmd_set_effect, 1);
    print_all_pwm(&hsm);
    
    /* Command: Set color to purple (0x04) */
    printf("\nCommand: Set color to purple\n");
    uint8_t cmd_set_color[] = {128, 0, 128};
    led_hsm_process_command(&hsm, 0x04, cmd_set_color, 3);
    print_all_pwm(&hsm);
    
    /* Command: Set brightness (0x05) */
    printf("\nCommand: Set brightness to 64\n");
    uint8_t cmd_set_brightness[] = {64};
    led_hsm_process_command(&hsm, 0x05, cmd_set_brightness, 1);
    print_all_pwm(&hsm);
}

static void test_all_effects(void)
{
    printf("\n=== Test 6: All Effects ===\n");
    
    led_hsm_t hsm;
    led_hsm_init(&hsm, pwm_callback);
    
    /* Power on */
    led_hsm_dispatch_event(&hsm, EV_LED_POWER_ON);
    
    /* Test each effect */
    led_effect_t effects[] = {
        EFFECT_STATIC,
        EFFECT_BREATH,
        EFFECT_BLINK,
        EFFECT_FLOW,
        EFFECT_GRADIENT,
        EFFECT_RAINBOW
    };
    
    const char *effect_names[] = {
        "Static",
        "Breath",
        "Blink",
        "Flow",
        "Gradient",
        "Rainbow"
    };
    
    for (int i = 0; i < sizeof(effects)/sizeof(effects[0]); i++) {
        printf("\n--- %s Effect ---\n", effect_names[i]);
        led_hsm_set_effect(&hsm, effects[i]);
        led_hsm_update_pwm(&hsm);
        print_all_pwm(&hsm);
        
        if (i < sizeof(effects)/sizeof(effects[0]) - 1) {
            usleep(500000); /* 500ms between effects */
        }
    }
}

/* ==================== Main Program ==================== */

int main(void)
{
    printf("LED FSM Test Program\n");
    printf("====================\n");
    
    /* Reset callback counter */
    pwm_callback_count = 0;
    memset(last_pwm_values, 0, sizeof(last_pwm_values));
    
    /* Run tests */
    test_power_on_off();
    test_effect_static();
    test_effect_breath();
    test_effect_flow();
    test_external_commands();
    test_all_effects();
    
    printf("\n=== Test Summary ===\n");
    printf("Total PWM callbacks: %d\n", pwm_callback_count);
    printf("All tests completed successfully.\n");
    
    return 0;
}
