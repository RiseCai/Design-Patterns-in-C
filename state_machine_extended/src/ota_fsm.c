/**
 * ota_fsm.c  2025-12-09
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
 * OTA (Over-The-Air) Upgrade FSM implementation for TWS Earphone System.
 * Handles firmware updates for multiple hardware submodules and large package management.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "mycommon.h"
#include "mytrace.h"
#include "ota_fsm.h"

/* State names */
static const char *state_names[] = {
    "OTA_IDLE",
    "OTA_CHECKING",
    "OTA_DOWNLOADING",
    "OTA_VALIDATING",
    "OTA_PREPARING",
    "OTA_INSTALLING",
    "OTA_REBOOTING",
    "OTA_ROLLBACK",
    "OTA_COMPLETE",
    "OTA_ERROR"
};

/* Event names */
static const char *event_names[] = {
    "OTA_EVT_START",
    "OTA_EVT_CHECK_UPDATE",
    "OTA_EVT_UPDATE_AVAILABLE",
    "OTA_EVT_NO_UPDATE",
    "OTA_EVT_DOWNLOAD_START",
    "OTA_EVT_DOWNLOAD_PROGRESS",
    "OTA_EVT_DOWNLOAD_COMPLETE",
    "OTA_EVT_VALIDATE_START",
    "OTA_EVT_VALIDATE_PASS",
    "OTA_EVT_VALIDATE_FAIL",
    "OTA_EVT_INSTALL_START",
    "OTA_EVT_INSTALL_PROGRESS",
    "OTA_EVT_INSTALL_COMPLETE",
    "OTA_EVT_REBOOT",
    "OTA_EVT_ROLLBACK",
    "OTA_EVT_ROLLBACK_COMPLETE",
    "OTA_EVT_ERROR",
    "OTA_EVT_CANCEL",
    "OTA_EVT_RESET"
};

/* Module type names */
static const char *module_type_names[] = {
    "OTA_MODULE_MAIN_CONTROLLER",
    "OTA_MODULE_BLUETOOTH",
    "OTA_MODULE_AUDIO_DSP",
    "OTA_MODULE_WIFI",
    "OTA_MODULE_SENSOR",
    "OTA_MODULE_DISPLAY",
    "OTA_MODULE_CUSTOM"
};

/* Module state names */
static const char *module_state_names[] = {
    "OTA_MODULE_IDLE",
    "OTA_MODULE_DOWNLOADING",
    "OTA_MODULE_VALIDATING",
    "OTA_MODULE_INSTALLING",
    "OTA_MODULE_VERIFYING",
    "OTA_MODULE_COMPLETE",
    "OTA_MODULE_FAILED"
};

/* Package format names */
static const char *package_format_names[] = {
    "OTA_PACKAGE_SINGLE",
    "OTA_PACKAGE_MULTIPART",
    "OTA_PACKAGE_DIFFERENTIAL"
};

/* Internal helper functions */
static void handle_idle(struct ota_fsm *ota, enum ota_event event, void *data);
static void handle_checking(struct ota_fsm *ota, enum ota_event event, void *data);
static void handle_downloading(struct ota_fsm *ota, enum ota_event event, void *data);
static void handle_validating(struct ota_fsm *ota, enum ota_event event, void *data);
static void handle_preparing(struct ota_fsm *ota, enum ota_event event, void *data);
static void handle_installing(struct ota_fsm *ota, enum ota_event event, void *data);
static void handle_rebooting(struct ota_fsm *ota, enum ota_event event, void *data);
static void handle_rollback(struct ota_fsm *ota, enum ota_event event, void *data);
static void handle_complete(struct ota_fsm *ota, enum ota_event event, void *data);
static void handle_error(struct ota_fsm *ota, enum ota_event event, void *data);

