cmake_minimum_required(VERSION 3.10)
project(system_coordinator_module C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Source files
set(SRC_FILES
    src/system_coordinator.c
    src/external_command_processor.c
    src/external_command_config.c
    src/audio_fsm.c
    src/comm_fsm.c
    src/ota_fsm.c
    src/power_fsm.c
    src/recording_fsm.c
)

# Include directories
include_directories(include)
include_directories(src)

# Create library
add_library(system_coordinator ${SRC_FILES})

# Create executable for testing
add_executable(test_system_coordinator tests/test_system_coordinator_integration.c)
target_link_libraries(test_system_coordinator system_coordinator)

# Additional test for recording FSM
add_executable(test_recording_fsm tests/test_recording_fsm.c)
target_link_libraries(test_recording_fsm system_coordinator)

# Optional: enable pthread if needed
find_package(Threads)
if(Threads_FOUND)
    target_link_libraries(test_system_coordinator Threads::Threads)
    target_link_libraries(test_recording_fsm Threads::Threads)
endif()

# Install
install(TARGETS system_coordinator DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)
