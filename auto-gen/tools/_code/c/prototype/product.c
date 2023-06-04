#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <myobj.h>
#include "product.h"

static struct product * product_ops_clone(struct product *product)
{
	printf("product::clone()\n");
	return 0;
}

static void product_ops_set_description(struct product *product, const char *desc)
{
	printf("product::set_description()\n");
	product->_description = description;
}

static void product_ops_set_sku(struct product *product, const char *sku)
{
	printf("product::set_sku()\n");
	product->_sku = sku;
}

static char * product_ops_get_description(struct product *product)
{
	printf("product::get_description()\n");
	return product->_description;
}

static char * product_ops_get_sku(struct product *product)
{
	printf("product::get_sku()\n");
	return product->_sku;
}
static struct product_ops product_ops = {
	.clone = product_ops_clone,
	.set_description = product_ops_set_description,
	.set_sku = product_ops_set_sku,
	.get_description = product_ops_get_description,
	.get_sku = product_ops_get_sku,
};

/** constructor(). */
void product_init(struct product *product)
{
	printf("product_init()\n");
	memset(product, 0, sizeof(*product));
	product->ops = &product_ops;
}