# Makefile for pipeline examples
# Builds data_pipeline_demo, task_pipeline_demo, and parallel_pipeline_os_example

CC = gcc
CFLAGS = -Wall -Wextra -I../../../../src -I./data_pipeline/include -I./task_pipeline/include -I./parallel_pipeline/include -I../../../../../os_abstraction_layer/include
LDFLAGS = -lm -lpthread

# Source directories
DATA_SRC = data_pipeline/src/data_pipeline.c
TASK_SRC = task_pipeline/src/task_pipeline.c
PARALLEL_SRC = parallel_pipeline/src/parallel_pipeline.c
COMMON_SRC = ../../../../src/efsm_protocol.c ../../../../src/parallel_fsm.c ../../../../src/fsm_os_adapter.c ../../../../../os_abstraction_layer/build/os_abstract_stub.c

# Demo sources
DATA_DEMO = data_pipeline/examples/data_pipeline_demo.c
TASK_DEMO = task_pipeline/examples/task_pipeline_demo.c

# OS example sources
DATA_OS_EXAMPLE = data_pipeline/examples/data_pipeline_os_example.c
TASK_OS_EXAMPLE = task_pipeline/examples/task_pipeline_os_example.c
PARALLEL_OS_EXAMPLE = parallel_pipeline/examples/parallel_pipeline_os_example.c

# Test sources
PARALLEL_TEST = parallel_pipeline/test/test_parallel_pipeline.c

# Targets
all: data_pipeline_demo task_pipeline_demo data_pipeline_os_example task_pipeline_os_example parallel_pipeline_os_example test_parallel_pipeline

data_pipeline_demo: $(DATA_DEMO) $(DATA_SRC) $(COMMON_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

task_pipeline_demo: $(TASK_DEMO) $(TASK_SRC) $(COMMON_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

data_pipeline_os_example: $(DATA_OS_EXAMPLE) $(DATA_SRC) $(COMMON_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

task_pipeline_os_example: $(TASK_OS_EXAMPLE) $(TASK_SRC) $(COMMON_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

parallel_pipeline_os_example: $(PARALLEL_OS_EXAMPLE) $(PARALLEL_SRC) $(TASK_SRC) $(COMMON_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

test_parallel_pipeline: $(PARALLEL_TEST) $(PARALLEL_SRC) $(TASK_SRC) $(COMMON_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f data_pipeline_demo task_pipeline_demo data_pipeline_os_example task_pipeline_os_example parallel_pipeline_os_example test_parallel_pipeline

.PHONY: all clean
