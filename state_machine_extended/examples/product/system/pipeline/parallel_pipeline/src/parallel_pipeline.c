/**
 * parallel_pipeline.c  2026-01-04
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
 * Parallel task pipeline implementation.
 */

#include "parallel_pipeline.h"
#include <stdlib.h>
#include <string.h>

/* Macro to suppress unused parameter warnings */
#ifndef UNUSED
#if defined(__GNUC__) || defined(__clang__)
#define UNUSED(x) (void)(x)
#else
#define UNUSED(x) (void)(x)
#endif
#endif

/* Forward declarations */
static void parallel_workflow_try_start_ready_tasks(struct parallel_workflow *pw);
static struct thread_pool *thread_pool_create(int num_threads) __attribute__((unused));
static void thread_pool_destroy(struct thread_pool *pool) __attribute__((unused));
static int thread_pool_submit_task(struct thread_pool *pool, 
                                   struct parallel_task *task,
                                   struct parallel_workflow *workflow) __attribute__((unused));

/* OS adapter functions (provided by os_abstract.h) */

/* Thread pool for task execution */
struct thread_pool {
    os_thread_t **threads;
    int thread_count;
    os_queue_t *task_queue;
    os_semaphore_t *available_workers;
    bool running;
};

/* Task queue item */
struct pool_task_item {
    struct parallel_task *task;
    struct parallel_workflow *workflow;
};

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

static void parallel_workflow_try_start_ready_tasks(struct parallel_workflow *pw)
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

/* Thread pool implementation */

static void worker_thread_func(void *arg)
{
    struct thread_pool *pool = (struct thread_pool *)arg;
    if (!pool) return;
    
    while (pool->running) {
        /* Wait for available work */
        os_semaphore_take(pool->available_workers, OS_WAIT_FOREVER);
        
        /* Get task from queue */
        struct pool_task_item item;
        memset(&item, 0, sizeof(item));
        if (os_queue_receive(pool->task_queue, &item, OS_WAIT_FOREVER) == OS_OK) {
            if (item.task && item.workflow) {
                /* Execute task function */
                TRACE_INFO("Worker executing task %d", item.task->task_id);
                if (item.task->task_function) {
                    item.task->task_function(item.task->task_arg);
                }
                
                /* Mark task as completed */
                item.task->state = PTASK_COMPLETED;
                task_efsm_finish(&item.task->base_task, item.task->result);
                
                /* Notify workflow */
                parallel_workflow_mark_task_completed(item.workflow, item.task->task_id);
            }
        }
    }
}

static struct thread_pool *thread_pool_create(int num_threads)
{
    struct thread_pool *pool = (struct thread_pool *)malloc(sizeof(struct thread_pool));
    if (!pool) return NULL;
    
    memset(pool, 0, sizeof(struct thread_pool));
    pool->thread_count = num_threads;
    pool->running = true;
    
    /* Create task queue */
    pool->task_queue = os_queue_create(sizeof(struct pool_task_item), 32);
    if (!pool->task_queue) {
        free(pool);
        return NULL;
    }
    
    /* Create semaphore for worker availability */
    pool->available_workers = os_semaphore_create(num_threads, num_threads);
    if (!pool->available_workers) {
        os_queue_destroy(pool->task_queue);
        free(pool);
        return NULL;
    }
    
    /* Create worker threads */
    pool->threads = (os_thread_t **)malloc(num_threads * sizeof(os_thread_t *));
    if (!pool->threads) {
        os_semaphore_destroy(pool->available_workers);
        os_queue_destroy(pool->task_queue);
        free(pool);
        return NULL;
    }
    
    for (int i = 0; i < num_threads; i++) {
        pool->threads[i] = os_thread_create(worker_thread_func, pool, "pool_worker", 4096, OS_THREAD_PRIORITY_NORMAL);
        if (!pool->threads[i]) {
            TRACE_ERROR("Failed to create worker thread %d", i);
            /* Clean up already created threads */
            for (int j = 0; j < i; j++) {
                os_thread_join(pool->threads[j], OS_WAIT_FOREVER);
            }
            free(pool->threads);
            os_semaphore_destroy(pool->available_workers);
            os_queue_destroy(pool->task_queue);
            free(pool);
            return NULL;
        }
    }
    
