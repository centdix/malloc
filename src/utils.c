#include "malloc.h"

void show_alloc_mem() {
    pthread_mutex_lock(&g_malloc_mutex);
    
    t_heap *cur = HEAD;
    printf("Memory Allocation State:\n");
    while (cur) {
        printf("Heap (%s): %p, Total Size: %zu, Free Size: %zu\n",
               (cur->total_size == TINY_HEAP_SIZE) ? "TINY" :
               (cur->total_size == SMALL_HEAP_SIZE) ? "SMALL" : "LARGE",
               (void *)cur, cur->total_size, cur->free_size);

        t_block *block = cur->blocks;
        while (block) {
            printf("  Block: %p, Size: %zu, Free: %d\n",
                   (void *)block, block->size, block->free);
            block = block->next;
        }

        cur = cur->next;
    }

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