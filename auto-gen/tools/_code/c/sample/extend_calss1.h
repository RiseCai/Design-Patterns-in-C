/**
 * extend_calss1.h  2023-06-03
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
#ifndef __EXTEND_CALSS1_H__
#define __EXTEND_CALSS1_H__

#include <mycommon.h>
#include <mytrace.h>

#include "base_class.h"

struct extend_calss1 {
	struct base_class base_class;
	char * _name;
};

/** constructor(). */
void extend_calss1_init(struct extend_calss1 *extend_calss1, char *name);

#endif /* __EXTEND_CALSS1_H__ */