/**
 * formatter.h - dependency used by Method injection
 */
#ifndef __FORMATTER_H__
#define __FORMATTER_H__

#include <stddef.h>

#include <mycommon.h>
#include <mytrace.h>

struct formatter_ops;
struct formatter {
	struct formatter_ops *ops;
};
struct formatter_ops {
	void (*format)(struct formatter *, const char *in, char *out, size_t sz);
};

/** constructor(). */
void formatter_init(struct formatter *formatter);

static inline void formatter_format(struct formatter *formatter, const char *in,
	char *out, size_t sz)
{
	formatter->ops->format(formatter, in, out, sz);
}

#endif /* __FORMATTER_H__ */
