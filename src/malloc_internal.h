#ifndef MALLOC_INTERNAL_H
#define MALLOC_INTERNAL_H

// Internal allocator interface. Only the implementation files (src/*.c) include
// this; external callers and tests use the public includes/malloc.h. Keeping the
// *_nolock cores, the lock and the heap helpers out of the public header means
// callers cannot bypass g_malloc_mutex by invoking the unlocked cores directly.

#include "malloc.h"

#include <stdbool.h>
#include <stddef.h>
#include <sys/mman.h>
#include <unistd.h>
#include <pthread.h>

// malloc must return memory aligned for any type; on x86_64 that is 16 bytes.
#define ALIGNMENT 16
#define ALIGN_UP(x) (((x) + (ALIGNMENT - 1)) & ~((size_t)(ALIGNMENT - 1)))

// aligned(ALIGNMENT) rounds each header's sizeof up to a multiple of 16, so a
// data pointer (header + sizeof(header)) stays 16-aligned: the heap base is
// page-aligned, sizeof(t_heap)=48 and sizeof(t_block)=32 are both multiples of
// 16, and allocation sizes are rounded with ALIGN_UP -> every block lands on a
// 16-byte boundary.
typedef struct s_block {
    struct s_block *next;
    size_t size;
    bool free;
} __attribute__((aligned(ALIGNMENT))) t_block;

typedef struct s_heap {
    struct s_heap *next;
    struct s_heap *prev;
    t_block *blocks;
    size_t block_count;
    size_t free_size;
    size_t total_size;
} __attribute__((aligned(ALIGNMENT))) t_heap;

#define TINY_HEAP_SIZE (size_t)(getpagesize() * 16)
#define SMALL_HEAP_SIZE (size_t)(getpagesize() * 128)
#define TINY_BLOCK_SIZE 128
#define SMALL_BLOCK_SIZE 4096

// Single global heap list.
extern t_heap *HEAD;

// Global lock serializing all allocator entry points. Recursive as a safety
// margin: the public wrappers take it once and the *_nolock cores never re-take
// it, so a recursive type guards against any future re-entrant output path.
extern pthread_mutex_t g_malloc_mutex;

// Core allocation logic. These assume g_malloc_mutex is already held, which lets
// realloc/calloc reuse them without re-locking. The public malloc/free/realloc
// are thin locking wrappers around these.
void *malloc_nolock(size_t size);
void free_nolock(void *ptr);
void *realloc_nolock(void *ptr, size_t size);

// Heap initialization and management.
t_heap *init_heap(size_t heap_size);
t_heap *add_new_heap(size_t heap_size);
t_heap *create_heap_for_size(size_t block_size);
t_heap *find_suitable_heap(size_t block_size);
size_t get_heap_size(size_t block_size);
void *fill_free_block(t_heap *heap, size_t size);
t_heap *find_heap_for_ptr(void *ptr);

#endif
