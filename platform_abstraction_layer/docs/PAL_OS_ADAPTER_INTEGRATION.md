# PAL OS Adapter Integration Guide

## Overview

The **PAL OS Adapter** is a bridge between the **Platform Abstraction Layer (PAL)** and the **OS Abstraction Layer** used by the State Machine Extended framework and VIPER architecture. This integration enables hardware-agnostic development of embedded systems with the following benefits:

1. **Platform Independence**: Write state machines and application logic once, run on multiple hardware platforms
2. **Visual Simulation**: Use PAL's Windows/Linux simulator for GUI-based debugging
3. **Hardware Abstraction**: Access GPIO, PWM, ADC, UART, etc. through a unified API
4. **Framework Integration**: Seamlessly work with State Machine Extended and VIPER architecture

## Architecture

```
┌─────────────────────────────────────────────────────────┐
│                  Application Layer                       │
│  ┌─────────────┐  ┌─────────────────┐  ┌─────────────┐  │
│  │   VIPER     │  │ State Machines  │  │ User Code   │  │
│  │ Architecture│  │ (Mealy, Moore,  │  │             │  │
│  │             │  │  EFSM, etc.)    │  │             │  │
│  └──────┬──────┘  └────────┬────────┘  └──────┬──────┘  │
└─────────┼───────────────────┼──────────────────┼─────────┘
          │                   │                  │
┌─────────┼───────────────────┼──────────────────┼─────────┐
│         ▼                   ▼                  ▼         │
│  ┌─────────────────────────────────────────────────────┐ │
│  │              OS Abstraction Layer                   │ │
│  │  (Threads, Mutexes, Queues, Timers, Events)         │ │
│  └──────────────────────────┬──────────────────────────┘ │
└─────────────────────────────┼─────────────────────────────┘
                              │
┌─────────────────────────────┼─────────────────────────────┐
│         ▼                   ▼                  ▼         │
│  ┌─────────────────────────────────────────────────────┐ │
│  │           PAL OS Adapter (This Component)          │ │
│  │  • Maps OS abstractions to PAL hardware            │ │
│  │  • Converts hardware events to OS events           │ │
│  │  • Provides platform-specific implementations      │ │
│  └──────────────────────────┬──────────────────────────┘ │
└─────────────────────────────┼─────────────────────────────┘
                              │
┌─────────────────────────────┼─────────────────────────────┐
│         ▼                   ▼                  ▼         │
│  ┌─────────────────────────────────────────────────────┐ │
│  │        Platform Abstraction Layer (PAL)             │ │
│  │  • Windows Simulator (SDL2 GUI)                    │ │
│  │  • Linux Simulator                                 │ │
│  │  • STM32 HAL Implementation                        │ │
│  │  • ESP32 IDF Implementation                        │ │
│  └──────────────────────────┬──────────────────────────┘ │
└─────────────────────────────┼─────────────────────────────┘
                              │
                    ┌─────────┼─────────┐
                    ▼         ▼         ▼
            ┌─────────┐ ┌─────────┐ ┌─────────┐
            │ Windows │ │  STM32  │ │  ESP32  │
            │   GUI   │ │ Hardware│ │ Hardware│
            └─────────┘ └─────────┘ └─────────┘
```

## Key Components

### 1. PAL OS Adapter Interface (`pal_os_adapter.h`)

The adapter provides:
- **Initialization**: Platform-specific configuration
- **Hardware Event Integration**: GPIO interrupts, timer events, etc.
- **OS Abstraction Mapping**: Threads, mutexes, queues using PAL backend
- **FSM Context Creation**: Ready-to-use state machine contexts

### 2. Implementation (`pal_os_adapter.c`)

Core functionality:
- **GPIO Event Queues**: Convert hardware interrupts to OS queue events
- **Timer Integration**: PAL-based timers for state machines
- **Thread Management**: Platform-agnostic thread creation
- **Event Processing**: Main loop integration for GUI and hardware events

### 3. Example Applications

- **LED State Machine Example**: Control virtual/hardware LEDs using Mealy machines
- **GPIO FSM Example**: Button input with state-based LED control
- **VIPER Integration**: Demonstrates VIPER architecture with PAL hardware
- **Multi-Platform Demo**: Shows code portability across platforms

