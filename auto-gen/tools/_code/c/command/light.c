/**
 * light.c  2022-12-24
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
 * Need to issue requests to objects without knowing anything about the operation being requested or the receiver of the request.
    Encapsulate a request as an object, thereby letting you parameterize clients with different requests, queue or log requests, and support undoable operations.
    Promote "invocation of a method on an object" to full object status
    An object-oriented callback
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>

#include "light.h"

/** constructor(). */
void light_init(struct light *light)
{
	_MY_TRACE_STR("light_init()\n");
	memset(light, 0, sizeof(*light));
}

void light_turnon(struct light *light)
{
	_MY_TRACE_STR("light::turnon()\n");
}

void light_turnoff(struct light *light)
{
	_MY_TRACE_STR("light::turnoff()\n");
}