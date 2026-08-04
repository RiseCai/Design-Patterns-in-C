/**
 * memory_repository.h - concrete Repository
 */
#ifndef __MEMORY_REPOSITORY_H__
#define __MEMORY_REPOSITORY_H__

#include "repository.h"

#define MEMORY_REPOSITORY_CAP 8

struct memory_repository {
	struct repository repository;
	char names[MEMORY_REPOSITORY_CAP][32];
	int count;
};

/** constructor(). */
void memory_repository_init(struct memory_repository *memory_repository);

#endif /* __MEMORY_REPOSITORY_H__ */
