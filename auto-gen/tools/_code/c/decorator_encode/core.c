/**
 * core.c  2023-06-03
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
#include "core.h"

/** constructor(). */
#undef core_init
/** constructor(). */
void core_init(struct core *core);

static void core_ops_write(struct interface *interface, char *buf)
{
	/* struct core *a_core = container_of(interface, typeof(*a_core), interface);
	*/
	_MY_TRACE_STR("core::write()\n");
}

static void core_ops_read(struct interface *interface, char *buf)
{
	/* struct core *a_core = container_of(interface, typeof(*a_core), interface);
	*/
	_MY_TRACE_STR("core::read()\n");
}

static struct interface_ops interface_ops = {
	.write = core_ops_write,
	.read = core_ops_read,
};

/** constructor(). */
void core_init(struct core *core)
{
	_MY_TRACE_STR("core_init()\n");
	memset(core, 0, sizeof(*core));
	interface_init(&core->interface);
	CLASS_OPS_INIT_SUPER(core->interface.ops, interface_ops);
}