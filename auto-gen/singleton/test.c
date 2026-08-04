/**
 * test.c - Singleton pattern
 */
#include <stdio.h>
#include <stdlib.h>

#include <mycommon.h>
#include <myobj.h>
#include <mytrace.h>
#include <test_suite.h>
#include "singleton.h"

static int test_main(void)
{
	struct singleton *one;
	struct singleton *two;

	_MY_TRACE_STR("test::main()\n");

	one = singleton_get_instance();
	two = singleton_get_instance();

	if (!one || !two) {
		printf("singleton_get_instance failed\n");
		return 1;
	}
	if (one != two) {
		printf("FAIL: get_instance returned different pointers\n");
		return 1;
	}

	singleton_set_name(one, "app_config");
	singleton_set_value(one, 42);

	printf("singleton name=%s value=%d ptr=%p\n",
		singleton_get_name(two), singleton_get_value(two), (void *)two);

	if (singleton_get_value(two) != 42) {
		printf("FAIL: value not shared across get_instance calls\n");
		return 1;
	}
	return 0;
}

static int test_main_entry(char *output, size_t sz)
{
	int rc = test_main();
	if (rc)
		snprintf(output, sz, "singleton test failed");
	return rc;
}

void main_entry_test(void);
void main_entry_test(void)
{
	my_test_suite_add(test_main_entry, "Test singleton");
}
