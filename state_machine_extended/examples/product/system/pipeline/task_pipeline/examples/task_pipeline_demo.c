/**
 * task_pipeline_demo.c  2025-12-16
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
 * Demo of task processing pipeline.
 */
#include "../include/task_pipeline.h"
#include <stdio.h>
#include <string.h>

static void task1_on_start(struct task_efsm *task)
{
    printf("Task %d on_start: processing...\n", task->context.task_id);
    // Simulate work
    printf("Task %d work done.\n", task->context.task_id);
    task_efsm_finish(task, (void *)"result1");
}

static void task1_on_finish(struct task_efsm *task)
{
    printf("Task %d on_finish: result = %s\n", task->context.task_id, (char *)task->context.result);
    message_queue_send_task_status("task_queue", task);
}

static void task1_on_fail(struct task_efsm *task)
{
    printf("Task %d on_fail: %s\n", task->context.task_id, task->context.error_msg);
}

static void task2_on_start(struct task_efsm *task)
{
    printf("Task %d on_start: processing...\n", task->context.task_id);
    // Simulate failure
    task_efsm_fail(task, "Simulated failure");
}

static void task2_on_fail(struct task_efsm *task)
{
    printf("Task %d on_fail: retrying...\n", task->context.task_id);
    task_efsm_retry(task);
    // After retry, we could start again (for demo, we just finish)
    task_efsm_start(task);
    task_efsm_finish(task, (void *)"result2");
}

int main(void)
{
    struct workflow_manager wm;
    struct task_efsm task1, task2;

    printf("=== Task Pipeline Demo ===\n");

    workflow_manager_init(&wm, 5);

    // Initialize tasks
    task_efsm_init(&task1, 1, (void *)"params1");
    task1.on_start = task1_on_start;
    task1.on_finish = task1_on_finish;
    task1.on_fail = task1_on_fail;

    task_efsm_init(&task2, 2, (void *)"params2");
    task2.on_start = task2_on_start;
    task2.on_fail = task2_on_fail;

    // Add tasks to workflow
    workflow_manager_add_task(&wm, &task1);
    workflow_manager_add_task(&wm, &task2);

    // Start workflow
    workflow_manager_start(&wm);

    // Simulate external events
    printf("--- Simulating external events ---\n");
    task_efsm_start(&task1);
    task_efsm_start(&task2);

    // Pause and resume workflow
    workflow_manager_pause(&wm);
    workflow_manager_resume(&wm);

    // Stop workflow
    workflow_manager_stop(&wm);

    // Send results via REST API
    rest_api_post_task_result("http://api.example.com/task/result", task1.context.result);

    printf("Demo completed.\n");
    return 0;
}
