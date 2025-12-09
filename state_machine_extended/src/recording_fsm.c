/**
 * recording_fsm.c  2025-12-09
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
 * Recording Control FSM implementation for TWS Earphone System.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "mycommon.h"
#include "mytrace.h"
#include "recording_fsm.h"

/* State names */
static const char *state_names[] = {
    "REC_IDLE",
    "REC_WAIT_SYNC",
    "REC_RECORDING",
    "REC_PAUSED",
    "REC_STOPPING",
    "REC_ERROR"
};

/* Event names */
static const char *event_names[] = {
    "REC_EVT_START",
    "REC_EVT_STOP",
    "REC_EVT_PAUSE",
    "REC_EVT_RESUME",
    "REC_EVT_SYNC_COMPLETE",
    "REC_EVT_EARPHONE_READY",
    "REC_EVT_CASE_READY",
    "REC_EVT_BUFFER_FULL",
    "REC_EVT_STORAGE_FULL",
    "REC_EVT_ERROR"
};

/* Source names */
static const char *source_names[] = {
    "REC_SOURCE_EARPHONE",
    "REC_SOURCE_CASE",
    "REC_SOURCE_BOTH"
};

/* Internal helper functions */
static void handle_idle(struct recording_fsm *rec, enum recording_event event, void *data);
static void handle_wait_sync(struct recording_fsm *rec, enum recording_event event, void *data);
static void handle_recording(struct recording_fsm *rec, enum recording_event event, void *data);
static void handle_paused(struct recording_fsm *rec, enum recording_event event, void *data);
static void handle_stopping(struct recording_fsm *rec, enum recording_event event, void *data);
static void handle_error(struct recording_fsm *rec, enum recording_event event, void *data);

static void enter_idle(struct recording_fsm *rec);
static void enter_wait_sync(struct recording_fsm *rec);
static void enter_recording(struct recording_fsm *rec);
static void enter_paused(struct recording_fsm *rec);
static void enter_stopping(struct recording_fsm *rec);
static void enter_error(struct recording_fsm *rec);

static void exit_state(struct recording_fsm *rec);
static void do_action(struct recording_fsm *rec);


/* Initialize recording FSM */
void recording_fsm_init(struct recording_fsm *rec)
{
    if (!rec) return;
    
    memset(rec, 0, sizeof(*rec));
    rec->current_state = REC_IDLE;
    rec->previous_state = REC_IDLE;
    rec->source = REC_SOURCE_EARPHONE;
    rec->sample_rate = 44100;
    rec->bit_depth = 16;
    rec->channels = 2;
    rec->earphone_ready = 0;
    rec->case_ready = 0;
    rec->error_code = 0;
    rec->user_data = NULL;
    
    /* Generate default filename */
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(rec->filename, sizeof(rec->filename), "recording_%Y%m%d_%H%M%S.wav", tm_info);
    
    _MY_TRACE_STR("recording_fsm_init: initialized\n");
}

/* Destroy recording FSM */
void recording_fsm_destroy(struct recording_fsm *rec)
{
    if (!rec) return;
    _MY_TRACE_STR("recording_fsm_destroy: cleaning up\n");
    /* Nothing to free for now */
}

/* Dispatch event to current state */
void recording_fsm_dispatch_event(struct recording_fsm *rec, enum recording_event event, void *data)
{
    if (!rec) return;
    
    _MY_TRACE_STR("recording_fsm_dispatch_event: state=%s, event=%s\n",
                  recording_fsm_get_state_name(rec->current_state),
                  recording_fsm_get_event_name(event));
    
    switch (rec->current_state) {
        case REC_IDLE:
            handle_idle(rec, event, data);
            break;
        case REC_WAIT_SYNC:
            handle_wait_sync(rec, event, data);
            break;
        case REC_RECORDING:
            handle_recording(rec, event, data);
            break;
        case REC_PAUSED:
            handle_paused(rec, event, data);
            break;
        case REC_STOPPING:
            handle_stopping(rec, event, data);
            break;
        case REC_ERROR:
            handle_error(rec, event, data);
            break;
        default:
            _MY_TRACE_STR("Unknown state: %d\n", rec->current_state);
            break;
    }
    
    /* Perform do action after handling event */
    do_action(rec);
}

/* Get current state */
enum recording_state recording_fsm_get_state(struct recording_fsm *rec)
{
    return rec ? rec->current_state : REC_ERROR;
}