static void enter_idle(struct ota_fsm *ota);
static void enter_checking(struct ota_fsm *ota);
static void enter_downloading(struct ota_fsm *ota);
static void enter_validating(struct ota_fsm *ota);
static void enter_preparing(struct ota_fsm *ota);
static void enter_installing(struct ota_fsm *ota);
static void enter_rebooting(struct ota_fsm *ota);
static void enter_rollback(struct ota_fsm *ota);
static void enter_complete(struct ota_fsm *ota);
static void enter_error(struct ota_fsm *ota);

static void exit_state(struct ota_fsm *ota);
static void do_action(struct ota_fsm *ota);

static void transition_to(struct ota_fsm *ota, enum ota_state new_state);
static void update_progress(struct ota_fsm *ota, int progress);
static void update_module_progress(struct ota_fsm *ota, int module_index, int progress);
static void set_module_state(struct ota_fsm *ota, int module_index, enum ota_module_state state);
static void start_download(struct ota_fsm *ota);
static void start_validation(struct ota_fsm *ota);
static void start_installation(struct ota_fsm *ota);
static void start_rollback(struct ota_fsm *ota);
static void report_error(struct ota_fsm *ota, int code, const char *msg);

/* Initialize OTA FSM */
void ota_fsm_init(struct ota_fsm *ota)
{
    if (!ota) return;
    
    memset(ota, 0, sizeof(*ota));
    ota->current_state = OTA_IDLE;
    ota->previous_state = OTA_IDLE;
    ota->package_format = OTA_PACKAGE_SINGLE;
    ota->package_size = 0;
    ota->downloaded_size = 0;
    ota->module_count = 0;
    ota->modules = NULL;
    ota->current_module_index = -1;
    ota->is_multipart = 0;
    ota->part_current = 0;
    ota->part_total = 1;
    ota->part_size = 0;
    ota->overall_progress = 0;
    ota->error_code = 0;
    ota->user_data = NULL;
    
    /* Default package URL */
    strcpy(ota->package_url, "https://ota.example.com/firmware.bin");
    strcpy(ota->package_version, "1.0.0");
    
    _MY_TRACE_STR("ota_fsm_init: initialized\n");
}

/* Destroy OTA FSM */
void ota_fsm_destroy(struct ota_fsm *ota)
{
    if (!ota) return;
    
    _MY_TRACE_STR("ota_fsm_destroy: cleaning up\n");
    
    if (ota->modules) {
        free(ota->modules);
        ota->modules = NULL;
        ota->module_count = 0;
    }
}

/* Dispatch event to current state */
void ota_fsm_dispatch_event(struct ota_fsm *ota, enum ota_event event, void *data)
{
    if (!ota) return;
    
    _MY_TRACE_STR("ota_fsm_dispatch_event: state=%s, event=%s\n",
                  ota_fsm_get_state_name(ota->current_state),
                  ota_fsm_get_event_name(event));
    
    switch (ota->current_state) {
        case OTA_IDLE:
            handle_idle(ota, event, data);
            break;
        case OTA_CHECKING:
            handle_checking(ota, event, data);
            break;
        case OTA_DOWNLOADING:
            handle_downloading(ota, event, data);
            break;
        case OTA_VALIDATING:
            handle_validating(ota, event, data);
            break;
        case OTA_PREPARING:
            handle_preparing(ota, event, data);
            break;
        case OTA_INSTALLING:
            handle_installing(ota, event, data);
            break;
        case OTA_REBOOTING:
            handle_rebooting(ota, event, data);
            break;
        case OTA_ROLLBACK:
            handle_rollback(ota, event, data);
            break;
        case OTA_COMPLETE:
            handle_complete(ota, event, data);
            break;
        case OTA_ERROR:
            handle_error(ota, event, data);
            break;
        default:
            _MY_TRACE_STR("Unknown state: %d\n", ota->current_state);
            break;
    }
    
    /* Perform do action after handling event */
    do_action(ota);
}

/* Get current state */
enum ota_state ota_fsm_get_state(struct ota_fsm *ota)
{
    return ota ? ota->current_state : OTA_ERROR;
}

/* Get state name */
const char *ota_fsm_get_state_name(enum ota_state state)
{
    if (state >= 0 && state < (int)(sizeof(state_names)/sizeof(state_names[0]))) {
        return state_names[state];
    }
    return "UNKNOWN_STATE";
}

