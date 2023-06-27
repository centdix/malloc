#include "include.h"

void    print_heap_state(t_heap *heap) {
    if (heap == NULL) {
        printf("ERROR: TRYING TO PRINT NULL HEAP\n");
        return ;
    }
    t_heap *cur = heap;
    while (cur) {
        printf("======= HEAP %p ======\n", cur);
        printf("block count: %zu\n", cur->block_count);
        printf("total size: %zu\n", cur->total_size);
        printf("free size: %zu\n", cur->free_size);
        printf("  ** blocks **\n");
        t_block *cur_block = (t_block *)HEAP_SHIFT(cur);
        while (cur_block) {
            printf("\tblock %p\n", cur_block);
            printf("\tsize: %zu\n", cur_block->size);
            printf("\tfreed: %d\n", cur_block->freed);
            cur_block = cur_block->next;
        }
        cur = cur->next;
    }
}