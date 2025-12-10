/**
 * external_command_demo.c  2025-12-09
 * 
 * Demonstration of external command processing with configurable modes.
 * Shows how to integrate external commands with system coordinator FSM.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/external_command_processor.h"
#include "../src/system_coordinator.h"
#include "../src/external_command_config.h"

/* Demo callback function */
static void demo_command_callback(enum command_code cmd, void *data, size_t size, void *user_data)
{
    printf("[Callback] Command received: %s, Data size: %zu\n", 
           external_command_get_code_name(cmd), size);
    
    if (data && size > 0) {
        printf("  Data: ");
        for (size_t i = 0; i < size && i < 16; i++) {
            printf("%02X ", ((uint8_t*)data)[i]);
        }
        printf("\n");
    }
}

/* Demo queue callback function */
static void demo_queue_callback(struct command_message *msg, void *user_data)
{
    printf("[Queue] Processing queued command: %s, Priority: %d\n",
           external_command_get_code_name(msg->cmd), msg->priority);
}

/* Create a sample binary command packet */
static uint8_t* create_binary_command(enum command_code cmd, const uint8_t *data, uint8_t data_len, size_t *packet_size)
{
    static struct command_packet packet;
    
    packet.start_marker = 0xAA;
    packet.command_code = cmd;
    packet.data_length = data_len;
    
    if (data && data_len > 0) {
        memcpy(packet.data, data, data_len);
    }
    
    /* Calculate checksum */
    uint8_t checksum = 0;
    uint8_t *bytes = (uint8_t*)&packet;
    for (size_t i = 0; i < sizeof(struct command_packet) - 1; i++) {
        checksum ^= bytes[i];
    }
    packet.checksum = checksum;
    
    *packet_size = sizeof(struct command_packet);
    return (uint8_t*)&packet;
}

/* Test different processing modes */
static void test_callback_mode(struct external_command_processor *proc)
{
    printf("\n=== Testing Callback Mode ===\n");
    
    /* Configure for callback mode */
    struct external_command_config config;
    external_command_config_init(&config);
    config.mode = CMD_MODE_CALLBACK;
    config.protocol = CMD_PROTOCOL_BINARY;
    config.callback = demo_command_callback;
    config.callback_user_data = NULL;
    
    external_command_processor_configure(proc, &config);
    
    /* Create test commands */
    size_t packet_size;
    uint8_t *packet;
    
    /* Test REC_START command */
    uint8_t rec_data[] = {0x01, 0x02, 0x03};
    packet = create_binary_command(CMD_REC_START, rec_data, sizeof(rec_data), &packet_size);
    external_command_processor_process_raw(proc, packet, packet_size, NULL);
    
    /* Test POWER_ON command */
    packet = create_binary_command(CMD_POWER_ON, NULL, 0, &packet_size);
    external_command_processor_process_raw(proc, packet, packet_size, NULL);
}

static void test_queue_mode(struct external_command_processor *proc)
{
    printf("\n=== Testing Queue Mode ===\n");
    
    /* Configure for queue mode */
    struct external_command_config config;
    external_command_config_init(&config);
    config.mode = CMD_MODE_MESSAGE_QUEUE;
    config.protocol = CMD_PROTOCOL_BINARY;
    config.queue_size = 5;
    config.queue_timeout_ms = 1000;
    config.queue_callback = demo_queue_callback;
    config.queue_user_data = NULL;
    
    external_command_processor_configure(proc, &config);
    
    /* Create multiple commands to fill queue */
    size_t packet_size;
    uint8_t *packet;
    
    for (int i = 0; i < 3; i++) {
        uint8_t data[] = {i, i+1, i+2};
        packet = create_binary_command(CMD_REC_START + i, data, sizeof(data), &packet_size);
        external_command_processor_process_raw(proc, packet, packet_size, NULL);
    }
    
    /* Show queue status */
    printf("Queue size: %d/%d\n", 
           external_command_processor_queue_size(proc),
           external_command_processor_queue_capacity(proc));
    
    /* Process queued messages */
    external_command_processor_flush_queue(proc);
}

static void test_direct_event_mode(struct external_command_processor *proc, struct system_coordinator *sys)
{
    printf("\n=== Testing Direct Event Mode ===\n");
    
    /* Set system coordinator */
    external_command_processor_set_coordinator(proc, sys);
    
    /* Configure for direct event mode */
    struct external_command_config config;
    external_command_config_init(&config);
    config.mode = CMD_MODE_DIRECT_EVENT;
    config.protocol = CMD_PROTOCOL_BINARY;
    
    external_command_processor_configure(proc, &config);
    
    /* Show current system state */
    printf("Current system state: %s\n", 
           system_coordinator_get_state_name(system_coordinator_get_state(sys)));
    
    /* Send POWER_ON command */
    size_t packet_size;
    uint8_t *packet = create_binary_command(CMD_POWER_ON, NULL, 0, &packet_size);
    external_command_processor_process_raw(proc, packet, packet_size, NULL);
    
    printf("After POWER_ON command, system state: %s\n",
           system_coordinator_get_state_name(system_coordinator_get_state(sys)));
    
    /* Send REC_START command */
    uint8_t rec_data[] = {0x01};
    packet = create_binary_command(CMD_REC_START, rec_data, sizeof(rec_data), &packet_size);
    external_command_processor_process_raw(proc, packet, packet_size, NULL);
    
    printf("After REC_START command, system state: %s\n",
           system_coordinator_get_state_name(system_coordinator_get_state(sys)));
    
    /* Send STREAMING_START command */
    uint8_t stream_start_data[] = {0x02, 0x03};
    packet = create_binary_command(CMD_STREAMING_START, stream_start_data, sizeof(stream_start_data), &packet_size);
    external_command_processor_process_raw(proc, packet, packet_size, NULL);
    
    printf("After STREAMING_START command, system state: %s\n",
           system_coordinator_get_state_name(system_coordinator_get_state(sys)));
    
    /* Send STREAMING_STOP command */
    uint8_t stream_stop_data[] = {0x04};
    packet = create_binary_command(CMD_STREAMING_STOP, stream_stop_data, sizeof(stream_stop_data), &packet_size);
    external_command_processor_process_raw(proc, packet, packet_size, NULL);
    
    printf("After STREAMING_STOP command, system state: %s\n",
           system_coordinator_get_state_name(system_coordinator_get_state(sys)));
}

