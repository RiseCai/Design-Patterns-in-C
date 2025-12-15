/**
 * data_pipeline.c  2025-12-16
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
 * Implementation of data processing pipeline using parallel FSM.
 */
#include "../include/data_pipeline.h"
#include <string.h>
#include <stdlib.h>

void pipeline_stage_fsm(struct pipeline_stage_component *stage, enum pipeline_event event);

void data_pipeline_init(struct data_pipeline *dp)
{
    if (!dp) return;
    memset(dp, 0, sizeof(*dp));
    dp->context.current_state = PIPELINE_IDLE;
    for (int i = 0; i < STAGE_COUNT; i++) {
        dp->context.stage_states[i] = STAGE_IDLE;
        dp->stages[i] = NULL;
    }
    parallel_machine_init(&dp->parallel_machine, STAGE_COUNT);
}

void data_pipeline_start(struct data_pipeline *dp)
{
    if (!dp || dp->context.current_state != PIPELINE_IDLE) return;
    dp->context.current_state = PIPELINE_RUNNING;
    TRACE_INFO("Data pipeline started.");
    // Start all stages
    for (int i = 0; i < STAGE_COUNT; i++) {
        if (dp->stages[i]) {
            pipeline_stage_fsm(dp->stages[i], PIPELINE_EVENT_START);
        }
    }
}

void data_pipeline_pause(struct data_pipeline *dp)
{
    if (!dp || dp->context.current_state != PIPELINE_RUNNING) return;
    dp->context.current_state = PIPELINE_PAUSED;
    TRACE_INFO("Data pipeline paused.");
    // Pause all stages
    for (int i = 0; i < STAGE_COUNT; i++) {
        if (dp->stages[i]) {
            pipeline_stage_fsm(dp->stages[i], PIPELINE_EVENT_PAUSE);
        }
    }
}

void data_pipeline_resume(struct data_pipeline *dp)
{
    if (!dp || dp->context.current_state != PIPELINE_PAUSED) return;
    dp->context.current_state = PIPELINE_RUNNING;
    TRACE_INFO("Data pipeline resumed.");
    // Resume all stages
    for (int i = 0; i < STAGE_COUNT; i++) {
        if (dp->stages[i]) {
            pipeline_stage_fsm(dp->stages[i], PIPELINE_EVENT_RESUME);
        }
    }
}

void data_pipeline_stop(struct data_pipeline *dp)
{
    if (!dp) return;
    dp->context.current_state = PIPELINE_IDLE;
    TRACE_INFO("Data pipeline stopped.");
    // Stop all stages
    for (int i = 0; i < STAGE_COUNT; i++) {
        if (dp->stages[i]) {
            pipeline_stage_fsm(dp->stages[i], PIPELINE_EVENT_STOP);
        }
    }
}

void data_pipeline_reset(struct data_pipeline *dp)
{
    if (!dp) return;
    dp->context.current_state = PIPELINE_IDLE;
    dp->context.error_code = 0;
    dp->context.error_msg[0] = '\0';
    for (int i = 0; i < STAGE_COUNT; i++) {
        dp->context.stage_states[i] = STAGE_IDLE;
    }
    TRACE_INFO("Data pipeline reset.");
}

void pipeline_stage_init(struct pipeline_stage_component *stage, enum pipeline_stage id, struct data_pipeline_context *ctx)
{
    if (!stage || !ctx) return;
    memset(stage, 0, sizeof(*stage));
    stage->stage_id = id;
    stage->pipeline_ctx = ctx;
    stage->process = NULL;
    /* Initialize base parallel_component fields */
    stage->base.id = id;
    stage->base.state = &ctx->stage_states[id]; /* point to stage state */
    stage->base.step = NULL; /* no step function defined */
    stage->base.entry = NULL;
    stage->base.do_action = NULL;
    stage->base.exit = NULL;
}

void pipeline_stage_process(struct pipeline_stage_component *stage)
{
    if (!stage || !stage->process) return;
    stage->process(stage);
}

void pipeline_stage_fsm(struct pipeline_stage_component *stage, enum pipeline_event event)
{
    if (!stage) return;
    switch (event) {
        case PIPELINE_EVENT_START:
            stage->pipeline_ctx->stage_states[stage->stage_id] = STAGE_ACTIVE;
            TRACE_INFO("Stage %d started.", stage->stage_id);
            break;
        case PIPELINE_EVENT_PAUSE:
            TRACE_INFO("Stage %d paused.", stage->stage_id);
            break;
        case PIPELINE_EVENT_RESUME:
            TRACE_INFO("Stage %d resumed.", stage->stage_id);
            break;
        case PIPELINE_EVENT_STOP:
            stage->pipeline_ctx->stage_states[stage->stage_id] = STAGE_IDLE;
            TRACE_INFO("Stage %d stopped.", stage->stage_id);
            break;
        case PIPELINE_EVENT_STAGE_COMPLETE:
            stage->pipeline_ctx->stage_states[stage->stage_id] = STAGE_COMPLETED;
            TRACE_INFO("Stage %d completed.", stage->stage_id);
            break;
        case PIPELINE_EVENT_ERROR:
            stage->pipeline_ctx->stage_states[stage->stage_id] = STAGE_FAILED;
            TRACE_INFO("Stage %d failed.", stage->stage_id);
            break;
        default:
            break;
    }
}

/* Example adapters (stubs) */
void file_adapter_read_image(const char *filename, void **data)
{
    TRACE_INFO("Reading image from %s", filename);
    *data = malloc(1); // dummy
}

void file_adapter_write_image(const char *filename, void *data)
{
    TRACE_INFO("Writing image to %s", filename);
    free(data);
}

void network_adapter_upload_to_cloud(void *data)
{
    TRACE_INFO("Uploading data to cloud.");
}

void database_adapter_store_task_status(const char *task_id, void *status)
{
    TRACE_INFO("Storing task status for %s", task_id);
}