    TRACE_INFO("Thread pool created with %d workers", num_threads);
    return pool;
}

static void thread_pool_destroy(struct thread_pool *pool)
{
    if (!pool) return;
    
    pool->running = false;
    
    /* Signal all workers to exit */
    for (int i = 0; i < pool->thread_count; i++) {
        os_semaphore_give(pool->available_workers);
    }
    
    /* Join all threads */
    for (int i = 0; i < pool->thread_count; i++) {
        if (pool->threads[i]) {
            os_thread_join(pool->threads[i], OS_WAIT_FOREVER);
        }
    }
    
    /* Clean up resources */
    if (pool->threads) {
        free(pool->threads);
    }
    
    if (pool->task_queue) {
        os_queue_destroy(pool->task_queue);
    }
    
    if (pool->available_workers) {
        os_semaphore_destroy(pool->available_workers);
    }
    
    free(pool);
    TRACE_INFO("Thread pool destroyed");
}

static int thread_pool_submit_task(struct thread_pool *pool, 
                                   struct parallel_task *task,
                                   struct parallel_workflow *workflow)
{
    if (!pool || !task || !workflow) return -1;
    
    struct pool_task_item item;
    memset(&item, 0, sizeof(item));
    item.task = task;
    item.workflow = workflow;
    
    /* Wait for semaphore to control concurrency */
    if (os_semaphore_take(pool->available_workers, 1000) != OS_OK) {
        TRACE_ERROR("Failed to acquire worker semaphore for task %d", task->task_id);
        return -1;
    }
    
    /* Submit task to queue */
    if (os_queue_send(pool->task_queue, &item, 1000) != OS_OK) {
        os_semaphore_give(pool->available_workers); /* Release semaphore */
        TRACE_ERROR("Failed to submit task %d to thread pool", task->task_id);
        return -1;
    }
    
    TRACE_INFO("Task %d submitted to thread pool", task->task_id);
    return 0;
}

/* External system adapters (stub implementations) */

void parallel_mq_send_task_status(const char *task_id, const char *status)
{
    TRACE_INFO("[MQ] Sending task status: task=%s, status=%s", task_id, status);
    /* Stub implementation */
}

void parallel_rest_post_task_result(const char *url, const char *task_id, void *result)
{
    TRACE_INFO("[REST] POST to %s: task=%s, result=%p", url, task_id, result);
    /* Stub implementation */
}

/* ==================== Data Stream Extensions ==================== */

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

int parallel_task_add_input_stream(struct parallel_task *task,
                                  data_stream_t stream,
                                  const char *stream_name)
{
    if (!task || !stream || !stream_name) {
        TRACE_ERROR("Invalid parameters for add_input_stream");
        return -1;
    }
    
    if (task->stream_count >= MAX_DATA_STREAMS) {
        TRACE_ERROR("Task %d cannot add more streams (max=%d)", task->task_id, MAX_DATA_STREAMS);
        return -1;
    }
    
    /* Allocate input streams array if not already allocated */
    if (!task->input_streams) {
        task->input_streams = (data_stream_t *)malloc(MAX_DATA_STREAMS * sizeof(data_stream_t));
        if (!task->input_streams) {
            TRACE_ERROR("Failed to allocate input streams array for task %d", task->task_id);
            return -1;
        }
        memset(task->input_streams, 0, MAX_DATA_STREAMS * sizeof(data_stream_t));
    }
    
    /* Add stream */
    task->input_streams[task->stream_count] = stream;
    task->stream_names[task->stream_count] = stream_name;
    task->stream_count++;
    
    /* Update task type if needed */
    if (task->type == TASK_TYPE_LEGACY) {
        task->type = TASK_TYPE_HYBRID;
    }
    
    TRACE_INFO("Task %d added input stream '%s'", task->task_id, stream_name);
    return 0;
}

