/**
 * variable.c  2023-06-03
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
 * Uses a class hierarchy to represent the grammar given.
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <myobj.h>
#include "variable.h"

static void variable_ops_traverse(struct operand *operand, int level)
{
	/* struct variable *a_variable = container_of(operand, typeof(*a_variable), operand);
	*/
	_MY_TRACE_STR("variable::traverse()\n");
}

static int variable_ops_eval(struct operand *operand, struct context *ctx)
{
	/* struct variable *a_variable = container_of(operand, typeof(*a_variable), operand);
	*/
	_MY_TRACE_STR("variable::eval()\n");
	return 0;
}

static struct operand_ops operand_ops = {
	.traverse = variable_ops_traverse,
	.eval = variable_ops_eval,
};

/** constructor(). */
void variable_init(struct variable *variable, char *name)
{
	_MY_TRACE_STR("variable_init()\n");
	memset(variable, 0, sizeof(*variable));
	operand_init(&variable->operand);
	CLASS_OPS_INIT(variable->operand.ops, operand_ops);
	variable->_name = name;
}