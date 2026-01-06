/**
 * embedded_ai_aide_dag_demo.c  2026-01-06
 *
 * Copyright (C) 2000-2026 All Right Reserved
 *
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Embedded AI Aide DAG Demo
 * Demonstrates the complex DAG workflow for embedded AI assistant system
 * with 6 specialized aides and resource-aware scheduling.
 */

#include "parallel_pipeline.h"
#include "data_stream.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

/* ============================================================================
 * DATA STRUCTURES
 * ============================================================================ */

/* Input data structures */
struct speech_input {
    int16_t *audio_buffer;
    size_t buffer_size;
    int sample_rate;
    timestamp_t timestamp;
};

struct text_input {
    char text[2048];
    timestamp_t timestamp;
};

/* Processed data structures */
struct processed_input {
    char normalized_text[2048];
    char emotion_type[32];
    float emotion_intensity;
    char intent_type[32];
    float intent_confidence;
    timestamp_t processed_time;
};

/* Aide result structures */
struct aide_result {
    int aide_id;
    char aide_type[32];
    char response[4096];
    float confidence;
    int processing_time_ms;
    timestamp_t timestamp;
};

/* Coordination data */
struct coordination_data {
    int activated_aides[6];
    int aide_count;
    float weights[6];
    char coordination_strategy[64];
    timestamp_t timestamp;
};

/* Final response */
struct response_data {
    char text_response[4096];
    char voice_response_url[256];
    char image_response_url[256];
    char personalization_notes[512];
    float overall_confidence;
    timestamp_t timestamp;
};

/* ============================================================================
 * GLOBAL DATA STREAMS
 * ============================================================================ */

static data_stream_t speech_stream;
static data_stream_t text_stream;
static data_stream_t processed_input_stream;
static data_stream_t aide_results_stream;
static data_stream_t coordination_stream;
static data_stream_t response_stream;

/* ============================================================================
 * TASK IMPLEMENTATIONS
 * ============================================================================ */

/* ------------------------ INPUT PROCESSING LAYER ------------------------ */

/* Task 1: Speech Recognizer */
void speech_recognizer_task(void *arg)
{
    int task_id = *(int *)arg;

    TRACE_INFO("Speech Recognizer [%d]: Starting speech recognition", task_id);

    struct speech_input input;
    struct processed_input output = {0};

    /* Simulate reading from speech stream */
    if (data_stream_read(speech_stream, &input, sizeof(input), 1000) == DATA_STREAM_SUCCESS) {
    /* Simulate speech recognition */
    usleep(200000); // Simulate processing time (200ms)

        strcpy(output.normalized_text, "Hello, how can I help you today?");
        output.processed_time = os_get_timestamp();

        TRACE_INFO("Speech Recognizer [%d]: Recognized text: %s", task_id, output.normalized_text);

        /* Write to processed input stream */
        data_stream_write(processed_input_stream, &output, sizeof(output), 1000);
    }

    TRACE_INFO("Speech Recognizer [%d]: Recognition complete", task_id);
}

/* Task 2: Text Preprocessor */
void text_preprocessor_task(void *arg)
{
    int task_id = *(int *)arg;

    TRACE_INFO("Text Preprocessor [%d]: Starting text preprocessing", task_id);

    struct text_input input;
    struct processed_input output = {0};

    /* Read from text stream */
    if (data_stream_read(text_stream, &input, sizeof(input), 1000) == DATA_STREAM_SUCCESS) {
        /* Simulate text preprocessing */
        usleep(30000); // 30ms

        // Simple normalization
        strcpy(output.normalized_text, input.text);
        output.processed_time = os_get_timestamp();

        TRACE_INFO("Text Preprocessor [%d]: Processed text: %s", task_id, output.normalized_text);

        /* Write to processed input stream */
        data_stream_write(processed_input_stream, &output, sizeof(output), 1000);
    }

    TRACE_INFO("Text Preprocessor [%d]: Preprocessing complete", task_id);
}

/* Task 3: Emotion Analyzer */
void emotion_analyzer_task(void *arg)
{
    int task_id = *(int *)arg;

    TRACE_INFO("Emotion Analyzer [%d]: Starting emotion analysis", task_id);

    struct processed_input data;

    if (data_stream_read(processed_input_stream, &data, sizeof(data), 1000) == DATA_STREAM_SUCCESS) {
        usleep(80000); // 80ms

        // Simulate emotion analysis
        strcpy(data.emotion_type, "NEUTRAL");
        data.emotion_intensity = 0.3f;

        TRACE_INFO("Emotion Analyzer [%d]: Detected emotion: %s (%.1f)",
                  task_id, data.emotion_type, data.emotion_intensity);

        // Write back to stream for next task
        data_stream_write(processed_input_stream, &data, sizeof(data), 1000);
    }

    TRACE_INFO("Emotion Analyzer [%d]: Analysis complete", task_id);
}

