/**
 * base_class.c  2023-06-03
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <myobj.h>
#include "base_class.h"

static int base_class_ops_eval(struct base_class *base_class)
{
	_MY_TRACE_STR("base_class::eval()\n");
	return 0;
}
static struct base_class_ops base_class_ops = {
	.eval = base_class_ops_eval,
};

/** constructor(). */
void base_class_init(struct base_class *base_class)
{
	_MY_TRACE_STR("base_class_init()\n");
	memset(base_class, 0, sizeof(*base_class));
	base_class->ops = &base_class_ops;
}