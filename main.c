#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>

typedef struct  s_heap {
    struct s_heap	*next;
    struct s_heap	*prev;
    size_t			block_count;
	size_t			free_size;
	size_t			total_size;
}               t_heap;

typedef struct s_block {
    struct s_block  *next;
    struct s_block  *prev;
	size_t			size;
	bool			freed;
}               t_block;

#define HEAP_SHIFT(start) ((void*)start + sizeof(t_heap))
#define BLOCK_SHIFT(start) ((void*)start + sizeof(t_block))

t_heap	*HEAD = NULL;

t_heap	*create_new_heap() {
	int sz = getpagesize();
	t_heap	*heap = mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	heap->next = NULL;
	heap->prev = NULL;
	heap->block_count = 0;
	heap->free_size = sz;
	heap->total_size = sz;
	return heap;
}

t_block	*create_new_block(t_heap *heap, size_t size) {
	t_block *block = (t_block*)HEAP_SHIFT(heap);
	if (heap->block_count > 0) {
		int offset = 0;
		t_block	*cur = block;
		for (int i = 0; i < heap->block_count; i++) {
			cur = (t_block *)(HEAP_SHIFT(heap) + offset);
			offset += cur->size + sizeof(t_block);
		}
		block = (t_block *)(HEAP_SHIFT(heap) + offset);
		cur->next = block;
	}
	block->next = NULL;
	block->prev = NULL;
	block->size = size;
	block->freed = false;
	heap->block_count += 1;
	heap->free_size -= size;
	return BLOCK_SHIFT(block);
}

void        *my_malloc(size_t size) {
	if (HEAD == NULL) {
		HEAD = create_new_heap();
		printf("fewf");
	}

	t_heap	*heap = HEAD;
	t_block *ret = create_new_block(heap, size);
	printf("----heap----\n");
	printf("free size: %d\n", heap->free_size);
	printf("block count: %d\n", heap->block_count);
	printf("----blocks list----\n");
	t_block *cur = (t_block *)HEAP_SHIFT(heap);
	while (cur) {
		printf("size: %d\n", cur->size);
		cur = cur->next;
	}
	return ret;
}

int main() {
    int *array = my_malloc(sizeof(int) * 5);
    // array[0] = 3;
    // array[9] =  8;
    // for (int i = 0; i < 10; i++) {
    //     printf("%d\n", array[i]);
    // }
	int *array2 = my_malloc(sizeof(int) * 10);
	int *array3 = my_malloc(sizeof(int) * 15);
    array2[0] = 3;
    array2[9] =  8;
    // for (int i = 0; i < 10; i++) {
    //     printf("%d\n", array2[i]);
    // }
    // free(array);
}