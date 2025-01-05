#include "include.h"

t_heap			*HEAD = NULL;
pthread_mutex_t	g_malloc_mutex = PTHREAD_MUTEX_INITIALIZER;

void *_malloc(size_t size) {
	printf("malloc size: %zu\n", size);
	// If size is 0, return
	if (size == 0) {
		return NULL;
	}
	size = ALIGN(size, sizeof(void *));

	// Create new heap if no heap created until now
	if (HEAD == NULL) {
		HEAD = create_new_heap(size);
		printf("created new heap\n");
		if (!HEAD) {
			printf("Failed to create heap");
			return NULL;
		}
	}

	// Place the block in the appropriate heap
	t_heap	*heap = HEAD;
	t_block *ret = NULL;
	size_t appropriate_heap_size = find_most_appropriate_heap_size(size);
	while (heap) {
		if (heap->free_size >= size && heap->total_size == appropriate_heap_size) {
			// Try filling freed block
			ret = fill_freed_block(heap, size);
			printf("fill_freed_block: %p\n", ret);
			if (!ret) {
				// Create a new block
				ret = create_new_block(heap, size);
				printf("create_new_block: %p\n", ret);
				if (!ret) {
					printf("create_new_block failed\n");
				}
			}
			if (ret) {
				printf("returning ret\n");
				return ret;
			}
		}
		heap = heap->next;
	}

	// If no appropriate heap found, create a new one
	if (!ret) {
		printf("creating new heap\n");
		heap = create_new_heap(size);
		printf("creating new block\n");
		ret = create_new_block(heap, size);
	}

	return ret;
}

void _free(void *addr) {
	if (!addr) return;

	t_block *block = (t_block *)(addr - sizeof(t_block));
	printf("block: %p\n", block);
	t_heap *heap = find_block_heap(block);
	if (!heap) {
		printf("ERROR: TRYING TO FREE BLOCK NOT IN HEAP\n");
		return;
	}

	if (block->freed) {
		printf("ERROR: DOUBLE FREE DETECTED\n");
		return;
	}

	block->freed = true;
	heap->free_size += block->size;
	try_to_merge_block(heap, block);
	printf("try_to_merge_block: %p\n", block);
	t_block *last = find_last_block_of_heap(heap);
	if (last == block) {
		remove_block_from_heap(heap, block);
		printf("removed block\n");
	}
	if (heap->block_count == 0) {
		remove_heap(heap);
		munmap(heap, heap->total_size);
		printf("removed heap\n");
	}
}

void *_realloc(void *addr, size_t size) {
	if (!addr) {
		return _malloc(size);
	}

	if (size == 0) {
		_free(addr);
		return NULL;
	}

	t_block *block = (t_block *)((char *)addr - sizeof(t_block));
	t_heap *heap = find_block_heap(block);
	
	if (!heap || block->freed) {
		return NULL;
	}

	size = ALIGN(size, sizeof(void *));

	// If new size is smaller, shrink in place
	if (size <= block->size) {
		size_t excess = block->size - size;
		
		// Only split if we save significant space
		if (excess >= sizeof(t_block) + 16) {
			t_block *new_block = (t_block *)((char *)block + sizeof(t_block) + size);
			new_block->size = excess - sizeof(t_block);
			new_block->freed = true;
			new_block->next = block->next;
			new_block->prev = block;
			
			if (block->next) {
				block->next->prev = new_block;
			}
			
			block->next = new_block;
			block->size = size;
			
			heap->block_count++;
			heap->free_size += excess - sizeof(t_block);
		}
		return addr;
	}

	// Try to expand in place if possible
	size_t needed_size = size - block->size;
	if (block->next && block->next->freed && 
		(block->next->size + sizeof(t_block)) >= needed_size) {
		// Merge with next block
		block->size += sizeof(t_block) + block->next->size;
		block->next = block->next->next;
		if (block->next) {
			block->next->prev = block;
		}
		heap->block_count--;
		return addr;
	}

	// Otherwise, allocate new block and copy
	void *new_ptr = _malloc(size);
	if (!new_ptr) {
		return NULL;
	}
	
	memcpy(new_ptr, addr, block->size);
	_free(addr);
	
	return new_ptr;
}

void *my_malloc(size_t size) {
	pthread_mutex_lock(&g_malloc_mutex);
	void *ptr = _malloc(size);
	pthread_mutex_unlock(&g_malloc_mutex);
	return ptr;
}

void my_free(void *ptr) {
	pthread_mutex_lock(&g_malloc_mutex);
	_free(ptr);
	pthread_mutex_unlock(&g_malloc_mutex);
}

void *my_realloc(void *ptr, size_t size) {
	pthread_mutex_lock(&g_malloc_mutex);
	void *new_ptr = _realloc(ptr, size);
	pthread_mutex_unlock(&g_malloc_mutex);
	return new_ptr;
}