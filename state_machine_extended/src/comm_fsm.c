/**
 * comm_fsm.c  2025-12-08
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
 * Communication FSM implementation for TWS Earphone System.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mycommon.h"
#include "mytrace.h"
#include "comm_fsm.h"

/* State names */
static const char *state_names[] = {
    "COMM_IDLE",
    "COMM_CONNECTING",
    "COMM_CONNECTED",
    "COMM_TRANSFERRING",
    "COMM_DISCONNECTING",
    "COMM_ERROR"
};

/* Event names */
static const char *event_names[] = {
    "COMM_EVT_CONNECT",
    "COMM_EVT_DISCONNECT",
    "COMM_EVT_CONNECTED",
    "COMM_EVT_DISCONNECTED",
    "COMM_EVT_DATA_READY",
    "COMM_EVT_DATA_SENT",
    "COMM_EVT_DATA_RECEIVED",
    "COMM_EVT_TIMEOUT",
    "COMM_EVT_ERROR"
};

/* Protocol names */
static const char *protocol_names[] = {
    "COMM_PROTOCOL_BLUETOOTH",
    "COMM_PROTOCOL_WIFI",
    "COMM_PROTOCOL_USB"
};

/* Internal helper functions */
static void handle_idle(struct comm_fsm *comm, enum comm_event event, void *data);
static void handle_connecting(struct comm_fsm *comm, enum comm_event event, void *data);
static void handle_connected(struct comm_fsm *comm, enum comm_event event, void *data);
static void handle_transferring(struct comm_fsm *comm, enum comm_event event, void *data);
static void handle_disconnecting(struct comm_fsm *comm, enum comm_event event, void *data);
static void handle_error(struct comm_fsm *comm, enum comm_event event, void *data);

static void enter_idle(struct comm_fsm *comm);
static void enter_connecting(struct comm_fsm *comm);
static void enter_connected(struct comm_fsm *comm);
static void enter_transferring(struct comm_fsm *comm);
static void enter_disconnecting(struct comm_fsm *comm);
static void enter_error(struct comm_fsm *comm);

static void exit_state(struct comm_fsm *comm);
static void do_action(struct comm_fsm *comm);

/* Default operations */
static struct comm_fsm_ops default_ops = {
    .init = NULL,
    .destroy = NULL,
    .handle_event = NULL,
    .enter_idle = enter_idle,
    .enter_connecting = enter_connecting,
    .enter_connected = enter_connected,
    .enter_transferring = enter_transferring,
    .enter_disconnecting = enter_disconnecting,
    .enter_error = enter_error,
    .exit_state = exit_state,
    .do_action = do_action
};

/* Initialize communication FSM */
void comm_fsm_init(struct comm_fsm *comm)
{
    if (!comm) return;
    
    memset(comm, 0, sizeof(*comm));
    comm->current_state = COMM_IDLE;
    comm->previous_state = COMM_IDLE;
    comm->protocol = COMM_PROTOCOL_BLUETOOTH;
    comm->signal_strength = 0;
    comm->data_rate = 0;
    comm->is_connected = 0;
    comm->error_code = 0;
    comm->user_data = NULL;
    
    /* Set default operations */
    comm->ops = &default_ops;
    
    _MY_TRACE_STR("comm_fsm_init: initialized\n");
}

/* Destroy communication FSM */
void comm_fsm_destroy(struct comm_fsm *comm)
{
    if (!comm) return;
    _MY_TRACE_STR("comm_fsm_destroy: cleaning up\n");
    /* Nothing to free for now */
}

/* Dispatch event to current state */
void comm_fsm_dispatch_event(struct comm_fsm *comm, enum comm_event event, void *data)
{
    if (!comm) return;
    
    _MY_TRACE_STR("comm_fsm_dispatch_event: state=%s, event=%s\n",
                  comm_fsm_get_state_name(comm->current_state),
                  comm_fsm_get_event_name(event));
    
    switch (comm->current_state) {
        case COMM_IDLE:
            handle_idle(comm, event, data);
            break;
        case COMM_CONNECTING:
            handle_connecting(comm, event, data);
            break;
        case COMM_CONNECTED:
            handle_connected(comm, event, data);
            break;
        case COMM_TRANSFERRING:
            handle_transferring(comm, event, data);
            break;
        case COMM_DISCONNECTING:
            handle_disconnecting(comm, event, data);
            break;
        case COMM_ERROR:
            handle_error(comm, event, data);
            break;
        default:
            _MY_TRACE_STR("Unknown state: %d\n", comm->current_state);
            break;
    }
    
    /* Perform do action after handling event */
    if (comm->ops && comm->ops->do_action) {
        comm->ops->do_action(comm);
    }
}