int parallel_task_add_output_stream(struct parallel_task *task,
                                   data_stream_t stream,
                                   const char *stream_name)
{
    if (!task || !stream || !stream_name) {
        TRACE_ERROR("Invalid parameters for add_output_stream");
        return -1;
    }
    
    if (task->stream_count >= MAX_DATA_STREAMS) {
        TRACE_ERROR("Task %d cannot add more streams (max=%d)", task->task_id, MAX_DATA_STREAMS);
        return -1;
    }
    
    /* Allocate output streams array if not already allocated */
    if (!task->output_streams) {
        task->output_streams = (data_stream_t *)malloc(MAX_DATA_STREAMS * sizeof(data_stream_t));
        if (!task->output_streams) {
            TRACE_ERROR("Failed to allocate output streams array for task %d", task->task_id);
            return -1;
        }
        memset(task->output_streams, 0, MAX_DATA_STREAMS * sizeof(data_stream_t));
    }
    
    /* Add stream */
    task->output_streams[task->stream_count] = stream;
    task->stream_names[task->stream_count] = stream_name;
    task->stream_count++;
    
    /* Update task type if needed */
    if (task->type == TASK_TYPE_LEGACY) {
        task->type = TASK_TYPE_HYBRID;
    }
    
    TRACE_INFO("Task %d added output stream '%s'", task->task_id, stream_name);
    return 0;
}

int parallel_task_check_dependencies_ex(struct parallel_task *task)
{
    if (!task) return 0;
    
    /* Use polymorphic check if set */
    if (task->check_dependencies) {
        return task->check_dependencies(task);
    }
    
    /* Default implementation based on task type */
    switch (task->type) {
        case TASK_TYPE_LEGACY:
            /* Only check task dependencies */
            if (task->workflow) {
                return parallel_workflow_check_dependencies(task->workflow, task->task_id);
            }
            return (task->dependency_count == 0);
            
        case TASK_TYPE_HYBRID:
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
            
        case TASK_TYPE_DATAFLOW:
            /* Only check data dependencies */
            for (int i = 0; i < task->stream_count; i++) {
                if (task->input_streams && task->input_streams[i]) {
                    if (!data_stream_has_data(task->input_streams[i])) {
                        return 0;
                    }
                }
            }
            return 1;
            
        default:
            return 0;
    }
}

/* Data stream service (stub implementation) */

struct data_stream_service {
    int stream_count;
    data_stream_t *streams;
    os_mutex_t *mutex;
};

struct data_stream_service *data_stream_service_create(void)
{
    struct data_stream_service *service = (struct data_stream_service *)malloc(sizeof(struct data_stream_service));
    if (!service) return NULL;
    
    memset(service, 0, sizeof(struct data_stream_service));
    service->mutex = os_mutex_create();
    if (!service->mutex) {
        free(service);
        return NULL;
    }
    
    TRACE_INFO("Data stream service created");
    return service;
}

void data_stream_service_destroy(struct data_stream_service *service)
{
    if (!service) return;
    
    /* Destroy all streams */
    if (service->streams) {
        for (int i = 0; i < service->stream_count; i++) {
            if (service->streams[i]) {
                data_stream_destroy(service->streams[i]);
            }
        }
        free(service->streams);
    }
    
    if (service->mutex) {
        os_mutex_destroy(service->mutex);
    }
    
    free(service);
    TRACE_INFO("Data stream service destroyed");
}

