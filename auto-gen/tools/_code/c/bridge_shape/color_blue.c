/**
 * color_blue.c  2023-06-03
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
#include "color_blue.h"

static void color_blue_ops_render(struct color *color)
{
	/* struct color_blue *a_color_blue = container_of(color, typeof(*a_color_blue), color);
	*/
	printf("color_blue::render()\n");
}

static struct color_ops color_ops = {
	.render = color_blue_ops_render,
};

/** constructor(). */
void color_blue_init(struct color_blue *color_blue)
{
	printf("color_blue_init()\n");
	memset(color_blue, 0, sizeof(*color_blue));
	color_init(&color_blue->color);
	CLASS_OPS_INIT(color_blue->color.ops, color_ops);
}