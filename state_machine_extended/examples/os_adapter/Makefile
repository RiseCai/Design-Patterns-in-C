# Makefile for building OS adapter examples
# Assumes OS abstraction layer library is installed in ../os_abstraction_layer/build

CC = gcc
CFLAGS = -I../../src -I../../../os_abstraction_layer/include -Wall -Wextra -DUSE_OS_ABSTRACTION
LDFLAGS = -L../../../os_abstraction_layer/build -los_abstract -lpthread

# List of examples
EXAMPLES = mealy_ui_example efsm_protocol_example moore_hierarchical_example acceptor_regex_example parallel_fsm_example

all: $(EXAMPLES)

%_example: %_example.c ../../src/fsm_os_adapter.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

clean:
	rm -f $(EXAMPLES) *.o

.PHONY: all clean
