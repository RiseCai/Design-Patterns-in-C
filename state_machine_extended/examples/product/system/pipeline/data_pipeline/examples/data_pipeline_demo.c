/**
 * data_pipeline_demo.c  2025-12-16
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
 * Demo of data processing pipeline.
 */
#include "../include/data_pipeline.h"
#include <stdio.h>

static void stage_input_process(struct pipeline_stage_component *stage)
{
    printf("Stage INPUT processing...\n");
    // Simulate reading file
    file_adapter_read_image("input.jpg", &stage->pipeline_ctx->input_data);
    pipeline_stage_fsm(stage, PIPELINE_EVENT_STAGE_COMPLETE);
}

static void stage_preprocess_process(struct pipeline_stage_component *stage)
{
    printf("Stage PREPROCESS processing...\n");
    // Simulate preprocessing
    pipeline_stage_fsm(stage, PIPELINE_EVENT_STAGE_COMPLETE);
}

static void stage_process_process(struct pipeline_stage_component *stage)
{
    printf("Stage PROCESS processing...\n");
    // Simulate processing
    pipeline_stage_fsm(stage, PIPELINE_EVENT_STAGE_COMPLETE);
}

static void stage_postprocess_process(struct pipeline_stage_component *stage)
{
    printf("Stage POSTPROCESS processing...\n");
    // Simulate postprocessing
    pipeline_stage_fsm(stage, PIPELINE_EVENT_STAGE_COMPLETE);
}

static void stage_output_process(struct pipeline_stage_component *stage)
{
    printf("Stage OUTPUT processing...\n");
    // Simulate writing file
    file_adapter_write_image("output.jpg", stage->pipeline_ctx->input_data);
    network_adapter_upload_to_cloud(stage->pipeline_ctx->output_data);
    pipeline_stage_fsm(stage, PIPELINE_EVENT_STAGE_COMPLETE);
}

int main(void)
{
    struct data_pipeline dp;
    struct pipeline_stage_component stages[STAGE_COUNT];

    printf("=== Data Pipeline Demo ===\n");

    data_pipeline_init(&dp);

    // Initialize stages
    for (int i = 0; i < STAGE_COUNT; i++) {
        pipeline_stage_init(&stages[i], i, &dp.context);
        dp.stages[i] = &stages[i];
    }

    // Assign process functions
    stages[STAGE_INPUT].process = stage_input_process;
    stages[STAGE_PREPROCESS].process = stage_preprocess_process;
    stages[STAGE_PROCESS].process = stage_process_process;
    stages[STAGE_POSTPROCESS].process = stage_postprocess_process;
    stages[STAGE_OUTPUT].process = stage_output_process;

    // Start pipeline
    data_pipeline_start(&dp);

    // Process each stage sequentially (for demo)
    for (int i = 0; i < STAGE_COUNT; i++) {
        pipeline_stage_process(&stages[i]);
    }

    // Pause and resume
    data_pipeline_pause(&dp);
    data_pipeline_resume(&dp);

    // Stop pipeline
    data_pipeline_stop(&dp);

    printf("Demo completed.\n");
    return 0;
}
