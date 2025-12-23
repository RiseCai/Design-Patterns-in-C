# VIPER Architecture Implementation with State Machines

This project implements the VIPER architecture pattern using the 5 types of state machines available in the state_machine_extended project.

## Architecture Overview

VIPER (View, Interactor, Presenter, Entity, Router) is an architectural pattern that separates concerns into distinct layers. This implementation maps each VIPER component to an appropriate state machine type:

### Component to State Machine Mapping

| VIPER Component | State Machine Type | Purpose |
|----------------|-------------------|---------|
| **View** | Mealy Event-Driven | Handles user interface events and updates |
| **Presenter** | Parallel FSM | Coordinates multiple components and manages data flow |
| **Interactor** | Moore Hierarchical | Implements business logic with hierarchical states |
| **Entity** | EFSM (Extended FSM) | Manages data models with extended variables |
| **Router** | Acceptor FSM | Handles navigation and routing patterns |

## Directory Structure

```
viper_architecture/
├── include/           # Public headers
│   ├── viper.h       # Main VIPER interface
│   ├── viper_view.h  # View component interface
│   ├── viper_presenter.h
│   ├── viper_interactor.h
│   ├── viper_entity.h
│   └── viper_router.h
├── src/              # Implementation files
│   ├── viper_view.c
│   ├── viper_presenter.c
│   ├── viper_interactor.c
│   ├── viper_entity.c
│   └── viper_router.c
├── examples/         # Example applications
│   └── todo_app/     # Todo application example
└── docs/             # Documentation
```

## Data Flow

The VIPER data flow is implemented using state machine events:

1. **User Action** → View FSM (Mealy) receives input
2. **View Event** → Presenter FSM (Parallel) coordinates response
3. **Presenter Request** → Interactor FSM (Moore) processes business logic
4. **Interactor Operation** → Entity FSM (EFSM) updates data model
5. **Navigation Need** → Router FSM (Acceptor) handles routing

## Benefits of State Machine Implementation

1. **Formal State Management**: Each component has well-defined states
2. **Event-Driven Architecture**: Clear event handling between components
3. **Testability**: State machines are easily testable with defined inputs/outputs
4. **Maintainability**: Clear state transitions and separation of concerns
5. **Reusability**: State machine components can be reused across applications

## Building

```bash
cd viper_architecture
mkdir build && cd build
cmake ..
make
```

## Example Usage

See the `examples/todo_app` directory for a complete implementation of a todo application using the VIPER architecture with state machines.

## Dependencies

- Existing state machine implementations from `state_machine_extended/`
- Standard C library
- (Optional) CMake for building

## License

Part of the Design-Patterns-in-C project.
