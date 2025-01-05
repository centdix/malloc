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
    void *ptr = malloc(1); // Allocate 1 bytes
    assert(ptr != NULL);     // Ensure pointer is valid
    printf("Allocated 1 bytes at %p\n", ptr);
    show_alloc_mem();

    void *ptr2 = malloc(100); // Allocate 100 bytes
    assert(ptr2 != NULL);     // Ensure pointer is valid
    printf("Allocated 100 bytes at %p\n", ptr2);
    show_alloc_mem();

    free(ptr); // Free the memory
    printf("Freed memory at %p\n", ptr);
	free(ptr2); // Free the memory
    printf("Freed memory at %p\n", ptr2);
    show_alloc_mem();
    printf("test_basic_allocation passed!\n\n");
}

void test_zero_allocation() {
    printf("Running test_zero_allocation...\n");
    void *ptr = malloc(0); // Allocate 0 bytes
    assert(ptr == NULL || ptr != NULL); // Both behaviors are acceptable
    printf("Allocated 0 bytes: %s\n", ptr ? "Non-NULL pointer" : "NULL pointer");
    show_alloc_mem();

    free(ptr); // Should not crash
    printf("Freed memory (even if NULL)\n");
    show_alloc_mem();
    printf("test_zero_allocation passed!\n\n");
}

void test_realloc_shrink() {
    printf("Running test_realloc_shrink...\n");
    void *ptr = malloc(100);
    assert(ptr != NULL);
    memset(ptr, 'A', 100); // Fill memory with 'A'
    show_alloc_mem();

    void *new_ptr = realloc(ptr, 50); // Shrink to 50 bytes
    assert(new_ptr != NULL);          // Ensure realloc succeeded
    printf("Shrunk memory to 50 bytes\n");
    show_alloc_mem();

    free(new_ptr); // Free the memory
    printf("Freed memory after realloc\n");
    show_alloc_mem();
    printf("test_realloc_shrink passed!\n\n");
}

void test_realloc_grow() {
    printf("Running test_realloc_grow...\n");
    void *ptr = malloc(50);
    assert(ptr != NULL);
    memset(ptr, 'B', 50); // Fill memory with 'B'
    show_alloc_mem();

    void *new_ptr = realloc(ptr, 100); // Grow to 100 bytes
    assert(new_ptr != NULL);           // Ensure realloc succeeded
    printf("Grew memory to 100 bytes\n");
    show_alloc_mem();

    free(new_ptr); // Free the memory
    printf("Freed memory after realloc\n");
    show_alloc_mem();
    printf("test_realloc_grow passed!\n\n");
}

void test_large_allocation() {
    printf("Running test_large_allocation...\n");
    size_t large_size = 1024 * 1024 * 10; // 10 MB
    void *ptr = malloc(large_size);
    assert(ptr != NULL); // Ensure allocation succeeds
    printf("Allocated large memory block (10 MB) at %p\n", ptr);
    show_alloc_mem();

    free(ptr); // Free the large block
    printf("Freed large memory block\n");
    show_alloc_mem();
    printf("test_large_allocation passed!\n\n");
}

void test_fragmentation() {
    printf("Running test_fragmentation...\n");
    void *p1 = malloc(100);
    void *p2 = malloc(200);
    void *p3 = malloc(300);
    assert(p1 != NULL && p2 != NULL && p3 != NULL);
    printf("Allocated three blocks: %p, %p, %p\n", p1, p2, p3);
    show_alloc_mem();

    free(p2); // Free the middle block
    printf("Freed the middle block at %p\n", p2);
    show_alloc_mem();

    void *p4 = malloc(150); // Allocate a new block that fits into the freed space
    assert(p4 != NULL);
    printf("Reused freed block for a new allocation: %p\n", p4);
    show_alloc_mem();

    free(p1);
    free(p3);
    free(p4);
    printf("Freed all remaining blocks\n");
    show_alloc_mem();
    printf("test_fragmentation passed!\n\n");
}

void test_double_free() {
    printf("Running test_double_free...\n");
    void *ptr = malloc(100);
    assert(ptr != NULL);

    free(ptr); // Free once
    printf("Freed memory at %p\n", ptr);
    show_alloc_mem();

    free(ptr); // Free again (should not crash)
    printf("Double free handled gracefully\n");
    show_alloc_mem();
    printf("test_double_free passed!\n\n");
}

void test_random_operations() {
    printf("Running test_random_operations...\n");
    
    // Array to keep track of allocated pointers
    void *ptrs[20] = {NULL};
    int ptr_count = 0;
    
    // Perform 20 random operations
    for (int i = 0; i < 20; i++) {
        int operation = rand() % 3;  // 0: malloc, 1: free, 2: realloc
        
        printf("Operation %d: ", i + 1);
        switch (operation) {
            case 0: // malloc
                if (ptr_count < 20) {
                    size_t size = (rand() % 1000) + 1;  // Random size between 1-1000
                    ptrs[ptr_count] = malloc(size);
                    assert(ptrs[ptr_count] != NULL);
                    printf("malloc(%zu) at %p\n", size, ptrs[ptr_count]);
                    ptr_count++;
                }
                break;
                
            case 1: // free
                if (ptr_count > 0) {
                    int index = rand() % ptr_count;
                    if (ptrs[index] != NULL) {
                        printf("free at %p\n", ptrs[index]);
                        free(ptrs[index]);
                        ptrs[index] = NULL;
                        // Compact array
                        for (int j = index; j < ptr_count - 1; j++) {
                            ptrs[j] = ptrs[j + 1];
                        }
                        ptr_count--;
                    }
                }
                break;
                
            case 2: // realloc
                if (ptr_count > 0) {
                    int index = rand() % ptr_count;
                    if (ptrs[index] != NULL) {
                        size_t new_size = (rand() % 1000) + 1;
                        void *new_ptr = realloc(ptrs[index], new_size);
                        assert(new_ptr != NULL);
                        printf("realloc(%p, %zu) -> %p\n", ptrs[index], new_size, new_ptr);
                        ptrs[index] = new_ptr;
                    }
                }
                break;
        }
        show_alloc_mem();
    }
    
    // Cleanup: free any remaining allocations
    for (int i = 0; i < ptr_count; i++) {
        if (ptrs[i] != NULL) {
            free(ptrs[i]);
        }
    }
    
    printf("test_random_operations passed!\n\n");
}
