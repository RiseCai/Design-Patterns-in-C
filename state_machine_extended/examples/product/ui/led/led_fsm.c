/**
 * led_fsm.c  2025-12-15
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Auto generate for Design Patterns in C *
 * 
 * LED FSM implementation for controlling 4 RGB LEDs with 12 PWM channels.
 * Moore hierarchical state machine with external command interface.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <mycommon.h>
#include <mytrace.h>
#include "moore_hierarchical.h"
#include "led_fsm.h"

/* ==================== Local State Definitions ==================== */

/* Forward declarations of state structures */
static struct moore_state state_led_off;
static struct moore_state state_led_on;
static struct moore_state state_effect_static;
static struct moore_state state_effect_breath;
static struct moore_state state_breath_up;
static struct moore_state state_breath_down;
static struct moore_state state_effect_blink;
static struct moore_state state_blink_on;
static struct moore_state state_blink_off;
static struct moore_state state_effect_gradient;
static struct moore_state state_effect_flow;
static struct moore_state state_flow_pos[4]; /* 0-3 */
static struct moore_state state_effect_rainbow;

/* ==================== State Entry/Exit/Do Actions ==================== */

/* LED OFF state actions */
static void led_off_entry(void)
{
    _MY_TRACE_STR("led_off_entry\n");
    /* Turn off all LEDs */
}

static void led_off_do(void)
{
    /* Nothing to do in OFF state */
}

static void led_off_exit(void)
{
    _MY_TRACE_STR("led_off_exit\n");
}

/* LED ON state actions */
static void led_on_entry(void)
{
    _MY_TRACE_STR("led_on_entry\n");
}

static void led_on_do(void)
{
    /* Update PWM based on current effect */
}

static void led_on_exit(void)
{
    _MY_TRACE_STR("led_on_exit\n");
}

/* Static effect actions */
static void effect_static_entry(void)
{
    _MY_TRACE_STR("effect_static_entry\n");
}

static void effect_static_do(void)
{
    /* Set all LEDs to configured color */
}

static void effect_static_exit(void)
{
    _MY_TRACE_STR("effect_static_exit\n");
}

/* Breath effect actions */
static void effect_breath_entry(void)
{
    _MY_TRACE_STR("effect_breath_entry\n");
}

static void effect_breath_do(void)
{
    /* Update breath phase */
}

static void effect_breath_exit(void)
{
    _MY_TRACE_STR("effect_breath_exit\n");
}

static void breath_up_entry(void)
{
    _MY_TRACE_STR("breath_up_entry\n");
}

static void breath_up_do(void)
{
    /* Increase brightness */
}

static void breath_up_exit(void)
{
    _MY_TRACE_STR("breath_up_exit\n");
}

static void breath_down_entry(void)
{
    _MY_TRACE_STR("breath_down_entry\n");
}

static void breath_down_do(void)
{
    /* Decrease brightness */
}

static void breath_down_exit(void)
{
    _MY_TRACE_STR("breath_down_exit\n");
}

/* Blink effect actions */
static void effect_blink_entry(void)
{
    _MY_TRACE_STR("effect_blink_entry\n");
}

static void effect_blink_do(void)
{
    /* Toggle between on/off */
}

static void effect_blink_exit(void)
{
    _MY_TRACE_STR("effect_blink_exit\n");
}

static void blink_on_entry(void)
{
    _MY_TRACE_STR("blink_on_entry\n");
}

static void blink_on_do(void)
{
    /* LEDs on */
}

static void blink_on_exit(void)
{
    _MY_TRACE_STR("blink_on_exit\n");
}

static void blink_off_entry(void)
{
    _MY_TRACE_STR("blink_off_entry\n");
}

static void blink_off_do(void)
{
    /* LEDs off */
}

static void blink_off_exit(void)
{
    _MY_TRACE_STR("blink_off_exit\n");
}

/* Flow effect actions */
static void effect_flow_entry(void)
{
    _MY_TRACE_STR("effect_flow_entry\n");
}

static void effect_flow_do(void)
{
    /* Update flow position */
}

static void effect_flow_exit(void)
{
    _MY_TRACE_STR("effect_flow_exit\n");
}

static void flow_pos_entry(void)
{
    _MY_TRACE_STR("flow_pos_entry\n");
}

static void flow_pos_do(void)
{
    /* Set appropriate LED on based on position */
}

static void flow_pos_exit(void)
{
    _MY_TRACE_STR("flow_pos_exit\n");
}

