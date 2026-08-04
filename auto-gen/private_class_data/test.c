/**
 * test.c - Private Class Data
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mycommon.h>
#include <myobj.h>
#include <mytrace.h>
#include <test_suite.h>
#include "circle.h"

static int test_main(void)
{
	struct circle c;
	double circ;
	double expected;
	double diff;

	_MY_TRACE_STR("test::main()\n");

	if (circle_init(&c, 2.0, "red") != 0) {
		printf("FAIL: circle_init\n");
		return 1;
	}

	diff = circle_get_radius(&c) - 2.0;
	if (diff < 0)
		diff = -diff;
	if (diff > 1e-9) {
		printf("FAIL: radius\n");
		circle_uninit(&c);
		return 1;
	}
	if (strcmp(circle_get_color(&c), "red") != 0) {
		printf("FAIL: color\n");
		circle_uninit(&c);
		return 1;
	}

	/* Color is mutable through API; radius has no setter (immutable). */
	circle_set_color(&c, "blue");
	if (strcmp(circle_get_color(&c), "blue") != 0) {
		printf("FAIL: set_color\n");
		circle_uninit(&c);
		return 1;
	}

	circ = circle_circumference(&c);
	printf("private_class_data: r=%.1f color=%s circumference~=%.3f\n",
		circle_get_radius(&c), circle_get_color(&c), circ);

	expected = 2.0 * 3.14159265358979323846 * 2.0;
	diff = circ - expected;
	if (diff < 0)
		diff = -diff;
	if (diff > 1e-9) {
		printf("FAIL: circumference\n");
		circle_uninit(&c);
		return 1;
	}

	/* Clients only see opaque struct circle_data *; no direct field access. */
	circle_uninit(&c);
	return 0;
}

static int test_main_entry(char *output, size_t sz)
{
	int rc = test_main();
	if (rc)
		snprintf(output, sz, "private_class_data test failed");
	return rc;
}

void main_entry_test(void);
void main_entry_test(void)
{
	my_test_suite_add(test_main_entry, "Test private_class_data");
}
