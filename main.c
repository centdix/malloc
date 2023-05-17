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
	heap->all_size = sz;
}

t_block	*create_new_block(t_heap *heap, size_t size) {
	t_block block;
	block->next = NULL;
	block->prev = NULL;
	block->size = size;
	block->freed = false;
	heap->block_count += 1;
	heap->free_size -= size;
	return block;
}

void        *my_malloc(size_t size) {
	t_heap	*heap = HEAD;
	if (HEAD == NULL) {
		heap = create_new_heap();
	}

	t_block	*start = HEAP_SHIFT(heap);
	if (heap->block_count == 0) {
		start = create_new_block(size);
		return BLOCK_SHIFT(start);
	} else {
		
	}


}

int main() {
    // int *array = my_malloc(sizeof(int) * 10);
    // array[0] = 3;
    // array[9] =  8;
    // for (int i = 0; i < 10; i++) {
    //     printf("%d\n", array[i]);
    // }
	int sz = getpagesize();
	printf("%d\n",sz);
    // free(array);
}