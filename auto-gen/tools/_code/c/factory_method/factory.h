#ifndef __FACTORY_H__
#define __FACTORY_H__

#include <mycommon.h>

struct factory_ops;

struct factory {
	struct factory_ops *ops;
};
struct factory_ops {
	struct product * (*create)(struct factory *);
};

/** constructor(). */
void factory_init(struct factory *factory);

void factory_do_it(struct factory *factory);

static inline struct product * factory_create(struct factory *factory)
{
	return factory->ops->create(factory);
}

#endif /* __FACTORY_H__ */