/**
 * test.c - Memento pattern
 */
#include <stdio.h>
#include <stdlib.h>

#include <mycommon.h>
#include <myobj.h>
#include <mytrace.h>
#include <test_suite.h>
#include "originator.h"
#include "caretaker.h"

static int test_main(void)
{
	struct originator *originator;
	struct caretaker *caretaker;
	struct memento *saved;
	int before;
	int after;

	_MY_TRACE_STR("test::main()\n");

	originator = malloc(sizeof(*originator));
	if (!originator)
		return 1;
	originator_init(originator);

	caretaker = malloc(sizeof(*caretaker));
	if (!caretaker)
		return 1;
	caretaker_init(caretaker);

	originator_set_state(originator, 100);
	saved = originator_create_memento(originator);
	if (!saved)
		return 1;
	caretaker_set_memento(caretaker, saved);

	before = originator_get_state(originator);
	originator_set_state(originator, 200);
	originator_set_memento(originator, caretaker_get_memento(caretaker));
	after = originator_get_state(originator);

	printf("memento: before=%d changed=200 restored=%d\n", before, after);

	free(saved);
	free(caretaker);
	free(originator);

	if (before != 100 || after != 100) {
		printf("FAIL: restore did not recover saved state\n");
		return 1;
	}
	return 0;
}

static int test_main_entry(char *output, size_t sz)
{
	int rc = test_main();
	if (rc)
		snprintf(output, sz, "memento test failed");
	return rc;
}

void main_entry_test(void);
void main_entry_test(void)
{
	my_test_suite_add(test_main_entry, "Test memento");
}
