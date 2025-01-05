#include "include.h"

// Show allocated memory
void show_alloc_mem() {
    t_heap *heap = HEAD;
    size_t total = 0;

    printf("------\n");
    while (heap) {
        const char *heap_type;
        if (heap->total_size == TINY_HEAP_ALLOCATION_SIZE) {
            heap_type = "TINY";
        } else if (heap->total_size == SMALL_HEAP_ALLOCATION_SIZE) {
            heap_type = "SMALL";
        } else {
            heap_type = "LARGE";
        }

        printf("%s : %p | %zu blocks\n", heap_type, (void *)heap, heap->block_count);

        t_block *block = (t_block *)HEAP_SHIFT(heap);
        while (block) {
            if (!block->freed) {
                printf("%p - %p : %zu bytes\n",
                       (void *)BLOCK_SHIFT(block),
                       (void *)((char *)BLOCK_SHIFT(block) + block->size),
                       block->size);
                total += block->size;
            } else {
                printf("freed block => %p\n", block);
            }
            block = block->next;
        }
        heap = heap->next;
    }
    printf("Total : %zu bytes\n", total);
    printf("------\n");
}