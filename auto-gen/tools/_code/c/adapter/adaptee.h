/**
 * adaptee.h  2023-06-03
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
#ifndef __ADAPTEE_H__
#define __ADAPTEE_H__

#include <mycommon.h>

struct adaptee {
};

/** constructor(). */
void adaptee_init(struct adaptee *adaptee);

void adaptee_specific_request(struct adaptee *adaptee);

#endif /* __ADAPTEE_H__ */