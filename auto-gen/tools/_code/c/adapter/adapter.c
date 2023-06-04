/**
 * adapter.c  2023-06-03
 * anonymouse(anonymouse@email)
 *
 * Copyright (C) 2000-2014 All Right Reserved
 * 
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Auto generate for Design Patterns in C
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <myobj.h>
#include "adapter.h"

static void adapter_ops_request(struct target *target)
{
	/* struct adapter *a_adapter = container_of(target, typeof(*a_adapter), target);
	*/
	printf("adapter::request()\n");
}

static struct target_ops target_ops = {
	.request = adapter_ops_request,
};

/** constructor(). */
void adapter_init(struct adapter *adapter)
{
	printf("adapter_init()\n");
	memset(adapter, 0, sizeof(*adapter));
	target_init(&adapter->target);
	CLASS_OPS_INIT(adapter->target.ops, target_ops);
}