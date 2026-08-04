/**
 * singleton.h
 *
 * Ensure a class has only one instance and provide a global point of access.
 * Participants:
 *   Singleton
 *     - defines get_instance() that returns the unique instance
 *     - may be responsible for creating its own unique instance
 */
#ifndef __SINGLETON_H__
#define __SINGLETON_H__

#include <mycommon.h>
#include <mytrace.h>

struct singleton_ops;
struct singleton {
	struct singleton_ops *ops;
	char name[32];
	int value;
};
struct singleton_ops {
	void (*set_name)(struct singleton *, const char *name);
	void (*set_value)(struct singleton *, int value);
	const char *(*get_name)(struct singleton *);
	int (*get_value)(struct singleton *);
};

/** Global access point: always returns the same instance. */
struct singleton *singleton_get_instance(void);

static inline void singleton_set_name(struct singleton *singleton, const char *name)
{
	singleton->ops->set_name(singleton, name);
}
static inline void singleton_set_value(struct singleton *singleton, int value)
{
	singleton->ops->set_value(singleton, value);
}
static inline const char *singleton_get_name(struct singleton *singleton)
{
	return singleton->ops->get_name(singleton);
}
static inline int singleton_get_value(struct singleton *singleton)
{
	return singleton->ops->get_value(singleton);
}

#endif /* __SINGLETON_H__ */