/* Get event name */
const char *ota_fsm_get_event_name(enum ota_event event)
{
    if (event >= 0 && event < (int)(sizeof(event_names)/sizeof(event_names[0]))) {
        return event_names[event];
    }
    return "UNKNOWN_EVENT";
}

/* Configuration functions */
void ota_fsm_set_package_url(struct ota_fsm *ota, const char *url)
{
    if (!ota || !url) return;
    strncpy(ota->package_url, url, sizeof(ota->package_url)-1);
    ota->package_url[sizeof(ota->package_url)-1] = '\0';
    _MY_TRACE_STR("ota_fsm_set_package_url: %s\n", ota->package_url);
}

void ota_fsm_set_package_version(struct ota_fsm *ota, const char *version)
{
    if (!ota || !version) return;
    strncpy(ota->package_version, version, sizeof(ota->package_version)-1);
    ota->package_version[sizeof(ota->package_version)-1] = '\0';
    _MY_TRACE_STR("ota_fsm_set_package_version: %s\n", ota->package_version);
}

void ota_fsm_set_package_format(struct ota_fsm *ota, enum ota_package_format format)
{
    if (!ota) return;
    ota->package_format = format;
    _MY_TRACE_STR("ota_fsm_set_package_format: %s\n", ota_fsm_get_package_format_name(format));
}

void ota_fsm_set_package_size(struct ota_fsm *ota, size_t size)
{
    if (!ota) return;
    ota->package_size = size;
    _MY_TRACE_STR("ota_fsm_set_package_size: %zu bytes\n", size);
}

/* Module management */
void ota_fsm_add_module(struct ota_fsm *ota, enum ota_module_type type, const char *name, const char *version)
{
    if (!ota || !name || !version) return;
    
    struct ota_module *new_modules = realloc(ota->modules, (ota->module_count + 1) * sizeof(struct ota_module));
    if (!new_modules) {
        _MY_TRACE_STR("ota_fsm_add_module: memory allocation failed\n");
        return;
    }
    
    ota->modules = new_modules;
    struct ota_module *module = &ota->modules[ota->module_count];
    
    module->type = type;
    strncpy(module->name, name, sizeof(module->name)-1);
    module->name[sizeof(module->name)-1] = '\0';
    strncpy(module->version, version, sizeof(module->version)-1);
    module->version[sizeof(module->version)-1] = '\0';
    module->firmware_size = 0;
    memset(module->checksum, 0, sizeof(module->checksum));
    module->state = OTA_MODULE_IDLE;
    module->progress = 0;
    module->error_code = 0;
    
    ota->module_count++;
    
    _MY_TRACE_STR("ota_fsm_add_module: %s (type=%s, version=%s)\n", 
                  name, ota_fsm_get_module_type_name(type), version);
}

void ota_fsm_remove_module(struct ota_fsm *ota, int index)
{
    if (!ota || index < 0 || index >= ota->module_count) return;
    
    _MY_TRACE_STR("ota_fsm_remove_module: removing %s\n", ota->modules[index].name);
    
    for (int i = index; i < ota->module_count - 1; i++) {
        ota->modules[i] = ota->modules[i + 1];
    }
    
    ota->module_count--;
    
    if (ota->module_count == 0) {
        free(ota->modules);
        ota->modules = NULL;
    } else {
        struct ota_module *new_modules = realloc(ota->modules, ota->module_count * sizeof(struct ota_module));
        if (new_modules) {
            ota->modules = new_modules;
        }
    }
}

int ota_fsm_get_module_count(struct ota_fsm *ota)
{
    return ota ? ota->module_count : 0;
}

struct ota_module *ota_fsm_get_module(struct ota_fsm *ota, int index)
{
    if (!ota || index < 0 || index >= ota->module_count) return NULL;
    return &ota->modules[index];
}

/* Progress tracking */
int ota_fsm_get_overall_progress(struct ota_fsm *ota)
{
    return ota ? ota->overall_progress : 0;
}

