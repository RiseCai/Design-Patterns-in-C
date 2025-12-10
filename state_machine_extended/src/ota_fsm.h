/**
 * ota_fsm.h  2025-12-09
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
 * OTA (Over-The-Air) Upgrade FSM for TWS Earphone System.
 * Handles firmware updates for multiple hardware submodules and large package management.
 */
#ifndef __OTA_FSM_H__
#define __OTA_FSM_H__

#include "mycommon.h"
#include "mytrace.h"
#include <stdint.h>

/* Forward declaration */
struct ota_module;

/** OTA states */
enum ota_state {
    OTA_IDLE,              /* Idle, waiting for upgrade command */
    OTA_CHECKING,          /* Checking for update availability */
    OTA_DOWNLOADING,       /* Downloading firmware package */
    OTA_VALIDATING,        /* Validating package integrity */
    OTA_PREPARING,         /* Preparing installation (unpack, split, etc.) */
    OTA_INSTALLING,        /* Installing to target submodules */
    OTA_REBOOTING,         /* Rebooting device */
    OTA_ROLLBACK,         /* Rolling back to previous version */
    OTA_COMPLETE,          /* Upgrade successfully completed */
    OTA_ERROR              /* Upgrade error */
};

/** OTA events */
enum ota_event {
    OTA_EVT_START,                 /* Start upgrade process */
    OTA_EVT_CHECK_UPDATE,          /* Check for updates */
    OTA_EVT_UPDATE_AVAILABLE,      /* Update available */
    OTA_EVT_NO_UPDATE,             /* No update available */
    OTA_EVT_DOWNLOAD_START,        /* Start download */
    OTA_EVT_DOWNLOAD_PROGRESS,     /* Download progress update */
    OTA_EVT_DOWNLOAD_COMPLETE,     /* Download complete */
    OTA_EVT_VALIDATE_START,        /* Start validation */
    OTA_EVT_VALIDATE_PASS,         /* Validation passed */
    OTA_EVT_VALIDATE_FAIL,         /* Validation failed */
    OTA_EVT_INSTALL_START,         /* Start installation */
    OTA_EVT_INSTALL_PROGRESS,      /* Installation progress */
    OTA_EVT_INSTALL_COMPLETE,      /* Installation complete */
    OTA_EVT_REBOOT,                /* Reboot device */
    OTA_EVT_ROLLBACK,              /* Rollback requested */
    OTA_EVT_ROLLBACK_COMPLETE,     /* Rollback completed */
    OTA_EVT_ERROR,                 /* Error occurred */
    OTA_EVT_CANCEL,                /* Cancel upgrade */
    OTA_EVT_RESET                  /* Reset OTA state machine */
};

/** OTA module types (hardware submodules) */
enum ota_module_type {
    OTA_MODULE_MAIN_CONTROLLER,
    OTA_MODULE_BLUETOOTH,
    OTA_MODULE_AUDIO_DSP,
    OTA_MODULE_WIFI,
    OTA_MODULE_SENSOR,
    OTA_MODULE_DISPLAY,
    OTA_MODULE_CUSTOM
};

/** OTA module state */
enum ota_module_state {
    OTA_MODULE_IDLE,
    OTA_MODULE_DOWNLOADING,
    OTA_MODULE_VALIDATING,
    OTA_MODULE_INSTALLING,
    OTA_MODULE_VERIFYING,
    OTA_MODULE_COMPLETE,
    OTA_MODULE_FAILED
};

/** OTA package format */
enum ota_package_format {
    OTA_PACKAGE_SINGLE,      /* Single firmware image */
    OTA_PACKAGE_MULTIPART,   /* Multipart package (multiple submodules) */
    OTA_PACKAGE_DIFFERENTIAL /* Differential update */
};

/** OTA module descriptor */
struct ota_module {
    enum ota_module_type type;
    char name[32];
    char version[32];
    size_t firmware_size;
    uint8_t checksum[32];
    enum ota_module_state state;
    int progress;           /* 0-100 */
    int error_code;
};

/** OTA context */
struct ota_fsm {
    enum ota_state current_state;
    enum ota_state previous_state;
    
