/**
 * animal.c  2023-06-03
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
#include "animal.h"

static void animal_ops_eat(struct animal *animal)
{
	printf("animal::eat()\n");
}
static struct animal_ops animal_ops = {
	.eat = animal_ops_eat,
};

/** constructor(). */
void animal_init(struct animal *animal)
{
	printf("animal_init()\n");
	memset(animal, 0, sizeof(*animal));
	animal->ops = &animal_ops;
}