int ota_fsm_get_module_progress(struct ota_fsm *ota, int module_index)
{
    if (!ota || module_index < 0 || module_index >= ota->module_count) return 0;
    return ota->modules[module_index].progress;
}

void ota_fsm_set_progress(struct ota_fsm *ota, int progress)
{
    if (!ota) return;
    if (progress < 0) progress = 0;
    if (progress > 100) progress = 100;
    ota->overall_progress = progress;
    _MY_TRACE_STR("ota_fsm_set_progress: %d%%\n", progress);
}

/* Error handling */
void ota_fsm_set_error(struct ota_fsm *ota, int code, const char *msg)
{
    if (!ota) return;
    ota->error_code = code;
    strncpy(ota->error_msg, msg, sizeof(ota->error_msg)-1);
    ota->error_msg[sizeof(ota->error_msg)-1] = '\0';
    _MY_TRACE_STR("ota_fsm_set_error: code=%d, msg=%s\n", code, msg);
    ota_fsm_dispatch_event(ota, OTA_EVT_ERROR, NULL);
}

int ota_fsm_get_error_code(struct ota_fsm *ota)
{
    return ota ? ota->error_code : 0;
}

const char *ota_fsm_get_error_msg(struct ota_fsm *ota)
{
    return ota ? ota->error_msg : "";
}

/* Status functions */
int ota_fsm_is_idle(struct ota_fsm *ota)
{
    return ota ? (ota->current_state == OTA_IDLE) : 0;
}

int ota_fsm_is_downloading(struct ota_fsm *ota)
{
    return ota ? (ota->current_state == OTA_DOWNLOADING) : 0;
}

int ota_fsm_is_installing(struct ota_fsm *ota)
{
    return ota ? (ota->current_state == OTA_INSTALLING) : 0;
}

int ota_fsm_is_complete(struct ota_fsm *ota)
{
    return ota ? (ota->current_state == OTA_COMPLETE) : 0;
}

int ota_fsm_is_error(struct ota_fsm *ota)
{
    return ota ? (ota->current_state == OTA_ERROR) : 0;
}

/* Utility functions */
const char *ota_fsm_get_module_type_name(enum ota_module_type type)
{
    if (type >= 0 && type < (int)(sizeof(module_type_names)/sizeof(module_type_names[0]))) {
        return module_type_names[type];
    }
    return "UNKNOWN_MODULE_TYPE";
}

const char *ota_fsm_get_module_state_name(enum ota_module_state state)
{
    if (state >= 0 && state < (int)(sizeof(module_state_names)/sizeof(module_state_names[0]))) {
        return module_state_names[state];
    }
    return "UNKNOWN_MODULE_STATE";
}

const char *ota_fsm_get_package_format_name(enum ota_package_format format)
{
    if (format >= 0 && format < (int)(sizeof(package_format_names)/sizeof(package_format_names[0]))) {
        return package_format_names[format];
    }
    return "UNKNOWN_PACKAGE_FORMAT";
}

/* Internal helper implementations */

static void transition_to(struct ota_fsm *ota, enum ota_state new_state)
{
    if (!ota || ota->current_state == new_state) return;
    
    _MY_TRACE_STR("transition_to: %s -> %s\n",
                  ota_fsm_get_state_name(ota->current_state),
                  ota_fsm_get_state_name(new_state));
    
    /* Exit current state */
    exit_state(ota);
    
    /* Update state */
    ota->previous_state = ota->current_state;
    ota->current_state = new_state;
    
    /* Enter new state */
    switch (new_state) {
        case OTA_IDLE:
            enter_idle(ota);
            break;
        case OTA_CHECKING:
            enter_checking(ota);
            break;
        case OTA_DOWNLOADING:
            enter_downloading(ota);
            break;
        case OTA_VALIDATING:
            enter_validating(ota);
            break;
        case OTA_PREPARING:
            enter_preparing(ota);
            break;
        case OTA_INSTALLING:
            enter_installing(ota);
            break;
        case OTA_REBOOTING:
            enter_rebooting(ota);
            break;
        case OTA_ROLLBACK:
            enter_rollback(ota);
            break;
        case OTA_COMPLETE:
            enter_complete(ota);
            break;
        case OTA_ERROR:
            enter_error(ota);
            break;
        default:
            _MY_TRACE_STR("Unknown state in transition_to: %d\n", new_state);
            break;
    }
}

