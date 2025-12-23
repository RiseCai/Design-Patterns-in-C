/**
 * viper_entity.h - VIPER Entity Component Interface
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * VIPER Entity component implemented as an Extended Finite State Machine (EFSM).
 * Manages data with extended variables and conditions.
 */

#ifndef __VIPER_ENTITY_H__
#define __VIPER_ENTITY_H__

#include <time.h>
#include "viper.h"
#include "../../state_machine_extended/src/efsm_protocol.h"

/**
 * Entity States
 */
typedef enum {
    ENTITY_STATE_INITIAL,
    ENTITY_STATE_LOADED,
    ENTITY_STATE_MODIFIED,
    ENTITY_STATE_SAVING,
    ENTITY_STATE_ERROR,
    ENTITY_STATE_VALID,
    ENTITY_STATE_INVALID,
} viper_entity_state_t;

/**
 * Entity Events
 */
typedef enum {
    ENTITY_EVENT_LOAD_DATA,
    ENTITY_EVENT_UPDATE_DATA,
    ENTITY_EVENT_VALIDATE_DATA,
    ENTITY_EVENT_SAVE_DATA,
    ENTITY_EVENT_DELETE_DATA,
    ENTITY_EVENT_QUERY_DATA,
} viper_entity_event_t;

/**
 * Entity Data Structure
 */
struct viper_entity_data {
    /* Data storage */
    void *data_buffer;
    size_t data_size;
    
    /* Metadata */
    char data_type[64];
    int version;
    time_t last_modified;
    
    /* Validation flags */
    unsigned int is_valid : 1;
    unsigned int is_dirty : 1;
    unsigned int is_persisted : 1;
    
    /* Extended variables for EFSM */
    int validation_score;
    char validation_errors[256];
};

/**
 * VIPER Entity Component
 */
struct viper_entity {
    /* Extended Finite State Machine */
    struct efsm_processor *fsm;
    
    /* Entity data */
    struct viper_entity_data data;
    
    /* Configuration */
    char entity_name[64];
    int entity_id;
    
    /* Data operations */
    int (*load_callback)(void **data, size_t *size);
    int (*save_callback)(const void *data, size_t size);
    int (*validate_callback)(const void *data, size_t size, char *errors, size_t errors_size);
};

/* Function prototypes */

/**
 * Initialize a VIPER Entity component
 * 
 * @param entity_name Name of the entity
 * @param entity_id Unique ID for the entity
 * @return New entity instance, NULL on error
 */
struct viper_entity *viper_entity_init(const char *entity_name,
                                       int entity_id);

/**
 * Destroy a VIPER Entity component
 * 
 * @param entity Entity to destroy
 */
void viper_entity_destroy(struct viper_entity *entity);

/**
 * Load data into entity
 * 
 * @param entity Entity instance
 * @param source Source identifier
 * @param source_data Source-specific data
 * @return 0 on success, negative on error
 */
int viper_entity_load_data(struct viper_entity *entity,
                           const char *source,
                           const void *source_data);

/**
 * Update entity data
 * 
 * @param entity Entity instance
 * @param new_data New data
 * @param data_size Size of new data
 * @return 0 on success, negative on error
 */
int viper_entity_update_data(struct viper_entity *entity,
                             const void *new_data,
                             size_t data_size);

/**
 * Validate entity data
 * 
 * @param entity Entity instance
 * @param validation_rules Validation rules to apply
 * @param rules_size Size of validation rules
 * @return Validation score (0-100), negative on error
 */
int viper_entity_validate_data(struct viper_entity *entity,
                               const void *validation_rules,
                               size_t rules_size);

/**
 * Save entity data
 * 
 * @param entity Entity instance
 * @param destination Destination identifier
 * @param destination_data Destination-specific data
 * @return 0 on success, negative on error
 */
int viper_entity_save_data(struct viper_entity *entity,
                           const char *destination,
                           const void *destination_data);

/**
 * Query entity data
 * 
 * @param entity Entity instance
 * @param query Query specification
 * @param query_size Size of query
 * @param result Result buffer (allocated by function)
 * @param result_size Size of result data
 * @return 0 on success, negative on error
 */
int viper_entity_query_data(struct viper_entity *entity,
                            const void *query,
                            size_t query_size,
                            void **result,
                            size_t *result_size);

/**
 * Get current entity state
 * 
 * @param entity Entity instance
 * @return Current entity state
 */
viper_entity_state_t viper_entity_get_state(const struct viper_entity *entity);

/**
 * Get entity data for debugging
 * 
 * @param entity Entity instance
 * @return Entity data structure
 */
const struct viper_entity_data *viper_entity_get_data(const struct viper_entity *entity);

#endif /* __VIPER_ENTITY_H__ */
