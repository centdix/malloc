#include <stdio.h>
#include <time.h>
#include "test_include.h"

int main(void) {
    printf("\n=== Starting Basic Tests ===\n\n");
    srand(time(NULL));
    
    test_basic_allocation();
    test_zero_allocation();
    test_realloc_shrink();
    test_realloc_grow();
    test_large_allocation();
    test_fragmentation();
    test_double_free();
    test_random_operations();
    
    printf("\n=== Basic Tests Completed ===\n");
    printf("\n=== Starting Thread Tests ===\n\n");
    
    if (run_thread_test() != 0) {
        printf("Thread tests failed!\n");
        return 1;
    }
    
    printf("\n=== Thread Tests Completed ===\n");
    printf("\n=== All Tests Passed Successfully! ===\n\n");
    
    return 0;
}
