#include "include.h"

t_heap			*HEAD = NULL;
pthread_mutex_t	malloc_mutex = PTHREAD_MUTEX_INITIALIZER;


void *my_malloc(size_t size) {
	// If size is 0, return
	if (size == 0) {
		return NULL;
	}
	size = ALIGN(size, sizeof(void *));

	pthread_mutex_lock(&malloc_mutex);
	// Create new heap if no heap created until now
	if (HEAD == NULL) {
		HEAD = create_new_heap(size);
		if (!HEAD) {
			printf("Failed to create heap");
			pthread_mutex_unlock(&malloc_mutex);
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
				pthread_mutex_unlock(&malloc_mutex);
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

	pthread_mutex_unlock(&malloc_mutex);
	return ret;
}

void my_free(void *addr) {
	if (!addr) return;

	pthread_mutex_lock(&malloc_mutex);
	t_block *block = addr - sizeof(t_block);
	t_heap *heap = find_block_heap(block);
	if (!heap) {
		printf("ERROR: TRYING TO FREE BLOCK NOT IN HEAP\n");
		pthread_mutex_unlock(&malloc_mutex);
		return ;
	}
	block->freed = true;
	heap->free_size += block->size;
	try_to_merge_block(heap, block);
	t_block *last = find_last_block_of_heap(heap);
	if (last == block) {
		printf("last\n");
		remove_block_from_heap(heap, block);
	}
	if (heap->block_count == 0) {
		printf("removing heap\n");
		remove_heap(heap);
		munmap(heap, heap->total_size);
		printf("removed heap\n");
	}
	pthread_mutex_unlock(&malloc_mutex);
}

void *my_realloc(void *addr, size_t size) {
	// If no addr given, behave like malloc
	if (!addr) {
		return my_malloc(size);
	}

	// If no size given, behave like free
	if (size == 0) {
		my_free(addr);
		return NULL;
	}

	// Find block heap
	t_block *block = addr - sizeof(t_block);
	t_heap *heap = find_block_heap(block);
	if (!heap) {
		printf("ERROR: TRYING TO REALLOC BLOCK NOT IN HEAP\n");
		return NULL;
	}

	// If new size is smaller that previous size, shrink the block
	if (block->size > size) {
		pthread_mutex_lock(&malloc_mutex);
		size_t leftover_size = block->size - size;

		if (leftover_size > sizeof(t_block) + 1) { // Ensure leftover space can form a new block
			t_block *new_block = (t_block *)((char *)BLOCK_SHIFT(block) + size);
			new_block->size = leftover_size - sizeof(t_block);
			new_block->freed = true;
			new_block->next = block->next;
			new_block->prev = block;

			if (block->next) {
				block->next->prev = new_block;
			}

			block->next = new_block;
			block->size = size;

			// Update the heap's free size
			t_heap *heap = find_block_heap(block);
			if (heap) {
				heap->free_size += leftover_size - sizeof(t_block);
				heap->block_count += 1;
			}
		}

		pthread_mutex_unlock(&malloc_mutex);
		return BLOCK_SHIFT(block); // Return the original pointer
	}

	// If next block is free and has enough space, merge the blocks
	if (block->next && block->next->freed == true && block->size + sizeof(t_block) + block->next->size >= size) {
		pthread_mutex_lock(&malloc_mutex);
		t_block *next = block->next;
		block->size += next->size;
		block->next = next->next;
		if (next->next)
			next->next->prev = block;
		heap->block_count -= 1;
		pthread_mutex_unlock(&malloc_mutex);
		return BLOCK_SHIFT(block);
	}
	void *new_addr = my_malloc(size);
	memcpy(new_addr, addr, block->size);
	my_free(addr);
	return new_addr;
}