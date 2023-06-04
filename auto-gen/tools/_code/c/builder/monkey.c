/**
 * monkey.c  2023-06-03
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
#include "monkey.h"

static void monkey_ops_eat(struct animal *animal)
{
	/* struct monkey *a_monkey = container_of(animal, typeof(*a_monkey), animal);
	*/
	printf("monkey::eat()\n");
}

static struct animal_ops animal_ops = {
	.eat = monkey_ops_eat,
};

/** constructor(). */
void monkey_init(struct monkey *monkey)
{
	printf("monkey_init()\n");
	memset(monkey, 0, sizeof(*monkey));
	animal_init(&monkey->animal);
	CLASS_OPS_INIT(monkey->animal.ops, animal_ops);
}