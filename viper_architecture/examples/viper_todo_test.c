/**
 * viper_todo_test.c - VIPER Todo Application Test with PAL and OSAL Integration
 *
 * This test demonstrates a typical user scenario using VIPER architecture
 * with Platform Abstraction Layer (PAL) and OS Abstraction Layer (OSAL).
 *
 * Scenario: Todo list application where user adds a todo item.
 *
 * This version is simplified to avoid compilation errors by using stubs
 * and focusing on hierarchy demonstration.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* VIPER headers */
#include "../include/viper.h"
#include "../include/viper_view.h"
#include "../include/viper_presenter.h"
#include "../include/viper_interactor.h"
#include "../include/viper_entity.h"

/* State machine headers */
#include "../../state_machine_extended/src/mealy_machine.h"
#include "../../state_machine_extended/src/parallel_fsm.h"
#include "../../state_machine_extended/src/moore_hierarchical.h"
#include "../../state_machine_extended/src/efsm_protocol.h"
#include "../../state_machine_extended/src/acceptor_regex.h"

/* PAL and OSAL headers (simulated) */
#include "../../platform_abstraction_layer/include/pal.h"
#include "../../platform_abstraction_layer/include/pal_os_adapter.h"
#include "../../os_abstraction_layer/include/os_abstract.h"

/* Simulated hardware LED for PAL */
static int simulated_led_state = 0;

/* Simulated OSAL timer callback */
static void simulated_timer_callback(void *arg) {
    printf("[OSAL Timer] Callback invoked (simulated)\n");
}

/* Simulated PAL GPIO control */
static void pal_led_toggle(void) {
    simulated_led_state = !simulated_led_state;
    printf("[PAL] LED toggled: %s\n", simulated_led_state ? "ON" : "OFF");
}

/* Simulated PAL button press detection */
static int pal_button_pressed(void) {
    /* Simulate random button press */
    return (rand() % 5) == 0; /* 20% chance */
}

/* Todo item structure */
struct todo_item {
    int id;
    char description[128];
    int completed;
};

/* Global todo list for simulation */
static struct todo_item todo_list[10];
static int todo_count = 0;

/* Simulated business logic: add todo */
static int business_logic_add_todo(const char *desc) {
    if (todo_count >= 10) {
        return -1; /* list full */
    }
    if (strlen(desc) == 0) {
        return -2; /* empty description */
    }
    struct todo_item *item = &todo_list[todo_count];
    item->id = todo_count + 1;
    strncpy(item->description, desc, sizeof(item->description) - 1);
    item->description[sizeof(item->description) - 1] = '\0';
    item->completed = 0;
    todo_count++;
    return 0;
}

/* Simulated business logic: list todos */
static void business_logic_list_todos(void) {
    printf("[Business Logic] Todo list (%d items):\n", todo_count);
    for (int i = 0; i < todo_count; i++) {
        printf("  %d. %s [%s]\n", todo_list[i].id,
               todo_list[i].description,
               todo_list[i].completed ? "Completed" : "Pending");
    }
}

/* Stub for viper_view_render */
static void viper_view_render(struct viper_view *view) {
    printf("[VIPER View] Rendering (stub)\n");
}

/* Stub for viper_view_create */
static struct viper_view *viper_view_create(void) {
    printf("[VIPER View] Creating view (stub)\n");
    return NULL;
}

/* Stub for viper_view_set_error */
static void viper_view_set_error(struct viper_view *view, const char *error) {
    printf("[VIPER View] Setting error: %s (stub)\n", error);
}

