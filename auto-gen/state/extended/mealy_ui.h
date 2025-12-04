/**
 * mealy_ui.h  2025-12-03
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
 * Mealy UI state machine example: Button states.
 */
#ifndef __MEALY_UI_H__
#define __MEALY_UI_H__

#include <mycommon.h>
#include <mytrace.h>

#include "../state/mealy_machine.h"

/** Concrete states */
struct mealy_state_idle {
    struct mealy_state base;
};

struct mealy_state_hovered {
    struct mealy_state base;
};

struct mealy_state_pressed {
    struct mealy_state base;
};

/** State constructors */
void mealy_state_idle_init(struct mealy_state_idle *s);
void mealy_state_hovered_init(struct mealy_state_hovered *s);
void mealy_state_pressed_init(struct mealy_state_pressed *s);

#endif /* __MEALY_UI_H__ */
