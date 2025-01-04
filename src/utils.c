#include "include.h"

size_t		find_most_appropriate_heap_size(size_t size) {
	if (size <= TINY_BLOCK_SIZE) {
		return TINY_HEAP_ALLOCATION_SIZE;
	} else if (size <= SMALL_BLOCK_SIZE) {
		return SMALL_HEAP_ALLOCATION_SIZE;
	} else {
		return size + sizeof(t_heap) + sizeof(t_block);
	}
}

int 		remove_heap(t_heap *heap) {
	printf("remove heap => %p\n", heap);
	if (heap == HEAD) {
		HEAD = heap->next;
		if (HEAD)
			HEAD->prev = NULL;
	} else {
		t_heap *prev = heap->prev;
		t_heap *next = heap->next;
		if (prev)
			prev->next = next;
		if (next)
			next->prev = prev;
	}
	printf("remove heap success\n");
	printf("====================================\n");
	return 0;
}

t_heap		*create_new_heap(size_t size) {
	printf("create new heap\n");
	size_t heap_size = find_most_appropriate_heap_size(size);
	t_heap	*heap = mmap(NULL, heap_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (heap == MAP_FAILED) {
		printf("mmap failed");
		return NULL;
	}
	heap->next = NULL;
	heap->prev = NULL;
	heap->block_count = 0;
	heap->free_size = heap_size - sizeof(t_heap);
	heap->total_size = heap_size;
	
	t_heap *cur = HEAD;
	if (cur) {
		while (cur->next) {
			cur = cur->next;
		}
		cur->next = heap;
		heap->prev = cur;
	}
	printf("create new heap success size => %zu, heap => %p\n", heap_size, heap);
	printf("====================================\n");
	return heap;
}

t_block		*create_new_block(t_heap *heap, size_t size) {
	printf("create_new_block in heap => %p\n", heap);
	t_block *block = (t_block*)HEAP_SHIFT(heap);
	if (heap->block_count > 0) {
		int offset = 0;
		t_block	*cur = block;
		for (int i = 0; i < heap->block_count; i++) {
			cur = (t_block *)(HEAP_SHIFT(heap) + offset);
			offset += cur->size + sizeof(t_block);
		}
		if (offset + sizeof(t_block) + size > heap->total_size) {
			printf("Not enough space in heap\n");
			printf("====================================\n");
			return NULL;
		}
		block = (t_block *)(HEAP_SHIFT(heap) + offset);
		block->prev = cur;
		cur->next = block;
	}
	block->next = NULL;
	if (heap->block_count == 0)
		block->prev = NULL;
	block->size = size;
	block->freed = false;
	heap->block_count += 1;
	heap->free_size -= size;
	printf("====================================\n");
	return BLOCK_SHIFT(block);
}

t_block		*fill_freed_block(t_heap *heap, size_t size) {
	printf("try filling block in heap => %p\n", heap);
	t_block *cur = (t_block *)HEAP_SHIFT(heap);
	while (cur) {
		if (cur->freed && cur->size >= size) {
			cur->freed = false;
			cur->size = size;
			heap->free_size -= cur->size;
			printf("fill_freed_block success in heap => %p\n", heap);
			return BLOCK_SHIFT(cur);
		}
		cur = cur->next;
	}
	printf("fill_freed_block failed\n");
	printf("====================================\n");
	return NULL;
}

t_heap		*find_block_heap(t_block *block) {
	t_heap	*heap = HEAD;
	while (heap) {
		t_block *cur = (t_block *)HEAP_SHIFT(heap);
		while (cur) {
			if (cur == block)
				return heap;
			cur = cur->next;
		}
		heap = heap->next;
	}
	return NULL;
}

void		remove_block_from_heap(t_heap *heap, t_block *block) {
	t_block *cur = (t_block *)HEAP_SHIFT(heap);
	heap->block_count -= 1;
	while (cur) {
		if (cur == block) {
			t_block *next = cur->next;
			t_block *prev = cur->prev;
			if (next)
				next->prev = prev;
			if (prev)
				prev->next = next;
		}
		cur = cur->next;
	}
}

t_block		*find_last_block_of_heap(t_heap *heap) {
	t_block *cur = (t_block *)HEAP_SHIFT(heap);
	while (cur->next) {
		cur = cur->next;
	}
	return cur;
}

void		try_to_merge_block(t_heap *heap, t_block *block) {
	printf("try_to_merge_block in heap => %p\n", heap);
	
	if (block->next && block->next->freed == true) {
		t_block *next = block->next;
		block->size += sizeof(t_block) + next->size;
		block->next = next->next;
		if (next->next)
			next->next->prev = block;
		heap->block_count -= 1;
		printf("try_to_merge_block success in heap => %p\n", heap);
		printf("====================================\n");
	}
	if (block->prev && block->prev->freed == true) {
		t_block *prev = block->prev;
		block->size += sizeof(t_block) + prev->size;
		block->prev = prev->prev;
		if (prev->prev)
			prev->prev->next = block;
		heap->block_count -= 1;
		printf("try_to_merge_block success in heap => %p\n", heap);
		printf("====================================\n");
	}
}