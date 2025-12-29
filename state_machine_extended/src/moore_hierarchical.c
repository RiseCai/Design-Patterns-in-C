/**
 * moore_hierarchical.c  2025-12-03
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
 * Complex Moore hierarchical state machine example: Smart Thermostat.
 * Demonstrates hierarchical states, event handling, and Moore outputs.
 */
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <mytrace.h>
#include "moore_hierarchical.h"

/* Forward declarations of states */
struct moore_state state_off;
struct moore_state state_on;
struct moore_state state_heating;
struct moore_state state_cooling;
struct moore_state state_idle;
struct moore_state state_fan_only;

/* Event definitions */
enum {
    EV_POWER_ON = 1,
    EV_POWER_OFF,
    EV_TEMP_HIGH,
    EV_TEMP_LOW,
    EV_TEMP_OK,
    EV_FAN_ON,
    EV_FAN_OFF,
};

/* Helper to add child states */
static void add_child(struct moore_state *parent, struct moore_state *child)
{
    if (parent->child_count < 5) {
        parent->children[parent->child_count] = child;
        parent->child_count++;
        child->parent = parent;
    }
}

/* State actions */
static void off_entry(void) { printf("[OFF] entry: system shutdown\n"); }
static void off_do(void) { printf("[OFF] do: no operation\n"); }
static int off_handle_event(struct moore_state *self, int event)
{
    if (event == EV_POWER_ON) {
        printf("[OFF] handle: power on -> transition to ON\n");
        return 1; /* request transition */
    }
    return 0;
}

static void on_entry(void) { printf("[ON] entry: system startup\n"); }
static void on_exit(void) { printf("[ON] exit: leaving ON\n"); }
static void on_do(void) { printf("[ON] do: monitoring temperature\n"); }
static int on_handle_event(struct moore_state *self, int event)
{
    if (event == EV_POWER_OFF) {
        printf("[ON] handle: power off -> transition to OFF\n");
        return 1;
    }
    if (event == EV_TEMP_HIGH) {
        printf("[ON] handle: temp high -> transition to COOLING\n");
        return 2;
    }
    if (event == EV_TEMP_LOW) {
        printf("[ON] handle: temp low -> transition to HEATING\n");
        return 3;
    }
    return 0;
}

static void heating_entry(void) { printf("[HEATING] entry: activate heater\n"); }
static void heating_exit(void) { printf("[HEATING] exit: deactivate heater\n"); }
static void heating_do(void) { printf("[HEATING] do: heating...\n"); }
static int heating_handle_event(struct moore_state *self, int event)
{
    if (event == EV_TEMP_OK) {
        printf("[HEATING] handle: temp ok -> transition to IDLE\n");
        return 1;
    }
    if (event == EV_POWER_OFF) {
        printf("[HEATING] handle: power off -> transition to OFF (via ON)\n");
        return 2;
    }
    return 0;
}

static void cooling_entry(void) { printf("[COOLING] entry: activate cooler\n"); }
static void cooling_exit(void) { printf("[COOLING] exit: deactivate cooler\n"); }
static void cooling_do(void) { printf("[COOLING] do: cooling...\n"); }
static int cooling_handle_event(struct moore_state *self, int event)
{
    if (event == EV_TEMP_OK) {
        printf("[COOLING] handle: temp ok -> transition to IDLE\n");
        return 1;
    }
    return 0;
}

static void idle_entry(void) { printf("[IDLE] entry: maintain temperature\n"); }
static void idle_do(void) { printf("[IDLE] do: temperature stable\n"); }
static int idle_handle_event(struct moore_state *self, int event)
{
    if (event == EV_TEMP_HIGH) {
        printf("[IDLE] handle: temp high -> transition to COOLING\n");
        return 1;
    }
    if (event == EV_TEMP_LOW) {
        printf("[IDLE] handle: temp low -> transition to HEATING\n");
        return 2;
    }
    if (event == EV_FAN_ON) {
        printf("[IDLE] handle: fan on -> transition to FAN_ONLY\n");
        return 3;
    }
    return 0;
}

static void fan_only_entry(void) { printf("[FAN_ONLY] entry: fan running\n"); }
static void fan_only_exit(void) { printf("[FAN_ONLY] exit: fan stopped\n"); }
static void fan_only_do(void) { printf("[FAN_ONLY] do: circulating air\n"); }
static int fan_only_handle_event(struct moore_state *self, int event)
{
    if (event == EV_FAN_OFF) {
        printf("[FAN_ONLY] handle: fan off -> transition to IDLE\n");
        return 1;
    }
    return 0;
}

