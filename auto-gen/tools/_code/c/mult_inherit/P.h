/**
 * P.h  2023-02-19
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
#ifndef __P_H__
#define __P_H__

#include <mycommon.h>
#include <mytrace.h>

struct P_ops;
/** show pure virtual */
struct P {
	struct P_ops *ops;
};
struct P_ops {
	void (*jump_up)(struct P *);
	void (*jump_down)(struct P *);
	struct P_ops *__super;
};

/** constructor(). */
void P_init(struct P *P);

static inline void P_jump_up(struct P *P)
{
	P->ops->jump_up(P);
}

static inline void P_jump_down(struct P *P)
{
	P->ops->jump_down(P);
}

#endif /* __P_H__ */