/**
 * M.c  2023-02-19
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
#include "M.h"

static void M_ops_put_here(struct M *M)
{
	_MY_TRACE_STR("M::put_here()\n");
}

static void M_ops_put_there(struct M *M)
{
	_MY_TRACE_STR("M::put_there()\n");
}
static struct M_ops M_ops = {
	.put_here = M_ops_put_here,
	.put_there = M_ops_put_there,
};

/** constructor(). */
void M_init(struct M *M)
{
	_MY_TRACE_STR("M_init()\n");
	memset(M, 0, sizeof(*M));
	M->ops = &M_ops;
}