    /* Package information */
    char package_url[256];
    char package_version[32];
    enum ota_package_format package_format;
    size_t package_size;
    size_t downloaded_size;
    uint8_t package_checksum[32];
    
    /* Submodule management */
    struct ota_module *modules;
    int module_count;
    int current_module_index;
    
    /* Large package management */
    int is_multipart;
    int part_current;
    int part_total;
    size_t part_size;
    
    /* Progress tracking */
    int overall_progress;   /* 0-100 */
    
    /* Error handling */
    int error_code;
    char error_msg[128];
    
    /* User data */
    void *user_data;
    
    /* Operations */
    const struct ota_fsm_ops *ops;
};

/** OTA FSM operations */
struct ota_fsm_ops {
    void (*init)(struct ota_fsm *ota);
    void (*destroy)(struct ota_fsm *ota);
    
    /* Event handlers */
    void (*handle_event)(struct ota_fsm *ota, enum ota_event event, void *data);
    
    /* State transition handlers */
    void (*enter_idle)(struct ota_fsm *ota);
    void (*enter_checking)(struct ota_fsm *ota);
    void (*enter_downloading)(struct ota_fsm *ota);
    void (*enter_validating)(struct ota_fsm *ota);
    void (*enter_preparing)(struct ota_fsm *ota);
    void (*enter_installing)(struct ota_fsm *ota);
    void (*enter_rebooting)(struct ota_fsm *ota);
    void (*enter_rollback)(struct ota_fsm *ota);
    void (*enter_complete)(struct ota_fsm *ota);
    void (*enter_error)(struct ota_fsm *ota);
    
    /* Exit handlers */
    void (*exit_state)(struct ota_fsm *ota);
    
    /* Do actions */
    void (*do_action)(struct ota_fsm *ota);
};

/** Main API functions */
void ota_fsm_init(struct ota_fsm *ota);
void ota_fsm_destroy(struct ota_fsm *ota);
void ota_fsm_dispatch_event(struct ota_fsm *ota, enum ota_event event, void *data);
enum ota_state ota_fsm_get_state(struct ota_fsm *ota);
const char *ota_fsm_get_state_name(enum ota_state state);
const char *ota_fsm_get_event_name(enum ota_event event);

/** Configuration functions */
void ota_fsm_set_package_url(struct ota_fsm *ota, const char *url);
void ota_fsm_set_package_version(struct ota_fsm *ota, const char *version);
void ota_fsm_set_package_format(struct ota_fsm *ota, enum ota_package_format format);
void ota_fsm_set_package_size(struct ota_fsm *ota, size_t size);

/** Module management */
void ota_fsm_add_module(struct ota_fsm *ota, enum ota_module_type type, const char *name, const char *version);
void ota_fsm_remove_module(struct ota_fsm *ota, int index);
int ota_fsm_get_module_count(struct ota_fsm *ota);
struct ota_module *ota_fsm_get_module(struct ota_fsm *ota, int index);

/** Progress tracking */
int ota_fsm_get_overall_progress(struct ota_fsm *ota);
int ota_fsm_get_module_progress(struct ota_fsm *ota, int module_index);
void ota_fsm_set_progress(struct ota_fsm *ota, int progress);

/** Error handling */
void ota_fsm_set_error(struct ota_fsm *ota, int code, const char *msg);
int ota_fsm_get_error_code(struct ota_fsm *ota);
const char *ota_fsm_get_error_msg(struct ota_fsm *ota);

/** Status functions */
int ota_fsm_is_idle(struct ota_fsm *ota);
int ota_fsm_is_downloading(struct ota_fsm *ota);
int ota_fsm_is_installing(struct ota_fsm *ota);
int ota_fsm_is_complete(struct ota_fsm *ota);
int ota_fsm_is_error(struct ota_fsm *ota);

/** Utility functions */
const char *ota_fsm_get_module_type_name(enum ota_module_type type);
const char *ota_fsm_get_module_state_name(enum ota_module_state state);
const char *ota_fsm_get_package_format_name(enum ota_package_format format);

#endif /* __OTA_FSM_H__ */
