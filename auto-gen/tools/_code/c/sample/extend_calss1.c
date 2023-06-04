/**
 * extend_calss1.c  2023-06-03
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
#include "extend_calss1.h"

static int extend_calss1_ops_eval(struct base_class *base_class)
{
	/* struct extend_calss1 *a_extend_calss1 = container_of(base_class, typeof(*a_extend_calss1), base_class);
	*/
	_MY_TRACE_STR("extend_calss1::eval()\n");
	return 0;
}

static int extend_calss1_ops_eval(struct base_class *base_class)
{
	/* struct extend_calss1 *a_extend_calss1 = container_of(base_class, typeof(*a_extend_calss1), base_class);
	*/
	_MY_TRACE_STR("extend_calss1::eval()\n");
	return 0;
}

static struct base_class_ops base_class_ops = {
	.eval = extend_calss1_ops_eval,
	.eval = extend_calss1_ops_eval,
};

/** constructor(). */
void extend_calss1_init(struct extend_calss1 *extend_calss1, char *name)
{
	_MY_TRACE_STR("extend_calss1_init()\n");
	memset(extend_calss1, 0, sizeof(*extend_calss1));
	base_class_init(&extend_calss1->base_class);
	CLASS_OPS_INIT(extend_calss1->base_class.ops, base_class_ops);
	extend_calss1->_name = name;
}