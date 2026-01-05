/**
 * parallel_task.h  2026-01-05
 *
 * Copyright (C) 2000-2026 All Right Reserved
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Parallel Task Core API
 * Core functionality for parallel task management including lifecycle,
 * state management, dependency handling, and threading.
 */

#ifndef __PARALLEL_TASK_H__
#define __PARALLEL_TASK_H__

#include "mycommon.h"
#include "mytrace.h"
#include "task_pipeline.h"
#include "fsm_os_adapter.h"
#include "parallel_fsm.h"

/* Forward declaration for data streams (defined in data_stream.h) */
#ifndef __DATA_STREAM_TYPES_DEFINED__
#define __DATA_STREAM_TYPES_DEFINED__
typedef struct data_stream *data_stream_t;
#endif

/* Define TRACE_ERROR if not already defined */
#ifndef TRACE_ERROR
#define TRACE_ERROR(fmt, ...) _MY_TRACE_STR("[ERROR] " fmt "\n", ##__VA_ARGS__)
#endif

/** Maximum number of dependencies per task */
#define MAX_DEPENDENCIES 10

/** Maximum number of data streams per task */
#define MAX_DATA_STREAMS 5

/** Task type for backward compatibility */
enum parallel_task_type {
    TASK_TYPE_LEGACY,      /* Only task dependencies */
    TASK_TYPE_HYBRID,      /* Task + data dependencies */
    TASK_TYPE_DATAFLOW     /* Only data dependencies */
};

/** Parallel task states (extends task states) */
enum parallel_task_state {
    PTASK_IDLE = 0,
    PTASK_READY,
    PTASK_RUNNING,
    PTASK_COMPLETED,
    PTASK_FAILED,
    PTASK_CANCELLED,
    PTASK_WAITING_DEPENDENCIES,
    PTASK_STATE_MAX
};

/** Parallel task structure */
struct parallel_task {
    struct task_efsm base_task;          /* Base task state machine */
    fsm_os_context_t *os_context;        /* Independent OS context per task */
    os_thread_t *thread;                 /* Task execution thread */
    void (*task_function)(void *arg);    /* Task execution function */
    void *task_arg;                      /* Task argument */
    int depends_on[MAX_DEPENDENCIES];    /* IDs of tasks this task depends on */
    int dependency_count;                /* Number of dependencies */
    int task_id;                         /* Unique task identifier */
    enum parallel_task_state state;      /* Current parallel task state */
    void *result;                        /* Task result (optional) */
    int error_code;                      /* Task error code */
    char error_msg[256];                 /* Task error message */
    struct parallel_workflow *workflow;  /* Parent workflow (optional) */

    /* Data stream extensions (added for hybrid model) */
    enum parallel_task_type type;        /* Task type for backward compatibility */
    data_stream_t *input_streams;        /* Input data streams */
    data_stream_t *output_streams;       /* Output data streams */
    int stream_count;                    /* Number of streams (both input+output) */
    const char *stream_names[MAX_DATA_STREAMS]; /* Names for each stream */

    /* Dependency check function pointer (polymorphic) */
    int (*check_dependencies)(struct parallel_task *task);
};

/* Forward declaration for workflow */
struct parallel_workflow;

/* Parallel task management */
void parallel_task_init(struct parallel_task *pt, int task_id,
                        void (*task_function)(void *arg), void *task_arg);
void parallel_task_add_dependency(struct parallel_task *pt, int depends_on_id);
void parallel_task_start(struct parallel_task *pt);
void parallel_task_cancel(struct parallel_task *pt);
void parallel_task_cleanup(struct parallel_task *pt);
enum parallel_task_state parallel_task_get_state(struct parallel_task *pt);
void *parallel_task_get_result(struct parallel_task *pt);

/* Task execution thread function */
void parallel_task_thread_func(void *arg);

/* Dependency resolution for tasks */
int parallel_task_check_dependencies_legacy(struct parallel_task *task);
int parallel_task_check_dependencies_hybrid(struct parallel_task *task);
int parallel_task_check_dependencies_dataflow(struct parallel_task *task);

#endif /* __PARALLEL_TASK_H__ */
