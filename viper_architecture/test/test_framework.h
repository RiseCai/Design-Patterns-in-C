/**
 * test_framework.h - Lightweight test framework for VIPER architecture
 *
 * Provides macros for assertions and test organization.
 */

#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Test result counters */
extern int test_pass_count;
extern int test_fail_count;

/* Assertion macros */
#define TEST_ASSERT(condition) \
    do { \
        if (!(condition)) { \
            printf("FAIL: %s:%d: %s\n", __FILE__, __LINE__, #condition); \
            test_fail_count++; \
            return; \
        } else { \
            test_pass_count++; \
        } \
    } while(0)

#define TEST_ASSERT_EQUAL(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            printf("FAIL: %s:%d: expected %lld, got %lld\n", __FILE__, __LINE__, (long long)(expected), (long long)(actual)); \
            test_fail_count++; \
            return; \
        } else { \
            test_pass_count++; \
        } \
    } while(0)

#define TEST_ASSERT_EQUAL_PTR(expected, actual) \
    do { \
        if ((expected) != (actual)) { \
            printf("FAIL: %s:%d: expected pointer %p, got %p\n", __FILE__, __LINE__, (expected), (actual)); \
            test_fail_count++; \
            return; \
        } else { \
            test_pass_count++; \
        } \
    } while(0)

#define TEST_ASSERT_EQUAL_STRING(expected, actual) \
    do { \
        if (strcmp((expected), (actual)) != 0) { \
            printf("FAIL: %s:%d: expected '%s', got '%s'\n", __FILE__, __LINE__, (expected), (actual)); \
            test_fail_count++; \
            return; \
        } else { \
            test_pass_count++; \
        } \
    } while(0)

#define TEST_ASSERT_NULL(ptr) \
    do { \
        if ((ptr) != NULL) { \
            printf("FAIL: %s:%d: expected NULL, got %p\n", __FILE__, __LINE__, (ptr)); \
            test_fail_count++; \
            return; \
        } else { \
            test_pass_count++; \
        } \
    } while(0)

#define TEST_ASSERT_NOT_NULL(ptr) \
    do { \
        if ((ptr) == NULL) { \
            printf("FAIL: %s:%d: expected non-NULL\n", __FILE__, __LINE__); \
            test_fail_count++; \
            return; \
        } else { \
            test_pass_count++; \
        } \
    } while(0)

/* Test case declaration */
#define TEST_CASE(name) void name(void)

/* Test suite runner */
#define RUN_TEST_SUITE(suite_name, test_cases) \
    do { \
        printf("\n=== Running test suite: " #suite_name " ===\n"); \
        fflush(stdout); \
        int total_tests = sizeof(test_cases) / sizeof(test_cases[0]); \
        for (int i = 0; i < total_tests; i++) { \
            printf("Running test %d/%d\n", i+1, total_tests); \
            fflush(stdout); \
            test_cases[i](); \
        } \
    } while(0)

/* Global test counters */
int test_pass_count = 0;
int test_fail_count = 0;

/* Print test summary */
static inline void print_test_summary(void) {
    printf("\n=== TEST SUMMARY ===\n");
    printf("Passed: %d\n", test_pass_count);
    printf("Failed: %d\n", test_fail_count);
    printf("Total:  %d\n", test_pass_count + test_fail_count);
    if (test_fail_count == 0) {
        printf("All tests passed!\n");
    } else {
        printf("Some tests failed.\n");
    }
}

#endif /* TEST_FRAMEWORK_H */
