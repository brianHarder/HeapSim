#include "allocator.h"
#include <assert.h>
#include <string.h>
#include <stdint.h>

#define MIN_ORDER 3
block_header *free_bins[32];
int max_order = -1;

static inline void write_footer(block_header *hdr) {
    block_footer *ftr = (block_footer *)((char *)hdr + hdr->size - FOOTER_SIZE);
    ftr->size      = hdr->size;
    ftr->allocated = hdr->allocated;
}

void *buddy_system(int size_of_payload) {
    if (size_of_payload % 8)
        size_of_payload += 8 - (size_of_payload % 8);
    int total = size_of_payload + OVERHEAD;

    // initialize bins
    if (max_order < 0) {
        assert((heap_size & (heap_size - 1)) == 0);
        
        max_order = 0;
        while ((1 << max_order) < heap_size) max_order++;
        
        for (int i = 0; i <= max_order; i++) {
            free_bins[i] = NULL;
        }
        
        block_header *init = (block_header *)heap_start;
        init->size      = heap_size;
        init->allocated = 0;
        init->prev = init->next = NULL;
        write_footer(init);
        free_bins[max_order] = init;
    }

    int order = MIN_ORDER;
    while ((1 << order) < total && order < max_order) order++;
    if ((1 << order) < total) return NULL;

    int o = order;
    while (o <= max_order && free_bins[o] == NULL) o++;
    if (o > max_order) return NULL;

    block_header *block = free_bins[o];
    free_bins[o] = block->next;
    if (free_bins[o]) free_bins[o]->prev = NULL;

    for (int cur = o; cur > order; cur--) {
        int half = (1 << cur) >> 1;

        // left buddy is at block, right at block+half
        block_header *right = (block_header *)((char *)block + half);

        block->size      = half;
        block->allocated = 0;
        block->prev = block->next = NULL;
        write_footer(block);

        right->size      = half;
        right->allocated = 0;
        right->prev = right->next = NULL;
        write_footer(right);

        right->next = free_bins[cur - 1];
        if (free_bins[cur - 1]) free_bins[cur - 1]->prev = right;
        free_bins[cur - 1] = right;
    }

    block->allocated = 1;
    block->size      = (1 << order);
    write_footer(block);

    return (char *)block + HEADER_SIZE;
}
