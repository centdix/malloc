#include "include.h"

t_heap		*HEAD = NULL;

void        *my_malloc(size_t size) {
	printf("my_malloc size => %zu\n", size);
	if (HEAD == NULL) {
		HEAD = create_new_heap(size);
	}

	t_heap	*heap = HEAD;
	t_block *ret = NULL;
	size_t appropriate_heap_size = find_most_appropriate_heap_size(size);
	while (heap) {
		if (heap->free_size >= size && heap->total_size <= appropriate_heap_size) {
			ret = try_filling_block_in_heap(heap, size);
			if (!ret) {
				ret = create_new_block(heap, size);
			}
			if (ret)
				return ret;
		}
		heap = heap->next;
	}
	if (!ret) {
		heap = create_new_heap(size);
		ret = create_new_block(heap, size);
	}
	return ret;
}

void		my_free(void *addr) {
	printf("my_free addr => %p\n", addr);
	t_block *block = addr - sizeof(t_block);
	t_heap *heap = find_block_heap(block);
	if (!heap) {
		printf("ERROR: TRYING TO FREE BLOCK NOT IN HEAP\n");
		return ;
	} else {
		printf("found heap => %p\n", heap);
	}
	block->freed = true;
	heap->free_size += block->size;
	try_to_merge_block(heap, block);
	t_block *last = find_last_block_of_heap(heap);
	if (last == block) {
		printf("last block in heap => %p\n", last);
		remove_block_from_heap(heap, block);
	}
	if (heap->block_count == 0) {
		printf("heap block count is 0, deallocating\n");
		remove_heap(heap);
		munmap(heap, heap->total_size);
	}
	printf("====================================\n");
}

void		*my_realloc(void *addr, size_t size) {
	t_block *block = addr - sizeof(t_block);
	t_heap *heap = find_block_heap(block);
	if (!heap) {
		printf("ERROR: TRYING TO REALLOC BLOCK NOT IN HEAP\n");
		return NULL;
	} else {
		printf("found heap => %p\n", heap);
	}
	if (block->next && block->next->freed == true && block->size + block->next->size >= size) {
		t_block *next = block->next;
		block->size += next->size;
		block->next = next->next;
		if (next->next)
			next->next->prev = block;
		heap->block_count -= 1;
		return BLOCK_SHIFT(block);
	}
	if (block->prev && block->prev->freed == true && block->size + block->prev->size >= size) {
		t_block *prev = block->prev;
		block->size += prev->size;
		block->prev = prev->prev;
		if (prev->prev)
			prev->prev->next = block;
		heap->block_count -= 1;
		return BLOCK_SHIFT(block);
	}
	void *new_addr = my_malloc(size);
	memcpy(new_addr, addr, block->size);
	my_free(addr);
	return new_addr;
}