/**
 * test.c - Null Object (Strategy)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mycommon.h>
#include <myobj.h>
#include <mytrace.h>
#include <test_suite.h>
#include "upper_strategy.h"
#include "null_strategy.h"
#include "context.h"

static int test_main(void)
{
	struct upper_strategy upper;
	struct context ctx;
	char out[64];

	_MY_TRACE_STR("test::main()\n");

	upper_strategy_init(&upper);
	context_init(&ctx, &upper.strategy);
	context_process(&ctx, "hello", out, sizeof(out));
	printf("upper => %s\n", out);
	if (strcmp(out, "HELLO") != 0) {
		printf("FAIL: upper strategy\n");
		return 1;
	}

	/* Clear to Null Object: process remains safe, passthrough input. */
	context_set_strategy(&ctx, 0);
	context_process(&ctx, "hello", out, sizeof(out));
	printf("null  => %s\n", out);
	if (strcmp(out, "hello") != 0) {
		printf("FAIL: null strategy should passthrough\n");
		return 1;
	}
	if (ctx._strategy != null_strategy_instance()) {
		printf("FAIL: unset strategy should become null_strategy_instance\n");
		return 1;
	}

	printf("null_object/strategy: upper + null both OK\n");
	return 0;
}

static int test_main_entry(char *output, size_t sz)
{
	int rc = test_main();
	if (rc)
		snprintf(output, sz, "null_object strategy test failed");
	return rc;
}

void main_entry_test(void);
void main_entry_test(void)
{
	my_test_suite_add(test_main_entry, "Test null_object strategy");
}