/* Get current state */
enum comm_state comm_fsm_get_state(struct comm_fsm *comm)
{
    return comm ? comm->current_state : COMM_ERROR;
}

/* Get state name */
const char *comm_fsm_get_state_name(enum comm_state state)
{
    if (state >= 0 && state < (int)(sizeof(state_names)/sizeof(state_names[0]))) {
        return state_names[state];
    }
    return "UNKNOWN_STATE";
}

/* Get event name */
const char *comm_fsm_get_event_name(enum comm_event event)
{
    if (event >= 0 && event < (int)(sizeof(event_names)/sizeof(event_names[0]))) {
        return event_names[event];
    }
    return "UNKNOWN_EVENT";
}

/* Configuration functions */
void comm_fsm_set_protocol(struct comm_fsm *comm, enum comm_protocol protocol)
{
    if (!comm) return;
    comm->protocol = protocol;
    _MY_TRACE_STR("comm_fsm_set_protocol: %s\n", protocol_names[protocol]);
}

void comm_fsm_set_device_address(struct comm_fsm *comm, const char *address)
{
    if (!comm || !address) return;
    strncpy(comm->device_address, address, sizeof(comm->device_address)-1);
    comm->device_address[sizeof(comm->device_address)-1] = '\0';
    _MY_TRACE_STR("comm_fsm_set_device_address: %s\n", address);
}

void comm_fsm_set_port(struct comm_fsm *comm, int port)
{
    if (!comm) return;
    comm->port = port;
    _MY_TRACE_STR("comm_fsm_set_port: %d\n", port);
}

void comm_fsm_set_baud_rate(struct comm_fsm *comm, int baud_rate)
{
    if (!comm) return;
    comm->baud_rate = baud_rate;
    _MY_TRACE_STR("comm_fsm_set_baud_rate: %d\n", baud_rate);
}

/* Status functions */
int comm_fsm_is_connected(struct comm_fsm *comm)
{
    return comm ? comm->is_connected : 0;
}

int comm_fsm_is_transferring(struct comm_fsm *comm)
{
    return comm ? (comm->current_state == COMM_TRANSFERRING) : 0;
}

int comm_fsm_get_signal_strength(struct comm_fsm *comm)
{
    return comm ? comm->signal_strength : 0;
}

unsigned long comm_fsm_get_bytes_sent(struct comm_fsm *comm)
{
    return comm ? comm->bytes_sent : 0;
}

unsigned long comm_fsm_get_bytes_received(struct comm_fsm *comm)
{
    return comm ? comm->bytes_received : 0;
}

/* Transfer functions */
void comm_fsm_start_transfer(struct comm_fsm *comm, const void *data, size_t size)
{
    if (!comm) return;
    _MY_TRACE_STR("comm_fsm_start_transfer: size=%zu\n", size);
    /* In real implementation, this would start data transfer */
    comm->bytes_sent += size;
    comm_fsm_dispatch_event(comm, COMM_EVT_DATA_READY, NULL);
}

void comm_fsm_stop_transfer(struct comm_fsm *comm)
{
    if (!comm) return;
    _MY_TRACE_STR("comm_fsm_stop_transfer\n");
    comm_fsm_dispatch_event(comm, COMM_EVT_DATA_SENT, NULL);
}

/* Error handling */
void comm_fsm_set_error(struct comm_fsm *comm, int code, const char *msg)
{
    if (!comm) return;
    comm->error_code = code;
    strncpy(comm->error_msg, msg, sizeof(comm->error_msg)-1);
    comm->error_msg[sizeof(comm->error_msg)-1] = '\0';
    _MY_TRACE_STR("comm_fsm_set_error: code=%d, msg=%s\n", code, msg);
    comm_fsm_dispatch_event(comm, COMM_EVT_ERROR, NULL);
}

int comm_fsm_get_error_code(struct comm_fsm *comm)
{
    return comm ? comm->error_code : 0;
}

const char *comm_fsm_get_error_msg(struct comm_fsm *comm)
{
    return comm ? comm->error_msg : "";
}

