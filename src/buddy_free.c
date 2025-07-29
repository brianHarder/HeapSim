#include "allocator.h"
#include <stddef.h>
#include <assert.h>
#include <stdint.h>

#define MIN_ORDER 3
extern block_header *free_bins[];
extern int max_order;

static inline void write_footer(block_header *hdr) {
    block_footer *ftr = (block_footer *)((char *)hdr + hdr->size - FOOTER_SIZE);
    ftr->size      = hdr->size;
    ftr->allocated = hdr->allocated;
}

// remove b from its bin[order] list
static inline void remove_bin(block_header *b, int order) {
    if (b->prev) b->prev->next = b->next;
    if (b->next) b->next->prev = b->prev;
    if (free_bins[order] == b) free_bins[order] = b->next;
}

void buddy_free(void *ptr) {
    if (!ptr) return;

    block_header *hdr = (block_header *)((char *)ptr - HEADER_SIZE);
    hdr->allocated = 0;
    write_footer(hdr);

    // compute initial order = log2(block size)
    int order = 0;
    while ((1 << order) < hdr->size) order++;
    assert((1 << order) == hdr->size);

    char *base = (char *)heap_start;
    size_t offset = (char *)hdr - base;

    while (order < max_order) {
        size_t buddy_ofs = offset ^ (1 << order);
        block_header *buddy = (block_header *)(base + buddy_ofs);

        if (buddy->allocated || buddy->size != (1 << order))
            break;

        remove_bin(buddy, order);

        if (buddy < hdr) {
            hdr = buddy;
            offset = buddy_ofs;
        }

        order++;
        hdr->size = (1 << order);
        write_footer(hdr);
    }

    hdr->prev = NULL;
    hdr->next = free_bins[order];
    if (free_bins[order]) free_bins[order]->prev = hdr;
    free_bins[order] = hdr;
}