/* Task 4: Intent Classifier */
void intent_classifier_task(void *arg)
{
    int task_id = *(int *)arg;

    TRACE_INFO("Intent Classifier [%d]: Starting intent classification", task_id);

    struct processed_input data;

    if (data_stream_read(processed_input_stream, &data, sizeof(data), 1000) == DATA_STREAM_SUCCESS) {
        usleep(60000); // 60ms

        // Simulate intent classification based on input
        if (strstr(data.normalized_text, "how") || strstr(data.normalized_text, "what")) {
            strcpy(data.intent_type, "TECHNICAL");
        } else if (strstr(data.normalized_text, "schedule") || strstr(data.normalized_text, "meeting")) {
            strcpy(data.intent_type, "DAILY");
        } else {
            strcpy(data.intent_type, "CONSULTING");
        }
        data.intent_confidence = 0.85f;

        TRACE_INFO("Intent Classifier [%d]: Classified intent: %s (%.1f)",
                  task_id, data.intent_type, data.intent_confidence);

        // Write back to stream for aides
        data_stream_write(processed_input_stream, &data, sizeof(data), 1000);
    }

    TRACE_INFO("Intent Classifier [%d]: Classification complete", task_id);
}

/* ------------------------ AIDE SCHEDULING LAYER ------------------------ */

/* Task 10: General Aide */
void general_aide_task(void *arg)
{
    int task_id = *(int *)arg;

    TRACE_INFO("General Aide [%d]: Starting general assistance", task_id);

    struct processed_input input;
    struct aide_result result = {
        .aide_id = task_id,
        .confidence = 0.9f,
        .timestamp = os_get_timestamp()
    };
    strcpy(result.aide_type, "GENERAL");

    if (data_stream_read(processed_input_stream, &input, sizeof(input), 1000) == DATA_STREAM_SUCCESS) {
        usleep(150000); // 150ms

        // Simple response generation
        snprintf(result.response, sizeof(result.response),
                "General response: %s", input.normalized_text);
        result.processing_time_ms = 150;

        TRACE_INFO("General Aide [%d]: Generated response: %s", task_id, result.response);

        data_stream_write(aide_results_stream, &result, sizeof(result), 1000);
    }

    TRACE_INFO("General Aide [%d]: Assistance complete", task_id);
}

/* Task 12: Technical Aide */
void technical_aide_task(void *arg)
{
    int task_id = *(int *)arg;

    TRACE_INFO("Technical Aide [%d]: Starting technical assistance", task_id);

    struct processed_input input;
    struct aide_result result = {
        .aide_id = task_id,
        .confidence = 0.95f,
        .timestamp = os_get_timestamp()
    };
    strcpy(result.aide_type, "TECHNICAL");

    TRACE_INFO("Technical Aide [%d]: Attempting to read data stream", task_id);

    // For broadcast mode, try to read data
    if (data_stream_read(processed_input_stream, &input, sizeof(input), 1000) == DATA_STREAM_SUCCESS) {
        TRACE_INFO("Technical Aide [%d]: Successfully read input data", task_id);

        usleep(800000); // 800ms

        // Technical response generation
        snprintf(result.response, sizeof(result.response),
                "Technical solution for: %s\n1. Analyze problem\n2. Research solutions\n3. Implement fix", input.normalized_text);
        result.processing_time_ms = 800;

        TRACE_INFO("Technical Aide [%d]: Generated technical response", task_id);

        data_stream_write(aide_results_stream, &result, sizeof(result), 1000);
        TRACE_INFO("Technical Aide [%d]: Wrote result to aide_results_stream", task_id);
    } else {
        TRACE_ERROR("Technical Aide [%d]: Failed to read input data", task_id);
    }

    TRACE_INFO("Technical Aide [%d]: Technical assistance complete", task_id);
}

