/**
 * P_Q.c  2023-02-19
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
#include "P_Q.h"

static void P_Q_ops_jump_up(struct P *P)
{
	/* struct P_Q *a_P_Q = container_of(P, typeof(*a_P_Q), P);
	*/
	_MY_TRACE_STR("P_Q::jump_up()\n");
}

static void P_Q_ops_jump_down(struct P *P)
{
	/* struct P_Q *a_P_Q = container_of(P, typeof(*a_P_Q), P);
	*/
	_MY_TRACE_STR("P_Q::jump_down()\n");
}

static struct P_ops P_ops = {
	.jump_up = P_Q_ops_jump_up,
	.jump_down = P_Q_ops_jump_down,
};

/** constructor(). */
void P_Q_init(struct P_Q *P_Q)
{
	_MY_TRACE_STR("P_Q_init()\n");
	memset(P_Q, 0, sizeof(*P_Q));
	P_init(&P_Q->P);
	CLASS_OPS_INIT_SUPER(P_Q->P.ops, P_ops);
}