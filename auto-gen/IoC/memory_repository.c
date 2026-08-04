/**
 * memory_repository.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <myobj.h>
#include "memory_repository.h"

static int memory_repository_ops_save(struct repository *repository, const char *name)
{
	struct memory_repository *repo =
		container_of(repository, typeof(*repo), repository);

	_MY_TRACE_STR("memory_repository::save()\n");
	if (!name || repo->count >= MEMORY_REPOSITORY_CAP)
		return -1;
	snprintf(repo->names[repo->count], sizeof(repo->names[0]), "%s", name);
	repo->count++;
	return 0;
}

static const char *memory_repository_ops_find(struct repository *repository, const char *name)
{
	struct memory_repository *repo =
		container_of(repository, typeof(*repo), repository);
	int i;

	_MY_TRACE_STR("memory_repository::find()\n");
	if (!name)
		return 0;
	for (i = 0; i < repo->count; ++i) {
		if (strcmp(repo->names[i], name) == 0)
			return repo->names[i];
	}
	return 0;
}

static struct repository_ops repository_ops = {
	.save = memory_repository_ops_save,
	.find = memory_repository_ops_find,
};

/** constructor(). */
void memory_repository_init(struct memory_repository *memory_repository)
{
	_MY_TRACE_STR("memory_repository_init()\n");
	memset(memory_repository, 0, sizeof(*memory_repository));
	repository_init(&memory_repository->repository);
	CLASS_OPS_INIT(memory_repository->repository.ops, repository_ops);
}
