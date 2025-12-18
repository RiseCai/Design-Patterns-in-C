/**
 * task_pipeline_os_example.c - Example of integrating Task Pipeline with OS abstraction layer.
 *
 * This example demonstrates how to use the EFSM OS adapter with the
 * task processing pipeline, enabling event‑driven task scheduling,
 * retry timers, and external system integration.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../src/fsm_os_adapter.h"
#include "../../examples/product/system/pipeline/task_pipeline/include/task_pipeline.h"

/* Simulated external system adapters */
static void mock_message_queue_send(const char *queue, void *status)
{
    printf("[MQ] Sending to queue '%s': %s\n", queue, (char *)status);
}

static void mock_rest_api_post(const char *url, void *result)
{
    printf("[REST] POST to %s: %s\n", url, (char *)result);
}

/* Task event generator thread */
static void task_event_generator(void *arg)
{
    fsm_os_context_t *os_ctx = (fsm_os_context_t *)arg;
    printf("[Event Generator] Thread started\n");

    /* Generate a sequence of task events */
    const char *events[] = {
        "TASK_SUBMIT",
        "TASK_START",
        "TASK_FINISH",
        "TASK_FAIL",
        "TASK_RETRY",
        "TASK_CANCEL"
    };
    for (int i = 0; i < 6; i++) {
        os_thread_sleep(300); /* simulate delay between events */
        printf("[Event Generator] Sending event: %s\n", events[i]);
        /* In a real implementation, we would send a structured event.
         * For simplicity, we just send a string.
         */
        fsm_os_send_event(os_ctx, events[i], strlen(events[i]) + 1, 100);
    }
    printf("[Event Generator] Thread finished\n");
}

/* Timer callback for task timeout */
static void task_timeout_callback(void *arg)
{
    struct task_efsm *task = (struct task_efsm *)arg;
    printf("[Timeout] Task %d timed out\n", task->context.task_id);
    task_efsm_fail(task, "Timeout");
}

int main(void)
{
    printf("=== Task Pipeline OS Adapter Example ===\n");

    if (os_abstract_init() != OS_OK) {
        fprintf(stderr, "Failed to initialize OS abstraction layer\n");
        return EXIT_FAILURE;
    }

    /* Create OS adapter for EFSM protocol (reused for task pipeline) */
    fsm_os_context_t *os_ctx = efsm_protocol_adapter_create();
    if (!os_ctx) {
        fprintf(stderr, "Failed to create OS adapter\n");
        os_abstract_deinit();
        return EXIT_FAILURE;
    }

    /* Initialize a workflow manager with capacity 3 */
    struct workflow_manager wm;
    workflow_manager_init(&wm, 3);

    /* Create three tasks */
    struct task_efsm task1, task2, task3;
    task_efsm_init(&task1, 1, "params1");
    task_efsm_init(&task2, 2, "params2");
    task_efsm_init(&task3, 3, "params3");

    /* Add tasks to workflow */
    workflow_manager_add_task(&wm, &task1);
    workflow_manager_add_task(&wm, &task2);
    workflow_manager_add_task(&wm, &task3);

    /* Start the workflow */
    workflow_manager_start(&wm);

    /* Start event generator thread */
    if (fsm_os_start_thread(os_ctx, task_event_generator, os_ctx) != OS_OK) {
        fprintf(stderr, "Failed to start event generator thread\n");
        fsm_os_context_destroy(os_ctx);
        os_abstract_deinit();
        return EXIT_FAILURE;
    }

    /* Set up a timeout timer for task1 (one‑shot, 2000 ms) */
    os_timer_callback_t timeout_cb = task_timeout_callback;
    os_ctx->timer = os_timer_create(timeout_cb, &task1, "task_timeout");
    if (!os_ctx->timer) {
        fprintf(stderr, "Failed to create timeout timer\n");
    } else {
        os_timer_start(os_ctx->timer, 2000, false); /* one‑shot */
    }

    /* Main loop: process events from the queue */
    printf("[Main] Processing task events...\n");
    for (int i = 0; i < 6; i++) {
        char event_buf[64];
        os_error_t err = fsm_os_receive_event(os_ctx, event_buf, sizeof(event_buf), 1000);
        if (err == OS_OK) {
            printf("[Main] Received event: %s\n", event_buf);
            /* Map event string to task event */
            if (strcmp(event_buf, "TASK_SUBMIT") == 0) {
                task_efsm_submit(&task1);
            } else if (strcmp(event_buf, "TASK_START") == 0) {
                task_efsm_start(&task1);
            } else if (strcmp(event_buf, "TASK_FINISH") == 0) {
                task_efsm_finish(&task1, "result");
            } else if (strcmp(event_buf, "TASK_FAIL") == 0) {
                task_efsm_fail(&task1, "simulated failure");
            } else if (strcmp(event_buf, "TASK_RETRY") == 0) {
                task_efsm_retry(&task1);
            } else if (strcmp(event_buf, "TASK_CANCEL") == 0) {
                task_efsm_cancel(&task1);
            }
        } else {
            printf("[Main] No event received (timeout)\n");
        }
    }

    /* Simulate external system integration */
    printf("[Main] Integrating with external systems...\n");
    mock_message_queue_send("task_status_queue", "Task completed");
    mock_rest_api_post("http://api.example.com/task", "{\"result\":\"success\"}");

    /* Stop workflow */
    workflow_manager_stop(&wm);

    /* Clean up */
    printf("[Main] Cleaning up...\n");
    fsm_os_stop_thread(os_ctx, 1000);
    fsm_os_context_destroy(os_ctx);
    os_abstract_deinit();

    printf("=== Example finished ===\n");
    return EXIT_SUCCESS;
}
