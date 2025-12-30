/**
 * system_coordinator.c  2025-12-05
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
 * System Coordinator FSM implementation for TWS Earphone System.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "system_coordinator.h"
#include "recording_fsm.h"
#include "ota_fsm.h"
#include "power_fsm.h"
#include "comm_fsm.h"
#include "audio_fsm.h"
#include "../../state_machine_extended/src/efsm_protocol.h"

/* Static operations structure */
static struct system_coordinator_ops sys_ops;

/* State names for debugging */
static const char *state_names[] = {
    "SYS_INIT",
    "SYS_IDLE",
    "SYS_RECORDING",
    "SYS_RECORDING_AND_UPLOADING",
    "SYS_UPLOADING",
    "SYS_OTA",
    "SYS_ERROR",
    "SYS_SLEEP",
    "SYS_SHUTDOWN",
    "SYS_RESET"
};

/* Event names for debugging */
static const char *event_names[] = {
    "SYS_EVT_POWER_ON",
    "SYS_EVT_POWER_OFF",
    "SYS_EVT_REC_START",
    "SYS_EVT_REC_STOP",
    "SYS_EVT_REC_PAUSE",
    "SYS_EVT_REC_RESUME",
    "SYS_EVT_UPLOAD_START",
    "SYS_EVT_UPLOAD_COMPLETE",
    "SYS_EVT_NETWORK_CONNECTED",
    "SYS_EVT_NETWORK_DISCONNECTED",
    "SYS_EVT_BLUETOOTH_CONNECTED",
    "SYS_EVT_BLUETOOTH_DISCONNECTED",
    "SYS_EVT_LOW_BATTERY",
    "SYS_EVT_CHARGING",
    "SYS_EVT_ERROR",
    "SYS_EVT_RESET",
    "SYS_EVT_TRANSFER_MODE_CHANGE",
    "SYS_EVT_STREAMING_START",
    "SYS_EVT_STREAMING_STOP",
    "SYS_EVT_FILE_UPLOAD_START",
    "SYS_EVT_FILE_UPLOAD_COMPLETE",
    /* OTA events */
    "SYS_EVT_OTA_START",
    "SYS_EVT_OTA_CANCEL",
    "SYS_EVT_OTA_CHECK",
    "SYS_EVT_OTA_STATUS",
    "SYS_EVT_OTA_PROGRESS",
    "SYS_EVT_OTA_COMPLETE",
    "SYS_EVT_OTA_ERROR"
};

/* Internal helper functions */
static void transition_to_state(struct system_coordinator *sys, enum system_state new_state);
static void forward_ota_event(struct system_coordinator *sys, enum system_event event, void *data);

/* Check if there is a pending firmware update */
static int has_pending_update(struct system_coordinator *sys) {
    if (!sys || !sys->ota_fsm) {
        return 0; /* No OTA FSM, no pending update */
    }
    
    /* Check if OTA FSM is in IDLE state and has a package URL set */
    if (ota_fsm_is_idle(sys->ota_fsm)) {
        /* In a real implementation, you would check for available updates
         * by calling a server or checking a configuration file.
         * For simulation purposes, we check if package URL is not empty
         * and not the default URL.
         */
        const char *default_url = "https://ota.example.com/firmware.bin";
        const char *current_url = sys->ota_fsm->package_url;
        
        if (current_url && current_url[0] != '\0' && 
            strcmp(current_url, default_url) != 0) {
            /* Non-default URL suggests a pending update */
            return 1;
        }
    }
    
    /* Check if OTA FSM is in CHECKING state (actively checking for updates) */
    if (ota_fsm_get_state(sys->ota_fsm) == OTA_CHECKING) {
        return 1; /* Update check in progress */
    }
    
    return 0; /* No pending update */
}

/* State entry handlers */
static void enter_init(struct system_coordinator *sys) {
    printf("System entering INIT state\n");
    
    /* Initialize all subsystems */
    int init_success = 1;
    
    if (sys->recording_fsm) {
        /* Initialize recording FSM */
        recording_fsm_init(sys->recording_fsm);
        if (recording_fsm_get_error_code(sys->recording_fsm) != 0) {
            printf("Recording FSM initialization failed: %s\n", 
                   recording_fsm_get_error_msg(sys->recording_fsm));
            init_success = 0;
        }
    }
    
    if (sys->comm_fsm) {
        /* Initialize communication FSM */
        /* Note: comm_fsm_init would be called by the owner */
        /* For now, just log */
        printf("Communication FSM initialization (deferred)\n");
    }
    
    if (sys->power_fsm) {
        /* Initialize power FSM */
        /* Note: power_fsm_init would be called by the owner */
        printf("Power FSM initialization (deferred)\n");
    }
    
    if (sys->audio_fsm) {
        /* Initialize audio FSM */
        /* Note: audio_fsm_init would be called by the owner */
        printf("Audio FSM initialization (deferred)\n");
    }
    
    if (sys->efsm_protocol) {
        /* Initialize EFSM Protocol State Machine */
        /* Note: efsm_processor_init would be called by the owner */
        printf("EFSM Protocol initialization (deferred)\n");
    }
    
    if (sys->ota_fsm) {
        /* Initialize OTA FSM */
        /* Note: ota_fsm_init would be called by the owner */
        printf("OTA FSM initialization (deferred)\n");
    }
    
    /* Check initialization success */
    if (!init_success) {
        printf("Subsystem initialization failed, transitioning to ERROR state\n");
        system_coordinator_set_error(sys, -1, "Subsystem initialization failed");
        transition_to_state(sys, SYS_ERROR);
        return;
    }
    
    /* After initialization, check if there is a pending firmware update */
    if (has_pending_update(sys)) {
        printf("Pending firmware update detected, transitioning to OTA state\n");
        transition_to_state(sys, SYS_OTA);
    } else {
        printf("No pending firmware update, transitioning to IDLE state\n");
        transition_to_state(sys, SYS_IDLE);
    }
}

