#include "../../includes/malloc.h"

#define M (1024 * 1024)

int main()
{
    // Allocate a spread of sizes across the TINY / SMALL / LARGE classes and
    // leave them live so show_alloc_mem() prints every zone.
    size_t sizes[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512,
                      1024, 1024 * 2, 1024 * 4, 1024 * 32,
                      M, 16 * M, 128 * M};
    size_t n = sizeof(sizes) / sizeof(sizes[0]);
    void *ptrs[17];

    for (size_t i = 0; i < n; i++) {
        ptrs[i] = malloc(sizes[i]);
        if (ptrs[i])
            ((char *)ptrs[i])[0] = 0; // touch the memory so the result is used
    }
    show_alloc_mem();
    return (0);
}