/**
 * data_pipeline_os_example.c - Example of integrating Data Pipeline with OS abstraction layer.
 *
 * This example demonstrates how to use the Parallel FSM OS adapter with the
 * data processing pipeline, enabling concurrent stage execution, event queues,
 * and timers.
 */

#include <stdio.h>
#include <stdlib.h>
#include "../../src/fsm_os_adapter.h"
#include "../../examples/product/system/pipeline/data_pipeline/include/data_pipeline.h"

/* Forward declarations for stage processing functions */
static void stage_input_process(struct pipeline_stage_component *stage);
static void stage_preprocess_process(struct pipeline_stage_component *stage);
static void stage_process_process(struct pipeline_stage_component *stage);
static void stage_postprocess_process(struct pipeline_stage_component *stage);
static void stage_output_process(struct pipeline_stage_component *stage);

/* Thread entry for a pipeline stage */
static void stage_thread(void *arg)
{
    struct pipeline_stage_component *stage = (struct pipeline_stage_component *)arg;
    if (!stage) return;

    printf("[Stage %d] Thread started\n", stage->stage_id);

    /* Process the stage */
    pipeline_stage_process(stage);

    printf("[Stage %d] Thread finished\n", stage->stage_id);
}

int main(void)
{
    printf("=== Data Pipeline OS Adapter Example ===\n");

    if (os_abstract_init() != OS_OK) {
        fprintf(stderr, "Failed to initialize OS abstraction layer\n");
        return EXIT_FAILURE;
    }

    /* Create OS adapter for parallel FSM (5 stages) */
    fsm_os_context_t *os_ctx = parallel_fsm_adapter_create(STAGE_COUNT);
    if (!os_ctx) {
        fprintf(stderr, "Failed to create OS adapter\n");
        os_abstract_deinit();
        return EXIT_FAILURE;
    }

    /* Initialize data pipeline */
    struct data_pipeline dp;
    data_pipeline_init(&dp);

    /* Initialize stages */
    struct pipeline_stage_component stages[STAGE_COUNT];
    for (int i = 0; i < STAGE_COUNT; i++) {
        pipeline_stage_init(&stages[i], i, &dp.context);
        dp.stages[i] = &stages[i];
    }

    /* Assign process functions */
    stages[STAGE_INPUT].process = stage_input_process;
    stages[STAGE_PREPROCESS].process = stage_preprocess_process;
    stages[STAGE_PROCESS].process = stage_process_process;
    stages[STAGE_POSTPROCESS].process = stage_postprocess_process;
    stages[STAGE_OUTPUT].process = stage_output_process;

    /* Start pipeline (changes state to RUNNING) */
    data_pipeline_start(&dp);

    /* Create a thread for each stage using OS adapter */
    for (int i = 0; i < STAGE_COUNT; i++) {
        /* Each stage could have its own OS context, but for simplicity we reuse the same adapter.
         * In a real scenario, each component might have its own thread.
         */
        fsm_os_context_t *stage_ctx = fsm_os_context_create();
        if (!stage_ctx) {
            fprintf(stderr, "Failed to create OS context for stage %d\n", i);
            continue;
        }
        /* Start a thread for the stage */
        fsm_os_start_thread(stage_ctx, stage_thread, &stages[i]);
        /* Store context somewhere for later cleanup (not implemented) */
        fsm_os_context_destroy(stage_ctx); /* For demo only; in real code you would keep it */
    }

    /* Simulate sending events to pipeline via OS queue */
    printf("[Main] Sending START event to pipeline\n");
    /* In a real implementation, we would send events to the pipeline's event queue.
     * For simplicity, we just call data_pipeline_start() again.
     */

    /* Wait for stages to complete (simulated) */
    os_thread_sleep(1000);

    /* Pause and resume pipeline */
    printf("[Main] Pausing pipeline\n");
    data_pipeline_pause(&dp);
    os_thread_sleep(500);
    printf("[Main] Resuming pipeline\n");
    data_pipeline_resume(&dp);

    /* Stop pipeline */
    printf("[Main] Stopping pipeline\n");
    data_pipeline_stop(&dp);

    /* Clean up OS adapter */
    fsm_os_context_destroy(os_ctx);
    os_abstract_deinit();

    printf("=== Example finished ===\n");
    return EXIT_SUCCESS;
}

/* Stage processing functions (stubs) */
static void stage_input_process(struct pipeline_stage_component *stage)
{
    printf("Stage INPUT processing...\n");
    file_adapter_read_image("input.jpg", &stage->pipeline_ctx->input_data);
    pipeline_stage_fsm(stage, PIPELINE_EVENT_STAGE_COMPLETE);
}

static void stage_preprocess_process(struct pipeline_stage_component *stage)
{
    printf("Stage PREPROCESS processing...\n");
    pipeline_stage_fsm(stage, PIPELINE_EVENT_STAGE_COMPLETE);
}

static void stage_process_process(struct pipeline_stage_component *stage)
{
    printf("Stage PROCESS processing...\n");
    pipeline_stage_fsm(stage, PIPELINE_EVENT_STAGE_COMPLETE);
}

static void stage_postprocess_process(struct pipeline_stage_component *stage)
{
    printf("Stage POSTPROCESS processing...\n");
    pipeline_stage_fsm(stage, PIPELINE_EVENT_STAGE_COMPLETE);
}

static void stage_output_process(struct pipeline_stage_component *stage)
{
    printf("Stage OUTPUT processing...\n");
    file_adapter_write_image("output.jpg", stage->pipeline_ctx->input_data);
    network_adapter_upload_to_cloud(stage->pipeline_ctx->output_data);
    pipeline_stage_fsm(stage, PIPELINE_EVENT_STAGE_COMPLETE);
}
