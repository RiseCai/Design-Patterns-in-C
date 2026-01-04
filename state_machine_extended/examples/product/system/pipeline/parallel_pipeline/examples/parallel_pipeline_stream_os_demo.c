/**
 * parallel_pipeline_stream_os_demo.c  2026-01-05
 *
 * Copyright (C) 2000-2026 All Right Reserved
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Demo program showing hybrid workflow-data stream functionality.
 * Demonstrates the parallel pipeline with data stream integration.
 */

#include "parallel_pipeline.h"
#include "data_stream.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

/* Data structures for data flow */
struct sensor_data {
    int sensor_id;
    float temperature;
    float humidity;
    timestamp_t timestamp;
};

struct processed_data {
    int sensor_id;
    float avg_temperature;
    float avg_humidity;
    int sample_count;
    char status[32];
};

struct alert_data {
    int sensor_id;
    char alert_type[32];
    float value;
    timestamp_t timestamp;
};

/* Global data streams */
static data_stream_t sensor_stream;
static data_stream_t processed_stream;
static data_stream_t alert_stream;

/* Task functions */

/* Sensor task - produces sensor data */
void sensor_task(void *arg)
{
    int sensor_id = *(int *)arg;

    TRACE_INFO("Sensor %d: Starting data collection", sensor_id);

    /* Simulate collecting sensor data */
    for (int i = 0; i < 5; i++) {
        struct sensor_data data = {
            .sensor_id = sensor_id,
            .temperature = 20.0f + (rand() % 100) / 10.0f,
            .humidity = 40.0f + (rand() % 400) / 10.0f,
            .timestamp = os_get_timestamp()
        };

        /* Write to sensor data stream */
        int rc = data_stream_write(sensor_stream, &data, sizeof(data), 1000);
        if (rc == DATA_STREAM_SUCCESS) {
            TRACE_INFO("Sensor %d: Sent data (temp=%.1f, hum=%.1f)",
                      sensor_id, data.temperature, data.humidity);
        } else {
            TRACE_ERROR("Sensor %d: Failed to write sensor data, rc=%d", sensor_id, rc);
        }

        sleep(1); /* Simulate sensor reading interval */
    }

    TRACE_INFO("Sensor %d: Data collection complete", sensor_id);
}

/* Processing task - consumes sensor data, produces processed data */
void processing_task(void *arg)
{
    int processor_id = *(int *)arg;

    TRACE_INFO("Processor %d: Starting data processing", processor_id);

    struct processed_data processed = {0};
    processed.sensor_id = processor_id;

    /* Process sensor data from stream */
    for (int i = 0; i < 5; i++) {
        struct sensor_data sensor_data;

        /* Read from sensor stream */
        int rc = data_stream_read(sensor_stream, &sensor_data, sizeof(sensor_data), 2000);
        if (rc == DATA_STREAM_SUCCESS) {
            /* Process the data */
            processed.avg_temperature = (processed.avg_temperature * processed.sample_count + sensor_data.temperature) / (processed.sample_count + 1);
            processed.avg_humidity = (processed.avg_humidity * processed.sample_count + sensor_data.humidity) / (processed.sample_count + 1);
            processed.sample_count++;

            TRACE_INFO("Processor %d: Processed sensor %d data (temp=%.1f, hum=%.1f)",
                      processor_id, sensor_data.sensor_id, sensor_data.temperature, sensor_data.humidity);

            /* Check for alerts */
            if (sensor_data.temperature > 28.0f) {
                struct alert_data alert = {
                    .sensor_id = sensor_data.sensor_id,
                    .value = sensor_data.temperature,
                    .timestamp = os_get_timestamp()
                };
                strcpy(alert.alert_type, "HIGH_TEMPERATURE");

                int alert_rc = data_stream_write(alert_stream, &alert, sizeof(alert), 1000);
                if (alert_rc == DATA_STREAM_SUCCESS) {
                    printf("[ALERT] Processor %d: High temperature alert sent (%.1f°C)\n",
                           processor_id, sensor_data.temperature);
                }
            }
        } else {
            TRACE_ERROR("Processor %d: Failed to read sensor data, rc=%d", processor_id, rc);
        }
    }

    /* Determine status */
    if (processed.avg_temperature > 25.0f) {
        strcpy(processed.status, "HOT");
    } else if (processed.avg_temperature < 15.0f) {
        strcpy(processed.status, "COLD");
    } else {
        strcpy(processed.status, "NORMAL");
    }

    /* Write processed data to output stream */
    int rc = data_stream_write(processed_stream, &processed, sizeof(processed), 1000);
    if (rc == DATA_STREAM_SUCCESS) {
        TRACE_INFO("Processor %d: Sent processed data (avg_temp=%.1f, status=%s)",
                  processor_id, processed.avg_temperature, processed.status);
    } else {
        TRACE_ERROR("Processor %d: Failed to write processed data, rc=%d", processor_id, rc);
    }

    TRACE_INFO("Processor %d: Data processing complete", processor_id);
}

