/**
 * parallel_pipeline_os_example.c  2026-01-04
 * 
 * Copyright (C) 2000-2026 All Right Reserved
 * 
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Auto generate for Design Patterns in C *
 * 
 * Example demonstrating parallel pipeline with OS abstraction.
 */

#include "parallel_pipeline.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Task functions */
void task1_function(void *arg)
{
    int *id = (int *)arg;
    TRACE_INFO("Task %d executing...", *id);
    sleep(1); /* Simulate work */
    TRACE_INFO("Task %d completed", *id);
}

void task2_function(void *arg)
{
    int *id = (int *)arg;
    TRACE_INFO("Task %d executing...", *id);
    sleep(2); /* Simulate longer work */
    TRACE_INFO("Task %d completed", *id);
}

void task3_function(void *arg)
{
    int *id = (int *)arg;
    TRACE_INFO("Task %d executing...", *id);
    sleep(1);
    TRACE_INFO("Task %d completed", *id);
}

void task4_function(void *arg)
{
    int *id = (int *)arg;
    TRACE_INFO("Task %d executing...", *id);
    sleep(3);
    TRACE_INFO("Task %d completed", *id);
}

/* Callbacks */
void on_workflow_complete(struct parallel_workflow *pw)
{
    TRACE_INFO("Workflow completed! Total tasks: %d", pw->task_count);
}

void on_task_complete(struct parallel_workflow *pw, int task_id)
{
    TRACE_INFO("Task %d completed in workflow", task_id);
    
    /* Send status via MQ */
    char status_msg[256];
    snprintf(status_msg, sizeof(status_msg), "Task %d completed", task_id);
    parallel_mq_send_task_status("task_status_queue", status_msg);
}

/* Main example */
int main(int argc, char *argv[])
{
    printf("=== Parallel Pipeline OS Adapter Example ===\n");
    
    /* Initialize OS abstraction */
    printf("[STUB] os_abstract_init\n");
    
    /* Create tasks */
    int task1_id = 1, task2_id = 2, task3_id = 3, task4_id = 4;
    
    struct parallel_task task1, task2, task3, task4;
    parallel_task_init(&task1, task1_id, task1_function, &task1_id);
    parallel_task_init(&task2, task2_id, task2_function, &task2_id);
    parallel_task_init(&task3, task3_id, task3_function, &task3_id);
    parallel_task_init(&task4, task4_id, task4_function, &task4_id);
    
    /* Set up dependencies: task3 depends on task1 and task2 */
    parallel_task_add_dependency(&task3, task1_id);
    parallel_task_add_dependency(&task3, task2_id);
    /* task4 depends on task3 */
    parallel_task_add_dependency(&task4, task3_id);
    
    /* Create workflow with capacity 10, max concurrent 2 */
    struct parallel_workflow workflow;
    parallel_workflow_init(&workflow, 10, 2);
    
    /* Set callbacks */
    workflow.on_workflow_complete = on_workflow_complete;
    workflow.on_task_complete = on_task_complete;
    
    /* Add tasks to workflow */
    parallel_workflow_add_task(&workflow, &task1);
    parallel_workflow_add_task(&workflow, &task2);
    parallel_workflow_add_task(&workflow, &task3);
    parallel_workflow_add_task(&workflow, &task4);
    
    printf("Added %d tasks to workflow.\n", workflow.task_count);
    printf("Dependencies: task3 -> task1,task2; task4 -> task3\n");
    
    /* Start workflow */
    printf("Starting parallel workflow...\n");
    parallel_workflow_start(&workflow);
    
    /* Monitor progress */
    printf("[Main] Monitoring workflow progress...\n");
    while (parallel_workflow_get_state(&workflow) == PWORKFLOW_RUNNING) {
        int progress = parallel_workflow_get_progress(&workflow);
        printf("Progress: %d%%\n", progress);
        sleep(1);
    }
    
    /* Final status */
    enum parallel_workflow_state final_state = parallel_workflow_get_state(&workflow);
    printf("Workflow finished with state: %d\n", final_state);
    
    /* Integrate with external systems */
    printf("[Main] Integrating with external systems...\n");
    parallel_mq_send_task_status("task_status_queue", "Workflow completed");
    parallel_rest_post_task_result("http://api.example.com/workflow", "workflow_123", NULL);
    
    /* Cleanup */
    printf("[Main] Cleaning up...\n");
    parallel_workflow_cleanup(&workflow);
    
    printf("=== Example finished ===\n");
    return 0;
}
