# Makefile for VIPER architecture tests

CC = gcc
SDL2_INC = $(SDL2_PATH)/include
SDL2_TTF_INC = $(SDL2_TTF_PATH)/include
CFLAGS = -Wall -Wextra -std=c99 -I../include -I../../state_machine_extended/src \
         -I../../platform_abstraction_layer/include -I../../os_abstraction_layer/include \
         -I$(SDL2_INC) -I$(SDL2_TTF_INC)
LDFLAGS = -lm

# SDL2 libraries (allow override via environment)
SDL2_PATH ?= ../../SDL2/SDL2-2.30.5
SDL2_TTF_PATH ?= ../../SDL2_ttf/SDL2_ttf-2.22.0
SDL2_LIB_PATH = $(SDL2_PATH)/lib/x64
SDL2_TTF_LIB_PATH = $(SDL2_TTF_PATH)/lib/x64
SDL_LIBS = -L$(SDL2_LIB_PATH) -L$(SDL2_TTF_LIB_PATH) -lSDL2 -lSDL2_ttf

# State machine source files
STATE_MACHINE_SRCS = ../../state_machine_extended/src/mealy_machine.c \
                     ../../state_machine_extended/src/parallel_fsm.c \
                     ../../state_machine_extended/src/moore_hierarchical.c \
                     ../../state_machine_extended/src/efsm_protocol.c \
                     ../../state_machine_extended/src/fsm_os_adapter.c \
                     ../../os_abstraction_layer/build/os_abstract_stub.c

# Test source files
TEST_SRCS = test_viper_module.c \
            test_integration_todo.c \
            test_pal_simulation.c \
            test_viper_view.c \
            test_viper_interactor.c \
            test_viper_entity.c \
            test_viper_presenter.c

# Test executables
TEST_EXES = $(TEST_SRCS:.c=)

# Default target: build all tests
all: $(TEST_EXES)

# Build each test executable
test_viper_module: test_viper_module.c ../src/viper.c ../src/viper_view.c ../src/viper_presenter.c ../src/viper_interactor.c ../src/viper_entity.c $(STATE_MACHINE_SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

test_integration_todo: test_integration_todo.c ../src/viper.c ../src/viper_view.c ../src/viper_presenter.c ../src/viper_interactor.c ../src/viper_entity.c $(STATE_MACHINE_SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

test_pal_simulation: test_pal_simulation.c ../src/viper.c ../src/viper_view.c ../src/viper_presenter.c ../src/viper_interactor.c ../src/viper_entity.c \
                     ../../platform_abstraction_layer/src/windows_simulator/pal_windows_simulator.c \
                     ../../platform_abstraction_layer/src/pal_os_adapter.c \
                     ../../os_abstraction_layer/build/os_abstract_stub.c \
                     $(STATE_MACHINE_SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS) $(SDL_LIBS)

test_viper_view: test_viper_view.c ../src/viper.c ../src/viper_view.c ../src/viper_presenter.c ../src/viper_interactor.c ../src/viper_entity.c $(STATE_MACHINE_SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

test_viper_interactor: test_viper_interactor.c ../src/viper.c ../src/viper_view.c ../src/viper_presenter.c ../src/viper_interactor.c ../src/viper_entity.c $(STATE_MACHINE_SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

test_viper_entity: test_viper_entity.c ../src/viper.c ../src/viper_view.c ../src/viper_presenter.c ../src/viper_interactor.c ../src/viper_entity.c $(STATE_MACHINE_SRCS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

test_viper_presenter: test_viper_presenter.c ../src/viper.c ../src/viper_view.c ../src/viper_presenter.c ../src/viper_interactor.c ../src/viper_entity.c $(STATE_MACHINE_SRCS)
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
