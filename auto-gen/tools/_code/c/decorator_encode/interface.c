/**
 * interface.c  2023-06-03
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
#include "interface.h"

static struct interface_ops interface_ops = {0
};

/** constructor(). */
void interface_init(struct interface *interface)
{
	_MY_TRACE_STR("interface_init()\n");
	memset(interface, 0, sizeof(*interface));
	interface->ops = &interface_ops;
}