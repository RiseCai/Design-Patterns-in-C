/**
 * shape_circle.c  2023-06-03
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
#include "shape_circle.h"

/** called by free(): put resources, forward to super. */
static void shape_circle_ops__destructor(struct shape *shape)
{
	TODO(Please add our **destructor** code here ...)
	/** PSEUDOCODE
	... do_something() to put resources ...
	printf("shape_circle::_destructor()\n");
	CLASS_SUPER(shape, _destructor);
	*/
}
/** free memory after call destructor(). */
static void shape_circle_ops_free(struct shape *shape)
{
	TODO(Please add our **free** code here ...)
	/** PSEUDOCODE
	struct shape_circle *a_shape_circle = container_of(shape, typeof(*a_shape_circle), shape);
	shape__destructor(shape);
	printf("shape_circle::free()\n");
	free(a_shape_circle);
	*/
}

static void shape_circle_ops_draw(struct shape *shape)
{
	/* struct shape_circle *a_shape_circle = container_of(shape, typeof(*a_shape_circle), shape);
	*/
	printf("shape_circle::draw()\n");
}

static struct shape_ops shape_ops = {
	._destructor = shape_circle_ops__destructor,
	.free = shape_circle_ops_free,
	.draw = shape_circle_ops_draw,
};

/** constructor(). */
void shape_circle_init(struct shape_circle *shape_circle)
{
	printf("shape_circle_init()\n");
	memset(shape_circle, 0, sizeof(*shape_circle));
	shape_init(&shape_circle->shape);
	CLASS_OPS_INIT_SUPER(shape_circle->shape.ops, shape_ops);
}