/* Get state name */
const char *recording_fsm_get_state_name(enum recording_state state)
{
    if (state >= 0 && state < (int)(sizeof(state_names)/sizeof(state_names[0]))) {
        return state_names[state];
    }
    return "UNKNOWN_STATE";
}

/* Get event name */
const char *recording_fsm_get_event_name(enum recording_event event)
{
    if (event >= 0 && event < (int)(sizeof(event_names)/sizeof(event_names[0]))) {
        return event_names[event];
    }
    return "UNKNOWN_EVENT";
}

/* Configuration functions */
void recording_fsm_set_source(struct recording_fsm *rec, enum recording_source source)
{
    if (!rec) return;
    rec->source = source;
    _MY_TRACE_STR("recording_fsm_set_source: %s\n", source_names[source]);
}

void recording_fsm_set_sample_rate(struct recording_fsm *rec, int sample_rate)
{
    if (!rec) return;
    rec->sample_rate = sample_rate;
    _MY_TRACE_STR("recording_fsm_set_sample_rate: %d\n", sample_rate);
}

void recording_fsm_set_bit_depth(struct recording_fsm *rec, int bit_depth)
{
    if (!rec) return;
    rec->bit_depth = bit_depth;
    _MY_TRACE_STR("recording_fsm_set_bit_depth: %d\n", bit_depth);
}

void recording_fsm_set_channels(struct recording_fsm *rec, int channels)
{
    if (!rec) return;
    rec->channels = channels;
    _MY_TRACE_STR("recording_fsm_set_channels: %d\n", channels);
}

/* Status functions */
int recording_fsm_is_recording(struct recording_fsm *rec)
{
    return rec ? (rec->current_state == REC_RECORDING) : 0;
}

int recording_fsm_is_paused(struct recording_fsm *rec)
{
    return rec ? (rec->current_state == REC_PAUSED) : 0;
}

int recording_fsm_is_idle(struct recording_fsm *rec)
{
    return rec ? (rec->current_state == REC_IDLE) : 0;
}

unsigned long recording_fsm_get_duration(struct recording_fsm *rec)
{
    return rec ? rec->duration_ms : 0;
}

unsigned long recording_fsm_get_file_size(struct recording_fsm *rec)
{
    return rec ? rec->file_size : 0;
}

/* Error handling */
void recording_fsm_set_error(struct recording_fsm *rec, int code, const char *msg)
{
    if (!rec) return;
    rec->error_code = code;
    strncpy(rec->error_msg, msg, sizeof(rec->error_msg)-1);
    rec->error_msg[sizeof(rec->error_msg)-1] = '\0';
    _MY_TRACE_STR("recording_fsm_set_error: code=%d, msg=%s\n", code, msg);
    recording_fsm_dispatch_event(rec, REC_EVT_ERROR, NULL);
}

int recording_fsm_get_error_code(struct recording_fsm *rec)
{
    return rec ? rec->error_code : 0;
}

const char *recording_fsm_get_error_msg(struct recording_fsm *rec)
{
    return rec ? rec->error_msg : "";
}

/* Device coordination */
void recording_fsm_set_earphone_ready(struct recording_fsm *rec, int ready)
{
    if (!rec) return;
    rec->earphone_ready = ready;
    _MY_TRACE_STR("recording_fsm_set_earphone_ready: %d\n", ready);
    if (ready) {
        recording_fsm_dispatch_event(rec, REC_EVT_EARPHONE_READY, NULL);
    }
}

void recording_fsm_set_case_ready(struct recording_fsm *rec, int ready)
{
    if (!rec) return;
    rec->case_ready = ready;
    _MY_TRACE_STR("recording_fsm_set_case_ready: %d\n", ready);
    if (ready) {
        recording_fsm_dispatch_event(rec, REC_EVT_CASE_READY, NULL);
    }
}

int recording_fsm_both_devices_ready(struct recording_fsm *rec)
{
    return rec ? (rec->earphone_ready && rec->case_ready) : 0;
}

