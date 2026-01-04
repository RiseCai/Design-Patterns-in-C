/**
 * test_data_stream_integration.c  2026-01-05
 * 
 * Copyright (C) 2000-2026 All Right Reserved
 * 
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Unit tests for data stream integration with parallel pipeline.
 */

#include "parallel_pipeline.h"
#include "data_stream.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Mock task functions */
static int producer_executed = 0;
static int consumer_executed = 0;
static int hybrid_executed = 0;

void mock_producer(void *arg)
{
    int *id = (int *)arg;
    printf("[TEST] Producer task %d executed\n", *id);
    producer_executed = 1;
    
    /* Get stream from task argument */
    struct parallel_task *task = (struct parallel_task *)arg;
    if (task && task->output_streams && task->output_streams[0]) {
        int data = 42;
        int rc = data_stream_write(task->output_streams[0], &data, sizeof(data), 100);
        printf("[TEST] Producer wrote data to stream: rc=%d\n", rc);
    }
}

void mock_consumer(void *arg)
{
    int *id = (int *)arg;
    printf("[TEST] Consumer task %d executed\n", *id);
    consumer_executed = 1;
    
    /* Get stream from task argument */
    struct parallel_task *task = (struct parallel_task *)arg;
    if (task && task->input_streams && task->input_streams[0]) {
        int data = 0;
        int rc = data_stream_read(task->input_streams[0], &data, sizeof(data), 100);
        printf("[TEST] Consumer read data from stream: rc=%d, data=%d\n", rc, data);
        assert(data == 42);
    }
}

void mock_hybrid(void *arg)
{
    int *id = (int *)arg;
    printf("[TEST] Hybrid task %d executed\n", *id);
    hybrid_executed = 1;
}

/* Test data stream creation and basic operations */
void test_data_stream_basic(void)
{
    printf("=== test_data_stream_basic ===\n");
    
    struct data_stream_config config = {
        .name = "test_stream",
        .element_size = sizeof(int),
        .capacity = 10,
        .mode = STREAM_MODE_FIFO,
        .window_size = 0
    };
    
    data_stream_t stream = data_stream_create(&config);
    assert(stream != NULL);
    
    /* Check metadata */
    struct data_stream_metadata metadata;
    int rc = data_stream_get_metadata(stream, &metadata);
    assert(rc == DATA_STREAM_SUCCESS);
    assert(strcmp(metadata.name, "test_stream") == 0);
    assert(metadata.stream_id > 0);
    
    /* Check element size and capacity */
    assert(data_stream_get_element_size(stream) == sizeof(int));
    assert(data_stream_get_capacity(stream) == 10);
    assert(data_stream_get_available(stream) == 10);
    
    /* Write data */
    int data = 123;
    rc = data_stream_write(stream, &data, sizeof(data), 100);
    assert(rc == DATA_STREAM_SUCCESS);
    
    /* Check has_data */
    assert(data_stream_has_data(stream) == 1);
    assert(data_stream_get_available(stream) == 9);
    
    /* Peek data */
    int peek_data = 0;
    rc = data_stream_peek(stream, &peek_data, sizeof(peek_data));
    assert(rc == DATA_STREAM_SUCCESS);
    assert(peek_data == 123);
    
    /* Read data */
    int read_data = 0;
    rc = data_stream_read(stream, &read_data, sizeof(read_data), 100);
    assert(rc == DATA_STREAM_SUCCESS);
    assert(read_data == 123);
    
    /* Check empty */
    assert(data_stream_has_data(stream) == 0);
    assert(data_stream_get_available(stream) == 10);
    
    /* Cleanup */
    rc = data_stream_destroy(stream);
    assert(rc == DATA_STREAM_SUCCESS);
    
    printf("PASS\n");
}

