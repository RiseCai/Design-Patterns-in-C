/**
 * shape.h  2023-06-03
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
#ifndef __SHAPE_H__
#define __SHAPE_H__

#include <mycommon.h>

#include "color.h"

struct shape_ops;

struct shape {
	struct shape_ops *ops;
	struct color * _color;
};
struct shape_ops {
	void (*_destructor)(struct shape *); /** called by free(): put resources, forward to super. */
	void (*free)(struct shape *); /** free memory after call destructor(). */
	void (*draw)(struct shape *);
	struct shape_ops *__super;
};

/** constructor(). */
void shape_init(struct shape *shape);

/** called by free(): put resources, forward to super. */
static inline void shape__destructor(struct shape *shape)
{
	shape->ops->_destructor(shape);
}
/** free memory after call destructor(). */
static inline void shape_free(struct shape *shape)
{
	shape->ops->free(shape);
}

static inline void shape_draw(struct shape *shape)
{
	shape->ops->draw(shape);
}

#endif /* __SHAPE_H__ */