/**
 * shape_circle.h  2023-06-03
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
#ifndef __SHAPE_CIRCLE_H__
#define __SHAPE_CIRCLE_H__

#include <mycommon.h>

#include "shape.h"

struct shape_circle {
	struct shape shape;
};

/** constructor(). */
void shape_circle_init(struct shape_circle *shape_circle);

#endif /* __SHAPE_CIRCLE_H__ */