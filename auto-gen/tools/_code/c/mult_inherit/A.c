/**
 * A.c  2023-02-19
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
#include "A.h"

static void A_ops_do_this(struct A *A)
{
	_MY_TRACE_STR("A::do_this()\n");
}

static void A_ops_do_that(struct A *A)
{
	_MY_TRACE_STR("A::do_that()\n");
}
static struct A_ops A_ops = {
	.do_this = A_ops_do_this,
	.do_that = A_ops_do_that,
};

/** constructor(). */
void A_init(struct A *A)
{
	_MY_TRACE_STR("A_init()\n");
	memset(A, 0, sizeof(*A));
	A->ops = &A_ops;
}