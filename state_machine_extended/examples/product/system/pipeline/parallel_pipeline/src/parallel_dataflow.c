/**
 * parallel_dataflow.c  2026-01-05
 *
 * Copyright (C) 2000-2026 All Right Reserved
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Parallel Data Flow Extensions Implementation
 * Extends parallel tasks with data stream capabilities for hybrid workflows.
 */

#include "parallel_dataflow.h"
#include "data_stream.h"
#include "parallel_workflow.h"
#include <stdlib.h>
#include <string.h>

/* Data stream extensions API */

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
