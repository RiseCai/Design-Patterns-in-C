/**
 * flipup.c  2022-12-24
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
#include "flipup.h"

static void flipup_ops_execute(struct command *command)
{
	/* struct flipup *a_flipup = container_of(command, typeof(*a_flipup), command);
	*/
	_MY_TRACE_STR("flipup::execute()\n");
}

static struct command_ops command_ops = {
	.execute = flipup_ops_execute,
};

/** constructor(). */
void flipup_init(struct flipup *flipup, struct light *light, int val)
{
	_MY_TRACE_STR("flipup_init()\n");
	memset(flipup, 0, sizeof(*flipup));
	command_init(&flipup->command);
	CLASS_OPS_INIT(flipup->command.ops, command_ops);
	flipup->_light = light;
	flipup->_val = val;
}