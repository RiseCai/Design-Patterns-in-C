/**
 * animal_builder_kitten.h  2023-06-03
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
#ifndef __ANIMAL_BUILDER_KITTEN_H__
#define __ANIMAL_BUILDER_KITTEN_H__

#include <mycommon.h>

#include "animal_builder.h"

struct animal_builder_kitten {
	struct animal_builder animal_builder;
};

/** constructor(). */
void animal_builder_kitten_init(struct animal_builder_kitten *animal_builder_kitten);

#endif /* __ANIMAL_BUILDER_KITTEN_H__ */