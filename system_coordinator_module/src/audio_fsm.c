/**
 * audio_fsm.c  2025-12-30
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
 * Audio Control FSM implementation for TWS Earphone System.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mycommon.h"
#include "mytrace.h"
#include "audio_fsm.h"

/* State names */
static const char *state_names[] = {
    "AUDIO_OFF",
    "AUDIO_IDLE",
    "AUDIO_PLAYING",
    "AUDIO_PAUSED",
    "AUDIO_RECORDING",
    "AUDIO_ERROR"
};

/* Event names */
static const char *event_names[] = {
    "AUDIO_EVT_PLAY",
    "AUDIO_EVT_PAUSE",
    "AUDIO_EVT_STOP",
    "AUDIO_EVT_RECORD_START",
    "AUDIO_EVT_RECORD_STOP",
    "AUDIO_EVT_VOLUME_UP",
    "AUDIO_EVT_VOLUME_DOWN",
    "AUDIO_EVT_MUTE",
    "AUDIO_EVT_UNMUTE",
    "AUDIO_EVT_ERROR"
};

/* Internal helper functions */
static void handle_off(struct audio_fsm *audio, enum audio_event event, void *data);
static void handle_idle(struct audio_fsm *audio, enum audio_event event, void *data);
static void handle_playing(struct audio_fsm *audio, enum audio_event event, void *data);
static void handle_paused(struct audio_fsm *audio, enum audio_event event, void *data);
static void handle_recording(struct audio_fsm *audio, enum audio_event event, void *data);
static void handle_error(struct audio_fsm *audio, enum audio_event event, void *data);

static void enter_off(struct audio_fsm *audio);
static void enter_idle(struct audio_fsm *audio);
static void enter_playing(struct audio_fsm *audio);
static void enter_paused(struct audio_fsm *audio);
static void enter_recording(struct audio_fsm *audio);
static void enter_error(struct audio_fsm *audio);

static void exit_state(struct audio_fsm *audio);
static void do_action(struct audio_fsm *audio);

/* State transition helper */
static void transition_to(struct audio_fsm *audio, enum audio_state new_state);

/* Initialize audio FSM */
void audio_fsm_init(struct audio_fsm *audio)
{
    if (!audio) return;
    
    memset(audio, 0, sizeof(*audio));
    audio->current_state = AUDIO_OFF;
    audio->previous_state = AUDIO_OFF;
    audio->user_data = NULL;
    
    _MY_TRACE_STR("audio_fsm_init: initialized\n");
}

/* Destroy audio FSM */
void audio_fsm_destroy(struct audio_fsm *audio)
{
    if (!audio) return;
    _MY_TRACE_STR("audio_fsm_destroy: cleaning up\n");
    /* Nothing to free for now */
}

/* Dispatch event to current state */
void audio_fsm_dispatch_event(struct audio_fsm *audio, enum audio_event event, void *data)
{
    if (!audio) return;
    
    _MY_TRACE_STR("audio_fsm_dispatch_event: state=%s, event=%s\n",
                  audio_fsm_get_state_name(audio->current_state),
                  audio_fsm_get_event_name(event));
    
    switch (audio->current_state) {
        case AUDIO_OFF:
            handle_off(audio, event, data);
            break;
        case AUDIO_IDLE:
            handle_idle(audio, event, data);
            break;
        case AUDIO_PLAYING:
            handle_playing(audio, event, data);
            break;
        case AUDIO_PAUSED:
            handle_paused(audio, event, data);
            break;
        case AUDIO_RECORDING:
            handle_recording(audio, event, data);
            break;
        case AUDIO_ERROR:
            handle_error(audio, event, data);
            break;
        default:
            _MY_TRACE_STR("Unknown state: %d\n", audio->current_state);
            break;
    }
    
    /* Perform do action after handling event */
    do_action(audio);
}

/* Get current state */
enum audio_state audio_fsm_get_state(struct audio_fsm *audio)
{
    return audio ? audio->current_state : AUDIO_OFF;
}

/* Get state name */
const char *audio_fsm_get_state_name(enum audio_state state)
{
    if (state >= 0 && state < (int)(sizeof(state_names)/sizeof(state_names[0]))) {
        return state_names[state];
    }
    return "UNKNOWN_STATE";
}

/* Get event name */
const char *audio_fsm_get_event_name(enum audio_event event)
{
    if (event >= 0 && event < (int)(sizeof(event_names)/sizeof(event_names[0]))) {
        return event_names[event];
    }
    return "UNKNOWN_EVENT";
}