/* Gradient effect actions */
static void effect_gradient_entry(void)
{
    _MY_TRACE_STR("effect_gradient_entry\n");
}

static void effect_gradient_do(void)
{
    /* Update gradient */
}

static void effect_gradient_exit(void)
{
    _MY_TRACE_STR("effect_gradient_exit\n");
}

/* Rainbow effect actions */
static void effect_rainbow_entry(void)
{
    _MY_TRACE_STR("effect_rainbow_entry\n");
}

static void effect_rainbow_do(void)
{
    /* Update rainbow colors */
}

static void effect_rainbow_exit(void)
{
    _MY_TRACE_STR("effect_rainbow_exit\n");
}

/* ==================== Event Handlers ==================== */

static int led_off_handle_event(struct moore_state *self, int event)
{
    (void)self;
    switch (event) {
        case EV_LED_POWER_ON:
            return 1; /* Request transition to LED_ON */
        default:
            return 0;
    }
}

static int led_on_handle_event(struct moore_state *self, int event)
{
    (void)self;
    switch (event) {
        case EV_LED_POWER_OFF:
            return 2; /* Request transition to LED_OFF (code 2) */
        case EV_LED_EFFECT_STATIC:
            return 3; /* Request transition to EFFECT_STATIC (code 3) */
        case EV_LED_EFFECT_BREATH:
            return 4; /* Request transition to EFFECT_BREATH (code 4) */
        case EV_LED_EFFECT_BLINK:
            return 5; /* Request transition to EFFECT_BLINK (code 5) */
        case EV_LED_EFFECT_FLOW:
            return 6; /* Request transition to EFFECT_FLOW (code 6) */
        case EV_LED_EFFECT_GRADIENT:
            return 7; /* Request transition to EFFECT_GRADIENT (code 7) */
        case EV_LED_EFFECT_RAINBOW:
            return 8; /* Request transition to EFFECT_RAINBOW (code 8) */
        default:
            return 0;
    }
}

static int effect_breath_handle_event(struct moore_state *self, int event)
{
    (void)self;
    switch (event) {
        case EV_LED_BREATH_STEP:
            return 9; /* Transition to BREATH_UP (code 9) */
        case EV_LED_EFFECT_STATIC:
            return 3; /* Transition to EFFECT_STATIC (code 3) */
        default:
            return 0;
    }
}

static int effect_flow_handle_event(struct moore_state *self, int event)
{
    (void)self;
    switch (event) {
        case EV_LED_FLOW_STEP:
            return 13; /* Transition to FLOW_POS_0 (code 13) */
        case EV_LED_EFFECT_STATIC:
            return 3; /* Transition to EFFECT_STATIC (code 3) */
        default:
            return 0;
    }
}

/* ==================== State Structure Initialization ==================== */

/* Helper macro to initialize a state */
#define INIT_STATE(name_var, name_str, parent_ptr, entry_fn, exit_fn, do_fn, event_fn) \
    do { \
        (name_var).name = (name_str); \
        (name_var).parent = (parent_ptr); \
        memset((name_var).children, 0, sizeof((name_var).children)); \
        (name_var).child_count = 0; \
        (name_var).entry_action = (entry_fn); \
        (name_var).exit_action = (exit_fn); \
        (name_var).do_action = (do_fn); \
        (name_var).handle_event = (event_fn); \
    } while (0)

/* Add child to parent */
static void add_child(struct moore_state *parent, struct moore_state *child)
{
    if (parent && parent->child_count < 5) {
        parent->children[parent->child_count] = child;
        parent->child_count++;
    }
}

