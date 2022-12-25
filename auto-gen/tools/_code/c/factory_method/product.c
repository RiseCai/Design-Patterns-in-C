#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <myobj.h>
#include "product.h"

static void product_ops_do_it(struct product *product, int a, int b)
{
	printf("product::do_it()\n");
}
static struct product_ops product_ops = {
	.do_it = product_ops_do_it,
};

/** constructor(). */
void product_init(struct product *product)
{
	printf("product_init()\n");
	memset(product, 0, sizeof(*product));
	product->ops = &product_ops;
}