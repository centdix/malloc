#include "include.h"

t_heap			*HEAD = NULL;
pthread_mutex_t	g_malloc_mutex = PTHREAD_MUTEX_INITIALIZER;

void *_malloc(size_t size) {
	// If size is 0, return
	if (size == 0) {
		return NULL;
	}
	size = ALIGN(size, sizeof(void *));

	// Create new heap if no heap created until now
	if (HEAD == NULL) {
		HEAD = create_new_heap(size);
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
			if (!ret) {
				// Create a new block
				ret = create_new_block(heap, size);
			}
			if (ret) {
				return ret;
			}
		}
		heap = heap->next;
	}

	// If no appropriate heap found, create a new one
	if (!ret) {
		heap = create_new_heap(size);
		ret = create_new_block(heap, size);
	}

	return ret;
}

void _free(void *addr) {
	if (!addr) return;

	t_block *block = (t_block *)(addr - sizeof(t_block));
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
	t_block *last = find_last_block_of_heap(heap);
	if (last == block) {
		remove_block_from_heap(heap, block);
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

	size = ALIGN(size, sizeof(void *));
	t_block *block = (t_block *)((char *)addr - sizeof(t_block));
	t_heap *heap = find_block_heap(block);
	printf("heap: %p\n", heap);
	
	if (!heap) {
		printf("ERROR: TRYING TO REALLOC BLOCK NOT IN HEAP\n");
		return NULL;
	}

	if (block->freed) {
		printf("ERROR: TRYING TO REALLOC FREED BLOCK\n");
		return NULL;
	}

	// If new size is the same as previous size, return the original pointer
	if (block->size == size) {
		printf("returning original pointer\n");
		return addr;
	}

	// If new size is smaller that previous size, shrink the block
	if (block->size > size) {
		size_t leftover_size = block->size - size;
		printf("leftover_size: %zu\n", leftover_size);
		if (leftover_size > sizeof(t_block) + 1) {
			printf("creating new block\n");
			t_block *new_block = (t_block *)((char *)BLOCK_SHIFT(block) + size);
			new_block->size = leftover_size - sizeof(t_block);
			new_block->freed = true;
			new_block->next = block->next;
			new_block->prev = block;

			if (block->next) {
				printf("setting prev of next block\n");
				block->next->prev = new_block;
			}

			block->next = new_block;
			block->size = size;

			heap->free_size += leftover_size - sizeof(t_block);
			heap->block_count += 1;
			printf("heap->block_count: %zu\n", heap->block_count);
		}

		return BLOCK_SHIFT(block);
	}

	// If next block is free and has enough space, merge the blocks
	if (block->next && block->next->freed == true && 
		block->size + sizeof(t_block) + block->next->size >= size) {
		t_block *next = block->next;
		printf("merging blocks\n");
		block->size += next->size;
		block->next = next->next;
		if (next->next)
			next->next->prev = block;
		heap->block_count -= 1;
		return BLOCK_SHIFT(block);
	}

	// If we can't reuse the current block, allocate a new one
	void *new_addr = _malloc(size);
	if (new_addr) {
		printf("copying data\n");
		memcpy(new_addr, addr, block->size);
		_free(addr);
		return new_addr;
	}

	printf("returning NULL\n");
	return NULL;
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