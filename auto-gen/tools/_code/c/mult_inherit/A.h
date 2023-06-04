/**
 * A.h  2023-02-19
 * anonymouse(anonymouse@email)
 *
 * Copyright (C) 2000-2014 All Right Reserved
 * 
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Auto generate for Design Patterns in C
 */
#ifndef __A_H__
#define __A_H__

#include <mycommon.h>
#include <mytrace.h>

struct A_ops;
/** show override */
struct A {
	struct A_ops *ops;
};
struct A_ops {
	void (*do_this)(struct A *);
	void (*do_that)(struct A *);
	struct A_ops *__super;
};

/** constructor(). */
void A_init(struct A *A);

static inline void A_do_this(struct A *A)
{
	A->ops->do_this(A);
}

static inline void A_do_that(struct A *A)
{
	A->ops->do_that(A);
}

#endif /* __A_H__ */