/* Initialize all states */
static void init_states(void)
{
    static int initialized = 0;
    if (initialized) return;
    
    /* Initialize states */
    INIT_STATE(state_led_off, "LED_OFF", NULL,
               led_off_entry, led_off_exit, led_off_do,
               led_off_handle_event);
    
    INIT_STATE(state_led_on, "LED_ON", NULL,
               led_on_entry, led_on_exit, led_on_do,
               led_on_handle_event);
    
    INIT_STATE(state_effect_static, "EFFECT_STATIC", &state_led_on,
               effect_static_entry, effect_static_exit, effect_static_do,
               NULL);
    
    INIT_STATE(state_effect_breath, "EFFECT_BREATH", &state_led_on,
               effect_breath_entry, effect_breath_exit, effect_breath_do,
               effect_breath_handle_event);
    
    INIT_STATE(state_breath_up, "BREATH_UP", &state_effect_breath,
               breath_up_entry, breath_up_exit, breath_up_do,
               NULL);
    
    INIT_STATE(state_breath_down, "BREATH_DOWN", &state_effect_breath,
               breath_down_entry, breath_down_exit, breath_down_do,
               NULL);
    
    INIT_STATE(state_effect_blink, "EFFECT_BLINK", &state_led_on,
               effect_blink_entry, effect_blink_exit, effect_blink_do,
               NULL);
    
    INIT_STATE(state_blink_on, "BLINK_ON", &state_effect_blink,
               blink_on_entry, blink_on_exit, blink_on_do,
               NULL);
    
    INIT_STATE(state_blink_off, "BLINK_OFF", &state_effect_blink,
               blink_off_entry, blink_off_exit, blink_off_do,
               NULL);
    
    INIT_STATE(state_effect_gradient, "EFFECT_GRADIENT", &state_led_on,
               effect_gradient_entry, effect_gradient_exit, effect_gradient_do,
               NULL);
    
    INIT_STATE(state_effect_flow, "EFFECT_FLOW", &state_led_on,
               effect_flow_entry, effect_flow_exit, effect_flow_do,
               effect_flow_handle_event);
    
    /* Initialize flow position states */
    for (int i = 0; i < 4; i++) {
        char name[16];
        snprintf(name, sizeof(name), "FLOW_POS_%d", i);
        INIT_STATE(state_flow_pos[i], name, &state_effect_flow,
                   flow_pos_entry, flow_pos_exit, flow_pos_do,
                   NULL);
    }
    
    INIT_STATE(state_effect_rainbow, "EFFECT_RAINBOW", &state_led_on,
               effect_rainbow_entry, effect_rainbow_exit, effect_rainbow_do,
               NULL);
    
    /* Build hierarchy */
    add_child(&state_led_on, &state_effect_static);
    add_child(&state_led_on, &state_effect_breath);
    add_child(&state_led_on, &state_effect_blink);
    add_child(&state_led_on, &state_effect_gradient);
    add_child(&state_led_on, &state_effect_flow);
    add_child(&state_led_on, &state_effect_rainbow);
    
    add_child(&state_effect_breath, &state_breath_up);
    add_child(&state_effect_breath, &state_breath_down);
    
    add_child(&state_effect_blink, &state_blink_on);
    add_child(&state_effect_blink, &state_blink_off);
    
    for (int i = 0; i < 4; i++) {
        add_child(&state_effect_flow, &state_flow_pos[i]);
    }
    
    initialized = 1;
}

/* ==================== State Transition Mapping ==================== */

/* Map return codes from handle_event to target states */
static struct moore_state *led_state_map(int code)
{
    switch (code) {
        case 1:  /* LED_ON (from LED_OFF) */
            return &state_led_on;
        case 2:  /* LED_OFF (from LED_ON) */
            return &state_led_off;
        case 3:  /* EFFECT_STATIC */
            return &state_effect_static;
        case 4:  /* EFFECT_BREATH */
            return &state_effect_breath;
        case 5:  /* EFFECT_BLINK */
            return &state_effect_blink;
        case 6:  /* EFFECT_FLOW */
            return &state_effect_flow;
        case 7:  /* EFFECT_GRADIENT */
            return &state_effect_gradient;
        case 8:  /* EFFECT_RAINBOW */
            return &state_effect_rainbow;
        case 9:  /* BREATH_UP */
            return &state_breath_up;
        case 10: /* BREATH_DOWN */
            return &state_breath_down;
        case 11: /* BLINK_ON */
            return &state_blink_on;
        case 12: /* BLINK_OFF */
            return &state_blink_off;
        case 13: /* FLOW_POS_0 */
            return &state_flow_pos[0];
        case 14: /* FLOW_POS_1 */
            return &state_flow_pos[1];
        case 15: /* FLOW_POS_2 */
            return &state_flow_pos[2];
        case 16: /* FLOW_POS_3 */
            return &state_flow_pos[3];
        default:
            return NULL;
    }
}

/* ==================== PWM Calculation Functions ==================== */

/* Apply brightness to color component */
static uint8_t apply_brightness(uint8_t component, uint8_t brightness)
{
    uint32_t temp = (uint32_t)component * brightness;
    return (uint8_t)(temp / MAX_BRIGHTNESS);
}

