#include "malloc.h"

#include "malloc.h"
#include <stdio.h>

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
        
        if (ptr > heap_start && ptr < heap_end) {
            return heap;
        }
        heap = heap->next;
    }
    return NULL;
} 