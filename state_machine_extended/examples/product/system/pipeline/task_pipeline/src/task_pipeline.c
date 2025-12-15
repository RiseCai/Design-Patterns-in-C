/**
 * task_pipeline.c  2025-12-16
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
 * Implementation of task processing pipeline using EFSM.
 */
#include "../include/task_pipeline.h"
#include <string.h>
#include <stdlib.h>

static void task_efsm_handle_event(struct task_efsm *task, enum task_event event, void *data);

void task_efsm_init(struct task_efsm *task, int task_id, void *params)
{
    if (!task) return;
    memset(task, 0, sizeof(*task));
    task->context.state = TASK_PENDING;
    task->context.task_id = task_id;
    task->context.parameters = params;
    task->context.retry_count = 0;
    task->context.max_retries = 3;
    task->context.error_msg[0] = '\0';
    efsm_processor_init(&task->base);
}

void task_efsm_submit(struct task_efsm *task)
{
    if (!task || task->context.state != TASK_PENDING) return;
    TRACE_INFO("Task %d submitted.", task->context.task_id);
    // Transition to pending (already pending)
}

void task_efsm_start(struct task_efsm *task)
{
    if (!task || task->context.state != TASK_PENDING) return;
    task->context.state = TASK_RUNNING;
    TRACE_INFO("Task %d started.", task->context.task_id);
    if (task->on_start) {
        task->on_start(task);
    }
}

void task_efsm_finish(struct task_efsm *task, void *result)
{
    if (!task || task->context.state != TASK_RUNNING) return;
    task->context.state = TASK_COMPLETED;
    task->context.result = result;
    TRACE_INFO("Task %d finished.", task->context.task_id);
    if (task->on_finish) {
        task->on_finish(task);
    }
}

void task_efsm_fail(struct task_efsm *task, const char *error)
{
    if (!task) return;
    task->context.state = TASK_FAILED;
    if (error) {
        strncpy(task->context.error_msg, error, sizeof(task->context.error_msg) - 1);
        task->context.error_msg[sizeof(task->context.error_msg) - 1] = '\0';
    }
    TRACE_INFO("Task %d failed: %s", task->context.task_id, error);
    if (task->on_fail) {
        task->on_fail(task);
    }
}

void task_efsm_retry(struct task_efsm *task)
{
    if (!task || task->context.state != TASK_FAILED) return;
    if (task->context.retry_count >= task->context.max_retries) {
        TRACE_INFO("Task %d exceeded max retries.", task->context.task_id);
        return;
    }
    task->context.retry_count++;
    task->context.state = TASK_PENDING;
    TRACE_INFO("Task %d retry %d/%d.", task->context.task_id, task->context.retry_count, task->context.max_retries);
}

void task_efsm_cancel(struct task_efsm *task)
{
    if (!task) return;
    task->context.state = TASK_CANCELLED;
    TRACE_INFO("Task %d cancelled.", task->context.task_id);
}

void workflow_manager_init(struct workflow_manager *wm, int capacity)
{
    if (!wm || capacity <= 0) return;
    memset(wm, 0, sizeof(*wm));
    wm->context.state = WORKFLOW_IDLE;
    wm->context.tasks = NULL;
    wm->context.task_count = 0;
    wm->context.completed_tasks = 0;
    wm->context.failed_tasks = 0;
    wm->task_fsms = (struct task_efsm **)malloc(capacity * sizeof(struct task_efsm *));
    if (wm->task_fsms) {
        wm->capacity = capacity;
        for (int i = 0; i < capacity; i++) {
            wm->task_fsms[i] = NULL;
        }
    } else {
        wm->capacity = 0;
    }
}

void workflow_manager_add_task(struct workflow_manager *wm, struct task_efsm *task)
{
    if (!wm || !task || wm->context.task_count >= wm->capacity) return;
    wm->task_fsms[wm->context.task_count] = task;
    wm->context.task_count++;
    TRACE_INFO("Task %d added to workflow.", task->context.task_id);
}

void workflow_manager_start(struct workflow_manager *wm)
{
    if (!wm || wm->context.state != WORKFLOW_IDLE) return;
    wm->context.state = WORKFLOW_EXECUTING;
    TRACE_INFO("Workflow started with %d tasks.", wm->context.task_count);
    for (int i = 0; i < wm->context.task_count; i++) {
        if (wm->task_fsms[i]) {
            task_efsm_start(wm->task_fsms[i]);
        }
    }
}

void workflow_manager_pause(struct workflow_manager *wm)
{
    if (!wm || wm->context.state != WORKFLOW_EXECUTING) return;
    wm->context.state = WORKFLOW_IDLE; // simplified
    TRACE_INFO("Workflow paused.");
}

void workflow_manager_resume(struct workflow_manager *wm)
{
    if (!wm || wm->context.state != WORKFLOW_IDLE) return;
    wm->context.state = WORKFLOW_EXECUTING;
    TRACE_INFO("Workflow resumed.");
}

void workflow_manager_stop(struct workflow_manager *wm)
{
    if (!wm) return;
    wm->context.state = WORKFLOW_FINISHED;
    TRACE_INFO("Workflow stopped.");
}

static void task_efsm_handle_event(struct task_efsm *task, enum task_event event, void *data)
{
    // This function would be called by the EFSM processor
    // For simplicity, we just log.
    TRACE_INFO("Task %d event %d", task->context.task_id, event);
}

/* Example adapters */
void message_queue_send_task_status(const char *queue, void *status)
{
    TRACE_INFO("Sending task status to queue %s", queue);
}

void rest_api_post_task_result(const char *url, void *result)
{
    TRACE_INFO("POST task result to %s", url);
}