/* State transition helper */
static void transition_to(struct audio_fsm *audio, enum audio_state new_state)
{
    if (!audio || audio->current_state == new_state) return;
    
    _MY_TRACE_STR("transition_to: %s -> %s\n",
                  audio_fsm_get_state_name(audio->current_state),
                  audio_fsm_get_state_name(new_state));
    
    /* Exit current state */
    exit_state(audio);
    
    /* Update state */
    audio->previous_state = audio->current_state;
    audio->current_state = new_state;
    
    /* Enter new state */
    switch (new_state) {
        case AUDIO_OFF:
            enter_off(audio);
            break;
        case AUDIO_IDLE:
            enter_idle(audio);
            break;
        case AUDIO_PLAYING:
            enter_playing(audio);
            break;
        case AUDIO_PAUSED:
            enter_paused(audio);
            break;
        case AUDIO_RECORDING:
            enter_recording(audio);
            break;
        case AUDIO_ERROR:
            enter_error(audio);
            break;
        default:
            break;
    }
}

/* State handlers */
static void handle_off(struct audio_fsm *audio, enum audio_event event, void *data)
{
    (void)data;
    switch (event) {
        case AUDIO_EVT_PLAY:
            _MY_TRACE_STR("OFF: PLAY -> IDLE\n");
            transition_to(audio, AUDIO_IDLE);
            break;
        case AUDIO_EVT_RECORD_START:
            _MY_TRACE_STR("OFF: RECORD_START -> RECORDING\n");
            transition_to(audio, AUDIO_RECORDING);
            break;
        default:
            _MY_TRACE_STR("OFF: event %s ignored\n", audio_fsm_get_event_name(event));
            break;
    }
}

static void handle_idle(struct audio_fsm *audio, enum audio_event event, void *data)
{
    (void)data;
    switch (event) {
        case AUDIO_EVT_PLAY:
            _MY_TRACE_STR("IDLE: PLAY -> PLAYING\n");
            transition_to(audio, AUDIO_PLAYING);
            break;
        case AUDIO_EVT_RECORD_START:
            _MY_TRACE_STR("IDLE: RECORD_START -> RECORDING\n");
            transition_to(audio, AUDIO_RECORDING);
            break;
        case AUDIO_EVT_STOP:
            _MY_TRACE_STR("IDLE: STOP -> OFF\n");
            transition_to(audio, AUDIO_OFF);
            break;
        case AUDIO_EVT_ERROR:
            _MY_TRACE_STR("IDLE: ERROR -> ERROR\n");
            transition_to(audio, AUDIO_ERROR);
            break;
        default:
            _MY_TRACE_STR("IDLE: event %s ignored\n", audio_fsm_get_event_name(event));
            break;
    }
}

static void handle_playing(struct audio_fsm *audio, enum audio_event event, void *data)
{
    (void)data;
    switch (event) {
        case AUDIO_EVT_PAUSE:
            _MY_TRACE_STR("PLAYING: PAUSE -> PAUSED\n");
            transition_to(audio, AUDIO_PAUSED);
            break;
        case AUDIO_EVT_STOP:
            _MY_TRACE_STR("PLAYING: STOP -> IDLE\n");
            transition_to(audio, AUDIO_IDLE);
            break;
        case AUDIO_EVT_ERROR:
            _MY_TRACE_STR("PLAYING: ERROR -> ERROR\n");
            transition_to(audio, AUDIO_ERROR);
            break;
        default:
            _MY_TRACE_STR("PLAYING: event %s ignored\n", audio_fsm_get_event_name(event));
            break;
    }
}

static void handle_paused(struct audio_fsm *audio, enum audio_event event, void *data)
{
    (void)data;
    switch (event) {
        case AUDIO_EVT_PLAY:
            _MY_TRACE_STR("PAUSED: PLAY -> PLAYING\n");
            transition_to(audio, AUDIO_PLAYING);
            break;
        case AUDIO_EVT_STOP:
            _MY_TRACE_STR("PAUSED: STOP -> IDLE\n");
            transition_to(audio, AUDIO_IDLE);
            break;
        case AUDIO_EVT_ERROR:
            _MY_TRACE_STR("PAUSED: ERROR -> ERROR\n");
            transition_to(audio, AUDIO_ERROR);
            break;
        default:
            _MY_TRACE_STR("PAUSED: event %s ignored\n", audio_fsm_get_event_name(event));
            break;
    }
}

