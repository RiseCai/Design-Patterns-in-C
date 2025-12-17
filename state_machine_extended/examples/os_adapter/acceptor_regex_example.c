/**
 * acceptor_regex_example.c - Example of using OS abstraction layer with Acceptor/Regex state machine.
 *
 * This example demonstrates how to integrate the Acceptor/Regex state machine
 * with OS timers for pattern matching timeout and queues for character input.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../src/acceptor_regex.h"
#include "../../src/fsm_os_adapter.h"

/* Thread that feeds characters to the acceptor */
static void input_feeder_thread(void *arg)
{
    fsm_os_context_t *os_ctx = (fsm_os_context_t *)arg;
    const char *pattern = "abc";
    int idx = 0;

    printf("[Input Feeder] Thread started, pattern='%s'\n", pattern);

    while (pattern[idx] != '\0') {
        char ch = pattern[idx];
        printf("[Input Feeder] Sending character '%c'\n", ch);
        os_error_t err = fsm_os_send_event(os_ctx, &ch, sizeof(ch), 100);
        if (err != OS_OK) {
            printf("[Input Feeder] Send failed: %d\n", err);
        }
        idx++;
        os_thread_sleep(200); /* simulate typing delay */
    }

    /* Send termination character */
    char term = '\0';
    fsm_os_send_event(os_ctx, &term, sizeof(term), 100);
    printf("[Input Feeder] Finished\n");
}

/* Timer callback for matching timeout */
static void timeout_callback(void *arg)
{
    struct acceptor_machine *acceptor = (struct acceptor_machine *)arg;
    printf("[Timer] Pattern matching timeout, resetting acceptor\n");
    /* No reset function in API; we can reinitialize */
    acceptor->current = acceptor->start;
}

int main(void)
{
    printf("=== Acceptor/Regex OS Adapter Example ===\n");

    if (os_abstract_init() != OS_OK) {
        fprintf(stderr, "Failed to initialize OS abstraction layer\n");
        return EXIT_FAILURE;
    }

    /* Create OS adapter for Acceptor/Regex */
    fsm_os_context_t *os_ctx = acceptor_regex_adapter_create();
    if (!os_ctx) {
        fprintf(stderr, "Failed to create OS adapter\n");
        os_abstract_deinit();
        return EXIT_FAILURE;
    }

    /* Create the acceptor state machine */
    struct acceptor_machine acceptor;
    /* For simplicity, we don't build a full state graph; just initialize with NULL */
    acceptor_machine_init(&acceptor, NULL); /* In reality, you would build states */

    /* Attach OS context */
    /* acceptor doesn't have user_data; we can store elsewhere */

    /* Start a thread that feeds characters */
    if (fsm_os_start_thread(os_ctx, input_feeder_thread, os_ctx) != OS_OK) {
        fprintf(stderr, "Failed to start input feeder thread\n");
        fsm_os_context_destroy(os_ctx);
        os_abstract_deinit();
        return EXIT_FAILURE;
    }

    /* Set up a timeout timer (one-shot, 5000 ms) */
    os_timer_callback_t timer_cb = timeout_callback;
    os_ctx->timer = os_timer_create(timer_cb, &acceptor, "timeout_timer");
    if (!os_ctx->timer) {
        fprintf(stderr, "Failed to create timeout timer\n");
    } else {
        os_timer_start(os_ctx->timer, 5000, false); /* one-shot */
    }

    /* Main loop: receive characters and feed them to the acceptor */
    printf("[Main] Waiting for characters...\n");
    while (1) {
        char ch;
        os_error_t err = fsm_os_receive_event(os_ctx, &ch, sizeof(ch), 3000);
        if (err != OS_OK) {
            printf("[Main] No more characters (timeout)\n");
            break;
        }
        if (ch == '\0') {
            printf("[Main] Received termination character\n");
            break;
        }
        printf("[Main] Processing character '%c'\n", ch);
        acceptor_regex_feed(&acceptor, ch);
        if (acceptor_regex_matched(&acceptor)) {
            printf("[Main] Pattern matched!\n");
            break;
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
