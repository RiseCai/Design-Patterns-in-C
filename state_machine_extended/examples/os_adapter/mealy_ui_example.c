/**
 * mealy_ui_example.c - Example of using OS abstraction layer with Mealy UI state machine.
 *
 * This example demonstrates how to integrate the Mealy UI state machine
 * with the OS abstraction layer for event queuing and timer‑based debouncing.
 */

#include <stdio.h>
#include <stdlib.h>
#include "../../src/mealy_ui.h"
#include "../../src/fsm_os_adapter.h"

/* Custom user data for the UI context */
typedef struct {
    int click_count;
    int hover_count;
} ui_context_t;

/* Thread entry function that processes events from the queue */
static void ui_event_thread(void *arg)
{
    struct mealy_machine *machine = (struct mealy_machine *)arg;
    fsm_os_context_t *os_ctx = machine->_user_data; /* stored in user_data */
    int event;

    printf("[UI Thread] Started.\n");

    while (1) {
        /* Wait for an event from the queue (blocking) */
        os_error_t err = fsm_os_receive_event(os_ctx, &event, sizeof(event), OS_WAIT_FOREVER);
        if (err != OS_OK) {
            printf("[UI Thread] Failed to receive event, error=%d\n", err);
            break;
        }

        /* Process the event */
        switch (event) {
            case 1: /* click */
                mealy_machine_event_click(machine);
                break;
            case 2: /* hover */
                mealy_machine_event_hover(machine);
                break;
            default:
                printf("[UI Thread] Unknown event %d\n", event);
        }
    }
}

/* Timer callback for debouncing */
static void debounce_timer_callback(void *arg)
{
    struct mealy_machine *machine = (struct mealy_machine *)arg;
    printf("[Timer] Debounce timer expired, processing pending click.\n");
    /* In a real implementation, you would process a pending click here */
}

int main(void)
{
    printf("=== Mealy UI OS Adapter Example ===\n");

    /* Initialize the OS abstraction layer */
    if (os_abstract_init() != OS_OK) {
        fprintf(stderr, "Failed to initialize OS abstraction layer\n");
        return EXIT_FAILURE;
    }

    /* Create the Mealy UI state machine */
    struct mealy_machine ui_machine;
    struct mealy_state_idle idle_state;
    mealy_state_idle_init(&idle_state);
    mealy_machine_init(&ui_machine, (struct mealy_state *)&idle_state);

    /* Create the OS adapter for Mealy UI */
    fsm_os_context_t *os_ctx = mealy_ui_adapter_create();
    if (!os_ctx) {
        fprintf(stderr, "Failed to create OS adapter\n");
        os_abstract_deinit();
        return EXIT_FAILURE;
    }

    /* Store the OS context in the machine's user_data for easy access */
    ui_machine._user_data = os_ctx;

    /* Start a thread to process UI events */
    if (fsm_os_start_thread(os_ctx, ui_event_thread, &ui_machine) != OS_OK) {
        fprintf(stderr, "Failed to start UI event thread\n");
        fsm_os_context_destroy(os_ctx);
        os_abstract_deinit();
        return EXIT_FAILURE;
    }

    /* Set up a timer for debouncing (one‑shot, 200 ms) */
    os_timer_callback_t timer_cb = debounce_timer_callback;
    os_ctx->timer = os_timer_create(timer_cb, &ui_machine, "debounce_timer");
    if (!os_ctx->timer) {
        fprintf(stderr, "Failed to create debounce timer\n");
    } else {
        os_timer_start(os_ctx->timer, 200, false); /* one‑shot */
    }

    /* Simulate some UI events by sending them to the queue */
    for (int i = 0; i < 5; i++) {
        int event = (i % 2 == 0) ? 1 : 2; /* alternate click and hover */
        printf("[Main] Sending event %d\n", event);
        os_error_t err = fsm_os_send_event(os_ctx, &event, sizeof(event), 100);
        if (err != OS_OK) {
            printf("[Main] Send failed: %d\n", err);
        }
        os_thread_sleep(100); /* wait a bit between events */
    }

    /* Let the thread process events for a while */
    os_thread_sleep(1000);

    /* Clean up */
    printf("[Main] Cleaning up...\n");
    fsm_os_stop_thread(os_ctx, 1000);
    fsm_os_context_destroy(os_ctx);
    os_abstract_deinit();

    printf("=== Example finished ===\n");
    return EXIT_SUCCESS;
}