static void enter_idle(struct system_coordinator *sys) {
    (void)sys; /* Unused parameter */
    printf("System entering IDLE state\n");
    /* System is ready for user interaction */
    /* Activate low-power mode if needed */
}

static void enter_recording(struct system_coordinator *sys) {
    printf("System entering RECORDING state\n");
    /* Start recording process */
    /* Activate audio processing */
    /* Establish Bluetooth connection if needed */
    /* Start recording FSM */
    if (sys->recording_fsm) {
        recording_fsm_dispatch_event(sys->recording_fsm, REC_EVT_START, NULL);
    }
    /* Ensure audio FSM is in recording mode */
    if (sys->audio_fsm) {
        audio_fsm_start_recording(sys->audio_fsm);
    }
    /* Ensure power FSM is in high-performance mode */
    if (sys->power_fsm) {
        power_fsm_dispatch_event(sys->power_fsm, POWER_EVT_WAKE_UP, NULL);
    }
    printf("Recording started\n");
}

static void enter_uploading(struct system_coordinator *sys) {
    printf("System entering UPLOADING state\n");
    /* Start file upload process */
    /* Activate 4G network connection */
    
    /* Ensure communication is connected for upload */
    if (sys->comm_fsm && !comm_fsm_is_connected(sys->comm_fsm)) {
        /* Attempt to connect */
        comm_fsm_dispatch_event(sys->comm_fsm, COMM_EVT_CONNECT, NULL);
    }
    
    /* Start file transfer */
    if (sys->comm_fsm && comm_fsm_is_connected(sys->comm_fsm)) {
        /* Notify communication FSM to start file upload */
        comm_fsm_dispatch_event(sys->comm_fsm, COMM_EVT_DATA_READY, NULL);
    }
    
    /* If there is a recording FSM, ensure recording is stopped */
    if (sys->recording_fsm && recording_fsm_is_recording(sys->recording_fsm)) {
        recording_fsm_dispatch_event(sys->recording_fsm, REC_EVT_STOP, NULL);
    }
    
    printf("Uploading started\n");
}

static void enter_recording_and_uploading(struct system_coordinator *sys) {
    printf("System entering RECORDING_AND_UPLOADING state\n");
    /* Start both recording and streaming transfer */
    /* Activate audio processing and network connection */
    
    /* Ensure recording is started */
    if (sys->recording_fsm && !recording_fsm_is_recording(sys->recording_fsm)) {
        recording_fsm_dispatch_event(sys->recording_fsm, REC_EVT_START, NULL);
    }
    
    /* Ensure communication is connected for streaming */
    if (sys->comm_fsm && !comm_fsm_is_connected(sys->comm_fsm)) {
        /* Attempt to connect */
        comm_fsm_dispatch_event(sys->comm_fsm, COMM_EVT_CONNECT, NULL);
    }
    
    /* Start streaming transfer */
    if (sys->comm_fsm && comm_fsm_is_connected(sys->comm_fsm)) {
        /* Notify communication FSM to start data transfer */
        comm_fsm_dispatch_event(sys->comm_fsm, COMM_EVT_DATA_READY, NULL);
    }
    
    printf("Recording and uploading started\n");
}

static void enter_error(struct system_coordinator *sys) {
    printf("System entering ERROR state: %s\n", sys->error_msg);
    /* Handle error condition */
    /* Notify user if possible */
    /* Attempt recovery if configured */
}

static void enter_sleep(struct system_coordinator *sys) {
    printf("System entering SLEEP state\n");
    /* Check if OTA is in progress - if yes, cancel it */
    if (sys->ota_fsm && ota_fsm_is_upgrading(sys->ota_fsm)) {
        printf("OTA in progress - cancelling before sleep\n");
        ota_fsm_dispatch_event(sys->ota_fsm, OTA_EVT_CANCEL, NULL);
    }
    /* Put subsystems into low-power mode */
    if (sys->power_fsm) {
        /* Notify power FSM to enter sleep */
        power_fsm_dispatch_event(sys->power_fsm, POWER_EVT_ENTER_SLEEP, NULL);
    }
    /* Stop recording if active */
    if (sys->recording_fsm && recording_fsm_is_recording(sys->recording_fsm)) {
        recording_fsm_dispatch_event(sys->recording_fsm, REC_EVT_STOP, NULL);
    }
    /* Disconnect communication if connected */
    if (sys->comm_fsm && comm_fsm_is_connected(sys->comm_fsm)) {
        comm_fsm_dispatch_event(sys->comm_fsm, COMM_EVT_DISCONNECT, NULL);
    }
    /* Put audio FSM into sleep */
    if (sys->audio_fsm) {
        audio_fsm_sleep(sys->audio_fsm);
    }
    /* Ensure EFSM protocol is paused */
    if (sys->efsm_protocol) {
        efsm_protocol_pause(sys->efsm_protocol);
    }
    printf("All subsystems set to low-power mode\n");
}

static void enter_ota(struct system_coordinator *sys) {
    printf("System entering OTA state\n");
    /* Ensure power FSM is in charging mode if possible */
    if (sys->power_fsm) {
        power_fsm_dispatch_event(sys->power_fsm, POWER_EVT_CHARGING_START, NULL);
    }
    /* Ensure communication is connected */
    if (sys->comm_fsm && !comm_fsm_is_connected(sys->comm_fsm)) {
        comm_fsm_dispatch_event(sys->comm_fsm, COMM_EVT_CONNECT, NULL);
    }
    /* Start OTA process */
    /* Notify OTA FSM to start upgrade */
    if (sys->ota_fsm) {
        ota_fsm_dispatch_event(sys->ota_fsm, OTA_EVT_START, NULL);
    }
    /* Block other subsystems from starting new operations */
    printf("OTA process started\n");
}

