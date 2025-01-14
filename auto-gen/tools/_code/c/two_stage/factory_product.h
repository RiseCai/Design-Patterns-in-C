/**
 * factory_product.h  2023-06-03
 * anonymouse(anonymouse@email)
 *
 * Copyright (C) 2000-2014 All Right Reserved
 * 
 * THIS CODE AND INFORMATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 * KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
 * PARTICULAR PURPOSE.
 *
 * Auto generate for Design Patterns in C
 */
#ifndef __FACTORY_PRODUCT_H__
#define __FACTORY_PRODUCT_H__

#include <mycommon.h>

#include "product.h"
		
#include "serializer.h"
		
#include "file_access_handler.h"
		
#include "domain_class.h"
		
#include "factory.h"

struct factory_product {
	struct factory factory;
	struct serializer * serializer;
	struct file_access_handler * file_access_handler;
};

/** constructor(). */
void factory_product_init(struct factory_product *factory_product);

#endif /* __FACTORY_PRODUCT_H__ */