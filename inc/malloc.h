#ifndef MALLOC_H
#define MALLOC_H

#include <stdbool.h>
#include <stddef.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef enum {
    HEAP_TINY,
    HEAP_SMALL,
    HEAP_LARGE
} e_heap_type;

typedef struct s_block {
    struct s_block *next;
    size_t size;
    bool free;
} t_block;

typedef struct s_heap {
    struct s_heap *next;
    struct s_heap *prev;
    t_block *blocks;   // Pointer to the first block in the heap
    size_t block_count;
    size_t free_size;
    size_t total_size;
    e_heap_type type;  // Type of the heap: TINY, SMALL, LARGE
} t_heap;

#define TINY_HEAP_SIZE (size_t)getpagesize()
#define SMALL_HEAP_SIZE (size_t)(getpagesize() * 4)
#define TINY_BLOCK_SIZE TINY_HEAP_SIZE / 128
#define SMALL_BLOCK_SIZE SMALL_HEAP_SIZE / 64

// Single global variable
extern t_heap *HEAD;

// Add mutex declaration
extern pthread_mutex_t g_malloc_mutex;

// Heap initialization and management
t_heap *init_heap(size_t heap_size, e_heap_type type);
t_heap *add_new_heap(size_t heap_size, e_heap_type type);
t_heap *find_or_create_heap(size_t size);

// Memory allocation
void *my_malloc(size_t size);
void my_free(void *ptr);
void *my_realloc(void *ptr, size_t size);

// Utility functions
void show_alloc_mem(void);
t_heap *find_heap_for_ptr(void *ptr);

#endif