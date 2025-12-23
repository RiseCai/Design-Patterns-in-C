/**
 * pal.h - Platform Abstraction Layer Main Interface
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Auto generate for Design-Patterns in C *
 * 
 * Platform Abstraction Layer for embedded hardware platforms.
 * Provides hardware-independent interfaces for GPIO, PWM, ADC, etc.
 */

#ifndef __PAL_H__
#define __PAL_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== Platform Configuration ==================== */

/**
 * Platform types supported by PAL
 */
typedef enum {
    PAL_PLATFORM_WINDOWS_SIMULATOR = 0,  /* Windows simulation environment */
    PAL_PLATFORM_LINUX_SIMULATOR,        /* Linux simulation environment */
    PAL_PLATFORM_STM32_F4,               /* STM32F4 series */
    PAL_PLATFORM_ESP32,                  /* ESP32 series */
    PAL_PLATFORM_COUNT                   /* Total number of platforms */
} pal_platform_t;

/**
 * PAL initialization configuration
 */
typedef struct {
    pal_platform_t platform;      /* Target platform */
    const char *config_file;      /* Configuration file path (optional) */
    uint32_t simulation_speed;    /* Simulation speed multiplier (1x = real-time) */
    bool enable_gui;              /* Enable GUI visualization (for simulators) */
    bool enable_logging;          /* Enable logging of hardware operations */
} pal_config_t;

/* ==================== Common Data Types ==================== */

/**
 * Generic status codes
 */
typedef enum {
    PAL_STATUS_OK = 0,
    PAL_STATUS_ERROR,
    PAL_STATUS_INVALID_PARAM,
    PAL_STATUS_NOT_INITIALIZED,
    PAL_STATUS_NOT_SUPPORTED,
    PAL_STATUS_TIMEOUT,
    PAL_STATUS_BUSY
} pal_status_t;

/**
 * Generic callback function type
 */
typedef void (*pal_callback_t)(void *context);

/* ==================== Performance Configuration ==================== */

/**
 * Performance configuration for simulators
 */
typedef struct {
    uint32_t target_fps;          /* Target frames per second (0 = unlimited) */
    uint32_t update_frequency_hz; /* Hardware update frequency */
    bool vsync_enabled;           /* Enable vertical sync */
    bool limit_cpu_usage;         /* Limit CPU usage when idle */
    uint32_t max_frame_time_ms;   /* Maximum frame time before warning */
} pal_performance_config_t;

/* ==================== Public API ==================== */

/**
 * Initialize the Platform Abstraction Layer
 * 
 * @param config Configuration for PAL initialization
 * @return PAL_STATUS_OK on success, error code otherwise
 */
pal_status_t pal_init(const pal_config_t *config);

/**
 * Deinitialize the Platform Abstraction Layer
 * 
 * @return PAL_STATUS_OK on success, error code otherwise
 */
pal_status_t pal_deinit(void);

/**
 * Get current platform type
 * 
 * @return Platform type
 */
pal_platform_t pal_get_platform(void);

/**
 * Get PAL version string
 * 
 * @return Version string
 */
const char *pal_get_version(void);

/**
 * Get PAL status string
 * 
 * @return Status description
 */
const char *pal_get_status_string(void);

/**
 * Set performance configuration
 * 
 * @param config Performance configuration
 * @return PAL_STATUS_OK on success, error code otherwise
 */
pal_status_t pal_set_performance_config(const pal_performance_config_t *config);

/**
 * Get current performance configuration
 * 
 * @param config Output performance configuration
 * @return PAL_STATUS_OK on success, error code otherwise
 */
pal_status_t pal_get_performance_config(pal_performance_config_t *config);

/**
 * Process platform events (call periodically in main loop)
 * This is particularly important for simulators with GUI
 * 
 * @return true if application should continue, false if should exit
 */
bool pal_process_events(void);

/**
 * Get elapsed time since PAL initialization in milliseconds
 * 
 * @return Elapsed time in milliseconds
 */
uint32_t pal_get_elapsed_time_ms(void);

/**
 * Delay execution for specified milliseconds
 * 
 * @param ms Milliseconds to delay
 */
void pal_delay_ms(uint32_t ms);

#ifdef __cplusplus
}
#endif

#endif /* __PAL_H__ */
