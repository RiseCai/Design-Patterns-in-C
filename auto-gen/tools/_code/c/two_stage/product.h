/**
 * product.h  2023-06-03
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
#ifndef __PRODUCT_H__
#define __PRODUCT_H__

#include <mycommon.h>

struct product {
	char name[32];
};

/** constructor(). */
void product_init(struct product *product);

void product_do_something(struct product *product);

#endif /* __PRODUCT_H__ */