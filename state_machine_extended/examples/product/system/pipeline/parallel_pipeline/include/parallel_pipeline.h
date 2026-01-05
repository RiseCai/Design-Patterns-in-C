/**
 * parallel_pipeline.h  2026-01-05
 *
 * Copyright (C) 2000-2026 All Right Reserved
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Parallel Pipeline Unified API
 * Main entry point for the decoupled parallel pipeline framework.
 * Includes all sub-modules for backward compatibility and unified access.
 */

#ifndef __PARALLEL_PIPELINE_H__
#define __PARALLEL_PIPELINE_H__

/*
 * Include all sub-modules for unified API access
 * This maintains backward compatibility while enabling modular usage
 */

/* Core modules - fundamental functionality */
#include "parallel_task.h"              /* Task lifecycle and management */
#include "parallel_workflow.h"          /* Workflow orchestration */

/* Extension modules - optional enhancements */
#include "parallel_dataflow.h"          /* Data stream integration */
#include "data_stream_service.h"        /* Data stream management */
#include "workflow_monitor.h"           /* Monitoring and metrics */

/* Integration modules - external system connectivity */
#include "parallel_adapters.h"          /* External system adapters */

#endif /* __PARALLEL_PIPELINE_H__ */