static void exit_state(struct ota_fsm *ota)
{
    if (!ota) return;
    
    switch (ota->current_state) {
        case OTA_IDLE:
            /* Nothing to exit */
            break;
        case OTA_CHECKING:
            _MY_TRACE_STR("Exiting CHECKING state\n");
            break;
        case OTA_DOWNLOADING:
            _MY_TRACE_STR("Exiting DOWNLOADING state\n");
            break;
        case OTA_VALIDATING:
            _MY_TRACE_STR("Exiting VALIDATING state\n");
            break;
        case OTA_PREPARING:
            _MY_TRACE_STR("Exiting PREPARING state\n");
            break;
        case OTA_INSTALLING:
            _MY_TRACE_STR("Exiting INSTALLING state\n");
            break;
        case OTA_REBOOTING:
            _MY_TRACE_STR("Exiting REBOOTING state\n");
            break;
        case OTA_ROLLBACK:
            _MY_TRACE_STR("Exiting ROLLBACK state\n");
            break;
        case OTA_COMPLETE:
            _MY_TRACE_STR("Exiting COMPLETE state\n");
            break;
        case OTA_ERROR:
            _MY_TRACE_STR("Exiting ERROR state\n");
            break;
        default:
            break;
    }
}

static void do_action(struct ota_fsm *ota)
{
    if (!ota) return;
    
    switch (ota->current_state) {
        case OTA_IDLE:
            /* Idle state does nothing */
            break;
        case OTA_CHECKING:
            /* Check for updates periodically */
            break;
        case OTA_DOWNLOADING:
            /* Continue downloading */
            break;
        case OTA_VALIDATING:
            /* Continue validation */
            break;
        case OTA_PREPARING:
            /* Continue preparation */
            break;
        case OTA_INSTALLING:
            /* Continue installation */
            break;
        case OTA_REBOOTING:
            /* Wait for reboot */
            break;
        case OTA_ROLLBACK:
            /* Continue rollback */
            break;
        case OTA_COMPLETE:
            /* Nothing to do */
            break;
        case OTA_ERROR:
            /* Error recovery actions */
            break;
        default:
            break;
    }
}

/* Enter state functions */
static void enter_idle(struct ota_fsm *ota)
{
    if (!ota) return;
    _MY_TRACE_STR("Entering IDLE state\n");
    ota->overall_progress = 0;
    ota->current_module_index = -1;
    ota->part_current = 0;
    ota->downloaded_size = 0;
}

static void enter_checking(struct ota_fsm *ota)
{
    if (!ota) return;
    _MY_TRACE_STR("Entering CHECKING state\n");
    ota->overall_progress = 5;
    /* Simulate checking for updates */
}

static void enter_downloading(struct ota_fsm *ota)
{
    if (!ota) return;
    _MY_TRACE_STR("Entering DOWNLOADING state\n");
    ota->overall_progress = 10;
    start_download(ota);
}

static void enter_validating(struct ota_fsm *ota)
{
    if (!ota) return;
    _MY_TRACE_STR("Entering VALIDATING state\n");
    ota->overall_progress = 60;
    start_validation(ota);
}

static void enter_preparing(struct ota_fsm *ota)
{
    if (!ota) return;
    _MY_TRACE_STR("Entering PREPARING state\n");
    ota->overall_progress = 70;
    /* Prepare installation */
}

static void enter_installing(struct ota_fsm *ota)
{
    if (!ota) return;
    _MY_TRACE_STR("Entering INSTALLING state\n");
    ota->overall_progress = 80;
    start_installation(ota);
}

