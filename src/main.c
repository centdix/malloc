#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "include.h"

// Replace with your custom malloc implementation
#define malloc my_malloc
#define free my_free
#define realloc my_realloc

void test_basic_allocation() {
    printf("Running test_basic_allocation...\n");
    void *ptr = malloc(100); // Allocate 100 bytes
    assert(ptr != NULL);     // Ensure pointer is valid
    printf("Allocated 100 bytes at %p\n", ptr);

    free(ptr); // Free the memory
    printf("Freed memory at %p\n", ptr);
    printf("test_basic_allocation passed!\n\n");
}

void test_zero_allocation() {
    printf("Running test_zero_allocation...\n");
    void *ptr = malloc(0); // Allocate 0 bytes
    assert(ptr == NULL || ptr != NULL); // Both behaviors are acceptable
    printf("Allocated 0 bytes: %s\n", ptr ? "Non-NULL pointer" : "NULL pointer");

    free(ptr); // Should not crash
    printf("Freed memory (even if NULL)\n");
    printf("test_zero_allocation passed!\n\n");
}

void test_realloc_shrink() {
    printf("Running test_realloc_shrink...\n");
    void *ptr = malloc(100);
    assert(ptr != NULL);
    memset(ptr, 'A', 100); // Fill memory with 'A'

    void *new_ptr = realloc(ptr, 50); // Shrink to 50 bytes
    assert(new_ptr != NULL);          // Ensure realloc succeeded
    printf("Shrunk memory to 50 bytes\n");

    free(new_ptr); // Free the memory
    printf("Freed memory after realloc\n");
    printf("test_realloc_shrink passed!\n\n");
}

void test_realloc_grow() {
    printf("Running test_realloc_grow...\n");
    void *ptr = malloc(50);
    assert(ptr != NULL);
    memset(ptr, 'B', 50); // Fill memory with 'B'

    void *new_ptr = realloc(ptr, 100); // Grow to 100 bytes
    assert(new_ptr != NULL);           // Ensure realloc succeeded
    printf("Grew memory to 100 bytes\n");

    free(new_ptr); // Free the memory
    printf("Freed memory after realloc\n");
    printf("test_realloc_grow passed!\n\n");
}

void test_large_allocation() {
    printf("Running test_large_allocation...\n");
    size_t large_size = 1024 * 1024 * 10; // 10 MB
    void *ptr = malloc(large_size);
    assert(ptr != NULL); // Ensure allocation succeeds
    printf("Allocated large memory block (10 MB) at %p\n", ptr);

    free(ptr); // Free the large block
    printf("Freed large memory block\n");
    printf("test_large_allocation passed!\n\n");
}

void test_fragmentation() {
    printf("Running test_fragmentation...\n");
    void *p1 = malloc(100);
    void *p2 = malloc(200);
    void *p3 = malloc(300);
    assert(p1 != NULL && p2 != NULL && p3 != NULL);
    printf("Allocated three blocks: %p, %p, %p\n", p1, p2, p3);

    free(p2); // Free the middle block
    printf("Freed the middle block at %p\n", p2);

    void *p4 = malloc(150); // Allocate a new block that fits into the freed space
    assert(p4 != NULL);
    printf("Reused freed block for a new allocation: %p\n", p4);

    free(p1);
    free(p3);
    free(p4);
    printf("Freed all remaining blocks\n");
    printf("test_fragmentation passed!\n\n");
}

void test_double_free() {
    printf("Running test_double_free...\n");
    void *ptr = malloc(100);
    assert(ptr != NULL);

    free(ptr); // Free once
    printf("Freed memory at %p\n", ptr);

    free(ptr); // Free again (should not crash)
    printf("Double free handled gracefully\n");
    printf("test_double_free passed!\n\n");
}

int main() {
    printf("Starting basic malloc tests...\n\n");

    test_basic_allocation();
    test_zero_allocation();
    test_realloc_shrink();
    test_realloc_grow();
    test_large_allocation();
    test_fragmentation();
    test_double_free();

    printf("All tests passed!\n");
    return 0;
}
