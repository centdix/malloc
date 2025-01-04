#include <sys/mman.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

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

#define ALIGN(size, align) (((size) + (align) - 1) & ~((align) - 1))
#define HEAP_SHIFT(start) ((void *)ALIGN((uintptr_t)(start) + sizeof(t_heap), sizeof(void *)))
#define BLOCK_SHIFT(start) ((void *)ALIGN((uintptr_t)(start) + sizeof(t_block), sizeof(void *)))

#define TINY_HEAP_ALLOCATION_SIZE (1 * getpagesize())
#define TINY_BLOCK_SIZE (TINY_HEAP_ALLOCATION_SIZE / 128)
#define SMALL_HEAP_ALLOCATION_SIZE (4 * getpagesize())
#define SMALL_BLOCK_SIZE (SMALL_HEAP_ALLOCATION_SIZE / 128)

extern t_heap	*HEAD;

void        *my_malloc(size_t size);
void        my_free(void *addr);
void        *my_realloc(void *addr, size_t size);

size_t      find_most_appropriate_heap_size(size_t size);
int         remove_heap(t_heap *heap);
t_heap	    *create_new_heap(size_t size);
t_block     *create_new_block(t_heap *heap, size_t size);
t_block	    *fill_freed_block(t_heap *heap, size_t size);
void		try_to_merge_block(t_heap *heap, t_block *block);
t_block		*find_last_block_of_heap(t_heap *heap);
void		remove_block_from_heap(t_heap *heap, t_block *block);
t_heap		*find_block_heap(t_block *block);

void        show_alloc_mem();