/* Calculate PWM values for static effect */
static void calculate_static_pwm(led_hsm_t *hsm)
{
    rgb_color_t *color = &hsm->config.color;
    uint8_t brightness = hsm->config.brightness;
    
    for (int i = 0; i < LED_COUNT; i++) {
        hsm->pwm_values[i * 3 + 0] = apply_brightness(color->r, brightness);
        hsm->pwm_values[i * 3 + 1] = apply_brightness(color->g, brightness);
        hsm->pwm_values[i * 3 + 2] = apply_brightness(color->b, brightness);
    }
}

/* Calculate PWM values for flow effect */
static void calculate_flow_pwm(led_hsm_t *hsm)
{
    rgb_color_t *color = &hsm->config.color;
    uint8_t brightness = hsm->config.brightness;
    uint8_t pos = hsm->config.flow_position;
    
    /* Turn off all LEDs first */
    memset(hsm->pwm_values, 0, PWM_CHANNELS);
    
    /* Turn on the LED at current position */
    if (pos < LED_COUNT) {
        hsm->pwm_values[pos * 3 + 0] = apply_brightness(color->r, brightness);
        hsm->pwm_values[pos * 3 + 1] = apply_brightness(color->g, brightness);
        hsm->pwm_values[pos * 3 + 2] = apply_brightness(color->b, brightness);
    }
}

/* Calculate PWM values for breath effect */
static void calculate_breath_pwm(led_hsm_t *hsm)
{
    rgb_color_t *color = &hsm->config.color;
    uint8_t phase = hsm->config.breath_phase;
    uint8_t brightness = hsm->config.brightness;
    
    /* Sinusoidal brightness: brightness * (sin(phase) + 1) / 2 */
    float rad = (float)phase * 2.0f * 3.14159265f / 255.0f;
    float factor = (sinf(rad) + 1.0f) / 2.0f;
    uint8_t current_brightness = (uint8_t)(brightness * factor);
    
    for (int i = 0; i < LED_COUNT; i++) {
        hsm->pwm_values[i * 3 + 0] = apply_brightness(color->r, current_brightness);
        hsm->pwm_values[i * 3 + 1] = apply_brightness(color->g, current_brightness);
        hsm->pwm_values[i * 3 + 2] = apply_brightness(color->b, current_brightness);
    }
}

/* Calculate PWM values for blink effect */
static void calculate_blink_pwm(led_hsm_t *hsm, int on)
{
    rgb_color_t *color = &hsm->config.color;
    uint8_t brightness = hsm->config.brightness;
    
    if (on) {
        for (int i = 0; i < LED_COUNT; i++) {
            hsm->pwm_values[i * 3 + 0] = apply_brightness(color->r, brightness);
            hsm->pwm_values[i * 3 + 1] = apply_brightness(color->g, brightness);
            hsm->pwm_values[i * 3 + 2] = apply_brightness(color->b, brightness);
        }
    } else {
        memset(hsm->pwm_values, 0, PWM_CHANNELS);
    }
}

/* ==================== Public API Implementation ==================== */

void led_hsm_init(led_hsm_t *hsm, void (*pwm_cb)(uint8_t ch, uint8_t value))
{
    if (!hsm) return;
    
    memset(hsm, 0, sizeof(*hsm));
    
    /* Initialize state hierarchy */
    init_states();
    
    /* Initialize base HSM */
    moore_hsm_init(&hsm->base_hsm, &state_led_off);
    
    /* Set default configuration */
    hsm->config.brightness = MAX_BRIGHTNESS;
    hsm->config.speed = 128;
    hsm->config.color.r = 255;
    hsm->config.color.g = 255;
    hsm->config.color.b = 255;
    hsm->config.effect = EFFECT_STATIC;
    hsm->config.flow_dir = FLOW_FORWARD;
    hsm->config.flow_position = 0;
    hsm->config.breath_phase = 0;
    hsm->config.breath_direction = 0;
    
    /* Set default timing */
    hsm->pwm_freq_hz = DEFAULT_PWM_FREQ_HZ;
    hsm->update_period_ms = DEFAULT_UPDATE_MS;
    hsm->flow_period_ms = DEFAULT_FLOW_MS;
    
    /* Set PWM callback */
    hsm->pwm_update_cb = pwm_cb;
    
    /* Initialize user_data to NULL */
    hsm->user_data = NULL;
    
    /* Initialize PWM values */
    memset(hsm->pwm_values, 0, PWM_CHANNELS);
    
    /* Call PWM update callback if available */
    if (hsm->pwm_update_cb) {
        for (int i = 0; i < PWM_CHANNELS; i++) {
            hsm->pwm_update_cb(i, hsm->pwm_values[i]);
        }
    }
}

