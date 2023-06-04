/**
 * character_factory.c  2023-06-03
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
 * The Flyweight pattern is used to avoid the overhead of large numbers of
    very similar classes. There are cases in programming where it seems that
    you need to generate a very large number of small class instances to
    represent data. Sometimes you can greatly reduce the number of different
    classes that you need to instantiate if you can recognize that the
    instances are fundamentally the same except for a few parameters. If you
    can move those variables outside the class instance and pass them in as
    part of a method call, the number of separate instances can be greatly
    reduced by sharing them.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>

#include "character_factory.h"

/** constructor(). */
void character_factory_init(struct character_factory *character_factory)
{
	_MY_TRACE_STR("character_factory_init()\n");
	memset(character_factory, 0, sizeof(*character_factory));
}

/** Uses lazy initialization */
struct character * character_factory_get_character(struct character_factory *character_factory, char c)
{
	_MY_TRACE_STR("character_factory::get_character()\n");
	return character_factory->_character;
}