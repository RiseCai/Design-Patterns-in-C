/**
 * external_command_config.h  2025-12-09
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Configuration for external command processing in TWS Earphone System.
 * Supports multiple processing modes (callback, message queue) with runtime switching.
 */

#ifndef __EXTERNAL_COMMAND_CONFIG_H__
#define __EXTERNAL_COMMAND_CONFIG_H__

#include "mycommon.h"
#include <stdint.h>
#include <stddef.h>

/* Command processing modes */
enum command_processing_mode {
    CMD_MODE_CALLBACK = 0,      /* Direct callback (Scheme 1) */
    CMD_MODE_MESSAGE_QUEUE,     /* Message queue (Scheme 2) */
    CMD_MODE_DIRECT_EVENT,      /* Direct event forwarding (Scheme 3) */
    CMD_MODE_HYBRID,            /* Hybrid mode: callback + queue */
    CMD_MODE_DISABLED           /* Command processing disabled */
};

/* Command protocol types */
enum command_protocol_type {
    CMD_PROTOCOL_BINARY = 0,    /* Simple binary protocol */
    CMD_PROTOCOL_JSON,          /* JSON protocol */
    CMD_PROTOCOL_CUSTOM         /* Custom protocol */
};

/* Command priority levels */
enum command_priority {
    CMD_PRIORITY_LOW = 0,
    CMD_PRIORITY_NORMAL,
    CMD_PRIORITY_HIGH,
    CMD_PRIORITY_CRITICAL
};

/* Command codes (matching system events) */
enum command_code {
    CMD_NONE = 0,
    CMD_REC_START = 0x01,
    CMD_REC_STOP = 0x02,
    CMD_REC_PAUSE = 0x03,
    CMD_REC_RESUME = 0x04,
    CMD_UPLOAD_START = 0x05,
    CMD_UPLOAD_STOP = 0x06,
    CMD_POWER_ON = 0x07,
    CMD_POWER_OFF = 0x08,
    CMD_RESET = 0x09,
    CMD_GET_STATUS = 0x0A,
    CMD_SET_VOLUME = 0x0B,
    CMD_SET_MODE = 0x0C,
    CMD_STREAMING_START = 0x0D,
    CMD_STREAMING_STOP = 0x0E,
    CMD_CUSTOM = 0xFF          /* Custom command */
};

/* Command packet structure (binary protocol) */
#pragma pack(push, 1)
struct command_packet {
    uint8_t start_marker;      /* 0xAA */
    uint8_t command_code;      /* enum command_code */
    uint8_t data_length;       /* 0-255 bytes */
    uint8_t data[255];         /* Command data */
    uint8_t checksum;          /* Simple checksum */
};
#pragma pack(pop)

/* Command message for queue */
struct command_message {
    enum command_code cmd;
    enum command_priority priority;
    uint32_t timestamp;
    uint16_t data_length;
    uint8_t data[256];
    void *source;              /* Source device/connection */
    uint32_t sequence_number;
};

/* Command callback function type */
typedef void (*command_callback_t)(enum command_code cmd, void *data, size_t size, void *user_data);

/* Message queue callback function type */
typedef void (*message_queue_callback_t)(struct command_message *msg, void *user_data);

/* Configuration structure */
struct external_command_config {
    /* Processing mode */
    enum command_processing_mode mode;
    
    /* Protocol settings */
    enum command_protocol_type protocol;
    
    /* Callback configuration */
    command_callback_t callback;
    void *callback_user_data;
    
    /* Message queue configuration */
    int queue_size;                    /* Max messages in queue */
    int queue_timeout_ms;              /* Timeout for queue operations */
    message_queue_callback_t queue_callback;
    void *queue_user_data;
    
    /* Direct event configuration */
    void *event_target;                /* Target for direct events (e.g., system_coordinator) */
    
    /* Hybrid mode configuration */
    int hybrid_threshold;              /* Messages above this priority use callback */
    
    /* Performance settings */
    int max_processing_time_ms;        /* Max time to process a command */
    int retry_count;                   /* Number of retries on failure */
    
    /* Statistics */
    uint32_t commands_received;
    uint32_t commands_processed;
    uint32_t commands_failed;
    
    /* Error handling */
    int last_error_code;
    char last_error_msg[128];
};

/* Configuration API */
void external_command_config_init(struct external_command_config *config);
void external_command_config_set_mode(struct external_command_config *config, 
                                      enum command_processing_mode mode);
void external_command_config_set_protocol(struct external_command_config *config,
                                          enum command_protocol_type protocol);
void external_command_config_set_callback(struct external_command_config *config,
                                          command_callback_t callback,
                                          void *user_data);
void external_command_config_set_queue(struct external_command_config *config,
                                       int queue_size,
                                       int timeout_ms,
                                       message_queue_callback_t callback,
                                       void *user_data);
void external_command_config_set_event_target(struct external_command_config *config,
                                              void *target);

/* Mode switching API */
int external_command_config_switch_mode(struct external_command_config *config,
                                        enum command_processing_mode new_mode);
int external_command_config_is_mode_supported(enum command_processing_mode mode);

/* Protocol parsing API */
int external_command_parse_binary(const uint8_t *data, size_t size, 
                                  struct command_message *msg);
int external_command_parse_json(const char *json_str, struct command_message *msg);
int external_command_validate_packet(const struct command_packet *packet);

/* Utility functions */
const char *external_command_get_mode_name(enum command_processing_mode mode);
const char *external_command_get_protocol_name(enum command_protocol_type protocol);
const char *external_command_get_code_name(enum command_code code);
enum command_priority external_command_get_default_priority(enum command_code code);

#endif /* __EXTERNAL_COMMAND_CONFIG_H__ */