void led_hsm_dispatch_event(led_hsm_t *hsm, int event)
{
    if (!hsm) return;
    
    /* Get current state */
    struct moore_state *current = hsm->base_hsm.current;
    if (!current) return;
    
    /* Walk up the hierarchy to find a handler */
    struct moore_state *state = current;
    while (state != NULL) {
        if (state->handle_event) {
            int code = state->handle_event(state, event);
            if (code > 0) {
                /* Map code to target state */
                struct moore_state *target = led_state_map(code);
                if (target) {
                    /* Debug print */
                    printf("[LED FSM] Transition from %s to %s (code=%d)\n", 
                           current->name ? current->name : "NULL",
                           target->name ? target->name : "NULL",
                           code);
                    /* Perform transition */
                    moore_hsm_transition(&hsm->base_hsm, target);
                }
                /* Event handled, stop bubbling */
                break;
            }
        }
        /* Move to parent state */
        state = state->parent;
    }
}

int led_hsm_set_effect(led_hsm_t *hsm, led_effect_t effect)
{
    if (!hsm) return -1;
    if (effect >= EFFECT_COUNT) return -1;
    
    hsm->config.effect = effect;
    
    /* Dispatch appropriate event */
    int event = 0;
    switch (effect) {
        case EFFECT_STATIC:
            event = EV_LED_EFFECT_STATIC;
            break;
        case EFFECT_BREATH:
            event = EV_LED_EFFECT_BREATH;
            break;
        case EFFECT_BLINK:
            event = EV_LED_EFFECT_BLINK;
            break;
        case EFFECT_GRADIENT:
            event = EV_LED_EFFECT_GRADIENT;
            break;
        case EFFECT_FLOW:
            event = EV_LED_EFFECT_FLOW;
            break;
        case EFFECT_RAINBOW:
            event = EV_LED_EFFECT_RAINBOW;
            break;
        default:
            return -1;
    }
    
    led_hsm_dispatch_event(hsm, event);
    return 0;
}

void led_hsm_set_color(led_hsm_t *hsm, uint8_t r, uint8_t g, uint8_t b)
{
    if (!hsm) return;
    
    hsm->config.color.r = r;
    hsm->config.color.g = g;
    hsm->config.color.b = b;
    
    /* Update PWM values */
    led_hsm_update_pwm(hsm);
}

void led_hsm_set_brightness(led_hsm_t *hsm, uint8_t brightness)
{
    if (!hsm) return;
    
    hsm->config.brightness = brightness;
    
    /* Update PWM values */
    led_hsm_update_pwm(hsm);
}

void led_hsm_set_speed(led_hsm_t *hsm, uint8_t speed)
{
    if (!hsm) return;
    
    hsm->config.speed = speed;
    
    /* Adjust timing based on speed */
    uint32_t flow_ms = DEFAULT_FLOW_MS * (256 - speed) / 256;
    if (flow_ms < 10) flow_ms = 10;
    hsm->flow_period_ms = flow_ms;
}

void led_hsm_set_flow_direction(led_hsm_t *hsm, flow_direction_t direction)
{
    if (!hsm) return;
    
    hsm->config.flow_dir = direction;
}

void led_hsm_config_timing(led_hsm_t *hsm, uint32_t pwm_freq_hz,
                           uint32_t update_ms, uint32_t flow_ms)
{
    if (!hsm) return;
    
    hsm->pwm_freq_hz = pwm_freq_hz;
    hsm->update_period_ms = update_ms;
    hsm->flow_period_ms = flow_ms;
}