static void enter_reset(struct system_coordinator *sys) {
    printf("System entering RESET state\n");
    /* Perform system reset operations */
    /* Reset all subsystems */
    if (sys->recording_fsm) {
        /* Reset recording FSM */
        recording_fsm_reset(sys->recording_fsm);
    }
    if (sys->comm_fsm) {
        /* Reset communication FSM */
        comm_fsm_reset(sys->comm_fsm);
    }
    if (sys->power_fsm) {
        /* Reset power FSM */
        power_fsm_reset(sys->power_fsm);
    }
    if (sys->audio_fsm) {
        /* Reset audio FSM */
        audio_fsm_reset(sys->audio_fsm);
    }
    if (sys->efsm_protocol) {
        /* Reset EFSM Protocol State Machine */
        efsm_protocol_reset(sys->efsm_protocol);
    }
    if (sys->ota_fsm) {
        /* Reset OTA FSM */
        ota_fsm_reset(sys->ota_fsm);
    }
    
    /* Clear error state */
    sys->error_code = 0;
    sys->error_msg[0] = '\0';
    
    /* After reset, transition to INIT state */
    transition_to_state(sys, SYS_INIT);
}

/* Optional init and destroy handlers */
static void init(struct system_coordinator *sys) {
    /* Optional initialization */
    printf("System coordinator init (optional)\n");
}

static void destroy(struct system_coordinator *sys) {
    /* Optional cleanup */
    printf("System coordinator destroy (optional)\n");
}

/* State exit handler */
static void exit_state(struct system_coordinator *sys) {
    printf("System exiting state %s\n", state_names[sys->current_state]);
    /* Clean up current state if needed */
    switch (sys->current_state) {
        case SYS_RECORDING:
            /* Stop any pending recording timers */
            break;
        case SYS_UPLOADING:
            /* Cancel any pending upload operations */
            break;
        case SYS_OTA:
            /* Ensure OTA is paused if possible */
            break;
        case SYS_SLEEP:
            /* Wake up any sleeping subsystems */
            break;
        default:
            /* No special cleanup */
            break;
    }
}

/* Do action handler */
static void do_action(struct system_coordinator *sys) {
    /* Periodic actions based on current state */
    switch (sys->current_state) {
        case SYS_RECORDING:
            /* Monitor recording progress */
            if (sys->recording_fsm) {
                int duration = recording_fsm_get_duration(sys->recording_fsm);
                printf("Recording duration: %d seconds\n", duration);
                /* Check for errors */
                if (recording_fsm_get_error_code(sys->recording_fsm) != 0) {
                    printf("Recording error detected: %s\n", 
                           recording_fsm_get_error_msg(sys->recording_fsm));
                    system_coordinator_dispatch_event(sys, SYS_EVT_ERROR, NULL);
                }
            }
            /* Check battery level */
            if (sys->power_fsm) {
                int battery = power_fsm_get_battery_level(sys->power_fsm);
                if (battery < 20) {
                    printf("Low battery during recording\n");
                    system_coordinator_dispatch_event(sys, SYS_EVT_LOW_BATTERY, NULL);
                }
            }
            break;
        case SYS_RECORDING_AND_UPLOADING:
            /* Monitor both recording and streaming */
            if (sys->recording_fsm) {
                int duration = recording_fsm_get_duration(sys->recording_fsm);
                printf("Recording duration: %d seconds\n", duration);
            }
            if (sys->comm_fsm) {
                int upload_speed = comm_fsm_get_upload_speed(sys->comm_fsm);
                printf("Upload speed: %d kbps\n", upload_speed);
                if (!comm_fsm_is_connected(sys->comm_fsm)) {
                    printf("Network disconnected during streaming\n");
                    system_coordinator_dispatch_event(sys, SYS_EVT_NETWORK_DISCONNECTED, NULL);
                }
            }
            break;
        case SYS_UPLOADING:
            /* Monitor upload progress */
            if (sys->comm_fsm) {
                int progress = comm_fsm_get_upload_progress(sys->comm_fsm);
                printf("Upload progress: %d%%\n", progress);
                if (!comm_fsm_is_connected(sys->comm_fsm)) {
                    printf("Network disconnected during upload\n");
                    system_coordinator_dispatch_event(sys, SYS_EVT_NETWORK_DISCONNECTED, NULL);
                }
            }
            break;
        case SYS_IDLE:
            /* Periodic system checks */
            if (sys->power_fsm) {
                int battery = power_fsm_get_battery_level(sys->power_fsm);
                printf("Battery level: %d%%\n", battery);
                if (battery < 10) {
                    printf("Critical battery, entering sleep\n");
                    system_coordinator_dispatch_event(sys, SYS_EVT_LOW_BATTERY, NULL);
                }
            }
            if (sys->ota_fsm && ota_fsm_has_update(sys->ota_fsm)) {
                printf("OTA update available, prompting user\n");
                /* Could auto-start OTA or wait for user confirmation */
            }
            break;
        case SYS_OTA:
            /* Monitor OTA progress */
            if (sys->ota_fsm) {
                int progress = ota_fsm_get_progress(sys->ota_fsm);
                printf("OTA progress: %d%%\n", progress);
                if (ota_fsm_get_error_code(sys->ota_fsm) != 0) {
                    printf("OTA error: %s\n", ota_fsm_get_error_msg(sys->ota_fsm));
                    system_coordinator_dispatch_event(sys, SYS_EVT_OTA_ERROR, NULL);
                }
            }
            break;
        case SYS_SLEEP:
            /* Check if we should wake up (e.g., charging started) */
            if (sys->power_fsm && power_fsm_is_charging(sys->power_fsm)) {
                printf("Charging detected, waking up\n");
                system_coordinator_dispatch_event(sys, SYS_EVT_POWER_ON, NULL);
            }
            break;
        default:
            /* No periodic actions for other states */
            break;
    }
}

/* Event handlers for each state */
static void handle_init_state(struct system_coordinator *sys, enum system_event event, void *data) {
    switch (event) {
        case SYS_EVT_RESET:
            printf("Reset requested during INIT state\n");
            transition_to_state(sys, SYS_RESET);
            break;
        default:
            /* INIT state only handles internal transitions */
            (void)sys; (void)event; (void)data;
            break;
    }
}

