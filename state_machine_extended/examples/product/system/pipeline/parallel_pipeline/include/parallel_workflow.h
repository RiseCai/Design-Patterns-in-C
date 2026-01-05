/**
 * parallel_workflow.h  2026-01-05
 *
 * Copyright (C) 2000-2026 All Right Reserved
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Parallel Workflow Management API
 * Orchestrates parallel task execution including scheduling, dependency
 * resolution, concurrency control, and state management.
 */

#ifndef __PARALLEL_WORKFLOW_H__
#define __PARALLEL_WORKFLOW_H__

#include "parallel_task.h"

/** Parallel workflow states */
enum parallel_workflow_state {
    PWORKFLOW_IDLE,
    PWORKFLOW_RUNNING,
    PWORKFLOW_PAUSED,
    PWORKFLOW_COMPLETED,
    PWORKFLOW_ERROR,
    PWORKFLOW_STATE_MAX
};

/** Parallel workflow manager */
struct parallel_workflow {
    struct parallel_task **tasks;        /* Array of tasks */
    int task_count;                      /* Number of tasks */
    int capacity;                        /* Maximum capacity */
    int max_concurrent;                  /* Maximum concurrent tasks */
    int running_tasks;                   /* Currently running tasks */
    enum parallel_workflow_state state;  /* Workflow state */
    os_semaphore_t *concurrency_sem;     /* Semaphore for concurrency control */
    os_mutex_t *mutex;                   /* Mutex for shared state protection */
    os_queue_t *event_queue;             /* Event queue for workflow events */
    void (*on_workflow_complete)(struct parallel_workflow *pw); /* Callback */
    void (*on_task_complete)(struct parallel_workflow *pw, int task_id); /* Callback */
    void *thread_pool;                   /* Private: Thread pool for task execution */

    /* Data stream extensions (added for hybrid model) */
    int enable_data_streams;             /* Flag to enable data stream support */
    struct data_stream_service *data_stream_service; /* Data stream service */
    struct workflow_monitor *monitor;    /* Workflow monitoring */
    struct parallel_task *(*task_factory)(int task_id, enum parallel_task_type type); /* Task factory */
};

/* Parallel workflow management */
void parallel_workflow_init(struct parallel_workflow *pw, int capacity, int max_concurrent);
void parallel_workflow_init_ex(struct parallel_workflow *pw,
                              int capacity,
                              int max_concurrent,
                              int enable_data_streams);
void parallel_workflow_add_task(struct parallel_workflow *pw, struct parallel_task *task);
void parallel_workflow_start(struct parallel_workflow *pw);
void parallel_workflow_pause(struct parallel_workflow *pw);
void parallel_workflow_resume(struct parallel_workflow *pw);
void parallel_workflow_stop(struct parallel_workflow *pw);
void parallel_workflow_reset(struct parallel_workflow *pw);
void parallel_workflow_cleanup(struct parallel_workflow *pw);
enum parallel_workflow_state parallel_workflow_get_state(struct parallel_workflow *pw);
int parallel_workflow_get_progress(struct parallel_workflow *pw); /* 0-100% */

/* Dependency resolution */
int parallel_workflow_check_dependencies(struct parallel_workflow *pw, int task_id);
void parallel_workflow_mark_task_completed(struct parallel_workflow *pw, int task_id);

/* Internal workflow management (private functions) */
void parallel_workflow_try_start_ready_tasks(struct parallel_workflow *pw);
int parallel_workflow_has_unmet_dependencies(struct parallel_workflow *pw, int task_id);

#endif /* __PARALLEL_WORKFLOW_H__ */
