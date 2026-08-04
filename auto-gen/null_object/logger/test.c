/**
 * test.c - Null Object (Logger)
 */
#include <stdio.h>
#include <stdlib.h>

#include <mycommon.h>
#include <myobj.h>
#include <mytrace.h>
#include <test_suite.h>
#include "console_logger.h"
#include "null_logger.h"
#include "service.h"

static int test_main(void)
{
	struct console_logger console;
	struct service with_console;
	struct service with_null;

	_MY_TRACE_STR("test::main()\n");

	console_logger_init(&console);
	service_init(&with_console, &console.logger);
	service_run(&with_console, "backup");

	/* Pass NULL: service installs Null Logger, no crash, no console noise. */
	service_init(&with_null, 0);
	service_run(&with_null, "silent-job");

	if (with_null._logger != null_logger_instance()) {
		printf("FAIL: NULL logger should become null_logger_instance\n");
		return 1;
	}
	printf("null_object/logger: console + null both OK\n");
	return 0;
}

static int test_main_entry(char *output, size_t sz)
{
	int rc = test_main();
	if (rc)
		snprintf(output, sz, "null_object logger test failed");
	return rc;
}

void main_entry_test(void);
void main_entry_test(void)
{
	my_test_suite_add(test_main_entry, "Test null_object logger");
}
