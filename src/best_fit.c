#include "allocator.h"
#include <unistd.h>
#include <limits.h>

void *best_fit(int size_of_payload) {
    if (size_of_payload % 8)
        size_of_payload += 8 - (size_of_payload % 8);

    int total_needed = size_of_payload + OVERHEAD;
    block_header *cur = free_list;
    block_header *best_block = NULL;
    int best_size = INT_MAX;

    // best-fit search: find the smallest block that can fit
    while (cur) {
        if (cur->size >= total_needed && cur->size < best_size) {
            best_block = cur;
            best_size = cur->size;
        }
        cur = cur->next;
    }

    if (best_block) {
        int orig_size = best_block->size;
        best_block->allocated = 1;
        best_block->size      = total_needed;
        
        block_footer *ftr = (block_footer *)((char *)best_block + best_block->size - FOOTER_SIZE);
        ftr->size      = best_block->size;
        ftr->allocated = best_block->allocated;

        int leftover = orig_size - total_needed;
        if ((size_t)leftover < OVERHEAD) {
            if (best_block->prev) best_block->prev->next = best_block->next;
            if (best_block->next) best_block->next->prev = best_block->prev;
            if (free_list == best_block) free_list = best_block->next;
        } else {
            block_header *next_free = (block_header *)((char *)best_block + total_needed);
            next_free->size      = leftover;
            next_free->allocated = 0;
            next_free->next      = best_block->next;
            next_free->prev      = best_block->prev;
            if (next_free->prev) next_free->prev->next = next_free;
            if (next_free->next) next_free->next->prev = next_free;
            
            block_footer *next_ftr = (block_footer *)((char *)next_free + next_free->size - FOOTER_SIZE);
            next_ftr->size      = next_free->size;
            next_ftr->allocated = next_free->allocated;
            
            if (free_list == best_block) free_list = next_free;
        }

        while (free_list && free_list->prev)
            free_list = free_list->prev;

        return (char *)best_block + HEADER_SIZE;
    }
    return NULL;
}