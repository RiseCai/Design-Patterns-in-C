#ifndef __PRODUCT_H__
#define __PRODUCT_H__

#include <mycommon.h>

struct product_ops;

struct product {
	struct product_ops *ops;
	char name[32];
};
struct product_ops {
	void (*do_it)(struct product *, int a, int b);
};

/** constructor(). */
void product_init(struct product *product);

static inline void product_do_it(struct product *product, int a, int b)
{
	product->ops->do_it(product, a, b);
}

#endif /* __PRODUCT_H__ */