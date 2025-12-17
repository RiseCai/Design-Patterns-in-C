/**
 * os_timer.h - OS Abstraction Layer Timer Interface
 * 
 * Copyright (C) 2000-2025 All Right Reserved
 * 
 * This file defines the timer abstraction interface.
 */

#ifndef __OS_TIMER_H__
#define __OS_TIMER_H__

#include "os_abstract.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Additional timer-specific APIs */

/**
 * Check if timer is running
 * @param timer Timer handle
 * @return true if running, false otherwise
 */
bool os_timer_is_running(os_timer_t *timer);

/**
 * Get timer period
 * @param timer Timer handle
 * @param[out] period_ms Period in milliseconds
 * @return OS_OK on success, error code otherwise
 */
os_error_t os_timer_get_period(os_timer_t *timer, uint32_t *period_ms);

/**
 * Change timer period (if supported)
 * @param timer Timer handle
 * @param period_ms New period in milliseconds
 * @return OS_OK on success, error code otherwise
 */
os_error_t os_timer_change_period(os_timer_t *timer, uint32_t period_ms);

#ifdef __cplusplus
}
#endif

#endif /* __OS_TIMER_H__ */