/* Initialize all states */
static void init_states(void)
{
    /* Off state */
    state_off.name = "Off";
    state_off.parent = NULL;
    state_off.child_count = 0;
    state_off.entry_action = off_entry;
    state_off.exit_action = NULL;
    state_off.do_action = off_do;
    state_off.handle_event = off_handle_event;

    /* On state (superstate) */
    state_on.name = "On";
    state_on.parent = NULL;
    state_on.child_count = 0;
    state_on.entry_action = on_entry;
    state_on.exit_action = on_exit;
    state_on.do_action = on_do;
    state_on.handle_event = on_handle_event;

    /* Substates of On */
    state_heating.name = "Heating";
    state_heating.parent = &state_on;
    state_heating.child_count = 0;
    state_heating.entry_action = heating_entry;
    state_heating.exit_action = heating_exit;
    state_heating.do_action = heating_do;
    state_heating.handle_event = heating_handle_event;

    state_cooling.name = "Cooling";
    state_cooling.parent = &state_on;
    state_cooling.child_count = 0;
    state_cooling.entry_action = cooling_entry;
    state_cooling.exit_action = cooling_exit;
    state_cooling.do_action = cooling_do;
    state_cooling.handle_event = cooling_handle_event;

    state_idle.name = "Idle";
    state_idle.parent = &state_on;
    state_idle.child_count = 0;
    state_idle.entry_action = idle_entry;
    state_idle.exit_action = NULL;
    state_idle.do_action = idle_do;
    state_idle.handle_event = idle_handle_event;

    state_fan_only.name = "FanOnly";
    state_fan_only.parent = &state_on;
    state_fan_only.child_count = 0;
    state_fan_only.entry_action = fan_only_entry;
    state_fan_only.exit_action = fan_only_exit;
    state_fan_only.do_action = fan_only_do;
    state_fan_only.handle_event = fan_only_handle_event;

    /* Build hierarchy */
    add_child(&state_on, &state_heating);
    add_child(&state_on, &state_cooling);
    add_child(&state_on, &state_idle);
    add_child(&state_on, &state_fan_only);
}

/* Implementation of HSM functions */
void moore_hsm_init(struct moore_hsm *hsm, struct moore_state *root)
{
    memset(hsm, 0, sizeof(*hsm));
    init_states();
    hsm->root = root;
    hsm->current = root;
    hsm->temperature = 22;
    hsm->target_temp = 20;
    if (root && root->entry_action) {
        root->entry_action();
    }
}

void moore_hsm_set_temperature(struct moore_hsm *hsm, int temp)
{
    hsm->temperature = temp;
    printf("Temperature updated: %d°C\n", temp);
}

void moore_hsm_set_target(struct moore_hsm *hsm, int target)
{
    hsm->target_temp = target;
    printf("Target temperature updated: %d°C\n", target);
}

struct moore_state *moore_hsm_find_state(struct moore_hsm *hsm, const char *name)
{
    /* Simple linear search; for real use, implement a map */
    struct moore_state *states[] = {&state_off, &state_on, &state_heating,
                                    &state_cooling, &state_idle, &state_fan_only};
    for (int i = 0; i < 6; i++) {
        if (strcmp(states[i]->name, name) == 0) {
            return states[i];
        }
    }
    return NULL;
}

static void transition_to(struct moore_hsm *hsm, struct moore_state *target)
{
    if (hsm->current == target) return;
    if (hsm->current && hsm->current->exit_action) {
        hsm->current->exit_action();
    }
    hsm->current = target;
    if (target && target->entry_action) {
        target->entry_action();
    }
}

void moore_hsm_transition(struct moore_hsm *hsm, struct moore_state *target)
{
    _MY_TRACE_STR("moore_hsm_transition\n");
    transition_to(hsm, target);
}

void moore_hsm_dispatch_event(struct moore_hsm *hsm, int event)
{
    _MY_TRACE_STR("moore_hsm_dispatch_event\n");
    /* Let current state handle event */
    if (hsm->current && hsm->current->handle_event) {
        int result = hsm->current->handle_event(hsm->current, event);
        /* Simple transition logic based on result */
        switch (result) {
            case 1: /* transition to Off */
                transition_to(hsm, &state_off);
                break;
            case 2: /* transition to On */
                transition_to(hsm, &state_on);
                break;
            case 3: /* transition to Heating */
                transition_to(hsm, &state_heating);
                break;
            case 4: /* transition to Cooling */
                transition_to(hsm, &state_cooling);
                break;
            case 5: /* transition to Idle */
                transition_to(hsm, &state_idle);
                break;
            case 6: /* transition to FanOnly */
                transition_to(hsm, &state_fan_only);
                break;
            default:
                /* no transition */
                break;
        }
    }
    /* Execute Moore output of current state */
    if (hsm->current && hsm->current->do_action) {
        hsm->current->do_action();
    }
}

/* Example usage */
int main_example(void)
{
    struct moore_hsm hsm;
    moore_hsm_init(&hsm, &state_off);
    printf("\n--- Starting Thermostat HSM ---\n");
    moore_hsm_dispatch_event(&hsm, EV_POWER_ON);
    moore_hsm_dispatch_event(&hsm, EV_TEMP_LOW);
    moore_hsm_dispatch_event(&hsm, EV_TEMP_OK);
    moore_hsm_dispatch_event(&hsm, EV_FAN_ON);
    moore_hsm_dispatch_event(&hsm, EV_FAN_OFF);
    moore_hsm_dispatch_event(&hsm, EV_POWER_OFF);
    return 0;
}
