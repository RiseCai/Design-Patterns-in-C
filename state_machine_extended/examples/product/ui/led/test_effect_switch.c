#include <stdio.h>
#include "led_fsm.h"

static void dummy_pwm_cb(uint8_t ch, uint8_t val) {
    // do nothing
}

int main() {
    led_hsm_t hsm;
    led_hsm_init(&hsm, dummy_pwm_cb);
    
    // Power on
    led_hsm_dispatch_event(&hsm, EV_LED_POWER_ON);
    
    // Set to static
    printf("Setting static effect\n");
    led_hsm_set_effect(&hsm, EFFECT_STATIC);
    printf("Current state name: %s\n", hsm.base_hsm.current->name);
    
    // Set to breath
    printf("Setting breath effect\n");
    led_hsm_set_effect(&hsm, EFFECT_BREATH);
    printf("Current state name: %s\n", hsm.base_hsm.current->name);
    
    // Set to flow
    printf("Setting flow effect\n");
    led_hsm_set_effect(&hsm, EFFECT_FLOW);
    printf("Current state name: %s\n", hsm.base_hsm.current->name);
    
    return 0;
}
