#include "malloc.h"

void free_heap(t_heap *heap) {
    if (!heap) return;

    munmap(heap, heap->total_size);
}

void remove_heap(t_heap *heap) {
    if (!heap) return;

    // Find the previous heap
    t_heap *current = HEAD;
    t_heap *prev = NULL;
    while (current && current != heap) {
        prev = current;
        current = current->next;
    }

    // Remove the heap from the doubly-linked list
    if (prev) {
        prev->next = heap->next;
    } else {
        HEAD = heap->next;
    }

    // Update the prev pointer of the next heap to maintain doubly-linked list
    if (heap->next) {
        heap->next->prev = prev;
    }
}

void free(void *ptr) {
    if (!ptr) return;

    pthread_mutex_lock(&g_malloc_mutex);

    // Find the heap this block belongs to first
    t_heap *heap = find_heap_for_ptr(ptr);

    // Validate heap exists before accessing block metadata
    if (!heap) {
        pthread_mutex_unlock(&g_malloc_mutex);
        return;
    }

    // Get the block header by moving back sizeof(t_block)
    t_block *block = (t_block *)((char *)ptr - sizeof(t_block));

    // Check if block is already free (double free protection)
    if (block->free) {
        pthread_mutex_unlock(&g_malloc_mutex);
        return;
    }
    
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

    // If only one free block is left, free the heap and remove it
    if (heap && ((heap->block_count == 1 && heap->blocks->free) || heap->block_count == 0)) {
        remove_heap(heap);
        free_heap(heap);
    }

    pthread_mutex_unlock(&g_malloc_mutex);
}