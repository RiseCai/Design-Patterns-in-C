/**
 * factory.h  2023-06-03
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
#ifndef __FACTORY_H__
#define __FACTORY_H__

#include <mycommon.h>

#include "product.h"
		
#include "domain_class.h"

struct factory_ops;

struct factory {
	struct factory_ops *ops;
};
struct factory_ops {
	struct product * (*create)(struct factory *, struct domain_class *dc);
};

/** constructor(). */
void factory_init(struct factory *factory);

static inline struct product * factory_create(struct factory *factory, struct domain_class *dc)
{
	return factory->ops->create(factory, dc);
}

#endif /* __FACTORY_H__ */