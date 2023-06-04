#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <myobj.h>
#include "product_cache.h"

static struct product_cache_ops product_cache_ops = {0
};

/** constructor(). */
void product_cache_init(struct product_cache *product_cache)
{
	printf("product_cache_init()\n");
	memset(product_cache, 0, sizeof(*product_cache));
	product_cache->ops = &product_cache_ops;
}

void product_cache_load_cache(void)
{
	printf("product_cache::load_cache()\n");
}

struct product * product_cache_get_product(const char *sku)
{
	printf("product_cache::get_product()\n");
	return product_cache->_product;
}