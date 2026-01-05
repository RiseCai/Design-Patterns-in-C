# Pipeline Examples: Data Processing, Task Processing, and Parallel Task Execution

This directory contains three pipeline implementations using different state machine types:

1. **Data Processing Pipeline** (`data_pipeline/`) – uses Parallel FSM for image/audio processing.
2. **Task Processing Pipeline** (`task_pipeline/`) – uses Extended Finite State Machine (EFSM) for workflow/task management.
3. **Parallel Task Pipeline** (`parallel_pipeline/`) – uses Parallel FSM with OS‑level threads for true concurrent task execution.

All pipelines demonstrate typical external system interactions (file system, network, database, message queue, REST API) and provide a complete, runnable example.

## 1. Data Processing Pipeline

### Overview
The data processing pipeline models a multi‑stage processing flow (input → pre‑process → process → post‑process → output). Each stage runs as a parallel component, allowing concurrent execution where dependencies permit.

### State Machine Type
**Parallel FSM** (from `parallel_fsm.h`)

- **Top‑level pipeline states**: `PIPELINE_IDLE`, `PIPELINE_RUNNING`, `PIPELINE_PAUSED`, `PIPELINE_ERROR`
- **Stage‑level states**: `STAGE_IDLE`, `STAGE_ACTIVE`, `STAGE_COMPLETED`, `STAGE_FAILED`
- **Events**: `PIPELINE_EVENT_START`, `PAUSE`, `RESUME`, `STOP`, `STAGE_COMPLETE`, `ERROR`, `RESET`

### External System Adapters
- **File system**: `file_adapter_read_image()`, `file_adapter_write_image()`
- **Network**: `network_adapter_upload_to_cloud()`
- **Database**: `database_adapter_store_task_status()`

### Example Run
The demo (`data_pipeline/examples/data_pipeline_demo.c`) simulates a complete pipeline lifecycle:
1. Start all stages.
2. Process each stage (with dummy external calls).
3. Pause and resume the pipeline.
4. Stop the pipeline.

Output:
```
== Data Pipeline Demo ===
Data pipeline started.
Stage 0 started.
...
Stage OUTPUT processing...
Writing image to output.jpg
Uploading data to cloud.
Stage 4 completed.
Data pipeline paused.
...
Data pipeline stopped.
Demo completed.
```

### Build & Run
```bash
cd state_machine_extended/examples/product/system/pipeline
make data_pipeline_demo
./data_pipeline_demo
```

## 2. Task Processing Pipeline

### Overview
The task processing pipeline models a workflow of dependent tasks (e.g., “download → validate → process → notify”). Each task is an EFSM that can carry extended data (parameters, results, priority) and conditionally transition based on that data.

### State Machine Type
**Extended Finite State Machine (EFSM)** (from `efsm_protocol.h`)

- **Task states**: `TASK_PENDING`, `TASK_RUNNING`, `TASK_COMPLETED`, `TASK_FAILED`, `TASK_CANCELLED`
- **Workflow states**: `WORKFLOW_IDLE`, `WORKFLOW_EXECUTING`, `WORKFLOW_FINISHED`
- **Events**: `TASK_SUBMIT`, `TASK_START`, `TASK_FINISH`, `TASK_FAIL`, `TASK_RETRY`

### External System Adapters
- **Message queue**: `mq_adapter_receive_task()`, `mq_adapter_send_result()`
- **REST API**: `rest_api_get_task()`, `rest_api_post_result()`
- **Database**: `db_adapter_store_task()`, `db_adapter_update_status()`

### Example Run
The demo (`task_pipeline/examples/task_pipeline_demo.c`) creates a simple workflow of three tasks and executes them sequentially, simulating external interactions.

Output:
```
== Task Pipeline Demo ===
Workflow created with 3 tasks.
Task 0 submitted.
Task 0 started.
Task 0 completed.
Task 1 started.
Task 1 completed.
Task 2 started.
Task 2 completed.
All tasks finished. Workflow completed.
```

### Build & Run
```bash
cd state_machine_extended/examples/product/system/pipeline
make task_pipeline_demo
./task_pipeline_demo
```

## 3. Parallel Task Pipeline

### Overview
The parallel task pipeline extends the task processing concept by executing each task in its own OS‑level thread, enabling true concurrency. It supports task dependency graphs (DAG) and configurable concurrency limits, making it suitable for workloads where tasks can run independently or with controlled parallelism.

### State Machine Type
**Parallel FSM with OS‑level threading** (built on `fsm_os_adapter.h` and `parallel_fsm.h`)

