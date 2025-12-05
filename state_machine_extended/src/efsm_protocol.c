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

/** State function prototypes */
static void idle_process(struct efsm_context *ctx);
static void idle_entry(struct efsm_context *ctx);
static void idle_exit(struct efsm_context *ctx);

static void receiving_process(struct efsm_context *ctx);
static void receiving_entry(struct efsm_context *ctx);
static void receiving_exit(struct efsm_context *ctx);

static void processing_process(struct efsm_context *ctx);
static void processing_entry(struct efsm_context *ctx);
static void processing_exit(struct efsm_context *ctx);

static void sending_process(struct efsm_context *ctx);
static void sending_entry(struct efsm_context *ctx);
static void sending_exit(struct efsm_context *ctx);

static void waiting_ack_process(struct efsm_context *ctx);
static void waiting_ack_entry(struct efsm_context *ctx);
static void waiting_ack_exit(struct efsm_context *ctx);

/** State objects */
static struct efsm_state idle_state = {
    .process = idle_process,
    .on_entry = idle_entry,
    .on_exit = idle_exit,
};

static struct efsm_state receiving_state = {
    .process = receiving_process,
    .on_entry = receiving_entry,
    .on_exit = receiving_exit,
};

static struct efsm_state processing_state = {
    .process = processing_process,
    .on_entry = processing_entry,
    .on_exit = processing_exit,
};

static struct efsm_state sending_state = {
    .process = sending_process,
    .on_entry = sending_entry,
    .on_exit = sending_exit,
};

static struct efsm_state waiting_ack_state = {
    .process = waiting_ack_process,
    .on_entry = waiting_ack_entry,
    .on_exit = waiting_ack_exit,
};

/** Internal helper to change state */
static void change_state(struct efsm_processor *p, struct efsm_state *new_state) {
    if (p->current_state && p->current_state->on_exit) {
        p->current_state->on_exit(&p->context);
    }
    p->current_state = new_state;
    if (new_state && new_state->on_entry) {
        new_state->on_entry(&p->context);
    }
}

/** Event types */
typedef enum {
    EVENT_PACKET_ARRIVAL,
    EVENT_PACKET_COMPLETE,
    EVENT_VALID,
    EVENT_INVALID,
    EVENT_SENT,
    EVENT_ACK_RECEIVED,
    EVENT_TIMEOUT,
} efsm_event_t;

/** Process events based on current state */
static void handle_event(struct efsm_processor *p, efsm_event_t event, void *data) {
    if (p->current_state == &idle_state) {
        if (event == EVENT_PACKET_ARRIVAL) {
            _MY_TRACE_STR("Idle -> Receiving (packet_arrival)\n");
            change_state(p, &receiving_state);
            // Store packet
            p->context.buffer = data;
            p->context.sequence_number++;
        }
    } else if (p->current_state == &receiving_state) {
        if (event == EVENT_PACKET_COMPLETE) {
            _MY_TRACE_STR("Receiving -> Processing (packet_complete)\n");
            change_state(p, &processing_state);
        }
    } else if (p->current_state == &processing_state) {
        if (event == EVENT_VALID) {
            _MY_TRACE_STR("Processing -> Sending (valid)\n");
            change_state(p, &sending_state);
        } else if (event == EVENT_INVALID) {
            _MY_TRACE_STR("Processing -> Idle (invalid)\n");
            change_state(p, &idle_state);
            p->context.buffer = NULL;
        }
    } else if (p->current_state == &sending_state) {
        if (event == EVENT_SENT) {
            _MY_TRACE_STR("Sending -> WaitingAck (sent)\n");
            change_state(p, &waiting_ack_state);
            p->context.timeout_counter = 10; // 10 ticks timeout
        }
    } else if (p->current_state == &waiting_ack_state) {
        if (event == EVENT_ACK_RECEIVED) {
            _MY_TRACE_STR("WaitingAck -> Idle (ack_received)\n");
            change_state(p, &idle_state);
            p->context.buffer = NULL;
            p->context.timeout_counter = 0;
        } else if (event == EVENT_TIMEOUT) {
            _MY_TRACE_STR("WaitingAck -> Sending (timeout)\n");
            change_state(p, &sending_state);
            p->context.timeout_counter = 10;
        }
    }
}

