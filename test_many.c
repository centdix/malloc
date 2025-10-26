#include <stdio.h>
#include <stdlib.h>

int main() {
    void *ptrs[1000];

    printf("Allocating 1000 blocks...\n");
    for (int i = 0; i < 1000; i++) {
        ptrs[i] = malloc(100);
        if (ptrs[i] == NULL) {
            printf("Failed at %d\n", i);
            return 1;
        }
        if (i % 100 == 0) {
            printf("Allocated %d blocks\n", i);
        }
    }

    printf("Freeing blocks...\n");
    for (int i = 0; i < 1000; i++) {
        free(ptrs[i]);
        if (i % 100 == 0) {
            printf("Freed %d blocks\n", i);
        }
    }

    printf("Success!\n");
    return 0;
}