static void handle_idle_state(struct system_coordinator *sys, enum system_event event, void *data) {
    (void)data; /* Unused parameter */
    switch (event) {
        case SYS_EVT_REC_START:
            printf("Starting recording from IDLE state\n");
            transition_to_state(sys, SYS_RECORDING);
            break;
        case SYS_EVT_POWER_OFF:
            printf("Powering off from IDLE state\n");
            transition_to_state(sys, SYS_SHUTDOWN);
            break;
        case SYS_EVT_LOW_BATTERY:
            printf("Low battery warning in IDLE state - entering SLEEP\n");
            transition_to_state(sys, SYS_SLEEP);
            break;
        case SYS_EVT_ERROR:
            printf("Error in IDLE state\n");
            transition_to_state(sys, SYS_ERROR);
            break;
        case SYS_EVT_OTA_START:
            printf("OTA start requested from IDLE state\n");
            transition_to_state(sys, SYS_OTA);
            break;
        case SYS_EVT_RESET:
            printf("Reset requested from IDLE state\n");
            transition_to_state(sys, SYS_RESET);
            break;
        case SYS_EVT_POWER_ON:
            printf("Power on in IDLE state (already powered)\n");
            break;
        case SYS_EVT_BLUETOOTH_CONNECTED:
            printf("Bluetooth connected\n");
            break;
        case SYS_EVT_BLUETOOTH_DISCONNECTED:
            printf("Bluetooth disconnected\n");
            break;
        case SYS_EVT_NETWORK_CONNECTED:
            printf("Network connected\n");
            break;
        case SYS_EVT_NETWORK_DISCONNECTED:
            printf("Network disconnected\n");
            break;
        case SYS_EVT_CHARGING:
            printf("Charging started\n");
            break;
        case SYS_EVT_TRANSFER_MODE_CHANGE:
            printf("Transfer mode changed\n");
            break;
        case SYS_EVT_STREAMING_START:
            printf("Streaming start requested, but system is idle\n");
            break;
        case SYS_EVT_STREAMING_STOP:
            /* Ignore */
            break;
        case SYS_EVT_FILE_UPLOAD_START:
            printf("File upload start requested, transitioning to UPLOADING\n");
            transition_to_state(sys, SYS_UPLOADING);
            break;
        case SYS_EVT_FILE_UPLOAD_COMPLETE:
            /* Ignore */
            break;
        case SYS_EVT_OTA_CHECK:
            printf("OTA check requested\n");
            /* Forwarded to OTA FSM */
            break;
        case SYS_EVT_OTA_STATUS:
            printf("OTA status requested\n");
            /* Forwarded to OTA FSM */
            break;
        case SYS_EVT_OTA_PROGRESS:
            printf("OTA progress update\n");
            /* Forwarded to OTA FSM */
            break;
        default:
            printf("Unhandled event %s in IDLE state\n", event_names[event]);
            break;
    }
}

static void handle_recording_state(struct system_coordinator *sys, enum system_event event, void *data) {
    switch (event) {
        case SYS_EVT_REC_STOP:
            printf("Stopping recording\n");
            transition_to_state(sys, SYS_UPLOADING);
            break;
        case SYS_EVT_REC_PAUSE:
            printf("Pausing recording\n");
            if (sys->recording_fsm) {
                recording_fsm_dispatch_event(sys->recording_fsm, REC_EVT_PAUSE, data);
            }
            /* System stays in RECORDING state but recording FSM will be paused */
            break;
        case SYS_EVT_REC_RESUME:
            printf("Resuming recording\n");
            if (sys->recording_fsm) {
                recording_fsm_dispatch_event(sys->recording_fsm, REC_EVT_RESUME, data);
            }
            break;
        case SYS_EVT_STREAMING_START:
            printf("Starting streaming while recording\n");
            transition_to_state(sys, SYS_RECORDING_AND_UPLOADING);
            break;
        case SYS_EVT_ERROR:
            printf("Error during recording\n");
            transition_to_state(sys, SYS_ERROR);
            break;
        case SYS_EVT_LOW_BATTERY:
            printf("Low battery during recording\n");
            /* Handle low battery - may need to stop recording */
            break;
        case SYS_EVT_RESET:
            printf("Reset requested during recording\n");
            transition_to_state(sys, SYS_RESET);
            break;
        case SYS_EVT_NETWORK_CONNECTED:
            printf("Network connected while recording, transitioning to RECORDING_AND_UPLOADING\n");
            transition_to_state(sys, SYS_RECORDING_AND_UPLOADING);
            break;
        case SYS_EVT_UPLOAD_START:
            printf("Upload start requested while recording, transitioning to RECORDING_AND_UPLOADING\n");
            transition_to_state(sys, SYS_RECORDING_AND_UPLOADING);
            break;
        case SYS_EVT_UPLOAD_COMPLETE:
            /* No upload in progress, ignore */
            break;
        case SYS_EVT_CHARGING:
            printf("Charging started while recording\n");
            break;
        case SYS_EVT_BLUETOOTH_CONNECTED:
            printf("Bluetooth connected while recording\n");
            break;
        case SYS_EVT_BLUETOOTH_DISCONNECTED:
            printf("Bluetooth disconnected while recording\n");
            break;
        case SYS_EVT_TRANSFER_MODE_CHANGE:
            printf("Transfer mode changed while recording\n");
            break;
        case SYS_EVT_STREAMING_STOP:
            /* Not streaming yet, ignore */
            break;
        case SYS_EVT_FILE_UPLOAD_START:
            printf("File upload start requested while recording, transitioning to RECORDING_AND_UPLOADING\n");
            transition_to_state(sys, SYS_RECORDING_AND_UPLOADING);
            break;
        case SYS_EVT_FILE_UPLOAD_COMPLETE:
            /* Ignore */
            break;
        case SYS_EVT_OTA_CHECK:
            /* Forwarded to OTA FSM */
            break;
        case SYS_EVT_OTA_STATUS:
            /* Forwarded to OTA FSM */
            break;
        case SYS_EVT_OTA_PROGRESS:
            /* Forwarded to OTA FSM */
            break;
        default:
            printf("Unhandled event %s in RECORDING state\n", event_names[event]);
            break;
    }
}

