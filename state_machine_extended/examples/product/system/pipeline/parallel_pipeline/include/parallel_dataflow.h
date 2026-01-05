/**
 * parallel_dataflow.h  2026-01-05
 *
 * Copyright (C) 2000-2026 All Right Reserved
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Parallel Data Flow Extensions API
 * Extends parallel tasks with data stream capabilities for hybrid
 * workflow-data stream execution models.
 */

#ifndef __PARALLEL_DATAFLOW_H__
#define __PARALLEL_DATAFLOW_H__

#include "parallel_task.h"
#include "data_stream.h"

/* Data stream extensions API */
int parallel_task_add_input_stream(struct parallel_task *task,
                                  data_stream_t stream,
                                  const char *stream_name);

int parallel_task_add_output_stream(struct parallel_task *task,
                                   data_stream_t stream,
                                   const char *stream_name);

int parallel_task_check_dependencies_ex(struct parallel_task *task);

#endif /* __PARALLEL_DATAFLOW_H__ */
