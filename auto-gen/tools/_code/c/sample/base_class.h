/**
 * base_class.h  2023-06-03
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
 * The info will insert to every source file explaining the pattern's design details.
    We also can append lists like this:
      - list 1, give point one
          detail provide for point one
      - list 2, offer point two
 */
#ifndef __BASE_CLASS_H__
#define __BASE_CLASS_H__

#include <mycommon.h>
#include <mytrace.h>

struct base_class_ops;

struct base_class {
	struct base_class_ops *ops;
};
struct base_class_ops {
	int (*eval)(struct base_class *);
	int (*eval)(struct base_class *);
};

/** constructor(). */
void base_class_init(struct base_class *base_class);

static inline int base_class_eval(struct base_class *base_class)
{
	return base_class->ops->eval(base_class);
}

static inline int base_class_eval(struct base_class *base_class)
{
	return base_class->ops->eval(base_class);
}

#endif /* __BASE_CLASS_H__ */