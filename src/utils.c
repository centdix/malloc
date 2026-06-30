#include "malloc_internal.h"
#include "libft.h"
#include <stdint.h>

// show_alloc_mem() runs while holding g_malloc_mutex, so its output path must
// stay allocation-free: under LD_PRELOAD any malloc() it triggers re-enters the
// allocator and reports state the dump itself just created. libft's ft_putstr_fd
// is a plain write() loop, so it is reused for the string parts. Numbers and
// pointers are formatted by hand because libft has no allocation-free 64-bit
// printer: ft_putnbr_u_fd/ft_putnbr_base are 32-bit (they would truncate a
// size_t or a pointer) and ft_itoa_base calls malloc.
static void put_uint(size_t n) {
    char buf[20]; // SIZE_MAX is 20 decimal digits on 64-bit
    size_t i = sizeof(buf);
    do {
        buf[--i] = (char)('0' + (n % 10));
        n /= 10;
    } while (n > 0);
    ssize_t w = write(STDOUT_FILENO, buf + i, sizeof(buf) - i);
    (void)w;
}

static void put_ptr(void *p) {
    char buf[16]; // a 64-bit pointer is at most 16 hex digits
    const char *hex = "0123456789abcdef";
    uintptr_t v = (uintptr_t)p;
    size_t i = sizeof(buf);
    ft_putstr_fd("0x", STDOUT_FILENO);
    do {
        buf[--i] = hex[v & 0xf];
        v >>= 4;
    } while (v > 0);
    ssize_t w = write(STDOUT_FILENO, buf + i, sizeof(buf) - i);
    (void)w;
}

void show_alloc_mem() {
    pthread_mutex_lock(&g_malloc_mutex);
    t_heap *cur = HEAD;
    size_t total_allocated = 0;
    ft_putstr_fd("Memory Allocation State:\n", STDOUT_FILENO);

    while (cur) {
        char *heap_type = (cur->total_size == TINY_HEAP_SIZE) ? "TINY" :
                          (cur->total_size == SMALL_HEAP_SIZE) ? "SMALL" : "LARGE";
        ft_putstr_fd(heap_type, STDOUT_FILENO);
        ft_putstr_fd(" : ", STDOUT_FILENO);
        put_ptr((void *)cur);
        ft_putstr_fd("\n", STDOUT_FILENO);

        t_block *block = cur->blocks;
        while (block) {
            if (!block->free) {
                void *start = (void *)((char *)block + sizeof(t_block));
                void *end = (void *)((char *)start + block->size);
                put_ptr(start);
                ft_putstr_fd(" - ", STDOUT_FILENO);
                put_ptr(end);
                ft_putstr_fd(" : ", STDOUT_FILENO);
                put_uint(block->size);
                ft_putstr_fd(" bytes\n", STDOUT_FILENO);
                total_allocated += block->size;
            }
            block = block->next;
        }

        cur = cur->next;
    }

    ft_putstr_fd("Total : ", STDOUT_FILENO);
    put_uint(total_allocated);
    ft_putstr_fd(" bytes\n", STDOUT_FILENO);
    pthread_mutex_unlock(&g_malloc_mutex);
}

t_heap *find_heap_for_ptr(void *ptr) {
    t_heap *heap = HEAD;
    while (heap) {
        void *heap_start = (void *)heap;
        void *heap_end = (void *)((char *)heap + heap->total_size);

        // Quick reject: ptr must fall inside this heap's mapping at all.
        if (ptr > heap_start && ptr < heap_end) {
            // Confirm ptr is the user pointer of an actual block in this heap,
            // not merely some interior address. Accepting interior pointers let
            // calls like free(p + 10) pass validation, after which the block
            // header would be read from the middle of a live allocation and
            // corrupt the heap. Walking the block list and requiring an exact
            // match rejects those before any metadata is touched.
            t_block *block = heap->blocks;
            while (block) {
                if ((void *)((char *)block + sizeof(t_block)) == ptr) {
                    return heap;
                }
                block = block->next;
            }
        }
        heap = heap->next;
    }
    return NULL;
} 