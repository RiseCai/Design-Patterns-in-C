/**
 * class1.c  2023-06-03
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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>

#include "class1.h"

/** constructor(). */
void class1_init(struct class1 *class1)
{
	_MY_TRACE_STR("class1_init()\n");
	memset(class1, 0, sizeof(*class1));
}

int class1_showthis(struct class1 *class1, char *name)
{
	_MY_TRACE_STR("class1::showthis()\n");
	return 0;
}

int class1_showthat(char *name)
{
	_MY_TRACE_STR("class1::showthat()\n");
	return 0;
}

void class1_set_value(struct class1 *class1, void value)
{
	_MY_TRACE_STR("class1::set_value()\n");
	class1->_value = value;
}

void class1_get_value(struct class1 *class1)
{
	_MY_TRACE_STR("class1::get_value()\n");
	return class1->_value;
}