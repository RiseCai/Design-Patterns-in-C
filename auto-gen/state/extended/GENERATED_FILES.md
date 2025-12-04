# Generated Files for Extended State Machine Types

This directory contains the following generated files for the five extended state machine types.

## 1. Mealy UI State Machine
- `mealy_ui.c` – Implementation (includes entry/do/exit actions)
- `mealy_ui.h` – Header
- `mealy_ui_diagram.puml` – Basic PlantUML diagram
- `mealy_ui_diagram_with_api.puml` – Diagram with API mapping (including entry/do/exit)
- `mealy_ui_diagram.html` – HTML view of basic diagram
- `mealy_ui_diagram_with_api.html` – HTML view with API mapping

## 2. EFSM Protocol State Machine
- `efsm_protocol.c` – Implementation (includes three‑stage entry/do/exit processing)
- `efsm_protocol.h` – Header
- `efsm_protocol_diagram.puml` – Basic PlantUML diagram
- `efsm_protocol_diagram_with_api.puml` – Diagram with API mapping (including entry/do/exit)
- `efsm_protocol_diagram.html` – HTML view of basic diagram
- `efsm_protocol_diagram_with_api.html` – HTML view with API mapping

## 3. Moore Hierarchical State Machine
- `moore_hierarchical.c` – Implementation (includes entry/do/exit actions per state)
- `moore_hierarchical.h` – Header
- `moore_hierarchical_complex.c` – Complex example
- `moore_hsm_state_diagram.puml` – Basic hierarchical diagram
- `moore_hsm_detailed.puml` – Detailed diagram
- `moore_hsm_state_diagram_with_api.puml` – Diagram with API mapping (including entry/do/exit)
- `moore_hsm_diagram.html` – HTML view of basic diagram
- `moore_hsm_detailed.html` – HTML view of detailed diagram
- `moore_hsm_state_diagram_with_api.html` – HTML view with API mapping
- `test_moore_complex.c` – Test program

## 4. Parallel State Machine
- `parallel_fsm.c` – Implementation (includes entry/do/exit actions per component)
- `parallel_fsm.h` – Header
- `parallel_fsm_diagram.puml` – Basic PlantUML diagram
- `parallel_fsm_diagram_with_api.puml` – Diagram with API mapping (including entry/do/exit)
- `parallel_fsm_diagram.html` – HTML view of basic diagram
- `parallel_fsm_diagram_with_api.html` – HTML view with API mapping

## 5. Acceptor FSM (Regular Expression)
- `acceptor_regex.c` – Implementation (includes entry/do/exit actions per state)
- `acceptor_regex.h` – Header
- `acceptor_regex_diagram.puml` – Basic PlantUML diagram
- `acceptor_regex_diagram_with_api.puml` – Diagram with API mapping (including entry/do/exit)
- `acceptor_regex_diagram.html` – HTML view of basic diagram
- `acceptor_regex_diagram_with_api.html` – HTML view with API mapping

## Mapping and Configuration Files
- `state_machine_mapping.csv` – Basic mapping table (includes entry/do/exit actions)
- `state_machine_mapping_requirements.csv` – Detailed mapping with priority levels (includes entry/do/exit columns)
- `fsm_config_mapping.json` – JSON bidirectional mapping (includes entry/do/exit actions)
- `fsm_config.json` – General configuration

## Documentation
- `README.md` – Original README
- `README_EXTENDED.md` – Extended documentation with mapping tables
- `GENERATED_FILES.md` – This file

## How to Use
1. View diagrams by opening the `.html` files in a browser.
2. Inspect code in the `.c` and `.h` files.
3. Modify diagrams by editing the `.puml` files.
4. Use mapping tables to understand relationships between scenarios and code.

## Notes
All files are generated according to the design specification for five state machine types:
1. UI Interface Control – Mealy event‑driven
2. Communication Protocol – EFSM + three‑stage
3. Embedded Control – Moore + hierarchical
4. Concurrent System – Parallel FSM
5. Regular Matching – Acceptor FSM

The bidirectional mapping ensures traceability between diagrams and code.
