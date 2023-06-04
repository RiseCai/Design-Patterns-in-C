/**
 * shape.c  2023-06-03
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
 * Composite better than inheritances,
 When:
 =====
 
                    ----Shape---
                   /            \
          Rectangle              Circle
         /        \            /      \
 BlueRectangle  RedRectangle BlueCircle RedCircle
 
 Refactor to:
 ============
 
           ----Shape---                        Color
          /           \                      /    \
 Rectangle(Color)   Circle(Color)           Blue   Red
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <myobj.h>
#include "shape.h"

/** called by free(): put resources, forward to super. */
static void shape_ops__destructor(struct shape *shape)
{
	TODO(Please add our **destructor** code here ...)
	printf("shape::_destructor()\n");
}
/** free memory after call destructor(). */
static void shape_ops_free(struct shape *shape)
{
	TODO(Please add our **free** code here ...)
	/** PSEUDOCODE
	shape__destructor(shape);
	printf("shape::free()\n");
	free(shape);
	*/
}

static void shape_ops_draw(struct shape *shape)
{
	printf("shape::draw()\n");
}
static struct shape_ops shape_ops = {
	._destructor = shape_ops__destructor,
	.free = shape_ops_free,
	.draw = shape_ops_draw,
};

/** constructor(). */
void shape_init(struct shape *shape)
{
	printf("shape_init()\n");
	memset(shape, 0, sizeof(*shape));
	shape->ops = &shape_ops;
}