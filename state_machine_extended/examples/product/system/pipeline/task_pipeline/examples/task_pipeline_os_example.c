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
#include "../../../../../../src/fsm_os_adapter.h"
#include "../include/task_pipeline.h"

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

    /* Generate a sequence of task events with task IDs */
    const char *events[] = {
        "1:TASK_FINISH",      /* task1 completes normally */
        "2:TASK_FAIL",        /* task2 fails */
        "2:TASK_RETRY",       /* task2 retries (state becomes PENDING) */
        "2:TASK_START",       /* task2 starts again */
        "2:TASK_FINISH",      /* task2 completes after retry */
        "3:TASK_CANCEL"       /* task3 is cancelled */
    };
    for (int i = 0; i < 6; i++) {
        os_thread_sleep(300); /* simulate delay between events */
        printf("[Event Generator] Sending event: %s\n", events[i]);
        char buf[256] = {0};
        strncpy(buf, events[i], sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';
        os_error_t send_err = fsm_os_send_event(os_ctx, buf, sizeof(buf), 100);
        if (send_err != OS_OK) {
            printf("[Event Generator] Error sending event\n");
        }
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

/* Helper to parse event string into task ID and event type */
static int parse_event(const char *event_str, int *task_id, char *event_type, size_t event_type_size)
{
    char *colon = strchr(event_str, ':');
    if (!colon) return -1;
    *task_id = atoi(event_str);
    strncpy(event_type, colon + 1, event_type_size - 1);
    event_type[event_type_size - 1] = '\0';
    return 0;
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

    /* Give the thread a moment to start and possibly send first event */
    os_thread_sleep(100);

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
        char event_buf[256];
        os_error_t err = fsm_os_receive_event(os_ctx, event_buf, sizeof(event_buf), 1000);
        if (err == OS_OK) {
            printf("[Main] Received event: %s\n", event_buf);
            int task_id;
            char event_type[32];
            if (parse_event(event_buf, &task_id, event_type, sizeof(event_type)) != 0) {
                printf("[Main] Invalid event format, skipping\n");
                continue;
            }
            /* Select task based on ID */
            struct task_efsm *task = NULL;
            if (task_id == 1) task = &task1;
            else if (task_id == 2) task = &task2;
            else if (task_id == 3) task = &task3;
            else {
                printf("[Main] Unknown task ID %d\n", task_id);
                continue;
            }
            /* Map event type to action */
            if (strcmp(event_type, "TASK_SUBMIT") == 0) {
                task_efsm_submit(task);
            } else if (strcmp(event_type, "TASK_START") == 0) {
                task_efsm_start(task);
            } else if (strcmp(event_type, "TASK_FINISH") == 0) {
                task_efsm_finish(task, "result");
            } else if (strcmp(event_type, "TASK_FAIL") == 0) {
                task_efsm_fail(task, "simulated failure");
            } else if (strcmp(event_type, "TASK_RETRY") == 0) {
                task_efsm_retry(task);
            } else if (strcmp(event_type, "TASK_CANCEL") == 0) {
                task_efsm_cancel(task);
            } else {
                printf("[Main] Unknown event type: %s\n", event_type);
            }
            /* Print task state after processing */
            printf("[Main] Task %d state: %d\n", task_id, task->context.state);
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
