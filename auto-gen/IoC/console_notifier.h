/**
 * console_notifier.h - concrete Notifier
 */
#ifndef __CONSOLE_NOTIFIER_H__
#define __CONSOLE_NOTIFIER_H__

#include "notifier.h"

struct console_notifier {
	struct notifier notifier;
	int notify_count;
};

/** constructor(). */
void console_notifier_init(struct console_notifier *console_notifier);

#endif /* __CONSOLE_NOTIFIER_H__ */
