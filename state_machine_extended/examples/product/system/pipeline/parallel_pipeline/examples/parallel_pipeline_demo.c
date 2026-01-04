/**
 * parallel_pipeline_demo.c  2026-01-05
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
 * Demo of parallel pipeline showing task dependencies and concurrent execution.
 */

#include "../include/parallel_pipeline.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* Task functions */
void demo_task_function(void *arg)
{
    int *id = (int *)arg;
    printf("Task %d executing...\n", *id);
    sleep(1); /* Simulate work */
    printf("Task %d completed\n", *id);
}

/* Callbacks */
void demo_on_workflow_complete(struct parallel_workflow *pw)
{
    printf("Workflow completed! Total tasks: %d\n", pw->task_count);
}

void demo_on_task_complete(struct parallel_workflow *pw, int task_id)
{
    printf("Task %d completed in workflow\n", task_id);
    
    /* Send status via MQ (demo) */
    char status_msg[256];
    snprintf(status_msg, sizeof(status_msg), "Task %d completed", task_id);
    parallel_mq_send_task_status("task_status_queue", status_msg);
}

/* Main demo */
int main(void)
{
    printf("=== Parallel Pipeline Demo ===\n");
    
    /* Create tasks */
    int task_ids[4] = {1, 2, 3, 4};
    
    struct parallel_task tasks[4];
    for (int i = 0; i < 4; i++) {
        parallel_task_init(&tasks[i], task_ids[i], demo_task_function, &task_ids[i]);
    }
    
    /* Set up dependencies: task3 depends on task1 and task2 */
    parallel_task_add_dependency(&tasks[2], 1); /* task3 depends on task1 */
    parallel_task_add_dependency(&tasks[2], 2); /* task3 depends on task2 */
    /* task4 depends on task3 */
    parallel_task_add_dependency(&tasks[3], 3);
    
    /* Create workflow with capacity 10, max concurrent 2 */
    struct parallel_workflow workflow;
    parallel_workflow_init(&workflow, 10, 2);
    
    /* Set callbacks */
    workflow.on_workflow_complete = demo_on_workflow_complete;
    workflow.on_task_complete = demo_on_task_complete;
    
    /* Add tasks to workflow */
    for (int i = 0; i < 4; i++) {
        parallel_workflow_add_task(&workflow, &tasks[i]);
    }
    
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
    
    /* Integrate with external systems (demo) */
    printf("[Main] Integrating with external systems...\n");
    parallel_mq_send_task_status("task_status_queue", "Workflow completed");
    parallel_rest_post_task_result("http://api.example.com/workflow", "workflow_123", NULL);
    
    /* Cleanup */
    printf("[Main] Cleaning up...\n");
    parallel_workflow_cleanup(&workflow);
    
    printf("=== Demo finished ===\n");
    return 0;
}
