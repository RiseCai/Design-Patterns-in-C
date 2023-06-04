/**
 * animal.h  2023-06-03
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
#ifndef __ANIMAL_H__
#define __ANIMAL_H__

#include <mycommon.h>

struct animal_ops;

struct animal {
	struct animal_ops *ops;
};
struct animal_ops {
	void (*eat)(struct animal *);
};

/** constructor(). */
void animal_init(struct animal *animal);

static inline void animal_eat(struct animal *animal)
{
	animal->ops->eat(animal);
}

#endif /* __ANIMAL_H__ */