/**
 * stack_fifo.c  2023-06-03
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
#include "stack_fifo.h"

/** called by free(): put resources, forward to super. */
static void stack_fifo_ops__destructor(struct stack *stack)
{
	TODO(Please add our **destructor** code here ...)
	/** PSEUDOCODE
	... do_something() to put resources ...
	printf("stack_fifo::_destructor()\n");
	CLASS_SUPER(stack, _destructor);
	*/
}
/** free memory after call destructor(). */
static void stack_fifo_ops_free(struct stack *stack)
{
	TODO(Please add our **free** code here ...)
	/** PSEUDOCODE
	struct stack_fifo *a_stack_fifo = container_of(stack, typeof(*a_stack_fifo), stack);
	stack__destructor(stack);
	printf("stack_fifo::free()\n");
	free(a_stack_fifo);
	*/
}

static int stack_fifo_ops_pop(struct stack *stack)
{
	/* struct stack_fifo *a_stack_fifo = container_of(stack, typeof(*a_stack_fifo), stack);
	*/
	printf("stack_fifo::pop()\n");
	return 0;
}

static struct stack_ops stack_ops = {
	._destructor = stack_fifo_ops__destructor,
	.free = stack_fifo_ops_free,
	.pop = stack_fifo_ops_pop,
};

/** constructor(). */
void stack_fifo_init(struct stack_fifo *stack_fifo)
{
	printf("stack_fifo_init()\n");
	memset(stack_fifo, 0, sizeof(*stack_fifo));
	stack_init(&stack_fifo->stack);
	CLASS_OPS_INIT_SUPER(stack_fifo->stack.ops, stack_ops);
}