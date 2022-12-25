#ifndef __FACTORY_PRODUCT_1_H__
#define __FACTORY_PRODUCT_1_H__

#include <mycommon.h>

#include "factory.h"

struct factory_product_1 {
	struct factory factory;
};

/** constructor(). */
void factory_product_1_init(struct factory_product_1 *factory_product_1);

#endif /* __FACTORY_PRODUCT_1_H__ */