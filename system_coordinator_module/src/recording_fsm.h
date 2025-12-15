/**
 * recording_fsm.h  2025-12-05
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
 * Recording Control FSM for TWS Earphone System.
 * Manages recording start/stop and coordinates between earphone and charging case.
 */
#ifndef __RECORDING_FSM_H__
#define __RECORDING_FSM_H__

#include "mycommon.h"
#include "mytrace.h"

/** Recording states */
enum recording_state {
    REC_IDLE,
    REC_WAIT_SYNC,
    REC_RECORDING,
    REC_PAUSED,
    REC_STOPPING,
    REC_ERROR
};

/** Recording events */
enum recording_event {
    REC_EVT_START,          /* Start recording (from earphone or case button) */
    REC_EVT_STOP,           /* Stop recording */
    REC_EVT_PAUSE,          /* Pause recording */
    REC_EVT_RESUME,         /* Resume recording */
    REC_EVT_SYNC_COMPLETE,  /* Device synchronization complete */
    REC_EVT_EARPHONE_READY, /* Earphone ready for recording */
    REC_EVT_CASE_READY,     /* Charging case ready for recording */
    REC_EVT_BUFFER_FULL,    /* Recording buffer full */
    REC_EVT_STORAGE_FULL,   /* Storage full */
    REC_EVT_ERROR           /* Recording error */
};

/** Recording source */
enum recording_source {
    REC_SOURCE_EARPHONE,    /* MIC+speaker mix from earphone */
    REC_SOURCE_CASE,        /* MIC from charging case */
    REC_SOURCE_BOTH         /* Both sources (requires synchronization) */
};

/** Recording context */
struct recording_fsm {
    enum recording_state current_state;
    enum recording_state previous_state;
    
    /* Recording configuration */
    enum recording_source source;
    int sample_rate;
    int bit_depth;
    int channels;
    
    /* Device status */
    int earphone_ready;
    int case_ready;
    
    /* Recording data */
    char filename[256];
    unsigned long file_size;
    unsigned long duration_ms;
    
    /* Error handling */
    int error_code;
    char error_msg[128];
    
    /* User data */
    void *user_data;
};

/** Recording FSM operations */
struct recording_fsm_ops {
    void (*init)(struct recording_fsm *rec);
    void (*destroy)(struct recording_fsm *rec);
    
    /* Event handlers */
    void (*handle_event)(struct recording_fsm *rec, enum recording_event event, void *data);
    
    /* State transition handlers */
    void (*enter_idle)(struct recording_fsm *rec);
    void (*enter_wait_sync)(struct recording_fsm *rec);
    void (*enter_recording)(struct recording_fsm *rec);
    void (*enter_paused)(struct recording_fsm *rec);
    void (*enter_stopping)(struct recording_fsm *rec);
    void (*enter_error)(struct recording_fsm *rec);
    
    /* Exit handlers */
    void (*exit_state)(struct recording_fsm *rec);
    
    /* Do actions */
    void (*do_action)(struct recording_fsm *rec);
};

/** Main API functions */
void recording_fsm_init(struct recording_fsm *rec);
void recording_fsm_destroy(struct recording_fsm *rec);
void recording_fsm_dispatch_event(struct recording_fsm *rec, enum recording_event event, void *data);
enum recording_state recording_fsm_get_state(struct recording_fsm *rec);
const char *recording_fsm_get_state_name(enum recording_state state);
const char *recording_fsm_get_event_name(enum recording_event event);

/** Configuration functions */
void recording_fsm_set_source(struct recording_fsm *rec, enum recording_source source);
void recording_fsm_set_sample_rate(struct recording_fsm *rec, int sample_rate);
void recording_fsm_set_bit_depth(struct recording_fsm *rec, int bit_depth);
void recording_fsm_set_channels(struct recording_fsm *rec, int channels);

/** Status functions */
int recording_fsm_is_recording(struct recording_fsm *rec);
int recording_fsm_is_paused(struct recording_fsm *rec);
int recording_fsm_is_idle(struct recording_fsm *rec);
unsigned long recording_fsm_get_duration(struct recording_fsm *rec);
unsigned long recording_fsm_get_file_size(struct recording_fsm *rec);

/** Error handling */
void recording_fsm_set_error(struct recording_fsm *rec, int code, const char *msg);
int recording_fsm_get_error_code(struct recording_fsm *rec);
const char *recording_fsm_get_error_msg(struct recording_fsm *rec);

/** Device coordination */
void recording_fsm_set_earphone_ready(struct recording_fsm *rec, int ready);
void recording_fsm_set_case_ready(struct recording_fsm *rec, int ready);
int recording_fsm_both_devices_ready(struct recording_fsm *rec);

#endif /* __RECORDING_FSM_H__ */
