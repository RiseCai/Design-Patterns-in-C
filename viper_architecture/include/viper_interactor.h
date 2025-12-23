/**
 * viper_interactor.h - VIPER Interactor Component Interface
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * VIPER Interactor component implemented as a Moore Hierarchical State Machine.
 * Implements business logic with hierarchical states.
 */

#ifndef __VIPER_INTERACTOR_H__
#define __VIPER_INTERACTOR_H__

#include "viper.h"
#include "../../state_machine_extended/src/moore_hierarchical.h"

/**
 * Interactor States (Hierarchical)
 */
typedef enum {
    INTERACTOR_STATE_IDLE,
    INTERACTOR_STATE_PROCESSING,
    INTERACTOR_STATE_FETCHING_DATA,
    INTERACTOR_STATE_VALIDATING,
    INTERACTOR_STATE_EXECUTING_BUSINESS_LOGIC,
    INTERACTOR_STATE_ERROR,
    INTERACTOR_SUBSTATE_DATA_PROCESSING,
    INTERACTOR_SUBSTATE_CALCULATION,
} viper_interactor_state_t;

/**
 * Interactor Events
 */
typedef enum {
    INTERACTOR_EVENT_REQUEST_DATA,
    INTERACTOR_EVENT_PROCESS_DATA,
    INTERACTOR_EVENT_VALIDATE_INPUT,
    INTERACTOR_EVENT_EXECUTE_BUSINESS_RULE,
    INTERACTOR_EVENT_COMPLETE,
    INTERACTOR_EVENT_ERROR,
} viper_interactor_event_t;

/**
 * Business Logic Context
 */
struct viper_business_context {
    /* Input data */
    void *input_data;
    size_t input_size;
    
    /* Output data */
    void *output_data;
    size_t output_size;
    
    /* Business rules */
    int rule_id;
    void *rule_context;
    
    /* Validation results */
    int validation_status;
    char validation_message[128];
};

/**
 * VIPER Interactor Component
 */
struct viper_interactor {
    /* Moore Hierarchical FSM */
    struct moore_hsm *fsm;
    
    /* Business context */
    struct viper_business_context context;
    
    /* Configuration */
    char interactor_name[64];
    int interactor_id;
    
    /* Entity reference */
    struct viper_entity *entity;
    
    /* Business logic callbacks */
    void *(*process_data_callback)(const void *input, size_t input_size, size_t *output_size);
    int (*validate_input_callback)(const void *input, size_t input_size, char *error_msg);
    void (*business_rule_callback)(void *context, int rule_id);
};

/* Function prototypes */

/**
 * Initialize a VIPER Interactor component
 * 
 * @param interactor_name Name of the interactor
 * @param interactor_id Unique ID for the interactor
 * @param entity Associated entity component
 * @return New interactor instance, NULL on error
 */
struct viper_interactor *viper_interactor_init(const char *interactor_name,
                                               int interactor_id,
                                               struct viper_entity *entity);

/**
 * Destroy a VIPER Interactor component
 * 
 * @param interactor Interactor to destroy
 */
void viper_interactor_destroy(struct viper_interactor *interactor);

/**
 * Process a business logic request
 * 
 * @param interactor Interactor instance
 * @param event Event to process
 * @param request_data Request data
 * @param request_size Size of request data
 * @param output Output buffer (allocated by function)
 * @param output_size Size of output data
 * @return 0 on success, negative on error
 */
int viper_interactor_process_request(struct viper_interactor *interactor,
                                     viper_interactor_event_t event,
                                     const void *request_data,
                                     size_t request_size,
                                     void **output,
                                     size_t *output_size);

/**
 * Execute business rule
 * 
 * @param interactor Interactor instance
 * @param rule_id Rule ID to execute
 * @param rule_context Rule context data
 * @return 0 on success, negative on error
 */
int viper_interactor_execute_rule(struct viper_interactor *interactor,
                                  int rule_id,
                                  void *rule_context);

/**
 * Validate input data
 * 
 * @param interactor Interactor instance
 * @param input_data Input data to validate
 * @param input_size Size of input data
 * @param error_msg Buffer for error message
 * @param error_msg_size Size of error message buffer
 * @return 0 if valid, negative if invalid
 */
int viper_interactor_validate_input(struct viper_interactor *interactor,
                                    const void *input_data,
                                    size_t input_size,
                                    char *error_msg,
                                    size_t error_msg_size);

/**
 * Get current interactor state
 * 
 * @param interactor Interactor instance
 * @return Current interactor state
 */
viper_interactor_state_t viper_interactor_get_state(const struct viper_interactor *interactor);

/**
 * Get business context for debugging
 * 
 * @param interactor Interactor instance
 * @return Business context structure
 */
const struct viper_business_context *viper_interactor_get_context(const struct viper_interactor *interactor);

#endif /* __VIPER_INTERACTOR_H__ */