/* Alert task - consumes alert data */
void alert_task(void *arg)
{
    int alert_handler_id = *(int *)arg;

    TRACE_INFO("Alert Handler %d: Starting alert monitoring", alert_handler_id);

    /* Simple completion - just finish immediately */
    TRACE_INFO("Alert Handler %d: Alert monitoring complete (immediate completion)", alert_handler_id);
}

/* Decision task - consumes processed data and makes decisions */
void decision_task(void *arg)
{
    int decision_maker_id = *(int *)arg;

    TRACE_INFO("Decision Maker %d: Starting decision making", decision_maker_id);

    /* Read processed data and make decisions */
    struct processed_data processed;
    int rc = data_stream_read(processed_stream, &processed, sizeof(processed), 5000);

    if (rc == DATA_STREAM_SUCCESS) {
        TRACE_INFO("Decision Maker %d: Received processed data from sensor %d",
                  decision_maker_id, processed.sensor_id);
        TRACE_INFO("Decision Maker %d: Average temperature: %.1f°C, Status: %s",
                  decision_maker_id, processed.avg_temperature, processed.status);

        /* Make decisions based on processed data */
        if (strcmp(processed.status, "HOT") == 0) {
            TRACE_INFO("Decision Maker %d: DECISION - Activate cooling system", decision_maker_id);
        } else if (strcmp(processed.status, "COLD") == 0) {
            TRACE_INFO("Decision Maker %d: DECISION - Activate heating system", decision_maker_id);
        } else {
            TRACE_INFO("Decision Maker %d: DECISION - Maintain current conditions", decision_maker_id);
        }
    } else {
        TRACE_ERROR("Decision Maker %d: Failed to read processed data, rc=%d", decision_maker_id, rc);
    }

    TRACE_INFO("Decision Maker %d: Decision making complete", decision_maker_id);
}

/* Callbacks */
void on_workflow_complete(struct parallel_workflow *pw)
{
    TRACE_INFO("=== WORKFLOW COMPLETED ===");
    TRACE_INFO("Total tasks: %d", pw->task_count);
    TRACE_INFO("Final state: %d", pw->state);
}

void on_task_complete(struct parallel_workflow *pw, int task_id)
{
    TRACE_INFO("Task %d completed in hybrid workflow", task_id);
}

/* Monitor callback - simplified version to avoid incomplete type issues */
void monitor_callback(void *user_data)
{
    (void)user_data;
    TRACE_INFO("=== WORKFLOW STATUS UPDATE ===");
    TRACE_INFO("Monitor callback triggered");
}

