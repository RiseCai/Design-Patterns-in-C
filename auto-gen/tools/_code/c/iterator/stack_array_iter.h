/**
 * stack_array_iter.h  2023-06-03
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
 * Add iterator to the container:
    - Design an "iterator" class for the "container" class
    - Add a createIterator() member to the container class
    - Clients ask the container object to create an iterator object
    - Clients use the first(), isDone(), next(), and currentItem() protocol
 */
#ifndef __STACK_ARRAY_ITER_H__
#define __STACK_ARRAY_ITER_H__

#include <mycommon.h>
#include <mytrace.h>

#include "stack_array.h"
		
#include "iterator.h"

/** Take traversal-of-a-collection functionality out of the collection and
    promote it to "full object status". */
struct stack_array_iter {
	struct iterator iterator;
	struct stack_array * _stack_array;
	int _pos; /** As cursor of stack_array */
};

/** constructor(). */
void stack_array_iter_init(struct stack_array_iter *stack_array_iter, struct stack_array *stack_array);

#endif /* __STACK_ARRAY_ITER_H__ */