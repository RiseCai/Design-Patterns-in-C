/**
 * dog.c  2023-06-03
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
 * The Visitor pattern is in essence about covering an important use case of
    double dispatch in a single-dispatch language. It leverages the single-
    dispatch mechanism in a two-step idiom. Therefore if you use reflection for
    one of those two steps, you are no longer implementing Visitor. One way to
    look at it is that the visitor pattern is a way of letting your clients add
    additional methods to all of your classes in a particular class hierarchy.
    It is useful when you have a fairly stable class hierarchy, but you have
    changing requirements of what needs to be done with that hierarchy. The
    Visitor design pattern works really well for "recursive" structures like
    directory trees, XML structures, or document outlines.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <myobj.h>
#include "dog.h"

static void dog_ops_do_it(struct animal *animal, struct animal_do *dome)
{
	/* struct dog *a_dog = container_of(animal, typeof(*a_dog), animal);
	*/
	_MY_TRACE_STR("dog::do_it()\n");
}

static struct animal_ops animal_ops = {
	.do_it = dog_ops_do_it,
};

/** constructor(). */
void dog_init(struct dog *dog)
{
	_MY_TRACE_STR("dog_init()\n");
	memset(dog, 0, sizeof(*dog));
	animal_init(&dog->animal);
	CLASS_OPS_INIT(dog->animal.ops, animal_ops);
}