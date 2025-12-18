/**
 * led_fsm.h  2025-12-15
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
 * LED FSM for controlling 4 RGB LEDs with 12 PWM channels.
 * Moore hierarchical state machine with external command interface.
 */

#ifndef LED_FSM_H
#define LED_FSM_H

#include <stdint.h>
#include "moore_hierarchical.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== Configuration ==================== */

#define LED_COUNT          4       /* Number of RGB LEDs */
#define PWM_CHANNELS       12      /* 4 LEDs × 3 colors (R,G,B) */
#define MAX_BRIGHTNESS     255     /* 8-bit PWM resolution */

/* Default timing values (configurable) */
#define DEFAULT_PWM_FREQ_HZ    1000    /* 1 kHz PWM frequency */
#define DEFAULT_UPDATE_MS      20      /* 20 ms effect update period */
#define DEFAULT_FLOW_MS        100     /* 100 ms flow step interval */

/* ==================== Effect Types ==================== */

typedef enum {
    EFFECT_NONE = 0,
    EFFECT_STATIC,      /* Solid color */
    EFFECT_BREATH,      /* Breathing (pulse) */
    EFFECT_BLINK,       /* Blinking on/off */
    EFFECT_GRADIENT,    /* Color gradient */
    EFFECT_FLOW,        /* Flow (running light) */
    EFFECT_RAINBOW,     /* Rainbow color cycle */
    EFFECT_COUNT        /* Total number of effects */
} led_effect_t;

/* Flow direction */
typedef enum {
    FLOW_FORWARD = 0,   /* LED0 → LED1 → LED2 → LED3 */
    FLOW_BACKWARD,      /* LED3 → LED2 → LED1 → LED0 */
    FLOW_PINGPONG       /* LED0 → LED3 → LED0 ... */
} flow_direction_t;

/* ==================== Event Definitions ==================== */

enum {
    /* Power control */
    EV_LED_POWER_ON = 1,
    EV_LED_POWER_OFF,
    
    /* Effect selection */
    EV_LED_EFFECT_STATIC,
    EV_LED_EFFECT_BREATH,
    EV_LED_EFFECT_BLINK,
    EV_LED_EFFECT_GRADIENT,
    EV_LED_EFFECT_FLOW,
    EV_LED_EFFECT_RAINBOW,
    
    /* Parameter adjustment */
    EV_LED_BRIGHTNESS_UP,
    EV_LED_BRIGHTNESS_DOWN,
    EV_LED_SPEED_UP,
    EV_LED_SPEED_DOWN,
    EV_LED_COLOR_CHANGE,
    
    /* Timer events */
    EV_LED_TIMER_TICK,      /* PWM update timer */
    EV_LED_FLOW_STEP,       /* Flow effect step */
    EV_LED_BREATH_STEP,     /* Breath effect step */
    
    /* External commands */
    EV_LED_CMD_RECEIVED,
};

/* ==================== Data Structures ==================== */

/* RGB color representation */
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} rgb_color_t;

/* LED configuration */
typedef struct {
    uint8_t brightness;         /* Global brightness 0-255 */
    uint8_t speed;              /* Effect speed 0-255 */
    rgb_color_t color;          /* Current color */
    led_effect_t effect;        /* Current effect */
    flow_direction_t flow_dir;  /* Flow direction */
    uint8_t flow_position;      /* Current position in flow (0-3) */
    uint8_t breath_phase;       /* 0-255 for breath effect */
    uint8_t breath_direction;   /* 0=up, 1=down */
} led_config_t;

/* Extended HSM for LED control */
typedef struct {
    struct moore_hsm base_hsm;          /* Base Moore HSM */
    led_config_t config;                /* LED configuration */
    uint8_t pwm_values[PWM_CHANNELS];   /* Current PWM values for 12 channels */
    uint32_t pwm_freq_hz;               /* PWM frequency in Hz */
    uint32_t update_period_ms;          /* Effect update period in ms */
    uint32_t flow_period_ms;            /* Flow step period in ms */
    void (*pwm_update_cb)(uint8_t ch, uint8_t value); /* PWM update callback */
    void *user_data;                    /* User-defined data (e.g., OS adapter) */
} led_hsm_t;

/* ==================== Public API ==================== */

/**
 * Initialize LED FSM.
 * 
 * @param hsm Pointer to LED HSM structure
 * @param pwm_cb Callback function to update PWM hardware
 */
void led_hsm_init(led_hsm_t *hsm, void (*pwm_cb)(uint8_t ch, uint8_t value));

/**
 * Dispatch event to LED FSM.
 * 
 * @param hsm Pointer to LED HSM
 * @param event Event to dispatch
 */
void led_hsm_dispatch_event(led_hsm_t *hsm, int event);

/**
 * Set LED effect.
 * 
 * @param hsm Pointer to LED HSM
 * @param effect Effect type
 * @return 0 on success, -1 on error
 */
int led_hsm_set_effect(led_hsm_t *hsm, led_effect_t effect);

/**
 * Set LED color.
 * 
 * @param hsm Pointer to LED HSM
 * @param r Red component (0-255)
 * @param g Green component (0-255)
 * @param b Blue component (0-255)
 */
void led_hsm_set_color(led_hsm_t *hsm, uint8_t r, uint8_t g, uint8_t b);

/**
 * Set LED brightness.
 * 
 * @param hsm Pointer to LED HSM
 * @param brightness Brightness value (0-255)
 */
void led_hsm_set_brightness(led_hsm_t *hsm, uint8_t brightness);

/**
 * Set effect speed.
 * 
 * @param hsm Pointer to LED HSM
 * @param speed Speed value (0-255)
 */
void led_hsm_set_speed(led_hsm_t *hsm, uint8_t speed);

/**
 * Set flow direction.
 * 
 * @param hsm Pointer to LED HSM
 * @param direction Flow direction
 */
void led_hsm_set_flow_direction(led_hsm_t *hsm, flow_direction_t direction);

/**
 * Configure timing parameters.
 * 
 * @param hsm Pointer to LED HSM
 * @param pwm_freq_hz PWM frequency in Hz
 * @param update_ms Effect update period in ms
 * @param flow_ms Flow step period in ms
 */
void led_hsm_config_timing(led_hsm_t *hsm, uint32_t pwm_freq_hz,
                           uint32_t update_ms, uint32_t flow_ms);

/**
 * Update PWM values based on current state and configuration.
 * This should be called periodically (e.g., from timer interrupt).
 * 
 * @param hsm Pointer to LED HSM
 */
void led_hsm_update_pwm(led_hsm_t *hsm);

/**
 * Get current PWM value for a channel.
 * 
 * @param hsm Pointer to LED HSM
 * @param channel PWM channel (0-11)
 * @return PWM value (0-255)
 */
uint8_t led_hsm_get_pwm(led_hsm_t *hsm, uint8_t channel);

/**
 * Process external command.
 * 
 * @param hsm Pointer to LED HSM
 * @param cmd Command ID
 * @param data Command data
 * @param len Data length
 * @return 0 on success, -1 on error
 */
int led_hsm_process_command(led_hsm_t *hsm, uint8_t cmd, 
                           const uint8_t *data, uint8_t len);

#ifdef __cplusplus
}
#endif

#endif /* LED_FSM_H */
