/**
 * class1.h  2023-06-03
 * anonymouse(anonymouse@email)
 *
 * Copyright (C) 2000-2014 All Right Reserved
 * 
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Auto generate for Design Patterns in C *
 * 
 * The info will insert to every source file explaining the pattern's design details.
    We also can append lists like this:
      - list 1, give point one
          detail provide for point one
      - list 2, offer point two
 */
#ifndef __CLASS1_H__
#define __CLASS1_H__

#include <mycommon.h>
#include <mytrace.h>

struct class1 {
	int _count;
	void _value;
};

/** constructor(). */
void class1_init(struct class1 *class1);

int class1_showthis(struct class1 *class1, char *name);

int class1_showthat(char *name);

void class1_set_value(struct class1 *class1, void value);

void class1_get_value(struct class1 *class1);

#endif /* __CLASS1_H__ */