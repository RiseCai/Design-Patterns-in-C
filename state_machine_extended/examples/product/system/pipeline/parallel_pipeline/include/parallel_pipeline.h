/**
 * parallel_pipeline.h  2026-01-04
 * 
 * Copyright (C) 2000-2026 All Right Reserved
 * 
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Auto generate for Design Patterns in C *
 * 
 * Parallel task pipeline with independent thread per task.
 */
#ifndef __PARALLEL_PIPELINE_H__
#define __PARALLEL_PIPELINE_H__

#include "mycommon.h"
#include "mytrace.h"
#include "task_pipeline.h"
#include "fsm_os_adapter.h"
#include "parallel_fsm.h"
#include "data_stream.h"

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

/** Parallel workflow states */
enum parallel_workflow_state {
    PWORKFLOW_IDLE,
    PWORKFLOW_RUNNING,
    PWORKFLOW_PAUSED,
    PWORKFLOW_COMPLETED,
    PWORKFLOW_ERROR,
    PWORKFLOW_STATE_MAX
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

/* Parallel task management */
void parallel_task_init(struct parallel_task *pt, int task_id,
                        void (*task_function)(void *arg), void *task_arg);
void parallel_task_add_dependency(struct parallel_task *pt, int depends_on_id);
void parallel_task_start(struct parallel_task *pt);
void parallel_task_cancel(struct parallel_task *pt);
void parallel_task_cleanup(struct parallel_task *pt);
enum parallel_task_state parallel_task_get_state(struct parallel_task *pt);
void *parallel_task_get_result(struct parallel_task *pt);

/* Parallel workflow management */
void parallel_workflow_init(struct parallel_workflow *pw, int capacity, int max_concurrent);
void parallel_workflow_add_task(struct parallel_workflow *pw, struct parallel_task *task);
void parallel_workflow_start(struct parallel_workflow *pw);
void parallel_workflow_pause(struct parallel_workflow *pw);
void parallel_workflow_resume(struct parallel_workflow *pw);
void parallel_workflow_stop(struct parallel_workflow *pw);
void parallel_workflow_reset(struct parallel_workflow *pw);
void parallel_workflow_cleanup(struct parallel_workflow *pw);
enum parallel_workflow_state parallel_workflow_get_state(struct parallel_workflow *pw);
int parallel_workflow_get_progress(struct parallel_workflow *pw); /* 0-100% */

/* Task execution thread function */
void parallel_task_thread_func(void *arg);

/* Dependency resolution */
int parallel_workflow_check_dependencies(struct parallel_workflow *pw, int task_id);
void parallel_workflow_mark_task_completed(struct parallel_workflow *pw, int task_id);

/* External system adapters (reuse from task_pipeline) */
typedef void (*parallel_mq_adapter_send)(const char *queue, const char *message);
typedef void (*parallel_rest_adapter_post)(const char *url, const char *json);

/* Example adapters */
void parallel_mq_send_task_status(const char *task_id, const char *status);
void parallel_rest_post_task_result(const char *url, const char *task_id, void *result);

/* Data stream extensions API */
void parallel_workflow_init_ex(struct parallel_workflow *pw,
                              int capacity,
                              int max_concurrent,
                              int enable_data_streams);

int parallel_task_add_input_stream(struct parallel_task *task,
                                  data_stream_t stream,
                                  const char *stream_name);

int parallel_task_add_output_stream(struct parallel_task *task,
                                   data_stream_t stream,
                                   const char *stream_name);

int parallel_task_check_dependencies_ex(struct parallel_task *task);

/* Data stream service */
struct data_stream_service *data_stream_service_create(void);
void data_stream_service_destroy(struct data_stream_service *service);
data_stream_t data_stream_service_create_stream(struct data_stream_service *service,
                                               const char *name,
                                               size_t element_size,
                                               int capacity);
void data_stream_service_destroy_stream(struct data_stream_service *service,
                                       data_stream_t stream);

/* Workflow monitor */
struct workflow_monitor *workflow_monitor_create(void);
void workflow_monitor_destroy(struct workflow_monitor *monitor);
int workflow_monitor_collect_stats(struct workflow_monitor *monitor,
                                  struct parallel_workflow *pw);
void workflow_monitor_print_report(struct workflow_monitor *monitor,
                                  FILE *output);
typedef void (*monitor_callback_t)(struct workflow_monitor *monitor,
                                  void *user_data);
int workflow_monitor_register_callback(struct workflow_monitor *monitor,
                                      monitor_callback_t callback,
                                      void *user_data,
                                      int interval_ms);

#endif /* __PARALLEL_PIPELINE_H__ */
