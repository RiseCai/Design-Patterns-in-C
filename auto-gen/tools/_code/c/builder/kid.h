/**
 * kid.h  2023-06-03
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
#ifndef __KID_H__
#define __KID_H__

#include <mycommon.h>

#include "animal_builder.h"

struct kid {
	char name[32];
};

/** constructor(). */
void kid_init(struct kid *kid);

void kid_make_animal(struct kid *kid, struct animal_builder *builder);

#endif /* __KID_H__ */