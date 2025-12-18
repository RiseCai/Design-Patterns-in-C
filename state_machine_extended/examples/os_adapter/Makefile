# Makefile for building OS adapter examples
# Assumes OS abstraction layer library is installed in ../os_abstraction_layer/build

CC = gcc
CFLAGS = -I../../src -I../../../os_abstraction_layer/include -Wall -Wextra -DUSE_OS_ABSTRACTION
LDFLAGS = -L../../../os_abstraction_layer/build -los_abstract -lpthread

# List of examples
EXAMPLES = mealy_ui_example efsm_protocol_example moore_hierarchical_example acceptor_regex_example parallel_fsm_example data_pipeline_os_example task_pipeline_os_example

all: $(EXAMPLES)

mealy_ui_example: mealy_ui_example.c ../../src/fsm_os_adapter.c ../../src/mealy_ui.c ../../src/mealy_machine.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

efsm_protocol_example: efsm_protocol_example.c ../../src/fsm_os_adapter.c ../../src/efsm_protocol.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

moore_hierarchical_example: moore_hierarchical_example.c ../../src/fsm_os_adapter.c ../../src/moore_hierarchical.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

acceptor_regex_example: acceptor_regex_example.c ../../src/fsm_os_adapter.c ../../src/acceptor_regex.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

parallel_fsm_example: parallel_fsm_example.c ../../src/fsm_os_adapter.c ../../src/parallel_fsm.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

data_pipeline_os_example: data_pipeline_os_example.c ../../src/fsm_os_adapter.c ../../src/parallel_fsm.c ../../examples/product/system/pipeline/data_pipeline/src/data_pipeline.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

task_pipeline_os_example: task_pipeline_os_example.c ../../src/fsm_os_adapter.c ../../src/efsm_protocol.c ../../examples/product/system/pipeline/task_pipeline/src/task_pipeline.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(EXAMPLES) *.o

.PHONY: all clean
