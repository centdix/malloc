#include "include.h"

size_t		find_most_appropriate_heap_size(size_t size) {
	if (size <= TINY_BLOCK_SIZE) {
		return TINY_HEAP_ALLOCATION_SIZE;
	} else if (size <= SMALL_BLOCK_SIZE) {
		return SMALL_HEAP_ALLOCATION_SIZE;
	} else {
		return ALIGN(size + sizeof(t_heap) + sizeof(t_block), getpagesize());
	}
}

int 		remove_heap(t_heap *heap) {
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
	return 0;
}

t_heap		*create_new_heap(size_t size) {
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
	return heap;
}

t_block		*create_new_block(t_heap *heap, size_t size) {
	if (!heap || size > heap->free_size) {
		return NULL;
	}

	t_block *block;
	if (heap->block_count == 0) {
		// First block in heap
		block = (t_block*)HEAP_SHIFT(heap);
	} else {
		// Find the last block and add after it
		t_block *last = (t_block*)HEAP_SHIFT(heap);
		while (last->next) {
			last = last->next;
		}
		block = (t_block*)((char*)last + sizeof(t_block) + last->size);
		last->next = block;
		block->prev = last;
	}

	block->next = NULL;
	block->size = size;
	block->freed = false;
	heap->block_count++;
	heap->free_size -= (size + sizeof(t_block));

	return BLOCK_SHIFT(block);
}

t_block		*fill_freed_block(t_heap *heap, size_t size) {
	if (!heap) return NULL;

	t_block *best_fit = NULL;
	size_t smallest_suitable_size = (size_t)-1;

	// Find the best fit among freed blocks
	t_block *cur = (t_block *)HEAP_SHIFT(heap);
	while (cur) {
		if (cur->freed && cur->size >= size) {
			if (cur->size < smallest_suitable_size) {
				best_fit = cur;
				smallest_suitable_size = cur->size;
			}
		}
		cur = cur->next;
	}

	if (!best_fit) return NULL;

	// Split block if it's significantly larger
	if (best_fit->size >= size + sizeof(t_block) + 16) {
		t_block *new_block = (t_block *)((char *)best_fit + sizeof(t_block) + size);
		new_block->size = best_fit->size - size - sizeof(t_block);
		new_block->freed = true;
		new_block->next = best_fit->next;
		new_block->prev = best_fit;
		
		if (best_fit->next) {
			best_fit->next->prev = new_block;
		}
		
		best_fit->next = new_block;
		best_fit->size = size;
		heap->block_count++;
	}

	best_fit->freed = false;
	heap->free_size -= best_fit->size;
	
	return BLOCK_SHIFT(best_fit);
}

t_heap *find_block_heap(t_block *block) {
	if (!block || !HEAD) {
		return NULL;
	}

	t_heap *heap = HEAD;
	while (heap) {
		void *heap_start = HEAP_SHIFT(heap);
		void *heap_end = (void *)((char *)heap + heap->total_size);
		
		if ((void *)block >= heap_start && (void *)block < heap_end) {
			return heap;
		}
		heap = heap->next;
	}
	return NULL;
}

void		remove_block_from_heap(t_heap *heap, t_block *block) {
	t_block *cur = (t_block *)HEAP_SHIFT(heap);
	while (cur) {
		if (cur == block) {
			t_block *next = cur->next;
			t_block *prev = cur->prev;
			if (next)
				next->prev = prev;
			if (prev)
				prev->next = next;
			heap->block_count -= 1;
			printf("removed block from heap => %p\n", heap);
			break;
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
	if (!heap || !block) {
		return;
	}

	// Merge with next block if possible
	if (block->next && block->next->freed) {
		t_block *next = block->next;
		block->size += sizeof(t_block) + next->size;
		block->next = next->next;
		if (next->next) {
			next->next->prev = block;
		}
		heap->block_count--;
		heap->free_size += sizeof(t_block);
	}

	// Merge with previous block if possible
	if (block->prev && block->prev->freed) {
		t_block *prev = block->prev;
		prev->size += sizeof(t_block) + block->size;
		prev->next = block->next;
		if (block->next) {
			block->next->prev = prev;
		}
		heap->block_count--;
		heap->free_size += sizeof(t_block);
	}
}