#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <myobj.h>
#include "disk.h"

static struct product * disk_ops_clone(struct product *product)
{
	/* struct disk *a_disk = container_of(product, typeof(*a_disk), product);
	*/
	printf("disk::clone()\n");
	return 0;
}

static void disk_ops_set_description(struct product *product, const char *desc)
{
	printf("disk::set_description()\n");
	/*
	struct disk *a_disk = container_of(product, typeof(*a_disk), product);
	a_disk->_description = description;
	*/
}

static void disk_ops_set_sku(struct product *product, const char *sku)
{
	printf("disk::set_sku()\n");
	/*
	struct disk *a_disk = container_of(product, typeof(*a_disk), product);
	a_disk->_sku = sku;
	*/
}

static char * disk_ops_get_description(struct product *product)
{
	printf("disk::get_description()\n");
	/*
	struct disk *a_disk = container_of(product, typeof(*a_disk), product);
	return a_disk->_description;
	*/
}

static char * disk_ops_get_sku(struct product *product)
{
	printf("disk::get_sku()\n");
	/*
	struct disk *a_disk = container_of(product, typeof(*a_disk), product);
	return a_disk->_sku;
	*/
}

static struct product_ops product_ops = {
	.clone = disk_ops_clone,
	.set_description = disk_ops_set_description,
	.set_sku = disk_ops_set_sku,
	.get_description = disk_ops_get_description,
	.get_sku = disk_ops_get_sku,
};

/** constructor(). */
void disk_init(struct disk *disk)
{
	printf("disk_init()\n");
	memset(disk, 0, sizeof(*disk));
	product_init(&disk->product);
	CLASS_OPS_INIT(disk->product.ops, product_ops);
}

void disk_set_duration(struct disk *disk, int duration)
{
	printf("disk::set_duration()\n");
	disk->_duration = duration;
}

int disk_get_duration(struct disk *disk)
{
	printf("disk::get_duration()\n");
	return disk->_duration;
}