/* State transition helper */
static void transition_to(struct comm_fsm *comm, enum comm_state new_state)
{
    if (!comm || comm->current_state == new_state) return;
    
    _MY_TRACE_STR("transition_to: %s -> %s\n",
                  comm_fsm_get_state_name(comm->current_state),
                  comm_fsm_get_state_name(new_state));
    
    /* Exit current state */
    if (comm->ops && comm->ops->exit_state) {
        comm->ops->exit_state(comm);
    }
    
    /* Update state */
    comm->previous_state = comm->current_state;
    comm->current_state = new_state;
    
    /* Enter new state */
    switch (new_state) {
        case COMM_IDLE:
            if (comm->ops && comm->ops->enter_idle) comm->ops->enter_idle(comm);
            break;
        case COMM_CONNECTING:
            if (comm->ops && comm->ops->enter_connecting) comm->ops->enter_connecting(comm);
            break;
        case COMM_CONNECTED:
            if (comm->ops && comm->ops->enter_connected) comm->ops->enter_connected(comm);
            break;
        case COMM_TRANSFERRING:
            if (comm->ops && comm->ops->enter_transferring) comm->ops->enter_transferring(comm);
            break;
        case COMM_DISCONNECTING:
            if (comm->ops && comm->ops->enter_disconnecting) comm->ops->enter_disconnecting(comm);
            break;
        case COMM_ERROR:
            if (comm->ops && comm->ops->enter_error) comm->ops->enter_error(comm);
            break;
        default:
            break;
    }
}

/* State handlers */
static void handle_idle(struct comm_fsm *comm, enum comm_event event, void *data)
{
    (void)data;
    switch (event) {
        case COMM_EVT_CONNECT:
            _MY_TRACE_STR("IDLE: CONNECT -> CONNECTING\n");
            transition_to(comm, COMM_CONNECTING);
            break;
        case COMM_EVT_ERROR:
            _MY_TRACE_STR("IDLE: ERROR -> ERROR\n");
            transition_to(comm, COMM_ERROR);
            break;
        default:
            _MY_TRACE_STR("IDLE: event %s ignored\n", comm_fsm_get_event_name(event));
            break;
    }
}

static void handle_connecting(struct comm_fsm *comm, enum comm_event event, void *data)
{
    (void)data;
    switch (event) {
        case COMM_EVT_CONNECTED:
            _MY_TRACE_STR("CONNECTING: CONNECTED -> CONNECTED\n");
            transition_to(comm, COMM_CONNECTED);
            break;
        case COMM_EVT_TIMEOUT:
        case COMM_EVT_ERROR:
            _MY_TRACE_STR("CONNECTING: %s -> ERROR\n", comm_fsm_get_event_name(event));
            transition_to(comm, COMM_ERROR);
            break;
        case COMM_EVT_DISCONNECT:
            _MY_TRACE_STR("CONNECTING: DISCONNECT -> DISCONNECTING\n");
            transition_to(comm, COMM_DISCONNECTING);
            break;
        default:
            _MY_TRACE_STR("CONNECTING: event %s ignored\n", comm_fsm_get_event_name(event));
            break;
    }
}

static void handle_connected(struct comm_fsm *comm, enum comm_event event, void *data)
{
    (void)data;
    switch (event) {
        case COMM_EVT_DATA_READY:
            _MY_TRACE_STR("CONNECTED: DATA_READY -> TRANSFERRING\n");
            transition_to(comm, COMM_TRANSFERRING);
            break;
        case COMM_EVT_DISCONNECT:
            _MY_TRACE_STR("CONNECTED: DISCONNECT -> DISCONNECTING\n");
            transition_to(comm, COMM_DISCONNECTING);
            break;
        case COMM_EVT_ERROR:
            _MY_TRACE_STR("CONNECTED: ERROR -> ERROR\n");
            transition_to(comm, COMM_ERROR);
            break;
        default:
            _MY_TRACE_STR("CONNECTED: event %s ignored\n", comm_fsm_get_event_name(event));
            break;
    }
}

static void handle_transferring(struct comm_fsm *comm, enum comm_event event, void *data)
{
    (void)data;
    switch (event) {
        case COMM_EVT_DATA_SENT:
            _MY_TRACE_STR("TRANSFERRING: DATA_SENT -> CONNECTED\n");
            transition_to(comm, COMM_CONNECTED);
            break;
        case COMM_EVT_DATA_RECEIVED:
            _MY_TRACE_STR("TRANSFERRING: DATA_RECEIVED (stay)\n");
            /* Update bytes received */
            if (comm) comm->bytes_received += 1024; /* Example increment */
            break;
        case COMM_EVT_DISCONNECT:
            _MY_TRACE_STR("TRANSFERRING: DISCONNECT -> DISCONNECTING\n");
            transition_to(comm, COMM_DISCONNECTING);
            break;
        case COMM_EVT_ERROR:
            _MY_TRACE_STR("TRANSFERRING: ERROR -> ERROR\n");
            transition_to(comm, COMM_ERROR);
            break;
        default:
            _MY_TRACE_STR("TRANSFERRING: event %s ignored\n", comm_fsm_get_event_name(event));
            break;
    }
}

