/**
 * parallel_workflow.c  2026-01-05
 *
 * Copyright (C) 2000-2026 All Right Reserved
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Parallel Workflow Management Implementation
 * Workflow orchestration including task scheduling, dependency resolution,
 * concurrency control, and state management.
 */

#include "parallel_workflow.h"
#include "data_stream_service.h"
#include "workflow_monitor.h"
#include <stdlib.h>
#include <string.h>

/* Parallel workflow management */

void parallel_workflow_init(struct parallel_workflow *pw, int capacity, int max_concurrent)
{
    if (!pw || capacity <= 0 || max_concurrent <= 0) {
        TRACE_ERROR("Invalid parameters for workflow init");
        return;
    }

    memset(pw, 0, sizeof(struct parallel_workflow));
    pw->capacity = capacity;
    pw->max_concurrent = max_concurrent;
    pw->state = PWORKFLOW_IDLE;
    pw->running_tasks = 0;

    /* Allocate task array */
    pw->tasks = (struct parallel_task **)malloc(capacity * sizeof(struct parallel_task *));
    if (!pw->tasks) {
        TRACE_ERROR("Failed to allocate task array");
        return;
    }
    memset(pw->tasks, 0, capacity * sizeof(struct parallel_task *));

    /* Create synchronization primitives */
    pw->mutex = os_mutex_create();
    pw->concurrency_sem = os_semaphore_create(max_concurrent, max_concurrent);
    pw->event_queue = os_queue_create(sizeof(int), 32);

    if (!pw->mutex || !pw->concurrency_sem || !pw->event_queue) {
        TRACE_ERROR("Failed to create OS primitives for workflow");
        /* Cleanup will be handled by cleanup function */
    }

    TRACE_INFO("Parallel workflow initialized (capacity=%d, max_concurrent=%d)",
               capacity, max_concurrent);
}

void parallel_workflow_init_ex(struct parallel_workflow *pw,
                              int capacity,
                              int max_concurrent,
                              int enable_data_streams)
{
    /* First do standard initialization */
    parallel_workflow_init(pw, capacity, max_concurrent);

    /* Set data stream flag */
    pw->enable_data_streams = enable_data_streams;

    /* Initialize data stream service if enabled */
    if (enable_data_streams) {
        pw->data_stream_service = data_stream_service_create();
        pw->monitor = workflow_monitor_create();
        /* Default task factory (creates legacy tasks) */
        pw->task_factory = NULL; /* TODO: implement factory */
    } else {
        pw->data_stream_service = NULL;
        pw->monitor = NULL;
        pw->task_factory = NULL;
    }

    TRACE_INFO("Parallel workflow extended init (data_streams=%d)", enable_data_streams);
}

void parallel_workflow_add_task(struct parallel_workflow *pw, struct parallel_task *task)
{
    if (!pw || !task || pw->task_count >= pw->capacity) {
        TRACE_ERROR("Cannot add task to workflow");
        return;
    }

    os_mutex_lock(pw->mutex, OS_WAIT_FOREVER);
    pw->tasks[pw->task_count++] = task;
    task->workflow = pw;  /* Set parent workflow */
    os_mutex_unlock(pw->mutex);

    TRACE_INFO("Task %d added to workflow (total=%d)", task->task_id, pw->task_count);
}

void parallel_workflow_start(struct parallel_workflow *pw)
{
    if (!pw || pw->state != PWORKFLOW_IDLE) {
        TRACE_ERROR("Workflow cannot start (state=%d)", pw->state);
        return;
    }

    pw->state = PWORKFLOW_RUNNING;
    TRACE_INFO("Parallel workflow started");

    os_mutex_lock(pw->mutex, OS_WAIT_FOREVER);

    /* Initialize all tasks to appropriate states */
    for (int i = 0; i < pw->task_count; i++) {
        struct parallel_task *task = pw->tasks[i];
        if (parallel_workflow_check_dependencies(pw, task->task_id)) {
            task->state = PTASK_READY;
        } else {
            task->state = PTASK_WAITING_DEPENDENCIES;
        }
    }

    /* Try to start ready tasks respecting concurrency limit */
    parallel_workflow_try_start_ready_tasks(pw);

    os_mutex_unlock(pw->mutex);
}

void parallel_workflow_pause(struct parallel_workflow *pw)
{
    if (!pw || pw->state != PWORKFLOW_RUNNING) {
        return;
    }

    pw->state = PWORKFLOW_PAUSED;
    TRACE_INFO("Parallel workflow paused");

    /* TODO: Implement task pausing */
}

void parallel_workflow_resume(struct parallel_workflow *pw)
{
    if (!pw || pw->state != PWORKFLOW_PAUSED) {
        return;
    }

    pw->state = PWORKFLOW_RUNNING;
    TRACE_INFO("Parallel workflow resumed");

    /* TODO: Implement task resuming */
}

void parallel_workflow_stop(struct parallel_workflow *pw)
{
    if (!pw) return;

    pw->state = PWORKFLOW_ERROR;
    TRACE_INFO("Parallel workflow stopped");

    /* Cancel all running tasks */
    for (int i = 0; i < pw->task_count; i++) {
        struct parallel_task *task = pw->tasks[i];
        if (task->state == PTASK_RUNNING || task->state == PTASK_READY) {
            parallel_task_cancel(task);
        }
    }
}

void parallel_workflow_reset(struct parallel_workflow *pw)
{
    if (!pw) return;

    parallel_workflow_stop(pw);

    /* Reset all tasks */
    for (int i = 0; i < pw->task_count; i++) {
        struct parallel_task *task = pw->tasks[i];
        task->state = PTASK_IDLE;
        task->error_code = 0;
        task->error_msg[0] = '\0';
        if (task->result) {
            free(task->result);
            task->result = NULL;
        }
    }

    pw->state = PWORKFLOW_IDLE;
    pw->running_tasks = 0;

    TRACE_INFO("Parallel workflow reset");
}