static void enter_rebooting(struct ota_fsm *ota)
{
    if (!ota) return;
    _MY_TRACE_STR("Entering REBOOTING state\n");
    ota->overall_progress = 95;
    /* Schedule reboot */
}

static void enter_rollback(struct ota_fsm *ota)
{
    if (!ota) return;
    _MY_TRACE_STR("Entering ROLLBACK state\n");
    ota->overall_progress = 50;
    start_rollback(ota);
}

static void enter_complete(struct ota_fsm *ota)
{
    if (!ota) return;
    _MY_TRACE_STR("Entering COMPLETE state\n");
    ota->overall_progress = 100;
    /* Notify completion */
}

static void enter_error(struct ota_fsm *ota)
{
    if (!ota) return;
    _MY_TRACE_STR("Entering ERROR state\n");
    /* Error handling */
}

/* Handle event functions */
static void handle_idle(struct ota_fsm *ota, enum ota_event event, void *data)
{
    if (!ota) return;
    
    switch (event) {
        case OTA_EVT_START:
            _MY_TRACE_STR("IDLE: Starting OTA process\n");
            transition_to(ota, OTA_CHECKING);
            break;
        case OTA_EVT_CHECK_UPDATE:
            _MY_TRACE_STR("IDLE: Manual check for updates\n");
            transition_to(ota, OTA_CHECKING);
            break;
        case OTA_EVT_ERROR:
            _MY_TRACE_STR("IDLE: Error event\n");
            transition_to(ota, OTA_ERROR);
            break;
        default:
            _MY_TRACE_STR("IDLE: Unhandled event %s\n", ota_fsm_get_event_name(event));
            break;
    }
}

static void handle_checking(struct ota_fsm *ota, enum ota_event event, void *data)
{
    if (!ota) return;
    
    switch (event) {
        case OTA_EVT_UPDATE_AVAILABLE:
            _MY_TRACE_STR("CHECKING: Update available\n");
            transition_to(ota, OTA_DOWNLOADING);
            break;
        case OTA_EVT_NO_UPDATE:
            _MY_TRACE_STR("CHECKING: No update available\n");
            transition_to(ota, OTA_IDLE);
            break;
        case OTA_EVT_ERROR:
            _MY_TRACE_STR("CHECKING: Error event\n");
            transition_to(ota, OTA_ERROR);
            break;
        case OTA_EVT_CANCEL:
            _MY_TRACE_STR("CHECKING: Cancel OTA\n");
            transition_to(ota, OTA_IDLE);
            break;
        default:
            _MY_TRACE_STR("CHECKING: Unhandled event %s\n", ota_fsm_get_event_name(event));
            break;
    }
}

static void handle_downloading(struct ota_fsm *ota, enum ota_event event, void *data)
{
    if (!ota) return;
    
    switch (event) {
        case OTA_EVT_DOWNLOAD_COMPLETE:
            _MY_TRACE_STR("DOWNLOADING: Download complete\n");
            transition_to(ota, OTA_VALIDATING);
            break;
        case OTA_EVT_DOWNLOAD_PROGRESS:
            if (data) {
                int progress = *(int *)data;
                update_progress(ota, progress);
            }
            break;
        case OTA_EVT_ERROR:
            _MY_TRACE_STR("DOWNLOADING: Error event\n");
            transition_to(ota, OTA_ERROR);
            break;
        case OTA_EVT_CANCEL:
            _MY_TRACE_STR("DOWNLOADING: Cancel OTA\n");
            transition_to(ota, OTA_IDLE);
            break;
        default:
            _MY_TRACE_STR("DOWNLOADING: Unhandled event %s\n", ota_fsm_get_event_name(event));
            break;
    }
}

