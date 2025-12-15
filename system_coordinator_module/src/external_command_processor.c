/**
 * external_command_processor.c  2025-12-09
 * 
 * Implementation of external command processor with configurable modes.
 */

#include "external_command_processor.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Private helper functions */
static int process_callback_mode(struct external_command_processor *proc,
                                 struct command_message *msg);
static int process_queue_mode(struct external_command_processor *proc,
                              struct command_message *msg);
static int process_direct_event_mode(struct external_command_processor *proc,
                                     struct command_message *msg);
static int process_hybrid_mode(struct external_command_processor *proc,
                               struct command_message *msg);

static int enqueue_message(struct external_command_processor *proc,
                           struct command_message *msg);
static int dequeue_message(struct external_command_processor *proc,
                           struct command_message *msg);
static void process_queued_messages(struct external_command_processor *proc);

static enum system_event command_to_system_event(enum command_code cmd);

/* Default operations */
static void default_init(struct external_command_processor *proc);
static void default_destroy(struct external_command_processor *proc);
static int default_process_data(struct external_command_processor *proc,
                                const void *data, size_t size, void *source);
static int default_process_message(struct external_command_processor *proc,
                                   struct command_message *msg);
static int default_switch_mode(struct external_command_processor *proc,
                               enum command_processing_mode new_mode);
static const struct external_command_config* default_get_config(struct external_command_processor *proc);
static int default_update_config(struct external_command_processor *proc,
                                 const struct external_command_config *config);
static void default_get_stats(struct external_command_processor *proc,
                              uint32_t *received, uint32_t *processed, uint32_t *failed);
static void default_reset_stats(struct external_command_processor *proc);

static const struct external_command_processor_ops DEFAULT_OPS = {
    .init = default_init,
    .destroy = default_destroy,
    .process_data = default_process_data,
    .process_message = default_process_message,
    .switch_mode = default_switch_mode,
    .get_config = default_get_config,
    .update_config = default_update_config,
    .get_stats = default_get_stats,
    .reset_stats = default_reset_stats
};

/* API implementation */
struct external_command_processor* external_command_processor_create(void)
{
    struct external_command_processor *proc = 
        (struct external_command_processor*)malloc(sizeof(struct external_command_processor));
    if (!proc) return NULL;
    
    /* Initialize configuration */
    external_command_config_init(&proc->config);
    
    /* Initialize queue */
    proc->queue = NULL;
    proc->queue_head = 0;
    proc->queue_tail = 0;
    proc->queue_count = 0;
    
    /* Allocate queue memory */
    if (proc->config.queue_size > 0) {
        proc->queue = (struct command_message*)malloc(
            sizeof(struct command_message) * proc->config.queue_size);
        if (!proc->queue) {
            free(proc);
            return NULL;
        }
    }
    
    /* Initialize other fields */
    proc->coordinator = NULL;
    proc->is_running = 0;
    proc->thread_id = 0;
    proc->ops = &DEFAULT_OPS;
    proc->priv_data = NULL;
    
    return proc;
}

void external_command_processor_destroy(struct external_command_processor *proc)
{
    if (!proc) return;
    
    /* Free queue */
    if (proc->queue) {
        free(proc->queue);
    }
    
    /* Free private data */
    if (proc->priv_data) {
        free(proc->priv_data);
    }
    
    free(proc);
}

