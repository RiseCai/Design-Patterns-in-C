# System Coordinator Module

This module contains the system coordinator and external command processor for the state machine extended project.

## Overview

The system coordinator manages multiple subsystem FSMs (power, audio, comm, OTA, recording) and coordinates their states. It also processes external commands via a command processor.

## Directory Structure

- `src/` – Core source files
  - `system_coordinator.c/.h`
  - `external_command_processor.c/.h`
  - `external_command_config.c/.h`
- `include/` – Common headers
  - `mycommon.h`
  - `mytrace.h`
  - `mytrace_local.h`
- `tests/` – Unit and integration tests
- `examples/` – Example usage programs
- `docs/` – Design and analysis documents
- `diagrams/` – PlantUML diagrams and generated HTML
- `dashboard/` – Web dashboard for monitoring

## Building

Use CMake or compile directly with gcc:

```bash
cd system_coordinator_module
mkdir build && cd build
cmake ..
make
```

## Usage

See examples in `examples/` for how to integrate the system coordinator.

## Dependencies

- Standard C library
- (Optional) pthread for multithreading

## License

Part of the Design-Patterns-in-C project.