## Integration with State Machine Extended

### Creating a PAL-based State Machine

```c
#include "pal_os_adapter.h"
#include "fsm_os_adapter.h"
#include "mealy.h"

void example_state_machine(void) {
    // 1. Configure PAL OS adapter
    pal_os_adapter_config_t config = {
        .target_platform = PAL_PLATFORM_WINDOWS_SIMULATOR,
        .enable_gui = true,
        .enable_hardware_events = true,
        .event_queue_size = 10
    };
    
    // 2. Initialize adapter
    pal_os_adapter_init(&config);
    
    // 3. Create FSM context using PAL backend
    fsm_os_context_t *ctx = pal_os_adapter_create_fsm_context(&config);
    
    // 4. Create state machine
    mealy_machine_t *fsm = mealy_machine_create(STATE_COUNT, EVENT_COUNT, ctx);
    
    // 5. Main loop with event processing
    while (pal_os_adapter_process_events() >= 0) {
        // Handle state machine events
        // Process hardware events
        os_delay(10);
    }
    
    // 6. Cleanup
    mealy_machine_destroy(fsm);
    fsm_os_context_destroy(ctx);
    pal_os_adapter_deinit();
}
```

### Hardware Event Handling

```c
// GPIO interrupt to state machine event
os_queue_t *gpio_queue = pal_os_adapter_create_gpio_event_queue(PIN_BUTTON, 5);

// In state machine event handler:
pal_hw_event_t event;
if (os_queue_receive(gpio_queue, &event, 100) == OS_OK) {
    if (event.type == PAL_HW_EVENT_GPIO_INTERRUPT) {
        // Convert to state machine event
        mealy_machine_handle_event(fsm, EVENT_BUTTON_PRESSED);
    }
}
```

## Integration with VIPER Architecture

### VIPER Components with PAL

| Component | PAL Integration | Responsibility |
|-----------|----------------|----------------|
| **View** | PAL GUI (SDL2) | Display hardware state, user interface |
| **Interactor** | PAL GPIO/Peripherals | Hardware control, sensor reading |
| **Presenter** | State Machines | Business logic, state management |
| **Entity** | PAL Hardware State | Data models representing hardware |
| **Router** | PAL Event System | Navigation, event routing |

### Example VIPER-PAL Integration

```c
// View uses PAL GUI
void view_update_led_state(bool led_on) {
    pal_gui_draw_led(led_on ? COLOR_GREEN : COLOR_RED);
}

// Interactor uses PAL GPIO
bool interactor_read_button(void) {
    pal_gpio_state_t state;
    pal_gpio_read(PIN_BUTTON, &state);
    return (state == PAL_GPIO_HIGH);
}

// Presenter uses state machine
void presenter_handle_button_press(void) {
    mealy_machine_handle_event(led_fsm, EVENT_BUTTON_PRESS);
}
```

## Platform Support

### 1. Windows Simulator
- **GUI**: SDL2-based visualization
- **Features**: Simulated GPIO, PWM, ADC, UART
- **Use Case**: Development, debugging, demonstration
- **Dependencies**: SDL2, SDL2_ttf

### 2. Linux Simulator
- **GUI**: Terminal or X11-based
- **Features**: Hardware simulation
- **Use Case**: CI/CD testing, headless operation
- **Dependencies**: X11 libraries (optional)

### 3. STM32 (ARM Cortex-M)
- **Hardware**: Direct register access or HAL
- **Features**: Real-time performance, low-level control
- **Use Case**: Production embedded systems
- **Dependencies**: STM32 HAL, CMSIS

### 4. ESP32 (Xtensa)
- **Hardware**: ESP-IDF framework
- **Features**: WiFi, BLE, dual-core
- **Use Case**: IoT devices, wireless applications
- **Dependencies**: ESP-IDF, FreeRTOS

## Build Configuration

### For Simulation (Windows/Linux)
```bash
cd platform_abstraction_layer/examples
make simulation
./pal_os_adapter_example
```

