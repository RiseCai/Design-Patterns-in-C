/**
 * test_moore_complex.c  2025-12-03
 * 
 * Test program for complex Moore hierarchical state machine.
 */
#include <stdio.h>
#include "moore_hierarchical.h"

/* Declare the example main from the complex implementation */
int main_example(void);

int main(void)
{
    printf("=== Testing Complex Moore Hierarchical State Machine ===\n");
    main_example();
    printf("=== Test completed ===\n");
    return 0;
}