/* State transition helper */
static void transition_to(struct recording_fsm *rec, enum recording_state new_state)
{
    if (!rec || rec->current_state == new_state) return;
    
    _MY_TRACE_STR("transition_to: %s -> %s\n",
                  recording_fsm_get_state_name(rec->current_state),
                  recording_fsm_get_state_name(new_state));
    
    /* Exit current state */
    exit_state(rec);
    
    /* Update state */
    rec->previous_state = rec->current_state;
    rec->current_state = new_state;
    
    /* Enter new state */
    switch (new_state) {
        case REC_IDLE:
            enter_idle(rec);
            break;
        case REC_WAIT_SYNC:
            enter_wait_sync(rec);
            break;
        case REC_RECORDING:
            enter_recording(rec);
            break;
        case REC_PAUSED:
            enter_paused(rec);
            break;
        case REC_STOPPING:
            enter_stopping(rec);
            break;
        case REC_ERROR:
            enter_error(rec);
            break;
        default:
            break;
    }
}

/* State handlers */
static void handle_idle(struct recording_fsm *rec, enum recording_event event, void *data)
{
    (void)data;
    switch (event) {
        case REC_EVT_START:
            _MY_TRACE_STR("IDLE: START -> WAIT_SYNC\n");
            transition_to(rec, REC_WAIT_SYNC);
            break;
        case REC_EVT_ERROR:
            _MY_TRACE_STR("IDLE: ERROR -> ERROR\n");
            transition_to(rec, REC_ERROR);
            break;
        default:
            _MY_TRACE_STR("IDLE: event %s ignored\n", recording_fsm_get_event_name(event));
            break;
    }
}

static void handle_wait_sync(struct recording_fsm *rec, enum recording_event event, void *data)
{
    (void)data;
    switch (event) {
        case REC_EVT_SYNC_COMPLETE:
            _MY_TRACE_STR("WAIT_SYNC: SYNC_COMPLETE -> RECORDING\n");
            transition_to(rec, REC_RECORDING);
            break;
        case REC_EVT_EARPHONE_READY:
            _MY_TRACE_STR("WAIT_SYNC: EARPHONE_READY (checking both devices)\n");
            if (recording_fsm_both_devices_ready(rec)) {
                recording_fsm_dispatch_event(rec, REC_EVT_SYNC_COMPLETE, NULL);
            }
            break;
        case REC_EVT_CASE_READY:
            _MY_TRACE_STR("WAIT_SYNC: CASE_READY (checking both devices)\n");
            if (recording_fsm_both_devices_ready(rec)) {
                recording_fsm_dispatch_event(rec, REC_EVT_SYNC_COMPLETE, NULL);
            }
            break;
        case REC_EVT_STOP:
            _MY_TRACE_STR("WAIT_SYNC: STOP -> IDLE\n");
            transition_to(rec, REC_IDLE);
            break;
        case REC_EVT_ERROR:
            _MY_TRACE_STR("WAIT_SYNC: ERROR -> ERROR\n");
            transition_to(rec, REC_ERROR);
            break;
        default:
            _MY_TRACE_STR("WAIT_SYNC: event %s ignored\n", recording_fsm_get_event_name(event));
            break;
    }
}

static void handle_recording(struct recording_fsm *rec, enum recording_event event, void *data)
{
    (void)data;
    switch (event) {
        case REC_EVT_PAUSE:
            _MY_TRACE_STR("RECORDING: PAUSE -> PAUSED\n");
            transition_to(rec, REC_PAUSED);
            break;
        case REC_EVT_STOP:
            _MY_TRACE_STR("RECORDING: STOP -> STOPPING\n");
            transition_to(rec, REC_STOPPING);
            break;
        case REC_EVT_BUFFER_FULL:
            _MY_TRACE_STR("RECORDING: BUFFER_FULL (pause automatically)\n");
            transition_to(rec, REC_PAUSED);
            break;
        case REC_EVT_STORAGE_FULL:
            _MY_TRACE_STR("RECORDING: STORAGE_FULL -> STOPPING\n");
            transition_to(rec, REC_STOPPING);
            break;
        case REC_EVT_ERROR:
            _MY_TRACE_STR("RECORDING: ERROR -> ERROR\n");
            transition_to(rec, REC_ERROR);
            break;
        default:
            _MY_TRACE_STR("RECORDING: event %s ignored\n", recording_fsm_get_event_name(event));
            break;
    }
}

static void handle_paused(struct recording_fsm *rec, enum recording_event event, void *data)
{
    (void)data;
    switch (event) {
        case REC_EVT_RESUME:
            _MY_TRACE_STR("PAUSED: RESUME -> RECORDING\n");
            transition_to(rec, REC_RECORDING);
            break;
        case REC_EVT_STOP:
            _MY_TRACE_STR("PAUSED: STOP -> STOPPING\n");
            transition_to(rec, REC_STOPPING);
            break;
        case REC_EVT_ERROR:
            _MY_TRACE_STR("PAUSED: ERROR -> ERROR\n");
            transition_to(rec, REC_ERROR);
            break;
        default:
            _MY_TRACE_STR("PAUSED: event %s ignored\n", recording_fsm_get_event_name(event));
            break;
    }
}

