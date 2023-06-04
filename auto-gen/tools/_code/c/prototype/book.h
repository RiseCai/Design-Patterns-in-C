#ifndef __BOOK_H__
#define __BOOK_H__

#include <mycommon.h>

#include "product.h"

struct book {
	struct product product;
	int pages_num;
};

/** constructor(). */
void book_init(struct book *book);

void book_set_pages_num(struct book *book, int pages);

int book_get_pages_num(struct book *book);

#endif /* __BOOK_H__ */