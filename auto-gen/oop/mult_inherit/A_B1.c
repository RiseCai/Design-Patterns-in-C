/**
 * A_B1.c  2014-05-14
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
#include "A_B1.h"

/** called by free(): put resources, forward to super. */
static void A_B1_ops__destructor(struct A *A)
{
	TODO(Please add our **destructor** code here ...)
	/** PSEUDOCODE
	... do_something() to put resources ...
	_MY_TRACE_STR("A_B1::_destructor()\n");
	CLASS_SUPER(A, _destructor);
	*/
}
/** free memory after call destructor(). */
static void A_B1_ops_free(struct A *A)
{
	TODO(Please add our **free** code here ...)
	/** PSEUDOCODE
	struct A_B1 *a_A_B1 = container_of(A, typeof(*a_A_B1), A);
	A__destructor(A);
	_MY_TRACE_STR("A_B1::free()\n");
	free(a_A_B1);
	*/
}

static void A_B1_ops_do_this(struct A *A)
{
	/* struct A_B1 *a_A_B1 = container_of(A, typeof(*a_A_B1), A);
	*/
	_MY_TRACE_STR("A_B1::do_this()\n");
}

static void A_B1_ops_do_that(struct A *A)
{
	/* struct A_B1 *a_A_B1 = container_of(A, typeof(*a_A_B1), A);
	*/
	_MY_TRACE_STR("A_B1::do_that()\n");
}

static struct A_ops A_ops = {
	._destructor = A_B1_ops__destructor,
	.free = A_B1_ops_free,
	.do_this = A_B1_ops_do_this,
	.do_that = A_B1_ops_do_that,
};

/** constructor(). */
void A_B1_init(struct A_B1 *A_B1)
{
	memset(A_B1, 0, sizeof(*A_B1));
	A_init(&A_B1->A);
	CLASS_OPS_INIT_SUPER(A_B1->A.ops, A_ops);
}