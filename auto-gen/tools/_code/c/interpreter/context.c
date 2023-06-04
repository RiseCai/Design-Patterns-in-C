/**
 * context.c  2023-06-03
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
 * Uses a class hierarchy to represent the grammar given.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>

#include "context.h"

/** constructor(). */
void context_init(struct context *context)
{
	_MY_TRACE_STR("context_init()\n");
	memset(context, 0, sizeof(*context));
}

int context_get_value(struct context *context, char *name)
{
	_MY_TRACE_STR("context::get_value()\n");
	return context->_value;
}