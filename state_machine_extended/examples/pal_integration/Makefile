# Makefile for PAL FSM Integration Example
#
# Copyright (C) 2000-2025 All Right Reserved

# ==================== Configuration ====================

# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -std=c99 -g -I../../src -I../../../platform_abstraction_layer/include -I../../examples/product/ui/led

# Linker flags
LDFLAGS = -L../../../platform_abstraction_layer/build -lpal -lpthread

# Source files
SRCS = pal_fsm_adapter.c pal_led_fsm_example.c

# Object files
OBJS = $(SRCS:.c=.o)

# Executable
TARGET = pal_led_fsm_example

# ==================== Build Rules ====================

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

pal_fsm_adapter.o: pal_fsm_adapter.c pal_fsm_adapter.h
	$(CC) $(CFLAGS) -c $< -o $@

pal_led_fsm_example.o: pal_led_fsm_example.c pal_fsm_adapter.h
	$(CC) $(CFLAGS) -c $< -o $@

# ==================== Dependencies ====================

# LED FSM dependencies
LED_FSM_SRCS = ../../examples/product/ui/led/led_fsm.c
LED_FSM_OBJS = led_fsm.o

led_fsm.o: $(LED_FSM_SRCS)
	$(CC) $(CFLAGS) -c $< -o $@

# FSM OS adapter dependencies
FSM_OS_SRCS = ../../src/fsm_os_adapter.c
FSM_OS_OBJS = fsm_os_adapter.o

fsm_os_adapter.o: $(FSM_OS_SRCS)
	$(CC) $(CFLAGS) -c $< -o $@

# ==================== Clean ====================

clean:
	rm -f $(OBJS) $(LED_FSM_OBJS) $(FSM_OS_OBJS) $(TARGET)

distclean: clean
	rm -f *.o *~ core

# ==================== Run ====================

run: $(TARGET)
	./$(TARGET)

# ==================== Test ====================

test: $(TARGET)
	@echo "Running PAL LED FSM example..."
	./$(TARGET)

# ==================== Platform-specific builds ====================

windows: CFLAGS += -DWINDOWS
windows: all

linux: CFLAGS += -DLINUX
linux: all

# ==================== Documentation ====================

docs:
	@echo "Generating documentation..."
	doxygen Doxyfile 2>/dev/null || echo "Doxygen not installed"

# ==================== Help ====================

help:
	@echo "Available targets:"
	@echo "  all       - Build the example (default)"
	@echo "  clean     - Remove object files and executable"
	@echo "  distclean - Remove all generated files"
	@echo "  run       - Build and run the example"
	@echo "  test      - Build and test the example"
	@echo "  windows   - Build for Windows"
	@echo "  linux     - Build for Linux"
	@echo "  docs      - Generate documentation"
	@echo "  help      - Show this help message"

.PHONY: all clean distclean run test windows linux docs help
