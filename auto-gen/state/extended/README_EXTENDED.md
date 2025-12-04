# Extended State Machine Types

This directory contains implementations and diagrams for five extended state machine types, as per the requirement.

## 1. Mealy UI State Machine (UI Interface Control)
- **Type**: Mealy event‑driven
- **States**: Idle, Hovered, Pressed (each with entry/do/exit sub‑states)
- **Events**: hover, click, keypress
- **Output**: Actions on transitions (button hover effect, visual feedback)
- **Entry/Do/Exit Actions**: Each state has entry, do, and exit actions (e.g., `idle_entry`, `idle_do`, `idle_exit`)
- **Code**: `mealy_ui.c`, `mealy_ui.h`
- **Diagram**: `mealy_ui_diagram.puml` (basic) and `mealy_ui_diagram_with_api.puml` (with API mapping)
- **HTML**: `mealy_ui_diagram.html`, `mealy_ui_diagram_with_api.html`
- **Bidirectional mapping**: Yes – diagram labels correspond to C functions (`idle_event_hover`, `hovered_event_click`, etc.) and entry/do/exit actions are mapped.

## 2. EFSM Protocol State Machine (Communication Protocol)
- **Type**: Extended Finite State Machine + three‑stage processing
- **States**: Idle, Receiving, Processing, Sending, WaitingAck (each with entry/do/exit sub‑states)
- **Events**: packet_arrival, packet_complete, valid, invalid, sent, ack_received, timeout
- **Extended variables**: `sequence_number`, `timeout_counter`
- **Entry/Do/Exit Actions**: Three‑stage processing (entry, do, exit) is integral to EFSM design.
- **Code**: `efsm_protocol.c`, `efsm_protocol.h`
- **Diagram**: `efsm_protocol_diagram.puml` (basic) and `efsm_protocol_diagram_with_api.puml` (with API mapping)
- **HTML**: `efsm_protocol_diagram.html`, `efsm_protocol_diagram_with_api.html`
- **Bidirectional mapping**: Variables and three‑stage processing are reflected in diagram notes; entry/do/exit actions are mapped.

## 3. Moore Hierarchical State Machine (Embedded Control)
- **Type**: Moore + hierarchical
- **States**: Off, On (with substates Idle, Running, Error) – each with entry/do/exit sub‑states
- **Events**: power_on, start, stop, error, reset
- **Output**: Actions on state entry (e.g., “LED on” when entering On)
- **Entry/Do/Exit Actions**: Each state has entry, do, and exit actions (e.g., `on_entry`, `on_do`, `on_exit`)
- **Code**: `moore_hierarchical.c`, `moore_hierarchical.h`
- **Diagram**: `moore_hsm_state_diagram.puml`, `moore_hsm_detailed.puml`, `moore_hsm_state_diagram_with_api.puml` (with API mapping)
- **HTML**: `moore_hsm_diagram.html`, `moore_hsm_detailed.html`, `moore_hsm_state_diagram_with_api.html`
- **Bidirectional mapping**: Hierarchical nesting matches code structure; entry/do/exit actions are mapped.

## 4. Parallel State Machine (Concurrent System)
- **Type**: Parallel FSM
- **Components**: Three independent FSMs (Component A, B, C) – each component state has entry/do/exit sub‑states
- **Events**: Broadcast events (start, data_ready, finish, enable, etc.)
- **Synchronization**: `parallel_machine_broadcast_event`
- **Entry/Do/Exit Actions**: Each component state has entry, do, and exit actions (e.g., `A_Idle_entry`, `A_Idle_do`, `A_Idle_exit`)
- **Code**: `parallel_fsm.c`, `parallel_fsm.h`
- **Diagram**: `parallel_fsm_diagram.puml` (basic) and `parallel_fsm_diagram_with_api.puml` (with API mapping)
- **HTML**: `parallel_fsm_diagram.html`, `parallel_fsm_diagram_with_api.html`
- **Bidirectional mapping**: Diagram shows parallel regions corresponding to `parallel_component` structs; API calls mapped to transitions; entry/do/exit actions are mapped.

## 5. Acceptor FSM (Regular Expression Matching)
- **Type**: Acceptor (recognizer) FSM
- **Example regex**: `ab*`
- **States**: q0 (initial), q1 (accepting), reject – each with entry/do/exit sub‑states
- **Events**: characters 'a', 'b'
- **Output**: Accept/reject decision
- **Entry/Do/Exit Actions**: Each state has entry, do, and exit actions (e.g., `q0_entry`, `q0_do`, `q0_exit`)
- **Code**: `acceptor_regex.c`, `acceptor_regex.h`
- **Diagram**: `acceptor_regex_diagram.puml` (basic) and `acceptor_regex_diagram_with_api.puml` (with API mapping)
- **HTML**: `acceptor_regex_diagram.html`, `acceptor_regex_diagram_with_api.html`
- **Bidirectional mapping**: Accepting states match `is_accepting` flag; transitions match `transitions` array; API calls mapped to each character input; entry/do/exit actions are mapped.

## Mapping Tables

### CSV Mapping Table
A CSV file `state_machine_mapping.csv` provides a quick reference for each scenario, including:

- Scenario
- Recommended FSM type
- State machine name
- Number of states and events
- Output type
- Extended variables (if any)
- Entry/Do/Exit Actions
- Code file
- API function prefix
- Bidirectional mapping support

### Requirements Mapping Table
A CSV file `state_machine_mapping_requirements.csv` provides a more detailed mapping with priority levels (1-5) and key features, including entry/do/exit action columns.

### Bidirectional Mapping Table
A CSV file `state_machine_bidirectional_mapping.csv` provides explicit mapping directions (Type->Code and Code->Type) for each scenario, with priority and implementation status.

### JSON Configuration Mapping
A JSON file `fsm_config_mapping.json` provides a complete bidirectional mapping between state machine types and code files, including API functions, state structs, entry/do/exit actions, and priority levels.

## How to Use

1. **View diagrams**: Open the `.html` files in a browser to see interactive state diagrams.
2. **Inspect code**: Each `.c` file contains a fully compilable implementation.
3. **Modify**: Adjust the PlantUML source (`.puml`) to reflect changes in the code.
4. **Extend**: Use the mapping tables as a template for adding new state machine types.
5. **Prioritize**: Use the mapping level (1-5) to decide which state machines to focus on for your project.

## Notes

- All diagrams are generated with PlantUML and can be edited with any PlantUML editor.
- The bidirectional mapping ensures that diagram elements (states, transitions, outputs) have direct counterparts in the C code, making the design traceable and maintainable.
- The Excel‑style CSV can be imported into spreadsheet software for further analysis.
- The JSON configuration can be used by automated tools to generate code or documentation.
