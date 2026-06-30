#include "malloc_internal.h"

void show_alloc_mem() {
    pthread_mutex_lock(&g_malloc_mutex);
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
    pthread_mutex_unlock(&g_malloc_mutex);
}

t_heap *find_heap_for_ptr(void *ptr) {
    t_heap *heap = HEAD;
    while (heap) {
        void *heap_start = (void *)heap;
        void *heap_end = (void *)((char *)heap + heap->total_size);

        // Quick reject: ptr must fall inside this heap's mapping at all.
        if (ptr > heap_start && ptr < heap_end) {
            // Confirm ptr is the user pointer of an actual block in this heap,
            // not merely some interior address. Accepting interior pointers let
            // calls like free(p + 10) pass validation, after which the block
            // header would be read from the middle of a live allocation and
            // corrupt the heap. Walking the block list and requiring an exact
            // match rejects those before any metadata is touched.
            t_block *block = heap->blocks;
            while (block) {
                if ((void *)((char *)block + sizeof(t_block)) == ptr) {
                    return heap;
                }
                block = block->next;
            }
        }
        heap = heap->next;
    }
    return NULL;
} 