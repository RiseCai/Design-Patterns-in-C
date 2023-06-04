/**
 * extend_class2.c  2023-06-03
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
#include "extend_class2.h"

static int extend_class2_ops_eval(struct base_class *base_class)
{
	/* struct extend_class2 *a_extend_class2 = container_of(base_class, typeof(*a_extend_class2), base_class);
	*/
	_MY_TRACE_STR("extend_class2::eval()\n");
	return 0;
}

static int extend_class2_ops_eval(struct base_class *base_class)
{
	/* struct extend_class2 *a_extend_class2 = container_of(base_class, typeof(*a_extend_class2), base_class);
	*/
	_MY_TRACE_STR("extend_class2::eval()\n");
	return 0;
}

static struct base_class_ops base_class_ops = {
	.eval = extend_class2_ops_eval,
	.eval = extend_class2_ops_eval,
};

/** constructor(). */
void extend_class2_init(struct extend_class2 *extend_class2, char *name)
{
	_MY_TRACE_STR("extend_class2_init()\n");
	memset(extend_class2, 0, sizeof(*extend_class2));
	base_class_init(&extend_class2->base_class);
	CLASS_OPS_INIT(extend_class2->base_class.ops, base_class_ops);
	extend_class2->_name = name;
}