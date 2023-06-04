#ifndef __DISK_H__
#define __DISK_H__

#include <mycommon.h>

#include "product.h"

struct disk {
	struct product product;
	int duration;
};

/** constructor(). */
void disk_init(struct disk *disk);

void disk_set_duration(struct disk *disk, int duration);

int disk_get_duration(struct disk *disk);

#endif /* __DISK_H__ */