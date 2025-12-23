/**
 * viper_pal_osal_hierarchy.c - VIPER, PAL, OSAL, and State Machine Hierarchy Demonstration
 *
 * This program prints the hierarchical relationship between VIPER architecture,
 * Platform Abstraction Layer (PAL), OS Abstraction Layer (OSAL), and State Machines.
 * It does not link with VIPER source code, thus avoiding compilation errors.
 */

#include <stdio.h>
#include <string.h>

/* Simulated component structures */
struct viper_view {
    const char *name;
    const char *fsm_type;
};

struct viper_presenter {
    const char *name;
    const char *fsm_type;
};

struct viper_interactor {
    const char *name;
    const char *fsm_type;
};

struct viper_entity {
    const char *name;
    const char *fsm_type;
};

struct pal_layer {
    const char *platform;
    const char *services[5];
    int service_count;
};

struct osal_layer {
    const char *os;
    const char *services[5];
    int service_count;
};

struct state_machine {
    const char *type;
    const char *description;
};

/* Print hierarchy */
void print_hierarchy(void) {
    printf("==================================================\n");
    printf("VIPER, PAL, OSAL, and State Machine Hierarchy\n");
    printf("==================================================\n\n");

    /* Top layer: Application (VIPER) */
    printf("1. APPLICATION LAYER (VIPER Architecture)\n");
    printf("   └── VIPER Module\n");
    printf("       ├── View (Mealy State Machine)\n");
    printf("       │   ├── States: IDLE, LOADING, DISPLAYING, ERROR\n");
    printf("       │   └── Events: USER_INPUT, REFRESH, ERROR_OCCURRED\n");
    printf("       ├── Presenter (Parallel FSM)\n");
    printf("       │   ├── Coordinates View, Interactor, Entity\n");
    printf("       │   └── Manages event routing\n");
    printf("       ├── Interactor (Moore Hierarchical FSM)\n");
    printf("       │   ├── Business logic execution\n");
    printf("       │   └── State transitions based on business rules\n");
    printf("       ├── Entity (EFSM - Extended Finite State Machine)\n");
    printf("       │   ├── Data model with extended state variables\n");
    printf("       │   └── States: INITIAL, LOADED, MODIFIED, SAVED, ERROR\n");
    printf("       └── Router (Acceptor/Regex FSM)\n");
    printf("           ├── Route matching using regular expressions\n");
    printf("           └── Navigation logic\n");
    printf("\n");

    /* Middle layer: OS Abstraction Layer (OSAL) */
    printf("2. OS ABSTRACTION LAYER (OSAL)\n");
    printf("   ├── Provides uniform OS services across platforms:\n");
    printf("   │   ├── Threads (create, join, yield)\n");
    printf("   │   ├── Mutexes and Semaphores\n");
    printf("   │   ├── Timers (create, start, stop)\n");
    printf("   │   ├── Message Queues\n");
    printf("   │   └── Event Loops\n");
    printf("   ├── Abstracts: POSIX, FreeRTOS, RT‑Thread, BareMetal\n");
    printf("   └── VIPER components call OSAL services via pal_os_adapter\n");
    printf("\n");

    /* Adapter layer: PAL OS Adapter */
    printf("3. PAL OS ADAPTER (Bridge between PAL and OSAL)\n");
    printf("   ├── Converts hardware events to OS events\n");
    printf("   ├── Maps OS services to hardware‑specific operations\n");
    printf("   ├── Creates FSM contexts for hardware‑bound state machines\n");
    printf("   └── Provides unified interface for:\n");
    printf("       ├── pal_os_adapter_init()\n");
    printf("       ├── pal_os_adapter_convert_event()\n");
    printf("       └── pal_os_adapter_create_fsm_context()\n");
    printf("\n");

    /* Bottom layer: Platform Abstraction Layer (PAL) */
    printf("4. PLATFORM ABSTRACTION LAYER (PAL)\n");
    printf("   ├── Abstracts hardware‑specific details:\n");
    printf("   │   ├── GPIO (digital I/O)\n");
    printf("   │   ├── UART (serial communication)\n");
    printf("   │   ├── SPI/I2C (bus communication)\n");
    printf("   │   ├── ADC/DAC (analog conversion)\n");
    printf("   │   └── PWM (pulse‑width modulation)\n");
    printf("   ├── Supported platforms:\n");
    printf("   │   ├── Windows Simulator (SDL2 GUI)\n");
    printf("   │   ├── STM32 (HAL)\n");
    printf("   │   ├── ESP32 (IDF)\n");
    printf("   │   └── Linux (generic)\n");
    printf("   └── VIPER interacts with PAL via pal_* functions\n");
    printf("\n");

    /* State Machine Layer (embedded within each VIPER component) */
    printf("5. STATE MACHINE LAYER (Embedded)\n");
    printf("   ├── Each VIPER component uses a specific FSM type:\n");
    printf("   │   ├── View: Mealy Machine (output depends on state and input)\n");
    printf("   │   ├── Presenter: Parallel FSM (multiple concurrent FSMs)\n");
    printf("   │   ├── Interactor: Moore Hierarchical FSM (hierarchical states)\n");
    printf("   │   ├── Entity: EFSM (extended state variables)\n");
    printf("   │   └── Router: Acceptor/Regex FSM (pattern matching)\n");
    printf("   └── All FSMs are coordinated through OSAL/PAL for timing/hardware\n");
    printf("\n");

    /* Interaction flow */
    printf("6. TYPICAL INTERACTION FLOW (Bottom‑Up)\n");
    printf("   1. Hardware interrupt (button press) detected by PAL\n");
    printf("   2. PAL converts to hardware event, passes to PAL OS adapter\n");
    printf("   3. PAL OS adapter converts to OS event, queues to OSAL\n");
    printf("   4. OSAL delivers event to application (VIPER View)\n");
    printf("   5. View processes event via Mealy FSM, triggers Presenter\n");
    printf("   6. Presenter (Parallel FSM) coordinates Interactor & Entity\n");
    printf("   7. Interactor (Moore FSM) executes business logic\n");
    printf("   8. Entity (EFSM) updates data model, may call OSAL services\n");
    printf("   9. OSAL uses PAL OS adapter to perform hardware operations\n");
    printf("   10. PAL drives actual hardware (LED, display, etc.)\n");
    printf("\n");

    /* Example scenario */
    printf("7. EXAMPLE SCENARIO: Todo App Add Item\n");
    printf("   [View] User clicks 'Add' button → Mealy FSM transitions to LOADING\n");
    printf("   [Presenter] Parallel FSM routes event to Interactor\n");
    printf("   [Interactor] Moore FSM validates input, executes business rule\n");
    printf("   [Entity] EFSM transitions to MODIFIED state, saves data\n");
    printf("   [OSAL] Timer started for auto‑save via pal_os_adapter\n");
    printf("   [PAL] LED blinks to confirm operation\n");
    printf("\n");
}

/* Simulate a simple state machine transition */
void simulate_transition(void) {
    printf("8. SIMULATION OF STATE TRANSITIONS\n");
    const char *states[] = {"IDLE", "LOADING", "DISPLAYING", "ERROR"};
    const char *events[] = {"USER_CLICK", "DATA_RECEIVED", "ERROR", "RESET"};
    
    printf("   Starting state: %s\n", states[0]);
    printf("   Event: %s → State: %s\n", events[0], states[1]);
    printf("   Event: %s → State: %s\n", events[1], states[2]);
    printf("   Event: %s → State: %s\n", events[2], states[3]);
    printf("   Event: %s → State: %s\n", events[3], states[0]);
    printf("\n");
}

int main(void) {
    printf("VIPER/PAL/OSAL Hierarchy Demonstration\n");
    printf("======================================\n\n");
    
    print_hierarchy();
    simulate_transition();
    
    printf("======================================\n");
    printf("Demonstration complete.\n");
    printf("This program illustrates the layered architecture without\n");
    printf("requiring compilation of the VIPER source code.\n");
    
    return 0;
}
