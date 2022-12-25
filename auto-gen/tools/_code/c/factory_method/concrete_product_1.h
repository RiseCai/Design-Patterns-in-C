#ifndef __CONCRETE_PRODUCT_1_H__
#define __CONCRETE_PRODUCT_1_H__

#include <mycommon.h>

#include "product.h"

struct concrete_product_1 {
	struct product product;
};

/** constructor(). */
void concrete_product_1_init(struct concrete_product_1 *concrete_product_1);

#endif /* __CONCRETE_PRODUCT_1_H__ */