#include "chain_handle1.h"

static void chain_handler1_ops_handle(struct chain_handle *base, int data)
{
	if (data % 3 == 1) {
		printf("H1 processed %d  \n", data);
	}
	else {
		printf("H1 passsed %d  \n", data);

		// 3. Delegate to the base class
		//chain_base_ops_get(base, i);
		chain_base_handle_super(base, data);
	}
}

static void chain_handler1_ops_close(struct chain_handle *base)
{
}

static struct chain_base_ops chain_handler1_ops = {
	.handle = chain_handler1_ops_handle,
	.close = chain_handler1_ops_close,
};

void chain_handle1_init(struct chain_handle1 *chain)
{
	chain_base_init(&chain->super);
	CLASS_OPS_INIT_SUPER(chain->super.ops, chain_handler1_ops);
#if 0
	chain_base_init(&chain->super);
	chain_handler1_ops.__super = chain->super;
	chain->self.ops = OPS_FILL(&chain_handler1_ops, chain->self.ops->__super);
#endif
}
