/**
 * factory.c  2023-06-03
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
#include "factory.h"

static struct product * factory_ops_create(struct factory *factory, struct domain_class *dc)
{
	printf("factory::create()\n");
	return 0;
}
static struct factory_ops factory_ops = {
	.create = factory_ops_create,
};

/** constructor(). */
void factory_init(struct factory *factory)
{
	printf("factory_init()\n");
	memset(factory, 0, sizeof(*factory));
	factory->ops = &factory_ops;
}