# OS Abstraction Layer Adapter for Extended State Machines

## Overview

This document describes the adapter layer that integrates the five extended state machines (Mealy UI, EFSM Protocol, Moore Hierarchical, Parallel FSM, Acceptor/Regex) with the portable OS abstraction layer (`os_abstract.h`). The adapter provides a uniform way to use OS services such as threads, mutexes, queues, timers, semaphores, event groups, and memory pools across different operating systems (Linux, FreeRTOS, RT‑Thread, Bare Metal).

## Design Goals

1. **Portability** – State machines can run on any OS supported by the abstraction layer.
2. **Concurrency** – Parallel FSM and other state machines can leverage OS threads and synchronization primitives.
3. **Event‑Driven Architecture** – Mealy UI and EFSM protocol can use OS queues and timers for event handling.
4. **Resource Management** – Moore hierarchical FSM can use memory pools for state allocation.
5. **Backward Compatibility** – Existing code continues to work without the adapter; the adapter is an optional enhancement.

## Adapter API

The adapter is defined in two files:

- `src/fsm_os_adapter.h` – Interface definitions and factory functions.
- `src/fsm_os_adapter.c` – Implementation.

### Key Structures

```c
typedef struct {
    os_thread_t *thread;
    os_queue_t *event_queue;
    os_timer_t *timer;
    os_mutex_t *mutex;
    os_semaphore_t *semaphore;
    os_event_group_t *event_group;
    os_memory_pool_t *memory_pool;
    void *user_data;
} fsm_os_context_t;
```

### Factory Functions

Each state machine type has a dedicated factory that creates a pre‑configured OS context:

- `mealy_ui_adapter_create()` – event queue + timer for debouncing.
- `efsm_protocol_adapter_create()` – larger message queue + timer + semaphore.
- `moore_hierarchical_adapter_create()` – memory pool + mutex.
- `parallel_fsm_adapter_create(int num_components)` – event group + semaphore.
- `acceptor_regex_adapter_create()` – timer for timeout.

### Helper Functions

- `fsm_os_context_create()` / `fsm_os_context_destroy()` – generic context management.
- `fsm_os_send_event()` / `fsm_os_receive_event()` – event queue operations.
- `fsm_os_start_thread()` / `fsm_os_stop_thread()` – thread management.
- `fsm_os_set_timer()` / `fsm_os_cancel_timer()` – timer control.

## Integration Steps

### 1. Include the Adapter Header

Add the following to your state machine source file:

```c
#ifdef USE_OS_ABSTRACTION
#include "fsm_os_adapter.h"
#endif
```

### 2. Choose an Adapter

Decide which adapter suits your state machine and create it during initialization:

```c
struct my_fsm fsm;
fsm_os_context_t *os_ctx = mealy_ui_adapter_create();
if (!os_ctx) {
    /* handle error */
}
```

### 3. Attach the OS Context

Store the OS context in your state machine’s user‑data field (if available) or in a dedicated field.

### 4. Use OS Services

- Send events via `fsm_os_send_event()`.
- Start a dedicated thread with `fsm_os_start_thread()`.
- Set timers with `fsm_os_set_timer()`.
- Synchronize with mutexes/semaphores using the pointers in the context.

### 5. Clean Up

Call `fsm_os_context_destroy()` when the state machine is no longer needed.

## Examples

Five complete examples are provided in `examples/os_adapter/`:

- `mealy_ui_example.c` – Shows event queuing and timer‑based debouncing.
- `efsm_protocol_example.c` – Demonstrates network message processing with retransmission timers.
- `moore_hierarchical_example.c` – Illustrates memory pool allocation and mutex‑protected state transitions.
- `acceptor_regex_example.c` – Shows character‑by‑character pattern matching with timeout.
- `parallel_fsm_example.c` – Demonstrates multi‑threaded component execution and synchronization.

To build all examples, a Makefile is provided in the same directory. Ensure the OS abstraction layer is compiled and linked, then run:

```bash
cd examples/os_adapter
make
```

Alternatively, compile a single example manually:

```bash
gcc -I../os_abstraction_layer/include -I./src -DUSE_OS_ABSTRACTION \
    mealy_ui_example.c src/fsm_os_adapter.c \
    -L../os_abstraction_layer/build -los_abstract -o mealy_ui_example
```

## Configuration

Define `USE_OS_ABSTRACTION` to enable the adapter code. If the macro is not defined, the state machines will work in their original, OS‑agnostic mode.

## Supported OSes

The adapter relies on the underlying OS abstraction layer, which currently provides:

- **POSIX‑compatible** (Linux, macOS) via `posix_compat/`
- **FreeRTOS** (planned)
- **RT‑Thread** (planned)
- **Bare‑metal** (stub implementations)

Refer to the `os_abstraction_layer` documentation for details on porting to a new OS.

## Limitations

- The adapter is optional; existing tests and applications do not require it.
- Thread safety is the responsibility of the user; the adapter provides the tools but does not enforce a particular concurrency model.
- Memory pools are fixed‑size; dynamic resizing is not supported.

## Future Enhancements

1. Add more OS‑specific adapters (FreeRTOS, RT‑Thread).
2. Provide a configuration system to tune queue sizes, stack sizes, etc.
3. Integrate with the existing Excel mapping to auto‑generate adapter code.

## Conclusion

The OS abstraction layer adapter enables the five extended state machines to leverage modern OS services while maintaining portability and backward compatibility. It is a practical step toward making the state‑machine framework suitable for embedded, desktop, and server environments.
