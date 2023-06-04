/**
 * stack_hanoi.c  2023-06-03
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
#include "stack_hanoi.h"

/** called by free(): put resources, forward to super. */
static void stack_hanoi_ops__destructor(struct stack *stack)
{
	TODO(Please add our **destructor** code here ...)
	/** PSEUDOCODE
	... do_something() to put resources ...
	printf("stack_hanoi::_destructor()\n");
	CLASS_SUPER(stack, _destructor);
	*/
}
/** free memory after call destructor(). */
static void stack_hanoi_ops_free(struct stack *stack)
{
	TODO(Please add our **free** code here ...)
	/** PSEUDOCODE
	struct stack_hanoi *a_stack_hanoi = container_of(stack, typeof(*a_stack_hanoi), stack);
	stack__destructor(stack);
	printf("stack_hanoi::free()\n");
	free(a_stack_hanoi);
	*/
}

static void stack_hanoi_ops_push(struct stack *stack, int val)
{
	/* struct stack_hanoi *a_stack_hanoi = container_of(stack, typeof(*a_stack_hanoi), stack);
	*/
	printf("stack_hanoi::push()\n");
}

static struct stack_ops stack_ops = {
	._destructor = stack_hanoi_ops__destructor,
	.free = stack_hanoi_ops_free,
	.push = stack_hanoi_ops_push,
};

/** constructor(). */
void stack_hanoi_init(struct stack_hanoi *stack_hanoi)
{
	printf("stack_hanoi_init()\n");
	memset(stack_hanoi, 0, sizeof(*stack_hanoi));
	stack_init(&stack_hanoi->stack);
	CLASS_OPS_INIT_SUPER(stack_hanoi->stack.ops, stack_ops);
}