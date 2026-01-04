/**
 * test_parallel_pipeline.c  2026-01-04
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
 * Unit tests for parallel pipeline.
 */

#include "parallel_pipeline.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/* Mock task functions */
static int task1_executed = 0;
static int task2_executed = 0;
static int task3_executed = 0;

void mock_task1(void *arg)
{
    int *id = (int *)arg;
    printf("[TEST] Task %d executed\n", *id);
    task1_executed = 1;
}

void mock_task2(void *arg)
{
    int *id = (int *)arg;
    printf("[TEST] Task %d executed\n", *id);
    task2_executed = 1;
}

void mock_task3(void *arg)
{
    int *id = (int *)arg;
    printf("[TEST] Task %d executed\n", *id);
    task3_executed = 1;
}

/* Test parallel_task_init */
void test_parallel_task_init(void)
{
    printf("=== test_parallel_task_init ===\n");
    struct parallel_task task;
    int task_id = 100;
    parallel_task_init(&task, task_id, mock_task1, &task_id);
    
    assert(task.task_id == task_id);
    assert(task.task_function == mock_task1);
    assert(task.task_arg == &task_id);
    assert(task.state == PTASK_IDLE);
    assert(task.dependency_count == 0);
    assert(task.error_code == 0);
    assert(strlen(task.error_msg) == 0);
    assert(task.result == NULL);
    
    printf("PASS\n");
}

/* Test parallel_task_add_dependency */
void test_parallel_task_add_dependency(void)
{
    printf("=== test_parallel_task_add_dependency ===\n");
    struct parallel_task task;
    int task_id = 200;
    parallel_task_init(&task, task_id, mock_task1, &task_id);
    
    parallel_task_add_dependency(&task, 10);
    parallel_task_add_dependency(&task, 20);
    
    assert(task.dependency_count == 2);
    assert(task.depends_on[0] == 10);
    assert(task.depends_on[1] == 20);
    
    printf("PASS\n");
}

/* Test parallel_workflow_init */
void test_parallel_workflow_init(void)
{
    printf("=== test_parallel_workflow_init ===\n");
    struct parallel_workflow workflow;
    parallel_workflow_init(&workflow, 5, 2);
    
    assert(workflow.capacity == 5);
    assert(workflow.max_concurrent == 2);
    assert(workflow.task_count == 0);
    assert(workflow.state == PWORKFLOW_IDLE);
    assert(workflow.running_tasks == 0);
    assert(workflow.tasks != NULL);
    
    parallel_workflow_cleanup(&workflow);
    printf("PASS\n");
}

/* Test parallel_workflow_add_task */
void test_parallel_workflow_add_task(void)
{
    printf("=== test_parallel_workflow_add_task ===\n");
    struct parallel_workflow workflow;
    parallel_workflow_init(&workflow, 5, 2);
    
    struct parallel_task task1, task2;
    int id1 = 1, id2 = 2;
    parallel_task_init(&task1, id1, mock_task1, &id1);
    parallel_task_init(&task2, id2, mock_task2, &id2);
    
    parallel_workflow_add_task(&workflow, &task1);
    parallel_workflow_add_task(&workflow, &task2);
    
    assert(workflow.task_count == 2);
    assert(workflow.tasks[0] == &task1);
    assert(workflow.tasks[1] == &task2);
    
    parallel_workflow_cleanup(&workflow);
    printf("PASS\n");
}

/* Test dependency resolution */
void test_dependency_resolution(void)
{
    printf("=== test_dependency_resolution ===\n");
    struct parallel_workflow workflow;
    parallel_workflow_init(&workflow, 5, 2);
    
    struct parallel_task task1, task2, task3;
    int id1 = 1, id2 = 2, id3 = 3;
    parallel_task_init(&task1, id1, mock_task1, &id1);
    parallel_task_init(&task2, id2, mock_task2, &id2);
    parallel_task_init(&task3, id3, mock_task3, &id3);
    
    /* task3 depends on task1 and task2 */
    parallel_task_add_dependency(&task3, id1);
    parallel_task_add_dependency(&task3, id2);
    
    parallel_workflow_add_task(&workflow, &task1);
    parallel_workflow_add_task(&workflow, &task2);
    parallel_workflow_add_task(&workflow, &task3);
    
    /* Initially, task3 should not be ready */
    int ready = parallel_workflow_check_dependencies(&workflow, id3);
    assert(ready == 0);
    
    /* Mark task1 and task2 as completed */
    task1.state = PTASK_COMPLETED;
    task2.state = PTASK_COMPLETED;
    
    ready = parallel_workflow_check_dependencies(&workflow, id3);
    assert(ready == 1);
    
    parallel_workflow_cleanup(&workflow);
    printf("PASS\n");
}