static void handle_recording_and_uploading_state(struct system_coordinator *sys, enum system_event event, void *data) {
    (void)data; /* Unused parameter */
    switch (event) {
        case SYS_EVT_REC_STOP:
            printf("Stopping recording in RECORDING_AND_UPLOADING state\n");
            transition_to_state(sys, SYS_UPLOADING);
            break;
        case SYS_EVT_STREAMING_STOP:
            printf("Stopping streaming in RECORDING_AND_UPLOADING state\n");
            transition_to_state(sys, SYS_RECORDING);
            break;
        case SYS_EVT_UPLOAD_COMPLETE:
            printf("Upload complete while recording\n");
            /* Continue recording, stop uploading */
            transition_to_state(sys, SYS_RECORDING);
            break;
        case SYS_EVT_NETWORK_DISCONNECTED:
            printf("Network disconnected during streaming\n");
            /* Continue recording only */
            transition_to_state(sys, SYS_RECORDING);
            break;
        case SYS_EVT_ERROR:
            printf("Error during recording and uploading\n");
            transition_to_state(sys, SYS_ERROR);
            break;
        case SYS_EVT_RESET:
            printf("Reset requested during recording and uploading\n");
            transition_to_state(sys, SYS_RESET);
            break;
        case SYS_EVT_REC_PAUSE:
            printf("Pausing recording while uploading\n");
            if (sys->recording_fsm) {
                recording_fsm_dispatch_event(sys->recording_fsm, REC_EVT_PAUSE, data);
            }
            break;
        case SYS_EVT_REC_RESUME:
            printf("Resuming recording while uploading\n");
            if (sys->recording_fsm) {
                recording_fsm_dispatch_event(sys->recording_fsm, REC_EVT_RESUME, data);
            }
            break;
        case SYS_EVT_NETWORK_CONNECTED:
            /* Already connected, ignore */
            break;
        case SYS_EVT_UPLOAD_START:
            /* Already uploading, ignore */
            break;
        case SYS_EVT_CHARGING:
            printf("Charging started while recording and uploading\n");
            break;
        case SYS_EVT_BLUETOOTH_CONNECTED:
            printf("Bluetooth connected while recording and uploading\n");
            break;
        case SYS_EVT_BLUETOOTH_DISCONNECTED:
            printf("Bluetooth disconnected while recording and uploading\n");
            break;
        case SYS_EVT_TRANSFER_MODE_CHANGE:
            printf("Transfer mode changed while recording and uploading\n");
            break;
        case SYS_EVT_STREAMING_START:
            /* Already streaming, ignore */
            break;
        case SYS_EVT_FILE_UPLOAD_START:
            /* Already uploading, ignore */
            break;
        case SYS_EVT_FILE_UPLOAD_COMPLETE:
            /* Ignore */
            break;
        case SYS_EVT_OTA_CHECK:
            /* Forwarded to OTA FSM */
            break;
        case SYS_EVT_OTA_STATUS:
            /* Forwarded to OTA FSM */
            break;
        case SYS_EVT_OTA_PROGRESS:
            /* Forwarded to OTA FSM */
            break;
        default:
            printf("Unhandled event %s in RECORDING_AND_UPLOADING state\n", event_names[event]);
            break;
    }
}

static void handle_uploading_state(struct system_coordinator *sys, enum system_event event, void *data) {
    (void)data; /* Unused parameter */
    switch (event) {
        case SYS_EVT_UPLOAD_COMPLETE:
            printf("Upload complete\n");
            transition_to_state(sys, SYS_IDLE);
            break;
        case SYS_EVT_NETWORK_DISCONNECTED:
            printf("Network disconnected during upload\n");
            /* Handle network loss - retry or error */
            break;
        case SYS_EVT_ERROR:
            printf("Error during upload\n");
            transition_to_state(sys, SYS_ERROR);
            break;
        case SYS_EVT_RESET:
            printf("Reset requested during uploading\n");
            transition_to_state(sys, SYS_RESET);
            break;
        case SYS_EVT_NETWORK_CONNECTED:
            /* Already connected, ignore */
            break;
        case SYS_EVT_UPLOAD_START:
            /* Already uploading, ignore */
            break;
        case SYS_EVT_CHARGING:
            printf("Charging started while uploading\n");
            break;
        case SYS_EVT_BLUETOOTH_CONNECTED:
            printf("Bluetooth connected while uploading\n");
            break;
        case SYS_EVT_BLUETOOTH_DISCONNECTED:
            printf("Bluetooth disconnected while uploading\n");
            break;
        case SYS_EVT_TRANSFER_MODE_CHANGE:
            printf("Transfer mode changed while uploading\n");
            break;
        case SYS_EVT_STREAMING_START:
            /* Start streaming while uploading? Not possible without recording */
            break;
        case SYS_EVT_STREAMING_STOP:
            /* Ignore */
            break;
        case SYS_EVT_FILE_UPLOAD_START:
            /* Already uploading, ignore */
            break;
        case SYS_EVT_FILE_UPLOAD_COMPLETE:
            /* Ignore */
            break;
        case SYS_EVT_OTA_CHECK:
            /* Forwarded to OTA FSM */
            break;
        case SYS_EVT_OTA_STATUS:
            /* Forwarded to OTA FSM */
            break;
        case SYS_EVT_OTA_PROGRESS:
            /* Forwarded to OTA FSM */
            break;
        default:
            printf("Unhandled event %s in UPLOADING state\n", event_names[event]);
            break;
    }
}