data_stream_t data_stream_service_create_stream(struct data_stream_service *service,
                                               const char *name,
                                               size_t element_size,
                                               int capacity)
{
    if (!service || !name) return NULL;

    struct data_stream_config config = {
        .name = name,
        .element_size = element_size,
        .capacity = capacity,
        .mode = STREAM_MODE_FIFO,  /* default mode */
        .window_size = 0
    };

    data_stream_t stream = data_stream_create(&config);
    if (!stream) return NULL;

    os_mutex_lock(service->mutex, OS_WAIT_FOREVER);

    /* Add to service's stream list */
    service->stream_count++;
    service->streams = (data_stream_t *)realloc(service->streams,
                                                service->stream_count * sizeof(data_stream_t));
    if (!service->streams) {
        service->stream_count = 0;
        os_mutex_unlock(service->mutex);
        data_stream_destroy(stream);
        return NULL;
    }

    service->streams[service->stream_count - 1] = stream;
    os_mutex_unlock(service->mutex);

    TRACE_INFO("Data stream '%s' created via service", name);
    return stream;
}

void data_stream_service_destroy_stream(struct data_stream_service *service,
                                       data_stream_t stream)
{
    if (!service || !stream) return;
    
    os_mutex_lock(service->mutex, OS_WAIT_FOREVER);
    
    /* Find and remove stream from list */
    for (int i = 0; i < service->stream_count; i++) {
        if (service->streams[i] == stream) {
            /* Shift remaining streams */
            for (int j = i; j < service->stream_count - 1; j++) {
                service->streams[j] = service->streams[j + 1];
            }
            service->stream_count--;
            service->streams = (data_stream_t *)realloc(service->streams, 
                                                        service->stream_count * sizeof(data_stream_t));
            break;
        }
    }
    
    os_mutex_unlock(service->mutex);
    
    /* Destroy the stream */
    data_stream_destroy(stream);
    TRACE_INFO("Data stream destroyed via service");
}

/* Workflow monitor (stub implementation) */

struct workflow_monitor {
    int task_count;
    int stream_count;
    timestamp_t start_time;
    os_mutex_t *mutex;
};

struct workflow_monitor *workflow_monitor_create(void)
{
    struct workflow_monitor *monitor = (struct workflow_monitor *)malloc(sizeof(struct workflow_monitor));
    if (!monitor) return NULL;
    
    memset(monitor, 0, sizeof(struct workflow_monitor));
    monitor->mutex = os_mutex_create();
    if (!monitor->mutex) {
        free(monitor);
        return NULL;
    }
    
    monitor->start_time = os_get_timestamp();
    TRACE_INFO("Workflow monitor created");
    return monitor;
}

void workflow_monitor_destroy(struct workflow_monitor *monitor)
{
    if (!monitor) return;
    
    if (monitor->mutex) {
        os_mutex_destroy(monitor->mutex);
    }
    
    free(monitor);
    TRACE_INFO("Workflow monitor destroyed");
}

int workflow_monitor_collect_stats(struct workflow_monitor *monitor,
                                  struct parallel_workflow *pw)
{
    if (!monitor || !pw) return -1;
    
    os_mutex_lock(monitor->mutex, OS_WAIT_FOREVER);
    
    monitor->task_count = pw->task_count;
    monitor->stream_count = 0; /* TODO: collect actual stream count */
    
    os_mutex_unlock(monitor->mutex);
    return 0;
}

void workflow_monitor_print_report(struct workflow_monitor *monitor,
                                  FILE *output)
{
    if (!monitor || !output) return;
    
    fprintf(output, "=== Workflow Monitor Report ===\n");
    fprintf(output, "Tasks: %d\n", monitor->task_count);
    fprintf(output, "Data Streams: %d\n", monitor->stream_count);
    fprintf(output, "Uptime: %lld ms\n", (long long)(os_get_timestamp() - monitor->start_time));
    fprintf(output, "==============================\n");
}

int workflow_monitor_register_callback(struct workflow_monitor *monitor,
                                      monitor_callback_t callback,
                                      void *user_data,
                                      int interval_ms)
{
    if (!monitor || !callback) return -1;
    
    TRACE_INFO("Monitor callback registered (interval=%d ms)", interval_ms);
    /* TODO: implement timer-based callback */
    return 0;
}
