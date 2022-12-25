#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <myobj.h>
#include "concrete_product_2.h"

static void concrete_product_2_ops_do_it(struct product *product, int a, int b)
{
	/* struct concrete_product_2 *a_concrete_product_2 = container_of(product, typeof(*a_concrete_product_2), product);
	*/
	printf("concrete_product_2::do_it()\n");
}

static struct product_ops product_ops = {
	.do_it = concrete_product_2_ops_do_it,
};

/** constructor(). */
void concrete_product_2_init(struct concrete_product_2 *concrete_product_2)
{
	printf("concrete_product_2_init()\n");
	memset(concrete_product_2, 0, sizeof(*concrete_product_2));
	product_init(&concrete_product_2->product);
	CLASS_OPS_INIT(concrete_product_2->product.ops, product_ops);
}