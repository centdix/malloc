#ifndef MALLOC_H
#define MALLOC_H

#include <stddef.h> // size_t

// Public allocator API. Implementation internals (heap structures, the lock and
// the *_nolock cores) live in src/malloc_internal.h.
void *malloc(size_t size);
void free(void *ptr);
void *realloc(void *ptr, size_t size);
void *calloc(size_t nmemb, size_t size);
void show_alloc_mem(void);

#endif
