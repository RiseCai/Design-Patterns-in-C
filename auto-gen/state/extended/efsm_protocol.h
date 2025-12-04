/**
 * efsm_protocol.h  2025-12-03
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
 * Extended Finite State Machine for communication protocol with three-stage processing.
 */
#ifndef __EFSM_PROTOCOL_H__
#define __EFSM_PROTOCOL_H__

#include <mycommon.h>
#include <mytrace.h>

/** EFSM context with extended variables */
struct efsm_context {
    int sequence_number;
    int timeout_counter;
    void *buffer;
};

/** EFSM state */
struct efsm_state {
    void (*process)(struct efsm_context *ctx);
    void (*on_entry)(struct efsm_context *ctx);
    void (*on_exit)(struct efsm_context *ctx);
};

/** Three-stage processor: input, process, output */
struct efsm_processor {
    struct efsm_state *current_state;
    struct efsm_context context;
};

void efsm_processor_init(struct efsm_processor *p);
void efsm_processor_dispatch_packet(struct efsm_processor *p, void *packet);
void efsm_processor_tick(struct efsm_processor *p);

#endif /* __EFSM_PROTOCOL_H__ */
