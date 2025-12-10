/**
 * external_command_config.c  2025-12-09
 * 
 * Implementation of external command configuration and processing.
 */

#include "external_command_config.h"
#include <string.h>
#include <time.h>

/* Default configuration */
static const struct external_command_config DEFAULT_CONFIG = {
    .mode = CMD_MODE_CALLBACK,
    .protocol = CMD_PROTOCOL_BINARY,
    .callback = NULL,
    .callback_user_data = NULL,
    .queue_size = 10,
    .queue_timeout_ms = 1000,
    .queue_callback = NULL,
    .queue_user_data = NULL,
    .event_target = NULL,
    .hybrid_threshold = CMD_PRIORITY_HIGH,
    .max_processing_time_ms = 500,
    .retry_count = 3,
    .commands_received = 0,
    .commands_processed = 0,
    .commands_failed = 0,
    .last_error_code = 0,
    .last_error_msg = ""
};

/* Mode names */
static const char *MODE_NAMES[] = {
    "Callback",
    "Message Queue",
    "Direct Event",
    "Hybrid",
    "Disabled"
};

/* Protocol names */
static const char *PROTOCOL_NAMES[] = {
    "Binary",
    "JSON",
    "Custom"
};

/* Command code names */
static const char *CODE_NAMES[] = {
    "NONE",
    "REC_START",
    "REC_STOP",
    "REC_PAUSE",
    "REC_RESUME",
    "UPLOAD_START",
    "UPLOAD_STOP",
    "POWER_ON",
    "POWER_OFF",
    "RESET",
    "GET_STATUS",
    "SET_VOLUME",
    "SET_MODE",
    "STREAMING_START",
    "STREAMING_STOP",
    "CUSTOM"
};

/* Default priorities for commands */
static enum command_priority DEFAULT_PRIORITIES[] = {
    CMD_PRIORITY_LOW,      /* NONE */
    CMD_PRIORITY_HIGH,     /* REC_START */
    CMD_PRIORITY_HIGH,     /* REC_STOP */
    CMD_PRIORITY_NORMAL,   /* REC_PAUSE */
    CMD_PRIORITY_NORMAL,   /* REC_RESUME */
    CMD_PRIORITY_NORMAL,   /* UPLOAD_START */
    CMD_PRIORITY_NORMAL,   /* UPLOAD_STOP */
    CMD_PRIORITY_CRITICAL, /* POWER_ON */
    CMD_PRIORITY_CRITICAL, /* POWER_OFF */
    CMD_PRIORITY_CRITICAL, /* RESET */
    CMD_PRIORITY_LOW,      /* GET_STATUS */
    CMD_PRIORITY_NORMAL,   /* SET_VOLUME */
    CMD_PRIORITY_NORMAL,   /* SET_MODE */
    CMD_PRIORITY_HIGH,     /* STREAMING_START */
    CMD_PRIORITY_HIGH,     /* STREAMING_STOP */
    CMD_PRIORITY_NORMAL    /* CUSTOM */
};

void external_command_config_init(struct external_command_config *config)
{
    if (!config) return;
    
    /* Copy default configuration */
    *config = DEFAULT_CONFIG;
}

void external_command_config_set_mode(struct external_command_config *config, 
                                      enum command_processing_mode mode)
{
    if (!config) return;
    
    /* Validate mode */
    if (mode >= CMD_MODE_DISABLED) {
        config->last_error_code = 1;
        strncpy(config->last_error_msg, "Invalid mode", sizeof(config->last_error_msg)-1);
        return;
    }
    
    config->mode = mode;
}

void external_command_config_set_protocol(struct external_command_config *config,
                                          enum command_protocol_type protocol)
{
    if (!config) return;
    
    if (protocol >= CMD_PROTOCOL_CUSTOM) {
        config->last_error_code = 2;
        strncpy(config->last_error_msg, "Invalid protocol", sizeof(config->last_error_msg)-1);
        return;
    }
    
    config->protocol = protocol;
}

void external_command_config_set_callback(struct external_command_config *config,
                                          command_callback_t callback,
                                          void *user_data)
{
    if (!config) return;
    
    config->callback = callback;
    config->callback_user_data = user_data;
}

void external_command_config_set_queue(struct external_command_config *config,
                                       int queue_size,
                                       int timeout_ms,
                                       message_queue_callback_t callback,
                                       void *user_data)
{
    if (!config) return;
    
    if (queue_size <= 0) {
        config->last_error_code = 3;
        strncpy(config->last_error_msg, "Invalid queue size", sizeof(config->last_error_msg)-1);
        return;
    }
    
    config->queue_size = queue_size;
    config->queue_timeout_ms = timeout_ms;
    config->queue_callback = callback;
    config->queue_user_data = user_data;
}

void external_command_config_set_event_target(struct external_command_config *config,
                                              void *target)
{
    if (!config) return;
    
    config->event_target = target;
}

