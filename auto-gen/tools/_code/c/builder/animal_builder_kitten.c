/**
 * animal_builder_kitten.c  2023-06-03
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
#include "animal_builder_kitten.h"

static void animal_builder_kitten_ops_build_header(struct animal_builder *animal_builder)
{
	/* struct animal_builder_kitten *a_animal_builder_kitten = container_of(animal_builder, typeof(*a_animal_builder_kitten), animal_builder);
	*/
	printf("animal_builder_kitten::build_header()\n");
}

static void animal_builder_kitten_ops_build_body(struct animal_builder *animal_builder)
{
	/* struct animal_builder_kitten *a_animal_builder_kitten = container_of(animal_builder, typeof(*a_animal_builder_kitten), animal_builder);
	*/
	printf("animal_builder_kitten::build_body()\n");
}

static void animal_builder_kitten_ops_build_leg(struct animal_builder *animal_builder)
{
	/* struct animal_builder_kitten *a_animal_builder_kitten = container_of(animal_builder, typeof(*a_animal_builder_kitten), animal_builder);
	*/
	printf("animal_builder_kitten::build_leg()\n");
}

static void animal_builder_kitten_ops_build_arm(struct animal_builder *animal_builder)
{
	/* struct animal_builder_kitten *a_animal_builder_kitten = container_of(animal_builder, typeof(*a_animal_builder_kitten), animal_builder);
	*/
	printf("animal_builder_kitten::build_arm()\n");
}

static void animal_builder_kitten_ops_build_tail(struct animal_builder *animal_builder)
{
	/* struct animal_builder_kitten *a_animal_builder_kitten = container_of(animal_builder, typeof(*a_animal_builder_kitten), animal_builder);
	*/
	printf("animal_builder_kitten::build_tail()\n");
}

static struct animal_builder_ops animal_builder_ops = {
	.build_header = animal_builder_kitten_ops_build_header,
	.build_body = animal_builder_kitten_ops_build_body,
	.build_leg = animal_builder_kitten_ops_build_leg,
	.build_arm = animal_builder_kitten_ops_build_arm,
	.build_tail = animal_builder_kitten_ops_build_tail,
};

/** constructor(). */
void animal_builder_kitten_init(struct animal_builder_kitten *animal_builder_kitten)
{
	printf("animal_builder_kitten_init()\n");
	memset(animal_builder_kitten, 0, sizeof(*animal_builder_kitten));
	animal_builder_init(&animal_builder_kitten->animal_builder);
	CLASS_OPS_INIT(animal_builder_kitten->animal_builder.ops, animal_builder_ops);
}