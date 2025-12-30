/**
 * comm_fsm.h  2025-12-08
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
 * Communication FSM for TWS Earphone System.
 * Handles Bluetooth/Wi-Fi connectivity and data transfer.
 */
#ifndef __COMM_FSM_H__
#define __COMM_FSM_H__

#include "mycommon.h"
#include "mytrace.h"

/** Communication states */
enum comm_state {
    COMM_IDLE,
    COMM_CONNECTING,
    COMM_CONNECTED,
    COMM_TRANSFERRING,
    COMM_DISCONNECTING,
    COMM_ERROR
};

/** Communication events */
enum comm_event {
    COMM_EVT_CONNECT,           /* Request to connect */
    COMM_EVT_DISCONNECT,        /* Request to disconnect */
    COMM_EVT_CONNECTED,         /* Connection established */
    COMM_EVT_DISCONNECTED,      /* Connection lost */
    COMM_EVT_DATA_READY,        /* Data ready to send */
    COMM_EVT_DATA_SENT,         /* Data sent successfully */
    COMM_EVT_DATA_RECEIVED,     /* Data received */
    COMM_EVT_TIMEOUT,           /* Operation timeout */
    COMM_EVT_ERROR              /* Communication error */
};

/** Communication protocol */
enum comm_protocol {
    COMM_PROTOCOL_BLUETOOTH,
    COMM_PROTOCOL_WIFI,
    COMM_PROTOCOL_USB
};

/** Communication context */
struct comm_fsm {
    enum comm_state current_state;
    enum comm_state previous_state;
    
    /* Communication configuration */
    enum comm_protocol protocol;
    char device_address[32];
    int port;
    int baud_rate;
    
    /* Connection status */
    int is_connected;
    int signal_strength;  /* 0-100 */
    int data_rate;        /* bytes per second */
    
    /* Transfer data */
    unsigned long bytes_sent;
    unsigned long bytes_received;
    unsigned long transfer_start_time;
    
    /* Error handling */
    int error_code;
    char error_msg[128];
    
    /* User data */
    void *user_data;
    
    /* Operations */
    const struct comm_fsm_ops *ops;
};

/** Communication FSM operations */
struct comm_fsm_ops {
    void (*init)(struct comm_fsm *comm);
    void (*destroy)(struct comm_fsm *comm);
    
    /* Event handlers */
    void (*handle_event)(struct comm_fsm *comm, enum comm_event event, void *data);
    
    /* State transition handlers */
    void (*enter_idle)(struct comm_fsm *comm);
    void (*enter_connecting)(struct comm_fsm *comm);
    void (*enter_connected)(struct comm_fsm *comm);
    void (*enter_transferring)(struct comm_fsm *comm);
    void (*enter_disconnecting)(struct comm_fsm *comm);
    void (*enter_error)(struct comm_fsm *comm);
    
    /* Exit handlers */
    void (*exit_state)(struct comm_fsm *comm);
    
    /* Do actions */
    void (*do_action)(struct comm_fsm *comm);
};

/** Main API functions */
void comm_fsm_init(struct comm_fsm *comm);
void comm_fsm_destroy(struct comm_fsm *comm);
void comm_fsm_dispatch_event(struct comm_fsm *comm, enum comm_event event, void *data);
enum comm_state comm_fsm_get_state(struct comm_fsm *comm);
const char *comm_fsm_get_state_name(enum comm_state state);
const char *comm_fsm_get_event_name(enum comm_event event);

/** Configuration functions */
void comm_fsm_set_protocol(struct comm_fsm *comm, enum comm_protocol protocol);
void comm_fsm_set_device_address(struct comm_fsm *comm, const char *address);
void comm_fsm_set_port(struct comm_fsm *comm, int port);
void comm_fsm_set_baud_rate(struct comm_fsm *comm, int baud_rate);

/** Status functions */
int comm_fsm_is_connected(struct comm_fsm *comm);
int comm_fsm_is_transferring(struct comm_fsm *comm);
int comm_fsm_get_signal_strength(struct comm_fsm *comm);
unsigned long comm_fsm_get_bytes_sent(struct comm_fsm *comm);
unsigned long comm_fsm_get_bytes_received(struct comm_fsm *comm);

/** Transfer functions */
void comm_fsm_start_transfer(struct comm_fsm *comm, const void *data, size_t size);
void comm_fsm_stop_transfer(struct comm_fsm *comm);

/** Upload monitoring */
int comm_fsm_get_upload_speed(struct comm_fsm *comm);        /* in kbps */
int comm_fsm_get_upload_progress(struct comm_fsm *comm);     /* 0-100% */

/** Reset function */
void comm_fsm_reset(struct comm_fsm *comm);

/** Error handling */
void comm_fsm_set_error(struct comm_fsm *comm, int code, const char *msg);
int comm_fsm_get_error_code(struct comm_fsm *comm);
const char *comm_fsm_get_error_msg(struct comm_fsm *comm);

#endif /* __COMM_FSM_H__ */