int external_command_processor_process(struct external_command_processor *proc,
                                       const void *data, size_t size,
                                       void *source)
{
    if (!proc || !data || size == 0) return 0;
    
    /* Update statistics */
    proc->config.commands_received++;
    
    /* Parse based on protocol */
    struct command_message msg;
    int parse_result = 0;
    
    switch (proc->config.protocol) {
        case CMD_PROTOCOL_BINARY:
            parse_result = external_command_parse_binary((const uint8_t*)data, size, &msg);
            break;
        case CMD_PROTOCOL_JSON:
            parse_result = external_command_parse_json((const char*)data, &msg);
            break;
        case CMD_PROTOCOL_CUSTOM:
            /* Custom parsing would be implemented by user */
            parse_result = 0;
            break;
        default:
            parse_result = 0;
    }
    
    if (!parse_result) {
        proc->config.commands_failed++;
        return 0;
    }
    
    /* Set source */
    msg.source = source;
    
    /* Process based on mode */
    int result = 0;
    switch (proc->config.mode) {
        case CMD_MODE_CALLBACK:
            result = process_callback_mode(proc, &msg);
            break;
        case CMD_MODE_MESSAGE_QUEUE:
            result = process_queue_mode(proc, &msg);
            break;
        case CMD_MODE_DIRECT_EVENT:
            result = process_direct_event_mode(proc, &msg);
            break;
        case CMD_MODE_HYBRID:
            result = process_hybrid_mode(proc, &msg);
            break;
        case CMD_MODE_DISABLED:
            /* Do nothing */
            result = 1;
            break;
        default:
            result = 0;
    }
    
    if (result) {
        proc->config.commands_processed++;
    } else {
        proc->config.commands_failed++;
    }
    
    return result;
}

int external_command_processor_process_raw(struct external_command_processor *proc,
                                           const uint8_t *data, size_t size,
                                           void *source)
{
    return external_command_processor_process(proc, data, size, source);
}

int external_command_processor_set_mode(struct external_command_processor *proc,
                                        enum command_processing_mode mode)
{
    if (!proc) return 0;
    
    /* Flush queue if switching from queue mode */
    if (proc->config.mode == CMD_MODE_MESSAGE_QUEUE && mode != CMD_MODE_MESSAGE_QUEUE) {
        external_command_processor_flush_queue(proc);
    }
    
    return external_command_config_switch_mode(&proc->config, mode);
}

enum command_processing_mode external_command_processor_get_mode(struct external_command_processor *proc)
{
    if (!proc) return CMD_MODE_DISABLED;
    return proc->config.mode;
}

int external_command_processor_configure(struct external_command_processor *proc,
                                         const struct external_command_config *config)
{
    if (!proc || !config) return 0;
    
    /* Update configuration */
    proc->config = *config;
    
    /* Reallocate queue if size changed */
    if (proc->queue && proc->config.queue_size > 0) {
        free(proc->queue);
        proc->queue = (struct command_message*)malloc(
            sizeof(struct command_message) * proc->config.queue_size);
        if (!proc->queue) return 0;
    }
    
    proc->queue_head = 0;
    proc->queue_tail = 0;
    proc->queue_count = 0;
    
    return 1;
}

const struct external_command_config* external_command_processor_get_configuration(struct external_command_processor *proc)
{
    if (!proc) return NULL;
    return &proc->config;
}

void external_command_processor_set_coordinator(struct external_command_processor *proc,
                                                struct system_coordinator *coord)
{
    if (!proc) return;
    proc->coordinator = coord;
}

struct system_coordinator* external_command_processor_get_coordinator(struct external_command_processor *proc)
{
    if (!proc) return NULL;
    return proc->coordinator;
}

int external_command_processor_queue_size(struct external_command_processor *proc)
{
    if (!proc) return 0;
    return proc->queue_count;
}

int external_command_processor_queue_capacity(struct external_command_processor *proc)
{
    if (!proc) return 0;
    return proc->config.queue_size;
}

int external_command_processor_flush_queue(struct external_command_processor *proc)
{
    if (!proc) return 0;
    
    /* Process all queued messages */
    process_queued_messages(proc);
    
    /* Reset queue */
    proc->queue_head = 0;
    proc->queue_tail = 0;
    proc->queue_count = 0;
    
    return 1;
}

