#ifndef TEST_INCLUDE_H
#define TEST_INCLUDE_H

#include "../includes/malloc.h"

// Test functions from test.c
void test_basic_allocation(void);
void test_zero_allocation(void);
void test_realloc_shrink(void);
void test_realloc_grow(void);
void test_large_allocation(void);
void test_fragmentation(void);
void test_double_free(void);
void test_random_operations(void);

// Test functions from thread.c
void *thread_function(void *arg);
int run_thread_test(void);

#endif
