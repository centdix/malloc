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

typedef struct s_block {
    struct s_block *next;
    size_t size;
    bool free;
} t_block;

typedef struct s_heap {
    struct s_heap *next;
    struct s_heap *prev;
    t_block *blocks;
    size_t block_count;
    size_t free_size;
    size_t total_size;
} t_heap;

#define TINY_HEAP_SIZE (size_t)(getpagesize() * 16)
#define SMALL_HEAP_SIZE (size_t)(getpagesize() * 128)
#define TINY_BLOCK_SIZE 128
#define SMALL_BLOCK_SIZE 4096

// Single global variable
extern t_heap *HEAD;

// Global lock serializing all allocator entry points.
// Recursive so that re-entrant paths are safe: realloc()/calloc() reach the
// allocator through *_nolock cores, but libc routines such as printf() (used by
// show_alloc_mem) call back into malloc() while we already hold the lock when
// this library is LD_PRELOAD'ed.
extern pthread_mutex_t g_malloc_mutex;

// Internal cores: perform the real work WITHOUT taking the lock. They assume
// the caller already holds g_malloc_mutex, which lets realloc reuse malloc/free
// without re-locking. The public malloc/free/realloc are thin locking wrappers.
void *malloc_nolock(size_t size);
void free_nolock(void *ptr);
void *realloc_nolock(void *ptr, size_t size);

// Heap initialization and management
t_heap *init_heap(size_t heap_size);
t_heap *add_new_heap(size_t heap_size);
t_heap *find_or_create_heap(size_t size);
t_heap *create_heap_for_size(size_t block_size);
t_heap *find_suitable_heap(size_t block_size);
size_t get_heap_size(size_t block_size);
void *fill_free_block(t_heap *heap, size_t size);

// Memory allocation
void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
void *calloc(size_t nmemb, size_t size);

// Utility functions
void show_alloc_mem(void);
t_heap *find_heap_for_ptr(void *ptr);

#endif