int external_command_config_switch_mode(struct external_command_config *config,
                                        enum command_processing_mode new_mode)
{
    if (!config) return 0;
    
    /* Check if mode is supported */
    if (!external_command_config_is_mode_supported(new_mode)) {
        config->last_error_code = 4;
        strncpy(config->last_error_msg, "Mode not supported", sizeof(config->last_error_msg)-1);
        return 0;
    }
    
    /* Special handling for mode switching */
    if (config->mode == CMD_MODE_MESSAGE_QUEUE && new_mode != CMD_MODE_MESSAGE_QUEUE) {
        /* Would need to flush queue before switching */
        /* For now, just log */
    }
    
    config->mode = new_mode;
    return 1;
}

int external_command_config_is_mode_supported(enum command_processing_mode mode)
{
    /* All modes are supported in this implementation */
    return (mode >= CMD_MODE_CALLBACK && mode <= CMD_MODE_DISABLED);
}

int external_command_parse_binary(const uint8_t *data, size_t size, 
                                  struct command_message *msg)
{
    if (!data || !msg || size < sizeof(struct command_packet)) {
        return 0;
    }
    
    const struct command_packet *packet = (const struct command_packet *)data;
    
    /* Validate packet */
    if (!external_command_validate_packet(packet)) {
        return 0;
    }
    
    /* Fill message */
    msg->cmd = (enum command_code)packet->command_code;
    msg->priority = external_command_get_default_priority(msg->cmd);
    msg->timestamp = (uint32_t)time(NULL);
    msg->data_length = packet->data_length;
    if (packet->data_length > 0 && packet->data_length <= sizeof(msg->data)) {
        memcpy(msg->data, packet->data, packet->data_length);
    }
    msg->source = NULL;
    msg->sequence_number = 0; /* Would be assigned by queue manager */
    
    return 1;
}

int external_command_parse_json(const char *json_str, struct command_message *msg)
{
    /* Simplified JSON parsing - in real implementation would use a JSON parser */
    if (!json_str || !msg) return 0;
    
    /* For demo purposes, parse simple format: {"cmd":"rec_start","priority":2} */
    if (strstr(json_str, "\"cmd\":\"rec_start\"")) {
        msg->cmd = CMD_REC_START;
    } else if (strstr(json_str, "\"cmd\":\"rec_stop\"")) {
        msg->cmd = CMD_REC_STOP;
    } else if (strstr(json_str, "\"cmd\":\"reset\"")) {
        msg->cmd = CMD_RESET;
    } else {
        msg->cmd = CMD_NONE;
        return 0;
    }
    
    msg->priority = CMD_PRIORITY_NORMAL;
    msg->timestamp = (uint32_t)time(NULL);
    msg->data_length = 0;
    msg->source = NULL;
    msg->sequence_number = 0;
    
    return 1;
}

int external_command_validate_packet(const struct command_packet *packet)
{
    if (!packet) return 0;
    
    /* Check start marker */
    if (packet->start_marker != 0xAA) {
        return 0;
    }
    
    /* Check command code validity */
    if (packet->command_code == CMD_NONE || 
        (packet->command_code > CMD_SET_MODE && packet->command_code != CMD_CUSTOM)) {
        return 0;
    }
    
    /* Check data length */
    if (packet->data_length > 255) {
        return 0;
    }
    
    /* Simple checksum validation */
    uint8_t checksum = 0;
    const uint8_t *bytes = (const uint8_t *)packet;
    for (size_t i = 0; i < sizeof(struct command_packet) - 1; i++) {
        checksum ^= bytes[i];
    }
    
    if (checksum != packet->checksum) {
        return 0;
    }
    
    return 1;
}

const char *external_command_get_mode_name(enum command_processing_mode mode)
{
    if (mode >= CMD_MODE_CALLBACK && mode <= CMD_MODE_DISABLED) {
        return MODE_NAMES[mode];
    }
    return "Unknown";
}

const char *external_command_get_protocol_name(enum command_protocol_type protocol)
{
    if (protocol >= CMD_PROTOCOL_BINARY && protocol <= CMD_PROTOCOL_CUSTOM) {
        return PROTOCOL_NAMES[protocol];
    }
    return "Unknown";
}

const char *external_command_get_code_name(enum command_code code)
{
    if (code >= CMD_NONE && code <= CMD_SET_MODE) {
        return CODE_NAMES[code];
    } else if (code == CMD_CUSTOM) {
        return CODE_NAMES[13]; /* "CUSTOM" */
    }
    return "Unknown";
}

enum command_priority external_command_get_default_priority(enum command_code code)
{
    if (code >= CMD_NONE && code <= CMD_SET_MODE) {
        return DEFAULT_PRIORITIES[code];
    }
    return CMD_PRIORITY_NORMAL;
}
