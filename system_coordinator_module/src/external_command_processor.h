/**
 * external_command_processor.h  2025-12-09
 * 
 * External command processor with configurable processing modes.
 * Supports switching between callback, message queue, and direct event modes.
 */

#ifndef __EXTERNAL_COMMAND_PROCESSOR_H__
#define __EXTERNAL_COMMAND_PROCESSOR_H__

#include "external_command_config.h"
#include "system_coordinator.h"

/* Forward declarations */
struct external_command_processor;

/* Processor operations */
struct external_command_processor_ops {
    /* Initialize processor */
    void (*init)(struct external_command_processor *proc);
    
    /* Destroy processor */
    void (*destroy)(struct external_command_processor *proc);
    
    /* Process incoming command data */
    int (*process_data)(struct external_command_processor *proc, 
                        const void *data, size_t size, void *source);
    
    /* Process command message */
    int (*process_message)(struct external_command_processor *proc,
                           struct command_message *msg);
    
    /* Switch processing mode */
    int (*switch_mode)(struct external_command_processor *proc,
                       enum command_processing_mode new_mode);
    
    /* Get current configuration */
    const struct external_command_config* (*get_config)(struct external_command_processor *proc);
    
    /* Update configuration */
    int (*update_config)(struct external_command_processor *proc,
                         const struct external_command_config *config);
    
    /* Get statistics */
    void (*get_stats)(struct external_command_processor *proc,
                      uint32_t *received, uint32_t *processed, uint32_t *failed);
    
    /* Reset statistics */
    void (*reset_stats)(struct external_command_processor *proc);
};

/* Command processor structure */
struct external_command_processor {
    /* Configuration */
    struct external_command_config config;
    
    /* System coordinator reference */
    struct system_coordinator *coordinator;
    
    /* Message queue (simplified implementation) */
    struct command_message *queue;
    int queue_head;
    int queue_tail;
    int queue_count;
    
    /* Processing thread control */
    int is_running;
    int thread_id;
    
    /* Operations */
    const struct external_command_processor_ops *ops;
    
    /* Private data */
    void *priv_data;
};

/* API functions */
struct external_command_processor* external_command_processor_create(void);
void external_command_processor_destroy(struct external_command_processor *proc);

/* Main processing functions */
int external_command_processor_process(struct external_command_processor *proc,
                                       const void *data, size_t size,
                                       void *source);
int external_command_processor_process_raw(struct external_command_processor *proc,
                                           const uint8_t *data, size_t size,
                                           void *source);

/* Mode management */
int external_command_processor_set_mode(struct external_command_processor *proc,
                                        enum command_processing_mode mode);
enum command_processing_mode external_command_processor_get_mode(struct external_command_processor *proc);

/* Configuration */
int external_command_processor_configure(struct external_command_processor *proc,
                                         const struct external_command_config *config);
const struct external_command_config* external_command_processor_get_configuration(struct external_command_processor *proc);

/* System coordinator integration */
void external_command_processor_set_coordinator(struct external_command_processor *proc,
                                                struct system_coordinator *coord);
struct system_coordinator* external_command_processor_get_coordinator(struct external_command_processor *proc);

/* Queue management */
int external_command_processor_queue_size(struct external_command_processor *proc);
int external_command_processor_queue_capacity(struct external_command_processor *proc);
int external_command_processor_flush_queue(struct external_command_processor *proc);

/* Statistics */
void external_command_processor_get_statistics(struct external_command_processor *proc,
                                               uint32_t *received, uint32_t *processed,
                                               uint32_t *failed, int *queue_usage);
void external_command_processor_reset_statistics(struct external_command_processor *proc);

/* Utility functions */
const char* external_command_processor_get_status(struct external_command_processor *proc);
int external_command_processor_is_running(struct external_command_processor *proc);
void external_command_processor_start(struct external_command_processor *proc);
void external_command_processor_stop(struct external_command_processor *proc);

#endif /* __EXTERNAL_COMMAND_PROCESSOR_H__ */
