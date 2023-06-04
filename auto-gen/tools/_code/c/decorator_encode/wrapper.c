/**
 * wrapper.c  2023-06-03
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
 * When parse package, use wrapper to add header or remove header.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <myobj.h>
#include "wrapper.h"

/** called by free(): put resources, forward to super. */
static void wrapper_ops__destructor(struct decorator *decorator)
{
	TODO(Please add our **destructor** code here ...)
	/** PSEUDOCODE
	... do_something() to put resources ...
	_MY_TRACE_STR("wrapper::_destructor()\n");
	CLASS_SUPER(decorator, _destructor);
	*/
}
/** free memory after call destructor(). */
static void wrapper_ops_free(struct decorator *decorator)
{
	TODO(Please add our **free** code here ...)
	/** PSEUDOCODE
	struct wrapper *a_wrapper = container_of(decorator, typeof(*a_wrapper), decorator);
	decorator__destructor(decorator);
	_MY_TRACE_STR("wrapper::free()\n");
	free(a_wrapper);
	*/
}

static struct decorator_ops decorator_ops = {
	._destructor = wrapper_ops__destructor,
	.free = wrapper_ops_free,
};

static struct interface_ops interface_ops_2 = {0
};

/** constructor(). */
void wrapper_init(struct wrapper *wrapper, struct interface *inner, char *str)
{
	_MY_TRACE_STR("wrapper_init()\n");
	memset(wrapper, 0, sizeof(*wrapper));
	decorator_init(&wrapper->decorator, inner);
	CLASS_OPS_INIT_SUPER(wrapper->decorator.ops, decorator_ops);
	CLASS_OPS_INIT_SUPER(wrapper->decorator.interface.ops, interface_ops_2);
}