static void handle_error_state(struct system_coordinator *sys, enum system_event event, void *data) {
    (void)data; /* Unused parameter */
    switch (event) {
        case SYS_EVT_RESET:
            printf("Resetting from ERROR state\n");
            transition_to_state(sys, SYS_RESET);
            break;
        case SYS_EVT_POWER_OFF:
            printf("Powering off from ERROR state\n");
            transition_to_state(sys, SYS_SHUTDOWN);
            break;
        case SYS_EVT_NETWORK_CONNECTED:
            printf("Network connected while in ERROR state\n");
            break;
        case SYS_EVT_NETWORK_DISCONNECTED:
            printf("Network disconnected while in ERROR state\n");
            break;
        case SYS_EVT_BLUETOOTH_CONNECTED:
            printf("Bluetooth connected while in ERROR state\n");
            break;
        case SYS_EVT_BLUETOOTH_DISCONNECTED:
            printf("Bluetooth disconnected while in ERROR state\n");
            break;
        case SYS_EVT_CHARGING:
            printf("Charging started while in ERROR state\n");
            break;
        case SYS_EVT_TRANSFER_MODE_CHANGE:
            printf("Transfer mode changed while in ERROR state\n");
            break;
        case SYS_EVT_STREAMING_START:
            /* Cannot start streaming while in error */
            break;
        case SYS_EVT_STREAMING_STOP:
            /* Ignore */
            break;
        case SYS_EVT_FILE_UPLOAD_START:
            /* Cannot start upload while in error */
            break;
        case SYS_EVT_FILE_UPLOAD_COMPLETE:
            /* Ignore */
            break;
        case SYS_EVT_OTA_CHECK:
            /* Forwarded to OTA FSM */
            break;
        case SYS_EVT_OTA_STATUS:
            /* Forwarded to OTA FSM */
            break;
        case SYS_EVT_OTA_PROGRESS:
            /* Forwarded to OTA FSM */
            break;
        default:
            printf("Unhandled event %s in ERROR state\n", event_names[event]);
            break;
    }
}

static void handle_sleep_state(struct system_coordinator *sys, enum system_event event, void *data) {
    (void)data; /* Unused parameter */
    switch (event) {
        case SYS_EVT_POWER_ON:
            printf("Waking from SLEEP state\n");
            transition_to_state(sys, SYS_IDLE);
            break;
        case SYS_EVT_POWER_OFF:
            printf("Power off from SLEEP state - entering SHUTDOWN\n");
            transition_to_state(sys, SYS_SHUTDOWN);
            break;
        case SYS_EVT_RESET:
            printf("Reset requested from SLEEP state\n");
            transition_to_state(sys, SYS_RESET);
            break;
        case SYS_EVT_NETWORK_CONNECTED:
            printf("Network connected while sleeping\n");
            break;
        case SYS_EVT_NETWORK_DISCONNECTED:
            printf("Network disconnected while sleeping\n");
            break;
        case SYS_EVT_BLUETOOTH_CONNECTED:
            printf("Bluetooth connected while sleeping\n");
            break;
        case SYS_EVT_BLUETOOTH_DISCONNECTED:
            printf("Bluetooth disconnected while sleeping\n");
            break;
        case SYS_EVT_CHARGING:
            printf("Charging started while sleeping\n");
            break;
        case SYS_EVT_TRANSFER_MODE_CHANGE:
            printf("Transfer mode changed while sleeping\n");
            break;
        case SYS_EVT_STREAMING_START:
            /* Cannot start streaming while sleeping */
            break;
        case SYS_EVT_STREAMING_STOP:
            /* Ignore */
            break;
        case SYS_EVT_FILE_UPLOAD_START:
            /* Cannot start upload while sleeping */
            break;
        case SYS_EVT_FILE_UPLOAD_COMPLETE:
            /* Ignore */
            break;
        case SYS_EVT_OTA_CHECK:
            /* Forwarded to OTA FSM */
            break;
        case SYS_EVT_OTA_STATUS:
            /* Forwarded to OTA FSM */
            break;
        case SYS_EVT_OTA_PROGRESS:
            /* Forwarded to OTA FSM */
            break;
        default:
            printf("Unhandled event %s in SLEEP state\n", event_names[event]);
            break;
    }
}

static void handle_ota_state(struct system_coordinator *sys, enum system_event event, void *data) {
    (void)data; /* Unused parameter */
    switch (event) {
        case SYS_EVT_OTA_COMPLETE:
            printf("OTA update completed successfully\n");
            transition_to_state(sys, SYS_IDLE);
            break;
        case SYS_EVT_OTA_CANCEL:
            printf("OTA update cancelled\n");
            transition_to_state(sys, SYS_IDLE);
            break;
        case SYS_EVT_OTA_ERROR:
            printf("OTA update failed\n");
            transition_to_state(sys, SYS_ERROR);
            break;
        case SYS_EVT_RESET:
            printf("Reset requested during OTA - cancelling OTA and resetting\n");
            /* Cancel OTA and go to reset */
            if (sys->ota_fsm) {
                ota_fsm_dispatch_event(sys->ota_fsm, OTA_EVT_CANCEL, NULL);
            }
            transition_to_state(sys, SYS_RESET);
            break;
        case SYS_EVT_POWER_OFF:
            printf("Power off during OTA - cancelling\n");
            /* Cancel OTA and go to shutdown */
            if (sys->ota_fsm) {
                ota_fsm_dispatch_event(sys->ota_fsm, OTA_EVT_CANCEL, NULL);
            }
            transition_to_state(sys, SYS_SHUTDOWN);
            break;
        case SYS_EVT_NETWORK_CONNECTED:
            printf("Network connected during OTA\n");
            break;
        case SYS_EVT_NETWORK_DISCONNECTED:
            printf("Network disconnected during OTA\n");
            break;
        case SYS_EVT_BLUETOOTH_CONNECTED:
            printf("Bluetooth connected during OTA\n");
            break;
        case SYS_EVT_BLUETOOTH_DISCONNECTED:
            printf("Bluetooth disconnected during OTA\n");
            break;
        case SYS_EVT_CHARGING:
            printf("Charging started during OTA\n");
            break;
        case SYS_EVT_TRANSFER_MODE_CHANGE:
            printf("Transfer mode changed during OTA\n");
            break;
        case SYS_EVT_STREAMING_START:
            /* Cannot start streaming while OTA */
            break;
        case SYS_EVT_STREAMING_STOP:
            /* Ignore */
            break;
        case SYS_EVT_FILE_UPLOAD_START:
            /* Cannot start upload while OTA */
            break;
        case SYS_EVT_FILE_UPLOAD_COMPLETE:
            /* Ignore */
            break;
        case SYS_EVT_OTA_CHECK:
            /* Already in OTA, ignore */
            break;
        case SYS_EVT_OTA_STATUS:
            /* Already in OTA, ignore */
            break;
        case SYS_EVT_OTA_PROGRESS:
            /* Already in OTA, ignore */
            break;
        default:
            printf("Unhandled event %s in OTA state\n", event_names[event]);
            break;
    }
}

