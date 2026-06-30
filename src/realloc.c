#include "malloc.h"
#include "libft.h"

// Core realloc logic. Assumes g_malloc_mutex is already held, so it reuses the
// *_nolock cores instead of the public malloc/free (which would re-lock).
void *realloc_nolock(void *ptr, size_t size) {
    // If ptr is NULL, realloc behaves like malloc
    if (!ptr) {
        return malloc_nolock(size);
    }

    // If size is 0 and ptr is not NULL, realloc behaves like free
    if (size == 0) {
        free_nolock(ptr);
        return NULL;
    }

    // Round up so an in-place split keeps the new block 16-aligned (matches
    // malloc_nolock, which rounds before storing block->size).
    size = ALIGN_UP(size);

    // Validate that ptr belongs to a valid heap before accessing block metadata
    t_heap *heap = find_heap_for_ptr(ptr);
    if (!heap) {
        return NULL;
    }

    // Get the block header
    t_block *block = (t_block *)((char *)ptr - sizeof(t_block));

    // Save the old size
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
            heap->block_count++;
            heap->free_size += new_block->size;
        }
        return ptr;
    }

    // Need to allocate a new block
    void *new_ptr = malloc_nolock(size);
    if (!new_ptr) {
        return NULL;
    }

    // Copy old data to new location
    ft_memcpy(new_ptr, ptr, old_size);

    // Free the old block
    free_nolock(ptr);

    return new_ptr;
}

// Public entry point: lock, run the core, unlock.
void *realloc(void *ptr, size_t size) {
    pthread_mutex_lock(&g_malloc_mutex);
    void *res = realloc_nolock(ptr, size);
    pthread_mutex_unlock(&g_malloc_mutex);
    return res;
}
