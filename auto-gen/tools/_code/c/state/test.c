/**
 *   2023-06-03
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
#include <stdio.h>
#include <stdlib.h>

#include <mycommon.h>
#include <myobj.h>
#include <mytrace.h>
#include <test_suite.h>
#include "state_on.h"
#include "state_off.h"
#include "machine.h"

static int test_main(void)
{
	_MY_TRACE_STR("test::main()\n");
	return 0;
}

static int test_main_entry(char *output, size_t sz)
{
	TODO(Please add our **state_test** code here ...)
	return 0;
}

void main_entry_test(void);
void main_entry_test(void)
{
	my_test_suite_add(test_main_entry, "Test state");
}