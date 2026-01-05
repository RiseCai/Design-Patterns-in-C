/**
 * parallel_task.c  2026-01-05
 *
 * Copyright (C) 2000-2026 All Right Reserved
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Parallel Task Core Implementation
 * Task lifecycle management including initialization, execution,
 * dependency checking, and cleanup.
 */

#include "parallel_task.h"
#include "parallel_workflow.h"
#include "data_stream.h"
#include <stdlib.h>
#include <string.h>

/* Parallel task management */

void parallel_task_init(struct parallel_task *pt, int task_id,
                        void (*task_function)(void *arg), void *task_arg)
{
    if (!pt) return;

    memset(pt, 0, sizeof(struct parallel_task));
    pt->task_id = task_id;
    pt->task_function = task_function;
    pt->task_arg = task_arg;
    pt->state = PTASK_IDLE;
    pt->dependency_count = 0;
    pt->error_code = 0;
    pt->error_msg[0] = '\0';
    pt->result = NULL;
    pt->workflow = NULL;

    /* Initialize base task */
    task_efsm_init(&pt->base_task, task_id, task_arg);

    /* Create OS context for this task */
    pt->os_context = fsm_os_context_create();
    if (pt->os_context) {
        pt->thread = NULL;
    }

    TRACE_INFO("Parallel task %d initialized", task_id);
}

void parallel_task_add_dependency(struct parallel_task *pt, int depends_on_id)
{
    if (!pt || pt->dependency_count >= MAX_DEPENDENCIES) {
        TRACE_ERROR("Cannot add dependency for task %d", pt->task_id);
        return;
    }

    pt->depends_on[pt->dependency_count++] = depends_on_id;
    TRACE_INFO("Task %d depends on task %d", pt->task_id, depends_on_id);
}

void parallel_task_start(struct parallel_task *pt)
{
    if (!pt || pt->state != PTASK_READY) {
        TRACE_ERROR("Task %d cannot start (state=%d)", pt->task_id, pt->state);
        return;
    }

    pt->state = PTASK_RUNNING;
    task_efsm_start(&pt->base_task);

    /* Create thread for task execution */
    if (pt->os_context && pt->task_function) {
        pt->thread = os_thread_create(parallel_task_thread_func, pt, "parallel_task", 4096, OS_THREAD_PRIORITY_NORMAL);
        if (!pt->thread) {
            TRACE_ERROR("Failed to create thread for task %d", pt->task_id);
            pt->state = PTASK_FAILED;
            strncpy(pt->error_msg, "Thread creation failed", sizeof(pt->error_msg)-1);
            /* Note: running_tasks increment and semaphore release are handled by caller */
        }
    }

    TRACE_INFO("Task %d started in thread", pt->task_id);
}

void parallel_task_cancel(struct parallel_task *pt)
{
    if (!pt) return;

    if (pt->state == PTASK_RUNNING) {
        /* Signal cancellation */
        pt->state = PTASK_CANCELLED;
        task_efsm_cancel(&pt->base_task);

        /* Thread cleanup will happen in thread function */
    } else {
        pt->state = PTASK_CANCELLED;
    }

    TRACE_INFO("Task %d cancelled", pt->task_id);
}

void parallel_task_cleanup(struct parallel_task *pt)
{
    if (!pt) return;

    if (pt->thread) {
        os_thread_join(pt->thread, OS_WAIT_FOREVER);
        pt->thread = NULL;
    }

    if (pt->os_context) {
        fsm_os_context_destroy(pt->os_context);
        pt->os_context = NULL;
    }

    if (pt->result) {
        free(pt->result);
        pt->result = NULL;
    }

    TRACE_INFO("Task %d cleaned up", pt->task_id);
}

enum parallel_task_state parallel_task_get_state(struct parallel_task *pt)
{
    return pt ? pt->state : PTASK_STATE_MAX;
}

void *parallel_task_get_result(struct parallel_task *pt)
{
    return pt ? pt->result : NULL;
}

/* Task execution thread function */
void parallel_task_thread_func(void *arg)
{
    struct parallel_task *pt = (struct parallel_task *)arg;
    if (!pt || !pt->task_function) {
        return;
    }

    TRACE_INFO("Task %d thread started", pt->task_id);

    /* Execute task function */
    pt->task_function(pt->task_arg);

    /* Mark task as completed */
    pt->state = PTASK_COMPLETED;
    task_efsm_finish(&pt->base_task, pt->result);

    /* Notify workflow if part of one */
    if (pt->workflow) {
        parallel_workflow_mark_task_completed(pt->workflow, pt->task_id);
    }

    TRACE_INFO("Task %d thread finished", pt->task_id);
}

/* Dependency resolution for tasks */
int parallel_task_check_dependencies_legacy(struct parallel_task *task)
{
    if (!task) return 0;

    /* Only check task dependencies */
    if (task->workflow) {
        return parallel_workflow_check_dependencies(task->workflow, task->task_id);
    }
    return (task->dependency_count == 0);
}

int parallel_task_check_dependencies_hybrid(struct parallel_task *task)
{
    if (!task) return 0;

    /* Check both task and data dependencies */
    if (task->workflow) {
        if (!parallel_workflow_check_dependencies(task->workflow, task->task_id)) {
            return 0;
        }
    }

    /* Check data stream dependencies */
    for (int i = 0; i < task->stream_count; i++) {
        if (task->input_streams && task->input_streams[i]) {
            if (!data_stream_has_data(task->input_streams[i])) {
                return 0;
            }
        }
    }
    return 1;
}

int parallel_task_check_dependencies_dataflow(struct parallel_task *task)
{
    if (!task) return 0;

    /* Only check data dependencies */
    for (int i = 0; i < task->stream_count; i++) {
        if (task->input_streams && task->input_streams[i]) {
            if (!data_stream_has_data(task->input_streams[i])) {
                return 0;
            }
        }
    }
    return 1;
}