void parallel_workflow_cleanup(struct parallel_workflow *pw)
{
    if (!pw) return;

    /* Cleanup all tasks */
    for (int i = 0; i < pw->task_count; i++) {
        if (pw->tasks[i]) {
            parallel_task_cleanup(pw->tasks[i]);
        }
    }

    /* Free task array */
    if (pw->tasks) {
        free(pw->tasks);
        pw->tasks = NULL;
    }

    /* Destroy OS primitives */
    if (pw->mutex) {
        os_mutex_destroy(pw->mutex);
        pw->mutex = NULL;
    }

    if (pw->concurrency_sem) {
        os_semaphore_destroy(pw->concurrency_sem);
        pw->concurrency_sem = NULL;
    }

    if (pw->event_queue) {
        os_queue_destroy(pw->event_queue);
        pw->event_queue = NULL;
    }

    /* Cleanup data stream extensions */
    if (pw->data_stream_service) {
        data_stream_service_destroy(pw->data_stream_service);
        pw->data_stream_service = NULL;
    }

    if (pw->monitor) {
        workflow_monitor_destroy(pw->monitor);
        pw->monitor = NULL;
    }

    TRACE_INFO("Parallel workflow cleaned up");
}

enum parallel_workflow_state parallel_workflow_get_state(struct parallel_workflow *pw)
{
    return pw ? pw->state : PWORKFLOW_STATE_MAX;
}

int parallel_workflow_get_progress(struct parallel_workflow *pw)
{
    if (!pw || pw->task_count == 0) {
        return 0;
    }

    int completed = 0;
    for (int i = 0; i < pw->task_count; i++) {
        if (pw->tasks[i]->state == PTASK_COMPLETED) {
            completed++;
        }
    }

    return (completed * 100) / pw->task_count;
}

/* Dependency resolution */

int parallel_workflow_check_dependencies(struct parallel_workflow *pw, int task_id)
{
    if (!pw) return 0;

    /* Find the task */
    struct parallel_task *task = NULL;
    for (int i = 0; i < pw->task_count; i++) {
        if (pw->tasks[i]->task_id == task_id) {
            task = pw->tasks[i];
            break;
        }
    }

    if (!task) return 0;

    /* If no dependencies, task is ready */
    if (task->dependency_count == 0) {
        return 1;
    }

    /* Check all dependencies */
    for (int i = 0; i < task->dependency_count; i++) {
        int dep_id = task->depends_on[i];
        int dep_completed = 0;

        /* Find dependency task */
        for (int j = 0; j < pw->task_count; j++) {
            if (pw->tasks[j]->task_id == dep_id) {
                if (pw->tasks[j]->state != PTASK_COMPLETED) {
                    return 0; /* Dependency not completed */
                }
                dep_completed = 1;
                break;
            }
        }

        if (!dep_completed) {
            return 0; /* Dependency not found */
        }
    }

    return 1; /* All dependencies completed */
}

void parallel_workflow_try_start_ready_tasks(struct parallel_workflow *pw)
{
    if (!pw || pw->state != PWORKFLOW_RUNNING) return;

    for (int i = 0; i < pw->task_count; i++) {
        struct parallel_task *task = pw->tasks[i];
        if (task->state == PTASK_WAITING_DEPENDENCIES || task->state == PTASK_READY) {
            if (parallel_workflow_check_dependencies(pw, task->task_id)) {
                /* Try to acquire concurrency semaphore */
                if (os_semaphore_take(pw->concurrency_sem, 0) == OS_OK) {
                    /* Increment running tasks count */
                    pw->running_tasks++;
                    task->state = PTASK_READY;
                    parallel_task_start(task);
                    /* If task failed immediately, adjust counts */
                    if (task->state == PTASK_FAILED) {
                        pw->running_tasks--;
                        os_semaphore_give(pw->concurrency_sem);
                    }
                } else {
                    /* No available concurrency slot, keep waiting */
                    task->state = PTASK_WAITING_DEPENDENCIES;
                    break; /* No more slots available */
                }
            }
        }
    }
}

void parallel_workflow_mark_task_completed(struct parallel_workflow *pw, int task_id)
{
    if (!pw) return;

    os_mutex_lock(pw->mutex, OS_WAIT_FOREVER);

    /* Update running tasks count */
    pw->running_tasks--;

    /* Release concurrency semaphore */
    os_semaphore_give(pw->concurrency_sem);

    /* Try to start tasks that are now ready */
    parallel_workflow_try_start_ready_tasks(pw);

    /* Check if workflow is complete */
    if (pw->running_tasks == 0) {
        int all_completed = 1;
        for (int i = 0; i < pw->task_count; i++) {
            if (pw->tasks[i]->state != PTASK_COMPLETED &&
                pw->tasks[i]->state != PTASK_FAILED &&
                pw->tasks[i]->state != PTASK_CANCELLED) {
                all_completed = 0;
                break;
            }
        }

        if (all_completed) {
            pw->state = PWORKFLOW_COMPLETED;
            if (pw->on_workflow_complete) {
                pw->on_workflow_complete(pw);
            }
        }
    }

    /* Trigger task completion callback */
    if (pw->on_task_complete) {
        pw->on_task_complete(pw, task_id);
    }

    os_mutex_unlock(pw->mutex);

    TRACE_INFO("Task %d marked as completed", task_id);
}
