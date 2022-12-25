#ifndef __CONCRETE_PRODUCT_2_H__
#define __CONCRETE_PRODUCT_2_H__

#include <mycommon.h>

#include "product.h"

struct concrete_product_2 {
	struct product product;
};

/** constructor(). */
void concrete_product_2_init(struct concrete_product_2 *concrete_product_2);

#endif /* __CONCRETE_PRODUCT_2_H__ */