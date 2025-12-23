#include <stdio.h>
#include "test_framework.h"

TEST_CASE(test_one) {
    printf("test_one running\n");
    TEST_ASSERT(1 == 1);
}

int main() {
    printf("Starting simple test\n");
    test_one();
    print_test_summary();
    return 0;
}
