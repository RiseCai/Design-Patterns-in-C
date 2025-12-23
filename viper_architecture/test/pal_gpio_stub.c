#include "../../platform_abstraction_layer/include/pal_gpio.h"
#include <stdio.h>
#include <stdbool.h>

/* pal_gpio_init is already defined in pal_windows_simulator.c */

#define MAX_PINS 256
static pal_gpio_state_t pin_states[MAX_PINS] = {0};
static bool pin_initialized[MAX_PINS] = {false};

/* Track PAL initialization via a flag that mirrors actual PAL state */
static bool pal_initialized = false;

/* Provide real implementations as weak symbols */
pal_status_t pal_init_real(const pal_config_t *config) __attribute__((weak));
pal_status_t pal_deinit_real(void) __attribute__((weak));

/* Default weak implementations */
static pal_status_t pal_init_real_stub(const pal_config_t *config)
{
    (void)config;
    pal_initialized = true;
    printf("[STUB] pal_init_real\n");
    return PAL_STATUS_OK;
}

static pal_status_t pal_deinit_real_stub(void)
{
    pal_initialized = false;
    printf("[STUB] pal_deinit_real\n");
    return PAL_STATUS_OK;
}

/* Weak aliases to allow overriding */
pal_status_t pal_init_real(const pal_config_t *config) __attribute__((weak, alias("pal_init_real_stub")));
pal_status_t pal_deinit_real(void) __attribute__((weak, alias("pal_deinit_real_stub")));

/* Override pal_init and pal_deinit to track initialization (weak linkage) */
pal_status_t pal_init(const pal_config_t *config) __attribute__((weak));
pal_status_t pal_deinit(void) __attribute__((weak));

static pal_status_t pal_init_stub(const pal_config_t *config)
{
    pal_status_t status = pal_init_real(config);
    if (status == PAL_STATUS_OK) {
        pal_initialized = true;
    }
    return status;
}

static pal_status_t pal_deinit_stub(void)
{
    pal_status_t status = pal_deinit_real();
    pal_initialized = false;
    return status;
}

pal_status_t pal_init(const pal_config_t *config) __attribute__((weak, alias("pal_init_stub")));
pal_status_t pal_deinit(void) __attribute__((weak, alias("pal_deinit_stub")));

pal_status_t pal_gpio_deinit(pal_gpio_pin_t pin)
{
    printf("[STUB] pal_gpio_deinit pin=%u\n", pin);
    if (pin < MAX_PINS) {
        pin_initialized[pin] = false;
    }
    return PAL_STATUS_OK;
}

pal_status_t pal_gpio_write(pal_gpio_pin_t pin, pal_gpio_state_t state)
{
    if (!pal_initialized) {
        return PAL_STATUS_NOT_INITIALIZED;
    }
    printf("[STUB] pal_gpio_write pin=%u state=%d\n", pin, state);
    if (pin < MAX_PINS) {
        pin_states[pin] = state;
        pin_initialized[pin] = true;
    }
    return PAL_STATUS_OK;
}

pal_status_t pal_gpio_read(pal_gpio_pin_t pin, pal_gpio_state_t *state)
{
    if (!pal_initialized) {
        return PAL_STATUS_NOT_INITIALIZED;
    }
    printf("[STUB] pal_gpio_read pin=%u\n", pin);
    if (pin < MAX_PINS && pin_initialized[pin]) {
        if (state) *state = pin_states[pin];
    } else {
        if (state) *state = PAL_GPIO_LOW;
    }
    return PAL_STATUS_OK;
}

pal_status_t pal_gpio_toggle(pal_gpio_pin_t pin)
{
    if (!pal_initialized) {
        return PAL_STATUS_NOT_INITIALIZED;
    }
    printf("[STUB] pal_gpio_toggle pin=%u\n", pin);
    if (pin < MAX_PINS && pin_initialized[pin]) {
        pin_states[pin] = (pin_states[pin] == PAL_GPIO_LOW) ? PAL_GPIO_HIGH : PAL_GPIO_LOW;
    }
    return PAL_STATUS_OK;
}

pal_status_t pal_gpio_set_interrupt_callback(pal_gpio_pin_t pin, 
                                             pal_callback_t callback, 
                                             void *context)
{
    if (!pal_initialized) {
        return PAL_STATUS_NOT_INITIALIZED;
    }
    printf("[STUB] pal_gpio_set_interrupt_callback pin=%u\n", pin);
    return PAL_STATUS_OK;
}

pal_status_t pal_gpio_interrupt_enable(pal_gpio_pin_t pin, bool enable)
{
    if (!pal_initialized) {
        return PAL_STATUS_NOT_INITIALIZED;
    }
    printf("[STUB] pal_gpio_interrupt_enable pin=%u enable=%d\n", pin, enable);
    return PAL_STATUS_OK;
}
