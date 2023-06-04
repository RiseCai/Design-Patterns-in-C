/**
 * shape_rectangle.c  2023-06-03
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
#include "shape_rectangle.h"

/** called by free(): put resources, forward to super. */
static void shape_rectangle_ops__destructor(struct shape *shape)
{
	TODO(Please add our **destructor** code here ...)
	/** PSEUDOCODE
	... do_something() to put resources ...
	printf("shape_rectangle::_destructor()\n");
	CLASS_SUPER(shape, _destructor);
	*/
}
/** free memory after call destructor(). */
static void shape_rectangle_ops_free(struct shape *shape)
{
	TODO(Please add our **free** code here ...)
	/** PSEUDOCODE
	struct shape_rectangle *a_shape_rectangle = container_of(shape, typeof(*a_shape_rectangle), shape);
	shape__destructor(shape);
	printf("shape_rectangle::free()\n");
	free(a_shape_rectangle);
	*/
}

static void shape_rectangle_ops_draw(struct shape *shape)
{
	/* struct shape_rectangle *a_shape_rectangle = container_of(shape, typeof(*a_shape_rectangle), shape);
	*/
	printf("shape_rectangle::draw()\n");
}

static struct shape_ops shape_ops = {
	._destructor = shape_rectangle_ops__destructor,
	.free = shape_rectangle_ops_free,
	.draw = shape_rectangle_ops_draw,
};

/** constructor(). */
void shape_rectangle_init(struct shape_rectangle *shape_rectangle)
{
	printf("shape_rectangle_init()\n");
	memset(shape_rectangle, 0, sizeof(*shape_rectangle));
	shape_init(&shape_rectangle->shape);
	CLASS_OPS_INIT_SUPER(shape_rectangle->shape.ops, shape_ops);
}