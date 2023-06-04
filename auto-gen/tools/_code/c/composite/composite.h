/**
 * composite.h  2023-06-03
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
#ifndef __COMPOSITE_H__
#define __COMPOSITE_H__

#include <mycommon.h>
#include <mytrace.h>

#include "component.h"

struct composite {
	struct component component;
	int value;
	struct component * children[32];
	int children_sz;
};

/** constructor(). */
void composite_init(struct composite *composite);

void composite_add(struct composite *composite, struct component *c);

#endif /* __COMPOSITE_H__ */