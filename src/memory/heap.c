#include "heap.h"
#include <stdint.h>

static uint8_t heap[1024*1024] __attribute__((aligned(8)));
static size_t used;

void heap_init(void) { used = 0; }

void *kmalloc(size_t n) {
    if (!n || n > SIZE_MAX - 7) return 0;
    size_t aligned = (n + 7) & ~(size_t)7;
    if (aligned > sizeof(heap) - used) return 0;
    void *p = &heap[used];
    used += aligned;
    return p;
}

void kfree(void *ptr) { (void)ptr; /* bump allocator: reclaiming is future work */ }
