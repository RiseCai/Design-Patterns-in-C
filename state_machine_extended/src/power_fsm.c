/**
 * power_fsm.c - Stub implementation for Power FSM
 */
#include "power_fsm.h"

void power_fsm_init(struct power_fsm *fsm) {
    (void)fsm;
}

void power_fsm_destroy(struct power_fsm *fsm) {
    (void)fsm;
}

void power_fsm_dispatch_event(struct power_fsm *fsm, enum power_event event, void *data) {
    (void)fsm; (void)event; (void)data;
}

enum power_state power_fsm_get_state(struct power_fsm *fsm) {
    (void)fsm;
    return POWER_OFF;
}

const char *power_fsm_get_state_name(enum power_state state) {
    (void)state;
    return "POWER_OFF";
}