- **Task states**: `PARALLEL_TASK_IDLE`, `PARALLEL_TASK_READY`, `PARALLEL_TASK_RUNNING`, `PARALLEL_TASK_COMPLETED`, `PARALLEL_TASK_FAILED`, `PARALLEL_TASK_CANCELLED`
- **Workflow states**: `PARALLEL_WORKFLOW_IDLE`, `PARALLEL_WORKFLOW_RUNNING`, `PARALLEL_WORKFLOW_PAUSED`, `PARALLEL_WORKFLOW_FINISHED`
- **Events**: `TASK_INIT`, `TASK_START`, `TASK_COMPLETE`, `TASK_FAIL`, `WORKFLOW_START`, `WORKFLOW_PAUSE`, `WORKFLOW_RESUME`, `WORKFLOW_STOP`

### Key Features
- **Thread‑per‑task mapping**: Each task runs in a dedicated thread created via the OS abstraction layer.
- **Dependency management**: Tasks can declare dependencies on other tasks (DAG). A task only starts when all its dependencies have completed.
- **Concurrency control**: The workflow enforces a maximum number of concurrently running tasks, preventing resource exhaustion.
- **Integration with external systems**: Built‑in adapters for message queues and REST APIs allow seamless integration with existing infrastructure.

### External System Adapters
- **Message queue**: `mq_adapter_send_task_status()` – sends task completion/failure events to a message queue.
- **REST API**: `rest_api_post_workflow_result()` – posts the final workflow result to a REST endpoint.
- **Database**: `db_adapter_store_task_metrics()` – stores execution time, thread ID, and other metrics.

### Example Run
The demo (`parallel_pipeline/examples/parallel_pipeline_demo.c`) creates a workflow of four tasks with dependencies:

```
Task1 ──┐
        ├── Task3 ── Task4
Task2 ──┘
```

The workflow runs with a concurrency limit of 2, meaning Task1 and Task2 run in parallel, Task3 starts after both finish, and Task4 runs after Task3.

Output (abbreviated):
```
== Parallel Pipeline Demo ===
Parallel task 1 initialized
Parallel task 2 initialized
Parallel task 3 initialized
Parallel task 4 initialized
Task 3 depends on task 1
Task 3 depends on task 2
Task 4 depends on task 3
Parallel workflow started
Task 1 started.
Task 2 started.
Task 1 completed
Task 2 completed
Task 3 started.
Task 3 completed
Task 4 started.
Task 4 completed
Workflow completed! Total tasks: 4
```

### Build & Run
```bash
cd state_machine_extended/examples/product/system/pipeline
make parallel_pipeline_demo
./parallel_pipeline_demo
```

## 4. Common Infrastructure

All three pipelines share a similar project layout:

```
pipeline/
├── data_pipeline/
│   ├── include/data_pipeline.h
│   ├── src/data_pipeline.c
│   └── examples/data_pipeline_demo.c
├── task_pipeline/
│   ├── include/task_pipeline.h
│   ├── src/task_pipeline.c
│   └── examples/task_pipeline_demo.c
├── parallel_pipeline/
│   ├── include/parallel_pipeline.h
│   ├── src/parallel_pipeline.c
│   ├── src/parallel_task.c
│   ├── src/parallel_workflow.c
│   ├── examples/parallel_pipeline_demo.c
│   ├── examples/parallel_pipeline_os_example.c
│   └── test/test_parallel_pipeline.c
├── Makefile
└── README.md (this file)
```

The `Makefile` provides targets for building each demo and cleaning up.

## 5. Design Decisions

### Why Parallel FSM for Data Processing?
- Image/audio processing stages are often independent and can run in parallel.
- Parallel FSM allows each stage to be a separate component with its own state, while the master machine coordinates overall pipeline control (start, pause, stop).
- Error handling is simplified: a failing stage can be isolated without stopping the whole pipeline.

### Why EFSM for Task Processing?
- Tasks carry context (parameters, results, dependencies) that influence state transitions.
- EFSM’s extended variables and guard conditions naturally model conditional workflows (e.g., “proceed only if previous task succeeded”).
- The same state machine can be reused for different task types by changing the extended data.

### External System Integration
Each adapter is implemented as a set of function pointers that can be plugged into the pipeline. This keeps the state‑machine logic clean and testable (adapters can be mocked).

## 6. Next Steps / Extensions

- Add real file I/O, network sockets, or database connectors.
- Implement a graphical dashboard that visualizes pipeline state (using the existing `dashboard/` tools).
- Introduce more complex dependency graphs (DAG) for task pipelines.
- Add performance metrics and logging.

## 7. References

- `state_machine_extended/src/parallel_fsm.h` – Parallel FSM implementation.
- `state_machine_extended/src/efsm_protocol.h` – EFSM implementation.
- `state_machine_extended/docs/` – Design analysis and diagrams.

---
*Generated as part of the Design Patterns in C project.*
