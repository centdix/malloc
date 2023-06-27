#include "include.h"

t_heap	*HEAD = NULL;

t_heap		*create_new_heap(size) {
	int heap_size = 0;
	if (size <= TINY_BLOCK_SIZE) {
		heap_size = TINY_HEAP_ALLOCATION_SIZE;
	} else if (size <= SMALL_BLOCK_SIZE) {
		heap_size = SMALL_HEAP_ALLOCATION_SIZE;
	} else {
		heap_size = size + typeof(t_heap) + typeof(t_block);
	}
	t_heap	*heap = mmap(NULL, heap_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	heap->next = NULL;
	heap->prev = NULL;
	heap->block_count = 0;
	heap->free_size = sz;
	heap->total_size = sz;
	return heap;
}

t_block		*create_new_block(t_heap *heap, size_t size) {
	t_block *block = (t_block*)HEAP_SHIFT(heap);
	if (heap->block_count > 0) {
		int offset = 0;
		t_block	*cur = block;
		for (int i = 0; i < heap->block_count; i++) {
			cur = (t_block *)(HEAP_SHIFT(heap) + offset);
			offset += cur->size + sizeof(t_block);
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
	return BLOCK_SHIFT(block);
}

t_block		*try_filling_block_in_heap(t_heap *heap, size_t size) {
	t_block *cur = (t_block *)HEAP_SHIFT(heap);
	while (cur) {
		if (cur->freed && cur->size >= size) {
			cur->freed = false;
			cur->size = size;
			heap->free_size -= cur->size;
			return BLOCK_SHIFT(cur);
		}
		cur = cur->next;
	}
	return NULL;
}

void        *my_malloc(size_t size) {
	if (HEAD == NULL) {
		HEAD = create_new_heap(size);
	}

	t_heap	*heap = HEAD;
	t_block *ret = NULL;
	ret = try_filling_block_in_heap(heap, size);
	if (!ret) {
		ret = create_new_block(heap, size);
	}
	return ret;
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
	if (block->next && block->next->freed == true) {
		t_block *next = block->next;
		block->size += next->size;
		block->next = next->next;
		if (next->next)
			next->next->prev = block;
		heap->block_count -= 1;
	}
	if (block->prev && block->prev->freed == true) {
		t_block *prev = block->prev;
		block->size += prev->size;
		block->prev = prev->prev;
		if (prev->prev)
			prev->prev->next = block;
		heap->block_count -= 1;
	}
}

void		my_free(void *addr) {
	t_block *block = addr - sizeof(t_block);
	t_heap *heap = find_block_heap(block);
	block->freed = true;
	heap->free_size += block->size;
	try_to_merge_block(heap, block);
	t_block *last = find_last_block_of_heap(heap);
	if (last == block) {
		remove_block_from_heap(heap, block);
	}
	if (heap->block_count == 0) {
		munmap(heap, heap->total_size);
		if (heap == HEAD) {
			HEAD = NULL;
		}
	}
}

int main() {
    int *array = my_malloc(sizeof(int) * 5);
	int *array2 = my_malloc(sizeof(int) * 10);
	int *array3 = my_malloc(sizeof(int) * 15);
	int *array4 = my_malloc(sizeof(int) * 15);
	int *array5 = my_malloc(sizeof(int) * 15);
	int *array6 = my_malloc(sizeof(int) * 15);
	print_heap_state(HEAD);
	// my_free(array);
	// my_free(array);
	my_free(array3);
	print_heap_state(HEAD);
	// int *array7 = my_malloc(sizeof(int) * 3);
	my_free(array5);
	print_heap_state(HEAD);
	my_free(array4);
	print_heap_state(HEAD);
}