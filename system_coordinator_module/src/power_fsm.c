/**
 * power_fsm.c  2025-12-30
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
 * Power Management FSM implementation for TWS Earphone System.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mycommon.h"
#include "mytrace.h"
#include "power_fsm.h"

/* State names */
static const char *state_names[] = {
    "POWER_OFF",
    "POWER_ON",
    "POWER_LOW_BATTERY",
    "POWER_CHARGING",
    "POWER_SLEEP"
};

/* Event names */
static const char *event_names[] = {
    "POWER_EVT_TURN_ON",
    "POWER_EVT_TURN_OFF",
    "POWER_EVT_LOW_BATTERY",
    "POWER_EVT_CHARGING_START",
    "POWER_EVT_CHARGING_STOP",
    "POWER_EVT_ENTER_SLEEP",
    "POWER_EVT_WAKE_UP"
};

/* Internal helper functions */
static void handle_off(struct power_fsm *pwr, enum power_event event, void *data);
static void handle_on(struct power_fsm *pwr, enum power_event event, void *data);
static void handle_low_battery(struct power_fsm *pwr, enum power_event event, void *data);
static void handle_charging(struct power_fsm *pwr, enum power_event event, void *data);
static void handle_sleep(struct power_fsm *pwr, enum power_event event, void *data);

static void enter_off(struct power_fsm *pwr);
static void enter_on(struct power_fsm *pwr);
static void enter_low_battery(struct power_fsm *pwr);
static void enter_charging(struct power_fsm *pwr);
static void enter_sleep(struct power_fsm *pwr);

static void exit_state(struct power_fsm *pwr);
static void do_action(struct power_fsm *pwr);

/* State transition helper */
static void transition_to(struct power_fsm *pwr, enum power_state new_state);

/* Initialize power FSM */
void power_fsm_init(struct power_fsm *pwr)
{
    if (!pwr) return;
    
    memset(pwr, 0, sizeof(*pwr));
    pwr->current_state = POWER_OFF;
    pwr->previous_state = POWER_OFF;
    pwr->user_data = NULL;
    
    _MY_TRACE_STR("power_fsm_init: initialized\n");
}

/* Destroy power FSM */
void power_fsm_destroy(struct power_fsm *pwr)
{
    if (!pwr) return;
    _MY_TRACE_STR("power_fsm_destroy: cleaning up\n");
    /* Nothing to free for now */
}

/* Dispatch event to current state */
void power_fsm_dispatch_event(struct power_fsm *pwr, enum power_event event, void *data)
{
    if (!pwr) return;
    
    _MY_TRACE_STR("power_fsm_dispatch_event: state=%s, event=%s\n",
                  power_fsm_get_state_name(pwr->current_state),
                  power_fsm_get_event_name(event));
    
    switch (pwr->current_state) {
        case POWER_OFF:
            handle_off(pwr, event, data);
            break;
        case POWER_ON:
            handle_on(pwr, event, data);
            break;
        case POWER_LOW_BATTERY:
            handle_low_battery(pwr, event, data);
            break;
        case POWER_CHARGING:
            handle_charging(pwr, event, data);
            break;
        case POWER_SLEEP:
            handle_sleep(pwr, event, data);
            break;
        default:
            _MY_TRACE_STR("Unknown state: %d\n", pwr->current_state);
            break;
    }
    
    /* Perform do action after handling event */
    do_action(pwr);
}

/* Get current state */
enum power_state power_fsm_get_state(struct power_fsm *pwr)
{
    return pwr ? pwr->current_state : POWER_OFF;
}

/* Get state name */
const char *power_fsm_get_state_name(enum power_state state)
{
    if (state >= 0 && state < (int)(sizeof(state_names)/sizeof(state_names[0]))) {
        return state_names[state];
    }
    return "UNKNOWN_STATE";
}

/* Get event name */
const char *power_fsm_get_event_name(enum power_event event)
{
    if (event >= 0 && event < (int)(sizeof(event_names)/sizeof(event_names[0]))) {
        return event_names[event];
    }
    return "UNKNOWN_EVENT";
}

