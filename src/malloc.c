#include "malloc.h"

t_heap *HEAD = NULL;
pthread_mutex_t g_malloc_mutex = PTHREAD_MUTEX_INITIALIZER;

t_heap *init_heap(size_t heap_size, e_heap_type type) {
    t_heap *heap = mmap(NULL, heap_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (heap == MAP_FAILED) {
        perror("mmap failed");
        return NULL;
    }

    heap->next = NULL;
    heap->prev = NULL;
    heap->blocks = (t_block *)((char *)heap + sizeof(t_heap));
    heap->block_count = 0;
    heap->free_size = heap_size - sizeof(t_heap);
    heap->total_size = heap_size;
    heap->type = type;

    // Initialize the first block
    heap->blocks->next = NULL;
    heap->blocks->size = heap->free_size;
    heap->blocks->free = true;

    return heap;
}

t_heap *add_new_heap(size_t heap_size, e_heap_type type) {
    t_heap *new_heap = init_heap(heap_size, type);
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

t_heap *find_or_create_heap(size_t size) {
    e_heap_type type;
    size_t heap_size;

    if (size <= TINY_BLOCK_SIZE) {
        type = HEAP_TINY;
        heap_size = TINY_HEAP_SIZE;
    } else if (size <= SMALL_BLOCK_SIZE) {
        type = HEAP_SMALL;
        heap_size = SMALL_HEAP_SIZE;
    } else {
        type = HEAP_LARGE;
        heap_size = size + sizeof(t_heap) + sizeof(t_block);
    }

    // Search for a suitable heap
    t_heap *cur = HEAD;
    while (cur) {
        if (cur->type == type && cur->free_size >= size) {
            return cur;
        }
        cur = cur->next;
    }

    // Create a new heap if no suitable heap is found
    return add_new_heap(heap_size, type);
}


void *my_malloc(size_t size) {
    if (size == 0) return NULL;

    pthread_mutex_lock(&g_malloc_mutex);
    
    size = (size + sizeof(void *) - 1) & ~(sizeof(void *) - 1); // Align size

    t_heap *heap = find_or_create_heap(size);
    if (!heap) {
        pthread_mutex_unlock(&g_malloc_mutex);
        return NULL;
    }

    // Find a free block in the selected heap
    t_block *block = heap->blocks;
    while (block) {
        if (block->free && block->size >= size) {
            block->free = false;
            heap->free_size -= sizeof(t_block) + size;

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
            pthread_mutex_unlock(&g_malloc_mutex);
            return result;
        }
        block = block->next;
    }

    pthread_mutex_unlock(&g_malloc_mutex);
    return NULL;
}