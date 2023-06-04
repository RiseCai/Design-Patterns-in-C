/**
 * column.c  2023-06-03
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
#include "column.h"

static struct component_ops component_ops = {0
};

/** constructor(). */
void column_init(struct column *column)
{
	_MY_TRACE_STR("column_init()\n");
	memset(column, 0, sizeof(*column));
	composite_init(&column->composite);
	CLASS_OPS_INIT_SUPER(column->composite.component.ops, component_ops);
}