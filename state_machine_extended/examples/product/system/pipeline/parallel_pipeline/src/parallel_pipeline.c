/**
 * parallel_pipeline.c  2026-01-05
 *
 * Copyright (C) 2000-2026 All Right Reserved
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Parallel Pipeline Main Entry Point
 * Provides unified access and backward compatibility after module decoupling.
 */

#include "parallel_pipeline.h"
#include <stdlib.h>
#include <string.h>

/*
 * This file now serves as the main entry point for the parallel pipeline framework.
 * All actual implementations have been moved to their respective modules:
 *
 * - parallel_task.c: Task lifecycle management
 * - parallel_workflow.c: Workflow orchestration
 * - parallel_dataflow.c: Data stream extensions
 * - data_stream_service.c: Data stream management
 * - workflow_monitor.c: Monitoring and metrics
 * - parallel_adapters.c: External system adapters
 *
 * This file remains for:
 * 1. Unified compilation
 * 2. Backward compatibility
 * 3. Future extensions that span multiple modules
 */

/* Marker for successful decoupling */
#ifdef __PARALLEL_PIPELINE_DECOUPLED__
#pragma message("Parallel Pipeline Framework: Successfully Decoupled into Modules")
#endif
