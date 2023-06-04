/**
 * kitten.c  2023-06-03
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
#include "kitten.h"

static void kitten_ops_eat(struct animal *animal)
{
	/* struct kitten *a_kitten = container_of(animal, typeof(*a_kitten), animal);
	*/
	printf("kitten::eat()\n");
}

static struct animal_ops animal_ops = {
	.eat = kitten_ops_eat,
};

/** constructor(). */
void kitten_init(struct kitten *kitten)
{
	printf("kitten_init()\n");
	memset(kitten, 0, sizeof(*kitten));
	animal_init(&kitten->animal);
	CLASS_OPS_INIT(kitten->animal.ops, animal_ops);
}