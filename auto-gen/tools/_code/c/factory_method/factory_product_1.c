#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <myobj.h>
#include "factory_product_1.h"

static struct product * factory_product_1_ops_create(struct factory *factory)
{
	/* struct factory_product_1 *a_factory_product_1 = container_of(factory, typeof(*a_factory_product_1), factory);
	*/
	printf("factory_product_1::create()\n");
	return 0;
}

static struct factory_ops factory_ops = {
	.create = factory_product_1_ops_create,
};

/** constructor(). */
void factory_product_1_init(struct factory_product_1 *factory_product_1)
{
	printf("factory_product_1_init()\n");
	memset(factory_product_1, 0, sizeof(*factory_product_1));
	factory_init(&factory_product_1->factory);
	CLASS_OPS_INIT(factory_product_1->factory.ops, factory_ops);
}