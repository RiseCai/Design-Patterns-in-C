/**
 * repository.c
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include "repository.h"

static int repository_ops_save(struct repository *repository, const char *name)
{
	(void)repository;
	(void)name;
	return -1;
}

static const char *repository_ops_find(struct repository *repository, const char *name)
{
	(void)repository;
	(void)name;
	return 0;
}

static struct repository_ops repository_ops = {
	.save = repository_ops_save,
	.find = repository_ops_find,
};

/** constructor(). */
void repository_init(struct repository *repository)
{
	_MY_TRACE_STR("repository_init()\n");
	memset(repository, 0, sizeof(*repository));
	repository->ops = &repository_ops;
}