/* State transition helper */
static void transition_to(struct power_fsm *pwr, enum power_state new_state)
{
    if (!pwr || pwr->current_state == new_state) return;
    
    _MY_TRACE_STR("transition_to: %s -> %s\n",
                  power_fsm_get_state_name(pwr->current_state),
                  power_fsm_get_state_name(new_state));
    
    /* Exit current state */
    exit_state(pwr);
    
    /* Update state */
    pwr->previous_state = pwr->current_state;
    pwr->current_state = new_state;
    
    /* Enter new state */
    switch (new_state) {
        case POWER_OFF:
            enter_off(pwr);
            break;
        case POWER_ON:
            enter_on(pwr);
            break;
        case POWER_LOW_BATTERY:
            enter_low_battery(pwr);
            break;
        case POWER_CHARGING:
            enter_charging(pwr);
            break;
        case POWER_SLEEP:
            enter_sleep(pwr);
            break;
        default:
            break;
    }
}

/* State handlers */
static void handle_off(struct power_fsm *pwr, enum power_event event, void *data)
{
    (void)data;
    switch (event) {
        case POWER_EVT_TURN_ON:
            _MY_TRACE_STR("OFF: TURN_ON -> ON\n");
            transition_to(pwr, POWER_ON);
            break;
        case POWER_EVT_CHARGING_START:
            _MY_TRACE_STR("OFF: CHARGING_START -> CHARGING\n");
            transition_to(pwr, POWER_CHARGING);
            break;
        default:
            _MY_TRACE_STR("OFF: event %s ignored\n", power_fsm_get_event_name(event));
            break;
    }
}

static void handle_on(struct power_fsm *pwr, enum power_event event, void *data)
{
    (void)data;
    switch (event) {
        case POWER_EVT_TURN_OFF:
            _MY_TRACE_STR("ON: TURN_OFF -> OFF\n");
            transition_to(pwr, POWER_OFF);
            break;
        case POWER_EVT_LOW_BATTERY:
            _MY_TRACE_STR("ON: LOW_BATTERY -> LOW_BATTERY\n");
            transition_to(pwr, POWER_LOW_BATTERY);
            break;
        case POWER_EVT_CHARGING_START:
            _MY_TRACE_STR("ON: CHARGING_START -> CHARGING\n");
            transition_to(pwr, POWER_CHARGING);
            break;
        case POWER_EVT_ENTER_SLEEP:
            _MY_TRACE_STR("ON: ENTER_SLEEP -> SLEEP\n");
            transition_to(pwr, POWER_SLEEP);
            break;
        default:
            _MY_TRACE_STR("ON: event %s ignored\n", power_fsm_get_event_name(event));
            break;
    }
}

static void handle_low_battery(struct power_fsm *pwr, enum power_event event, void *data)
{
    (void)data;
    switch (event) {
        case POWER_EVT_CHARGING_START:
            _MY_TRACE_STR("LOW_BATTERY: CHARGING_START -> CHARGING\n");
            transition_to(pwr, POWER_CHARGING);
            break;
        case POWER_EVT_TURN_OFF:
            _MY_TRACE_STR("LOW_BATTERY: TURN_OFF -> OFF\n");
            transition_to(pwr, POWER_OFF);
            break;
        case POWER_EVT_ENTER_SLEEP:
            _MY_TRACE_STR("LOW_BATTERY: ENTER_SLEEP -> SLEEP\n");
            transition_to(pwr, POWER_SLEEP);
            break;
        default:
            _MY_TRACE_STR("LOW_BATTERY: event %s ignored\n", power_fsm_get_event_name(event));
            break;
    }
}

static void handle_charging(struct power_fsm *pwr, enum power_event event, void *data)
{
    (void)data;
    switch (event) {
        case POWER_EVT_CHARGING_STOP:
            _MY_TRACE_STR("CHARGING: CHARGING_STOP -> ON\n");
            transition_to(pwr, POWER_ON);
            break;
        case POWER_EVT_TURN_OFF:
            _MY_TRACE_STR("CHARGING: TURN_OFF -> OFF\n");
            transition_to(pwr, POWER_OFF);
            break;
        case POWER_EVT_LOW_BATTERY:
            _MY_TRACE_STR("CHARGING: LOW_BATTERY (stay)\n");
            /* Stay in charging but note low battery */
            break;
        default:
            _MY_TRACE_STR("CHARGING: event %s ignored\n", power_fsm_get_event_name(event));
            break;
    }
}

