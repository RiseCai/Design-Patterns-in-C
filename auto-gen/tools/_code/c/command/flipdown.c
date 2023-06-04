/**
 * flipdown.c  2023-02-18
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
#include <myobj.h>
#include "flipdown.h"

static void flipdown_ops_execute(struct command *command)
{
	/* struct flipdown *a_flipdown = container_of(command, typeof(*a_flipdown), command);
	*/
	_MY_TRACE_STR("flipdown::execute()\n");
}

static struct command_ops command_ops = {
	.execute = flipdown_ops_execute,
};

/** constructor(). */
void flipdown_init(struct flipdown *flipdown, struct light *light, int val)
{
	_MY_TRACE_STR("flipdown_init()\n");
	memset(flipdown, 0, sizeof(*flipdown));
	command_init(&flipdown->command);
	CLASS_OPS_INIT(flipdown->command.ops, command_ops);
	flipdown->_light = light;
	flipdown->_val = val;
}