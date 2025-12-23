# Makefile for PAL OS Adapter Examples
#
# Builds examples demonstrating integration of Platform Abstraction Layer
# with State Machine Extended and VIPER architecture.

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I../include -I../../os_abstraction_layer/include -I../../state_machine_extended/src -I../../viper_architecture/include
LDFLAGS = -L../../os_abstraction_layer/build -L../../state_machine_extended/build -L../../viper_architecture/build
LIBS = -los_abstract -lfsm_extended -lviper -lm

# SDL2 libraries for Windows simulator (if available)
SDL2_CFLAGS = $(shell pkg-config --cflags sdl2 2>/dev/null || echo "")
SDL2_LIBS = $(shell pkg-config --libs sdl2 2>/dev/null || echo "-lSDL2 -lSDL2_ttf")

# Platform-specific flags
ifeq ($(OS),Windows_NT)
	CFLAGS += -DWINDOWS -D_WIN32
	LDFLAGS += -lws2_32
else
	CFLAGS += -DLINUX
endif

# Targets
TARGETS = pal_os_adapter_example

all: $(TARGETS)

# Main example
pal_os_adapter_example: pal_os_adapter_example.c ../src/pal_os_adapter.c
	$(CC) $(CFLAGS) $(SDL2_CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS) $(SDL2_LIBS)

# Simple GPIO example
gpio_example: gpio_example.c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(LIBS)

# Clean
clean:
	rm -f $(TARGETS) *.o *.exe

# Run examples
run: pal_os_adapter_example
	./pal_os_adapter_example

# Build for simulation (no hardware dependencies)
simulation: CFLAGS += -DSIMULATION -DPAL_SIMULATION
simulation: all

# Build for STM32 (cross-compilation example)
stm32: CC = arm-none-eabi-gcc
stm32: CFLAGS = -mcpu=cortex-m4 -mthumb -Wall -Wextra -std=c99 -I../include -I../../os_abstraction_layer/include -DSTM32 -DUSE_HAL_DRIVER
stm32: LDFLAGS = -TSTM32F407VGTx_FLASH.ld -specs=nosys.specs -specs=nano.specs -Wl,--gc-sections
stm32: LIBS = -lc -lm -lnosys
stm32: pal_os_adapter_example.c ../src/pal_os_adapter.c
	$(CC) $(CFLAGS) -o pal_os_adapter_stm32.elf $^ $(LDFLAGS) $(LIBS)

.PHONY: all clean run simulation stm32
