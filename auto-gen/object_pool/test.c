/**
 * test.c - Object Pool (connection pool)
 */
#include <stdio.h>
#include <stdlib.h>

#include <mycommon.h>
#include <myobj.h>
#include <mytrace.h>
#include <test_suite.h>
#include "connection_pool.h"

static int test_main(void)
{
	struct connection_pool pool;
	struct connection *c1;
	struct connection *c2;
	struct connection *c3;
	struct connection *c4;

	_MY_TRACE_STR("test::main()\n");
	connection_pool_init(&pool, 3, "db://127.0.0.1:5432");

	c1 = connection_pool_acquire(&pool);
	c2 = connection_pool_acquire(&pool);
	c3 = connection_pool_acquire(&pool);
	c4 = connection_pool_acquire(&pool); /* should fail: pool exhausted */

	if (!c1 || !c2 || !c3) {
		printf("FAIL: expected 3 acquired connections\n");
		return 1;
	}
	if (c4) {
		printf("FAIL: acquire should fail when pool is full\n");
		return 1;
	}
	if (connection_pool_in_use(&pool) != 3 || connection_pool_available(&pool) != 0) {
		printf("FAIL: in_use/available mismatch when full\n");
		return 1;
	}

	connection_execute(c1, "SELECT 1");
	connection_pool_release(&pool, c1);

	c4 = connection_pool_acquire(&pool);
	if (!c4 || c4 != c1) {
		printf("FAIL: released connection should be reused\n");
		return 1;
	}

	connection_pool_release(&pool, c2);
	connection_pool_release(&pool, c3);
	connection_pool_release(&pool, c4);

	printf("object_pool: capacity=3 reuse_ok available=%d\n",
		connection_pool_available(&pool));

	if (connection_pool_available(&pool) != 3) {
		printf("FAIL: all connections should be idle after release\n");
		return 1;
	}
	return 0;
}

static int test_main_entry(char *output, size_t sz)
{
	int rc = test_main();
	if (rc)
		snprintf(output, sz, "object_pool test failed");
	return rc;
}

void main_entry_test(void);
void main_entry_test(void)
{
	my_test_suite_add(test_main_entry, "Test object_pool");
}