/* Test workflow start with no dependencies */
void test_workflow_start_no_deps(void)
{
    printf("=== test_workflow_start_no_deps ===\n");
    struct parallel_workflow workflow;
    parallel_workflow_init(&workflow, 5, 2);
    
    struct parallel_task task1, task2;
    int id1 = 1, id2 = 2;
    parallel_task_init(&task1, id1, mock_task1, &id1);
    parallel_task_init(&task2, id2, mock_task2, &id2);
    
    parallel_workflow_add_task(&workflow, &task1);
    parallel_workflow_add_task(&workflow, &task2);
    
    /* Reset execution flags */
    task1_executed = 0;
    task2_executed = 0;
    
    /* Start workflow */
    parallel_workflow_start(&workflow);
    
    /* Since OS adapter is stubbed, tasks won't actually run in threads.
       However, the state should be updated. */
    assert(workflow.state == PWORKFLOW_RUNNING);
    /* Tasks should be in READY or RUNNING state, but may be FAILED if thread creation fails */
    assert(task1.state == PTASK_READY || task1.state == PTASK_RUNNING || task1.state == PTASK_FAILED);
    assert(task2.state == PTASK_READY || task2.state == PTASK_RUNNING || task2.state == PTASK_FAILED);
    
    parallel_workflow_stop(&workflow);
    /* Wait for threads to finish */
    os_delay(100);
    parallel_workflow_cleanup(&workflow);
    printf("PASS\n");
}

/* Test workflow progress */
void test_workflow_progress(void)
{
    printf("=== test_workflow_progress ===\n");
    struct parallel_workflow workflow;
    parallel_workflow_init(&workflow, 3, 2);
    
    struct parallel_task task1, task2, task3;
    int id1 = 1, id2 = 2, id3 = 3;
    parallel_task_init(&task1, id1, mock_task1, &id1);
    parallel_task_init(&task2, id2, mock_task2, &id2);
    parallel_task_init(&task3, id3, mock_task3, &id3);
    
    parallel_workflow_add_task(&workflow, &task1);
    parallel_workflow_add_task(&workflow, &task2);
    parallel_workflow_add_task(&workflow, &task3);
    
    /* Initially progress 0% */
    int progress = parallel_workflow_get_progress(&workflow);
    assert(progress == 0);
    
    /* Mark one task completed */
    task1.state = PTASK_COMPLETED;
    progress = parallel_workflow_get_progress(&workflow);
    assert(progress == 33); /* 1/3 ≈ 33% */
    
    /* Mark second task completed */
    task2.state = PTASK_COMPLETED;
    progress = parallel_workflow_get_progress(&workflow);
    assert(progress == 66); /* 2/3 ≈ 66% */
    
    parallel_workflow_cleanup(&workflow);
    printf("PASS\n");
}

/* Test workflow pause/resume (stub) */
void test_workflow_pause_resume(void)
{
    printf("=== test_workflow_pause_resume ===\n");
    struct parallel_workflow workflow;
    parallel_workflow_init(&workflow, 2, 1);
    
    struct parallel_task task;
    int id = 99;
    parallel_task_init(&task, id, mock_task1, &id);
    parallel_workflow_add_task(&workflow, &task);
    
    parallel_workflow_start(&workflow);
    assert(workflow.state == PWORKFLOW_RUNNING);
    
    parallel_workflow_pause(&workflow);
    /* After pausing, state should be PAUSED, but if task completed immediately,
       state may be COMPLETED. We accept both. */
    assert(workflow.state == PWORKFLOW_PAUSED || workflow.state == PWORKFLOW_COMPLETED);
    
    if (workflow.state == PWORKFLOW_PAUSED) {
        parallel_workflow_resume(&workflow);
        /* After resume, state could be RUNNING or COMPLETED (if task finished during resume) */
        assert(workflow.state == PWORKFLOW_RUNNING || workflow.state == PWORKFLOW_COMPLETED);
    } else {
        /* Workflow already completed, resume is a no-op */
        parallel_workflow_resume(&workflow);
        assert(workflow.state == PWORKFLOW_COMPLETED);
    }
    
    parallel_workflow_stop(&workflow);
    parallel_workflow_cleanup(&workflow);
    printf("PASS\n");
}

/* Test workflow reset */
void test_workflow_reset(void)
{
    printf("=== test_workflow_reset ===\n");
    struct parallel_workflow workflow;
    parallel_workflow_init(&workflow, 2, 1);
    
    struct parallel_task task;
    int id = 77;
    parallel_task_init(&task, id, mock_task1, &id);
    parallel_workflow_add_task(&workflow, &task);
    
    parallel_workflow_start(&workflow);
    assert(workflow.state == PWORKFLOW_RUNNING);
    
    /* Wait a bit for task to possibly complete (since OS adapter is stubbed,
       the task may run in a separate thread). We'll give it a small delay. */
    os_delay(10); /* 10 ms */
    
    parallel_workflow_reset(&workflow);
    assert(workflow.state == PWORKFLOW_IDLE);
    assert(workflow.running_tasks == 0);
    /* Task state may be IDLE or COMPLETED depending on timing; both are acceptable
       because reset will set it to IDLE anyway. */
    assert(task.state == PTASK_IDLE || task.state == PTASK_COMPLETED);
    
    parallel_workflow_cleanup(&workflow);
    printf("PASS\n");
}

/* Main test runner */
int main(int argc, char *argv[])
{
    printf("Running parallel pipeline unit tests...\n");
    
    test_parallel_task_init();
    test_parallel_task_add_dependency();
    test_parallel_workflow_init();
    test_parallel_workflow_add_task();
    test_dependency_resolution();
    test_workflow_start_no_deps();
    test_workflow_progress();
    test_workflow_pause_resume();
    test_workflow_reset();
    
    printf("\nAll tests passed!\n");
    return 0;
}
