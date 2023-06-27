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

#define TINY_HEAP_ALLOCATION_SIZE (4 * getpagesize())
#define TINY_BLOCK_SIZE (TINY_HEAP_ALLOCATION_SIZE / 128)
#define SMALL_HEAP_ALLOCATION_SIZE (32 * getpagesize())
#define SMALL_BLOCK_SIZE (SMALL_HEAP_ALLOCATION_SIZE / 128)

void    print_heap_state(t_heap *heap);