static void handle_reset_state(struct system_coordinator *sys, enum system_event event, void *data) {
    /* RESET state is transient - it automatically transitions to INIT */
    /* No events are processed in RESET state */
    (void)sys; (void)event; (void)data; /* Unused parameters */
    printf("System in RESET state - no events processed\n");
}

static void handle_shutdown_state(struct system_coordinator *sys, enum system_event event, void *data) {
    (void)data; /* Unused parameter */
    switch (event) {
        case SYS_EVT_POWER_ON:
            printf("Power on from SHUTDOWN state - transitioning to INIT\n");
            transition_to_state(sys, SYS_INIT);
            break;
        case SYS_EVT_RESET:
            printf("Reset requested from SHUTDOWN state\n");
            transition_to_state(sys, SYS_RESET);
            break;
        case SYS_EVT_NETWORK_CONNECTED:
            printf("Network connected while shutdown\n");
            break;
        case SYS_EVT_NETWORK_DISCONNECTED:
            printf("Network disconnected while shutdown\n");
            break;
        case SYS_EVT_BLUETOOTH_CONNECTED:
            printf("Bluetooth connected while shutdown\n");
            break;
        case SYS_EVT_BLUETOOTH_DISCONNECTED:
            printf("Bluetooth disconnected while shutdown\n");
            break;
        case SYS_EVT_CHARGING:
            printf("Charging started while shutdown\n");
            break;
        case SYS_EVT_TRANSFER_MODE_CHANGE:
            printf("Transfer mode changed while shutdown\n");
            break;
        case SYS_EVT_STREAMING_START:
            /* Cannot start streaming while shutdown */
            break;
        case SYS_EVT_STREAMING_STOP:
            /* Ignore */
            break;
        case SYS_EVT_FILE_UPLOAD_START:
            /* Cannot start upload while shutdown */
            break;
        case SYS_EVT_FILE_UPLOAD_COMPLETE:
            /* Ignore */
            break;
        case SYS_EVT_OTA_CHECK:
            /* Forwarded to OTA FSM */
            break;
        case SYS_EVT_OTA_STATUS:
            /* Forwarded to OTA FSM */
            break;
        case SYS_EVT_OTA_PROGRESS:
            /* Forwarded to OTA FSM */
            break;
        default:
            printf("Unhandled event %s in SHUTDOWN state\n", event_names[event]);
            break;
    }
}

/* Forward OTA event to OTA FSM */
static void forward_ota_event(struct system_coordinator *sys, enum system_event event, void *data) {
    if (!sys || !sys->ota_fsm) {
        return;
    }
    
    /* Map system event to OTA FSM event */
    enum ota_event ota_evt;
    switch (event) {
        case SYS_EVT_OTA_START:
            ota_evt = OTA_EVT_START;
            break;
        case SYS_EVT_OTA_CANCEL:
            ota_evt = OTA_EVT_CANCEL;
            break;
        case SYS_EVT_OTA_CHECK:
            ota_evt = OTA_EVT_CHECK_UPDATE;
            break;
        case SYS_EVT_OTA_STATUS:
            /* Status query - no direct mapping, treat as check update */
            ota_evt = OTA_EVT_CHECK_UPDATE;
            break;
        case SYS_EVT_OTA_PROGRESS:
            /* Progress update - map to download progress */
            ota_evt = OTA_EVT_DOWNLOAD_PROGRESS;
            break;
        case SYS_EVT_OTA_COMPLETE:
            /* OTA complete - map to install complete */
            ota_evt = OTA_EVT_INSTALL_COMPLETE;
            break;
        case SYS_EVT_OTA_ERROR:
            ota_evt = OTA_EVT_ERROR;
            break;
        default:
            /* Not an OTA event */
            return;
    }
    
    /* Dispatch to OTA FSM */
    ota_fsm_dispatch_event(sys->ota_fsm, ota_evt, data);
}

/* Main event handler */
static void handle_event(struct system_coordinator *sys, enum system_event event, void *data) {
    printf("System handling event: %s in state: %s\n", 
          event_names[event], state_names[sys->current_state]);
    
    /* Forward OTA events to OTA FSM */
    if (event >= SYS_EVT_OTA_START && event <= SYS_EVT_OTA_ERROR) {
        forward_ota_event(sys, event, data);
        /* OTA events may also affect system state, but for now we just forward */
        /* Optionally, we could prevent further processing */
        /* Do NOT return here; allow state-specific handler to process as well */
    }
    
    /* Call state-specific event handler */
    switch (sys->current_state) {
        case SYS_INIT:
            handle_init_state(sys, event, data);
            break;
        case SYS_IDLE:
            handle_idle_state(sys, event, data);
            break;
        case SYS_RECORDING:
            handle_recording_state(sys, event, data);
            break;
        case SYS_RECORDING_AND_UPLOADING:
            handle_recording_and_uploading_state(sys, event, data);
            break;
        case SYS_UPLOADING:
            handle_uploading_state(sys, event, data);
            break;
        case SYS_OTA:
            handle_ota_state(sys, event, data);
            break;
        case SYS_ERROR:
            handle_error_state(sys, event, data);
            break;
        case SYS_SLEEP:
            handle_sleep_state(sys, event, data);
            break;
        case SYS_SHUTDOWN:
            handle_shutdown_state(sys, event, data);
            break;
        case SYS_RESET:
            handle_reset_state(sys, event, data);
            break;
    }
}