/** State implementations */

static void idle_process(struct efsm_context *ctx) {
    _MY_TRACE_STR("idle_process\n");
    // Do nothing, waiting for packet
}

static void idle_entry(struct efsm_context *ctx) {
    _MY_TRACE_STR("idle_entry\n");
}

static void idle_exit(struct efsm_context *ctx) {
    _MY_TRACE_STR("idle_exit\n");
}

static void receiving_process(struct efsm_context *ctx) {
    _MY_TRACE_STR("receiving_process\n");
    // Simulate receiving data
}

static void receiving_entry(struct efsm_context *ctx) {
    _MY_TRACE_STR("receiving_entry\n");
}

static void receiving_exit(struct efsm_context *ctx) {
    _MY_TRACE_STR("receiving_exit\n");
}

static void processing_process(struct efsm_context *ctx) {
    _MY_TRACE_STR("processing_process\n");
    // Validate packet, decide valid/invalid
}

static void processing_entry(struct efsm_context *ctx) {
    _MY_TRACE_STR("processing_entry\n");
}

static void processing_exit(struct efsm_context *ctx) {
    _MY_TRACE_STR("processing_exit\n");
}

static void sending_process(struct efsm_context *ctx) {
    _MY_TRACE_STR("sending_process\n");
    // Send packet
}

static void sending_entry(struct efsm_context *ctx) {
    _MY_TRACE_STR("sending_entry\n");
}

static void sending_exit(struct efsm_context *ctx) {
    _MY_TRACE_STR("sending_exit\n");
}

static void waiting_ack_process(struct efsm_context *ctx) {
    _MY_TRACE_STR("waiting_ack_process\n");
    // Wait for ACK
}

static void waiting_ack_entry(struct efsm_context *ctx) {
    _MY_TRACE_STR("waiting_ack_entry\n");
}

static void waiting_ack_exit(struct efsm_context *ctx) {
    _MY_TRACE_STR("waiting_ack_exit\n");
}

/** Public API */

void efsm_processor_init(struct efsm_processor *p) {
    memset(p, 0, sizeof(*p));
    p->current_state = &idle_state;
    p->context.sequence_number = 0;
    p->context.timeout_counter = 0;
    p->context.buffer = NULL;
    if (p->current_state->on_entry) {
        p->current_state->on_entry(&p->context);
    }
}

void efsm_processor_dispatch_packet(struct efsm_processor *p, void *packet) {
    _MY_TRACE_STR("efsm_processor_dispatch_packet\n");
    // For simplicity, treat packet arrival as an event
    handle_event(p, EVENT_PACKET_ARRIVAL, packet);
    // Process current state
    if (p->current_state && p->current_state->process) {
        p->current_state->process(&p->context);
    }
}

void efsm_processor_tick(struct efsm_processor *p) {
    _MY_TRACE_STR("efsm_processor_tick\n");
    // Handle timeouts etc.
    if (p->context.timeout_counter > 0) {
        p->context.timeout_counter--;
        if (p->context.timeout_counter == 0) {
            // Timeout event
            handle_event(p, EVENT_TIMEOUT, NULL);
        }
    }
}

/** Additional event functions for testing */
void efsm_processor_packet_complete(struct efsm_processor *p) {
    handle_event(p, EVENT_PACKET_COMPLETE, NULL);
}

void efsm_processor_valid(struct efsm_processor *p) {
    handle_event(p, EVENT_VALID, NULL);
}

void efsm_processor_invalid(struct efsm_processor *p) {
    handle_event(p, EVENT_INVALID, NULL);
}

void efsm_processor_sent(struct efsm_processor *p) {
    handle_event(p, EVENT_SENT, NULL);
}

void efsm_processor_ack_received(struct efsm_processor *p) {
    handle_event(p, EVENT_ACK_RECEIVED, NULL);
}