static void handle_sleep(struct power_fsm *pwr, enum power_event event, void *data)
{
    (void)data;
    switch (event) {
        case POWER_EVT_WAKE_UP:
            _MY_TRACE_STR("SLEEP: WAKE_UP -> ON\n");
            transition_to(pwr, POWER_ON);
            break;
        case POWER_EVT_CHARGING_START:
            _MY_TRACE_STR("SLEEP: CHARGING_START -> CHARGING\n");
            transition_to(pwr, POWER_CHARGING);
            break;
        case POWER_EVT_TURN_OFF:
            _MY_TRACE_STR("SLEEP: TURN_OFF -> OFF\n");
            transition_to(pwr, POWER_OFF);
            break;
        default:
            _MY_TRACE_STR("SLEEP: event %s ignored\n", power_fsm_get_event_name(event));
            break;
    }
}

/* State entry actions */
static void enter_off(struct power_fsm *pwr)
{
    _MY_TRACE_STR("enter_off\n");
    printf("Power: OFF - System powered down\n");
}

static void enter_on(struct power_fsm *pwr)
{
    _MY_TRACE_STR("enter_on\n");
    printf("Power: ON - System operational\n");
}

static void enter_low_battery(struct power_fsm *pwr)
{
    _MY_TRACE_STR("enter_low_battery\n");
    printf("Power: LOW_BATTERY - Battery level critical\n");
}

static void enter_charging(struct power_fsm *pwr)
{
    _MY_TRACE_STR("enter_charging\n");
    printf("Power: CHARGING - Battery charging\n");
}

static void enter_sleep(struct power_fsm *pwr)
{
    _MY_TRACE_STR("enter_sleep\n");
    printf("Power: SLEEP - Low power mode\n");
}

/* State exit action */
static void exit_state(struct power_fsm *pwr)
{
    _MY_TRACE_STR("exit_state: %s\n", power_fsm_get_state_name(pwr->current_state));
    printf("Power: Exiting %s state\n", power_fsm_get_state_name(pwr->current_state));
}

/* Do action (executed in each state) */
static void do_action(struct power_fsm *pwr)
{
    switch (pwr->current_state) {
        case POWER_OFF:
            printf("Power: OFF - no action\n");
            break;
        case POWER_ON:
            printf("Power: ON - monitoring battery\n");
            break;
        case POWER_LOW_BATTERY:
            printf("Power: LOW_BATTERY - warning user\n");
            break;
        case POWER_CHARGING:
            printf("Power: CHARGING - updating battery level\n");
            break;
        case POWER_SLEEP:
            printf("Power: SLEEP - conserving energy\n");
            break;
        default:
            break;
    }
}

/* Get battery level (simulated) */
int power_fsm_get_battery_level(struct power_fsm *pwr)
{
    if (!pwr) return 0;
    /* Simulate battery level based on state */
    switch (pwr->current_state) {
        case POWER_OFF:
            return 0;
        case POWER_ON:
            return 75; /* 75% */
        case POWER_LOW_BATTERY:
            return 15; /* 15% */
        case POWER_CHARGING:
            return 50; /* 50% but charging */
        case POWER_SLEEP:
            return 60; /* 60% */
        default:
            return 0;
    }
}

/* Check if charging */
int power_fsm_is_charging(struct power_fsm *pwr)
{
    if (!pwr) return 0;
    return (pwr->current_state == POWER_CHARGING);
}

/* Reset power FSM to initial state */
void power_fsm_reset(struct power_fsm *pwr)
{
    if (!pwr) return;
    _MY_TRACE_STR("power_fsm_reset\n");
    pwr->current_state = POWER_OFF;
    pwr->previous_state = POWER_OFF;
    /* No other cleanup needed */
}
