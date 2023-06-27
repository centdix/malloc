#include "include.h"

void    print_heap_state(t_heap *heap) {
    if (heap == NULL) {
        printf("ERROR: TRYING TO PRINT NULL HEAP\n");
        return ;
    }
    printf("======= HEAP %p ======\n", heap);
    printf("block count: %d\n", heap->block_count);
    printf("total size: %d\n", heap->total_size);
    printf("free size: %d\n", heap->free_size);
    printf("  ** blocks **\n");
	t_block *cur = (t_block *)HEAP_SHIFT(heap);
    while (cur) {
        printf("\tblock %p\n", cur);
        printf("\tsize: %d\n", cur->size);
        printf("\tfreed: %d\n", cur->freed);
        cur = cur->next;
    }
}