static void handle_recording(struct audio_fsm *audio, enum audio_event event, void *data)
{
    (void)data;
    switch (event) {
        case AUDIO_EVT_RECORD_STOP:
            _MY_TRACE_STR("RECORDING: RECORD_STOP -> IDLE\n");
            transition_to(audio, AUDIO_IDLE);
            break;
        case AUDIO_EVT_ERROR:
            _MY_TRACE_STR("RECORDING: ERROR -> ERROR\n");
            transition_to(audio, AUDIO_ERROR);
            break;
        default:
            _MY_TRACE_STR("RECORDING: event %s ignored\n", audio_fsm_get_event_name(event));
            break;
    }
}

static void handle_error(struct audio_fsm *audio, enum audio_event event, void *data)
{
    (void)data;
    switch (event) {
        case AUDIO_EVT_STOP:
            _MY_TRACE_STR("ERROR: STOP -> OFF\n");
            transition_to(audio, AUDIO_OFF);
            break;
        case AUDIO_EVT_PLAY:
            _MY_TRACE_STR("ERROR: PLAY -> IDLE\n");
            transition_to(audio, AUDIO_IDLE);
            break;
        default:
            _MY_TRACE_STR("ERROR: event %s ignored\n", audio_fsm_get_event_name(event));
            break;
    }
}

/* State entry actions */
static void enter_off(struct audio_fsm *audio)
{
    _MY_TRACE_STR("enter_off\n");
    printf("Audio: OFF - Audio system powered down\n");
}

static void enter_idle(struct audio_fsm *audio)
{
    _MY_TRACE_STR("enter_idle\n");
    printf("Audio: IDLE - Ready to play or record\n");
}

static void enter_playing(struct audio_fsm *audio)
{
    _MY_TRACE_STR("enter_playing\n");
    printf("Audio: PLAYING - Playing audio\n");
}

static void enter_paused(struct audio_fsm *audio)
{
    _MY_TRACE_STR("enter_paused\n");
    printf("Audio: PAUSED - Audio paused\n");
}

static void enter_recording(struct audio_fsm *audio)
{
    _MY_TRACE_STR("enter_recording\n");
    printf("Audio: RECORDING - Recording audio\n");
}

static void enter_error(struct audio_fsm *audio)
{
    _MY_TRACE_STR("enter_error\n");
    printf("Audio: ERROR - Audio error occurred\n");
}

/* State exit action */
static void exit_state(struct audio_fsm *audio)
{
    _MY_TRACE_STR("exit_state: %s\n", audio_fsm_get_state_name(audio->current_state));
    printf("Audio: Exiting %s state\n", audio_fsm_get_state_name(audio->current_state));
}

/* Do action (executed in each state) */
static void do_action(struct audio_fsm *audio)
{
    switch (audio->current_state) {
        case AUDIO_OFF:
            printf("Audio: OFF - no action\n");
            break;
        case AUDIO_IDLE:
            printf("Audio: IDLE - waiting for command\n");
            break;
        case AUDIO_PLAYING:
            printf("Audio: PLAYING - streaming audio\n");
            break;
        case AUDIO_PAUSED:
            printf("Audio: PAUSED - holding position\n");
            break;
        case AUDIO_RECORDING:
            printf("Audio: RECORDING - capturing audio\n");
            break;
        case AUDIO_ERROR:
            printf("Audio: ERROR - waiting for recovery\n");
            break;
        default:
            break;
    }
}

/* Additional API functions for system coordinator */

void audio_fsm_start_recording(struct audio_fsm *fsm)
{
    if (!fsm) return;
    _MY_TRACE_STR("audio_fsm_start_recording\n");
    /* Transition to recording state if not already recording */
    if (fsm->current_state != AUDIO_RECORDING) {
        transition_to(fsm, AUDIO_RECORDING);
    }
}

void audio_fsm_sleep(struct audio_fsm *fsm)
{
    if (!fsm) return;
    _MY_TRACE_STR("audio_fsm_sleep\n");
    /* Transition to OFF state (low power) */
    if (fsm->current_state != AUDIO_OFF) {
        transition_to(fsm, AUDIO_OFF);
    }
}

void audio_fsm_reset(struct audio_fsm *fsm)
{
    if (!fsm) return;
    _MY_TRACE_STR("audio_fsm_reset\n");
    /* Reset to OFF state */
    fsm->current_state = AUDIO_OFF;
    fsm->previous_state = AUDIO_OFF;
    printf("Audio FSM reset to OFF\n");
}
