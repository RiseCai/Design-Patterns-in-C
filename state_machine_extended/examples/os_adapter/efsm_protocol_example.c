/**
 * efsm_protocol_example.c - Example of using OS abstraction layer with EFSM Protocol state machine.
 *
 * This example demonstrates how to integrate the EFSM protocol state machine
 * with OS queues for message passing and timers for retransmission.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../src/efsm_protocol.h"
#include "../../src/fsm_os_adapter.h"

/* Simulated network message */
typedef struct {
    uint8_t data[256];
    size_t len;
} network_message_t;

/* Thread that receives network messages and pushes them to the queue */
static void network_receiver_thread(void *arg)
{
    fsm_os_context_t *os_ctx = (fsm_os_context_t *)arg;
    network_message_t msg;

    printf("[Network Receiver] Thread started\n");

    while (1) {
        /* Simulate receiving a message from the network */
        memset(msg.data, 0, sizeof(msg.data));
        msg.len = snprintf((char *)msg.data, sizeof(msg.data), "Message %ld", (long)os_timer_get_tick());
        printf("[Network Receiver] Received: %s\n", msg.data);

        /* Send to the protocol queue */
        os_error_t err = fsm_os_send_event(os_ctx, &msg, sizeof(msg), 100);
        if (err != OS_OK) {
            printf("[Network Receiver] Send failed: %d\n", err);
        }

        os_thread_sleep(500); /* simulate network delay */
    }
}

/* Timer callback for retransmission */
static void retransmit_timer_callback(void *arg)
{
    struct efsm_processor *proc = (struct efsm_processor *)arg;
    printf("[Timer] Retransmission timeout, resending last packet\n");
    /* In a real implementation, you would trigger retransmission here */
}

int main(void)
{
    printf("=== EFSM Protocol OS Adapter Example ===\n");

    if (os_abstract_init() != OS_OK) {
        fprintf(stderr, "Failed to initialize OS abstraction layer\n");
        return EXIT_FAILURE;
    }

    /* Create the EFSM protocol state machine */
    struct efsm_processor proc;
    efsm_processor_init(&proc);

    /* Create OS adapter for EFSM protocol */
    fsm_os_context_t *os_ctx = efsm_protocol_adapter_create();
    if (!os_ctx) {
        fprintf(stderr, "Failed to create OS adapter\n");
        os_abstract_deinit();
        return EXIT_FAILURE;
    }

    /* Store OS context in the processor's user data (if available) */
    /* Note: efsm_processor doesn't have a user_data field; we can store it elsewhere.
       For demonstration, we just keep it separate.
     */

    /* Start a thread that simulates network reception */
    if (fsm_os_start_thread(os_ctx, network_receiver_thread, os_ctx) != OS_OK) {
        fprintf(stderr, "Failed to start network receiver thread\n");
        fsm_os_context_destroy(os_ctx);
        os_abstract_deinit();
        return EXIT_FAILURE;
    }

    /* Set up a retransmission timer (periodic, 1000 ms) */
    os_timer_callback_t timer_cb = retransmit_timer_callback;
    os_ctx->timer = os_timer_create(timer_cb, &proc, "retransmit_timer");
    if (!os_ctx->timer) {
        fprintf(stderr, "Failed to create retransmission timer\n");
    } else {
        os_timer_start(os_ctx->timer, 1000, true); /* periodic */
    }

    /* Main loop: process messages from the queue */
    printf("[Main] Waiting for messages...\n");
    for (int i = 0; i < 5; i++) {
        network_message_t msg;
        os_error_t err = fsm_os_receive_event(os_ctx, &msg, sizeof(msg), 2000);
        if (err == OS_OK) {
            printf("[Main] Processing message: %s\n", msg.data);
            /* Feed the message to the protocol state machine */
            efsm_processor_dispatch_packet(&proc, msg.data);
        } else {
            printf("[Main] No message received (timeout)\n");
        }
    }

    /* Clean up */
    printf("[Main] Cleaning up...\n");
    fsm_os_stop_thread(os_ctx, 1000);
    fsm_os_context_destroy(os_ctx);
    os_abstract_deinit();

    printf("=== Example finished ===\n");
    return EXIT_SUCCESS;
}
