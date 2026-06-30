#include "malloc_internal.h"
#include "libft.h"

// printf() is not an authorized function for this project, so all output goes
// through write(2) via these small helpers (libft's ft_put* also wrap write).

// Print a pointer as 0x-prefixed lowercase hexadecimal.
static void put_addr(void *ptr) {
    const char *hex = "0123456789abcdef";
    unsigned long n = (unsigned long)ptr;
    char buf[16];
    int i = 0;

    ft_putstr_fd("0x", 1);
    if (n == 0) {
        ft_putchar_fd('0', 1);
        return;
    }
    while (n > 0) {
        buf[i++] = hex[n % 16];
        n /= 16;
    }
    while (i > 0) {
        ft_putchar_fd(buf[--i], 1);
    }
}

// Print a size_t in decimal (ft_putnbr_fd only handles int, which overflows).
static void put_size(size_t n) {
    char buf[24];
    int i = 0;

    if (n == 0) {
        ft_putchar_fd('0', 1);
        return;
    }
    while (n > 0) {
        buf[i++] = '0' + (n % 10);
        n /= 10;
    }
    while (i > 0) {
        ft_putchar_fd(buf[--i], 1);
    }
}

// Print every used block of one zone, accumulating their sizes into *total.
static void show_heap(t_heap *heap, size_t *total) {
    t_block *block = heap->blocks;

    while (block) {
        if (!block->free) {
            void *start = (void *)((char *)block + sizeof(t_block));
            void *end = (void *)((char *)start + block->size);
            put_addr(start);
            ft_putstr_fd(" - ", 1);
            put_addr(end);
            ft_putstr_fd(" : ", 1);
            put_size(block->size);
            ft_putstr_fd(" bytes\n", 1);
            *total += block->size;
        }
        block = block->next;
    }
}

// Print the zones of one size class (matched on total_size), labelled with
// the given type. Returns nothing; bytes are added to *total.
static void show_class(size_t class_size, const char *label, size_t *total) {
    t_heap *cur = HEAD;

    while (cur) {
        bool is_large = (cur->total_size != TINY_HEAP_SIZE &&
                         cur->total_size != SMALL_HEAP_SIZE);
        bool match = class_size ? (cur->total_size == class_size) : is_large;
        if (match) {
            ft_putstr_fd((char *)label, 1);
            ft_putstr_fd(" : ", 1);
            put_addr((void *)cur);
            ft_putchar_fd('\n', 1);
            show_heap(cur, total);
        }
        cur = cur->next;
    }
}

void show_alloc_mem(void) {
    size_t total = 0;

    pthread_mutex_lock(&g_malloc_mutex);
    // Subject output order: TINY, then SMALL, then LARGE zones.
    show_class(TINY_HEAP_SIZE, "TINY", &total);
    show_class(SMALL_HEAP_SIZE, "SMALL", &total);
    show_class(0, "LARGE", &total);
    ft_putstr_fd("Total : ", 1);
    put_size(total);
    ft_putstr_fd(" bytes\n", 1);
    pthread_mutex_unlock(&g_malloc_mutex);
}

t_heap *find_heap_for_ptr(void *ptr) {
    t_heap *heap = HEAD;
    while (heap) {
        void *heap_start = (void *)heap;
        void *heap_end = (void *)((char *)heap + heap->total_size);

        // Check if pointer is within heap bounds
        if (ptr > heap_start && ptr < heap_end) {
            // Additional validation: check if ptr points to a valid block
            // by ensuring it's properly aligned and within the blocks region
            void *blocks_start = (void *)heap->blocks;

            if (ptr >= blocks_start && ptr < heap_end) {
                // Verify the pointer could be a valid allocation
                // (points to data section, not block header)
                t_block *potential_block = (t_block *)((char *)ptr - sizeof(t_block));

                // Check if this block header is within valid range
                if ((void *)potential_block >= blocks_start &&
                    (void *)potential_block < heap_end) {
                    return heap;
                }
            }
        }
        heap = heap->next;
    }
    return NULL;
} 