#include "allocator.h"
#include <unistd.h>
#include <limits.h>

void *worst_fit(int size_of_payload) {
    // align payload to 8 bytes
    if (size_of_payload % 8)
        size_of_payload += 8 - (size_of_payload % 8);

    int total_needed = size_of_payload + OVERHEAD;
    block_header *cur = free_list;
    block_header *worst_block = NULL;
    int worst_size = 0;

    // worst-fit search: find the largest block that can fit
    while (cur) {
        if (cur->size >= total_needed && cur->size > worst_size) {
            worst_block = cur;
            worst_size = cur->size;
        }
        cur = cur->next;
    }

    if (worst_block) {
        int orig_size = worst_block->size;
        worst_block->allocated = 1;
        worst_block->size      = total_needed;
        
        block_footer *ftr = (block_footer *)((char *)worst_block + worst_block->size - FOOTER_SIZE);
        ftr->size      = worst_block->size;
        ftr->allocated = worst_block->allocated;

        int leftover = orig_size - total_needed;
        if ((size_t)leftover < OVERHEAD) {
            if (worst_block->prev) worst_block->prev->next = worst_block->next;
            if (worst_block->next) worst_block->next->prev = worst_block->prev;
            if (free_list == worst_block) free_list = worst_block->next;
        } else {
            block_header *next_free = (block_header *)((char *)worst_block + total_needed);
            next_free->size      = leftover;
            next_free->allocated = 0;
            next_free->next      = worst_block->next;
            next_free->prev      = worst_block->prev;
            if (next_free->prev) next_free->prev->next = next_free;
            if (next_free->next) next_free->next->prev = next_free;
            
            // Write footer for new free block
            block_footer *next_ftr = (block_footer *)((char *)next_free + next_free->size - FOOTER_SIZE);
            next_ftr->size      = next_free->size;
            next_ftr->allocated = next_free->allocated;
            
            if (free_list == worst_block) free_list = next_free;
        }

        while (free_list && free_list->prev)
            free_list = free_list->prev;

        return (char *)worst_block + HEADER_SIZE;
    }
    return NULL;
}