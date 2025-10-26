#include <stdio.h>

void *malloc(size_t size);
void free(void *ptr);

int main() {
    printf("Starting minimal test\n");
    fflush(stdout);
    
    void *ptr = malloc(100);
    printf("After malloc: %p\n", ptr);
    fflush(stdout);
    
    if (ptr) {
        free(ptr);
        printf("After free\n");
        fflush(stdout);
    }
    
    printf("Test completed\n");
    return 0;
}