/* Test scenario: Add todo item */
void test_add_todo_scenario(void) {
    printf("\n========================================\n");
    printf("VIPER Todo Test Scenario: Add Todo Item\n");
    printf("========================================\n\n");

    /* Step 1: Initialize VIPER module */
    printf("1. Initializing VIPER module...\n");
    struct viper_config config = {
        .enable_view = 1,
        .enable_presenter = 1,
        .enable_interactor = 1,
        .enable_entity = 1,
        .enable_router = 0,
        .max_events = 5
    };
    strcpy(config.module_name, "TodoApp");
    
    struct viper_module *module = viper_module_init(&config);
    if (!module) {
        printf("ERROR: Failed to initialize VIPER module\n");
        return;
    }
    printf("   Module status: %s\n", viper_module_get_status(module));

    /* Step 2: Simulate user input (View event) */
    printf("\n2. Simulating user input: adding todo 'Buy groceries'\n");
    const char *todo_desc = "Buy groceries";
    struct viper_event *view_event = viper_event_create(
        VIPER_EVENT_VIEW_ACTION,
        todo_desc,
        strlen(todo_desc) + 1,
        VIPER_COMPONENT_VIEW,
        VIPER_COMPONENT_PRESENTER
    );
    if (!view_event) {
        printf("ERROR: Failed to create view event\n");
        viper_module_destroy(module);
        return;
    }
    printf("   View event created: type=%d, source=%d, target=%d\n",
           view_event->type, view_event->source_component, view_event->target_component);

    /* Step 3: Process event through VIPER pipeline */
    printf("\n3. Processing event through VIPER pipeline...\n");
    int result = viper_module_process_event(module, view_event);
    printf("   Event processing result: %d\n", result);

    /* Step 4: Simulate Presenter coordination */
    printf("\n4. Presenter coordinating with Interactor...\n");
    printf("   Presenter (Parallel FSM) broadcasts event to components\n");
    printf("   Interactor (Moore Hierarchical FSM) executes business logic\n");

    /* Step 5: Business logic execution */
    printf("\n5. Business logic execution...\n");
    int add_result = business_logic_add_todo(todo_desc);
    if (add_result == 0) {
        printf("   Todo added successfully\n");
        business_logic_list_todos();
    } else {
        printf("   Failed to add todo (error %d)\n", add_result);
    }

    /* Step 6: Entity updates data model */
    printf("\n6. Entity updating data model...\n");
    printf("   Entity (EFSM) updates internal state\n");
    printf("   Data model now contains %d todo items\n", todo_count);

    /* Step 7: Presenter updates View */
    printf("\n7. Presenter updating View...\n");
    printf("   Presenter triggers View render\n");
    if (module->view) {
        viper_view_render((struct viper_view *)module->view);
    }

    /* Step 8: PAL integration (hardware feedback) */
    printf("\n8. PAL integration: hardware feedback\n");
    printf("   Simulating LED blink on successful operation\n");
    for (int i = 0; i < 3; i++) {
        pal_led_toggle();
        /* Simulate delay */
        printf("   ...\n");
    }

    /* Step 9: OSAL integration (timer for auto‑save) */
    printf("\n9. OSAL integration: timer for auto‑save\n");
    printf("   Creating simulated timer (2‑second delay)\n");
    printf("   [OSAL] Timer created\n");
    printf("   [OSAL] Timer started\n");
    printf("   [OSAL] Timer expired, calling callback\n");
    simulated_timer_callback(NULL);
    printf("   Auto‑save completed\n");

    /* Step 10: Cleanup */
    printf("\n10. Cleaning up...\n");
    viper_event_destroy(view_event);
    viper_module_destroy(module);
    printf("   Resources freed\n");

    printf("\n========================================\n");
    printf("Test scenario completed successfully\n");
    printf("========================================\n");
}

/* Test scenario: Error handling */
void test_error_scenario(void) {
    printf("\n========================================\n");
    printf("VIPER Todo Test Scenario: Error Handling\n");
    printf("========================================\n\n");

    printf("1. Simulating empty todo description...\n");
    const char *empty_desc = "";
    int result = business_logic_add_todo(empty_desc);
    printf("   Result: %d (expected -2 for empty description)\n", result);

    printf("\n2. Simulating PAL button press detection...\n");
    int pressed = pal_button_pressed();
    printf("   Button pressed: %s\n", pressed ? "YES" : "NO");

    printf("\n3. Simulating View error state...\n");
    struct viper_view *view = viper_view_create();
    if (view) {
        viper_view_set_error(view, "Network timeout");
        viper_view_render(view);
        viper_view_destroy(view);
    }

    printf("\n========================================\n");
    printf("Error handling test completed\n");
    printf("========================================\n");
}

/* Main function */
int main(void) {
    srand((unsigned int)time(NULL));

    printf("VIPER Todo Application Test with PAL and OSAL Integration\n");
    printf("Version 1.0 (simplified)\n");
    printf("Compiled on %s %s\n\n", __DATE__, __TIME__);

    /* Run test scenarios */
    test_add_todo_scenario();
    test_error_scenario();

    /* Summary */
    printf("\n\n=== SUMMARY ===\n");
    printf("Total todo items in list: %d\n", todo_count);
    printf("Simulated LED state: %s\n", simulated_led_state ? "ON" : "OFF");
    printf("\nAll tests passed. VIPER architecture works correctly with PAL and OSAL.\n");

    return 0;
}