static void handle_disconnecting(struct comm_fsm *comm, enum comm_event event, void *data)
{
    (void)data;
    switch (event) {
        case COMM_EVT_DISCONNECTED:
            _MY_TRACE_STR("DISCONNECTING: DISCONNECTED -> IDLE\n");
            transition_to(comm, COMM_IDLE);
            break;
        case COMM_EVT_ERROR:
            _MY_TRACE_STR("DISCONNECTING: ERROR -> ERROR\n");
            transition_to(comm, COMM_ERROR);
            break;
        default:
            _MY_TRACE_STR("DISCONNECTING: event %s ignored\n", comm_fsm_get_event_name(event));
            break;
    }
}

static void handle_error(struct comm_fsm *comm, enum comm_event event, void *data)
{
    (void)data;
    switch (event) {
        case COMM_EVT_DISCONNECT:
            _MY_TRACE_STR("ERROR: DISCONNECT -> DISCONNECTING\n");
            transition_to(comm, COMM_DISCONNECTING);
            break;
        case COMM_EVT_CONNECT:
            _MY_TRACE_STR("ERROR: CONNECT -> CONNECTING\n");
            transition_to(comm, COMM_CONNECTING);
            break;
        default:
            _MY_TRACE_STR("ERROR: event %s ignored\n", comm_fsm_get_event_name(event));
            break;
    }
}

/* State entry actions */
static void enter_idle(struct comm_fsm *comm)
{
    _MY_TRACE_STR("enter_idle\n");
    comm->is_connected = 0;
    comm->signal_strength = 0;
    printf("Communication: Idle - Ready to connect\n");
}

static void enter_connecting(struct comm_fsm *comm)
{
    _MY_TRACE_STR("enter_connecting\n");
    comm->is_connected = 0;
    printf("Communication: Connecting to %s via %s\n",
           comm->device_address,
           protocol_names[comm->protocol]);
}

static void enter_connected(struct comm_fsm *comm)
{
    _MY_TRACE_STR("enter_connected\n");
    comm->is_connected = 1;
    comm->signal_strength = 85; /* Example signal strength */
    printf("Communication: Connected - Signal strength %d%%\n", comm->signal_strength);
}

static void enter_transferring(struct comm_fsm *comm)
{
    _MY_TRACE_STR("enter_transferring\n");
    comm->transfer_start_time = 0; /* Would be set to current time */
    printf("Communication: Transferring data\n");
}

static void enter_disconnecting(struct comm_fsm *comm)
{
    _MY_TRACE_STR("enter_disconnecting\n");
    comm->is_connected = 0;
    printf("Communication: Disconnecting...\n");
}

static void enter_error(struct comm_fsm *comm)
{
    _MY_TRACE_STR("enter_error\n");
    comm->is_connected = 0;
    printf("Communication: Error - %s (code %d)\n", comm->error_msg, comm->error_code);
}

/* State exit action */
static void exit_state(struct comm_fsm *comm)
{
    _MY_TRACE_STR("exit_state: %s\n", comm_fsm_get_state_name(comm->current_state));
    printf("Communication: Exiting %s state\n", comm_fsm_get_state_name(comm->current_state));
}

/* Do action (executed in each state) */
static void do_action(struct comm_fsm *comm)
{
    switch (comm->current_state) {
        case COMM_IDLE:
            printf("Communication: Idle - monitoring for connections\n");
            break;
        case COMM_CONNECTING:
            printf("Communication: Connecting - attempting handshake\n");
            break;
        case COMM_CONNECTED:
            printf("Communication: Connected - monitoring connection\n");
            break;
        case COMM_TRANSFERRING:
            printf("Communication: Transferring - processing data\n");
            break;
        case COMM_DISCONNECTING:
            printf("Communication: Disconnecting - cleaning up\n");
            break;
        case COMM_ERROR:
            printf("Communication: Error - waiting for recovery\n");
            break;
        default:
            break;
    }
}
