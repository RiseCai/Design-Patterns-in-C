# Makefile for VIPER, PAL, OSAL integration example

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I../include -I../../state_machine_extended/src \
         -I../../platform_abstraction_layer/include -I../../os_abstraction_layer/include
LDFLAGS = -lm

# Source files (only existing files)
SRCS = viper_pal_osal_integration.c \
       ../src/viper.c \
       ../src/viper_view.c

# Object files
OBJS = $(SRCS:.c=.o)

# Target executable
TARGET = viper_pal_osal_demo

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up
clean:
	rm -f $(OBJS) $(TARGET)

# Run the demo
run: $(TARGET)
	./$(TARGET)

# Simple compilation for testing
simple:
	$(CC) $(CFLAGS) viper_pal_osal_integration.c -o simple_demo

# Todo test compilation
todo_test: viper_todo_test.c
	$(CC) $(CFLAGS) viper_todo_test.c ../src/viper.c ../src/viper_view.c ../src/viper_presenter.c ../src/viper_interactor.c ../src/viper_entity.c -I../include -I../../state_machine_extended/src -I../../platform_abstraction_layer/include -I../../os_abstraction_layer/include -o viper_todo_test

.PHONY: all clean run simple todo_test
