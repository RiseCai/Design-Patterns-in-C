/**
 * composite.c  2023-06-03
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
#include "composite.h"

static void composite_ops_traverse(struct component *component)
{
	/* struct composite *a_composite = container_of(component, typeof(*a_composite), component);
	*/
	_MY_TRACE_STR("composite::traverse()\n");
}

static struct component_ops component_ops = {
	.traverse = composite_ops_traverse,
};

/** constructor(). */
void composite_init(struct composite *composite)
{
	_MY_TRACE_STR("composite_init()\n");
	memset(composite, 0, sizeof(*composite));
	component_init(&composite->component);
	CLASS_OPS_INIT_SUPER(composite->component.ops, component_ops);
}

void composite_add(struct composite *composite, struct component *c)
{
	_MY_TRACE_STR("composite::add()\n");
}