/* State transition helper */
static void transition_to_state(struct system_coordinator *sys, enum system_state new_state) {
    if (sys->current_state == new_state) {
        return; /* No change */
    }
    
    /* Exit current state */
    if (sys_ops.exit_state) {
        sys_ops.exit_state(sys);
    }
    
    /* Update state */
    sys->previous_state = sys->current_state;
    sys->current_state = new_state;
    
    printf("System transition: %s -> %s\n", 
          state_names[sys->previous_state], state_names[sys->current_state]);
    
    /* Enter new state */
    switch (new_state) {
        case SYS_INIT:
            if (sys_ops.enter_init) sys_ops.enter_init(sys);
            break;
        case SYS_IDLE:
            if (sys_ops.enter_idle) sys_ops.enter_idle(sys);
            break;
        case SYS_RECORDING:
            if (sys_ops.enter_recording) sys_ops.enter_recording(sys);
            break;
        case SYS_RECORDING_AND_UPLOADING:
            if (sys_ops.enter_recording_and_uploading) sys_ops.enter_recording_and_uploading(sys);
            break;
        case SYS_UPLOADING:
            if (sys_ops.enter_uploading) sys_ops.enter_uploading(sys);
            break;
        case SYS_OTA:
            if (sys_ops.enter_ota) sys_ops.enter_ota(sys);
            break;
        case SYS_ERROR:
            if (sys_ops.enter_error) sys_ops.enter_error(sys);
            break;
        case SYS_SLEEP:
            if (sys_ops.enter_sleep) sys_ops.enter_sleep(sys);
            break;
        case SYS_SHUTDOWN:
            /* Shutdown handling */
            break;
        case SYS_RESET:
            if (sys_ops.enter_reset) sys_ops.enter_reset(sys);
            break;
    }
}

/* Initialize operations */
static void init_operations(void) {
    static int initialized = 0;
    if (initialized) return;
    
    sys_ops.init = init;
    sys_ops.destroy = destroy;
    sys_ops.handle_event = handle_event;
    sys_ops.enter_init = enter_init;
    sys_ops.enter_idle = enter_idle;
    sys_ops.enter_recording = enter_recording;
    sys_ops.enter_recording_and_uploading = enter_recording_and_uploading;
    sys_ops.enter_uploading = enter_uploading;
    sys_ops.enter_ota = enter_ota;
    sys_ops.enter_error = enter_error;
    sys_ops.enter_sleep = enter_sleep;
    sys_ops.enter_reset = enter_reset;
    sys_ops.exit_state = exit_state;
    sys_ops.do_action = do_action;
    
    initialized = 1;
}

/* Public API implementation */
void system_coordinator_init(struct system_coordinator *sys) {
    if (!sys) return;
    
    memset(sys, 0, sizeof(struct system_coordinator));
    init_operations();
    
    sys->current_state = SYS_INIT;
    sys->previous_state = SYS_INIT;
    
    /* Enter INIT state which will transition to IDLE */
    enter_init(sys);
}

void system_coordinator_destroy(struct system_coordinator *sys) {
    if (!sys) return;
    
    /* Clean up subsystems */
    /* Note: Subsystems should be destroyed by their owners */
    
    memset(sys, 0, sizeof(struct system_coordinator));
}

void system_coordinator_dispatch_event(struct system_coordinator *sys, enum system_event event, void *data) {
    if (!sys) return;
    
    if (sys_ops.handle_event) {
        sys_ops.handle_event(sys, event, data);
    }
}

enum system_state system_coordinator_get_state(struct system_coordinator *sys) {
    return sys ? sys->current_state : SYS_ERROR;
}

const char *system_coordinator_get_state_name(enum system_state state) {
    if (state >= 0 && state < (int)(sizeof(state_names)/sizeof(state_names[0]))) {
        return state_names[state];
    }
    return "UNKNOWN_STATE";
}

const char *system_coordinator_get_event_name(enum system_event event) {
    if (event >= 0 && event < (int)(sizeof(event_names)/sizeof(event_names[0]))) {
        return event_names[event];
    }
    return "UNKNOWN_EVENT";
}

void system_coordinator_set_error(struct system_coordinator *sys, int code, const char *msg) {
    if (!sys) return;
    
    sys->error_code = code;
    if (msg) {
        strncpy(sys->error_msg, msg, sizeof(sys->error_msg) - 1);
        sys->error_msg[sizeof(sys->error_msg) - 1] = '\0';
    } else {
        sys->error_msg[0] = '\0';
    }
}

int system_coordinator_get_error_code(struct system_coordinator *sys) {
    return sys ? sys->error_code : -1;
}

const char *system_coordinator_get_error_msg(struct system_coordinator *sys) {
    return sys ? sys->error_msg : "Invalid system coordinator";
}

void system_coordinator_set_recording_fsm(struct system_coordinator *sys, struct recording_fsm *fsm) {
    if (sys) sys->recording_fsm = fsm;
}

void system_coordinator_set_comm_fsm(struct system_coordinator *sys, struct comm_fsm *fsm) {
    if (sys) sys->comm_fsm = fsm;
}

void system_coordinator_set_power_fsm(struct system_coordinator *sys, struct power_fsm *fsm) {
    if (sys) sys->power_fsm = fsm;
}

void system_coordinator_set_audio_fsm(struct system_coordinator *sys, struct audio_fsm *fsm) {
    if (sys) sys->audio_fsm = fsm;
}

void system_coordinator_set_efsm_protocol(struct system_coordinator *sys, struct efsm_protocol *efsm) {
    if (sys) sys->efsm_protocol = efsm;
}

void system_coordinator_set_ota_fsm(struct system_coordinator *sys, struct ota_fsm *fsm) {
    if (sys) sys->ota_fsm = fsm;
}
