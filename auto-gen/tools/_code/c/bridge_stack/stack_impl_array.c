/**
 * stack_impl_array.c  2023-06-03
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
#include "stack_impl_array.h"

/** called by free(): put resources, forward to super. */
static void stack_impl_array_ops__destructor(struct stack_impl *stack_impl)
{
	TODO(Please add our **destructor** code here ...)
	/** PSEUDOCODE
	... do_something() to put resources ...
	printf("stack_impl_array::_destructor()\n");
	CLASS_SUPER(stack_impl, _destructor);
	*/
}
/** free memory after call destructor(). */
static void stack_impl_array_ops_free(struct stack_impl *stack_impl)
{
	TODO(Please add our **free** code here ...)
	/** PSEUDOCODE
	struct stack_impl_array *a_stack_impl_array = container_of(stack_impl, typeof(*a_stack_impl_array), stack_impl);
	stack_impl__destructor(stack_impl);
	printf("stack_impl_array::free()\n");
	free(a_stack_impl_array);
	*/
}

static void stack_impl_array_ops_push(struct stack_impl *stack_impl, int val)
{
	/* struct stack_impl_array *a_stack_impl_array = container_of(stack_impl, typeof(*a_stack_impl_array), stack_impl);
	*/
	printf("stack_impl_array::push()\n");
}

static int stack_impl_array_ops_pop(struct stack_impl *stack_impl)
{
	/* struct stack_impl_array *a_stack_impl_array = container_of(stack_impl, typeof(*a_stack_impl_array), stack_impl);
	*/
	printf("stack_impl_array::pop()\n");
	return 0;
}

static int stack_impl_array_ops_top(struct stack_impl *stack_impl)
{
	/* struct stack_impl_array *a_stack_impl_array = container_of(stack_impl, typeof(*a_stack_impl_array), stack_impl);
	*/
	printf("stack_impl_array::top()\n");
	return 0;
}

static int stack_impl_array_ops_is_empty(struct stack_impl *stack_impl)
{
	/* struct stack_impl_array *a_stack_impl_array = container_of(stack_impl, typeof(*a_stack_impl_array), stack_impl);
	*/
	printf("stack_impl_array::is_empty()\n");
	return 0;
}

static int stack_impl_array_ops_is_full(struct stack_impl *stack_impl)
{
	/* struct stack_impl_array *a_stack_impl_array = container_of(stack_impl, typeof(*a_stack_impl_array), stack_impl);
	*/
	printf("stack_impl_array::is_full()\n");
	return 0;
}

static struct stack_impl_ops stack_impl_ops = {
	._destructor = stack_impl_array_ops__destructor,
	.free = stack_impl_array_ops_free,
	.push = stack_impl_array_ops_push,
	.pop = stack_impl_array_ops_pop,
	.top = stack_impl_array_ops_top,
	.is_empty = stack_impl_array_ops_is_empty,
	.is_full = stack_impl_array_ops_is_full,
};

/** constructor(). */
void stack_impl_array_init(struct stack_impl_array *stack_impl_array)
{
	printf("stack_impl_array_init()\n");
	memset(stack_impl_array, 0, sizeof(*stack_impl_array));
	stack_impl_init(&stack_impl_array->stack_impl);
	CLASS_OPS_INIT_SUPER(stack_impl_array->stack_impl.ops, stack_impl_ops);
}