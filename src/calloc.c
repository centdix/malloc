#include "malloc.h"
#include "libft.h"

void *calloc(size_t nmemb, size_t size) {
    // Check for multiplication overflow
    if (nmemb != 0 && size > (size_t)-1 / nmemb) {
        return NULL;
    }

    size_t total_size = nmemb * size;

    void *ptr = malloc(total_size);
    if (!ptr) {
        return NULL;
    }

    // Zero out the memory
    ft_bzero(ptr, total_size);

    return ptr;
}