void led_hsm_update_pwm(led_hsm_t *hsm)
{
    if (!hsm) return;
    
    /* Determine current state */
    struct moore_state *current = hsm->base_hsm.current;
    
    /* Update internal state based on current state */
    if (current == &state_breath_up) {
        /* Increase breath phase */
        if (hsm->config.breath_phase < 255) {
            hsm->config.breath_phase++;
        } else {
            /* Transition to breath down */
            moore_hsm_transition(&hsm->base_hsm, &state_breath_down);
        }
    } else if (current == &state_breath_down) {
        /* Decrease breath phase */
        if (hsm->config.breath_phase > 0) {
            hsm->config.breath_phase--;
        } else {
            /* Transition to breath up */
            moore_hsm_transition(&hsm->base_hsm, &state_breath_up);
        }
    } else if (current == &state_effect_flow || 
               (current >= &state_flow_pos[0] && current <= &state_flow_pos[3])) {
        /* Update flow position based on direction */
        uint8_t pos = hsm->config.flow_position;
        switch (hsm->config.flow_dir) {
            case FLOW_FORWARD:
                pos = (pos + 1) % LED_COUNT;
                break;
            case FLOW_BACKWARD:
                pos = (pos == 0) ? LED_COUNT - 1 : pos - 1;
                break;
            case FLOW_PINGPONG:
                /* Simple ping-pong: forward until last, then backward */
                static uint8_t pingpong_dir = 0; /* 0 forward, 1 backward */
                if (pingpong_dir == 0) {
                    if (pos == LED_COUNT - 1) {
                        pingpong_dir = 1;
                        pos--;
                    } else {
                        pos++;
                    }
                } else {
                    if (pos == 0) {
                        pingpong_dir = 0;
                        pos++;
                    } else {
                        pos--;
                    }
                }
                break;
        }
        hsm->config.flow_position = pos;
        /* Transition to appropriate flow position state */
        struct moore_state *target = &state_flow_pos[pos];
        moore_hsm_transition(&hsm->base_hsm, target);
    } else if (current == &state_effect_blink) {
        /* Toggle between blink_on and blink_off */
        if (hsm->base_hsm.current == &state_blink_on) {
            moore_hsm_transition(&hsm->base_hsm, &state_blink_off);
        } else {
            moore_hsm_transition(&hsm->base_hsm, &state_blink_on);
        }
    }
    
    /* Calculate PWM based on state */
    if (current == &state_led_off) {
        memset(hsm->pwm_values, 0, PWM_CHANNELS);
    } else if (current == &state_effect_static) {
        calculate_static_pwm(hsm);
    } else if (current == &state_effect_breath || 
               current == &state_breath_up || 
               current == &state_breath_down) {
        calculate_breath_pwm(hsm);
    } else if (current == &state_effect_blink || 
               current == &state_blink_on || 
               current == &state_blink_off) {
        /* Determine if we're in blink_on or blink_off */
        int on = (hsm->base_hsm.current == &state_blink_on);
        calculate_blink_pwm(hsm, on);
    } else if (current == &state_effect_flow || 
               (current >= &state_flow_pos[0] && current <= &state_flow_pos[3])) {
        calculate_flow_pwm(hsm);
    } else if (current == &state_effect_gradient) {
        /* For now, same as static */
        calculate_static_pwm(hsm);
    } else if (current == &state_effect_rainbow) {
        /* Simple rainbow: rotate colors */
        static uint8_t hue = 0;
        hue++;
        /* Convert HSV to RGB (simplified) */
        hsm->config.color.r = (hue % 3 == 0) ? 255 : 0;
        hsm->config.color.g = (hue % 3 == 1) ? 255 : 0;
        hsm->config.color.b = (hue % 3 == 2) ? 255 : 0;
        calculate_static_pwm(hsm);
    }
    
    /* Update hardware via callback */
    if (hsm->pwm_update_cb) {
        for (int i = 0; i < PWM_CHANNELS; i++) {
            hsm->pwm_update_cb(i, hsm->pwm_values[i]);
        }
    }
}

uint8_t led_hsm_get_pwm(led_hsm_t *hsm, uint8_t channel)
{
    if (!hsm || channel >= PWM_CHANNELS) return 0;
    return hsm->pwm_values[channel];
}

int led_hsm_process_command(led_hsm_t *hsm, uint8_t cmd, 
                           const uint8_t *data, uint8_t len)
{
    if (!hsm) return -1;
    
    switch (cmd) {
        case 0x01: /* Power on */
            led_hsm_dispatch_event(hsm, EV_LED_POWER_ON);
            break;
        case 0x02: /* Power off */
            led_hsm_dispatch_event(hsm, EV_LED_POWER_OFF);
            break;
        case 0x03: /* Set effect */
            if (len >= 1) {
                led_hsm_set_effect(hsm, (led_effect_t)data[0]);
            }
            break;
        case 0x04: /* Set color */
            if (len >= 3) {
                led_hsm_set_color(hsm, data[0], data[1], data[2]);
            }
            break;
        case 0x05: /* Set brightness */
            if (len >= 1) {
                led_hsm_set_brightness(hsm, data[0]);
            }
            break;
        case 0x06: /* Set speed */
            if (len >= 1) {
                led_hsm_set_speed(hsm, data[0]);
            }
            break;
        default:
            return -1;
    }
    
    return 0;
}
