/**
 * column.h  2023-06-03
 * anonymouse(anonymouse@email)
 *
 * Copyright (C) 2000-2014 All Right Reserved
 * 
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Auto generate for Design Patterns in C
 */
#ifndef __COLUMN_H__
#define __COLUMN_H__

#include <mycommon.h>
#include <mytrace.h>

#include "composite.h"

struct column {
	struct composite composite;
};

/** constructor(). */
void column_init(struct column *column);

#endif /* __COLUMN_H__ */