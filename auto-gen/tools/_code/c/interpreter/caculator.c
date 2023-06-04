/**
 * caculator.c  2023-06-03
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

#include "caculator.h"

/** constructor(). */
void caculator_init(struct caculator *caculator)
{
	_MY_TRACE_STR("caculator_init()\n");
	memset(caculator, 0, sizeof(*caculator));
}

/** return bool */
int caculator_precedence(char a, char b)
{
	_MY_TRACE_STR("caculator::precedence()\n");
	return 0;
}
/** convert expr from in-fix to post-fix */
char * caculator_convert_to_postfix(char *expr)
{
	_MY_TRACE_STR("caculator::convert_to_postfix()\n");
	return 0;
}

struct operand * caculator_build_syntax_tree(char *tree)
{
	_MY_TRACE_STR("caculator::build_syntax_tree()\n");
	return 0;
}