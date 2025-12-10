/**
 * system_coordinator.h  2025-12-05
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
 * System Coordinator FSM for TWS Earphone System.
 * Coordinates multiple subsystem state machines.
 */
#ifndef __SYSTEM_COORDINATOR_H__
#define __SYSTEM_COORDINATOR_H__

#include "mycommon.h"
#include "mytrace.h"

/* Forward declarations */
struct recording_fsm;
struct comm_fsm;
struct power_fsm;
struct audio_fsm;
struct efsm_protocol;
struct ota_fsm;

/** System states */
enum system_state {
    SYS_INIT,
    SYS_IDLE,
    SYS_RECORDING,           /* Only recording (no network) */
    SYS_RECORDING_AND_UPLOADING, /* Recording + streaming transfer */
    SYS_UPLOADING,           /* Only uploading (recording stopped) */
    SYS_ERROR,
    SYS_SLEEP,
    SYS_SHUTDOWN
};

/** System events */
enum system_event {
    SYS_EVT_POWER_ON,
    SYS_EVT_POWER_OFF,
    SYS_EVT_REC_START,
    SYS_EVT_REC_STOP,
    SYS_EVT_REC_PAUSE,
    SYS_EVT_REC_RESUME,
    SYS_EVT_UPLOAD_START,
    SYS_EVT_UPLOAD_COMPLETE,
    SYS_EVT_NETWORK_CONNECTED,
    SYS_EVT_NETWORK_DISCONNECTED,
    SYS_EVT_BLUETOOTH_CONNECTED,
    SYS_EVT_BLUETOOTH_DISCONNECTED,
    SYS_EVT_LOW_BATTERY,
    SYS_EVT_CHARGING,
    SYS_EVT_ERROR,
    SYS_EVT_RESET,
    /* New events for streaming and transfer mode */
    SYS_EVT_TRANSFER_MODE_CHANGE,   /* Transfer mode changed */
    SYS_EVT_STREAMING_START,        /* Start streaming */
    SYS_EVT_STREAMING_STOP,         /* Stop streaming */
    SYS_EVT_FILE_UPLOAD_START,      /* Start file upload */
    SYS_EVT_FILE_UPLOAD_COMPLETE,   /* File upload complete */
    /* OTA events */
    SYS_EVT_OTA_START,              /* Start OTA update */
    SYS_EVT_OTA_CANCEL,             /* Cancel OTA update */
    SYS_EVT_OTA_CHECK,              /* Check for OTA updates */
    SYS_EVT_OTA_STATUS,             /* Get OTA status */
    SYS_EVT_OTA_PROGRESS,           /* OTA progress update */
    SYS_EVT_OTA_COMPLETE,           /* OTA update complete */
    SYS_EVT_OTA_ERROR               /* OTA error */
};

/** System coordinator context */
struct system_coordinator {
    enum system_state current_state;
    enum system_state previous_state;
    
    /* Subsystem FSMs */
    struct recording_fsm *recording_fsm;
    struct comm_fsm *comm_fsm;
    struct power_fsm *power_fsm;
    struct audio_fsm *audio_fsm;
    struct efsm_protocol *efsm_protocol;  /* EFSM Protocol State Machine */
    struct ota_fsm *ota_fsm;              /* OTA FSM */
    
    /* User data */
    void *user_data;
    
    /* Error handling */
    int error_code;
    char error_msg[128];
};

/** System coordinator operations */
struct system_coordinator_ops {
    void (*init)(struct system_coordinator *sys);
    void (*destroy)(struct system_coordinator *sys);
    
    /* Event handlers */
    void (*handle_event)(struct system_coordinator *sys, enum system_event event, void *data);
    
    /* State transition handlers */
    void (*enter_init)(struct system_coordinator *sys);
    void (*enter_idle)(struct system_coordinator *sys);
    void (*enter_recording)(struct system_coordinator *sys);
    void (*enter_recording_and_uploading)(struct system_coordinator *sys);
    void (*enter_uploading)(struct system_coordinator *sys);
    void (*enter_error)(struct system_coordinator *sys);
    void (*enter_sleep)(struct system_coordinator *sys);
    
    /* Exit handlers */
    void (*exit_state)(struct system_coordinator *sys);
    
    /* Do actions */
    void (*do_action)(struct system_coordinator *sys);
};

/** Main API functions */
void system_coordinator_init(struct system_coordinator *sys);
void system_coordinator_destroy(struct system_coordinator *sys);
void system_coordinator_dispatch_event(struct system_coordinator *sys, enum system_event event, void *data);
enum system_state system_coordinator_get_state(struct system_coordinator *sys);
const char *system_coordinator_get_state_name(enum system_state state);
const char *system_coordinator_get_event_name(enum system_event event);

/** Error handling */
void system_coordinator_set_error(struct system_coordinator *sys, int code, const char *msg);
int system_coordinator_get_error_code(struct system_coordinator *sys);
const char *system_coordinator_get_error_msg(struct system_coordinator *sys);

/** Subsystem management */
void system_coordinator_set_recording_fsm(struct system_coordinator *sys, struct recording_fsm *fsm);
void system_coordinator_set_comm_fsm(struct system_coordinator *sys, struct comm_fsm *fsm);
void system_coordinator_set_power_fsm(struct system_coordinator *sys, struct power_fsm *fsm);
void system_coordinator_set_audio_fsm(struct system_coordinator *sys, struct audio_fsm *fsm);
void system_coordinator_set_efsm_protocol(struct system_coordinator *sys, struct efsm_protocol *efsm);
void system_coordinator_set_ota_fsm(struct system_coordinator *sys, struct ota_fsm *fsm);

#endif /* __SYSTEM_COORDINATOR_H__ */
