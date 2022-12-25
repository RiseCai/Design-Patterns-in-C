#ifndef __FACTORY_PRODUCT_2_H__
#define __FACTORY_PRODUCT_2_H__

#include <mycommon.h>

#include "factory.h"

struct factory_product_2 {
	struct factory factory;
};

/** constructor(). */
void factory_product_2_init(struct factory_product_2 *factory_product_2);

#endif /* __FACTORY_PRODUCT_2_H__ */