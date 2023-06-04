/**
 * stack_fifo.h  2023-06-03
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
#ifndef __STACK_FIFO_H__
#define __STACK_FIFO_H__

#include <mycommon.h>

#include "stack.h"

struct stack_fifo {
	struct stack stack;
};

/** constructor(). */
void stack_fifo_init(struct stack_fifo *stack_fifo);

#endif /* __STACK_FIFO_H__ */