static void handle_stopping(struct recording_fsm *rec, enum recording_event event, void *data)
{
    (void)data;
    switch (event) {
        case REC_EVT_STOP:
            _MY_TRACE_STR("STOPPING: STOP -> IDLE\n");
            transition_to(rec, REC_IDLE);
            break;
        case REC_EVT_ERROR:
            _MY_TRACE_STR("STOPPING: ERROR -> ERROR\n");
            transition_to(rec, REC_ERROR);
            break;
        default:
            _MY_TRACE_STR("STOPPING: event %s ignored\n", recording_fsm_get_event_name(event));
            break;
    }
}

static void handle_error(struct recording_fsm *rec, enum recording_event event, void *data)
{
    (void)data;
    switch (event) {
        case REC_EVT_STOP:
            _MY_TRACE_STR("ERROR: STOP -> IDLE\n");
            transition_to(rec, REC_IDLE);
            break;
        case REC_EVT_START:
            _MY_TRACE_STR("ERROR: START -> WAIT_SYNC\n");
            transition_to(rec, REC_WAIT_SYNC);
            break;
        default:
            _MY_TRACE_STR("ERROR: event %s ignored\n", recording_fsm_get_event_name(event));
            break;
    }
}

/* State entry actions */
static void enter_idle(struct recording_fsm *rec)
{
    _MY_TRACE_STR("enter_idle\n");
    rec->duration_ms = 0;
    rec->file_size = 0;
    printf("Recording: Idle - Ready to start recording\n");
}

static void enter_wait_sync(struct recording_fsm *rec)
{
    _MY_TRACE_STR("enter_wait_sync\n");
    printf("Recording: Waiting for device synchronization\n");
    printf("  Source: %s\n", source_names[rec->source]);
    printf("  Sample rate: %d Hz, Bit depth: %d, Channels: %d\n",
           rec->sample_rate, rec->bit_depth, rec->channels);
}

static void enter_recording(struct recording_fsm *rec)
{
    _MY_TRACE_STR("enter_recording\n");
    printf("Recording: Started - Saving to %s\n", rec->filename);
    printf("  Duration: %lu ms, File size: %lu bytes\n", rec->duration_ms, rec->file_size);
}

static void enter_paused(struct recording_fsm *rec)
{
    (void)rec; /* Unused parameter */
    _MY_TRACE_STR("enter_paused\n");
    printf("Recording: Paused - Ready to resume\n");
}

static void enter_stopping(struct recording_fsm *rec)
{
    (void)rec; /* Unused parameter */
    _MY_TRACE_STR("enter_stopping\n");
    printf("Recording: Stopping - Finalizing recording\n");
}

static void enter_error(struct recording_fsm *rec)
{
    _MY_TRACE_STR("enter_error\n");
    printf("Recording: Error - %s (code %d)\n", rec->error_msg, rec->error_code);
}

/* State exit action */
static void exit_state(struct recording_fsm *rec)
{
    _MY_TRACE_STR("exit_state: %s\n", recording_fsm_get_state_name(rec->current_state));
    printf("Recording: Exiting %s state\n", recording_fsm_get_state_name(rec->current_state));
}

/* Do action (executed in each state) */
static void do_action(struct recording_fsm *rec)
{
    switch (rec->current_state) {
        case REC_IDLE:
            printf("Recording: Idle - monitoring for start command\n");
            break;
        case REC_WAIT_SYNC:
            printf("Recording: Waiting sync - checking device status\n");
            break;
        case REC_RECORDING:
            rec->duration_ms += 100; /* Simulate time passing */
            rec->file_size += 1024;  /* Simulate data being written */
            printf("Recording: Recording - duration %lu ms, size %lu bytes\n", 
                   rec->duration_ms, rec->file_size);
            break;
        case REC_PAUSED:
            printf("Recording: Paused - waiting for resume or stop\n");
            break;
        case REC_STOPPING:
            printf("Recording: Stopping - saving final data\n");
            break;
        case REC_ERROR:
            printf("Recording: Error - waiting for recovery\n");
            break;
        default:
            break;
    }
}
