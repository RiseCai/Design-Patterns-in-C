/**
 * zero_copy_demo.c  2026-01-06
 *
 * Copyright (C) 2000-2026 All Right Reserved
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Zero-Copy Data Stream Demo
 * Demonstrates zero-copy data transfer capabilities.
 */

#include "data_stream.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

/* ============================================================================
 * DEMO DATA STRUCTURES
 * ============================================================================ */

struct sensor_data {
    int sensor_id;
    float temperature;
    float humidity;
    timestamp_t timestamp;
    char sensor_name[32];
};

/* ============================================================================
 * ZERO-COPY CONSUMER TASKS
 * ============================================================================ */

/* Consumer task 1: Temperature Monitor */
void *temperature_monitor(void *arg)
{
    data_stream_t stream = (data_stream_t)arg;
    int monitor_id = 1;

    printf("[Temperature Monitor %d] Starting zero-copy monitoring\n", monitor_id);

    while (1) {
        struct data_read_result result;

        /* Use zero-copy read */
        int status = data_stream_read_zero_copy(stream, &result, 2000); // 2 second timeout

        if (status == DATA_STREAM_SUCCESS) {
            /* Access data directly (ZERO-COPY) */
            struct sensor_data *sensor = (struct sensor_data *)result.ref.data;

            printf("[Temperature Monitor %d] Zero-copy read - Sensor %d: %.1f°C (ref_id=%u)\n",
                   monitor_id, sensor->sensor_id, sensor->temperature, result.ref.ref_id);

            /* Process data without copying */
            if (sensor->temperature > 30.0f) {
                printf("[Temperature Monitor %d] ALERT: High temperature detected!\n", monitor_id);
            }

            /* Release the reference when done */
            data_stream_release_reference(stream, result.ref.ref_id);

            /* Small delay between reads */
            usleep(500000); // 500ms
        } else if (status == DATA_STREAM_ERROR_TIMEOUT) {
            printf("[Temperature Monitor %d] Read timeout - assuming producer finished\n", monitor_id);
            break;
        } else if (status == DATA_STREAM_ERROR_ALREADY_CLOSED) {
            printf("[Temperature Monitor %d] Stream closed\n", monitor_id);
            break;
        } else {
            printf("[Temperature Monitor %d] Read error: %d\n", monitor_id, status);
            break;
        }
    }

    printf("[Temperature Monitor %d] Stopping\n", monitor_id);
    return NULL;
}

/* Consumer task 2: Humidity Monitor */
void *humidity_monitor(void *arg)
{
    data_stream_t stream = (data_stream_t)arg;
    int monitor_id = 2;

    printf("[Humidity Monitor %d] Starting zero-copy monitoring\n", monitor_id);

    while (1) {
        struct data_read_result result;

        /* Use zero-copy read */
        int status = data_stream_read_zero_copy(stream, &result, 2000); // 2 second timeout

        if (status == DATA_STREAM_SUCCESS) {
            /* Access data directly (ZERO-COPY) */
            struct sensor_data *sensor = (struct sensor_data *)result.ref.data;

            printf("[Humidity Monitor %d] Zero-copy read - Sensor %d: %.1f%% humidity (ref_id=%u)\n",
                   monitor_id, sensor->sensor_id, sensor->humidity, result.ref.ref_id);

            /* Process data without copying */
            if (sensor->humidity > 80.0f) {
                printf("[Humidity Monitor %d] ALERT: High humidity detected!\n", monitor_id);
            }

            /* Release the reference when done */
            data_stream_release_reference(stream, result.ref.ref_id);

            /* Small delay between reads */
            usleep(700000); // 700ms
        } else if (status == DATA_STREAM_ERROR_TIMEOUT) {
            printf("[Humidity Monitor %d] Read timeout - assuming producer finished\n", monitor_id);
            break;
        } else if (status == DATA_STREAM_ERROR_ALREADY_CLOSED) {
            printf("[Humidity Monitor %d] Stream closed\n", monitor_id);
            break;
        } else {
            printf("[Humidity Monitor %d] Read error: %d\n", monitor_id, status);
            break;
        }
    }

    printf("[Humidity Monitor %d] Stopping\n", monitor_id);
    return NULL;
}

/* ============================================================================
 * PRODUCER TASK
 * ============================================================================ */