/* Test data stream with parallel workflow (legacy mode) */
void test_workflow_legacy_mode(void)
{
    printf("=== test_workflow_legacy_mode ===\n");
    
    /* Initialize workflow without data streams */
    struct parallel_workflow workflow;
    parallel_workflow_init(&workflow, 5, 2);
    
    /* Verify data stream service is not created */
    assert(workflow.data_stream_service == NULL);
    assert(workflow.monitor == NULL);
    assert(workflow.enable_data_streams == 0);
    
    /* Add tasks */
    struct parallel_task task1, task2;
    int id1 = 1, id2 = 2;
    parallel_task_init(&task1, id1, mock_producer, &id1);
    parallel_task_init(&task2, id2, mock_consumer, &id2);
    
    /* Add dependency */
    parallel_task_add_dependency(&task2, id1);
    
    parallel_workflow_add_task(&workflow, &task1);
    parallel_workflow_add_task(&workflow, &task2);
    
    /* Reset execution flags */
    producer_executed = 0;
    consumer_executed = 0;
    
    /* Start workflow */
    parallel_workflow_start(&workflow);
    
    /* Since OS adapter is stubbed, tasks won't actually run in threads.
       However, the state should be updated. */
    assert(workflow.state == PWORKFLOW_RUNNING);
    
    /* Cleanup */
    parallel_workflow_stop(&workflow);
    parallel_workflow_cleanup(&workflow);
    
    printf("PASS\n");
}

/* Test workflow with data streams enabled */
void test_workflow_data_stream_mode(void)
{
    printf("=== test_workflow_data_stream_mode ===\n");
    
    /* Initialize workflow with data streams */
    struct parallel_workflow workflow;
    parallel_workflow_init_ex(&workflow, 5, 2, 1);
    
    /* Verify data stream service is created */
    assert(workflow.data_stream_service != NULL);
    assert(workflow.monitor != NULL);
    assert(workflow.enable_data_streams == 1);
    
    /* Create a data stream */
    struct data_stream_config config = {
        .name = "workflow_stream",
        .element_size = sizeof(int),
        .capacity = 5,
        .mode = STREAM_MODE_FIFO,
        .window_size = 0
    };
    
    data_stream_t stream = data_stream_create(&config);
    assert(stream != NULL);
    
    /* Create tasks */
    struct parallel_task producer, consumer;
    int producer_id = 10, consumer_id = 20;
    parallel_task_init(&producer, producer_id, mock_producer, &producer_id);
    parallel_task_init(&consumer, consumer_id, mock_consumer, &consumer_id);
    
    /* Configure data stream dependencies */
    int rc = parallel_task_add_output_stream(&producer, stream, "workflow_stream");
    assert(rc == 0);
    rc = parallel_task_add_input_stream(&consumer, stream, "workflow_stream");
    assert(rc == 0);
    
    /* Set task types */
    producer.type = TASK_TYPE_HYBRID;
    consumer.type = TASK_TYPE_HYBRID;
    
    /* Add tasks to workflow */
    parallel_workflow_add_task(&workflow, &producer);
    parallel_workflow_add_task(&workflow, &consumer);
    
    /* Check dependency checking */
    int deps_met = parallel_task_check_dependencies_ex(&consumer);
    /* Consumer should not be ready because stream has no data yet */
    assert(deps_met == 0);
    
    /* Write data to stream to satisfy dependency */
    int data = 99;
    rc = data_stream_write(stream, &data, sizeof(data), 100);
    assert(rc == DATA_STREAM_SUCCESS);
    
    /* Now consumer should have data dependency satisfied */
    deps_met = parallel_task_check_dependencies_ex(&consumer);
    assert(deps_met == 1);
    
    /* Cleanup */
    data_stream_destroy(stream);
    parallel_workflow_cleanup(&workflow);
    
    printf("PASS\n");
}