static void handle_validating(struct ota_fsm *ota, enum ota_event event, void *data)
{
    if (!ota) return;
    
    switch (event) {
        case OTA_EVT_VALIDATE_PASS:
            _MY_TRACE_STR("VALIDATING: Validation passed\n");
            transition_to(ota, OTA_PREPARING);
            break;
        case OTA_EVT_VALIDATE_FAIL:
            _MY_TRACE_STR("VALIDATING: Validation failed\n");
            transition_to(ota, OTA_ERROR);
            break;
        case OTA_EVT_ERROR:
            _MY_TRACE_STR("VALIDATING: Error event\n");
            transition_to(ota, OTA_ERROR);
            break;
        case OTA_EVT_CANCEL:
            _MY_TRACE_STR("VALIDATING: Cancel OTA\n");
            transition_to(ota, OTA_IDLE);
            break;
        default:
            _MY_TRACE_STR("VALIDATING: Unhandled event %s\n", ota_fsm_get_event_name(event));
            break;
    }
}

static void handle_preparing(struct ota_fsm *ota, enum ota_event event, void *data)
{
    if (!ota) return;
    
    switch (event) {
        case OTA_EVT_INSTALL_START:
            _MY_TRACE_STR("PREPARING: Start installation\n");
            transition_to(ota, OTA_INSTALLING);
            break;
        case OTA_EVT_ERROR:
            _MY_TRACE_STR("PREPARING: Error event\n");
            transition_to(ota, OTA_ERROR);
            break;
        case OTA_EVT_CANCEL:
            _MY_TRACE_STR("PREPARING: Cancel OTA\n");
            transition_to(ota, OTA_IDLE);
            break;
        default:
            _MY_TRACE_STR("PREPARING: Unhandled event %s\n", ota_fsm_get_event_name(event));
            break;
    }
}

static void handle_installing(struct ota_fsm *ota, enum ota_event event, void *data)
{
    if (!ota) return;
    
    switch (event) {
        case OTA_EVT_INSTALL_COMPLETE:
            _MY_TRACE_STR("INSTALLING: Installation complete\n");
            transition_to(ota, OTA_REBOOTING);
            break;
        case OTA_EVT_INSTALL_PROGRESS:
            if (data) {
                int progress = *(int *)data;
                update_progress(ota, progress);
            }
            break;
        case OTA_EVT_ERROR:
            _MY_TRACE_STR("INSTALLING: Error event\n");
            transition_to(ota, OTA_ERROR);
            break;
        case OTA_EVT_CANCEL:
            _MY_TRACE_STR("INSTALLING: Cancel OTA\n");
            transition_to(ota, OTA_IDLE);
            break;
        default:
            _MY_TRACE_STR("INSTALLING: Unhandled event %s\n", ota_fsm_get_event_name(event));
            break;
    }
}

static void handle_rebooting(struct ota_fsm *ota, enum ota_event event, void *data)
{
    if (!ota) return;
    
    switch (event) {
        case OTA_EVT_REBOOT:
            _MY_TRACE_STR("REBOOTING: Reboot completed\n");
            transition_to(ota, OTA_COMPLETE);
            break;
        case OTA_EVT_ERROR:
            _MY_TRACE_STR("REBOOTING: Error event\n");
            transition_to(ota, OTA_ERROR);
            break;
        default:
            _MY_TRACE_STR("REBOOTING: Unhandled event %s\n", ota_fsm_get_event_name(event));
            break;
    }
}

static void handle_rollback(struct ota_fsm *ota, enum ota_event event, void *data)
{
    if (!ota) return;
    
    switch (event) {
        case OTA_EVT_ROLLBACK_COMPLETE:
            _MY_TRACE_STR("ROLLBACK: Rollback completed\n");
            transition_to(ota, OTA_IDLE);
            break;
        case OTA_EVT_ERROR:
            _MY_TRACE_STR("ROLLBACK: Error event\n");
            transition_to(ota, OTA_ERROR);
            break;
        case OTA_EVT_CANCEL:
            _MY_TRACE_STR("ROLLBACK: Cancel rollback\n");
            transition_to(ota, OTA_IDLE);
            break;
        default:
            _MY_TRACE_STR("ROLLBACK: Unhandled event %s\n", ota_fsm_get_event_name(event));
            break;
    }
}

