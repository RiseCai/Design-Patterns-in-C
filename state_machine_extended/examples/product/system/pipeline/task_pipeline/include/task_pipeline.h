/**
 * task_pipeline.h  2025-12-16
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
 * Task processing pipeline using Extended Finite State Machine (EFSM).
 */
#ifndef __TASK_PIPELINE_H__
#define __TASK_PIPELINE_H__

#include "mycommon.h"
#include "mytrace.h"
#include "efsm_protocol.h"

/** Task events */
enum task_event {
    TASK_EVENT_SUBMIT,
    TASK_EVENT_START,
    TASK_EVENT_FINISH,
    TASK_EVENT_FAIL,
    TASK_EVENT_RETRY,
    TASK_EVENT_CANCEL,
    TASK_EVENT_TIMEOUT,
    TASK_EVENT_MAX
};

/** Task states */
enum task_state {
    TASK_PENDING,
    TASK_RUNNING,
    TASK_COMPLETED,
    TASK_FAILED,
    TASK_CANCELLED
};

/** Workflow states */
enum workflow_state {
    WORKFLOW_IDLE,
    WORKFLOW_EXECUTING,
    WORKFLOW_FINISHED,
    WORKFLOW_ERROR
};

/** Task context (extended variables) */
struct task_context {
    enum task_state state;
    int task_id;
    void *parameters;
    void *result;
    int retry_count;
    int max_retries;
    char error_msg[256];
};

/** Workflow context */
struct workflow_context {
    enum workflow_state state;
    struct task_context **tasks;
    int task_count;
    int completed_tasks;
    int failed_tasks;
};

/** EFSM for a single task */
struct task_efsm {
    struct efsm_processor base;
    struct task_context context;
    void (*on_start)(struct task_efsm *task);
    void (*on_finish)(struct task_efsm *task);
    void (*on_fail)(struct task_efsm *task);
};

/** Workflow manager */
struct workflow_manager {
    struct workflow_context context;
    struct task_efsm **task_fsms;
    int capacity;
};

/* Task management */
void task_efsm_init(struct task_efsm *task, int task_id, void *params);
void task_efsm_submit(struct task_efsm *task);
void task_efsm_start(struct task_efsm *task);
void task_efsm_finish(struct task_efsm *task, void *result);
void task_efsm_fail(struct task_efsm *task, const char *error);
void task_efsm_retry(struct task_efsm *task);
void task_efsm_cancel(struct task_efsm *task);

/* Workflow management */
void workflow_manager_init(struct workflow_manager *wm, int capacity);
void workflow_manager_add_task(struct workflow_manager *wm, struct task_efsm *task);
void workflow_manager_start(struct workflow_manager *wm);
void workflow_manager_pause(struct workflow_manager *wm);
void workflow_manager_resume(struct workflow_manager *wm);
void workflow_manager_stop(struct workflow_manager *wm);

/* External system adapters */
typedef void (*message_queue_send)(const char *queue, void *message);
typedef void (*message_queue_receive)(const char *queue, void **message);
typedef void (*rest_api_post)(const char *url, void *data);
typedef void (*rest_api_get)(const char *url, void **response);

/* Example adapters */
void message_queue_send_task_status(const char *queue, void *status);
void rest_api_post_task_result(const char *url, void *result);

#endif /* __TASK_PIPELINE_H__ */
