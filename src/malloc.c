#include "malloc.h"

t_heap *HEAD = NULL;

t_heap *init_heap(size_t heap_size) {
    // Validate that heap_size is large enough for metadata and at least one block
    size_t min_size = sizeof(t_heap) + sizeof(t_block) + 1;
    if (heap_size < min_size) {
        return NULL;
    }

    t_heap *heap = mmap(NULL, heap_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (heap == MAP_FAILED) {
        return NULL;
    }

    heap->next = NULL;
    heap->prev = NULL;
    heap->blocks = (t_block *)((char *)heap + sizeof(t_heap));
    heap->block_count = 1;  // Start at 1 since we initialize the first block
    heap->free_size = heap_size - sizeof(t_heap) - sizeof(t_block);
    heap->total_size = heap_size;

    // Initialize the first block
    heap->blocks->next = NULL;
    heap->blocks->size = heap->free_size;
    heap->blocks->free = true;

    return heap;
}

t_heap *add_new_heap(size_t heap_size) {
    t_heap *new_heap = init_heap(heap_size);
    if (!new_heap) return NULL;

    // Add the new heap to the global linked list
    if (HEAD == NULL) {
        HEAD = new_heap;
    } else {
        t_heap *cur = HEAD;
        while (cur->next) {
            cur = cur->next;
        }
        cur->next = new_heap;
        new_heap->prev = cur;
    }

    return new_heap;
}

size_t get_heap_size(size_t block_size) {
    if (block_size <= TINY_BLOCK_SIZE) {
        return TINY_HEAP_SIZE;
    } else if (block_size <= SMALL_BLOCK_SIZE) {
        return SMALL_HEAP_SIZE;
    } else {
        return block_size + sizeof(t_heap) + sizeof(t_block);
    }
}

t_heap *find_suitable_heap(size_t block_size) {
    size_t heap_size = get_heap_size(block_size);
    t_heap *cur = HEAD;
    while (cur) {
        if (cur->total_size == heap_size && cur->free_size >= block_size) {
            return cur;
        }
        cur = cur->next;
    }
    return NULL;
}

t_heap *create_heap_for_size(size_t block_size) {
    size_t heap_size = get_heap_size(block_size);
    return add_new_heap(heap_size);
}

void *fill_free_block(t_heap *heap, size_t size) {
    t_block *block = heap->blocks;
    while (block) {
        if (block->free && block->size >= size) {
            block->free = false;

            // Prevent integer underflow by checking before subtraction
            size_t reduction = sizeof(t_block) + size;
            if (heap->free_size >= reduction) {
                heap->free_size -= reduction;
            } else {
                // Should not happen in normal operation, but prevent underflow
                heap->free_size = 0;
            }

            // Split the block if there's excess space
            if (block->size >= size + sizeof(t_block) + 1) {
                t_block *new_block = (t_block *)((char *)block + sizeof(t_block) + size);
                new_block->size = block->size - size - sizeof(t_block);
                new_block->free = true;
                new_block->next = block->next;

                block->size = size;
                block->next = new_block;
                heap->block_count++;
            }

            void *result = (char *)block + sizeof(t_block);
            return result;
        }
        block = block->next;
    }
    return NULL;
}

void *malloc(size_t size) {
    if (size == 0) return NULL;

    size = (size + sizeof(void *) - 1) & ~(sizeof(void *) - 1); // Align size

    t_heap *heap = find_suitable_heap(size);
    if (!heap) {
        heap = create_heap_for_size(size);
        if (!heap) {
            return NULL;
        }
    }

    // Find a free block in the selected heap
    void *result = fill_free_block(heap, size);
    if (result) {
        return result;
    }

    // If no suitable block was found, create a new heap
    heap = create_heap_for_size(size);
    if (!heap) {
        return NULL;
    }

    result = fill_free_block(heap, size);
    if (result) {
        return result;
    }

    return NULL;
}