static void test_hybrid_mode(struct external_command_processor *proc)
{
    printf("\n=== Testing Hybrid Mode ===\n");
    
    /* Configure for hybrid mode */
    struct external_command_config config;
    external_command_config_init(&config);
    config.mode = CMD_MODE_HYBRID;
    config.protocol = CMD_PROTOCOL_BINARY;
    config.callback = demo_command_callback;
    config.callback_user_data = NULL;
    config.queue_size = 5;
    config.queue_callback = demo_queue_callback;
    config.queue_user_data = NULL;
    config.hybrid_threshold = CMD_PRIORITY_HIGH;
    
    external_command_processor_configure(proc, &config);
    
    /* Test high priority command (should use callback) */
    printf("Sending high priority command (CRITICAL)...\n");
    size_t packet_size;
    uint8_t *packet = create_binary_command(CMD_POWER_ON, NULL, 0, &packet_size);
    external_command_processor_process_raw(proc, packet, packet_size, NULL);
    
    /* Test low priority command (should use queue) */
    printf("Sending low priority command (NORMAL)...\n");
    uint8_t data[] = {0x01, 0x02};
    packet = create_binary_command(CMD_SET_VOLUME, data, sizeof(data), &packet_size);
    external_command_processor_process_raw(proc, packet, packet_size, NULL);
    
    /* Show queue status */
    printf("Queue size after low priority command: %d/%d\n",
           external_command_processor_queue_size(proc),
           external_command_processor_queue_capacity(proc));
    
    /* Process queued messages */
    external_command_processor_flush_queue(proc);
}

static void test_mode_switching(struct external_command_processor *proc)
{
    printf("\n=== Testing Mode Switching ===\n");
    
    /* Start in callback mode */
    struct external_command_config config;
    external_command_config_init(&config);
    config.mode = CMD_MODE_CALLBACK;
    config.protocol = CMD_PROTOCOL_BINARY;
    config.callback = demo_command_callback;
    
    external_command_processor_configure(proc, &config);
    printf("Initial mode: %s\n", external_command_get_mode_name(external_command_processor_get_mode(proc)));
    
    /* Switch to queue mode */
    external_command_processor_set_mode(proc, CMD_MODE_MESSAGE_QUEUE);
    printf("After switch to: %s\n", external_command_get_mode_name(external_command_processor_get_mode(proc)));
    
    /* Switch to direct event mode */
    external_command_processor_set_mode(proc, CMD_MODE_DIRECT_EVENT);
    printf("After switch to: %s\n", external_command_get_mode_name(external_command_processor_get_mode(proc)));
    
    /* Switch back to callback mode */
    external_command_processor_set_mode(proc, CMD_MODE_CALLBACK);
    printf("After switch to: %s\n", external_command_get_mode_name(external_command_processor_get_mode(proc)));
}

static void show_statistics(struct external_command_processor *proc)
{
    printf("\n=== Statistics ===\n");
    
    uint32_t received, processed, failed;
    int queue_usage;
    
    external_command_processor_get_statistics(proc, &received, &processed, &failed, &queue_usage);
    
    printf("Commands received: %u\n", received);
    printf("Commands processed: %u\n", processed);
    printf("Commands failed: %u\n", failed);
    printf("Queue usage: %d\n", queue_usage);
    printf("Success rate: %.1f%%\n", received > 0 ? (processed * 100.0 / received) : 0.0);
}

int main(void)
{
    printf("External Command Processor Demo\n");
    printf("===============================\n");
    
    /* Create command processor */
    struct external_command_processor *proc = external_command_processor_create();
    if (!proc) {
        printf("Failed to create command processor\n");
        return 1;
    }
    
    /* Create system coordinator for testing */
    struct system_coordinator sys;
    system_coordinator_init(&sys);
    
    /* Test different modes */
    test_callback_mode(proc);
    test_queue_mode(proc);
    test_direct_event_mode(proc, &sys);
    test_hybrid_mode(proc);
    test_mode_switching(proc);
    
    /* Show final statistics */
    show_statistics(proc);
    
    /* Show processor status */
    printf("\n=== Final Status ===\n");
    printf("%s\n", external_command_processor_get_status(proc));
    
    /* Cleanup */
    external_command_processor_destroy(proc);
    system_coordinator_destroy(&sys);
    
    printf("\nDemo completed successfully.\n");
    return 0;
}