/* Task 14: Consulting Aide */
void consulting_aide_task(void *arg)
{
    int task_id = *(int *)arg;

    TRACE_INFO("Consulting Aide [%d]: Starting consulting assistance", task_id);

    struct processed_input input;
    struct aide_result result = {
        .aide_id = task_id,
        .confidence = 0.88f,
        .timestamp = os_get_timestamp()
    };
    strcpy(result.aide_type, "CONSULTING");

    if (data_stream_read(processed_input_stream, &input, sizeof(input), 1000) == DATA_STREAM_SUCCESS) {
        usleep(1200000); // 1200ms

        // Consulting response generation
        snprintf(result.response, sizeof(result.response),
                "Consulting analysis for: %s\n• Strategic assessment\n• Risk evaluation\n• Recommendation: Proceed with caution", input.normalized_text);
        result.processing_time_ms = 1200;

        TRACE_INFO("Consulting Aide [%d]: Generated consulting response", task_id);

        data_stream_write(aide_results_stream, &result, sizeof(result), 1000);
    }

    TRACE_INFO("Consulting Aide [%d]: Consulting assistance complete", task_id);
}

/* ------------------------ COORDINATION LAYER ------------------------ */

/* Task 20: Aide Coordinator */
void aide_coordinator_task(void *arg)
{
    int task_id = *(int *)arg;

    TRACE_INFO("Aide Coordinator [%d]: Starting coordination", task_id);

    struct coordination_data coord = {
        .aide_count = 0,
        .timestamp = os_get_timestamp()
    };

    // Collect aide results
    struct aide_result results[6];
    int result_count = 0;

    while (result_count < 3) {  // Only 3 aides: 10, 12, 14
        if (data_stream_read(aide_results_stream, &results[result_count], sizeof(struct aide_result), 500) == DATA_STREAM_SUCCESS) {
            coord.activated_aides[coord.aide_count++] = results[result_count].aide_id;

            // Calculate weights based on confidence
            coord.weights[coord.aide_count - 1] = results[result_count].confidence;

            result_count++;
        } else {
            break; // No more results
        }
    }

    if (coord.aide_count > 0) {
        strcpy(coord.coordination_strategy, "WEIGHTED_VOTING");

        TRACE_INFO("Aide Coordinator [%d]: Coordinated %d aides with strategy: %s",
                  task_id, coord.aide_count, coord.coordination_strategy);

        data_stream_write(coordination_stream, &coord, sizeof(coord), 1000);
    }

    TRACE_INFO("Aide Coordinator [%d]: Coordination complete", task_id);
}

/* ------------------------ DECISION FUSION LAYER ------------------------ */

/* Task 30: Result Integrator */
void result_integrator_task(void *arg)
{
    int task_id = *(int *)arg;

    TRACE_INFO("Result Integrator [%d]: Starting integration", task_id);

    struct coordination_data coord;
    if (data_stream_read(coordination_stream, &coord, sizeof(coord), 1000) == DATA_STREAM_SUCCESS) {
        usleep(100000); // 100ms

        TRACE_INFO("Result Integrator [%d]: Integrated results from %d aides",
                  task_id, coord.aide_count);
    }

    TRACE_INFO("Result Integrator [%d]: Integration complete", task_id);
}

/* ------------------------ OUTPUT GENERATION LAYER ------------------------ */

/* Task 40: Multimodal Generator */
void multimodal_generator_task(void *arg)
{
    int task_id = *(int *)arg;

    TRACE_INFO("Multimodal Generator [%d]: Starting generation", task_id);

    struct response_data response = {
        .overall_confidence = 0.87f,
        .timestamp = os_get_timestamp()
    };

    // Generate text response
    strcpy(response.text_response, "Integrated AI response combining multiple expert analyses");
    strcpy(response.voice_response_url, "/audio/response_001.wav");
    strcpy(response.image_response_url, "/images/chart_001.png");
    strcpy(response.personalization_notes, "Response adapted for technical user profile");

    TRACE_INFO("Multimodal Generator [%d]: Generated multimodal response", task_id);

    data_stream_write(response_stream, &response, sizeof(response), 1000);

    TRACE_INFO("Multimodal Generator [%d]: Generation complete", task_id);
}

/* ============================================================================
 * CALLBACKS
 * ============================================================================ */

void on_workflow_complete(struct parallel_workflow *pw)
{
    TRACE_INFO("=== AI AIDE DAG WORKFLOW COMPLETED ===");
    TRACE_INFO("Total tasks: %d", pw->task_count);
    TRACE_INFO("Final state: %d", pw->state);
}

void on_task_complete(struct parallel_workflow *pw, int task_id)
{
    TRACE_INFO("Task %d completed in AI Aide DAG", task_id);
}

