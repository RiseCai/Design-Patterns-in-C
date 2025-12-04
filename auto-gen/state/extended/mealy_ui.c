/**
 * mealy_ui.c  2025-12-03
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
 * Mealy UI state machine example implementation.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <mytrace.h>
#include "mealy_ui.h"
#include "../state/mealy_machine.h"

/* Forward declarations */
static void idle_event_click(struct mealy_state *state, struct mealy_machine *m);
static void idle_event_hover(struct mealy_state *state, struct mealy_machine *m);
static void idle_event_keypress(struct mealy_state *state, struct mealy_machine *m, int key);
static void idle_render(struct mealy_state *state, struct mealy_machine *m);

static void hovered_event_click(struct mealy_state *state, struct mealy_machine *m);
static void hovered_event_hover(struct mealy_state *state, struct mealy_machine *m);
static void hovered_render(struct mealy_state *state, struct mealy_machine *m);

static void pressed_event_click(struct mealy_state *state, struct mealy_machine *m);
static void pressed_render(struct mealy_state *state, struct mealy_machine *m);

/* Idle state */
struct mealy_state_idle {
    struct mealy_state base;
};

static struct mealy_state_ops idle_ops = {
    ._destructor = NULL,
    .free = NULL,
    .event_click = idle_event_click,
    .event_hover = idle_event_hover,
    .event_keypress = idle_event_keypress,
    .render = idle_render,
    .__super = NULL,
};

void mealy_state_idle_init(struct mealy_state_idle *s)
{
    memset(s, 0, sizeof(*s));
    s->base.ops = &idle_ops;
}

static void idle_event_click(struct mealy_state *state, struct mealy_machine *m)
{
    _MY_TRACE_STR("idle: click -> transition to pressed\n");
    struct mealy_state_pressed *pressed = malloc(sizeof(*pressed));
    mealy_state_pressed_init(pressed);
    mealy_machine_set_state(m, (struct mealy_state *)pressed);
    /* output action */
    printf("UI: button pressed visual feedback\n");
}

static void idle_event_hover(struct mealy_state *state, struct mealy_machine *m)
{
    _MY_TRACE_STR("idle: hover -> transition to hovered\n");
    struct mealy_state_hovered *hovered = malloc(sizeof(*hovered));
    mealy_state_hovered_init(hovered);
    mealy_machine_set_state(m, (struct mealy_state *)hovered);
    printf("UI: button hover effect\n");
}

static void idle_event_keypress(struct mealy_state *state, struct mealy_machine *m, int key)
{
    _MY_TRACE_STR("idle: keypress ignored\n");
}

static void idle_render(struct mealy_state *state, struct mealy_machine *m)
{
    printf("Rendering idle button\n");
}

/* Hovered state */
struct mealy_state_hovered {
    struct mealy_state base;
};

static struct mealy_state_ops hovered_ops = {
    .event_click = hovered_event_click,
    .event_hover = hovered_event_hover,
    .render = hovered_render,
};

void mealy_state_hovered_init(struct mealy_state_hovered *s)
{
    memset(s, 0, sizeof(*s));
    s->base.ops = &hovered_ops;
}

static void hovered_event_click(struct mealy_state *state, struct mealy_machine *m)
{
    _MY_TRACE_STR("hovered: click -> pressed\n");
    struct mealy_state_pressed *pressed = malloc(sizeof(*pressed));
    mealy_state_pressed_init(pressed);
    mealy_machine_set_state(m, (struct mealy_state *)pressed);
    printf("UI: button pressed from hover\n");
}

static void hovered_event_hover(struct mealy_state *state, struct mealy_machine *m)
{
    /* already hovered, no transition */
    _MY_TRACE_STR("hovered: hover again\n");
}

static void hovered_render(struct mealy_state *state, struct mealy_machine *m)
{
    printf("Rendering hovered button with glow\n");
}

/* Pressed state */
struct mealy_state_pressed {
    struct mealy_state base;
};

static struct mealy_state_ops pressed_ops = {
    .event_click = pressed_event_click,
    .render = pressed_render,
};

void mealy_state_pressed_init(struct mealy_state_pressed *s)
{
    memset(s, 0, sizeof(*s));
    s->base.ops = &pressed_ops;
}

static void pressed_event_click(struct mealy_state *state, struct mealy_machine *m)
{
    _MY_TRACE_STR("pressed: click -> idle\n");
    struct mealy_state_idle *idle = malloc(sizeof(*idle));
    mealy_state_idle_init(idle);
    mealy_machine_set_state(m, (struct mealy_state *)idle);
    printf("UI: button released\n");
}

static void pressed_render(struct mealy_state *state, struct mealy_machine *m)
{
    printf("Rendering pressed button (depressed)\n");
}
