/**
 * core.h  2023-06-03
 * anonymouse(anonymouse@email)
 *
 * Copyright (C) 2000-2014 All Right Reserved
 * 
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Auto generate for Design Patterns in C *
 * 
 * When parse package, use wrapper to add header or remove header.
 */
#ifndef __CORE_H__
#define __CORE_H__

#include <mycommon.h>
#include <mytrace.h>

#include "interface.h"

struct core {
	struct interface interface;
};

/** constructor(). */
#define core_init(...) _Static_assert(0, "private constructor, cannot be called directly.")

#endif /* __CORE_H__ */