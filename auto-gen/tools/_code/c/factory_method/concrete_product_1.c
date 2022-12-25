#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <myobj.h>
#include "concrete_product_1.h"

static void concrete_product_1_ops_do_it(struct product *product, int a, int b)
{
	/* struct concrete_product_1 *a_concrete_product_1 = container_of(product, typeof(*a_concrete_product_1), product);
	*/
	printf("concrete_product_1::do_it()\n");
}

static struct product_ops product_ops = {
	.do_it = concrete_product_1_ops_do_it,
};

/** constructor(). */
void concrete_product_1_init(struct concrete_product_1 *concrete_product_1)
{
	printf("concrete_product_1_init()\n");
	memset(concrete_product_1, 0, sizeof(*concrete_product_1));
	product_init(&concrete_product_1->product);
	CLASS_OPS_INIT(concrete_product_1->product.ops, product_ops);
}