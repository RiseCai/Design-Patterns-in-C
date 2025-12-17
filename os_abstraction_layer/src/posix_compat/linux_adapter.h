/**
 * linux_adapter.h - Linux POSIX Adapter for OS Abstraction Layer
 *
 * Copyright (C) 2000-2025 All Right Reserved
 *
 * This file implements the Linux-specific adapter using POSIX APIs.
 */

#ifndef __LINUX_ADAPTER_H__
#define __LINUX_ADAPTER_H__

#include "os_abstract.h"
#include "os_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Linux-specific implementation structure */
extern const struct os_impl linux_impl;

/* Linux-specific helper functions */
int linux_to_posix_error(int err);
int posix_to_os_error(int err);

#ifdef __cplusplus
}
#endif

#endif /* __LINUX_ADAPTER_H__ */