static void handle_complete(struct ota_fsm *ota, enum ota_event event, void *data)
{
    if (!ota) return;
    
    switch (event) {
        case OTA_EVT_START:
            _MY_TRACE_STR("COMPLETE: Starting new OTA process\n");
            transition_to(ota, OTA_CHECKING);
            break;
        case OTA_EVT_RESET:
            _MY_TRACE_STR("COMPLETE: Resetting OTA\n");
            transition_to(ota, OTA_IDLE);
            break;
        default:
            _MY_TRACE_STR("COMPLETE: Unhandled event %s\n", ota_fsm_get_event_name(event));
            break;
    }
}

static void handle_error(struct ota_fsm *ota, enum ota_event event, void *data)
{
    if (!ota) return;

    switch (event) {
        case OTA_EVT_RESET:
            _MY_TRACE_STR("ERROR: Resetting OTA\n");
            transition_to(ota, OTA_IDLE);
            break;
        case OTA_EVT_CANCEL:
            _MY_TRACE_STR("ERROR: Cancelling OTA\n");
            transition_to(ota, OTA_IDLE);
            break;
        case OTA_EVT_ROLLBACK:
            _MY_TRACE_STR("ERROR: Starting rollback\n");
            transition_to(ota, OTA_ROLLBACK);
            break;
        default:
            _MY_TRACE_STR("ERROR: Unhandled event %s\n", ota_fsm_get_event_name(event));
            break;
    }
}

/* Helper functions */

static void update_progress(struct ota_fsm *ota, int progress)
{
    if (!ota) return;
    if (progress < 0) progress = 0;
    if (progress > 100) progress = 100;
    ota->overall_progress = progress;
    _MY_TRACE_STR("update_progress: %d%%\n", progress);
}

static void update_module_progress(struct ota_fsm *ota, int module_index, int progress)
{
    if (!ota || module_index < 0 || module_index >= ota->module_count) return;
    if (progress < 0) progress = 0;
    if (progress > 100) progress = 100;
    ota->modules[module_index].progress = progress;
    _MY_TRACE_STR("update_module_progress: module %d -> %d%%\n", module_index, progress);
}

static void set_module_state(struct ota_fsm *ota, int module_index, enum ota_module_state state)
{
    if (!ota || module_index < 0 || module_index >= ota->module_count) return;
    ota->modules[module_index].state = state;
    _MY_TRACE_STR("set_module_state: module %d -> %s\n", module_index, ota_fsm_get_module_state_name(state));
}

static void start_download(struct ota_fsm *ota)
{
    if (!ota) return;
    _MY_TRACE_STR("start_download: starting download from %s\n", ota->package_url);
    /* Simulate download start */
    ota->downloaded_size = 0;
    ota->part_current = 0;
    ota->overall_progress = 10;
}

static void start_validation(struct ota_fsm *ota)
{
    if (!ota) return;
    _MY_TRACE_STR("start_validation: validating package\n");
    /* Simulate validation start */
    ota->overall_progress = 60;
}

static void start_installation(struct ota_fsm *ota)
{
    if (!ota) return;
    _MY_TRACE_STR("start_installation: installing firmware\n");
    /* Simulate installation start */
    ota->overall_progress = 80;
    ota->current_module_index = 0;
    if (ota->module_count > 0) {
        set_module_state(ota, 0, OTA_MODULE_INSTALLING);
    }
}

static void start_rollback(struct ota_fsm *ota)
{
    if (!ota) return;
    _MY_TRACE_STR("start_rollback: rolling back firmware\n");
    /* Simulate rollback start */
    ota->overall_progress = 50;
    ota->current_module_index = 0;
    if (ota->module_count > 0) {
        set_module_state(ota, 0, OTA_MODULE_INSTALLING);
    }
}

static void report_error(struct ota_fsm *ota, int code, const char *msg)
{
    if (!ota) return;
    ota->error_code = code;
    strncpy(ota->error_msg, msg, sizeof(ota->error_msg)-1);
    ota->error_msg[sizeof(ota->error_msg)-1] = '\0';
    _MY_TRACE_STR("report_error: code=%d, msg=%s\n", code, msg);
}
