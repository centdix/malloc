#include "malloc.h"

void my_free(void *ptr) {
    if (!ptr) return;

    pthread_mutex_lock(&g_malloc_mutex);

    // Get the block header by moving back sizeof(t_block)
    t_block *block = (t_block *)((char *)ptr - sizeof(t_block));
    
    // Validate the block before using it
    if (block->free) {
        // Block is already free, possible double free
        pthread_mutex_unlock(&g_malloc_mutex);
        return;
    }
    
    // Find the heap this block belongs to
    t_heap *heap = find_heap_for_ptr(ptr);
    if (heap) {
        // Mark the block as free
        block->free = true;
        
        // Update heap's free size with the actual block size
        heap->free_size += block->size;
        
        // Try to merge with adjacent free blocks
        t_block *current = heap->blocks;
        while (current && current->next) {
            if (current->free && current->next->free) {
                // Update free_size before merging
                heap->free_size += sizeof(t_block);
                
                // Merge blocks
                current->size += sizeof(t_block) + current->next->size;
                current->next = current->next->next;
                heap->block_count--;
            } else {
                current = current->next;
            }
        }
    }

    pthread_mutex_unlock(&g_malloc_mutex);
}