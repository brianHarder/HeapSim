#include "allocator.h"
#include <sys/mman.h>
#include <unistd.h>
#include <sys/types.h>

// shared across all heuristics
block_header *free_list = NULL;
block_header *heap_start = NULL;
int heap_size = 0;

static inline void write_footer(block_header *hdr) {
    block_footer *ftr = (block_footer *)((char *)hdr + hdr->size - FOOTER_SIZE);
    ftr->size      = hdr->size;
    ftr->allocated = hdr->allocated;
}

int my_init(int size_of_region) {
    // align to page
    if (size_of_region % 4096)
        size_of_region += 4096 - (size_of_region % 4096);
    heap_size = size_of_region;

    // allocate memory
    free_list = (block_header *)mmap(NULL, size_of_region,
                                     PROT_READ | PROT_WRITE,
                                     MAP_ANON  | MAP_PRIVATE,
                                     -1, 0);
    if (free_list == MAP_FAILED) return -1;

    heap_start = free_list;
    free_list->size      = size_of_region;
    free_list->allocated = 0;
    free_list->next = free_list->prev = NULL;
    write_footer(free_list);
    return 0;
}