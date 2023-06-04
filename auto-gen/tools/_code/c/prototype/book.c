#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <mycommon.h>
#include <myobj.h>
#include "book.h"

static struct product * book_ops_clone(struct product *product)
{
	/* struct book *a_book = container_of(product, typeof(*a_book), product);
	*/
	printf("book::clone()\n");
	return 0;
}

static void book_ops_set_description(struct product *product, const char *desc)
{
	printf("book::set_description()\n");
	/*
	struct book *a_book = container_of(product, typeof(*a_book), product);
	a_book->_description = description;
	*/
}

static void book_ops_set_sku(struct product *product, const char *sku)
{
	printf("book::set_sku()\n");
	/*
	struct book *a_book = container_of(product, typeof(*a_book), product);
	a_book->_sku = sku;
	*/
}

static char * book_ops_get_description(struct product *product)
{
	printf("book::get_description()\n");
	/*
	struct book *a_book = container_of(product, typeof(*a_book), product);
	return a_book->_description;
	*/
}

static char * book_ops_get_sku(struct product *product)
{
	printf("book::get_sku()\n");
	/*
	struct book *a_book = container_of(product, typeof(*a_book), product);
	return a_book->_sku;
	*/
}

static struct product_ops product_ops = {
	.clone = book_ops_clone,
	.set_description = book_ops_set_description,
	.set_sku = book_ops_set_sku,
	.get_description = book_ops_get_description,
	.get_sku = book_ops_get_sku,
};

/** constructor(). */
void book_init(struct book *book)
{
	printf("book_init()\n");
	memset(book, 0, sizeof(*book));
	product_init(&book->product);
	CLASS_OPS_INIT(book->product.ops, product_ops);
}

void book_set_pages_num(struct book *book, int pages)
{
	printf("book::set_pages_num()\n");
	book->_pages_num = pages_num;
}

int book_get_pages_num(struct book *book)
{
	printf("book::get_pages_num()\n");
	return book->_pages_num;
}