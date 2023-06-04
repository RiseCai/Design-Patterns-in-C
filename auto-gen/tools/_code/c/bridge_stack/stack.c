/**
 * stack.c  2023-06-03
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
#include "stack.h"

/** called by free(): put resources, forward to super. */
static void stack_ops__destructor(struct stack *stack)
{
	TODO(Please add our **destructor** code here ...)
	printf("stack::_destructor()\n");
}
/** free memory after call destructor(). */
static void stack_ops_free(struct stack *stack)
{
	TODO(Please add our **free** code here ...)
	/** PSEUDOCODE
	stack__destructor(stack);
	printf("stack::free()\n");
	free(stack);
	*/
}

static void stack_ops_push(struct stack *stack, int val)
{
	printf("stack::push()\n");
}

static int stack_ops_pop(struct stack *stack)
{
	printf("stack::pop()\n");
	return 0;
}

static int stack_ops_top(struct stack *stack)
{
	printf("stack::top()\n");
	return 0;
}

static int stack_ops_is_empty(struct stack *stack)
{
	printf("stack::is_empty()\n");
	return 0;
}

static int stack_ops_is_full(struct stack *stack)
{
	printf("stack::is_full()\n");
	return 0;
}
static struct stack_ops stack_ops = {
	._destructor = stack_ops__destructor,
	.free = stack_ops_free,
	.push = stack_ops_push,
	.pop = stack_ops_pop,
	.top = stack_ops_top,
	.is_empty = stack_ops_is_empty,
	.is_full = stack_ops_is_full,
};

/** constructor(). */
void stack_init(struct stack *stack)
{
	printf("stack_init()\n");
	memset(stack, 0, sizeof(*stack));
	stack->ops = &stack_ops;
}