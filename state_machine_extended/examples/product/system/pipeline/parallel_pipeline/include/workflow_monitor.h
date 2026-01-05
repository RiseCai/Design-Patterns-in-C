/**
 * workflow_monitor.h  2026-01-05
 *
 * Copyright (C) 2000-2026 All Right Reserved
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Workflow Monitor API
 * Provides monitoring, metrics collection, and performance analysis
 * for parallel pipeline workflow execution.
 */

#ifndef __WORKFLOW_MONITOR_H__
#define __WORKFLOW_MONITOR_H__

#include "parallel_workflow.h"

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

/* Workflow monitor management */
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

#endif /* __WORKFLOW_MONITOR_H__ */
