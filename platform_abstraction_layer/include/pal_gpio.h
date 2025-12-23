/**
 * pal_gpio.h - Platform Abstraction Layer GPIO Interface
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Auto generate for Design-Patterns in C *
 * 
 * GPIO (General Purpose Input/Output) abstraction for embedded hardware.
 */

#ifndef __PAL_GPIO_H__
#define __PAL_GPIO_H__

#include "pal.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== GPIO Configuration ==================== */

/**
 * GPIO pin numbers
 * Note: These are logical pin numbers, mapped to physical pins per platform
 */
typedef uint16_t pal_gpio_pin_t;

/**
 * GPIO pin modes
 */
typedef enum {
    PAL_GPIO_MODE_INPUT = 0,      /* Input mode */
    PAL_GPIO_MODE_OUTPUT,         /* Output mode */
    PAL_GPIO_MODE_INPUT_PULLUP,   /* Input with internal pull-up */
    PAL_GPIO_MODE_INPUT_PULLDOWN, /* Input with internal pull-down */
    PAL_GPIO_MODE_ANALOG,         /* Analog mode */
    PAL_GPIO_MODE_ALTERNATE       /* Alternate function mode */
} pal_gpio_mode_t;

/**
 * GPIO pin states
 */
typedef enum {
    PAL_GPIO_LOW = 0,             /* Logic low (0V) */
    PAL_GPIO_HIGH = 1             /* Logic high (3.3V/5V) */
} pal_gpio_state_t;

/**
 * GPIO interrupt triggers
 */
typedef enum {
    PAL_GPIO_INT_NONE = 0,        /* No interrupt */
    PAL_GPIO_INT_RISING,          /* Rising edge trigger */
    PAL_GPIO_INT_FALLING,         /* Falling edge trigger */
    PAL_GPIO_INT_BOTH,            /* Both edges trigger */
    PAL_GPIO_INT_LEVEL_HIGH,      /* Level high trigger */
    PAL_GPIO_INT_LEVEL_LOW        /* Level low trigger */
} pal_gpio_int_t;

/**
 * GPIO configuration structure
 */
typedef struct {
    pal_gpio_mode_t mode;         /* Pin mode */
    pal_gpio_int_t interrupt;     /* Interrupt configuration */
    uint32_t debounce_ms;         /* Debounce time in milliseconds (0 = disabled) */
    bool initial_state;           /* Initial state for output pins */
} pal_gpio_config_t;

/* ==================== LED Matrix Support ==================== */

/**
 * LED matrix configuration
 */
typedef struct {
    uint8_t rows;                 /* Number of rows in matrix */
    uint8_t columns;              /* Number of columns in matrix */
    pal_gpio_pin_t *row_pins;     /* Array of row pin numbers */
    pal_gpio_pin_t *column_pins;  /* Array of column pin numbers */
    bool common_cathode;          /* True for common cathode, false for common anode */
    uint8_t brightness;           /* Brightness level (0-255) */
} pal_led_matrix_config_t;

/**
 * LED matrix handle
 */
typedef void* pal_led_matrix_handle_t;

/* ==================== Public GPIO API ==================== */

/**
 * Initialize a GPIO pin
 * 
 * @param pin GPIO pin number
 * @param config GPIO configuration
 * @return PAL_STATUS_OK on success, error code otherwise
 */
pal_status_t pal_gpio_init(pal_gpio_pin_t pin, const pal_gpio_config_t *config);

/**
 * Deinitialize a GPIO pin
 * 
 * @param pin GPIO pin number
 * @return PAL_STATUS_OK on success, error code otherwise
 */
pal_status_t pal_gpio_deinit(pal_gpio_pin_t pin);

/**
 * Write to a GPIO pin
 * 
 * @param pin GPIO pin number
 * @param state Pin state to write
 * @return PAL_STATUS_OK on success, error code otherwise
 */
pal_status_t pal_gpio_write(pal_gpio_pin_t pin, pal_gpio_state_t state);

/**
 * Read from a GPIO pin
 * 
 * @param pin GPIO pin number
 * @param state Pointer to store pin state
 * @return PAL_STATUS_OK on success, error code otherwise
 */
pal_status_t pal_gpio_read(pal_gpio_pin_t pin, pal_gpio_state_t *state);

/**
 * Toggle a GPIO pin
 * 
 * @param pin GPIO pin number
 * @return PAL_STATUS_OK on success, error code otherwise
 */
pal_status_t pal_gpio_toggle(pal_gpio_pin_t pin);

/**
 * Set GPIO interrupt callback
 * 
 * @param pin GPIO pin number
 * @param callback Callback function to call on interrupt
 * @param context User context passed to callback
 * @return PAL_STATUS_OK on success, error code otherwise
 */
pal_status_t pal_gpio_set_interrupt_callback(pal_gpio_pin_t pin, 
                                             pal_callback_t callback, 
                                             void *context);

/**
 * Enable/disable GPIO interrupt
 * 
 * @param pin GPIO pin number
 * @param enable True to enable, false to disable
 * @return PAL_STATUS_OK on success, error code otherwise
 */
pal_status_t pal_gpio_interrupt_enable(pal_gpio_pin_t pin, bool enable);

/* ==================== LED Matrix API ==================== */

/**
 * Initialize an LED matrix
 * 
 * @param config LED matrix configuration
 * @param handle Pointer to store LED matrix handle
 * @return PAL_STATUS_OK on success, error code otherwise
 */
pal_status_t pal_led_matrix_init(const pal_led_matrix_config_t *config,
                                 pal_led_matrix_handle_t *handle);

/**
 * Deinitialize an LED matrix
 * 
 * @param handle LED matrix handle
 * @return PAL_STATUS_OK on success, error code otherwise
 */
pal_status_t pal_led_matrix_deinit(pal_led_matrix_handle_t handle);

/**
 * Set LED matrix pixel
 * 
 * @param handle LED matrix handle
 * @param row Row index (0-based)
 * @param column Column index (0-based)
 * @param state True to turn on, false to turn off
 * @return PAL_STATUS_OK on success, error code otherwise
 */
pal_status_t pal_led_matrix_set_pixel(pal_led_matrix_handle_t handle,
                                      uint8_t row, uint8_t column,
                                      bool state);

/**
 * Clear entire LED matrix
 * 
 * @param handle LED matrix handle
 * @return PAL_STATUS_OK on success, error code otherwise
 */
pal_status_t pal_led_matrix_clear(pal_led_matrix_handle_t handle);

/**
 * Update LED matrix display (for multiplexed matrices)
 * 
 * @param handle LED matrix handle
 * @return PAL_STATUS_OK on success, error code otherwise
 */
pal_status_t pal_led_matrix_update(pal_led_matrix_handle_t handle);

/**
 * Set LED matrix brightness
 * 
 * @param handle LED matrix handle
 * @param brightness Brightness level (0-255)
 * @return PAL_STATUS_OK on success, error code otherwise
 */
pal_status_t pal_led_matrix_set_brightness(pal_led_matrix_handle_t handle,
                                           uint8_t brightness);

#ifdef __cplusplus
}
#endif

#endif /* __PAL_GPIO_H__ */
