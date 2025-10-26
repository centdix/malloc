#include "malloc.h"

#include "malloc.h"
#include <stdio.h>

void show_alloc_mem() {
    t_heap *cur = HEAD;
    size_t total_allocated = 0;
    printf("Memory Allocation State:\n");

    while (cur) {
        const char *heap_type = (cur->total_size == TINY_HEAP_SIZE) ? "TINY" :
                                (cur->total_size == SMALL_HEAP_SIZE) ? "SMALL" : "LARGE";
        printf("%s : %p\n", heap_type, (void *)cur);

        t_block *block = cur->blocks;
        while (block) {
            if (!block->free) {
                void *start = (void *)((char *)block + sizeof(t_block));
                void *end = (void *)((char *)start + block->size);
                printf("%p - %p : %zu bytes\n", start, end, block->size);
                total_allocated += block->size;
            }
            block = block->next;
        }

        cur = cur->next;
    }

    printf("Total : %zu bytes\n", total_allocated);
}

t_heap *find_heap_for_ptr(void *ptr) {
    t_heap *heap = HEAD;
    while (heap) {
        void *heap_start = (void *)heap;
        void *heap_end = (void *)((char *)heap + heap->total_size);

        // Check if pointer is within heap bounds
        if (ptr > heap_start && ptr < heap_end) {
            // Additional validation: check if ptr points to a valid block
            // by ensuring it's properly aligned and within the blocks region
            void *blocks_start = (void *)heap->blocks;

            if (ptr >= blocks_start && ptr < heap_end) {
                // Verify the pointer could be a valid allocation
                // (points to data section, not block header)
                t_block *potential_block = (t_block *)((char *)ptr - sizeof(t_block));

                // Check if this block header is within valid range
                if ((void *)potential_block >= blocks_start &&
                    (void *)potential_block < heap_end) {
                    return heap;
                }
            }
        }
        heap = heap->next;
    }
    return NULL;
} 