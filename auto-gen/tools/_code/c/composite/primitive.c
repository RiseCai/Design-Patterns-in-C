/**
 * primitive.c  2023-06-03
 * anonymouse(anonymouse@email)
 *
 * Copyright (C) 2000-2014 All Right Reserved
 * 
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Auto generate for Design Patterns in C
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <myobj.h>
#include "primitive.h"

static void primitive_ops_traverse(struct component *component)
{
	/* struct primitive *a_primitive = container_of(component, typeof(*a_primitive), component);
	*/
	_MY_TRACE_STR("primitive::traverse()\n");
}

static struct component_ops component_ops = {
	.traverse = primitive_ops_traverse,
};

/** constructor(). */
void primitive_init(struct primitive *primitive)
{
	_MY_TRACE_STR("primitive_init()\n");
	memset(primitive, 0, sizeof(*primitive));
	component_init(&primitive->component);
	CLASS_OPS_INIT_SUPER(primitive->component.ops, component_ops);
}