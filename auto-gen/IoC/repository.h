/**
 * repository.h - abstract dependency (DIP)
 */
#ifndef __REPOSITORY_H__
#define __REPOSITORY_H__

#include <mycommon.h>
#include <mytrace.h>

struct repository_ops;
struct repository {
	struct repository_ops *ops;
};
struct repository_ops {
	int (*save)(struct repository *, const char *name);
	const char *(*find)(struct repository *, const char *name);
};

/** constructor(). */
void repository_init(struct repository *repository);

static inline int repository_save(struct repository *repository, const char *name)
{
	return repository->ops->save(repository, name);
}
static inline const char *repository_find(struct repository *repository, const char *name)
{
	return repository->ops->find(repository, name);
}

#endif /* __REPOSITORY_H__ */
