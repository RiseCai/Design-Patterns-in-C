/**
 * power_fsm.h - Power FSM interface
 */
#ifndef __POWER_FSM_H__
#define __POWER_FSM_H__

#include "mycommon.h"

/* Power states */
enum power_state {
    POWER_OFF,
    POWER_ON,
    POWER_LOW_BATTERY,
    POWER_CHARGING,
    POWER_SLEEP
};

/* Power events */
enum power_event {
    POWER_EVT_TURN_ON,
    POWER_EVT_TURN_OFF,
    POWER_EVT_LOW_BATTERY,
    POWER_EVT_CHARGING_START,
    POWER_EVT_CHARGING_STOP,
    POWER_EVT_ENTER_SLEEP,
    POWER_EVT_WAKE_UP
};

/* Power FSM context */
struct power_fsm {
    enum power_state current_state;
    enum power_state previous_state;
    void *user_data;
};

/* API functions */
void power_fsm_init(struct power_fsm *fsm);
void power_fsm_destroy(struct power_fsm *fsm);
void power_fsm_dispatch_event(struct power_fsm *fsm, enum power_event event, void *data);
enum power_state power_fsm_get_state(struct power_fsm *fsm);
const char *power_fsm_get_state_name(enum power_state state);
const char *power_fsm_get_event_name(enum power_event event);
int power_fsm_get_battery_level(struct power_fsm *fsm);
int power_fsm_is_charging(struct power_fsm *fsm);
void power_fsm_reset(struct power_fsm *fsm);

#endif /* __POWER_FSM_H__ */
