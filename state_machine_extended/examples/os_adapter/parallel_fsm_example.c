/**
 * parallel_fsm_example.c - Example of using OS abstraction layer with Parallel FSM.
 *
 * This example demonstrates how to run multiple component FSMs in separate
 * threads and synchronize them using OS abstraction layer primitives.
 */

#include <stdio.h>
#include <stdlib.h>
#include "../../src/parallel_fsm.h"
#include "../../src/fsm_os_adapter.h"

/* Component‑specific data */
typedef struct {
    int id;
    int counter;
} component_data_t;

/* Thread entry for a single component */
static void component_thread(void *arg)
{
    struct parallel_component *comp = (struct parallel_component *)arg;
    component_data_t *data = (component_data_t *)comp->state;

    printf("[Component %d] Thread started\n", data->id);

    /* Simulate some work */
    for (int i = 0; i < 3; i++) {
        printf("[Component %d] Step %d\n", data->id, i);
        comp->do_action(comp);
        os_thread_sleep(100 * (data->id + 1)); /* different delay per component */
    }

    printf("[Component %d] Thread finished\n", data->id);
}

int main(void)
{
    printf("=== Parallel FSM OS Adapter Example ===\n");

    if (os_abstract_init() != OS_OK) {
        fprintf(stderr, "Failed to initialize OS abstraction layer\n");
        return EXIT_FAILURE;
    }

    /* Create a parallel machine with capacity 3 */
    struct parallel_machine pm;
    parallel_machine_init(&pm, 3);

    /* Create OS adapter for parallel FSM (3 components) */
    fsm_os_context_t *os_ctx = parallel_fsm_adapter_create(3);
    if (!os_ctx) {
        fprintf(stderr, "Failed to create OS adapter\n");
        os_abstract_deinit();
        return EXIT_FAILURE;
    }

    /* Create three component data instances */
    component_data_t data1 = { .id = 1, .counter = 0 };
    component_data_t data2 = { .id = 2, .counter = 0 };
    component_data_t data3 = { .id = 3, .counter = 0 };

    /* Create components using the existing factory (simplified) */
    struct parallel_component *comp1 = parallel_component_create(
        1, &data1, component_a_step, component_a_entry, component_a_do, component_a_exit);
    struct parallel_component *comp2 = parallel_component_create(
        2, &data2, component_b_step, component_b_entry, component_b_do, component_b_exit);
    struct parallel_component *comp3 = parallel_component_create(
        3, &data3, component_c_step, component_c_entry, component_c_do, component_c_exit);

    if (!comp1 || !comp2 || !comp3) {
        fprintf(stderr, "Failed to create components\n");
        fsm_os_context_destroy(os_ctx);
        os_abstract_deinit();
        return EXIT_FAILURE;
    }

    /* Add components to the parallel machine */
    parallel_machine_add_component(&pm, comp1);
    parallel_machine_add_component(&pm, comp2);
    parallel_machine_add_component(&pm, comp3);

    /* Start a thread for each component */
    for (int i = 0; i < pm.count; i++) {
        struct parallel_component *comp = pm.components[i];
        /* Each component gets its own OS context in a real scenario,
         * but for simplicity we reuse the same adapter.
         */
        fsm_os_context_t *comp_ctx = fsm_os_context_create();
        if (!comp_ctx) {
            fprintf(stderr, "Failed to create OS context for component %d\n", i);
            continue;
        }
        fsm_os_start_thread(comp_ctx, component_thread, comp);
        /* Store the OS context in the component's state? Not implemented here. */
        fsm_os_context_destroy(comp_ctx); /* just for demonstration */
    }

    /* Broadcast an event to all components */
    printf("[Main] Broadcasting START event\n");
    parallel_machine_broadcast_event(&pm, EVENT_START);

    /* Wait for threads to finish (simulated) */
    os_thread_sleep(2000);

    /* Synchronize components */
    printf("[Main] Synchronizing components\n");
    parallel_machine_sync(&pm);

    /* Clean up */
    printf("[Main] Cleaning up...\n");
    fsm_os_context_destroy(os_ctx);
    os_abstract_deinit();

    printf("=== Example finished ===\n");
    return EXIT_SUCCESS;
}
