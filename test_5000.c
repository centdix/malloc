#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    void *ptrs[5000];
    
    printf("Allocating 5000 blocks...\n");
    for (int i = 0; i < 5000; i++) {
        size_t size = (rand() % 1024) + 1;
        ptrs[i] = malloc(size);
        if (ptrs[i] == NULL) {
            printf("Failed at %d\n", i);
            return 1;
        }
        memset(ptrs[i], 'A', size - 1);
        ((char *)ptrs[i])[size - 1] = '\0';
        
        if (i % 500 == 0) {
            printf("Allocated %d blocks\n", i);
        }
    }
    
    printf("Freeing blocks...\n");
    for (int i = 4999; i >= 0; i--) {
        if (ptrs[i] != NULL)
            free(ptrs[i]);
        if (i % 500 == 0) {
            printf("Freed down to %d\n", i);
        }
    }
    
    printf("Success!\n");
    return 0;
}
