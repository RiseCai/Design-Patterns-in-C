/**
 * kid.c  2023-06-03
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

#include "kid.h"

/** constructor(). */
void kid_init(struct kid *kid)
{
	printf("kid_init()\n");
	memset(kid, 0, sizeof(*kid));
}

void kid_make_animal(struct kid *kid, struct animal_builder *builder)
{
	printf("kid::make_animal()\n");
}