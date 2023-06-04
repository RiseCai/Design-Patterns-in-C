/**
 * stack_impl.c  2023-06-03
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
#include "stack_impl.h"

/** called by free(): put resources, forward to super. */
static void stack_impl_ops__destructor(struct stack_impl *stack_impl)
{
	TODO(Please add our **destructor** code here ...)
	printf("stack_impl::_destructor()\n");
}
/** free memory after call destructor(). */
static void stack_impl_ops_free(struct stack_impl *stack_impl)
{
	TODO(Please add our **free** code here ...)
	/** PSEUDOCODE
	stack_impl__destructor(stack_impl);
	printf("stack_impl::free()\n");
	free(stack_impl);
	*/
}

static void stack_impl_ops_push(struct stack_impl *stack_impl, int val)
{
	printf("stack_impl::push()\n");
}

static int stack_impl_ops_pop(struct stack_impl *stack_impl)
{
	printf("stack_impl::pop()\n");
	return 0;
}

static int stack_impl_ops_top(struct stack_impl *stack_impl)
{
	printf("stack_impl::top()\n");
	return 0;
}

static int stack_impl_ops_is_empty(struct stack_impl *stack_impl)
{
	printf("stack_impl::is_empty()\n");
	return 0;
}

static int stack_impl_ops_is_full(struct stack_impl *stack_impl)
{
	printf("stack_impl::is_full()\n");
	return 0;
}
static struct stack_impl_ops stack_impl_ops = {
	._destructor = stack_impl_ops__destructor,
	.free = stack_impl_ops_free,
	.push = stack_impl_ops_push,
	.pop = stack_impl_ops_pop,
	.top = stack_impl_ops_top,
	.is_empty = stack_impl_ops_is_empty,
	.is_full = stack_impl_ops_is_full,
};

/** constructor(). */
void stack_impl_init(struct stack_impl *stack_impl)
{
	printf("stack_impl_init()\n");
	memset(stack_impl, 0, sizeof(*stack_impl));
	stack_impl->ops = &stack_impl_ops;
}