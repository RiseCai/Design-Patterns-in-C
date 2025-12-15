# Makefile for pipeline examples
# Builds both data_pipeline_demo and task_pipeline_demo

CC = gcc
CFLAGS = -Wall -Wextra -I../../../../src -I./data_pipeline/include -I./task_pipeline/include
LDFLAGS = -lm

# Source directories
DATA_SRC = data_pipeline/src/data_pipeline.c
TASK_SRC = task_pipeline/src/task_pipeline.c
COMMON_SRC = ../../../../src/efsm_protocol.c ../../../../src/parallel_fsm.c

# Demo sources
DATA_DEMO = data_pipeline/examples/data_pipeline_demo.c
TASK_DEMO = task_pipeline/examples/task_pipeline_demo.c

# Targets
all: data_pipeline_demo task_pipeline_demo

data_pipeline_demo: $(DATA_DEMO) $(DATA_SRC) $(COMMON_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

task_pipeline_demo: $(TASK_DEMO) $(TASK_SRC) $(COMMON_SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f data_pipeline_demo task_pipeline_demo

.PHONY: all clean
