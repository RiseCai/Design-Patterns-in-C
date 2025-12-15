/**
 * data_pipeline.h  2025-12-16
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
 * Data processing pipeline (image, audio) using parallel FSM.
 */
#ifndef __DATA_PIPELINE_H__
#define __DATA_PIPELINE_H__

#include "mycommon.h"
#include "mytrace.h"
#include "parallel_fsm.h"

/** Pipeline events */
enum pipeline_event {
    PIPELINE_EVENT_START,
    PIPELINE_EVENT_PAUSE,
    PIPELINE_EVENT_RESUME,
    PIPELINE_EVENT_STOP,
    PIPELINE_EVENT_STAGE_COMPLETE,
    PIPELINE_EVENT_ERROR,
    PIPELINE_EVENT_RESET,
    PIPELINE_EVENT_MAX
};

/** Pipeline top-level states */
enum pipeline_state {
    PIPELINE_IDLE,
    PIPELINE_RUNNING,
    PIPELINE_PAUSED,
    PIPELINE_ERROR
};

/** Processing stage identifiers */
enum pipeline_stage {
    STAGE_INPUT,
    STAGE_PREPROCESS,
    STAGE_PROCESS,
    STAGE_POSTPROCESS,
    STAGE_OUTPUT,
    STAGE_COUNT
};

/** Stage-specific states */
enum stage_state {
    STAGE_IDLE,
    STAGE_ACTIVE,
    STAGE_COMPLETED,
    STAGE_FAILED
};

/** Data pipeline context */
struct data_pipeline_context {
    enum pipeline_state current_state;
    enum stage_state stage_states[STAGE_COUNT];
    void *input_data;
    void *output_data;
    int error_code;
    char error_msg[256];
};

/** Stage FSM component */
struct pipeline_stage_component {
    struct parallel_component base;
    enum pipeline_stage stage_id;
    struct data_pipeline_context *pipeline_ctx;
    void (*process)(struct pipeline_stage_component *stage);
};

/** Data pipeline master */
struct data_pipeline {
    struct parallel_machine parallel_machine;
    struct data_pipeline_context context;
    struct pipeline_stage_component *stages[STAGE_COUNT];
};

/* Pipeline management */
void data_pipeline_init(struct data_pipeline *dp);
void data_pipeline_start(struct data_pipeline *dp);
void data_pipeline_pause(struct data_pipeline *dp);
void data_pipeline_resume(struct data_pipeline *dp);
void data_pipeline_stop(struct data_pipeline *dp);
void data_pipeline_reset(struct data_pipeline *dp);

/* Stage management */
void pipeline_stage_init(struct pipeline_stage_component *stage, enum pipeline_stage id, struct data_pipeline_context *ctx);
void pipeline_stage_process(struct pipeline_stage_component *stage);
void pipeline_stage_fsm(struct pipeline_stage_component *stage, enum pipeline_event event);

/* External system adapters */
typedef void (*file_adapter_read)(const char *filename, void **data);
typedef void (*file_adapter_write)(const char *filename, void *data);
typedef void (*network_adapter_upload)(void *data);
typedef void (*network_adapter_download)(const char *url, void **data);
typedef void (*database_adapter_store)(const char *key, void *data);
typedef void (*database_adapter_retrieve)(const char *key, void **data);

/* Example adapters (to be implemented in src/) */
void file_adapter_read_image(const char *filename, void **data);
void file_adapter_write_image(const char *filename, void *data);
void network_adapter_upload_to_cloud(void *data);
void database_adapter_store_task_status(const char *task_id, void *status);

#endif /* __DATA_PIPELINE_H__ */