### For STM32
```bash
make stm32
# Flash to device using OpenOCD or ST-Link
```

### For ESP32
```bash
# Use ESP-IDF build system
idf.py build
idf.py flash
```

## Benefits of PAL OS Adapter Integration

### 1. Development Efficiency
- **Visual Debugging**: See hardware state in GUI during development
- **Rapid Prototyping**: Test logic on PC before deploying to hardware
- **Continuous Testing**: Run tests on simulator in CI/CD pipeline

### 2. Code Quality
- **Platform Independence**: Single codebase for multiple targets
- **Abstraction Layers**: Clear separation of concerns
- **Testability**: Simulated hardware enables unit testing

### 3. Maintenance
- **Consistent API**: Same interface across all platforms
- **Easy Porting**: Switch platforms with configuration change
- **Future Proofing**: Support new hardware by adding PAL implementation

### 4. Team Collaboration
- **Hardware/Software Parallel Development**: Software team can develop without hardware
- **Demonstration**: Show functionality without physical devices
- **Documentation**: Visual representation of system behavior

## Migration Guide

### From Direct Hardware Access to PAL

1. **Identify hardware dependencies** in your state machines
2. **Replace direct register access** with PAL API calls
3. **Update build system** to include PAL and adapter
4. **Test on simulator** before deploying to hardware
5. **Iterate and refine** based on platform-specific requirements

### Example Migration

**Before (STM32-specific):**
```c
// STM32 HAL specific
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
```

**After (PAL-based):**
```c
// Platform independent
pal_gpio_write(PIN_LED, PAL_GPIO_HIGH);
```

## Performance Considerations

### Simulation vs Hardware
| Aspect | Simulation | Hardware |
|--------|------------|----------|
| **Timing Accuracy** | Approximate | Precise |
| **Performance** | CPU-bound | Hardware-accelerated |
| **I/O Latency** | Simulated | Real hardware |
| **Resource Usage** | Higher memory | Optimized for target |

### Optimization Tips
1. **Use `#ifdef` for platform-specific optimizations**
2. **Minimize GUI updates in production builds**
3. **Use event-driven design instead of polling**
4. **Profile on target hardware for final tuning**

## Troubleshooting

### Common Issues

1. **GUI not showing**: Ensure SDL2 is installed and `enable_gui=true`
2. **Hardware events not firing**: Check interrupt configuration and debounce settings
3. **State machine stuck**: Verify event queue handling and timeout values
4. **Build errors**: Check include paths and library dependencies

### Debugging Techniques
- **Enable PAL logging**: Set `enable_logging=true` in configuration
- **Use simulator first**: Debug on PC before moving to hardware
- **Check return values**: All PAL functions return status codes
- **Monitor event queues**: Use `os_queue_count()` to verify event flow

## Future Enhancements

### Planned Features
1. **More Peripheral Support**: I2C, SPI, CAN, Ethernet
2. **Advanced Visualization**: 3D hardware rendering, signal graphs
3. **Cloud Integration**: Remote monitoring and control
4. **AI/ML Integration**: Predictive maintenance, anomaly detection

### Community Contributions
- **New Platform Support**: Raspberry Pi, Arduino, RISC-V
- **Additional Examples**: Complex state machines, real-world applications
- **Tooling Improvements**: IDE plugins, debugger integration

## Conclusion

The PAL OS Adapter provides a powerful bridge between hardware abstraction and software frameworks. By integrating PAL with State Machine Extended and VIPER architecture, developers can:

1. **Accelerate development** with visual simulation
2. **Improve code quality** through platform independence
3. **Reduce time-to-market** with parallel hardware/software development
4. **Enhance maintainability** with clear abstraction layers

This integration represents a best-practice approach to embedded systems development, combining the strengths of hardware abstraction, state machine design patterns, and clean architecture principles.

---

*For more information, see:*
- [PAL Documentation](../README.md)
- [State Machine Extended Documentation](../../state_machine_extended/docs/)
- [VIPER Architecture Documentation](../../viper_architecture/docs/)
- [OS Abstraction Layer Documentation](../../os_abstraction_layer/docs/)