/* Main demo */
int main(int argc, char *argv[])
{
    printf("=== Parallel Pipeline Stream OS Demo ===\n");
    printf("Demonstrating hybrid workflow-data stream functionality\n\n");

    /* Initialize random seed */
    srand((unsigned int)time(NULL));

    /* Create data streams */
    printf("[Setup] Creating data streams...\n");

    struct data_stream_config sensor_config = {
        .name = "sensor_data",
        .element_size = sizeof(struct sensor_data),
        .capacity = 10,
        .mode = STREAM_MODE_FIFO,
        .window_size = 0
    };
    sensor_stream = data_stream_create(&sensor_config);
    if (!sensor_stream) {
        fprintf(stderr, "Failed to create sensor data stream\n");
        return 1;
    }

    struct data_stream_config processed_config = {
        .name = "processed_data",
        .element_size = sizeof(struct processed_data),
        .capacity = 5,
        .mode = STREAM_MODE_LATEST,  /* Keep only latest processed data */
        .window_size = 0
    };
    processed_stream = data_stream_create(&processed_config);
    if (!processed_stream) {
        fprintf(stderr, "Failed to create processed data stream\n");
        return 1;
    }

    struct data_stream_config alert_config = {
        .name = "alert_data",
        .element_size = sizeof(struct alert_data),
        .capacity = 20,
        .mode = STREAM_MODE_FIFO,
        .window_size = 0
    };
    alert_stream = data_stream_create(&alert_config);
    if (!alert_stream) {
        fprintf(stderr, "Failed to create alert data stream\n");
        return 1;
    }

    printf("Data streams created successfully\n");

    /* Create tasks */
    printf("[Setup] Creating hybrid tasks...\n");

    int sensor1_id = 1, sensor2_id = 2;
    int processor1_id = 10, processor2_id = 20;
    int alert_handler_id = 100;
    int decision_maker_id = 200;

    struct parallel_task sensor1, sensor2;
    struct parallel_task processor1, processor2;
    struct parallel_task alert_handler;
    struct parallel_task decision_maker;

    /* Initialize sensor tasks */
    parallel_task_init(&sensor1, 1, sensor_task, &sensor1_id);
    parallel_task_init(&sensor2, 2, sensor_task, &sensor2_id);

    /* Configure sensor tasks to produce to sensor stream */
    parallel_task_add_output_stream(&sensor1, sensor_stream, "sensor_data");
    parallel_task_add_output_stream(&sensor2, sensor_stream, "sensor_data");
    sensor1.type = TASK_TYPE_HYBRID;
    sensor2.type = TASK_TYPE_HYBRID;

    /* Initialize processor tasks */
    parallel_task_init(&processor1, 3, processing_task, &processor1_id);
    parallel_task_init(&processor2, 4, processing_task, &processor2_id);

    /* Configure processor tasks: consume from sensor stream, produce to processed stream */
    parallel_task_add_input_stream(&processor1, sensor_stream, "sensor_data");
    parallel_task_add_output_stream(&processor1, processed_stream, "processed_data");
    parallel_task_add_input_stream(&processor2, sensor_stream, "sensor_data");
    parallel_task_add_output_stream(&processor2, processed_stream, "processed_data");
    processor1.type = TASK_TYPE_HYBRID;
    processor2.type = TASK_TYPE_HYBRID;

    /* Initialize alert handler task */
    parallel_task_init(&alert_handler, 5, alert_task, &alert_handler_id);

    /* Configure alert handler: consume from alert stream */
    parallel_task_add_input_stream(&alert_handler, alert_stream, "alert_data");
    alert_handler.type = TASK_TYPE_HYBRID;  /* Hybrid: no task dependencies, starts immediately, can process data streams */

    /* Initialize decision maker task */
    parallel_task_init(&decision_maker, 6, decision_task, &decision_maker_id);

    /* Configure decision maker: consume from processed stream */
    parallel_task_add_input_stream(&decision_maker, processed_stream, "processed_data");
    decision_maker.type = TASK_TYPE_DATAFLOW;

    /* Set up task dependencies (hybrid model) */
    printf("[Setup] Configuring task dependencies...\n");

    /* Processors depend on sensors (task dependencies) */
    parallel_task_add_dependency(&processor1, 1);  /* processor1 depends on sensor1 */
    parallel_task_add_dependency(&processor2, 2);  /* processor2 depends on sensor2 */

    /* Decision maker depends on processors (task dependencies) */
    parallel_task_add_dependency(&decision_maker, 3);  /* depends on processor1 */
    parallel_task_add_dependency(&decision_maker, 4);  /* depends on processor2 */

    /* Create workflow with data streams enabled */
    printf("[Setup] Creating hybrid workflow...\n");
    struct parallel_workflow workflow;
    parallel_workflow_init_ex(&workflow, 10, 3, 1);  /* Enable data streams */

    /* Set callbacks */
    workflow.on_workflow_complete = on_workflow_complete;
    workflow.on_task_complete = on_task_complete;

    /* Add tasks to workflow */
    parallel_workflow_add_task(&workflow, &sensor1);
    parallel_workflow_add_task(&workflow, &sensor2);
    parallel_workflow_add_task(&workflow, &processor1);
    parallel_workflow_add_task(&workflow, &processor2);
    parallel_workflow_add_task(&workflow, &alert_handler);
    parallel_workflow_add_task(&workflow, &decision_maker);

    printf("Hybrid workflow setup complete:\n");
    printf("- 2 sensor tasks (data producers)\n");
    printf("- 2 processor tasks (hybrid: task + data dependencies)\n");
    printf("- 1 alert handler task (data-only dependencies)\n");
    printf("- 1 decision maker task (task + data dependencies)\n");
    printf("- 3 data streams: sensor_data, processed_data, alert_data\n\n");

    /* Register monitor callback - disabled due to function signature mismatch */
    /* workflow_monitor_register_callback(workflow.monitor,
                                       monitor_callback,
                                       NULL,
                                       2000); */  /* Every 2 seconds */

    /* Start workflow */
    printf("[Execution] Starting hybrid workflow...\n");
    printf("[Debug] Workflow state before start: %d\n", workflow.state);
    printf("[Debug] Workflow task count: %d\n", workflow.task_count);
    for (int i = 0; i < workflow.task_count; i++) {
        if (workflow.tasks[i]) {
            printf("[Debug] Task %d: state=%d, type=%d, deps=%d\n",
                   workflow.tasks[i]->task_id,
                   workflow.tasks[i]->state,
                   workflow.tasks[i]->type,
                   workflow.tasks[i]->dependency_count);
        }
    }
    parallel_workflow_start(&workflow);
    printf("[Debug] Workflow state after start: %d\n", workflow.state);
    printf("[Debug] After start - task states:\n");
    for (int i = 0; i < workflow.task_count; i++) {
        if (workflow.tasks[i]) {
            printf("[Debug] Task %d: state=%d\n", workflow.tasks[i]->task_id, workflow.tasks[i]->state);
        }
    }

    /* Monitor progress */
    printf("[Execution] Monitoring workflow execution...\n");
    while (parallel_workflow_get_state(&workflow) == PWORKFLOW_RUNNING) {
        int progress = parallel_workflow_get_progress(&workflow);
        printf("Progress: %d%%\n", progress);

        /* Print stream statistics */
        struct data_stream_stats stats;
        if (data_stream_get_stats(sensor_stream, &stats) == DATA_STREAM_SUCCESS) {
            printf("Sensor Stream: %d writes, %d reads, %d queued\n",
                   stats.write_count, stats.read_count, stats.current_queue_depth);
        }

        sleep(2);
    }

    /* Final status */
    printf("\n[Execution] Workflow completed!\n");

    /* Print final monitor report */
    workflow_monitor_print_report(workflow.monitor, stdout);

    /* Cleanup */
    printf("\n[Cleanup] Cleaning up resources...\n");

    parallel_workflow_cleanup(&workflow);

    data_stream_destroy(sensor_stream);
    data_stream_destroy(processed_stream);
    data_stream_destroy(alert_stream);

    printf("=== Demo completed successfully ===\n");
    return 0;
}