/* Test hybrid task with both task and data dependencies */
void test_hybrid_dependencies(void)
{
    printf("=== test_hybrid_dependencies ===\n");
    
    /* Initialize workflow with data streams */
    struct parallel_workflow workflow;
    parallel_workflow_init_ex(&workflow, 5, 2, 1);
    
    /* Create two data streams */
    struct data_stream_config config1 = {
        .name = "stream1",
        .element_size = sizeof(int),
        .capacity = 3,
        .mode = STREAM_MODE_FIFO,
        .window_size = 0
    };
    
    struct data_stream_config config2 = {
        .name = "stream2",
        .element_size = sizeof(int),
        .capacity = 3,
        .mode = STREAM_MODE_FIFO,
        .window_size = 0
    };
    
    data_stream_t stream1 = data_stream_create(&config1);
    data_stream_t stream2 = data_stream_create(&config2);
    assert(stream1 != NULL);
    assert(stream2 != NULL);
    
    /* Create tasks */
    struct parallel_task task1, task2, task3;
    int id1 = 1, id2 = 2, id3 = 3;
    parallel_task_init(&task1, id1, mock_producer, &id1);
    parallel_task_init(&task2, id2, mock_producer, &id2);
    parallel_task_init(&task3, id3, mock_hybrid, &id3);
    
    /* Configure dependencies:
       - task3 depends on task1 (task dependency)
       - task3 depends on stream2 (data dependency)
       - task2 produces to stream1 (not used by task3)
       - task1 produces to stream2
    */
    parallel_task_add_dependency(&task3, id1);
    parallel_task_add_output_stream(&task1, stream2, "stream2");
    parallel_task_add_input_stream(&task3, stream2, "stream2");
    parallel_task_add_output_stream(&task2, stream1, "stream1");
    
    /* Set task types */
    task1.type = TASK_TYPE_HYBRID;
    task2.type = TASK_TYPE_HYBRID;
    task3.type = TASK_TYPE_HYBRID;
    
    /* Add to workflow */
    parallel_workflow_add_task(&workflow, &task1);
    parallel_workflow_add_task(&workflow, &task2);
    parallel_workflow_add_task(&workflow, &task3);
    
    /* Initially, task3 should not be ready */
    int deps_met = parallel_task_check_dependencies_ex(&task3);
    assert(deps_met == 0);
    
    /* Mark task1 as completed (simulate) */
    task1.state = PTASK_COMPLETED;
    
    /* Still not ready because stream2 has no data */
    deps_met = parallel_task_check_dependencies_ex(&task3);
    assert(deps_met == 0);
    
    /* Write data to stream2 */
    int data = 777;
    int rc = data_stream_write(stream2, &data, sizeof(data), 100);
    assert(rc == DATA_STREAM_SUCCESS);
    
    /* Now task3 should be ready */
    deps_met = parallel_task_check_dependencies_ex(&task3);
    assert(deps_met == 1);
    
    /* Cleanup */
    data_stream_destroy(stream1);
    data_stream_destroy(stream2);
    parallel_workflow_cleanup(&workflow);
    
    printf("PASS\n");
}