void external_command_processor_get_statistics(struct external_command_processor *proc,
                                               uint32_t *received, uint32_t *processed,
                                               uint32_t *failed, int *queue_usage)
{
    if (!proc) return;
    
    if (received) *received = proc->config.commands_received;
    if (processed) *processed = proc->config.commands_processed;
    if (failed) *failed = proc->config.commands_failed;
    if (queue_usage) *queue_usage = proc->queue_count;
}

void external_command_processor_reset_statistics(struct external_command_processor *proc)
{
    if (!proc) return;
    
    proc->config.commands_received = 0;
    proc->config.commands_processed = 0;
    proc->config.commands_failed = 0;
}

const char* external_command_processor_get_status(struct external_command_processor *proc)
{
    if (!proc) return "Invalid processor";
    
    static char status[256];
    snprintf(status, sizeof(status),
             "Mode: %s, Protocol: %s, Received: %u, Processed: %u, Failed: %u, Queue: %d/%d",
             external_command_get_mode_name(proc->config.mode),
             external_command_get_protocol_name(proc->config.protocol),
             proc->config.commands_received,
             proc->config.commands_processed,
             proc->config.commands_failed,
             proc->queue_count,
             proc->config.queue_size);
    
    return status;
}

int external_command_processor_is_running(struct external_command_processor *proc)
{
    if (!proc) return 0;
    return proc->is_running;
}

void external_command_processor_start(struct external_command_processor *proc)
{
    if (!proc) return;
    proc->is_running = 1;
}

void external_command_processor_stop(struct external_command_processor *proc)
{
    if (!proc) return;
    proc->is_running = 0;
}

/* Private helper functions */
static int process_callback_mode(struct external_command_processor *proc,
                                 struct command_message *msg)
{
    if (!proc || !msg) return 0;
    
    /* Call user callback if set */
    if (proc->config.callback) {
        proc->config.callback(msg->cmd, msg->data, msg->data_length,
                              proc->config.callback_user_data);
        return 1;
    }
    
    return 0;
}

static int process_queue_mode(struct external_command_processor *proc,
                              struct command_message *msg)
{
    if (!proc || !msg) return 0;
    
    /* Enqueue message */
    if (!enqueue_message(proc, msg)) {
        return 0;
    }
    
    /* Process queued messages if we have a queue callback */
    if (proc->config.queue_callback) {
        process_queued_messages(proc);
    }
    
    return 1;
}

static int process_direct_event_mode(struct external_command_processor *proc,
                                     struct command_message *msg)
{
    if (!proc || !msg || !proc->coordinator) return 0;
    
    /* Convert command to system event */
    enum system_event event = command_to_system_event(msg->cmd);
    if (event == SYS_EVT_RESET && msg->cmd != CMD_RESET) {
        /* Unknown command */
        return 0;
    }
    
    /* Dispatch event to system coordinator */
    system_coordinator_dispatch_event(proc->coordinator, event, msg->data);
    return 1;
}

static int process_hybrid_mode(struct external_command_processor *proc,
                               struct command_message *msg)
{
    if (!proc || !msg) return 0;
    
    /* Check priority threshold */
    if (msg->priority >= proc->config.hybrid_threshold) {
        /* High priority: use callback */
        return process_callback_mode(proc, msg);
    } else {
        /* Low priority: use queue */
        return process_queue_mode(proc, msg);
    }
}

static int enqueue_message(struct external_command_processor *proc,
                           struct command_message *msg)
{
    if (!proc || !msg || !proc->queue) return 0;
    
    /* Check if queue is full */
    if (proc->queue_count >= proc->config.queue_size) {
        return 0;
    }
    
    /* Copy message to queue */
    memcpy(&proc->queue[proc->queue_tail], msg, sizeof(struct command_message));
    
    /* Update tail and count */
    proc->queue_tail = (proc->queue_tail + 1) % proc->config.queue_size;
    proc->queue_count++;
    
    return 1;
}

static int dequeue_message(struct external_command_processor *proc,
                           struct command_message *msg)
{
    if (!proc || !msg || !proc->queue || proc->queue_count == 0) return 0;
    