void *sensor_producer(void *arg)
{
    data_stream_t stream = (data_stream_t)arg;

    printf("[Sensor Producer] Starting zero-copy data production\n");

    /* Sample sensor data */
    struct sensor_data sensors[3] = {
        {1, 25.5f, 65.0f, 0, "Living Room"},
        {2, 28.2f, 72.0f, 0, "Kitchen"},
        {3, 22.8f, 58.0f, 0, "Bedroom"}
    };

    for (int i = 0; i < 10; i++) { // Produce 10 readings
        /* Select a random sensor */
        int sensor_idx = rand() % 3;
        struct sensor_data *sensor = &sensors[sensor_idx];

        /* Add some variation */
        sensor->temperature += ((rand() % 200) - 100) * 0.1f; // ±10°C variation
        sensor->humidity += ((rand() % 100) - 50) * 0.1f; // ±5% variation
        sensor->timestamp = os_get_timestamp();

        printf("[Sensor Producer] Writing sensor data for %s (%.1f°C, %.1f%%)\n",
               sensor->sensor_name, sensor->temperature, sensor->humidity);

        /* Use zero-copy write */
        int status = data_stream_write_zero_copy(stream, sensor, sizeof(struct sensor_data), 1000);

        if (status != DATA_STREAM_SUCCESS) {
            printf("[Sensor Producer] Write failed: %d\n", status);
            break;
        }

        /* Wait before next reading */
        sleep(1);
    }

    printf("[Sensor Producer] Finished producing data\n");

    /* Signal end of production by destroying the stream */
    printf("[Sensor Producer] Destroying stream to signal consumers\n");
    data_stream_destroy(stream);

    printf("[Sensor Producer] Producer thread exiting\n");

    return NULL;
}

/* ============================================================================
 * COMPARISON DEMO: COPY vs ZERO-COPY
 * ============================================================================ */

void compare_copy_vs_zero_copy()
{
    printf("\n=== Performance Comparison: Copy vs Zero-Copy ===\n");
    printf("Note: Performance test skipped in demo to avoid complexity\n");
    printf("Zero-copy benefits are more apparent with large data structures\n");
    printf("and multiple concurrent consumers in real-world scenarios.\n");
}

/* ============================================================================
 * MAIN DEMO
 * ============================================================================ */

int main(int argc, char *argv[])
{
    printf("=== Zero-Copy Data Stream Demo ===\n");
    printf("Demonstrating zero-copy data transfer capabilities\n\n");

    /* Seed random number generator */
    srand((unsigned int)time(NULL));

    /* Create zero-copy broadcast stream */
    struct data_stream_config config = {
        .name = "sensor_broadcast",
        .element_size = sizeof(struct sensor_data),
        .capacity = 8,  // Small buffer for demo
        .mode = STREAM_MODE_BROADCAST
    };

    data_stream_t sensor_stream = data_stream_create(&config);
    if (!sensor_stream) {
        printf("Failed to create sensor stream\n");
        return 1;
    }

    printf("[Setup] Created broadcast stream for sensor data\n");

    /* Register multiple consumers */
    data_stream_add_consumer(sensor_stream, 1); // Temperature monitor
    data_stream_add_consumer(sensor_stream, 2); // Humidity monitor

    printf("[Setup] Registered 2 consumers for zero-copy access\n");

    /* Create consumer threads */
    pthread_t temp_thread, humidity_thread, producer_thread;

    printf("[Setup] Starting consumer threads...\n");

    pthread_create(&temp_thread, NULL, temperature_monitor, sensor_stream);
    pthread_create(&humidity_thread, NULL, humidity_monitor, sensor_stream);

    /* Give consumers time to start */
    usleep(500000); // 500ms

    /* Start producer */
    printf("[Setup] Starting producer thread...\n");
    pthread_create(&producer_thread, NULL, sensor_producer, sensor_stream);

    /* Wait for producer to finish */
    pthread_join(producer_thread, NULL);

    /* Wait for consumers to finish */
    printf("[Cleanup] Waiting for consumers to finish...\n");
    sleep(3); // Give consumers time to detect stream closure

    /* Join consumer threads */
    pthread_join(temp_thread, NULL);
    pthread_join(humidity_thread, NULL);

    /* Run performance comparison */
    compare_copy_vs_zero_copy();

    /* Note: Stream was already destroyed by producer thread */
    printf("[Cleanup] Stream was already destroyed by producer\n");

    printf("\n=== Zero-Copy Demo Completed ===\n");
    printf("Key benefits of zero-copy:\n");
    printf("• Eliminates memory copy overhead\n");
    printf("• Reduces CPU cache pollution\n");
    printf("• Enables efficient multi-consumer scenarios\n");
    printf("• Provides direct memory access with reference counting\n");

    return 0;
}
