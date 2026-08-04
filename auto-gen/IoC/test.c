/**
 * test.c - IoC / Dependency Injection
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <mycommon.h>
#include <myobj.h>
#include <mytrace.h>
#include <test_suite.h>
#include "memory_repository.h"
#include "console_notifier.h"
#include "prefix_formatter.h"
#include "app_service.h"
#include "service_locator.h"

static int test_di_styles(void)
{
	struct memory_repository repo;
	struct console_notifier notifier;
	struct prefix_formatter formatter;
	struct app_service svc;
	const char *found;

	_MY_TRACE_STR("test::di_styles()\n");

	memory_repository_init(&repo);
	console_notifier_init(&notifier);
	prefix_formatter_init(&formatter, "u:");

	/* 1) Constructor injection */
	app_service_init(&svc, &repo.repository);

	/* 2) Setter injection */
	app_service_set_notifier(&svc, &notifier.notifier);

	/* 3) Method injection (formatter for this call) */
	if (app_service_create_user(&svc, "alice", &formatter.formatter) != 0) {
		printf("FAIL: create_user with method-injected formatter\n");
		return 1;
	}
	found = repository_find(&repo.repository, "u:alice");
	if (!found) {
		printf("FAIL: expected formatted name saved\n");
		return 1;
	}
	if (notifier.notify_count != 1) {
		printf("FAIL: setter-injected notifier not called\n");
		return 1;
	}

	/* 4) Interface injection (re-bind notifier through injectable API) */
	notifier.notify_count = 0;
	injectable_inject_notifier(&svc.injectable, &notifier.notifier);
	if (app_service_create_user(&svc, "bob", 0) != 0) {
		printf("FAIL: create_user bob\n");
		return 1;
	}
	if (!repository_find(&repo.repository, "bob")) {
		printf("FAIL: bob not saved\n");
		return 1;
	}
	if (notifier.notify_count != 1) {
		printf("FAIL: interface-injected notifier not called\n");
		return 1;
	}

	printf("IoC DI: ctor/setter/method/interface OK\n");
	return 0;
}

static int test_service_locator(void)
{
	struct memory_repository repo;
	struct app_service svc;
	struct repository *resolved;

	_MY_TRACE_STR("test::service_locator()\n");
	service_locator_reset();
	memory_repository_init(&repo);

	service_locator_register("repository", &repo.repository);
	resolved = service_locator_resolve("repository");
	if (resolved != &repo.repository) {
		printf("FAIL: locator resolve\n");
		return 1;
	}

	/* Client still receives dependency from outside -- here via locator. */
	app_service_init(&svc, resolved);
	if (app_service_create_user(&svc, "carol", 0) != 0) {
		printf("FAIL: create via locator-resolved repo\n");
		return 1;
	}
	if (!repository_find(&repo.repository, "carol")) {
		printf("FAIL: carol not saved\n");
		return 1;
	}

	printf("IoC service_locator: OK (note: hides dependencies vs explicit DI)\n");
	return 0;
}

static int test_main_entry(char *output, size_t sz)
{
	if (test_di_styles() != 0) {
		snprintf(output, sz, "IoC DI styles failed");
		return 1;
	}
	if (test_service_locator() != 0) {
		snprintf(output, sz, "IoC service locator failed");
		return 1;
	}
	return 0;
}

void main_entry_test(void);
void main_entry_test(void)
{
	my_test_suite_add(test_main_entry, "Test IoC/DI");
}