    /* Copy message from queue */
    memcpy(msg, &proc->queue[proc->queue_head], sizeof(struct command_message));
    
    /* Update head and count */
    proc->queue_head = (proc->queue_head + 1) % proc->config.queue_size;
    proc->queue_count--;
    
    return 1;
}

static void process_queued_messages(struct external_command_processor *proc)
{
    if (!proc || !proc->config.queue_callback) return;
    
    struct command_message msg;
    while (dequeue_message(proc, &msg)) {
        proc->config.queue_callback(&msg, proc->config.queue_user_data);
    }
}

static enum system_event command_to_system_event(enum command_code cmd)
{
    switch (cmd) {
        case CMD_REC_START:
            return SYS_EVT_REC_START;
        case CMD_REC_STOP:
            return SYS_EVT_REC_STOP;
        case CMD_REC_PAUSE:
            return SYS_EVT_REC_PAUSE;
        case CMD_REC_RESUME:
            return SYS_EVT_REC_RESUME;
        case CMD_UPLOAD_START:
            return SYS_EVT_UPLOAD_START;
        case CMD_UPLOAD_STOP:
            return SYS_EVT_UPLOAD_COMPLETE;
        case CMD_POWER_ON:
            return SYS_EVT_POWER_ON;
        case CMD_POWER_OFF:
            return SYS_EVT_POWER_OFF;
        case CMD_RESET:
            return SYS_EVT_RESET;
        case CMD_GET_STATUS:
            /* No direct system event for status query */
            break;
        case CMD_SET_VOLUME:
        case CMD_SET_MODE:
            /* Custom handling needed */
            break;
        case CMD_STREAMING_START:
            return SYS_EVT_STREAMING_START;
        case CMD_STREAMING_STOP:
            return SYS_EVT_STREAMING_STOP;
        case CMD_CUSTOM:
            /* Custom handling needed */
            break;
        default:
            break;
    }
    
    return SYS_EVT_RESET; /* Default */
}

/* Default operations implementation */
static void default_init(struct external_command_processor *proc)
{
    /* Nothing to do */
}

static void default_destroy(struct external_command_processor *proc)
{
    /* Nothing to do */
}

static int default_process_data(struct external_command_processor *proc,
                                const void *data, size_t size, void *source)
{
    return external_command_processor_process(proc, data, size, source);
}

static int default_process_message(struct external_command_processor *proc,
                                   struct command_message *msg)
{
    if (!proc || !msg) return 0;
    
    /* Process based on mode */
    switch (proc->config.mode) {
        case CMD_MODE_CALLBACK:
            return process_callback_mode(proc, msg);
        case CMD_MODE_MESSAGE_QUEUE:
            return process_queue_mode(proc, msg);
        case CMD_MODE_DIRECT_EVENT:
            return process_direct_event_mode(proc, msg);
        case CMD_MODE_HYBRID:
            return process_hybrid_mode(proc, msg);
        default:
            return 0;
    }
}

static int default_switch_mode(struct external_command_processor *proc,
                               enum command_processing_mode new_mode)
{
    return external_command_processor_set_mode(proc, new_mode);
}

static const struct external_command_config* default_get_config(struct external_command_processor *proc)
{
    if (!proc) return NULL;
    return &proc->config;
}

static int default_update_config(struct external_command_processor *proc,
                                 const struct external_command_config *config)
{
    return external_command_processor_configure(proc, config);
}

static void default_get_stats(struct external_command_processor *proc,
                              uint32_t *received, uint32_t *processed, uint32_t *failed)
{
    if (!proc) return;
    
    if (received) *received = proc->config.commands_received;
    if (processed) *processed = proc->config.commands_processed;
    if (failed) *failed = proc->config.commands_failed;
}

static void default_reset_stats(struct external_command_processor *proc)
{
    external_command_processor_reset_statistics(proc);
}
