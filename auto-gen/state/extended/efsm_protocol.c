/**
 * efsm_protocol.c  2025-12-03
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
 * EFSM protocol implementation.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <mytrace.h>
#include "efsm_protocol.h"

static void idle_process(struct efsm_context *ctx)
{
    _MY_TRACE_STR("idle_process\n");
}

static void idle_entry(struct efsm_context *ctx)
{
    _MY_TRACE_STR("idle_entry\n");
}

static void idle_exit(struct efsm_context *ctx)
{
    _MY_TRACE_STR("idle_exit\n");
}

static struct efsm_state idle_state = {
    .process = idle_process,
    .on_entry = idle_entry,
    .on_exit = idle_exit,
};

void efsm_processor_init(struct efsm_processor *p)
{
    memset(p, 0, sizeof(*p));
    p->current_state = &idle_state;
    p->context.sequence_number = 0;
    p->context.timeout_counter = 0;
    p->context.buffer = NULL;
}

void efsm_processor_dispatch_packet(struct efsm_processor *p, void *packet)
{
    _MY_TRACE_STR("efsm_processor_dispatch_packet\n");
    /* Process packet based on current state */
    if (p->current_state && p->current_state->process) {
        p->current_state->process(&p->context);
    }
}

void efsm_processor_tick(struct efsm_processor *p)
{
    _MY_TRACE_STR("efsm_processor_tick\n");
    /* Handle timeouts etc. */
    if (p->context.timeout_counter > 0) {
        p->context.timeout_counter--;
    }
}
