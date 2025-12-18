/**
 * moore_hierarchical_example.c - Example of using OS abstraction layer with Moore Hierarchical state machine.
 *
 * This example demonstrates how to integrate the Moore hierarchical state machine
 * with OS memory pools for state allocation and mutexes for thread-safe transitions.
 */

#include <stdio.h>
#include <stdlib.h>
#include "../../src/moore_hierarchical.h"
#include "../../src/fsm_os_adapter.h"

/* Custom data for hierarchical states */
typedef struct {
    int depth;
    char name[32];
} hierarchical_data_t;

/* Thread that periodically triggers state transitions */
static void transition_thread(void *arg)
{
    struct moore_hsm *hfsm = (struct moore_hsm *)arg;
    printf("[Transition Thread] Started\n");

    for (int i = 0; i < 5; i++) {
        printf("[Transition Thread] Triggering event %d\n", i);
        moore_hsm_dispatch_event(hfsm, i % 3 + 1); /* some event (EV_POWER_ON etc.) */
        os_thread_sleep(300);
    }

    printf("[Transition Thread] Finished\n");
}

int main(void)
{
    printf("=== Moore Hierarchical OS Adapter Example ===\n");

    if (os_abstract_init() != OS_OK) {
        fprintf(stderr, "Failed to initialize OS abstraction layer\n");
        return EXIT_FAILURE;
    }

    /* Create OS adapter for Moore hierarchical FSM */
    fsm_os_context_t *os_ctx = moore_hierarchical_adapter_create();
    if (!os_ctx) {
        fprintf(stderr, "Failed to create OS adapter\n");
        os_abstract_deinit();
        return EXIT_FAILURE;
    }

    /* Create the hierarchical state machine */
    struct moore_hsm hfsm;
    /* Use the root state defined in moore_hierarchical.c (state_off) */
    extern struct moore_state state_off;
    moore_hsm_init(&hfsm, &state_off);

    /* Attach the OS context (e.g., for memory pool) */
    /* hfsm doesn't have user_data field, we can store it elsewhere if needed */

    /* Use the memory pool to allocate a state (if needed) */
    if (os_ctx->memory_pool) {
        hierarchical_data_t *data = os_memory_pool_alloc(os_ctx->memory_pool, 100);
        if (data) {
            data->depth = 1;
            snprintf(data->name, sizeof(data->name), "RootState");
            printf("[Main] Allocated state data from pool: %s\n", data->name);
            /* In a real scenario, you would attach this to the state machine */
            os_memory_pool_free(os_ctx->memory_pool, data);
        }
    }

    /* Start a thread that triggers transitions */
    if (fsm_os_start_thread(os_ctx, transition_thread, &hfsm) != OS_OK) {
        fprintf(stderr, "Failed to start transition thread\n");
        fsm_os_context_destroy(os_ctx);
        os_abstract_deinit();
        return EXIT_FAILURE;
    }

    /* Use a mutex to protect state changes (simulated) */
    if (os_ctx->mutex) {
        os_mutex_lock(os_ctx->mutex, 100);
        printf("[Main] Mutex locked, performing critical section\n");
        /* Execute some state machine action */
        moore_hsm_dispatch_event(&hfsm, 1); /* EV_POWER_ON */
        os_mutex_unlock(os_ctx->mutex);
    }

    /* Wait for the thread to finish */
    os_thread_sleep(2000);

    /* Clean up */
    printf("[Main] Cleaning up...\n");
    fsm_os_stop_thread(os_ctx, 1000);
    fsm_os_context_destroy(os_ctx);
    os_abstract_deinit();

    printf("=== Example finished ===\n");
    return EXIT_SUCCESS;
}
