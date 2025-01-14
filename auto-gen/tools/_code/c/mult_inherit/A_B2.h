/**
 * A_B2.h  2023-02-19
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
#ifndef __A_B2_H__
#define __A_B2_H__

#include <mycommon.h>
#include <mytrace.h>

#include "A.h"

/** show override */
struct A_B2 {
	struct A A;
};

/** constructor(). */
void A_B2_init(struct A_B2 *A_B2);

#endif /* __A_B2_H__ */