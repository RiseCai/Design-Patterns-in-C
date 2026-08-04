/**
 * notifier.h - abstract dependency for Setter / Interface injection
 */
#ifndef __NOTIFIER_H__
#define __NOTIFIER_H__

#include <mycommon.h>
#include <mytrace.h>

struct notifier_ops;
struct notifier {
	struct notifier_ops *ops;
};
struct notifier_ops {
	void (*notify)(struct notifier *, const char *msg);
};

/** constructor(). */
void notifier_init(struct notifier *notifier);

static inline void notifier_notify(struct notifier *notifier, const char *msg)
{
	notifier->ops->notify(notifier, msg);
}

#endif /* __NOTIFIER_H__ */
