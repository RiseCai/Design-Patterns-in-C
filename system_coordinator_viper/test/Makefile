# Makefile for System Coordinator VIPER tests

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -DMYTRACE_NO_EXECINFO=1 -I. \
         -I../include \
         -I../../state_machine_extended/src \
         -I../../auto-gen/util \
         -I../../viper_architecture/include \
         -I../../platform_abstraction_layer/include \
         -I../../os_abstraction_layer/include
LDFLAGS = -lm

# State machine source files
STATE_MACHINE_SRCS = ../../state_machine_extended/src/mealy_machine.c \
                     ../../state_machine_extended/src/parallel_fsm.c \
                     ../../state_machine_extended/src/moore_hierarchical.c \
                     ../../state_machine_extended/src/efsm_protocol.c \
                     ../../state_machine_extended/src/acceptor_regex.c \
                     ../../state_machine_extended/src/fsm_os_adapter.c \
                     ../../os_abstraction_layer/build/os_abstract_stub.c

# SCV source files
SCV_SRCS = ../src/scv.c \
           ../src/scv_view.c \
           ../src/scv_presenter.c \
           ../src/scv_interactor.c \
           ../src/scv_entity.c \
           ../src/scv_router.c \
           ../src/scv_event_bus.c

# Test source files
TEST_SRCS = test_scv_presenter.c test_scv_entity.c test_scv_interactor.c test_scv_integration.c test_scv_router.c test_scv_user_scenarios.c test_scv_comprehensive.c

# Test executables
TEST_EXES = $(TEST_SRCS:.c=)

# Default target: build all tests
all: $(TEST_EXES)

# Build test_scv_presenter
test_scv_presenter: test_scv_presenter.c ../src/scv_presenter.c ../src/scv_entity.c ../src/scv_interactor.c ../src/scv_event_bus.c $(STATE_MACHINE_SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Build test_scv_entity
test_scv_entity: test_scv_entity.c ../src/scv_entity.c ../src/scv_event_bus.c $(STATE_MACHINE_SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Build test_scv_interactor
test_scv_interactor: test_scv_interactor.c ../src/scv_interactor.c ../src/scv_entity.c ../src/scv_event_bus.c $(STATE_MACHINE_SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Build test_scv_integration
test_scv_integration: test_scv_integration.c $(SCV_SRCS) $(STATE_MACHINE_SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Build test_scv_router
test_scv_router: test_scv_router.c ../src/scv_router.c ../src/scv_entity.c ../src/scv_interactor.c ../src/scv_presenter.c ../src/scv_view.c ../src/scv_event_bus.c $(STATE_MACHINE_SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Build test_scv_user_scenarios
test_scv_user_scenarios: test_scv_user_scenarios.c $(SCV_SRCS) $(STATE_MACHINE_SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Build test_scv_comprehensive
test_scv_comprehensive: test_scv_comprehensive.c $(SCV_SRCS) $(STATE_MACHINE_SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Run all tests
run: all
	@echo "Running all tests..."
	@for test in $(TEST_EXES); do \
		echo "=== Running $$test ==="; \
		./$$test; \
	done

# CI target
ci: all
	@echo "CI: Running all tests..."
	@for test in $(TEST_EXES); do \
		echo "=== Running $$test ==="; \
		./$$test || exit 1; \
	done
	@echo "All tests passed."

# Clean up
clean:
	rm -f $(TEST_EXES) *.o

.PHONY: all run ci clean