/* ============================================================================
 * MAIN DEMO
 * ============================================================================ */

int main(int argc, char *argv[])
{
    printf("=== Embedded AI Aide DAG Demo ===\n");
    printf("Demonstrating complex DAG with 6 specialized aides\n\n");

    /* Initialize random seed */
    srand((unsigned int)time(NULL));

    /* Create data streams */
    printf("[Setup] Creating data streams...\n");

    // Speech stream
    struct data_stream_config speech_config = {
        .name = "speech_input",
        .element_size = sizeof(struct speech_input),
        .capacity = 5,
        .mode = STREAM_MODE_FIFO,
        .window_size = 0
    };
    speech_stream = data_stream_create(&speech_config);

    // Text stream
    struct data_stream_config text_config = {
        .name = "text_input",
        .element_size = sizeof(struct text_input),
        .capacity = 10,
        .mode = STREAM_MODE_LATEST,
        .window_size = 0
    };
    text_stream = data_stream_create(&text_config);

    // Processed input stream - use BROADCAST mode with reference counting
    struct data_stream_config processed_config = {
        .name = "processed_input",
        .element_size = sizeof(struct processed_input),
        .capacity = 16,  // Increased capacity for multiple consumers
        .mode = STREAM_MODE_BROADCAST,
        .window_size = 0
    };
    processed_input_stream = data_stream_create(&processed_config);

    /* Register consumers for broadcast mode */
    // Task 10, 12, 14 all read from processed_input_stream
    data_stream_add_consumer(processed_input_stream, 10);  // General Aide
    data_stream_add_consumer(processed_input_stream, 12);  // Technical Aide
    data_stream_add_consumer(processed_input_stream, 14);  // Consulting Aide

    // Aide results stream
    struct data_stream_config aide_results_config = {
        .name = "aide_results",
        .element_size = sizeof(struct aide_result),
        .capacity = 10,
        .mode = STREAM_MODE_LATEST,
        .window_size = 6
    };
    aide_results_stream = data_stream_create(&aide_results_config);

    // Coordination stream
    struct data_stream_config coordination_config = {
        .name = "coordination",
        .element_size = sizeof(struct coordination_data),
        .capacity = 5,
        .mode = STREAM_MODE_FIFO,
        .window_size = 0
    };
    coordination_stream = data_stream_create(&coordination_config);

    // Response stream
    struct data_stream_config response_config = {
        .name = "response",
        .element_size = sizeof(struct response_data),
        .capacity = 3,
        .mode = STREAM_MODE_LATEST,
        .window_size = 0
    };
    response_stream = data_stream_create(&response_config);

    printf("Data streams created successfully\n");

    /* Create input data */
    printf("[Setup] Preparing input data...\n");

    // Simulate text input
    struct text_input text_input = {
        .text = "How do I optimize my embedded system's performance?",
        .timestamp = os_get_timestamp()
    };

    // Write to text stream
    data_stream_write(text_stream, &text_input, sizeof(text_input), 1000);
    printf("Input text: %s\n", text_input.text);

    /* Add a small delay to ensure data is written before tasks start */
    usleep(10000); // 10ms delay

    /* Create tasks */
    printf("[Setup] Creating AI Aide DAG tasks...\n");

    // Input processing tasks (IDs 1-4)
    int speech_id = 1, text_id = 2, emotion_id = 3, intent_id = 4;
    struct parallel_task speech_task, text_task, emotion_task, intent_task;

    parallel_task_init(&speech_task, 1, speech_recognizer_task, &speech_id);
    parallel_task_init(&text_task, 2, text_preprocessor_task, &text_id);
    parallel_task_init(&emotion_task, 3, emotion_analyzer_task, &emotion_id);
    parallel_task_init(&intent_task, 4, intent_classifier_task, &intent_id);

    // Aide tasks (IDs 10, 12, 14)
    int general_id = 10, technical_id = 12, consulting_id = 14;
    struct parallel_task general_aide, technical_aide, consulting_aide;

    parallel_task_init(&general_aide, 10, general_aide_task, &general_id);
    parallel_task_init(&technical_aide, 12, technical_aide_task, &technical_id);
    parallel_task_init(&consulting_aide, 14, consulting_aide_task, &consulting_id);

    // Coordination tasks (ID 20, 30, 40)
    int coordinator_id = 20, integrator_id = 30, generator_id = 40;
    struct parallel_task coordinator, integrator, generator;

    parallel_task_init(&coordinator, 20, aide_coordinator_task, &coordinator_id);
    parallel_task_init(&integrator, 30, result_integrator_task, &integrator_id);
    parallel_task_init(&generator, 40, multimodal_generator_task, &generator_id);

    /* Set up dependencies */
    printf("[Setup] Configuring DAG dependencies...\n");

    // Input processing chain: task2 -> task3 -> task4
    parallel_task_add_dependency(&emotion_task, 2);    // emotion depends on text preprocessor
    parallel_task_add_dependency(&intent_task, 3);     // intent depends on emotion analyzer

    // Aides depend on complete input processing (task4)
    parallel_task_add_dependency(&general_aide, 4);    // depends on intent classifier
    parallel_task_add_dependency(&technical_aide, 4);
    parallel_task_add_dependency(&consulting_aide, 4);

    // Coordinator depends on aides
    parallel_task_add_dependency(&coordinator, 10);
    parallel_task_add_dependency(&coordinator, 12);
    parallel_task_add_dependency(&coordinator, 14);

    // Integrator depends on coordinator
    parallel_task_add_dependency(&integrator, 20);

    // Generator depends on integrator
    parallel_task_add_dependency(&generator, 30);

    /* Create workflow with embedded constraints */
    printf("[Setup] Creating DAG workflow (max_concurrent=3)...\n");
    struct parallel_workflow workflow;
    parallel_workflow_init_ex(&workflow, 50, 3, 1);  // Enable data streams, max concurrent = 3

    /* Set callbacks */
    workflow.on_workflow_complete = on_workflow_complete;
    workflow.on_task_complete = on_task_complete;

    /* Add all tasks to workflow */
    parallel_workflow_add_task(&workflow, &text_task);
    parallel_workflow_add_task(&workflow, &emotion_task);
    parallel_workflow_add_task(&workflow, &intent_task);

    parallel_workflow_add_task(&workflow, &general_aide);
    parallel_workflow_add_task(&workflow, &technical_aide);
    parallel_workflow_add_task(&workflow, &consulting_aide);

    parallel_workflow_add_task(&workflow, &coordinator);
    parallel_workflow_add_task(&workflow, &integrator);
    parallel_workflow_add_task(&workflow, &generator);

    printf("DAG workflow setup complete:\n");
    printf("- 3 input processing tasks (parallel)\n");
    printf("- 3 specialized aides (conditional activation)\n");
    printf("- 3 coordination tasks (sequential dependencies)\n");
    printf("- 9 data streams for inter-task communication\n");
    printf("- Resource-aware scheduling (max 3 concurrent)\n\n");

    /* Start workflow */
    printf("[Execution] Starting AI Aide DAG workflow...\n");
    parallel_workflow_start(&workflow);

    /* Monitor progress */
    printf("[Execution] Monitoring DAG execution...\n");
    int max_iterations = 30; // Max 30 seconds
    int iteration = 0;

    while (parallel_workflow_get_state(&workflow) == PWORKFLOW_RUNNING && iteration < max_iterations) {
        int progress = parallel_workflow_get_progress(&workflow);
        printf("Progress: %d%%\n", progress);

        // Check response stream
        struct response_data response;
        if (data_stream_read(response_stream, &response, sizeof(response), 100) == DATA_STREAM_SUCCESS) {
            printf("[Response Generated] %s\n", response.text_response);
            printf("[Confidence] %.1f%%\n", response.overall_confidence * 100);
        }

        sleep(1);
        iteration++;
    }

    // Force completion if still running
    if (parallel_workflow_get_state(&workflow) == PWORKFLOW_RUNNING) {
        printf("[Execution] Workflow still running after %d seconds, forcing completion\n", max_iterations);
        parallel_workflow_stop(&workflow);
        sleep(2); // Wait for cleanup
    }

    /* Final status */
    printf("\n[Execution] AI Aide DAG workflow completed!\n");

    /* Print final monitor report */
    workflow_monitor_print_report(workflow.monitor, stdout);

    /* Cleanup */
    printf("\n[Cleanup] Cleaning up resources...\n");

    parallel_workflow_cleanup(&workflow);

    data_stream_destroy(speech_stream);
    data_stream_destroy(text_stream);
    data_stream_destroy(processed_input_stream);
    data_stream_destroy(aide_results_stream);
    data_stream_destroy(coordination_stream);
    data_stream_destroy(response_stream);

    printf("=== Embedded AI Aide DAG Demo completed successfully ===\n");
    return 0;
}
