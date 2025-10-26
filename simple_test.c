#include <stdio.h>
#include "../includes/malloc.h"

int main() {
    printf("Test starting\n");
    fflush(stdout);
    
    void *ptr = malloc(1);
    printf("After malloc(1): %p\n", ptr);
    fflush(stdout);
    
    show_alloc_mem();
    printf("After show_alloc_mem\n");
    fflush(stdout);
    
    free(ptr);
    printf("After free\n");
    fflush(stdout);
    
    return 0;
}