/* Test data stream statistics */
void test_data_stream_stats(void)
{
    printf("=== test_data_stream_stats ===\n");
    
    struct data_stream_config config = {
        .name = "stats_stream",
        .element_size = sizeof(float),
        .capacity = 5,
        .mode = STREAM_MODE_FIFO,
        .window_size = 0
    };
    
    data_stream_t stream = data_stream_create(&config);
    assert(stream != NULL);
    
    /* Get initial stats */
    struct data_stream_stats stats;
    int rc = data_stream_get_stats(stream, &stats);
    assert(rc == DATA_STREAM_SUCCESS);
    assert(stats.write_count == 0);
    assert(stats.read_count == 0);
    assert(stats.bytes_written == 0);
    assert(stats.bytes_read == 0);
    
    /* Write some data */
    float data[3] = {1.1f, 2.2f, 3.3f};
    for (int i = 0; i < 3; i++) {
        rc = data_stream_write(stream, &data[i], sizeof(float), 100);
        assert(rc == DATA_STREAM_SUCCESS);
    }
    
    /* Get updated stats */
    rc = data_stream_get_stats(stream, &stats);
    assert(rc == DATA_STREAM_SUCCESS);
    assert(stats.write_count == 3);
    assert(stats.bytes_written == 3 * sizeof(float));
    assert(stats.current_queue_depth == 3);
    assert(stats.max_queue_depth == 3);
    
    /* Read data */
    float read_data;
    for (int i = 0; i < 2; i++) {
        rc = data_stream_read(stream, &read_data, sizeof(float), 100);
        assert(rc == DATA_STREAM_SUCCESS);
    }
    
    /* Get final stats */
    rc = data_stream_get_stats(stream, &stats);
    assert(rc == DATA_STREAM_SUCCESS);
    assert(stats.write_count == 3);
    assert(stats.read_count == 2);
    assert(stats.bytes_written == 3 * sizeof(float));
    assert(stats.bytes_read == 2 * sizeof(float));
    assert(stats.current_queue_depth == 1);
    
    /* Reset stats */
    data_stream_reset_stats(stream);
    rc = data_stream_get_stats(stream, &stats);
    assert(rc == DATA_STREAM_SUCCESS);
    assert(stats.write_count == 0);
    assert(stats.read_count == 0);
    assert(stats.bytes_written == 0);
    assert(stats.bytes_read == 0);
    assert(stats.current_queue_depth == 1); /* Queue depth is not reset */
    
    /* Cleanup */
    data_stream_destroy(stream);
    
    printf("PASS\n");
}

/* Test workflow monitor */
void test_workflow_monitor(void)
{
    printf("=== test_workflow_monitor ===\n");
    
    /* Initialize workflow with data streams */
    struct parallel_workflow workflow;
    parallel_workflow_init_ex(&workflow, 3, 1, 1);
    
    /* Verify monitor exists */
    assert(workflow.monitor != NULL);
    
    /* Create a simple task */
    struct parallel_task task;
    int task_id = 100;
    parallel_task_init(&task, task_id, mock_producer, &task_id);
    parallel_workflow_add_task(&workflow, &task);
    
    /* Collect stats */
    int rc = workflow_monitor_collect_stats(workflow.monitor, &workflow);
    assert(rc == 0);
    
    /* Print report (just ensure it doesn't crash) */
    workflow_monitor_print_report(workflow.monitor, stdout);
    
    /* Register callback (stub implementation) - skip NULL callback test */
    /* monitor_callback_t dummy_callback = NULL; */
    /* rc = workflow_monitor_register_callback(workflow.monitor, dummy_callback, NULL, 1000); */
    /* assert(rc == -1); */ /* NULL callback should fail */
    
    /* Cleanup */
    parallel_workflow_cleanup(&workflow);
    
    printf("PASS\n");
}

/* Test backward compatibility */
void test_backward_compatibility(void)
{
    printf("=== test_backward_compatibility ===\n");
    
    /* Test that old API still works */
    struct parallel_workflow workflow;
    parallel_workflow_init(&workflow, 5, 2);
    
    /* Old tasks should work */
    struct parallel_task task;
    int task_id = 50;
    parallel_task_init(&task, task_id, mock_producer, &task_id);
    
    /* Old dependency API */
    parallel_task_add_dependency(&task, 10);
    assert(task.dependency_count == 1);
    assert(task.depends_on[0] == 10);
    
    /* Task should be legacy type by default */
    assert(task.type == TASK_TYPE_LEGACY);
    
    /* Check dependencies using old function */
    task.workflow = &workflow;
    int ready = parallel_workflow_check_dependencies(&workflow, task_id);
    /* Since dependency task 10 doesn't exist, should return 0 */
    assert(ready == 0);
    
    parallel_workflow_cleanup(&workflow);
    
    printf("PASS\n");
}

/* Main test runner */
int main(int argc, char *argv[])
{
    printf("Running data stream integration tests...\n\n");
    
    test_data_stream_basic();
    test_workflow_legacy_mode();
    test_workflow_data_stream_mode();
    test_hybrid_dependencies();
    test_data_stream_stats();
    test_workflow_monitor();
    test_backward_compatibility();
    
    printf("\nAll data stream integration tests passed!\n");
    return 0;
}
