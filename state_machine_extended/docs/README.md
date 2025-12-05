# Extended State Machine Types

This directory contains extensions to the base state pattern, implementing five specialized state machine types as per the requirement.

## 1. Mealy Event-Driven FSM (UI Control)

- **Location**: `mealy_machine.h`, `mealy_machine.c`, `mealy_ui.h`, `mealy_ui.c`
- **Description**: Output depends on both current state and input event. Suitable for UI interfaces where user actions (click, hover, keypress) trigger state transitions and immediate UI updates.
- **Example**: Button with idle, hovered, pressed states.

## 2. EFSM + Three-Stage FSM (Communication Protocol)

- **Location**: `efsm_protocol.h`
- **Description**: Extended Finite State Machine with internal variables (sequence number, timeout). Three-stage processing: input, process, output. Used for protocol handling like TCP/IP state machines.

## 3. Moore Hierarchical FSM (Embedded Control)

- **Location**: `moore_hierarchical.h`, `moore_hierarchical_complex.c`
- **Description**: Moore machine where output depends only on state. Supports hierarchical nesting (superstates and substates). Ideal for embedded systems with layered control logic.
- **Complex Example**: Smart thermostat with states Off, On (superstate) with substates Heating, Cooling, Idle, FanOnly. Demonstrates event handling, hierarchical transitions, and Moore outputs.

## 4. Parallel State Machine (Concurrent Systems)

- **Location**: `parallel_fsm.h`
- **Description**: Multiple independent FSMs running concurrently, with synchronization mechanisms. Useful for multi‑threaded or multi‑component systems.

## 5. Acceptor FSM (Regex Matching)

- **Location**: `acceptor_regex.h`
- **Description**: Deterministic finite automaton that accepts or rejects input strings. Can be used for pattern matching, lexical analysis, etc.

## Configuration Mapping

- `state_machine_types.csv` – Excel‑compatible table mapping scenarios to FSM types, with technical attributes.
- `fsm_config.json` – Machine‑readable mapping of FSM types to implementation files and their states/events.

## How to Use

Each FSM type is designed as a drop‑in module following the same design conventions as the original state pattern (struct‑based, ops table, trace macros). To integrate:

1. Include the corresponding header.
2. Initialize the machine with an initial state.
3. Dispatch events via the provided API.
4. Implement concrete states by defining ops functions.

Refer to `mealy_ui.c` for a complete working example.

## Design Compliance

All extensions adhere to the existing design specification:
- Use of `mycommon.h` and `mytrace.h` for tracing and common macros.
- Opaque structs with function‑pointer operations.
- Memory management consistent with the base pattern (malloc/free).
- No global variables; context passed explicitly.

## Next Steps

- Implement concrete states for each FSM type as needed.
- Add unit tests in the `test.c` style.
- Extend the configuration mapping to auto‑generate code skeletons.
