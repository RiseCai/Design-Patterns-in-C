/**
 * prefix_formatter.h - concrete Formatter for method injection demo
 */
#ifndef __PREFIX_FORMATTER_H__
#define __PREFIX_FORMATTER_H__

#include "formatter.h"

struct prefix_formatter {
	struct formatter formatter;
	char prefix[16];
};

/** constructor(). */
void prefix_formatter_init(struct prefix_formatter *prefix_formatter, const char *prefix);

#endif /* __PREFIX_FORMATTER_H__ */
