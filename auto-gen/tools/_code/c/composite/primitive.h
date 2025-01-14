/**
 * primitive.h  2023-06-03
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
#ifndef __PRIMITIVE_H__
#define __PRIMITIVE_H__

#include <mycommon.h>
#include <mytrace.h>

#include "component.h"

struct primitive {
	struct component component;
	int value;
};

/** constructor(). */
void primitive_init(struct primitive *primitive);

#endif /* __PRIMITIVE_H__ */