#include "malloc.h"
#include "libft.h"

void *realloc(void *ptr, size_t size) {
    // If ptr is NULL, realloc behaves like malloc
    if (!ptr) {
        return malloc(size);
    }

    // If size is 0 and ptr is not NULL, realloc behaves like free
    if (size == 0) {
        free(ptr);
        return NULL;
    }

    pthread_mutex_lock(&g_malloc_mutex);

    // Get the block header
    t_block *block = (t_block *)((char *)ptr - sizeof(t_block));

    // Save the old size before potentially releasing the lock
    size_t old_size = block->size;

    // If the requested size fits in the current block, reuse it
    if (old_size >= size) {
        // Split the block if there's enough extra space
        if (old_size >= size + sizeof(t_block) + 1) {
            t_block *new_block = (t_block *)((char *)block + sizeof(t_block) + size);
            new_block->size = old_size - size - sizeof(t_block);
            new_block->free = true;
            new_block->next = block->next;

            block->size = size;
            block->next = new_block;

            // Update heap metadata
            t_heap *heap = find_heap_for_ptr(ptr);
            if (heap) {
                heap->block_count++;
                heap->free_size += new_block->size;
            }
        }
        pthread_mutex_unlock(&g_malloc_mutex);
        return ptr;
    }

    // Need to allocate a new block
    // Keep the mutex locked to prevent ptr from being freed by another thread
    void *new_ptr = malloc(size);
    if (!new_ptr) {
        pthread_mutex_unlock(&g_malloc_mutex);
        return NULL;
    }

    // Copy old data to new location (mutex still locked)
    ft_memcpy(new_ptr, ptr, old_size);

    // Mark old block as free (don't call free() to avoid deadlock)
    block->free = true;
    t_heap *heap = find_heap_for_ptr(ptr);
    if (heap) {
        heap->free_size += old_size;

        // Try to merge with adjacent free blocks
        t_block *current = heap->blocks;
        while (current && current->next) {
            if (current->free && current->next->free) {
                heap->free_size += sizeof(t_block);
                current->size += sizeof(t_block) + current->next->size;
                current->next = current->next->next;
                heap->block_count--;
            } else {
                current = current->next;
            }
        }

        // Check if heap should be freed
        if ((heap->block_count == 1 && heap->blocks->free) || heap->block_count == 0) {
            // Remove from list
            if (heap->prev) {
                heap->prev->next = heap->next;
            } else {
                HEAD = heap->next;
            }
            if (heap->next) {
                heap->next->prev = heap->prev;
            }
            pthread_mutex_unlock(&g_malloc_mutex);
            munmap(heap, heap->total_size);
            return new_ptr;
        }
    }

    pthread_mutex_unlock(&g_malloc_mutex);
    return new_ptr;
} 